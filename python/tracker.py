from matplotlib import pyplot as plt
import numpy as np
from hungarian import hungarian


def dist2(obs, pred):
	return ((pred.center[0] - obs.center[0])**2 + (pred.center[1] - obs.center[1])**2)

	
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

class Tracker:
	def __init__(self):
		self.numID = 0
		self.history = []

	def loadData(self, fname):
		f = open(fname, "r")
		self.history = []
		for line in f:
			elems = line.rstrip().split(" ")
			frameNum = int(elems[0])
			numObs = int(elems[1])
			currStates = []
			for stateStr in elems[2:]:
				currStates.append(parseEllipse(stateStr))
			self.history.append(currStates)

	def predictPosition(self, flies1, flies2):
		positions = {}
		for ee in flies2.iterkeys():
			if ee in flies1:
				dx = flies2[ee].center[0] - flies1[ee].center[0]
				dy = flies2[ee].center[1] - flies1[ee].center[1]
				centerd = np.sqrt((dx**2. + dy**2.0))
				newx = flies2[ee].center[0] + dx
				newy = flies2[ee].center[1] + dy
				positions[ee] = Ellipse(-1, flies2[ee].angle, (newx, newy), (flies2[ee].size[0], flies2[ee].size[1])) 
		return positions

# Modified from implementation in Ctrax
	def matchFlies(self, cost, maxcost):
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
			print "Using brute force"
			return (obsfortarget, isunassigned)
	
		# otherwise, use hungarian matching algorithm
		n = ntargets + nobs
		weights = np.zeros((n,n))
		weights[0:nobs, 0:ntargets] = cost
		weights[0:nobs, ntargets:n] = maxcost
		weights[nobs:n, 0:ntargets] = maxcost
		(targetforobs, obsfortarget) = hungarian(weights)
	
		# remove dummy targets
		obsfortarget = obsfortarget[0:ntargets]
		# unassign obs assigned to dummy targets
		isunassigned = targetforobs[0:nobs] >= ntargets
		obsfortarget[obsfortarget >= nobs] = -1
		print "Using hungarian"
		return (obsfortarget, isunassigned)

	# Modified from implementation in Ctrax	
	# old1, old2, obs are dictionaries of ellipses
	def findFlies(self, old0, old1, obs):
		maxCost = 70000

		# predict new positions	
		targets = self.predictPosition(old0, old1)
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

		#print "cost = " + str(cost)

		obs_for_target, unass_obs = self.matchFlies(cost, maxCost)
		print "Best matches:", obs_for_target
		print "Num unassigned obs:", np.sum(unass_obs)
	
		# make a  dictionary with best matches for each prediction
		flies = {}
		for tt in range(len(targets)):
			if obs_for_target[tt] >= 0:
				obs[obs_for_target[tt]].ID = ids[tt]
				flies[ids[tt]] = obs[obs_for_target[tt]]
			# just copy targets over that didn't match any obs
			# (This assumes the fly isn't moving)
		#	else:
		#		flies[ids[tt]] = targets[ids[tt]]

		# deal with the targets that didn't match any obs
		for oo in range(len(obs)):
			if unass_obs[oo]:
				obs[oo].ID = self.getNewID()
				flies[obs[oo].ID] = obs[oo]
		return flies

	def track(self):
		output = []
#	for i in range(len(history)):
		nframe = 0
		for i in range(len(self.history)):
			print "*** Frame %d ***" % i
			currStates = self.history[i]
			# initialize flies with ID
			if nframe == 0:
				flies = {}
				for ell in currStates:
					ell.ID = self.getNewID()
					flies[ell.ID] = ell		
				print "targ (%d) = %s"%(len(flies), str(flies))
				print "obs (%d) = %s"%(len(currStates), str(currStates))
			elif nframe == 1:
				flies = self.findFlies(output[nframe-1], output[nframe-1], currStates)
			else:
				flies = self.findFlies(output[nframe-2], output[nframe-1], currStates)	
			output.append(flies)
			nframe += 1
		return output

	def getNewID(self):
		self.numID += 1
		return self.numID

	
