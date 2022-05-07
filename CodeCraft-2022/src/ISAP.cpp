//
// Created by devilcry on 2022/3/19.
//

#include "ISAP.h"
#include "algorithm"
#include "iostream"
#include "Solver.h"
using namespace std;

void ISAP::init()
{
    tot = 0;
    edge.clear();
    edge.resize(maxEdge);
    head.clear();
    head.resize(maxEdge, -1);
    gap.clear();
    gap.resize(maxEdge);
    dep.clear();
    dep.resize(maxEdge);
    pre.clear();
    pre.resize(maxEdge);
    cur.clear();
    cur.resize(maxEdge);
}

void ISAP::addEdge(int u, int v, int capacity, int flow=0)
{
    edge[tot].to = v;
    edge[tot].cap = capacity;
    edge[tot].next = head[u];
    edge[tot].flow = 0;
    head[u] = tot++;

    edge[tot].to = u;
    edge[tot].cap = 0;
    edge[tot].next = head[v];
    edge[tot].flow = 0;
    head[v] = tot++;
}

void ISAP::isap(int st, int en)
{
    int INF = 0x3f3f3f3f;
    for (int i = 0; i < head.size(); ++i) {
        cur[i] = head[i];
    }
    int u=st,ans=0;
    pre[u]=-1,gap[0]=maxNode;
    while(dep[st]<maxNode){
        if(u==en){
            int Min=INF;
            for(int i=pre[u];i!=-1;i=pre[edge[i^1].to])
                Min=min(Min,edge[i].cap-edge[i].flow);
            for(int i=pre[u];i!=-1;i=pre[edge[i^1].to])
                edge[i].flow+=Min,edge[i^1].flow-=Min;
            u=st,ans+=Min;
            continue;
        }
        bool flag=false;
        int v;
        for(int i=cur[u];i!=-1;i=edge[i].next){
            v=edge[i].to;
            if(edge[i].cap-edge[i].flow&&dep[v]+1==dep[u]){
                flag=true,cur[u]=pre[v]=i;
                break;
            }
        }
        if(flag){
            u=v;continue;
        }
        int Min=maxNode;
        for(int i=head[u];i!=-1;i=edge[i].next){
            if(edge[i].cap-edge[i].flow&&dep[edge[i].to]<Min)
                Min=dep[edge[i].to],cur[u]=i;
//            cout<<u-1<<" "<<edge[i].to<<"sid="<<edge[i].to-M-1<<" "<<endl;
        }

        gap[dep[u]]--;
        if(!gap[dep[u]]){
            maxflow = ans;
            return;
        }
        dep[u]=Min+1,gap[dep[u]]++;
        if(u!=st) u=edge[pre[u]^1].to;
    }
    maxflow = ans;
}

void ISAP::run()
{
    /**
     * superS       [0]
     * userNodeId   [1, M]
     * siteNodeId   [M+1, M+N]
     * superT       [M+N+1]
     */
    int superS = 0, superT = M+N+1;
    maxNode = N+M+2, maxEdge = 2 * (M*N+M+N);
    init();
    for (int uid = 0; uid < M; ++uid) {
        addEdge(superS, uid+1, userNode[uid].dBWList[Ti], userNode[uid].dBWList[Ti]);
    }
    for (int uid = 0; uid < M; ++uid) {
        for (int sid : qosEdge[uid]){
            addEdge(uid+1, M+1+sid, siteNode[sid].rBW);
        }
    }
    for (int sid = 0; sid < N; ++sid) {
        addEdge(M+1+sid, superT, siteNode[sid].rBW);
    }

    isap(superS, superT);
    if (maxflow != sumDBWAtTime[Ti])
        cout<<"ERROR"<<endl;
}

void ISAP::run_with_bound()
{
    int superS = 0, superT = M+N+1;
    maxNode = N+M+2, maxEdge = 2 * (M*N+M+N);
    init();
    for (int uid = 0; uid < M; ++uid) {
        addEdge(superS, uid+1, userNode[uid].dBWList[Ti], userNode[uid].dBWList[Ti]);
    }

    for (int uid = 0; uid < M; ++uid) {
        for (int sid : qosEdge[uid]){
            addEdge(uid+1, M+1+sid, siteNode[sid].tBW);
        }
    }

    for (int sid = 0; sid < N; ++sid) {
        addEdge(sid+M+1, superT, min(arg_bound[sid], siteNode[sid].tBW));
    }
    isap(superS, superT);

}

//void ISAP::assignBW()
//{
//    int sid, bw;
//    for (int uid = 0; uid < M; ++uid) {
//        for (int i = head[uid+1]; i != -1 ; i=edge[i].next) {
//            if (edge[i].flow > 0){
//                sid = edge[i].to - M - 1;
//                bw = edge[i].flow;
//                userNode[uid].dBW -= bw;
//                siteNode[sid].rBW -= bw;
//                siteNode[sid].uBWList[Ti] += bw;
//            }
//        }
//    }
//}

///    1     95   96 97 98 99 100