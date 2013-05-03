import zmq
import random
import time
import math
import sys
import multiprocessing
import logging

REQUEST_TIMEOUT = 2500
REQUEST_RETRIES = 3
SERVER_ENDPOINT = "tcp://localhost:5556"

class ODDControlWorker(multiprocessing.Process):
  def __init__(self, task_q):
    multiprocessing.Process.__init__(self)
    self.task_q = task_q
    self.save_to_disk = False
    self.log_file_name = ""
    self.log_file = None
    return

# pass tuples (task_name, task_args) where task_args is a dict
  def run(self):
    self.context = zmq.Context(1)

    self.odors = read_odd_file("odors.txt")
    self.client = self.context.socket(zmq.REQ)
    self.client.connect(SERVER_ENDPOINT)

    self.poll = zmq.Poller()
    self.poll.register(self.client, zmq.POLLIN)

    time.sleep(1)
    while True:
# get command off task
      next_task_name, next_task_args = self.task_q.get()
      if next_task_name == "quit":
        self.context.term()
        break
      else:
        task = getattr(self, next_task_name)
        result = task(**next_task_args)
    #else:
    #  self.context.term()
    #  break

  def open_log_file(self, log_fname):
    self.save_to_disk = True
    self.log_file = open(log_fname, 'w')
    self.log_file_name = log_fname

  def close_log_file(self):
    self.save_to_disk = False
    self.log_file_name = ''
    self.log_file.close()
    self.log_file = None

  def send_odor_command(self, odor, stim, delay, timestamp, framenumber):
    blank = int(8*int(math.floor(odor/8)))
    request = "%d %d %d %d " % (odor, stim, delay, blank)
    self.client.send(request)
    if self.save_to_disk:
      loginfo = "S %d %d %d %d %d %d %d\n" % (odor, stim, delay, blank, timestamp, framenumber, time.mktime(time.gmtime()))
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
            loginfo = "R %d %d %d %d %d %d %d\n" % (odor, stim, delay, blank, timestamp, framenumber, time.mktime(time.gmtime()))
            self.log_file.write(loginfo)
          expect_reply = False
#        else:
#          # lost connection with server...try to reconnect
#          self.client.setsockopt(zmq.LINGER, 0)
#          self.client.close()
#          self.poll.unregister(self.client)
#          retries_left -= 1
#          if retries_left == 0:
#            break
#          self.client = self.context.socket(zmq.REQ)
#          self.client.connect(SERVER_ENDPOINT)
#          self.poll.register(self.client, zmq.POLLIN)
#          self.client.send(request)
#          print "retrying..."

# File format
# name1 port1 delay1 on1
# name2 port2 delay2 on2
# etc...
def read_odd_file(fname):
  f = open(fname,'r')
  odors = []
  for line in f:
    elems = line.rstrip().split()
    name = elems[0]
    odor = int(elems[1])
    delay = int(elems[2])
    stim = int(elems[3])
    odors.append((odor, delay, stim))
  return odors

def main():
  #multiprocessing.log_to_stderr(logging.DEBUG)
  q = multiprocessing.Queue()
  worker = ODDControlWorker(q)
  worker.start()
  q.put(("open_log_file", {"log_fname":"log.txt"}))
  odors = read_odd_file("odors.txt")
  for o in odors:
    print o
  go = True
  sending_odor = False
  idx = 0
  t0 = time.time()
  total_delay = 0
  while go:
    t1 = time.time()
    if not sending_odor:
      print "Sending odor ", idx
      o = odors[idx]
      total_delay = o[1] + o[2]
      q.put(("send_odor_command", {"odor":o[0], "stim":o[1], "delay":o[2], "timestamp":t1, "framenumber":0}))
      sending_odor = True
      t0 = time.time()
    if (t1 - t0) > total_delay:
      sending_odor = False
      if (idx+1) < len(odors):
        idx += 1
      else:
        break
  q.put(("close_log_file",{}))
  worker.join()
  return

if __name__ == "__main__":
  main()
