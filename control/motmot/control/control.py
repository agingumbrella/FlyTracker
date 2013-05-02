#!/usr/bin/python
import zmq
import time
import math
import sys
import motmot.fview.traited_plugin as traited_plugin
import traits.api as traits
import numpy as np
from traitsui.api import View, Item, Group, FileEditor
from multiprocessing import Process

if 1:
  # https://mail.enthought.com/pipermail/enthought-dev/2008-May/014709.html
  import logging
  logger = logging.getLogger()
  logger.addHandler(logging.StreamHandler(sys.stderr))
  logger.setLevel(logging.DEBUG)

# run control of the ODD and communcation with the DIO board
# in a separate process, so that it can delay, etc.
# without messing up the video acquisition
# Also, have it deal with the logging.
class ODDControlWorker(Process):
  def __init__(self, task_q):
    Process.__init__(self)
    self.task_q = task_q
    return
  
# pass tuples (task_name, task_args) where task_args is a dict
  def run(self):
    proc_name = self.name
    while True:
      next_task_name, next_task_args = self.task_q.get()
      task = getattr(self, next_task_name)
      result = task(**next_task_args)
    else:
      break

  def load
  def open_log_file(self, log_fname):
    pass

  def close_log_file(self):
    pass

  def update(self, timestamp, framenum):
    pass
 
  def send_odor_command(self, odor, stim, delay, timestamp, framenumber):
    blank = int(8*int(math.floor(odor/8)))
    self.socket.send("%d %d %d %d " % (odor, stim, delay, blank))
    if self.save_to_disk:
      loginfo = "Sent odor %d %d %d %d at T=%f F=%d\n" % (odor, stim, delay, blank, timestamp, framenumber)
      self.log_file.write(loginfo)


#
# File has format:
# PreAllOdorDelay
# PostAllOdorDelay
# InterOdorDelay
# OdorName1 OnTime1 
# OdorName2 OnTime2 
# OdorName3 OnTime3 
# ...
#
# where PreAllOdorDelay is how long 

  def read_odd_file(self, fname):
    f = open(fname, 'r')
    n = 0
    odornames = []
    delay = {}
    for line in f:
      elems = line.rstrip().split()
      odornames.append(elems[0])
      delay[elems[0]] = int(elems[1])
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
  Nth_frame = traits.Int(100)

  traits_view = View(Group(\
    Item(name='save_to_disk'),\
    Item(name="odd_filename", editor=FileEditor(dialog_style="open")),\
    Item(name="log_filename_prefix"),\
    Item(name="log_filename", style="readonly"),\
    Item(name="Nth_frame")))

  def __init__(self, *args, **kwargs):
    super(OdorControl,self).__init__(*args, **kwargs)
    self._list_of_timestamp_data = []
    self.last_trigger_timestamp = {}
    self.log_file = None
  def _save_to_disk_changed(self):
    if self.save_to_disk:
      self.log_filename = self.log_filename_prefix + time.strftime('%Y%m%d_%H%M%S') + ".txt"
      self.log_file = open(self.log_filename, "w")
      print "Saving to disk..."
    else:
      print "Closing file..."
      self.log_file.close()
      self.log_file = None
      print "Closed", repr(self.log_filename)
      self.log_filename = ""

  def _odd_filename_changed(self):
    print "Using ODD file ", repr(self.odd_filename)
    #readODDFile(self.odd_filename)

  def camera_starting_notification(self, cam_id, pixel_format=None, max_width=None, max_height=None):
    self.context = zmq.Context()
    self.socket = self.context.socket(zmq.PUB)
    self.socket.bind("tcp://*:5556")
    time.sleep(1)
    print "Socket initialized on port 5556..."

  def process_frame(self, cam_id, buf, buf_offset, timestamp, framenumber):
    draw_points = []
    draw_linesegs = []

    now = time.time()
    self.last_update_time = now
    if framenumber % self.Nth_frame == 0:
      pass
#      self.sendOdorCommand(3, 2, 2)

    self.last_trigger_timestamp[cam_id] = timestamp
    return draw_points, draw_linesegs


