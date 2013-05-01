#!/usr/bin/python

from optparse import OptionParser
from matplotlib import pyplot as plt
from motmot.FlyMovieFormat.FlyMovieFormat import *
import numpy as np
import cv2
import cv
from tracker import *

def drawPositions(frame, states):
	for state in states:
		cv2.ellipse(frame, (state.center, state.size, state.angle), (0, 255, 0), 2)
		cv2.circle(frame, state.center, 5, (255,0,0), 2)
		cv2.putText(frame, str(state.ID), state.center, cv2.FONT_HERSHEY_PLAIN, 3, (255,255,255), 4)
	return frame

def drawTracks(frame, stateSeq):
	for i in range(1,len(stateSeq)):
		curr = stateSeq[i]
		prev = stateSeq[i-1]
		for key, val in curr.iteritems():
			if key in prev:
				cv2.line(frame, prev[key].center, curr[key].center, (0, 0, 255), 2)
	return frame

def displayScaled(winName, img, scaleFactor):
	currsize = img.shape
	img = cv2.resize(img, (currsize[1]/scaleFactor, currsize[0]/scaleFactor))
	cv2.imshow(winName, img)

def main():
	# parse options
	# TODO implement adding commandline flags
	if len(sys.argv) == 3:
		movieFilename = sys.argv[1]
		dataFilename = sys.argv[2]
	#movieFilename = "/home/jlab/Documents/WillData/or83bGal420130422_172747.fmf"
	#dataFilename = "/home/jlab/history.txt"

	# init display
	cv2.startWindowThread()
	WIN_RF = "Display"
	cv2.namedWindow(WIN_RF)

	# load video
	fmf = FlyMovie(movieFilename)

	print "Loading movie info..."
	nframes = fmf.compute_n_frames_from_file_size()
	print "Num frames: %d" % nframes
	print "Width: %d, Height: %d" % (fmf.get_width(), fmf.get_height())

	# load tracker data
	tracker = Tracker()
	history = tracker.loadData(dataFilename)

	# tracking algorithm	
	tracked_pos = tracker.track()



	scaleFactor = 3
	for i in range(1,nframes):
		currstates = [x for x in tracked_pos[i-1].itervalues()]
#		currstates = tracker.history[i-1]
		currframe, timestamp = fmf.get_frame(i)
		currframe = cv2.cvtColor(currframe, cv2.COLOR_GRAY2RGB)
		currframe = drawTracks(currframe, tracked_pos[1:i])
		currframe = drawPositions(currframe, currstates)
		displayScaled(WIN_RF, currframe, scaleFactor)
		key = cv2.waitKey(0)

if __name__ == "__main__":
	main()
