#include "deploy.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <stack>
#include <map>
#include <queue>
#include <deque>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>
#include <functional>
#include <time.h>
#include <stdio.h>
#include <numeric>
#include <random>
using namespace std;

#define N 100010
#define INF 100000000

default_random_engine e;//引擎
normal_distribution<double> normal_distr(0,1);//均值方差


struct Edge
{//定义边的结构体
    int from,to,cap,flow,cost;
    Edge(int u,int v,int ca,int f,int co):from(u),to(v),cap(ca),flow(f),cost(co){};//初始化结构体
};

class MCMF{
    public:
        int n,m,s,t;//定义顶点的个数，边的个数，源点，汇点
        vector<Edge> edges;//定义边的vector数组用于存储边
        vector<int> G[N];//定义顶点的vector数组用于存储顶点数组
        int inq[N];//是否在队列中
        int d[N];//距离
        int p[N];//上一条弧
        int a[N];//可改进量
        int flow=0,cost=0;//存储最终的最小费用与最大流
        int demand_flow=0;//消费者需要提供的流量


        stack<int> road_stack;//存储路径输出的栈
        stack<int> edge_stack;//存储路径中的边，方便更改边的反向流量
        int visited[10010];//访问标记数组
        int path_flow;//缓存每条路径最后流向汇点的流量
        int flow_sum=0;
        int road_cnt=0;//路径计数器
        int flag=0;
        int graph_node_num;//存储图中的网络节点个数
        string out_str;//存储最后的输出结果
        vector<int> out_degree;//用来存储每个节点的出度
        vector<int> server_adress;//用来存储服务器节点候选地址
        vector<int> net_to_consume;//用来存储链接消费节点的网络节点地址

        vector<int> particle;//粒子个体，采用二进制编码

        void init(int my_n,int my_m,int my_s,int my_t, int my_graph_node_num)
        {
            n=my_n;m=my_m;s=my_s;t=my_t;graph_node_num=my_graph_node_num;//初始化参数
            for(int i=0;i<n;i++) G[i].clear();//初始化顶点数组
            edges.clear();//初始化边数组
        }

        //成员函数定义
        void addedge(int from,int to,int cap,int cost)
        {   //加边
            edges.push_back(Edge(from,to,cap,0,cost));//加边from,to,cap,cost其中flow为0
            edges.push_back(Edge(to,from,0,0,-cost));//加逆变to,from,-cost,其中cap,flow均为0
            int m = edges.size();//获取边的个数
            G[from].push_back(m-2);//G[i]中存储由i节点出发的边
            G[to].push_back(m-1);
        }

        bool SPFA(int s,int t,int &flow,int &cost)
        {//寻找最小费用的增广路，使用引用同时修改原flow,cost
            for(int i=0;i<n;i++)
                d[i]=INF;
            memset(inq,0,sizeof(inq));//初始化队列
            d[s]=0;inq[s]=1;p[s]=0;a[s]=INF;
            queue<int> Q;
            Q.push(s);
            while(!Q.empty())
            {
                int u=Q.front();//取队首元素
                Q.pop();//出队
                inq[u]--;//出队，标记u不在队列中
                for(int i=0;i<G[u].size();i++)
                {//遍历由u点出发的所有边
                    Edge& e=edges[G[u][i]];//通过定点u和标记i取出边e
                    if(e.cap>e.flow && d[e.to]>d[u]+e.cost)//满足可增广且可变短
                    {
                        d[e.to]=d[u]+e.cost;//松弛操作有效
                        p[e.to]=G[u][i];//保存G[u][i]为e.to的上一条弧
                        a[e.to]=min(a[u],e.cap-e.flow);
                        if(!inq[e.to])
                        {
                            inq[e.to]++;
                            Q.push(e.to);
                        }
                    }
                }
            }
            if(d[t]==INF) return false;//汇点不可达则退出
            flow+=a[t];
            cost+=d[t]*a[t];
            int u=t;
            while(u!=s)//更新正向边和反向边
            {
                edges[p[u]].flow+=a[t];
                edges[p[u]^1].flow-=a[t];
                u=edges[p[u]].from;
            }
            return true;
        }

        void MincotMaxflow(int s,int t){
            while(SPFA(s,t,flow,cost));
        }

        void print_path()
        {//dfs找到一条路就把它输出出来，然后修改路径中的流量
            stack<int> my_edge_stack = edge_stack;
            path_flow = INF;
            vector<int> road;//存储路径的数组
            out_str = out_str + "\n";

            while(!road_stack.empty()){
                int i = road_stack.top();
                road.push_back(i);
                //printf("%d ",road_stack.top());
                road_stack.pop();
            }
            //printf("\n");
            while(!edge_stack.empty()){
                int i = edge_stack.top();
                Edge& e = edges[i];
                //printf("%d %d %d %d %d \n",e.from,e.to, e.cap, e.flow, e.cost);
                path_flow = (e.flow < path_flow ? e.flow:path_flow);
                edge_stack.pop();
            }

            while(!my_edge_stack.empty()){
                int i = my_edge_stack.top();
                Edge& e = edges[i];
                e.flow -= path_flow;
                my_edge_stack.pop();
            }
            road_cnt++;
            flow_sum += path_flow;
            //printf("找到第%d条路啦！路的流量为%d,累计提供流量为:%d\n",road_cnt,path_flow,flow_sum);
            //printf("正确输出格式：");
            for(int i=1;i<(road.size()-1);i++){
                if(i == (road.size()-2)) road[i] -= graph_node_num;
                //printf("%d ", road[i]);
                out_str = out_str+to_string(road[i])+" ";
            }
            //printf("%d\n", path_flow);
            out_str = out_str + to_string(path_flow);
            //printf("\n");
            
        }

        int dfs_find_road(int u)
        {//dfs找路，找到就返回
            if(u == t){print_path();flag=1;return 1;}
            else{
                for(int i=0;i<G[u].size();i++){//遍历由u出发的所有边
                    Edge& e = edges[G[u][i]];//取边
                    if(!visited[e.to] && e.flow > 0){
                        visited[e.to]=1;
                        road_stack.push(e.to);
                        edge_stack.push(G[u][i]);
                        //printf("%d %d %d %d %d \n",e.from,e.to, e.cap, e.flow, e.cost);
                        dfs_find_road(e.to);
                        if(flag){return 1;}
                        edge_stack.pop();
                        road_stack.pop();
                        visited[e.to]=0;
                    }

                }

            }
            return 0;

        }

        void get_out()
        {//已知终点和开始节点开始找
            //printf("开始路径输出：\n");
            memset(visited,0,sizeof(visited));
            visited[s]=1;
            road_stack.push(s);
            while(dfs_find_road(s)){
                memset(visited,0,sizeof(visited));
                visited[s]=1;
                road_stack.push(s);
            }
        }

        void create_server_edges()
        {//参照粒子个体二进制编码给出的服务器部署方案建图
            for(int i=0; i<particle.size();i++){
                if(particle[i] == 1) server_adress.push_back(i);
            }
            for(int i = 0; i< server_adress.size();i++){
                //printf("根据服务器部建边：%d %d %d %d\n",server_adress[i],t,INF,0);
                addedge(server_adress[i],t,INF,0);
            }
        }

};



vector<int> G0[N];//不建服务器边的原始图
vector<Edge> edges0;//保存边的信息
vector<int>  net_to_consume;//与消费节点相连的网络节点

int demand_flow=0;//消费节点总的带宽需求

void addedge(int from,int to,int cap,int cost)
{   //加边
    edges0.push_back(Edge(from,to,cap,0,cost));//加边from,to,cap,cost其中flow为0
    edges0.push_back(Edge(to,from,0,0,-cost));//加逆变to,from,-cost,其中cap,flow均为0
    int m = edges0.size();//获取边的个数
    G0[from].push_back(m-2);//G[i]中存储由i节点出发的边
    G0[to].push_back(m-1);
}

double sigmod(double x){//sigmod函数
    //为防止Sigmoid函数饱和，可以将粒子的速度限定在一定的范围内[-Vmax,Vmax]

    if(x <= 0 ) return (1 - 2/(1+exp(-x)));
    else return (2/(1+exp(-x)) -1);

}

//你要完成的功能入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename){
    int graph_node_num, graph_edge_num, consume_node_num;//节点的个数，边的个数，消费节点的个数
    int server_price;//每台服务器的价格
    char seg[]=" ";
    //读入数据开始建图
    int space_line_count = 0;//用于计算空格行
    int j;//用于控制文件行输入以及循环值的控制
    int start_node,end_node,total_bandwidth,cost_per_bandwidth;//用于暂存每行的网络图节点与边的信息
    int consume_node,net_node,demand_bandwidth;//由于暂存消费节点与网路图节点的链接信息以及消费需求带宽
    double start_time,finish_time;//存储程序开始时间与结束时间
    int server_num = 7;//用于保存服务器的个数，建图的时候统计边需要用到
    int last_cost;//存储最后所需要花费的费用
    int n,m,s,t;

    for (int i=0;i<line_num;i++){
        if(strlen(topo[i]) == 2) {
            space_line_count++;
            continue;
        }

        switch(space_line_count){

            case 0 :{
                char *substr = strtok(topo[i],seg);
                j = 0;
                while(substr != NULL){
                //strcpy(charlist[j],substr);
                if(j == 0) graph_node_num = atoi(substr);
                if(j == 1) graph_edge_num = atoi(substr);
                if(j == 2) consume_node_num = atoi(substr);
                j++;
                //printf("%s\n",substr);
                substr = strtok(NULL,seg);
                    }

                cout << "统计网络节点个数:"<< graph_node_num <<"，边的个数:"<< graph_edge_num <<"，消费节点的个数:" << consume_node_num << endl;
                //建立超级源点和超级汇点
                s = graph_node_num+consume_node_num;
                t = graph_node_num+consume_node_num+1;
                //初始化mcmf的节点个数以及边的个数
                n = graph_node_num+consume_node_num+2;//节点个数=网络节点总数+消费节点总数+源点1+汇点1
                m = graph_edge_num+2*consume_node_num+server_num;//边的个数=网络边的总数+消费节点建边个数+服务器建边个数
                break;
            }
            case 1 :{
                char *substr = strtok(topo[i],seg);
                j = 0;
                while(substr != NULL){
                //strcpy(charlist[j],substr);
                if(j == 0) server_price = atoi(substr);
                j++;
                //printf("%s\n",substr);
                substr = strtok(NULL,seg);
                    }
                cout << "统计服务器的费用:" << server_price << endl;
                break;

            }
            case 2 :{
                char *substr = strtok(topo[i],seg);
                j = 0;
                while(substr != NULL){
                //strcpy(charlist[j],substr);
                if(j == 0) start_node = atoi(substr);
                if(j == 1) end_node = atoi(substr);
                if(j == 2) total_bandwidth = atoi(substr);
                if(j == 3) cost_per_bandwidth = atoi(substr);
                j++;
                //printf("%s\n",substr);
                substr = strtok(NULL,seg);
                    }
                //cout << "读取边的带宽以及费用信息，并建图" << start_node << "," << end_node << "," << total_bandwidth << "," <<cost_per_bandwidth<< endl;
                addedge(start_node, end_node, total_bandwidth, cost_per_bandwidth);
                addedge(end_node, start_node, total_bandwidth, cost_per_bandwidth);
                break;
            }
            case 3 :{
                //对于消费节点与所连网络节点间建立带宽为消费需求带宽，费用为0的边
                //对于超级源点S与每个消费节点之间连理带宽为消费需求，费用为0的边
                //消费节点的编号为实际编号+网络节点的总数（id+graph_node_num）

                char *substr = strtok(topo[i],seg);
                j = 0;
                while(substr != NULL){
                //strcpy(charlist[j],substr);
                if(j == 0) consume_node = atoi(substr);
                if(j == 1) net_node = atoi(substr);
                if(j == 2) demand_bandwidth = atoi(substr);
                j++;
                //printf("%s\n",substr);
                substr = strtok(NULL,seg);
                    }

                //cout << "统计消费节点与网络节点的链接信息以及消费需求带宽，并建图" << consume_node+graph_node_num << "," << net_node << ","<< demand_bandwidth << endl;
                //建立带宽为消费需求带宽，费用为0的边。

                addedge((consume_node+graph_node_num), net_node, demand_bandwidth, 0);

                // 建边S and consume_node

                addedge(s,consume_node+graph_node_num, INF, 0);

                demand_flow += demand_bandwidth;
                net_to_consume.push_back(net_node);

                break;
            }
            case 4 : break;
            default: cout << "出错啦！" << endl;
            
        }
    }//NED FOR

    /**
    以上读文件，建立了基本的图，网络节点与网络节点按要求建边，消费节点与网络节点建立带宽为消费需求带宽，费用为0的边
    以下开始按照单源单汇算法建立符合算法要求的图（最大流+spfa）

    假想超级源点（S）： graph_node_num+consume_node_num
    假想超级汇点（T）： graph_node_num+consume_node_num+1

    建边：

    （建边S and consume_node）将超级源点S与所有消费节点之间建立带宽为消费需求带宽，费用为0的边
    将超级汇点T与所有服务器节点之间建立带宽为无穷大，费用为0的边

    (建边T and server_node) 然后对S->T采用最小费用流算法，计算出给定流量下最小的费用
    **/



    cout <<endl<< "超级源点S:" << s << "，超级汇点T:" << t << "，消费节点总的带宽需求："<<demand_flow<<endl<<endl;

    //MCMF new_mcmf(mcmf);


    //建边T and server_node 
    //case0 最佳的服务器选址为：7, 13, 15, 22, 37, 38, 43,先手工建个边试试
    //case4
    /*
    mcmf.addedge(12,mcmf.t,INF,0);
    mcmf.addedge(15,mcmf.t,INF,0);
    mcmf.addedge(20,mcmf.t,INF,0);
    mcmf.addedge(22,mcmf.t,INF,0);
    mcmf.addedge(26,mcmf.t,INF,0);
    mcmf.addedge(37,mcmf.t,INF,0);
    mcmf.addedge(48,mcmf.t,INF,0);*/
    
    /**关于随机数
    srand((unsigned)time(0)) 设定产生随机数的种子
    rand()/(RAND_MAX+1.0) 生成0-1之之间的数
    **/
   
    //=================================开始遗传算法==========================================//
    //设置参数miu,0<delta1<1,0<delta2<1,0<pc0<pc1<pc2,0<pm0<pm1<pm2,alpha,beta,T,N,Tmax
    #define POP_SIZE 4 //种群规模
    #define MAX_ITER 500 //最大进化代数

    //double miu=1.5,delta1=0.05,delta2=0.05;//用于产生较好的初始化种群
    double pcmax=1.0,pcmin=0.5,pc = 1.0;
    //double pm0=0.0001,pm1=0.001,pm2=0.1;
    //double alpha=0.78,beta=0.0005;

    double pm=0.3;//设定交叉和变异遗传的概率

    int optimal_repeat_cnt=0;//重复最优解的次数
    int generate_cnt=0;//遗传算法迭代次数

    vector<int> POP[POP_SIZE];//父代种群
    vector<int> NEXT_POP[POP_SIZE];//子代种群


    int total_cost[POP_SIZE];//当前种群所有个体的费用
    int g_best_id;//适应度最好个体的id
    int g_best_value=INF;//保存种群历史最优解的值
    vector<int> g_best;//种群历史最优解



    //计算v,生成初始种群
    srand((unsigned)time(0));

    for(int i=0;i<POP_SIZE;i++){

        int cnt=3;//每次随机产生3个序列，选取最好的那个序列，留下
        double direction_c=0.0;//当前解，建设服务器费用与线路费用的比例，提供初始化种群的方向
        int curren_cost=INF;//当前解的花费

        while(cnt){
            //随机产生个体
            vector<int> Gen;//随机产生的基因序列
            //cout<<"候选基因序列：";
            for(int j=0;j<graph_node_num;j++){//随机产生基因序列
                if(rand()/(RAND_MAX+1.0) < 0.3) Gen.push_back(1);
                else Gen.push_back(0);
                //Gen.push_back(rand()%2);//随机产生实数0或1
                //cout << Gen[j];
            }
            //cout<<endl;
            cnt--;

            //计算随机基因序列表征解的费用值
            int temp_flow = 0;
            for(int j=0;j<graph_node_num;j++){//遍历虽有的网络节点
                if(Gen[j] == 1){//如果该节点是服务器，则计算出度
                    for(int k=0;k<G0[j].size();k++){
                        Edge& e = edges0[G0[j][k]];//取边
                        temp_flow += e.cap;//统计服务器可提供的最大流量
                    }
                }
            }
            //printf("当前个体出度为：%d\n",temp_flow );
            if(temp_flow < demand_flow){//流量不足，无需计算
                printf("当前个体出度不够，舍去！\n");
                continue;
            }

            //运行最小费用流算法，计算当前个体最小费用，从而计算适应度
            MCMF* mcmf = new MCMF();
            mcmf->init(n,m,s,t,graph_node_num);
            //复制图--------------------------------------------------//
            for(int j=0; j<edges0.size(); j++){
                mcmf->edges.push_back(edges0[j]);
            }
            for(int j=0;j< graph_node_num+consume_node_num+1;j++){
                for(int k=0;k<G0[j].size();k++){
                    mcmf->G[j].push_back(G0[j][k]);
                }
            }
            //复制图结束---------------------------------------------//

            //给mcmf中的个体赋值
            for(int j=0; j< graph_node_num;j++){
                mcmf->particle.push_back(Gen[j]);
            }
            //根据个体表征的解给服务器与汇点建边
            mcmf->create_server_edges();
            //运行最小费用最大流
            mcmf->MincotMaxflow(mcmf->s,mcmf->t);
            last_cost = mcmf->server_adress.size()*server_price + mcmf->cost;

            //比较当前费用，留下较优的，放进初始种群
            if(last_cost < curren_cost){//如果当期那基因序列较优
                printf("该候选基因较优！%d\n",cnt);
                curren_cost = last_cost;//更新当前花费
                POP[i].clear();//初始化种群当前个体基因序列
                for(int j=0;j<graph_node_num;j++){
                    POP[i].push_back(Gen[j]);//将当前基因序列放进初始种群
                    NEXT_POP[i].push_back(POP[i][j]);//初始化子代种群
                }
            }
            direction_c = (double)(mcmf->server_adress.size()*server_price)/mcmf->cost;//求direction_c参数
            if(mcmf->flow >= demand_flow && last_cost < total_cost[g_best_id]) g_best_id = i;
            printf("当前随机个体%i,可提供流量:%d,当前总的费用：%d,direction_c：%f\n",i,mcmf->flow,last_cost,direction_c);
            delete mcmf;//回收内存

        }

        /*
        if(direction_c >= miu){//direction_c较大则说明，当前建设费用过高，应该以一定的概率减小服务器个数
            for(int j=0;j<graph_node_num;j++){
                if(rand()/(RAND_MAX+1.0) < delta1){
                    if(POP[i][j] == 1) POP[i][j]=0;
                } 
            }
        }
        else {//否则则说明链接费用过高，以一定的比例增加1
            for(int j=0;j<graph_node_num;j++){
                if(rand()/(RAND_MAX+1.0) < delta2){
                    if(POP[i][j] == 0) POP[i][j]=1;
                }
            }

        }*/

    }


    //关于初始解的一些探索，首先90s内通过机器找到最优，恐怕是很难的了，图太大了这本身就是个NP难问题
    //既然是比赛，那就有些比赛的技巧
    //比赛的规则是，谁在指定时间内找出的花费最小就得分高，粗略的估计，出度越大，选为服务器时，单位流量内的设施建设费用就越小
    //因此可以先人工构造一个次优解，或者构造一些覆盖范围较大的解

    //计算所有点的出度
    vector<int> flow_out;//存储虽有网络节点的出度
    for(int i=0;i<graph_node_num;i++){
        flow_out.push_back(0);//初始化第i个节点的出度为0
        for(int j=0;j<G0[i].size();j++){
            Edge& e= edges0[G0[i][j]];//取边
            flow_out[i] += e.cap;
        }
    }
    //将节点的编号按照出度大小排序
    vector<int> sorted_flow_out;//排完序的数组
    for(int i=0;i<graph_node_num;i++){
        sorted_flow_out.push_back(i);//初始化节点编号排序数组
    }
    for(int i=0;i<graph_node_num;i++){
        for(int j=i+1;j<graph_node_num;j++){
            if(flow_out[sorted_flow_out[j]] > flow_out[sorted_flow_out[i]]){//交换,每次保证当前位置的出度最大
                int temp = sorted_flow_out[i];
                sorted_flow_out[i] = sorted_flow_out[j];
                sorted_flow_out[j] = temp;
            } 
        }
    }

    //输出出度数组
    /*
    for(int i=0;i<graph_node_num;i++){
        printf("(节点编号：%d出度：%d)\n",sorted_flow_out[i],flow_out[sorted_flow_out[i]]);
    }
    cout<<endl;*/

        for(int i=0;i<graph_node_num;i++){
        POP[0][i]=0;
        POP[1][i]=0;
    }

    //方法1：设定，初始的全局最优为服务器直连，保证始终都能找到解

    for(int i=0;i<net_to_consume.size();i++){//尽量保证该个体的基因能留在种群中
        POP[0][net_to_consume[i]]=1;
        POP[1][net_to_consume[i]]=1;

    }
    
    /*
    //方法2：人为的设定一些出度较大的点加入进去
    for(int i=0;i<(graph_node_num/4);i++){
        int cnt = 20;
        int curren_flow_out = 0;
        int k_to_s;//选定部署服务器的位置

        while(cnt){//连续产生五个网络节点，选择出度最大的那个节点不布置服务器
            int k = rand()%graph_node_num;
            if(flow_out[k] > curren_flow_out) k_to_s=k;
            cnt--;
        }
        POP[2][k_to_s]=1;//将选中的点，部署服务器

    }
    //方法3：人为额设定一些出度较小的点加进去
    for(int i=0;i<(graph_node_num/4);i++){
        int cnt = 20;
        int curren_flow_out = INF;
        int k_to_s;//选定部署服务器的位置

        while(cnt){//连续产生五个网络节点，选择出度最大的那个节点不布置服务器
            int k = rand()%graph_node_num;
            if(flow_out[k] < curren_flow_out) k_to_s=k;
            cnt--;
        }
        POP[3][k_to_s]=1;//将选中的点，部署服务器

    }*/


    start_time = clock();
    finish_time= clock();

    //while 循环体表示开始进化
    while(((finish_time - start_time)/CLOCKS_PER_SEC < 80) && generate_cnt < MAX_ITER){//每一次循环进行一次种群的更新并计算适应度

        printf("第%d代原种群：\n",generate_cnt+1);

        for(int i=0;i<POP_SIZE;i++){//将费用数组初始化为INF
            total_cost[i]=INF;
        }
        g_best_id = 0;//初始化当前种群最好的个体

        //对种群的每个个体计算其适应度（花费越小适应度越大）

        for(int i=0;i<POP_SIZE;i++){//遍历种群中的每一个个体求其适应度
            //先计算其出度，如不满足消费需求则，不用求费用流,费用直接为INF

            int temp_flow = 0;
            for(int j=0;j<graph_node_num;j++){//遍历虽有的网络节点
                if(POP[i][j] == 1) temp_flow += flow_out[j];
            }
            //printf("当前个体出度为：%d\n",temp_flow );
            if(temp_flow < demand_flow){//流量不足，无需计算
                printf("当前个体出度不够，舍去！\n");
                continue;
            }
            //运行最小费用流算法，计算当前个体最小费用，从而计算适应度
            MCMF* mcmf = new MCMF();
            mcmf->init(n,m,s,t,graph_node_num);
            //复制图--------------------------------------------------//
            for(int j=0; j<edges0.size(); j++){
                mcmf->edges.push_back(edges0[j]);
            }
            for(int j=0;j< graph_node_num+consume_node_num+1;j++){
                for(int k=0;k<G0[j].size();k++){
                    mcmf->G[j].push_back(G0[j][k]);
                }
            }
            //复制图结束---------------------------------------------//

            //给mcmf中的个体赋值
            for(int j=0; j< graph_node_num;j++){
                mcmf->particle.push_back(POP[i][j]);
            }
            //根据个体表征的解给服务器与汇点建边
            mcmf->create_server_edges();
            //运行最小费用最大流
            mcmf->MincotMaxflow(mcmf->s,mcmf->t);
            last_cost = mcmf->server_adress.size()*server_price + mcmf->cost;
            //计算当前最小费用，亦即适应度最好
            if(mcmf->flow >= demand_flow){
                total_cost[i] = last_cost;//保存当前个体的费用
                if(last_cost < total_cost[g_best_id]) g_best_id = i;
            }
            printf("个体%i,可提供流量:%d,当前总的费用：%d,种群最小费用：%d\n",i,mcmf->flow,last_cost,total_cost[g_best_id]);
            delete mcmf;//回收内存
        }//END-FOR


        for(int i=0;i<POP_SIZE;i++){//输出费用数组
            cout<<total_cost[i]<<",";
        }

        cout<<"当前费用数组输出完毕！"<<endl;
        cout<<"当前种群最好的位置是:"<<g_best_id<<",费用为："<<total_cost[g_best_id]<<"种群历史最优解："<<g_best_value<<endl;
        //如果当前种群最优解好于历史最优解，则保存解，并更新历史最优解的值
        int old_g_best_value = g_best_value;

        if(total_cost[g_best_id] < g_best_value){
            g_best.clear();
            g_best_value = total_cost[g_best_id];
            for(int j=0;j<graph_node_num;j++){
                g_best.push_back(POP[g_best_id][j]);
            }
        }
        if(g_best_value == old_g_best_value) optimal_repeat_cnt++;//如果历史最优解，长时间无法更新，则这种群可能陷入全局最或者是局部最优




        //执行选择算子SO,采用锦标赛规则进行选择出子代种群

        for(int i=0;i<POP_SIZE;i++){

            //每次随机挑出两个个体，留下花费较小的那个，加入到下一代种群
            int k1 = rand()%POP_SIZE;
            int k2 = rand()%POP_SIZE;
            int k;//最后胜出的选手
            if(total_cost[k1] < total_cost[k2]) k=k1;
            else k=k2;
            for(int j=0;j<graph_node_num;j++){
                NEXT_POP[i][j]=POP[k][j];//将选出的个体遗传到下一代种群中
            }
        }

        //执行交叉算子，以一定的概率pc,对种群中的个体进行交叉遗传
        //为简单起见，将1号个体和2号个体，3号个体和4号个体……依次类推，进行交叉遗传
        //交叉采用概率交叉与均匀交叉相结合

        //自适应pc,利用遗传的代数来动态选择,初始较大且下降缓慢，后期逐步减少一个常量，加快收敛速度
        if(pc > pcmin) pc = pcmax*cos((3.14/2)*(generate_cnt/MAX_ITER));
        else pc = pcmin;

        printf("当前交叉概率pc为:%f\n",pc);

        int cnt_one= (5*graph_node_num / 10) + 1;//定义交叉序列模板中1的个数，取概率交叉中1出现的概率为0.3
        vector<int> cross_sequence;//交叉序列
        for(int i=0;i<(POP_SIZE/2); i++){
            if(rand()/(RAND_MAX+1.0) < pc){//以交叉概率pc发生交叉
                //生成交叉序列
                cross_sequence.clear();//清空序列
                for(int j=0;j<graph_node_num;j++) cross_sequence.push_back(0);//初始化交叉队列为0
                for(int j=0;j<graph_node_num;j++){//生成交叉序列
                    for(int k_cnt=0;k_cnt < cnt_one;k_cnt++){//生成规定个数的1
                        int k = rand()%graph_node_num;
                        cross_sequence[k] = 1;
                    }
                }
                //随机选择两个个体进行交叉遗传，按交叉序列进行交叉遗传，进行POP_SIZE/2次
                for(int j=0;j<graph_node_num;j++){
                    if(cross_sequence[j] == 1){//如果交叉序列当前位置为1，则交换个体对中相应的二进制位
                        int temp = NEXT_POP[i][j];
                        NEXT_POP[2*i][j] = NEXT_POP[2*i+1][j];
                        NEXT_POP[2*i+1][j] = temp;
                    }
                }

            }
        }
        //执行变异算子，以一定的概率pm,对子代中的个体进行变异
        for(int i=0;i<POP_SIZE;i++){
            if(rand()/(RAND_MAX+1.0) < pm){//以概率pm进行变异
                int k = rand()%graph_node_num;//随机选择该个体的一个基因位
                NEXT_POP[i][k]=1-NEXT_POP[i][k];//反转该基因位
            }
        }
        cout << "产生一个随机概率试试:"<<rand()/(RAND_MAX+1.0)<<endl;
        generate_cnt++;
        finish_time=clock();//取当前运行时间

        //将POP种群更新为NEXT_POP种群,开始新的一代遗传，否则g_best_id始终保存着上一代POP种群中最优的解
        int diff_cnt =0;
        for(int i=0;i<POP_SIZE;i++){
            for(int j=0;j<graph_node_num;j++){
                if(POP[i][j] != NEXT_POP[i][j]) diff_cnt++;
                POP[i][j]=NEXT_POP[i][j];
            }
        }
        cout <<"种群差异度为:"<<diff_cnt<<endl;

    }
    

    //=======================================================================================//
    printf("遗传算法算法总共运行时间：%f s\n",(finish_time - start_time)/CLOCKS_PER_SEC);
    //找到当前最优解
    MCMF* mcmf = new MCMF();
    mcmf->init(n,m,s,t,graph_node_num);
    //复制图
    for(int j=0; j<edges0.size(); j++){
        mcmf->edges.push_back(edges0[j]);
    }
    for(int j=0;j< graph_node_num+consume_node_num+1;j++){
        for(int k=0;k<G0[j].size();k++){
            mcmf->G[j].push_back(G0[j][k]);
        }
    }
    //给mcmf中的粒子赋值
    for(int j=0; j< graph_node_num;j++){
        mcmf->particle.push_back(g_best[j]);
    }

    //根据粒子的解给服务器与汇点建边
    mcmf->create_server_edges();
    //运行最小费用最大流
    mcmf->MincotMaxflow(mcmf->s,mcmf->t);
    last_cost = mcmf->server_adress.size()*server_price + mcmf->cost;
    //利用深搜DSF找出残存网络中源点到汇点的路劲
    //因为建图的时候所有反向边的容量均为0，因此跑完费用流，所有反向边容量不为0的边都是有流量经过的边
    //每找出一条路径就把该路径所有经过的边的容量减去相应的流量
    mcmf->get_out();
    //printf("路径数量：%d\n", mcmf1.road_cnt);
    // 需要输出的内容
    mcmf->out_str = to_string(mcmf->road_cnt) + "\n" + mcmf->out_str;//加上路径的条数，即为最终的输出
    //cout << mcmf2->out_str<<endl;
    string out_str;
    if (demand_flow == mcmf->flow) out_str = mcmf->out_str;
    else out_str = "NA";
    printf("找到最小费用为:%d,可提供流量为:%d，消费节点需求流量为：%d,路径已输出。\n",last_cost,mcmf->flow,demand_flow);
    //cout<<out_str<<endl;
    char * topo_file = (char *) out_str.c_str();//此处将string转成ctring便于结果输出
    //char * topo_file = (char *)"18\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";
    // 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
    write_result(topo_file, filename);
}