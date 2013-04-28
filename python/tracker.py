#!/usr/bin/python

from optparse import OptionParser
from matplotlib import pyplot as plt
from motmot.FlyMovieFormat.FlyMovieFormat import *
import numpy as np
import hungarian
import cv2
import cv

def displayScaled(winName, img, scaleFactor):
	currsize = img.shape
	img = cv2.resize(img, (currsize[1]/scaleFactor, currsize[0]/scaleFactor))
	cv2.imshow(winName, img)

def loadTrackerData(fname):
	f = open(fname, "r")
	history = []
	for line in f:
		elems = line.rstrip().split(" ")
		frameNum = int(elems[0])
		numObs = int(elems[1])
		currStates = []
		for stateStr in elems[2:]:
			currStates.append(parseEllipse(stateStr))
		history.append(currStates)
	return history
	
class Ellipse:
	def __init__(self, ID=None, angle=None, center=None, size=None, isDummy=False):
		self.ID = ID
		self.angle = angle
		self.center = center
		self.size = size
		self.isDummy = isDummy

	def __repr__(self):
		return "(%f,%f)" % (self.center[0], self.center[1])

def parseEllipse(stateString):
	elems = stateString.split(":")
	angle = float(elems[0])
	center = (int(round(float(elems[1]))), int(round(float(elems[2]))))
	size = (float(elems[3]), float(elems[4]))
	return Ellipse(-1, angle, center, size)

# flies1 and flies2 are 2 dictionaries of ellipses
# returns dictionary of ellipses
def predictPosition(flies1, flies2):
	positions = {}
	for ee in flies2.iterkeys():
		if ee in flies1:
			dx = flies2[ee].center[0] - flies1[ee].center[0]
			dy = flies2[ee].center[1] - flies1[ee].center[1]
			centerd = np.sqrt((dx**2. + dy**2.0))
			newx = flies2[ee].center[0] + dx
			newy = flies2[ee].center[1] + dy
			positions[ee] = Ellipse(flies2[ee].angle, (newx, newy), (flies2[ee].size[0], flies2[ee].size[1])) 
	return positions

# Modified from implementation in Ctrax
def matchFlies(cost, maxcost):
	# number of targets from previous frame
	ntargets = cost.shape[1]
	# number of observations in current frame
	nobs = cost.shape[0]
	# try greed assignment
	obsfortarget = cost.argmin(axis=0)
	# make sure not assigning obs when should be assigning to dummy	
	mincost = cost.min(axis=0)
	obsfortarget[mincost > maxcost] = -1
	isconflict = 0
	isunassigned = np.empty((nobs, 1))
	isunassigned[:] = True
	for i in range(ntargets):
		if obsfortarget[i] < 0:
			continue
		if isunassigned[obsfortarget[i]] == False:
			isconflict = True
		isunassigned[obsfortarget[i]] = False
	if not isconflict:
		return (obsfortarget, isunassigned)
	
	# otherwise, use hungarian matching algorithm
	n = ntargets + nobs
	weights = np.zeros((n,n))
	weights[0:nobs, 0:ntargets] = cost
	weights[0:nobs, ntargets:n] = maxcost
	weights[nobs:n, 0:ntargets] = maxcost
	(targetforobs, obsfortarget) = hungarian.hungarian(weights)
	
	# remove dummy targets
	obsfortarget = obsfortarget[0:ntargets]
	# unassign obs assigned to dummy targets
	isunassigned = targetforobs[0:nobs] >= ntargets
	obsfortarget[obsfortarget >= nobs] = -1
	return (obsfortarget, isunassigned)

def dist2(obs, pred):
	return ((pred.center[0] - obs.center[0])**2 + (pred.center[1] - obs.center[1])**2)

# Modified from implementation in Ctrax	
# old1, old2, obs are dictionaries of ellipses
def findFlies(old0, old1, obs):
	maxCost = 1000

	# predict new positions	
	targets = predictPosition(old0, old1)
	print "targ (%d) = %s"%(len(targets), str(targets))
	print "obs (%d) = %s"%(len(obs), str(obs))

	ids = []
	for i in targets.iterkeys():
		ids.append(i)
	vals = []
	for i in targets.itervalues():
		vals.append(i)
	
	# make cost matrix
	cost = np.empty((len(obs), len(targets)))
	for i, o in enumerate(obs):
		for j, t in enumerate(vals):
			if t.isDummy:
				cost[i,j] = maxcost
			else:
				cost[i,j] = dist2(o, t)
	obs_for_target, unass_obs = matchFlies(cost, maxCost)
	print "Best matches:", obs_for_target

	# make a  dictionary with best matches for each prediction
	flies = {}
	for tt in range(len(targets)):
		if obs_for_target[tt] >= 0:
			obs[obs_for_target[tt]].ID = ids[tt]
			flies[ids[tt]] = obs[obs_for_target[tt]]
	for oo in range(len(obs)):
		if unass_obs[oo]:
			obs[oo].ID = getNewID()
	return flies
	
def drawFrame(frame, states):
	colored = cv2.cvtColor(frame, cv2.COLOR_GRAY2RGB)
	for state in states:
		cv2.ellipse(colored, (state.center, state.size, state.angle), (0, 255, 0), 2)
		cv2.circle(colored, state.center, 5, (255,0,0), 2)
		cv2.putText(colored, str(state.ID), state.center, cv2.FONT_HERSHEY_PLAIN, 2, (255,255,255))
	return colored

numID = 0
def getNewID():
	global numID 
	numID += 1
	return numID

def tracking(history):
	output = []
#	for i in range(len(history)):
	for i in range(5):
		print "Frame %d" % i
		currStates = history[i]
		# initialize flies with ID
		if i == 0:
			flies = {}
			for ell in currStates:
				ell.ID = getNewID()
				flies[ell.ID] = ell	
		elif i == 1:
			flies = findFlies(output[i-1], output[i-1], currStates)
		else:
			flies = findFlies(output[i-2], output[i-1], currStates)			
		output.append(flies)
	return output

def main():
	# parse options
	# TODO implement adding commandline flags
	movieFilename = "/home/jlab/Documents/WillData/or83bGal420130422_172747.fmf"	
	dataFilename = "/home/jlab/history.txt"
	
	# load tracker data
	history = loadTrackerData(dataFilename)

	# tracking algorithm
		
	# load video
	cv2.startWindowThread()
	WIN_RF = "Display"
	cv2.namedWindow(WIN_RF)

	tracked_pos = tracking(history)

	fmf = FlyMovie(movieFilename)
	nframes = fmf.compute_n_frames_from_file_size()

	scaleFactor = 3
#	for i in range(1,nframes):
#		currstates = [x for x in tracked_pos[i-1].itervalues()]
#		currframe, timestamp = fmf.get_frame(i)
#		currframe = drawFrame(currframe, currstates)
#		displayScaled(WIN_RF, currframe, scaleFactor)
#		key = cv2.waitKey(100)

			
if __name__ == "__main__":
	main()
