#ifndef SOLVER_H
#define SOLVER_H

#include "UserNode.h"
#include "SiteNode.h"
#include "ISAP.h"
#include <map>
#include <vector>

extern int N, M; // N 表示总边缘节点的数量； M 表示总用户节点的数量；
extern int maxQoS; // QoS的约束上限
extern int T, Ti; // 总时间节点数量和当前时间节点。
extern int maxSuperNode;

extern UserNode userNode[40]; // 存储所有的用户节点
extern SiteNode siteNode[140]; // 存储所有的边缘节点
extern map<string, int>un2id,sn2id; // 存储用户节点名称和边缘节点名称与其id的映射
extern int usQoS[40][140]; // 存储用户节点到边缘节点的QoS
extern vector<int> qosEdge[40];
extern vector<int> mn_bound, ok_bound, arg_bound;
extern vector<bool> oksid;

extern vector<pair<string,int> > recorder[8930][40]; // 记录每天各用户节点的分配情况 表示uid将recorder[uid].second带宽分配给recorder[uid].first边缘节点

extern ISAP isapMaxFlow;
extern int rec[8928][35][135];

// 以下为所有定义的解题中间方法
// 数据读取
void read_site_bandwidth_csv(string path);
void read_demand_csv(string path);
void read_qos_csv(string path);
void read_config_ini(string path);

void get_qosEdge();     //以vector方式存储uid满足maxQos约束的sid序列 节省时间开销
void prepare_a_new_day(); // 对当前Ti时间准备所有变量的初始值

void recordMaxFlow();  // 记录边缘节点T天最大流后可行流的带宽

void updateSuperNode();

void get_sorted_uids(int *res); // 分配边缘节点时对用户的优先级进行排序

inline void arrange_bw(int uid, int sid, int bw);

void arrange_site_to_user(int day); // 将边缘节点分配给uid用户，并记录分配方案至recorder

void print_arrange_result(FILE *fp); // 将当天的分配情况打印至文件fp

void recordUse(vector<int>& use, vector<int>& arg_bound);

int get_total_cost(); // 获取当前总成本
#endif