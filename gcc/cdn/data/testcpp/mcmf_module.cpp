#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
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
int visited[100010]={0};//访问标志数组
stack<int> road_stack;//存储路径输出的栈
int last_flow;//缓存每条路径最后流向汇点的流量
int road_cnt=0;//路径计数器

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
    //将当前路径栈内的元素，按顺序打印出来
    stack<int> my_s = road_stack;
    while(!my_s.empty()){
        printf("%d ",my_s.top());
        my_s.pop();
    }
    printf("%d\n", last_flow);
    road_cnt++;//每打印一条路劲，则路径计数器加1
}

inline void dfs_get_out(int p){//p为当前节点
    if(p == t) {
        print_path();
        visited[p]=0;
    }
    else{
        for(int i = head[p];i;i=a[i].next){
            if(!visited[a[i].to] && a[i^1].remain > 0 ){
                road_stack.push(a[i].to);//将当前有效边的终止节点入栈
                printf("%d %d %d \n",p,a[i].to,a[i^1].remain);
                last_flow = a[i^1].remain;
                visited[a[i].to]=1;
                dfs_get_out(a[i].to);//对当前有效边的终止节点进行dfs
                road_stack.pop();//回溯，需要将之前入栈的节点出栈
        }
    }

    }
}

int main()
{   /**
    scanf("%d%d%d%d",&n,&m,&s,&t);
    for(int i=1;i<=m;i++)
      scanf("%d%d%d%d",&w,&x,&y,&z),add(w,x,y,z);
    mcmf(s,t,x,y);
    printf("%d %d",x,y);
    **/
    m=4,n=5,s=4,t=3;
    add(4,2,30,2);
    add(4,3,20,3);
    add(2,3,20,1);
    add(2,1,30,9);
    add(1,3,40,5);
    mcmf(s,t,x,y);
    printf("%d %d\n",x,y);//最大流量，最小费用
    //利用深搜DSF找出残存网络中源点到汇点的路劲
    //因为建图的时候所有反向边的容量均为0，因此跑完费用流，所有反向边容量不为0的边都是有流量经过的边
    //每找出一条路径就把该路径所有经过的边的容量减去相应的流量
    road_stack.push(s);//将源点s入栈 
    visited[s]=1;//以汇点为初始节点，并将其的访问数组对应值设为1
    dfs_get_out(s);
    printf("路径数量：%d\n", road_cnt);
    return 0;
}