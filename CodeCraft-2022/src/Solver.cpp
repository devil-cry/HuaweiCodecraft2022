#include "Solver.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#define MIN(a,b) a<b?a:b
#define MAX(a,b) a>b?a:b

using namespace std;

string trimstr(string s){
    size_t n = s.find_last_not_of(" \r\n\t");
    if (n != string::npos){
        s.erase(n + 1, s.size() - n);
    }
    n = s.find_first_not_of(" \r\n\t");
    if (n != string::npos){
        s.erase(0, n);
    }
    return s;
}

void read_site_bandwidth_csv(string path){
    ifstream fin(path);
    string line;
    int row=0;
    getline(fin, line); // 跳过第一行
    while(getline(fin, line)){
        line = trimstr(line);
        istringstream sin(line);
        vector<string> fields;
        string field;
        while (getline(sin, field, ',')){
            fields.push_back(field);
        }
        siteNode[row].init(fields[0], atoi(fields[1].c_str()));
        sn2id[fields[0]] = row;
        row += 1;
    }
    N = row;
}
void read_demand_csv(string path){
    ifstream fin(path);
    string line;
    int row=0, col;
    while(getline(fin, line)){
        line = trimstr(line);
        istringstream sin(line);
        vector<string> fields;
        string field;
        while (getline(sin, field, ',')){
            fields.push_back(field);
        }
        if(row==0){
            col = 0;
            for(vector<string>::iterator it=fields.begin()+1;it!=fields.end();it++){
                userNode[col].init(*it);
                un2id[*it] = col;
                col += 1;
            }
            M = col;
        }else{
            col = 0;
            for(vector<string>::iterator it=fields.begin()+1;it!=fields.end();it++){
                userNode[col].set_dBW_at_t(row-1, atoi((*it).c_str()) );
                col += 1;
            }
        }
        row += 1;
    }
    T = row-1;
}
void read_qos_csv(string path){
    ifstream fin(path);
    string line;
    int row=0, col;
    int uidList[40];
    while(getline(fin, line)){
        line = trimstr(line);
        istringstream sin(line);
        vector<string> fields;
        string field;
        while (getline(sin, field, ',')){
            fields.push_back(field);
        }
        if(row==0){
            col = 0;
            for(vector<string>::iterator it=fields.begin()+1;it!=fields.end();it++){
                uidList[col] = un2id[*it];
                col += 1;
            }
        }else{
            col = 0;
            string sn = *fields.begin();
            for(vector<string>::iterator it=fields.begin()+1;it!=fields.end();it++){
                usQoS[uidList[col]][sn2id[sn]] = atoi((*it).c_str());
                col += 1;
            }
        }
        row += 1;
    }
}
void read_config_ini(string path){
    ifstream fin(path);
    string line;
    getline(fin, line); // 跳过第一行
    getline(fin, line);
    line = trimstr(line);
    istringstream sin(line);
    vector<string> fields;
    string field;
    while (getline(sin, field, '=')){
        fields.push_back(field);
    }
    maxQoS = atoi((*(fields.begin()+1)).c_str());
}

void get_qosEdge()
{
    for (int uid = 0; uid < M; ++uid) {
        for (int sid = 0; sid < N; ++sid) {
            if (usQoS[uid][sid] < maxQoS){
                qosEdge[uid].push_back(sid);
            }
        }
    }
}

void prepare_a_new_day(){
    for(int i=0;i<M;i++){
        userNode[i].dBW = userNode[i].dBWList[Ti]; // 初始化Ti天所有用户节点的带宽需求

        recorder[Ti][i].clear(); // 清空分配方案的记录
    }
    for(int i=0;i<N;i++){
        siteNode[i].rBW = siteNode[i].tBW; // 初始化当天该边缘节点的剩余带宽为其总带宽
        siteNode[i].uBWList[Ti] = 0; // 初始化当天该边缘节点的所使用带宽为0
    }
}

void get_sorted_uids(int *res){
    vector<pair<int,int> > DemandAndUID;
    for(int i=0;i<M;i++){
        DemandAndUID.push_back(make_pair(userNode[i].dBW,i));
    }
    sort(DemandAndUID.begin(), DemandAndUID.end());
    for(int i=0;i<M;i++) res[DemandAndUID[i].second] = M-1-i;
}

void recordMaxFlow()    //清零mxFlowList 并计算带宽分配
{
    int sid, bw;
//    for (int i = 0; i < N; ++i) {
//        siteNode[i].mxFlowList[Ti] = 0;
//    }
    for (int uid = 0; uid < M; ++uid) {
        for (int i = isapMaxFlow.head[uid+1]; i != -1 ; i=isapMaxFlow.edge[i].next) {
            bw = isapMaxFlow.edge[i].flow;
            if (bw>0){
                sid = isapMaxFlow.edge[i].to - M - 1;
//                siteNode[sid].mxFlowList[Ti] += bw;
                rec[Ti][uid][sid] = bw;
                siteNode[sid].uBWList[Ti] += bw;
            }
        }
    }
}

void recordUse(vector<int>& use, vector<int>& _arg_bound)
{
    for (int sid = 0; sid < N; ++sid) {
        for (int i = isapMaxFlow.head[sid+M+1]; i != -1 ; i=isapMaxFlow.edge[i].next) {
            int bw = isapMaxFlow.edge[i].flow;
            if (bw > _arg_bound[sid]){
                //cerr << bw << " " << arg_bound[sid] << endl;
                ++use[sid];
//                siteNode[sid].historyBW.push_back(bw);
            }
        }
    }
}

//void updateSuperNode()
//{
//    vector<bool> isFullAssigned(M, false);
//    vector<int> sumFlowofSite(N, 0);
//    vector<bool> flag(N, false);
//    for (int uid = 0; uid < M; ++uid) {
//        int sumAssignedBW = 0;
//        for (int i = isapMaxFlow.head[uid+1], sid, bw; i != -1; i=isapMaxFlow.edge[i].next) {
//            bw = isapMaxFlow.edge[i].flow;
//            if (bw > 0){
//                sid = isapMaxFlow.edge[i].to - M - 1;
//                sumAssignedBW += isapMaxFlow.edge[i].flow;
//                sumFlowofSite[sid] += isapMaxFlow.edge[i].flow;
//            }
//        }
//        if (sumAssignedBW < userNode[uid].dBWList[Ti]){
//            for (int i = isapMaxFlow.head[uid+1], sid, bw; i != -1; i=isapMaxFlow.edge[i].next) {
//                bw = isapMaxFlow.edge[i].flow;
//                if (bw > 0){
//                    sid = isapMaxFlow.edge[i].to - M - 1;
//                    flag[sid] = true;
//                }
//            }
//        }
//    }
//    for (int sid = 0; sid < N; ++sid) {
//        if (flag[sid])
//            oksid[sid] = false;
//        if (flag[sid] && ! siteNode[sid].isSuperNodeAtTime[Ti]){
//            if (siteNode[sid].remainSuperChance == 0){
//                mn_bound[sid] = max(mn_bound[sid], arg_bound[sid]);
////                arg_bound[sid] = arg_bound[sid] * 10 / 9 * 95 / 100;
//            } else {
//                siteNode[sid].isSuperNodeAtTime[Ti] = true;
//                siteNode[sid].remainSuperChance-- ;
//            }
//        }
//    }
//}

//void identifySuperNode()
//{
//    int mxSuperNodeTime = T * 5 / 100;
//    vector<pair<int,int> > demandAndTime(T);
//    for (int sid = 0; sid < N; ++sid) {
//
//        for (int i = 0; i < T; ++i) {
//            demandAndTime[i]=make_pair(siteNode[sid].mxFlowList[i],i);
//            siteNode[sid].orderBWAtIndex.push_back(siteNode[sid].mxFlowList[i]);
//        }
//        sort(siteNode[sid].orderBWAtIndex.begin(), siteNode[sid].orderBWAtIndex.end());
//        sort(demandAndTime.begin(), demandAndTime.end());
//        for (int i = 0; i < mxSuperNodeTime; ++i) {
//            siteNode[sid].isSuperNodeAtTime[demandAndTime[T-1-i].second] = true;
//        }
//        for (int i = 0; i < T; ++i) {
//            siteNode[sid].bwIndexAtTime[demandAndTime[i].second] = i;
//        }
//    }
//}

inline void arrange_bw(int uid, int sid, int bw)
{
    userNode[uid].dBW -= bw;
    siteNode[sid].rBW -= bw;
    siteNode[sid].uBWList[Ti] += bw;
}

void arrange_site_to_user(int day)
{
    int bw;
    for (int uid=0; uid<M; uid++) {
        recorder[day][uid].clear();
        for (int sid : qosEdge[uid]) {
            bw = rec[day][uid][sid];
            if (bw > 0){
//                arrange_bw(uid, sid, bw);
                recorder[day][uid].push_back(pair<string, int>(siteNode[sid].sn, bw));
            }
        }
    }
}

void print_arrange_result(FILE *fp){
    for(int uid=0;uid<M;uid++){
        fprintf(fp, "%s:", userNode[uid].un.c_str());
        // printf("%s:", siteNode[uid].sn.c_str());
        bool isFirst = true;
        for(vector<pair<string, int>>::iterator it=recorder[Ti][uid].begin(); it!=recorder[Ti][uid].end(); it++){
            if(isFirst) isFirst = false;
            else fprintf(fp, ",");
            fprintf(fp, "<%s,%d>", it->first.c_str(), it->second);
            // printf("<%s,%d>", it->first.c_str(), it->second);
        }
        fprintf(fp, "\n");
        // printf("\n");
    }
}

int get_total_cost(){
    int i95 = T*5/100, cost = 0;
    vector<int> v;
    for(int sid=0;sid<N;sid++){
        v.clear();
        for (int i = 0; i < T; ++i) {
            v.push_back(siteNode[sid].uBWList[i]);
        }

        sort(v.begin(),v.end());
//#define DEBUG
#ifdef DEBUG
        cerr << "site " << sid << " : " << v[T-1-i95] << endl;
        for (int i = 0; i < T; ++i) {
            cerr << v[i] << " ";
        }
        cerr << endl;
#endif
        cost += v[T-1-i95];
    }
    return cost;
}