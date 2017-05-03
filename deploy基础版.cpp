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
using namespace std;

#define N 100010
#define INF 100000000

struct Edge
{//定义边的结构体
    int from,to,cap,flow,cost;
    Edge(int u,int v,int ca,int f,int co):from(u),to(v),cap(ca),flow(f),cost(co){};//初始化结构体
};

struct MCMF
{
    int n,m,s,t;//定义，顶点的个数，边的个数，源点，和汇点
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

    void init(int n)//初始化
    {   //printf("初始图……\n");
        this->n=n;
        for(int i=0;i<n;i++)
            G[i].clear();//初始化图的顶点vector数组
        edges.clear();//初始化图的边vector数组
    }

    void addedge(int from,int to,int cap,int cost)//加边
    {   //printf("建边……\n");
        edges.push_back(Edge(from,to,cap,0,cost));//加边from,to,cap,cost其中flow为0
        edges.push_back(Edge(to,from,0,0,-cost));//加逆变to,from,-cost,其中cap,flow均为0
        int m=edges.size();//获取边的个数
        G[from].push_back(m-2);//G[i]中存储由i节点出发的边
        G[to].push_back(m-1);
    }

    bool SPFA(int s,int t,int &flow,int &cost)//寻找最小费用的增广路，使用引用同时修改原flow,cost
    {
        
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
    void MincotMaxflow(int s,int t)
    {   printf("运行最小费用最大流算法！\n" );
        while(SPFA(s,t,flow,cost));
    }

    void print_path(){//dfs找到一条路就把它输出出来，然后修改路径中的流量
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

    int dfs_find_road(int u){//dfs找路，找到就返回
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
    void get_out(){//已知终点和开始节点开始找
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

    /**
    void count_out_degree(){//统计所有点的出度(这里的度以流量为衡量标准)
        for(int i=0;i< graph_node_num;i++){//遍历每个网络节点
            int temp_flow = 0;
            for(int j=0;j<G[i].size();j++){
                Edge& e = edges[G[i][j]];//取边
                //printf("%d %d %d\n",e.from,e.to,e.cap);
                temp_flow += e.cap;//累加流量
            }
                out_degree.push_back(temp_flow);
        }

        int average = accumulate(out_degree.begin(), out_degree.end(),0) / graph_node_num;
        printf("average out_degree:%d\n", average);
        for(int i =0; i<graph_node_num;i++){//遍历map输出定点的出度信息
            //if(out_degree[i] > average)  {server_adress.push_back(i);printf("%d %d\n",i,out_degree[i]);}
            printf("%d %d\n",i,out_degree[i]);
        }

        for(int i =0; i< net_to_consume.size();i++){//消费节点直连服务器
            server_adress.push_back(net_to_consume[i]);
        }
    }**/

    vector<int> particle;//粒子个体，采用二进制编码

    void create_server_edges(){//参照粒子个体二进制编码给出的服务器部署方案建图
        for(int i=0; i<particle.size();i++){
            if(particle[i] == 1) server_adress.push_back(i);
        }
        for(int i = 0; i< server_adress.size();i++){
            printf("根据服务器部建边：%d %d %d %d\n",server_adress[i],t,INF,0);
            addedge(server_adress[i],t,INF,0);
        }
    }


};


//你要完成的功能入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename){
    int graph_node_num, graph_edge_num, consume_node_num;//节点的个数，边的个数，消费节点的个数
    int sever_price;//每台服务器的价格
    char seg[]=" ";
    //char charlist[50][50]={""};
    //读入数据开始建图
    int space_line_count = 0;//用于计算空格行
    int j;//用于控制文件行输入以及循环值的控制
    int start_node,end_node,total_bandwidth,cost_per_bandwidth;//用于暂存每行的网络图节点与边的信息
    int consume_node,net_node,demand_bandwidth;//由于暂存消费节点与网路图节点的链接信息以及消费需求带宽
    double start_time,finish_time;//存储程序开始时间与结束时间
    int server_num = 7;//用于保存服务器的个数，建图的时候统计边需要用到
    int last_cost;//存储最后所需要花费的费用

    start_time = clock();//取开始时间
    MCMF mcmf;//实例化最小费用最大流，开始读取数据并建图
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
                mcmf.s = graph_node_num+consume_node_num;
                mcmf.t = graph_node_num+consume_node_num+1;
                //初始化mcmf的节点个数以及边的个数
                mcmf.n = graph_node_num+consume_node_num+2;//节点个数=网络节点总数+消费节点总数+源点1+汇点1
                mcmf.m = graph_edge_num+2*consume_node_num+server_num;//边的个数=网络边的总数+消费节点建边个数+服务器建边个数
                mcmf.graph_node_num = graph_node_num;
                //mcmf实例初始化
                mcmf.init(mcmf.n);

                break;
            }
            case 1 :{
                char *substr = strtok(topo[i],seg);
                j = 0;
                while(substr != NULL){
                //strcpy(charlist[j],substr);
                if(j == 0) sever_price = atoi(substr);
                j++;
                //printf("%s\n",substr);
                substr = strtok(NULL,seg);
                    }
                cout << "统计服务器的费用:" << sever_price << endl;
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
                mcmf.addedge(start_node, end_node, total_bandwidth, cost_per_bandwidth);
                mcmf.addedge(end_node, start_node, total_bandwidth, cost_per_bandwidth);
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
                mcmf.addedge((consume_node+graph_node_num), net_node, demand_bandwidth, 0);
                // 建边S and consume_node
                mcmf.addedge(mcmf.s, consume_node+graph_node_num, INF, 0);
                mcmf.demand_flow += demand_bandwidth;
                //直接把服务器选在消费节点旁
                //mcmf.addedge(net_node,mcmf.t,INF,0);
                mcmf.net_to_consume.push_back(net_node);
                break;
            }
            case 4 : break;
            default: cout << "出错啦！" << endl;
            
        }
        
        //cout << "space_line_count:" << space_line_count << endl;
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

    cout << "超级源点S:" << mcmf.s << "，超级汇点T:" << mcmf.t << endl;

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
    
    //建立服务器到超级汇点的边，费用为0，流量限

    for(int i=0; i< mcmf.graph_node_num;i++){
        mcmf.particle.push_back(0);//
    }
    mcmf.particle[7]=1;
    mcmf.particle[13]=1;
    mcmf.particle[15]=1;
    mcmf.particle[20]=1;
    mcmf.particle[37]=1;
    mcmf.particle[38]=1;
    mcmf.particle[42]=1;
    mcmf.create_server_edges();

    //至此建图完毕，开始采用费用流算法，求得最小费用和最大流量
    mcmf.MincotMaxflow(mcmf.s,mcmf.t);
    finish_time = clock();//取结束时间
    cout << "运行一次最小费用流所需时间：" << finish_time - start_time << "ms" << endl;
    //总的费用=服务器费用+流量费用
    //last_cost = mcmf.server_adress.size()*sever_price+ mcmf.cost;

    last_cost = mcmf.server_adress.size()*sever_price+ mcmf.cost;
    cout << "需要的提供的流量："<< mcmf.demand_flow << "，可提供的最大流量："<< mcmf.flow << "，需要带宽租用费用：" << mcmf.cost << "，需要总的费用："<<last_cost<<endl;
    //利用深搜DSF找出残存网络中源点到汇点的路劲
    //因为建图的时候所有反向边的容量均为0，因此跑完费用流，所有反向边容量不为0的边都是有流量经过的边
    //每找出一条路径就把该路径所有经过的边的容量减去相应的流量
    mcmf.get_out();
    printf("路径数量：%d\n", mcmf.road_cnt);
    // 需要输出的内容
    mcmf.out_str = to_string(mcmf.road_cnt) + "\n" + mcmf.out_str;//加上路径的条数，即为最终的输出
    cout << mcmf.out_str<<endl;
    string out_str;
    if (mcmf.demand_flow == mcmf.flow) out_str = mcmf.out_str;
    else out_str = "NA";
    //char * topo_file = (char *)"18\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";
    char * topo_file = (char *) out_str.c_str();//此处将string转成ctring便于结果输出

    // 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
    write_result(topo_file, filename);
}