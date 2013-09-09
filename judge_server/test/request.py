#!/usr/bin/env python
#-*- coding:utf8 -*-
import zmq
import sys
import json
import threading

from time import ctime



class Request(object):
    def __init__(self, sock_front_addr):
        self.sock_addr = sock_front_addr
        self.context = zmq.Context()
        self.sock = self.context.socket(zmq.REQ)
        self.sock.connect(sock_front_addr)

    def send(self, userdata):
        self.sock.send(userdata)

    def recv(self):
        return self.sock.recv()


def requestOnce(codetype, filepath, run_id):
    json_data = {
        'time_limit': 1000, # MS
        'program_id': 1,
        'code_type': codetype,
        'source_code': 'only test',
        'run_times': 3,
        'memory_limit': 40000, # KB
        'run_id': run_id,
        'trace': True,
    	'runner': 1000
        # 'currentTime':
    }

    json_data['source_code'] = file(filepath).read()
    py_data = json.dumps(json_data, encoding="utf-8")
    req = Request("tcp://0.0.0.0:7878")
    # req = Request("ipc://router.ipc")
    req.send(py_data)
    print 'send:', py_data
    print 'recv:', req.recv()


def main(codetype, filepath, nums = 1):
    # while True:
    if nums == 1:
        t = threading.Timer(3, exit)
        t.start()

    threads = []
    for num in xrange(0, nums):
        t = threading.Thread(target=requestOnce, args=(codetype, filepath, num))
        threads.append(t)

    for thread in threads:
    # for num in xrange(0, nums):
        thread.start()

    for thread in threads:
        thread.join()

    print 'all DONE at: ', ctime()

def exit():
    print 'bye'
    sys.exit()


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print "usage: %s CodeType filepath test_num" % sys.argv[0];
        print '''enum CodeType {
                kTypeC = 0,
                kTypeCpp,
                kTypePython,
                kTypeJava,
                kTypeUnknown,
              };'''
    elif len(sys.argv) == 3:
        main(int(sys.argv[1]), sys.argv[2])
    else:
        main(int(sys.argv[1]), sys.argv[2], int(sys.argv[3]))
