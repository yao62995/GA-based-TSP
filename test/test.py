#!/usr/bin/python
#  -*- coding: utf-8 -*-
# author: yao62995 <yao_62995@163.com>

import random
from ga_traveller import Traveller

s = Traveller()


def rand_float(_min, _max):
    return _min + random.random() * (_max - _min)

# 设置城市节点数
city_num = 500
# 设置计算时间
time_out = 20000

traveller = Traveller()
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
traveller.SetDebugMode(True)

for idx in xrange(city_num):
    # 随机生成经度~(30.0, 40.0)，纬度~(110.0, 130.0)之间的地点
    px = rand_float(30.0, 40.0)
    py = rand_float(110.0, 130.0)
    traveller.AddCity(px, py, idx)

ret = traveller.Compute(time_out)
print "result: ", ret
print "state: ", traveller.GetStateDesc()
