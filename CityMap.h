/**
 * Instruction: Definition of Map Traveller Algorithm
 * revised from project "http://download.csdn.net/detail/simplecoding/469120"
 * 
 * Author: yao62995 <yao_62995@163.com>
 * Last Revise: 2016.10.19
 */

#ifndef TRAVELLER_CITYMAP_H_
#define  TRAVELLER_CITYMAP_H_

#include "time.h"
#include <vector>
#include <string>

#include "DPoint.h"

#define CM_ROOM_CREATE_SIZE     10
#define CM_CITY_DRAW_SIZE       2
#define CM_OFFSET               8

using std::string;

typedef struct stru_gene
{
    int * index;
    double mark;
    int killRate;
}GENE;

class CityMap
{
public:
    CityMap(void);
    ~CityMap(void);

protected:
    DPoint *        m_pDPCitySites;  // city positions
    int             m_iRoomSize;   // space size 
    int             m_iCityNum;   // city number
    bool            m_bCompute;  // is computing
    bool            m_bKillMsg;  // kill signal
    double **       m_ppdDistanceM;  // distance matrix
    int             m_iDMSize;  // matrix size
    int *           m_piBestIndex;   // best sequence index
    int *           m_piCurrBestIndex;    // current best sequence index
    int             m_GenNum;   // generation number
    int             m_bestGen;  // best generation
    double          m_dBestMark;  // best mark value
    int             m_iJumpCountdown;  // jump count down counter
    int             m_iJumpCount;  // jump count
    double          m_dAVGMark;  // average mark value
    int             m_tsTimeUsed;  // time usage
    char            m_stDesc[200];   // state description

public:
    // global argument
    static int      CM_SEED_NUM;  // seed number
    static int      CM_CHILDREN_NUM;  // children number
    static int      CM_MAX_TRANS_NUM;  // max number of trans
    static int      CM_JUMP_COUNTDOWN_INIT;   // count down init value
    static double   CM_JUMP_COUNTDOWN_INC;   // count down increment
    static bool     CM_DEBUG_MODE;  // mode


protected:
    // variant function
    static void Variant(GENE & gsource, GENE & gdest, int * ptemp, int size, int varate);
    // working thread
    static int ThreadProc(void* pParam);

    // destroy distance matrix
    void DestroyDistanceMatrix();
    // compute distance matrix
    void ComputeDistanceMatrix();
    // mark score
    void Mark(GENE & gene);
    //  quadrangle optimization
    void QuadrangleOptimise(GENE & gene);

public:
    // get state description
    char* GetStateDescription();

    std::vector<int> Compute(int time_out);

    // start compute
    bool StartCompute();
    // stop compute
    bool StopCompute();
    // is computing
    inline bool IsComputing() {return m_bCompute;}

    // get average mark 
    inline double GetAVGMark() {return this->m_dAVGMark;}
    // get best mark
    inline double GetBestMark() {return this->m_dBestMark;}
    // get city number
    inline int GetCityNum() {return this->m_iCityNum;}

    // clear data
    void Clear();
    // add city
    void AddCity(double x, double y, int id);
    // delete city
    void DeleteCity(int index);
    
    // get city axis
    inline DPoint GetCitySite(int index)
    {
        if(index >= 0 && index < this->m_iCityNum)
        {
            return this->m_pDPCitySites[index];
        }
        return DPoint(0.0, 0.0);
    }
    // set city axis
    inline void SetCitySite(int index , double x, double y)
    {
        if(!m_bCompute && index >= 0 && index < this->m_iCityNum)
        {
            this->m_pDPCitySites[index].x = x;
            this->m_pDPCitySites[index].y = y;
        }
    }
};

#endif