#!/usr/bin/env python
#-*- coding:utf8-*-
import os
import sys

import threading
import zmq
import json



json_data = {
        'time_limit': 1000, # MS
        'program_id': 1,
        'code_type': 0,
        'source_code': 'only test',
        'run_times': 3,
        'memory_limit': 40000, # KB
        'run_id': 0,
        'trace': False,
        'runner': 1000
        # 'currentTime':
    }


class Request(object):
    def __init__(self, sock_front_addr, context):
        self.sock_addr = sock_front_addr
        # self.context = zmq.Context()
        self.context = context

        self.sock = self.context.socket(zmq.REQ)
        self.sock.connect(sock_front_addr)

    def send(self, userdata):
        self.sock.send(userdata)

    def recv(self):
        return self.sock.recv()

def requestOne(data, cxt):
    sock_front_addr = "tcp://0.0.0.0:7878"
    req = Request(sock_front_addr, cxt)
    req.send(data)
    rmsg = req.recv()
    print rmsg



def judge_proc(filepath):
    context = zmq.Context()
    # while True:
        # data = q.get()
    json_data['source_code'] = file(filepath).read()
    json_data['run_id'] = 0
    json_data['code_type'] = 0
    py_data = json.dumps(json_data, encoding="utf-8")
    t = threading.Thread(target=requestOne, args=(py_data, context))
    t.start()
    t.join()

filepath = sys.argv[1]

judge_proc(filepath)



        # submitdata = convert(data)
        # run = Run()
        # res = run.judge(submitdata['code_path'], submitdata['test_path'],
        #                 submitdata['datacounts'])
        # print res
        # result = res['result']
        # user = User.objects.get(id=submitdata['user_id'])
        # problem_id = submitdata['problem_id']
        # problem = Problem.objects.get(id=problem_id)
        # submit = Submit.objects.get(id=submitdata['submit_id'])
        # try:
        #     contest_id = submitdata['contest_id']
        #     contest = Contest.objects.get(pk=contest_id)
        #     if contest.status == 0:
        #         start_time = contest.start_time
        #         static, created = ContestStatics.objects.get_or_create(contest=contest, problem_id=problem_id)
        #         template, created = ContestTemplate.objects.get_or_create(problem__id=problem_id, contest=contest)
        #         paper, created = ContestPaper.objects.get_or_create(contest=contest, user=user)
        #         if ContestUserProblem.objects.filter(template=template,
        #             paper=paper, is_aced=True).exists():
        #             pro_isac = True
        #         else:
        #             pro_isac = False
        #     is_contest = True
        # except:
        #     is_contest = False
        # try:
        #     chapter_id = submitdata['chapter_id']
        #     chapter = Chapter.objects.get(id=chapter_id)
        #     user_chapter = UserChapter.objects.get_or_create(user=user, chapter=chapter)
        #     user_chapter_problem = UserChapterProblem.objects.get_or_create(
        #         user=user,
        #         problem=problem,
        #         chapter=chapter)
        #     print user_chapter_problem
        #     print user_chapter_problem[0].aced
        #     if user_chapter_problem[0].aced:
        #         is_chapter = False
        #     else:
        #         is_chapter = True
        # except:
        #     is_chapter = False
        # if result == 'Accepted':
        #     time_used = int(res['timeused'])
        #     memory = int(res['memoryused'])
        #     pro_ac_update(user, problem)
        #     daysubmit_ac_update()
        #     if is_contest and contest.status==0:
        #         static_ac_update(static, int(submitdata['compiler']))
        #         rank_ac_update(template, paper, start_time, pro_isac, contest)
        # else:
        #     time_used = 0
        #     memory = 0
        #     pro_wr_update(user, problem)
        #     if is_contest and contest.status==0:
        #         static_wr_update(static, result, int(submitdata['compiler']))
        #         rank_wr_update(template, paper, start_time, pro_isac, contest)
        # if is_contest:
        #     submit_update(submit, time_used, memory, result, contest, user)
        # else:
        #     submit_update(submit, time_used, memory, result, '', '')
        # daysubmit_update()#for online statistic


# q = Queue()
# pro = Process(target=judge_proc, args=(q,))
# pro.start()
# while True:
#     cli, addr = sock.accept()
#     data = cli.recv(1024)
#     q.put(data)
#     cli.close()
