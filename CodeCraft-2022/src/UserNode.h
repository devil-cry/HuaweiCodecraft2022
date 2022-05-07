#ifndef USERNODE_H
#define USERNODE_H

#include <string>
#include "vector"
using namespace std;

class UserNode{
public:
    string un;  // 用户节点的名称
    int dBW; // 用户节点当前所需带宽
    int dBWList[8930]; // 用户在各时刻的带宽需求数组
    UserNode(){};
    UserNode(string un);
    void init(string un); // 设置用户节点名称的un
    void set_dBW_at_t(int ti, int dBW); // 设置用户节点在ti天的需求带宽为dBW
};

#endif