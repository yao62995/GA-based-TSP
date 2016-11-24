/**
 * Instruction: Implemention of Map Traveller Algorithm,
 * revised from project "http://download.csdn.net/detail/simplecoding/469120"
 * 
 * Author: yao62995 <yao_62995@163.com>
 * Last Revise: 2016.10.19
 */

#include "CityMap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <chrono>
#include <thread>

#ifdef WIN32
#define CLOCKS_PER_MILLI_SEC ((clock_t)1)
#else
#define CLOCKS_PER_MILLI_SEC  1000
#endif


using std::vector;


void Sleep(int milliseconds)
{
    /*#ifdef WIN32
    Sleep(milliseconds);
    #else
    usleep(milliseconds * 1000);
    #endif // win32*/
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

CityMap::CityMap(void)
    : m_pDPCitySites(NULL)
    , m_iRoomSize(0)
    , m_iCityNum(0)
    , m_bCompute(false)
    , m_bKillMsg(false)
    , m_ppdDistanceM(NULL)
    , m_iDMSize(0)
    , m_piBestIndex(NULL)
    , m_piCurrBestIndex(NULL)
    , m_GenNum(1)
    , m_bestGen(1)
    , m_dBestMark(1.0)
    , m_iJumpCountdown(CM_JUMP_COUNTDOWN_INIT)
    , m_iJumpCount(0)
    , m_dAVGMark(1.0)
    , m_tsTimeUsed(0)
{
}

CityMap::~CityMap(void)
{
    if(this->m_pDPCitySites)
    {
        delete [] m_pDPCitySites;
        m_pDPCitySites = NULL;
    }
    if(this->m_piBestIndex)
    {
        delete [] m_piBestIndex;
        m_piBestIndex = NULL;
    }
    if(this->m_piCurrBestIndex)
    {
        delete [] m_piCurrBestIndex;
        m_piCurrBestIndex = NULL;
    }
    this->m_iCityNum = 0;
    this->m_iRoomSize = 0;
}


// 全局参数
// 种子数量
int CityMap::CM_SEED_NUM = 12;
// 子女数量
int CityMap::CM_CHILDREN_NUM = 36;
// 最大转移数
int CityMap::CM_MAX_TRANS_NUM = 10;
// 灾变初始值
int CityMap::CM_JUMP_COUNTDOWN_INIT = 800;
// 灾变倍增值
double CityMap::CM_JUMP_COUNTDOWN_INC = 3.6;
// debug mode
bool CityMap::CM_DEBUG_MODE = false;


char* CityMap::GetStateDescription() {
    sprintf(m_stDesc, "%d个城市 第%d代 最优产生于第%d代 分数%lf "
            "里程%lf 灾变倒计数%04d 灾变计数%03d 群体平均分%lf 用时%d(ms)", 
            m_iCityNum, m_GenNum, m_bestGen, m_dBestMark,
            1.0 / m_dBestMark * 6367000.0, m_iJumpCountdown, m_iJumpCount, m_dAVGMark,m_tsTimeUsed);
    return m_stDesc;
}

// 变异函数
void CityMap::Variant(GENE & gsource, GENE & gdest, int * ptemp, int size, int varate)
{
    static int i;
    static int j, k, l, n, m;
    static int tmp;
    memcpy(gdest.index, gsource.index, sizeof(int) * size);
    for(i = 0; i < varate; i++)
    {
        switch(rand() % 2)
        {
        case 0:
            // 逆序变异
            {
                j = rand() % size;
                k = rand() % size;
                if(j == k)
                {
                    k = (k + 1) % size;
                }
                if(j > k)
                {
                    k += size;
                    for(l = j; l < j + k - l; l++)
                    {
                        n = (j + k - l) % size;
                        m = l % size;
                        tmp = gdest.index[m];
                        gdest.index[m] = gdest.index[n];
                        gdest.index[n] = tmp;
                    }
                }
                else
                {
                    for(l = j; l < j + k - l; l++)
                    {
                        tmp = gdest.index[l];
                        gdest.index[l] = gdest.index[j + k - l];
                        gdest.index[j + k - l] = tmp;
                    }
                }
            }
            break;
        case 1:
            // 转移变异
            {
                j = rand() % size;
                k = rand() % CM_MAX_TRANS_NUM;
                if(k == 0)
                {
                    k = 1;
                }
                l = rand() % (size - k) + 1;
                n = (j + k) % size + l > size ? size - (j + k) % size : l;
                memcpy(ptemp, gdest.index + (j + k) % size, n * sizeof(int));
                if(n < l)
                {
                    memcpy(ptemp + n, gdest.index, (l - n) * sizeof(int));
                }
                n = j + k > size ? size - j : k;
                memcpy(ptemp + l, gdest.index + j, n * sizeof(int));
                if(n < k)
                {
                    memcpy(ptemp + l + n, gdest.index, (k - n) * sizeof(int));
                }
                m = k + l;
                n = j + m > size ? size - j :m;
                memcpy(gdest.index + j, ptemp, n * sizeof(int));
                if(n < m)
                {
                    memcpy(gdest.index, ptemp + n, (m - n) * sizeof(int));
                }
            }
            break;
        default:
            break;
        }
    }
}

// 辅助线程
int CityMap::ThreadProc(void* pParam)
{
    CityMap * pClass = (CityMap *)pParam;
    if(pClass->m_iCityNum <= 1)
    {
        return 0;
    }
    srand((int)time(NULL));
    pClass->m_bCompute = true;
    static int i, j;
    static GENE tgene;
    static int bullet;
    static int totalBullet;
    static int maxCountdown = CM_JUMP_COUNTDOWN_INIT;
    // 分配空间
    int num = pClass->m_iCityNum;
    int * ptmp = new int[num];
    int commSize = CM_SEED_NUM * (1 + CM_CHILDREN_NUM);
    GENE * comm = new GENE[commSize];
    for(i = 0; i < commSize; i++)
    {
        comm[i].index = new int [num];
        comm[i].mark  = 0.0;
    }
    // 计算距离矩阵
    pClass->ComputeDistanceMatrix();
    // 生成初始群落
    clock_t begin = clock();
    pClass->m_GenNum = 1;
    pClass->m_iJumpCount = 0;
    memcpy(comm[0].index, pClass->m_piBestIndex, sizeof(int) * num);
    pClass->Mark(comm[0]);
    pClass->QuadrangleOptimise(comm[0]);
    pClass->m_dBestMark = comm[0].mark;
    pClass->m_bestGen = pClass->m_GenNum;
    double maxMark = comm[0].mark;
    int maxIndex = 0;
    double totalMark = maxMark;
    pClass->m_iJumpCountdown = CM_JUMP_COUNTDOWN_INIT;
    for(i = 1; i < CM_SEED_NUM; i++)
    {
        // 变异 （逆序， 转移）
        Variant(comm[0], comm[i], ptmp, num, pClass->m_iJumpCountdown);
        // 打分
        pClass->Mark(comm[i]);
        totalMark += comm[i].mark;
        if(maxMark < comm[i].mark)
        {
            maxMark = comm[i].mark;
            maxIndex = i;
        }
    }
    // 移动最优基因
    if(maxIndex != 0)
    {
        // maxIndex 与 0 交换
        tgene.index = comm[0].index;
        tgene.mark = comm[0].mark;
        comm[0].index = comm[maxIndex].index;
        comm[0].mark = comm[maxIndex].mark;
        comm[maxIndex].index = tgene.index;
        comm[maxIndex].mark = tgene.mark;
        maxIndex = 0;
        // 四边形优化
        pClass->QuadrangleOptimise(comm[0]);
        maxMark = comm[0].mark;
        memcpy(pClass->m_piCurrBestIndex, pClass->m_piBestIndex, sizeof(int) * num);
        memcpy(pClass->m_piBestIndex, pClass->m_piCurrBestIndex, sizeof(int) * num);
        pClass->m_dBestMark = maxMark;
        pClass->m_bestGen = pClass->m_GenNum;
    }
    int indNum = CM_SEED_NUM;
    pClass->m_dAVGMark = (totalMark) / indNum;
    // 开始进化
    while(!pClass->m_bKillMsg)
    {
        totalMark = 0.0;
        // 变异
        for(i = 0; i < CM_SEED_NUM; i++)
        {
            totalMark += comm[i].mark;
            for(j = 0; j < CM_CHILDREN_NUM; j++)
            {
                Variant(comm[i], comm[indNum], ptmp, num, 1);
                pClass->Mark(comm[indNum]);
                totalMark += comm[indNum].mark;
                if(maxMark < comm[indNum].mark)
                {
                    maxMark = comm[indNum].mark;
                    maxIndex = indNum;
                }
                indNum++;
            }
        }
        pClass->m_dAVGMark = (totalMark) / indNum;
        // 移动最优基因
        if(maxIndex != 0)
        {
            tgene.index = comm[0].index;
            tgene.mark = comm[0].mark;
            comm[0].index = comm[maxIndex].index;
            comm[0].mark = comm[maxIndex].mark;
            comm[maxIndex].index = tgene.index;
            comm[maxIndex].mark = tgene.mark;
            maxIndex = 0;
            memcpy(pClass->m_piCurrBestIndex, comm[0].index, sizeof(int) * num);
            if(maxMark > pClass->m_dBestMark)
            {
                memcpy(pClass->m_piBestIndex, pClass->m_piCurrBestIndex, sizeof(int) * num);
                pClass->m_dBestMark = maxMark;
                pClass->m_bestGen = pClass->m_GenNum;
            }
            int forcastCountdown = int((maxCountdown - pClass->m_iJumpCountdown) * CM_JUMP_COUNTDOWN_INC);
            if(forcastCountdown > maxCountdown)
            {
                maxCountdown = forcastCountdown;
            }
            pClass->m_iJumpCountdown = maxCountdown;
        }
        else
        {
            pClass->m_iJumpCountdown--;
            if(pClass->m_iJumpCountdown <= 0)
            {
                pClass->QuadrangleOptimise(comm[0]);
                if(maxMark < comm[0].mark)
                {
                    pClass->m_iJumpCountdown = maxCountdown;
                    maxMark = comm[0].mark;
                    memcpy(pClass->m_piCurrBestIndex, comm[0].index, sizeof(int) * num);
                    if(maxMark > pClass->m_dBestMark)
                    {
                        memcpy(pClass->m_piBestIndex, pClass->m_piCurrBestIndex, sizeof(int) * num);
                        pClass->m_dBestMark = maxMark;
                        pClass->m_bestGen = pClass->m_GenNum;
                    }
                }
                else
                {
                    srand((unsigned int)time(NULL));
                    maxCountdown = CM_JUMP_COUNTDOWN_INIT;
                    pClass->m_iJumpCountdown = maxCountdown;
                    // 已经陷入局部最优，灾变
                    pClass->m_iJumpCount++;
                    Variant(comm[0], comm[0], ptmp, num, 20);
                    pClass->Mark(comm[0]);
                    maxMark = comm[0].mark;
                    for(i = 1; i < CM_SEED_NUM; i++)
                    {
                        Variant(comm[0], comm[i], ptmp, num, 20);
                        pClass->Mark(comm[i]);
                        totalMark += comm[i].mark;
                        if(maxMark < comm[i].mark)
                        {
                            maxMark = comm[i].mark;
                            maxIndex = i;
                        }
                    }
                    // 移动最优基因
                    if(maxIndex != 0)
                    {
                        tgene.index = comm[0].index;
                        tgene.mark = comm[0].mark;
                        comm[0].index = comm[maxIndex].index;
                        comm[0].mark = comm[maxIndex].mark;
                        comm[maxIndex].index = tgene.index;
                        comm[maxIndex].mark = tgene.mark;
                        maxIndex = 0;
                    }
                    indNum = CM_SEED_NUM;
                }
            }
        }
        // 轮盘赌
        totalMark -= comm[0].mark;
        totalBullet = 0;
        for(i = 1; i < indNum; i++)
        {
            comm[i].killRate = int(10000.0 * comm[i].mark / totalMark);
            totalBullet += comm[i].killRate;
        }
        while(indNum > CM_SEED_NUM)
        {
            bullet = rand() % totalBullet;
            for(i = 1; i < indNum; i++)
            {
                if(bullet <= comm[i].killRate)
                {
                    // 命中
                    totalBullet -= comm[i].killRate;
                    tgene.index = comm[indNum - 1].index;
                    tgene.mark = comm[indNum - 1].mark;
                    tgene.killRate = comm[indNum - 1].killRate;
                    comm[indNum - 1].index = comm[i].index;
                    comm[indNum - 1].mark = comm[i].mark;
                    comm[indNum - 1].killRate = comm[i].killRate;
                    comm[i].index = tgene.index;
                    comm[i].mark = tgene.mark;
                    comm[i].killRate = tgene.killRate;
                    indNum--;
                    break;
                }
                else
                {
                    bullet -= comm[i].killRate;
                }
            }
        }
        pClass->m_GenNum++;
        clock_t end = clock();
        pClass->m_tsTimeUsed = (end - begin) / CLOCKS_PER_MILLI_SEC;
        if (CityMap::CM_DEBUG_MODE && (pClass->m_GenNum % 200 == 0)) {
            printf("%s\n", pClass->GetStateDescription());
        }
    }
    // 释放空间
    for(i = 0; i < commSize; i++)
    {
        delete [] comm[i].index;
        comm[i].index = NULL;
    }
    delete [] comm;
    comm = NULL;

    // 销毁距离矩阵
    pClass->DestroyDistanceMatrix();

    tgene.index = NULL;
    pClass->m_bCompute = false;

    return 0;   // thread completed successfully
}

// 销毁距离矩阵
void CityMap::DestroyDistanceMatrix()
{
    if(this->m_ppdDistanceM)
    {
        for(int i = 1; i < this->m_iDMSize; i++)
        {
            delete [] this->m_ppdDistanceM[i];
            this->m_ppdDistanceM[i] = NULL;
        }
        delete [] this->m_ppdDistanceM;
        this->m_ppdDistanceM = NULL;
    }
}

// 计算距离矩阵
void CityMap::ComputeDistanceMatrix()
{
    int i, j;
    this->DestroyDistanceMatrix();
    // 创建存储空间
    this->m_iDMSize = this->m_iCityNum;
    this->m_ppdDistanceM = new double * [m_iDMSize];
    m_ppdDistanceM[0] = NULL;
    for(i = 1; i < this->m_iDMSize; i++)
    {
        this->m_ppdDistanceM[i] = new double[i];
    }
    // 计算距离
    for(i = 1; i < m_iDMSize; i++)
    {
        for(j = 0; j < i; j++)
        {
            m_ppdDistanceM[i][j] = m_pDPCitySites[i].Distance(this->m_pDPCitySites[j]);
        }
    }
}

// 打分
void CityMap::Mark(GENE & gene)
{
    static int i;
    gene.mark = this->m_pDPCitySites[gene.index[0]].Distance(this->m_pDPCitySites[gene.index[m_iCityNum - 1]]);
    for(i = 1; i < this->m_iCityNum; i++)
    {
        // gene.mark += this->m_pDPCitySites[gene.index[i]].Distance(this->m_pDPCitySites[gene.index[i - 1]]);
        if(gene.index[i] > gene.index[i - 1])
        {
            gene.mark += m_ppdDistanceM[gene.index[i]][gene.index[i - 1]];
        }
        else
        {
            gene.mark += m_ppdDistanceM[gene.index[i - 1]][gene.index[i]];
        }
    }
    
    if(gene.mark < 0.00001)
    {
        gene.mark = 1.0;
    }
    else
    {
        gene.mark = 1.0 / gene.mark;
    }
}

// 四边形优化
void CityMap::QuadrangleOptimise(GENE & gene)
{
    if(this->m_iCityNum < 3)
    {
        return;
    }
    static int i, tmp;
    static bool change;
    static double d0, d1, d2, d3;
    static int count;
    count = 0;
    do
    {
        change = false;
        d0 = this->m_pDPCitySites[gene.index[0]].Distance(
                this->m_pDPCitySites[gene.index[m_iCityNum - 1]]);
        d1 = this->m_pDPCitySites[gene.index[1]].Distance(
                this->m_pDPCitySites[gene.index[m_iCityNum - 1]]);
        // 寻找钝角
        for(i = 1; i < this->m_iCityNum - 1; i++)
        {
            d2 = this->m_pDPCitySites[gene.index[i]].Distance(
                    this->m_pDPCitySites[gene.index[i + 1]]);
            d3 = this->m_pDPCitySites[gene.index[i - 1]].Distance(
                    this->m_pDPCitySites[gene.index[i + 1]]);
            if(d0 + d2 > d1 + d3)
            {
                // 交换公共点顺序
                tmp = gene.index[i];
                gene.index[i] = gene.index[i - 1];
                gene.index[i - 1] = tmp;
                change = true;
            }
            d0 = this->m_pDPCitySites[gene.index[i]].Distance(
                this->m_pDPCitySites[gene.index[i - 1]]);
            d1 = d3;
        }
        count++;
        if(count > 5)
        {
            break;
        }
    }
    while(change);
    this->Mark(gene);
}

vector<int> CityMap::Compute(int time_out) {
    clock_t begin = clock();
    // start compute
    StartCompute();
    int elapsed_tm = 0;
    do {
        Sleep(50);
        clock_t end = clock();
        elapsed_tm = (end - begin) / CLOCKS_PER_MILLI_SEC; 
    } while (elapsed_tm < time_out);
    // stop compute
    StopCompute();
    // record result
    vector<int> ret;
    for (int i = 0; i < this->m_iCityNum; i++) {
        int i_city = *(this->m_piBestIndex + i);
        ret.push_back(this->m_pDPCitySites[i_city].id);
    }
    return ret;
}

// start compute
bool CityMap::StartCompute()
{
    if(this->m_bCompute == false)
    {
        this->m_bKillMsg = false;
        std::thread(CityMap::ThreadProc, this).detach();
    }
    return this->m_bCompute;
}

// stop compute
bool CityMap::StopCompute()
{
    this->m_bKillMsg = true;
    for(int i = 0; i < 10 && m_bCompute; i++)
    {
        Sleep(50);
    }
    this->m_bKillMsg = false;
    return !m_bCompute;
}


// clear data
void CityMap::Clear()
{
    this->m_tsTimeUsed = 0;
    this->m_iCityNum = 0;
}

// add city
void CityMap::AddCity(double x, double y, int id)
{
    if(this->m_bCompute)
    {
        return;
    }
    // extent memory
    if(this->m_iCityNum >= this->m_iRoomSize)
    {
        int newSize = this->m_iRoomSize + CM_ROOM_CREATE_SIZE;
        DPoint * newRoom = new DPoint [newSize];
        if(this->m_pDPCitySites)
        {
            memcpy(newRoom, m_pDPCitySites, sizeof(DPoint) * m_iRoomSize);
            delete [] m_pDPCitySites;
        }
        int * newIndex = new int [newSize];
        int * newCurrIndex = new int [newSize];
        if(this->m_pDPCitySites)
        {
            memcpy(newIndex, m_piBestIndex, sizeof(int) * m_iRoomSize);
            delete [] m_piBestIndex;
            memcpy(newCurrIndex, m_piCurrBestIndex, sizeof(int) * m_iRoomSize);
            delete [] m_piCurrBestIndex;
        }
        m_pDPCitySites = newRoom;
        m_piBestIndex = newIndex;
        m_piCurrBestIndex = newCurrIndex;
        m_iRoomSize = newSize;
    }
    // add city
    this->m_pDPCitySites[m_iCityNum].x = x;
    this->m_pDPCitySites[m_iCityNum].y = y;
    this->m_pDPCitySites[m_iCityNum].id = id;
    m_piBestIndex[m_iCityNum] = m_iCityNum;
    m_piCurrBestIndex[m_iCityNum] = m_iCityNum;
    m_iCityNum++;
}

// delete city
void CityMap::DeleteCity(int index)
{
    if(this->m_bCompute)
    {
        return;
    }
    if(index >= 0 && index < m_iCityNum)
    {
        int i;
        for(i = 0; i < m_iCityNum - 1; i++)
        {
            if(m_piBestIndex[i] == index)
            {
                break;
            }
            else if(m_piBestIndex[i] > index)
            {
                m_piBestIndex[i]--;
            }
        }
        for(; i < m_iCityNum - 1; i++)
        {
            m_piBestIndex[i] = m_piBestIndex[i + 1];
            if(m_piBestIndex[i] > index)
            {
                m_piBestIndex[i]--;
            }
        }

        for(i = 0; i < m_iCityNum - 1; i++)
        {
            if(m_piCurrBestIndex[i] == index)
            {
                break;
            }
            else if(m_piCurrBestIndex[i] > index)
            {
                m_piCurrBestIndex[i]--;
            }
        }
        for(; i < m_iCityNum - 1; i++)
        {
            m_piCurrBestIndex[i] = m_piCurrBestIndex[i + 1];
            if(m_piCurrBestIndex[i] > index)
            {
                m_piCurrBestIndex[i]--;
            }
        }

        for(; index < m_iCityNum - 1; index++)
        {
            m_pDPCitySites[index] = m_pDPCitySites[index + 1];
        }
        m_iCityNum--;
    }
}

