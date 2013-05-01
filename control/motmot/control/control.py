#!/usr/bin/python
import zmq
import time
import math
import motmot.fview.traited_plugin as traited_plugin
import traits.api as traits
import numpy as np
from traitsui.api import View, Item, Group, FileEditor

# design: 
# once video acquisition is started, send information about odor delays to Arduino
class OdorControl(traited_plugin.HasTraits_FViewPlugin):
  enabled = traits.Bool(False)
  plugin_name = "Odor Control"
  last_update_time = traits.Float(-np.inf, transient=True)
  odd_filename = traits.File(exists=True)
  log_filename = traits.File(exists=True)
  traits_view = View(Group(\
    Item("odd_filename", editor=FileEditor(dialog_style="open"), label="Select ODD File"),\
    Item("log_filename", editor=FileEditor(dialog_style="save"), label="Select log file name")))

  def __init__(self, *args, **kwargs):
    super(OdorControl,self).__init__(*args, **kwargs)

  def camera_starting_notification(self, cam_id, pixel_format=None, max_width=None, max_height=None):
    self.context = zmq.Context()
    self.socket = context.socket(zmq.PUB)
    self.socket.bind("tcp://*:5556")
    time.sleep(1)

  def process_frame(self, cam_id, buf, buf_offset, timestamp, framenumber):
    draw_points = []
    draw_linesegs = []

    if self.frame.IsShown():
      now = time.time()
      self.last_update_time = now
      if framenumber % 100 == 0:
        self.sendOdorCommand(3, 2, 2)

    return draw_points, draw_linesegs

  def sendOdorCommand(self, odor, stim, delay):
    blank = int(8*int(math.floor(odor/8)))
    self.socket.send("%d %d %d %d " % (odor, stim, delay, blank))
    print "Send odor request %d %d %d %d at %f" % (odor, stim, delay, blank, time.mktime(time.gmtime()))

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

def readODDFile(fname):
  f = open(fname, 'r')
  n = 0
  odornames = []
  delay = {}
  for line in f:
    elems = line.rstrip().split()
    odornames.append(elems[0])
    delay[elems[0]] = int(elems[1])
