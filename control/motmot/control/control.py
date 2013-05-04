#!/usr/bin/python
import zmq
import time
import math
import sys
import motmot.fview.traited_plugin as traited_plugin
import traits.api as traits
import numpy as np
from traitsui.api import View, Item, Group, FileEditor
from multiprocessing import Process, Queue, Manager
#from threading import Thread
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
class ODDControlWorker(Process):
  def __init__(self, task_q):
    Process.__init__(self)
    self.task_q = task_q
    self.save_to_disk = False
    self.log_file_name = ""
    self.log_file = None
    return

 # pass tuples (task_name, task_args) where task_args is a dict
  def run(self):
    self.context = zmq.Context(1)

    self.client = self.context.socket(zmq.REQ)
    self.client.connect(SERVER_ENDPOINT)

    self.poll = zmq.Poller()
    self.poll.register(self.client, zmq.POLLIN)

#    time.sleep(1)

    while True:
      time.sleep(1)
      print "Hello"
#      next_task_name, next_task_args = self.task_q.get()
#      if next_task_name == "quit":
#        self.context.term()
#        break
#      else:
#        task = getattr(self, next_task_name)
#        result = task(**next_task_args)

  def open_log_file(self, log_fname):
    self.save_to_disk = True
    self.log_file = open(log_fname, 'w')
    self.log_file_name = log_fname

  def close_log_file(self):
    self.save_to_disk = False
    self.log_file_name = ''
    self.log_file.close()
    self.log_file = None

  def send_odor_command(self, odor_name, odor, stim, delay, timestamp, framenumber):
    blank = int(8*int(math.floor(odor/8)))
    request = "%d %d %d %d " % (odor, stim, delay, blank)
    self.client.send(request)
    if self.save_to_disk:
      loginfo = "S %s %d %d %d %d %d %d %d\n" % (odor_name, odor, stim, delay, blank, timestamp, framenumber, time.mktime(time.gmtime()))
      self.log_file.write(loginfo)
    time.sleep(stim + delay)
    expect_reply = True
    while expect_reply:
      socks = dict(self.poll.poll(REQUEST_TIMEOUT))
      if socks.get(self.client) == zmq.POLLIN:
        reply = self.client.recv()
        if not reply:
          break
        else:
          if self.save_to_disk:
            loginfo = "R %s %d %d %d %d %d %d %d\n" % (odor_name, odor, stim, delay, blank, timestamp, framenumber, time.mktime(time.gmtime()))
            self.log_file.write(loginfo)
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

  traits_view = View(Group(\
    Item(name='save_to_disk'),\
    Item(name="odd_filename", editor=FileEditor(dialog_style="open")),\
    Item(name="log_filename_prefix"),\
    Item(name="log_filename", style="readonly")))

  def __init__(self, *args, **kwargs):
    super(OdorControl,self).__init__(*args, **kwargs)
    self._list_of_timestamp_data = []
    self.odors = []
    self.last_trigger_timestamp = {}
    self.sending_odor = False
    self.delay = 0
    self.has_started = False
    print "Socket initialized on port 5556..."
    self.has_started = True
    self.manager = Manager()
    self.q = self.manager.Queue()
    self.worker = ODDControlWorker(self.q)
    print "Worker process created..."
    self.worker.daemon = True
    self.worker.start()
    print "Worker process initialized..."

  def _save_to_disk_changed(self):
    print "Changing save to disk"
    if self.has_started:
      if self.save_to_disk:
        self.log_filename = self.log_filename_prefix + time.strftime('%Y%m%d_%H%M%S') + ".txt"
        self.q.put(("open_log_file", {"log_fname":self.log_filename}))
        print "Saving ", repr(self.log_filename), " to disk..."
      else:
        print "Closed", repr(self.log_filename)
        self.q.put(("close_log_file", {}))
        self.log_filename = ""

  def _odd_filename_changed(self):
    print "Changing ODD filename"
    try:
      print "Using ODD file ", repr(self.odd_filename)
      self.odors = read_odd_file(repr(self.odd_filename))
    except IOError:
      print "Couldn't open ODD file ", repr(self.odd_filename)

  def quit(self):
    if self.has_started:
      self.q.put(("quit", {}))
      self.worker.join()

  def camera_starting_notification(self, cam_id, pixel_format=None, max_width=None, max_height=None):
    pass

  def process_frame(self, cam_id, buf, buf_offset, timestamp, framenumber):
    if framenumber == 0:
      self.odors = read_odd_file("/home/jlab/odors.txt")
      print self.odors
    draw_points = []
    draw_linesegs = []

    now = time.time()
    if len(self.odors) > 0:
      if not self.sending_odor:
        print "Sending odor ", o[0]
        o = self.odors.pop()
        self.delay = o[1] + o[2]
        q.put(("send_odor_command", {"odor_name":o[0], "odor":o[1], "stim":o[2], "delay":o[3], "timestamp":timestamp, "framenumber":framenumber}))
        self.sending_odor = True
        self.last_update_time = time.time()
      else:
        if (self.last_update_time - now) > self.delay:
          self.sending_odor = False
    self.last_trigger_timestamp[cam_id] = timestamp
    return draw_points, draw_linesegs

