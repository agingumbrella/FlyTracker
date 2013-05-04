#!/usr/bin/python
import zmq
import time
import math
import sys
import motmot.fview.traited_plugin as traited_plugin
import traits.api as traits
import numpy as np
from traitsui.api import View, Item, Group, FileEditor
#from multiprocessing import Process, Queue, Manager
from threading import Thread
#from Queue import Queue

if 1:
  # https://mail.enthought.com/pipermail/enthought-dev/2008-May/014709.html
  import logging
  logger = logging.getLogger()
  logger.addHandler(logging.StreamHandler(sys.stderr))
  logger.setLevel(logging.DEBUG)

REQUEST_TIMEOUT = 2500
REQUEST_RETIRES = 3
SERVER_ENDPOINT = "tcp://localhost:5556"

# run control of the ODD and communcation with the DIO board
# in a separate process, so that it can delay, etc.
# without messing up the video acquisition
# Also, have it deal with the logging.
class ODDControlWorker(Thread):
  def __init__(self, odor, stim, delay, client, poll):
    Thread.__init__(self)
    self.odor = odor
    self.stim = stim
    self.delay = delay
    self.client = client
    self.poll = poll
    return

 # pass tuples (task_name, task_args) where task_args is a dict
  def run(self):
    self.send_odor_command(self.odor, self.stim, self.delay)

  def send_odor_command(self, odor, stim, delay):
    blank = int(8*int(math.floor(odor/8)))
    request = "%d %d %d %d " % (odor, stim, delay, blank)
    self.client.send(request)
    time.sleep(stim + delay)
    expect_reply = True
    while expect_reply:
      socks = dict(self.poll.poll(REQUEST_TIMEOUT))
      if socks.get(self.client) == zmq.POLLIN:
        reply = self.client.recv()
        if not reply:
          break
        else:
          expect_reply = False


#
# File has format:
# OdorName1 Port1 Delay1 OnTime1 
# OdorName2 Port2 Delay2 OnTime2 
# OdorName3 Port3 Delay3 OnTime3 
# ...
#
def read_odd_file(fname):
  f = open(fname, 'r')
  odors = []
  for line in f:
    elems = line.rstrip().split()
    name = elems[0]
    odor = int(elems[1])
    delay = int(elems[2])
    stim = int(elems[3])
    odors.append((name, odor, delay, stim))
  odors.reverse() # so pops come off in the right order
  return odors

# design: 
# once video acquisition is started, send information about odor delays to Arduino
class OdorControl(traited_plugin.HasTraits_FViewPlugin):
  enabled = traits.Bool(False)
  plugin_name = "Odor Control"
  last_update_time = traits.Float(-np.inf, transient=True)
  save_to_disk = traits.Bool(False,transient=True)
  log_filename_prefix = traits.String('odd_log_data_')
  log_filename = traits.File
  odd_filename = traits.File
  persistent_attr_names = traits.List(['odd_filename', 'log_filename'])
  deliver_odors = traits.Bool(False, transient=True)
  traits_view = View(Group(\
    Item(name="deliver_odors"),\
    Item(name='save_to_disk'),\
    Item(name="odd_filename", editor=FileEditor(dialog_style="open")),\
    Item(name="log_filename_prefix"),\
    Item(name="log_filename", style="readonly")))

  def __init__(self, *args, **kwargs):
    super(OdorControl,self).__init__(*args, **kwargs)
    print "Initialized odor control"
    self._list_of_timestamp_data = []
    self.odors = []
    self.last_trigger_timestamp = {}
    self.sending_odor = False
    self.total_delay = 0
    self.has_started = False
    self.log_file = None
    self.worker = None
    self.context = zmq.Context(1)
    self.client = self.context.socket(zmq.REQ)
    self.client.connect(SERVER_ENDPOINT)
    self.poll = zmq.Poller()
    self.poll.register(self.client, zmq.POLLIN)
    print "Socket initialized on port 5556..."

    self.has_started = True

    # variables for current odor
    self.odor_name = ""
    self.odor = 0
    self.stim = 0
    self.delay = 0

  def _save_to_disk_changed(self):
    print "Changing save to disk"
    if self.has_started:
      if self.save_to_disk:
        self.log_filename = self.log_filename_prefix + time.strftime('%Y%m%d_%H%M%S') + ".txt"
        self.log_file = open(self.log_filename, 'w')
        print "Saving ", repr(self.log_filename), " to disk..."
      else:
        print "Closed", repr(self.log_filename)
        self.log_file.close()
        self.log_file = None
        self.log_filename = ""

  def _odd_filename_changed(self):
    print "Changing ODD filename"
    try:
      print "Using ODD file ", self.odd_filename
      self.odors = read_odd_file(self.odd_filename.encode('ascii'))
    except IOError:
      print "Couldn't open ODD file ", self.odd_filename

  def quit(self):
    pass
    if self.save_to_disk:
      self.log_file.close()
    if self.has_started and self.worker is not None:
      self.worker.join()

  def camera_starting_notification(self, cam_id, pixel_format=None, max_width=None, max_height=None):
    self.has_started = True
    pass

  def process_frame(self, cam_id, buf, buf_offset, timestamp, framenumber):
    draw_points = []
    draw_linesegs = []
    now = time.time()
    if self.deliver_odors:
      if not self.sending_odor:
        self.odor_name, self.odor, self.stim, self.delay = self.odors.pop()
        if self.save_to_disk:
          loginfo = "S %s %d %d %d %d %d %d\n" % (self.odor_name, self.odor, self.stim, self.delay, framenumber, time.mktime(time.gmtime()), timestamp)
          self.log_file.write(loginfo)
        self.total_delay = self.stim + self.delay
        print "Sending odor ", self.odor_name, " with total delay ", self.total_delay
        self.worker = ODDControlWorker(self.odor, self.stim, self.delay, self.client, self.poll)
        self.worker.daemon = True
        self.worker.start()
        self.sending_odor = True
        self.last_update_time = time.time()
      else:
        if (now - self.last_update_time) > self.total_delay:
          print "Changing odor from ", self.odor_name
          self.worker.join()
          self.sending_odor = False
          if self.save_to_disk:
            loginfo = "R %s %d %d %d %d %d %d\n" % (self.odor_name, self.odor, self.stim, self.delay, framenumber, time.mktime(time.gmtime()), timestamp)
            self.log_file.write(loginfo)
          if len(self.odors) == 0:
            self.deliver_odors = False
            # reload odors
            self.odors = read_odd_file(self.odd_filename.encode('ascii'))
    return draw_points, draw_linesegs

