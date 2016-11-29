#!/usr/bin/python
#  -*- coding: utf-8 -*-
# author: yao62995 <yao_62995@163.com>

import random
from ga_traveller import Traveller
import matplotlib.pyplot as plt


def rand_float(_min, _max):
    return _min + random.random() * (_max - _min)


# 设置城市节点数
city_num = 1000


def compute(points, time_out):
    traveller = Traveller()
    for idx in xrange(len(points[0])):
        px, py = points[0][idx], points[1][idx]
        traveller.AddCity(px, py, idx)
    # 种子数量
    traveller.SetSeedNum(12)
    # 子女数量
    traveller.SetChildNum(36)
    # 最大转移数
    traveller.SetMaxTrans(10)
    # 灾变初始值
    traveller.SetCountDownInit(800)
    # 灾变倍增值
    traveller.SetCountDownInc(3.6)
    # 调试模式
    traveller.SetDebugMode(False)
    traveller
    ret = traveller.Compute(time_out)
    print "time_out:", time_out, ", state: ", traveller.GetStateDesc()
    return ret


def draw_plot(fig_id, points, label, time_out=0):
    ax = plt.subplot(fig_id)
    time_out *= 1000
    if time_out > 0:
        ret = compute(points, time_out)
    else:
        ret = range(len(points[0]))
    plt.plot(map(lambda x: points[0][x], ret), map(lambda x: points[1][x], ret))
    ax.set_title(label)


points_list = [[], []]
for idx in xrange(city_num):
    # 随机生成经度~(30.0, 40.0)，纬度~(110.0, 130.0)之间的地点
    px = rand_float(30.0, 40.0)
    py = rand_float(110.0, 130.0)
    points_list[0].append(px)
    points_list[1].append(py)

# draw picture
plt.figure(1)
time_out_list = [0, 2, 4, 6, 8, 10]
for i, t in enumerate(time_out_list):
    draw_plot(321+i, points_list, "chart%d (%dsec)" % (i, t), t)
plt.show()
