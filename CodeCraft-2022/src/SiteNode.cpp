#include "SiteNode.h"
#include "algorithm"

void SiteNode::init(string sn, int tBW){
    this->sn = sn;
    this->tBW = tBW;
}

void SiteNode::updataHistory()
{
    sort(historyBW.begin(), historyBW.end());
    int i94 = historyBW.size()-1-historyBW.size()/20;
    boundI94 = historyBW[i94];
}