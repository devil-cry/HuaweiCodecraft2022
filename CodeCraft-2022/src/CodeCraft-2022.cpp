#include <bits/stdc++.h>

#include "Solver.h"
#include "ISAP.h"
using namespace std;


// 以下为所有定义的全局变量
int N, M; // N 表示总边缘节点的数量； M 表示总用户节点的数量；
int maxQoS; // QoS的约束上限
int T, Ti; // 总时间节点数量和当前时间节点。
int maxSuperNode;

UserNode userNode[40]; // 存储所有的用户节点
SiteNode siteNode[140]; // 存储所有的边缘节点
map<string, int>un2id,sn2id; // 存储用户节点名称和边缘节点名称与其id的映射
int usQoS[40][140]; // 存储用户节点到边缘节点的QoS
vector<int> qosEdge[40];
vector<int> arg_bound;

vector<pair<string,int> > recorder[8930][40]; // 记录每天各用户节点的分配情况 表示uid将recorder[uid].second带宽分配给recorder[uid].first边缘节点

int rec[8928][35][135];

ISAP isapMaxFlow;

int main()
{
//# define DEBUG
    // 读取数据
    string pre_in = "/data/", suf_in = ".csv", pre_out = "/output/";
# ifdef DEBUG
    pre_in = "../data/", suf_in = ".csv";
    pre_out = "../output/";
//    pre_in = "../sup_data/", suf_in = "_t6.csv";
# endif
    read_site_bandwidth_csv(pre_in+"site_bandwidth"+suf_in);
    read_demand_csv(pre_in+"demand"+suf_in);
    read_qos_csv(pre_in+"qos"+suf_in);
//    read_config_ini(pre_in + "config.ini");
    read_config_ini(pre_in+"config.ini");
    FILE *fp = fopen((pre_out + "solution.txt").c_str(), "w");


    get_qosEdge();

    for (Ti = 0; Ti < T; ++Ti) {
        prepare_a_new_day();

        isapMaxFlow.sumDBWAtTime.push_back(0);
        for (int uid = 0; uid < M; ++uid) {
            isapMaxFlow.sumDBWAtTime[Ti] += userNode[uid].dBW;
        }
        //isapMaxFlow.run();
        //recordMaxFlow();
    }

    maxSuperNode = T * 5 /100;

//    vector<pair<int,int> > sortedTi;
//    for (int i = 0,sum; i < T; ++i) {
//        sum = 0;
//        for (int uid = 0; uid < M; ++uid) {
//            sum += userNode[uid].dBWList[i];
//        }
//        sortedTi.push_back(make_pair(sum, i));
//    }
//    sort(sortedTi.begin(), sortedTi.end());

    const int score = 3475000;
//    const int best_score = 3475000;
    const int addBound = 50;

    int sum_tbw = 0;
    for (int i = 0; i < N; ++i){
        sum_tbw += siteNode[i].tBW;
    }

    for (int i = 0; i < N; ++i) {
        arg_bound.push_back(int(ceil(1.0 * score * siteNode[i].tBW / sum_tbw)));
//        arg_bound.push_back(int(ceil(1.0 * score * weight[i][T-1-maxSuperNode] / sum_tbw)));
    }


    vector<int> _arg_bound(arg_bound);

    auto func = [&](){
        vector<int> seq(N), use(N);
        iota(seq.begin(), seq.end(), 0);
        for(Ti=0; Ti<T; Ti++){

            arg_bound = _arg_bound;

            isapMaxFlow.run_with_bound();
            sort(seq.begin(), seq.end(), [&](int a, int b){
                return use[a] < use[b];
            });
            int cnt = 1;
            while (isapMaxFlow.maxflow != isapMaxFlow.sumDBWAtTime[Ti]) {
                arg_bound = _arg_bound;
                for (int i = 0; i < cnt; ++i) {
                    arg_bound[seq[i]] = siteNode[seq[i]].tBW;
                }
                isapMaxFlow.run_with_bound();
                ++cnt;
            }

//            recordUse();

            vector<bool> successUsedSid(N, false);

            bool ok = true;
            for (int sid = 0; sid < N; ++sid) {
                for (int i = isapMaxFlow.head[sid+M+1]; i != -1 ; i=isapMaxFlow.edge[i].next) {
                    int bw = isapMaxFlow.edge[i].flow;
                    if (bw > _arg_bound[sid]){
                        if (use[sid] < maxSuperNode){
                            ++use[sid];
                            successUsedSid[sid] = true;
                        }else{
                            ok = false;
                        }
                    }
                }
            }

            if (!ok){
//                cerr << "!ok" <<endl;
                arg_bound = _arg_bound;
                for (int sid = 0; sid < N; ++sid) {     //先把当天已经占用超级节点名额的节点不设上界
                    if (successUsedSid[sid])
                        arg_bound[sid] = siteNode[sid].tBW;
                }
                isapMaxFlow.run_with_bound();


                while (isapMaxFlow.maxflow != isapMaxFlow.sumDBWAtTime[Ti]){

                    vector<bool> isDangerSid(N, false);


                    for (int uid = 0, sumAssignedBW; uid < M; ++uid) {     //对每个与需求还未满足的用户所连的边缘节点
                        sumAssignedBW = 0;
                        for (int i=isapMaxFlow.head[uid+1],bw; i!=-1; i=isapMaxFlow.edge[i].next) {
                            bw = isapMaxFlow.edge[i].flow;
                            if (bw > 0){
                                sumAssignedBW += bw;
                            }
                        }
                        if (sumAssignedBW < userNode[uid].dBWList[Ti]){
                            int bw = userNode[uid].dBWList[Ti] - sumAssignedBW;
                            int ok_len = 0;     //用户节点连接的非超级节点个数
                            for (int sid : qosEdge[uid]) {
                                if (!successUsedSid[sid] && !isDangerSid[sid]){
                                    ok_len ++;
                                }
                            }
                            vector<pair<int,int> > vv;
                            for (int sid : qosEdge[uid]) {      //当天不是超级节点 未满足总需求的用户所连节点平均提升带宽上限
                                if (!successUsedSid[sid]){
                                    vv.push_back(make_pair(_arg_bound[sid], sid));
//                                    _arg_bound[sid] = min(_arg_bound[sid] + bw / ok_len + 1, siteNode[sid].tBW);
                                }
                            }
                            while (bw > 0){
                                sort(vv.begin(),vv.end());
                                int sid = vv[0].second, p=0;
                                for (int i = 0; i < vv.size(); ++i) {
                                    if (vv[i].first < siteNode[vv[i].second].tBW){
                                        sid = vv[i].second;
                                        p = i;
                                        break;
                                    }
                                }
                                int tbw = min(siteNode[sid].tBW - _arg_bound[sid], min(bw, 50));
                                _arg_bound[sid] += tbw;
                                vv[p].first += tbw;
                                bw -= tbw;
                            }

                        }
                    }

                    arg_bound = _arg_bound;
                    for (int sid = 0; sid < N; ++sid) {     //先把当天已经占用超级节点名额的节点不设上界
                        if (successUsedSid[sid])
                            arg_bound[sid] = siteNode[sid].tBW;
                    }
                    isapMaxFlow.run_with_bound();
                }
//                cerr << "ok ed!" << endl;
            }


            recordMaxFlow();

            arrange_site_to_user(Ti);
        }
    };

    func();

//    for (int sid = 0; sid < N; ++sid) {
//        siteNode[sid].updataHistory();
//    }
//
//    int dangerBoundIndex = T * 85 / 100;
//    for (Ti = 0; Ti < T; ++Ti) {
//        for (int uid = 0; uid < M; ++uid) {
//            vector<int> danSid;
//            for (int sid : qosEdge[uid]) {
//                if (rec[Ti][uid][sid] >= siteNode[sid].historyBW[dangerBoundIndex]){
//                    danSid.push_back(sid);
//                }
//            }
//        }
//    }

    for (Ti = 0; Ti < T; ++Ti) {
        print_arrange_result(fp);
    }


    printf("Total Cost: %d\n", get_total_cost());

    return 0;
}