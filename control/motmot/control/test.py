import zmq
import random
import time
import math

context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.bind("tcp://*:5556")
time.sleep(1)
for odor in range(16):
#  odor = random.randint(0, 15)
  delay = 10
  stim = 10
  blank = int(8*int(math.floor(odor/8)))
  socket.send("%d %d %d %d " % (odor, stim, delay, blank))
  print "Sent odor request %d %d %d %d at %f" % (odor, stim, delay, blank, time.mktime(time.gmtime()))
  time.sleep(delay+stim)
