#!/usr/bin/python
import serial
import motmot.fview.traited_plugin as traited_plugin

# design: 
# once video acquisition is started, send information about odor delays to Arduino
class OdorControl(traited_plugin.HasTraits_FViewPlugin):
	plugin_name = "odor control"
	nframe

	def __init__(self):
		self.ser = serial.Serial("/dev/ttyACM0")
		ser.write('1')

	def camera_starting_notification(self, cam_id, pixel_format=None, max_width=None, max_height=None):
		return

	def process_frame(self, cam_id, buf, buf_offset, timestamp, framenumber):
		draw_points = []
		draw_linesegs = []
		return draw_points, draw_linesegs
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

		

if __name__ == "__main__":
	main()
