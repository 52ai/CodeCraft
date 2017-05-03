#include "deploy.h"
#include <stdio.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <time.h>
#include <stack>
using namespace std;

const int INF=1e9; //定义无穷大
struct queue
{//用数组实现队列
    int h,t;
    int a[100010];
    inline void clear() {h=1,t=0;}//初始化队列
    inline void push(int n) {a[++t]=n;}//入队
    inline int front() {return a[h];}//获取队首元素
    inline void pop() {h++;}//出队
    inline bool empty() {return h>t;}//当h>t时，则表示当前队列为空，返回true
}q;

struct edge
{//用结构体定义边的数组a
    int next,to,remain,cost;//next,to,remain,costß
}a[100010];//该数组存储所有边的信息

int head[100010];//记录该节点所连边在边数组a中的地址
int dis[5010];//存储最短路径
int f[5010];//以终止节点的标号，存储边
int u[5010];//用来存储，当前由当前节点出发的所有边中最小流量
bool b[5010];//是否在队列中

int n,m,s,t;//存储总节点个数，总的边数，源点，汇点
int x,y,w,z;//存储边的起始点，终止点，边的容量以及单位流量的费用
int num=1;//边的计数器
int visited_father[100010];//存储按图访问顺序节点的父节点
int visited[1000010];//访问标记数组
stack<int> road_stack;//存储路径输出的栈
stack<int> flow_stack;//存储路径中边的反向流量
stack<int> edge_stack;//存储路径中的边，方便更改边的反向流量

int path_flow;//缓存每条路径最后流向汇点的流量
int flow_sum=0;
int road_cnt=0;//路径计数器
int flag=0;


inline int min(int a,int b) {return a<b?a:b;}//定义内联函数min(a,b)返回a与b的最小值

inline void add(int x,int y,int z,int w)//x表示边的起始点，y表示边的终止点，z表示边的容量，w表示边的单位流量的费用
{//定义添加边的函数
    a[++num].next=head[x];//当前边的next变量存储的是上一条由x出发边的地址
    a[num].to=y;//将当前边的终止节点存入边的信息中
    a[num].remain=z;//将当前边的容量存入边的信息中
    a[num].cost=w;//将当前边的费用存入边的信息中
    head[x]=num;//将当前边的地址信息存入head[x]中

    a[++num].next=head[y];//存储上一条由y出发边的地址
    a[num].to=x;//建立反向边
    a[num].cost=-w;//反向边的费用为-cost[x][y]
    head[y]=num;//将当前由y出发边的地址存入head[y]中
}

inline bool SPFA(int s,int t,int &x,int &y)
{//采用spfa算法求当前流量下最短路径，若存在则返回true,若不存在则返回fasle
    memset(dis,0x3f,sizeof(dis));//将内存初始化为极大值
    dis[s]=0;//源点到源点的最短距离定义为0
    q.clear();//初始化队列
    q.push(s);//将s入队
    b[s]=1;//将访问数组中s对应的值置为1表示已访问
    u[s]=INF;//将最小流量数组中s对应的值初始化为无穷大
    while(!q.empty())
    {//当队列不为空执行下面操作
        int tmp=q.front();q.pop();//取队首元素，并出队
        b[tmp]=0;
        for(int i=head[tmp];i;i=a[i].next){//遍历由当前节点tmp出发所有边的信息
          if(a[i].remain && dis[a[i].to]>dis[tmp]+a[i].cost)
          {//如果当前边还有剩余容量且松弛操作成立，则进行如下操作
            f[a[i].to]=i;//将当前边，以终止节点标号的形式存储在f数组中
            dis[a[i].to]=dis[tmp]+a[i].cost;//更新最短路径数组
            u[a[i].to]=min(u[tmp],a[i].remain);//取由当前节点出发路径中最小的流量，并以边的终止节点序号的形式存储在u数组中
            if(!b[a[i].to]) q.push(a[i].to),b[a[i].to]=1;//如果下一个节点没有入过队，则将该点入队，并将访问数组中对应的值置为1
          }
        }
    }
    if(dis[t]>INF) return 0;//如果路径不可达，则返回0，算法结束
    //更新当前最大流量，以及最大流量下所需的费用
    x+=u[t];//否则将当前最大流量扩展为x+u[t]
    y+=dis[t]*u[t];//且所需的费用为y+dis[t]*u[t]

    //构建残存图，从源点往回构建
    int p=t;
    while(p!=s)
    {//更新正向边与反向边 i^1 就是取i得反向边，也就相当于i+1,这是由边存储时候决定的   
        a[f[p]].remain-=u[t];//在最短路径中，找到以p为终止节点的边，并将其容量减去u[t]
        a[f[p]^1].remain+=u[t];//在最短路径中找到以p为起始节点的边，将其容量加上u[t],因为反向边的费用是负的
        //printf("%d %d\n", f[p],f[p]+1);
        p=a[f[p]^1].to;//从汇点出发反向找当前最短路径中的上一条边，直至源点结束
    }

    return 1;//如果存在最短路径，则返回1，继续执行算法
}

inline void mcmf(int s,int t,int &x,int &y)
{//开始最小费用最大流算法
    x=y=0;
    while(SPFA(s,t,x,y));//策略不断求当前流量下最短路径，直至不存在最短路径，程序结束
}

inline void print_path(){
    stack<int> my_edge_stack = edge_stack;
    path_flow = INF;//初始化路径流量

    while(!road_stack.empty()){
        printf("%d ", road_stack.top());
        road_stack.pop();
    }
    printf("\n");

    while(!edge_stack.empty()){
        int i = edge_stack.top();
        printf("%d %d\n", a[i].to, a[i^1].remain);
        path_flow = (a[i^1].remain < path_flow ? a[i^1].remain : path_flow);
        edge_stack.pop();
    }

    while(!my_edge_stack.empty()){
        int i=my_edge_stack.top();
        a[i^1].remain -= path_flow;
        //printf("%d %d\n", a[i].to, a[i^1].remain);
        my_edge_stack.pop();
    }

    road_cnt++;
    flow_sum += path_flow;
    printf("找到第%d条路啦！路的流量为%d,累计提供流量为:%d\n",road_cnt,path_flow,flow_sum);

}

inline int dfs_find_road(int p){//dfs找路，找到就返回
    if(p == t) {print_path();flag=1;return 1;}
    else{
        for(int i= head[p];i;i=a[i].next){
            if(!visited[a[i].to] && a[i^1].remain > 0){
                //if(a[i].to == 3) printf("异常数：%d %d %d\n",p,a[i].to,a[i^1].remain);
                visited[a[i].to]=1;
                road_stack.push(a[i].to);
                edge_stack.push(i);
                //printf("%d %d %d \n",p,a[i].to,a[i^1].remain);
                dfs_find_road(a[i].to);
                //if(a[i].to == 3) printf("%d %d %d\n",p,a[i].to,a[i^1].remain);
                if(flag) {return 1;}
                edge_stack.pop();
                road_stack.pop();
                visited[a[i].to]=0;
            }
        }
    }
    return 0;
}

inline void get_out(){//已知开始节点和终止节点开始找
    int my_cnt = 100;
    printf("起点:%d,终点:%d\n", s,t);
    memset(visited,0,sizeof(visited));
    visited[s]=1;
    road_stack.push(s);
    while(dfs_find_road(s)){
        my_cnt--;
        if(my_cnt == 0 ) break;
        memset(visited,0,sizeof(visited));
        visited[s]=1;
        road_stack.push(s);
    }
    /**
    int i = head[34];
    while(i){
        printf("%d %d\n",a[i].to,a[i].remain );
        i = a[i].next;
    }
    **/
}


//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename)
{	int graph_node_num, graph_edge_num, consume_node_num;//节点的个数，边的个数，消费节点的个数
	int sever_price;//每台服务器的价格
    char seg[]=" ";
    //char charlist[50][50]={""};
	//读入数据开始建图
    int space_line_count = 0;//用于计算空格行
    int j;//用于控制文件行输入以及循环值的控制
    int start_node,end_node,total_bandwidth,cost_per_bandwidth;//用于暂存每行的网络图节点与边的信息
    int consume_node,net_node,demand_bandwidth;//由于暂存消费节点与网路图节点的链接信息以及消费需求带宽
    int last_cost;//存储最后总的费用
    double start_time,finish_time;//存储程序开始时间与结束时间
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
                add(start_node, end_node, total_bandwidth, cost_per_bandwidth);
                add(end_node, start_node, total_bandwidth, cost_per_bandwidth);
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
                add((consume_node+graph_node_num), net_node, demand_bandwidth, 0);
                // 建边S and consume_node
                add(s, consume_node+graph_node_num, INF, 0);
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

    cout << "S:" << s << "T:" << t << endl;

    //建边T and server_node 
    //case0 最佳的服务器选址为：7, 13, 15, 22, 37, 38, 43,先手工建个边试试
    /**case4
    add(12,t,INF,0);
    add(15,t,INF,0);
    add(20,t,INF,0);
    add(22,t,INF,0);
    add(26,t,INF,0);
    add(37,t,INF,0);
    add(48,t,INF,0);
    **/

    add(7,t,INF,0);
    add(13,t,INF,0);
    add(15,t,INF,0);
    add(22,t,INF,0);
    add(37,t,INF,0);
    add(38,t,INF,0);
    add(43,t,INF,0);


    start_time = clock();//取开始时间
    //至此建图完毕，开始采用费用流算法，求得最小费用和最大流量
    mcmf(s,t,x,y);
    finish_time = clock();//取结束时间
    cout << "运行一次最小费用流所需时间：" << finish_time - start_time << "ms" << endl;
    //总的费用=服务器费用+流量费用
    last_cost = 7*sever_price+ y;
    cout <<"提供最大流量："<< x << "，需要带宽租用费用：" << y << "，需要总的费用："<<last_cost<<endl;
    //利用深搜DSF找出残存网络中源点到汇点的路劲
    //因为建图的时候所有反向边的容量均为0，因此跑完费用流，所有反向边容量不为0的边都是有流量经过的边
    //每找出一条路径就把该路径所有经过的边的容量减去相应的流量
    get_out();

    //printf("路径数量：%d\n", road_cnt);
	// 需要输出的内容
	char * topo_file = (char *)"18\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);

}
