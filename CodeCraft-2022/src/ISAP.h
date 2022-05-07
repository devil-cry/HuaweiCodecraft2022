//
// Created by devilcry on 2022/3/19.
//

#ifndef ISAP_H
#define ISAP_H

#include "UserNode.h"
#include "SiteNode.h"
#include "vector"
#include "map"

extern int N, M; // N 表示总边缘节点的数量； M 表示总用户节点的数量；
extern int maxQoS; // QoS的约束上限
extern int T, Ti; // 总时间节点数量和当前时间节点。

extern UserNode userNode[40]; // 存储所有的用户节点
extern SiteNode siteNode[140]; // 存储所有的边缘节点
extern map<string, int>un2id,sn2id; // 存储用户节点名称和边缘节点名称与其id的映射
extern int usQoS[40][140]; // 存储用户节点到边缘节点的QoS
extern vector<int> qosEdge[40];
extern vector<int> mn_bound, ok_bound, arg_bound;

struct Edge{
    int to,next,cap,flow;
};

class ISAP {
public:
    std::vector<Edge> edge;
    std::vector<int> head, gap, dep, pre, cur;
    std::vector<int> sumDBWAtTime;
    int tot;
    int maxNode = N+M+2, maxEdge = 2 * (M*N+M+N);
    int maxflow;

    void addEdge(int u, int v, int capacity, int flow);
    void isap(int st, int en);
    void init();
    void run();
    void run_with_bound();
};


#endif //ISAP_H
