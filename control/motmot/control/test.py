import zmq
import random
import time
import math
import sys

REQUEST_TIMEOUT = 2500
REQUEST_RETRIES = 3
SERVER_ENDPOINT = "tcp://localhost:5556"

class ODDControlWorker(Process):
  def __init__(self, task_q):
    Process.__init__(self)
    self.task_q = task_q
    self.save_to_disk = False
    self.context = zmq.context(1)
    self.client = self.context.socket(zmq.REQ)
    self.poll = zmq.Poller()
    self.poll.register(client, zmq.POLLIN)
    time.sleep(1)
    return
  
# pass tuples (task_name, task_args) where task_args is a dict
  def run(self):
    proc_name = self.name
    while True:
# get command off task
      next_task_name, next_task_args = self.task_q.get()
      task = getattr(self, next_task_name)
      result = task(**next_task_args)
    else:
      self.context.term()
      break

  def open_log_file(self, log_fname):
    pass

  def close_log_file(self):
    pass

  def update(self, timestamp, framenum):
    pass
 
  def send_odor_command(self, odor, stim, delay, timestamp, framenumber):
    blank = int(8*int(math.floor(odor/8)))
    self.client.send("%d %d %d %d " % (odor, stim, delay, blank))
    if self.save_to_disk:
      loginfo = "Sent odor %d %d %d %d at T=%f F=%d\n" % (odor, stim, delay, blank, timestamp, framenumber)
      self.log_file.write(loginfo)
    time.sleep(stim + delay)
    expect_reply = True
    while expect_reply:
      socks = dict(poll.poll(REQUEST_TIMEOUT))
      if socks.get(client) == zmq.POLLIN:
        reply = client.recv()
        if not reply:
          break
        else:
          if self.save_to_disk:
            self.log_file.write(reply)
          expect_reply = False
      else:
        # lost connection with server...try to reconnect
        client.setsockopt(zmq.LINGER, 0)
        client.close()
        poll.unregister(client)
        client.connect(SERVER_ENDPOINT)
        poll.register(client, zmq.POLLIN)
        client.send(request)
