#include <iostream>
#include <stdio.h>
#include <string.h>
#include<algorithm>
#include<queue>
using namespace std;
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
const int INF=1000000000,N=1000000;///N表示点的最多数量
int head[N],dis[N],path[N];
bool vis[N];
int ip;
struct data {
    int to,next,w;
} tu[N*10];
void init() {
    ip=0;
    memset(head,-1,sizeof(head));//初始化head数组元素为-1
    memset(path,0,sizeof(path));//初始化路径数组
}
void add(int u,int v,int w) {//通过每条边的信息构建图
    tu[ip].to=v;
    tu[ip].w=w;
    tu[ip].next=head[u];
    head[u]=ip++;
}
int pre[N];
void spfa(int s,int n) {
    queue<int>q;//定义一个队列q,用于存储待优化的节点
    for (int i=0; i<=n; i++)
        dis[i]=pre[i]=INF;//初始化最短路径数组为无穷大数
    memset(vis,0,sizeof(vis));//初始化访问数组vis元素全部为0
    q.push(s);//将初始待优化节点入队
    dis[s]=0;//初始节点到初始节点的距离为0
    while(!q.empty()) {//当队列不为空的时候执行下面语句
        int h=q.front();//访问队列首元素
        q.pop();//出队
        vis[h]=0;//将vis数组中标号为队列中首节点的值置为0
        for (int i=head[h]; i!=-1; i=tu[i].next) {//访问所有与队首节点相连的所有节点，直到该节点的下一个节点为-1则结束
            int v=tu[i].to;//获取当前节点的标号
            int w=tu[i].w;//获取上一节点到当前节点的权重
            if (dis[v]>dis[h]+w) {//对当前节点使用松弛操作
                pre[v]=w;//记录最短路径中的权重
                dis[v]=dis[h]+w;//更新最短路径数组
                path[v]=h;
                if (!vis[v]) {//如果v没有被访问过，进行下列操作
                    vis[v]=1;//将访问数组vis[v]的值置为1，表示该元素已被访问过
                    q.push(v);//如果没有被访问过，应该将其入队
                }
            } else if(dis[v]==dis[h]+w){
                pre[v]=min(pre[v],w);//如果松弛操作无效的话，则更新权重记录数组中的信息为min(pre[v],w)
                path[v]=h;
            }
                
        }
    }
}
void printpath(int k){//递归输出最短路径的节点循序
    if (path[k]!=0) printpath(path[k]);
    cout << k << ' ';

}

int main(int argc, char** argv) {
    init();//初始化head数组元素为-1
    int n,m;//n表示节点个数，m表示图中边的个数
    cin>>n>>m;
    for(int i=0; i<m; i++) {//循环输入每条边的信息，起始节点，终止节点，权重
        int a,b,w;
        cin>>a>>b>>w;
        add(a,b,w);//将输入边的信息添加到图中
        add(b,a,w);//由于是边双向的，因此还需反向添加边
    }
    spfa(1,n);//采用spfa算法求1到所有节点的最短路径
    int ans=0;
    for(int i=2; i<=n; i++){
        cout<<"权重路径："<< pre[i]<<endl;
        ans+=pre[i];

    }
    cout<<ans<<endl;
    cout<<"---------------华丽的分割线，下面输出最短路径的节点顺序-----------------"<<endl;
    for (int i=1;i<=n;i++){
        printpath(i);
        cout<<endl;
    }

    return 0;
}