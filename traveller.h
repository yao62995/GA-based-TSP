/**
 * Instruction: client API for swig 
 * 
 * Author: yao62995 <yao_62995@163.com>
 * Last Revise: 2016.10.19
 */

#ifndef TRAVELLER_TRAVELLER_H_
#define TRAVELLER_TRAVELLER_H_

#include <vector>
#include "CityMap.h"

class Traveller {
public:
    Traveller() {
        // set default value for params
        CityMap::CM_SEED_NUM = 12;
        CityMap::CM_CHILDREN_NUM = 36;
        CityMap::CM_MAX_TRANS_NUM = 10;
        CityMap::CM_JUMP_COUNTDOWN_INIT = 80;
        CityMap::CM_JUMP_COUNTDOWN_INC = 3.6;
    }
    // set global params
    inline void SetSeedNum(int seed_num) {CityMap::CM_SEED_NUM = seed_num;}
    inline void SetChildNum(int child_num) {CityMap::CM_CHILDREN_NUM = child_num;}
    inline void SetMaxTrans(int max_trans) {CityMap::CM_MAX_TRANS_NUM = max_trans;}
    inline void SetCountDownInit(int count_down_init) {CityMap::CM_JUMP_COUNTDOWN_INIT = count_down_init;}
    inline void SetCountDownInc(double count_down_inc) {CityMap::CM_JUMP_COUNTDOWN_INC = count_down_inc;}
    inline void SetDebugMode(bool debug) {CityMap::CM_DEBUG_MODE = debug;}
    
    void AddCity(double px, double py, int id) {m_cityMap.AddCity(px, py, id);}

    std::vector<int> Compute(int time_out) {return m_cityMap.Compute(time_out);}

    char* GetStateDesc() {return m_cityMap.GetStateDescription();}

private:
    CityMap m_cityMap;
};

#endif
