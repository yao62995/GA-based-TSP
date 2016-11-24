/**
 * Instruction: Test Case File
 * 
 * Author: yao62995 <yao_62995@163.com>
 * Last Revise: 2016.10.19
 */

#include <vector>
#include <iostream>
#include "CityMap.h"
#include "traveller.h"

using namespace std;

int randint(int min, int max) {
    return min + (rand() % (int)(max - min + 1));
}

double rand(double min, double max) {
    int min_int = (int)(min * 1000);
    int max_int = (int)(max * 1000);
    int rand_int = min_int + (rand() % (int)(max_int - min_int + 1));
    return (double)(rand_int) / 1000.0;
}

int main(int argc, char** argv) {
    CityMap::CM_SEED_NUM = 12;
    CityMap::CM_CHILDREN_NUM = 35;
    CityMap::CM_MAX_TRANS_NUM = 10;
    CityMap::CM_JUMP_COUNTDOWN_INIT = 800;
    CityMap::CM_JUMP_COUNTDOWN_INC = 3.6;
    CityMap::CM_DEBUG_MODE = true;
    Traveller cm;
    cout << "usage: ./bin_name city_num  time_out" << endl;
    int i;
    int city_num = atoi(argv[1]);
    int time_out = atoi(argv[2]);
    for (i = 0; i < city_num; i++) {
        double px = rand(30.0, 40.0);
        double py = rand(110.0, 130.0);
        cm.AddCity(px, py, i);
        // cm.AddCity((100 + rand() % 9800) / 10000.0, (100 + rand() % 9800) / 10000.0, i);
    }
    
    vector<int> ret = cm.Compute(time_out);
    /*
    cout << "best index: [";
    for (i = 0; i < ret.size(); i++) {
        cout << ret[i] << ", ";
    }
    cout << "]" << endl;*/
    cout << cm.GetStateDesc() << endl << endl;
    return 0;
}
