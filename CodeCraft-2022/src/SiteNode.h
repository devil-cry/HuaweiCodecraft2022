#ifndef SITENODE_H
#define SITENODE_H

#include <string>
#include "vector"
using namespace std;

class SiteNode{
public:
    string sn;  // 边缘节点的名称
    int tBW; // 边缘节点的总带宽
    int rBW; // 边缘节点的剩余带宽
    int uBWList[8930]; // 记录过去各时间点所使用的带宽
    int mxFlowList[8930];
    int boundI94 = 0;
    int upper_bound_Flow;
    int remainSuperChance;   //在second时刻sid的流量为first 最多存储 T*5/100 + 1 个
    vector<int> historyBW;
    void updataHistory();
    int bwIndexAtTime[8930];
    bool isSuperNodeAtTime[8930]; // 记录当前时间点该边缘节点是否是超级节点
    SiteNode(){};
    void init(string sn, int tBW); // 设置当前边缘节点名称为sn，总带宽为tBW
};



#endif
