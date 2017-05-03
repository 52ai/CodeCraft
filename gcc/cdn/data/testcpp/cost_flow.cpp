/**
最小费用流模板解决最小费用流问题
**/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <math.h>
#include <queue>
#include <stack>
using namespace std;
typedef long long ll;
const   int oo=1e9;//表示无穷大数
const   int mm=11111111;//表示边的最大数量，记住要是原图的两倍，在加边的时候都是双向的
const   int mn=888888;//表示点的最大数量
int node,src,dest,edge;
/**node 表示结点数，src表示源点,dest表示汇点，edge统计边数**/
int ver[mm],flow[mm],cost[mm],nex[mm];
/**ver表示边指向的节点，flow表示边的容量,cost表示边的费用,next表示链表的下一条边**/
int head[mn],dis[mn],p[mn],q[mn],vis[mn];
/**这些变量基本与最大流相同，增加了cost 表示边的费用，p记录可行流上节点对应的反向边*/
void prepare(int _node,int _src,int _dest)
{
    node=_node,src=_src,dest=_dest;//初始化节点数，以及源点src和汇点dest
    //for(int i=0; i<node; i++)head[i]=-1,vis[i]=0;
    memset(head,-1,sizeof(head));//初始化head数组元素全部为-1
    memset(vis,0,sizeof(vis));//初始化访问记录数据元素全部为0
    edge=0;//统计变数为0
}
void addedge(int u,int v,int f,int c)
{//增加一条u到v，容量为f,费用为c的边
    ver[edge]=v,flow[edge]=f,cost[edge]=c,nex[edge]=head[u],head[u]=edge++;
    ver[edge]=u,flow[edge]=0,cost[edge]=-c,nex[edge]=head[v],head[v]=edge++;
}
bool spfa()/**spfa 求最短路，并用 p 记录最短路上的边*/
{
    int i,u,v,l,r=0,tmp;
    //for(i=0; i<node; ++i)dis[i]=oo;
    memset(dis,oo,sizeof(dis))//初始化距离路径
    dis[q[r++]=src]=0;
    p[src]=p[dest]=-1;
    for(l=0; l!=r; (++l>=mn)?l=0:l)
        for(i=head[u=q[l]],vis[u]=0; i>=0; i=nex[i])
            if(flow[i]&&dis[v=ver[i]]>(tmp=dis[u]+cost[i]))
            {
                dis[v]=tmp;
                p[v]=i^1;
                if(vis[v]) continue;
                vis[q[r++]=v]=1;
                if(r>=mn)r=0;
            }
    return p[dest]>-1;
}
int SpfaFlow()/**源点到汇点的一条最短路即可行流，不断的找这样的可行流*/
{
    int i,ret=0,delta;
    while(spfa())
    {
        for(i=p[dest],delta=oo; i>=0; i=p[ver[i]])
            if(flow[i^1]<delta)delta=flow[i^1];
        for(i=p[dest]; i>=0; i=p[ver[i]])
            flow[i]+=delta,flow[i^1]-=delta;
        ret+=delta*dis[dest];
    }
    return ret;
}
/*****************************以上是最小费用流模板**************************************/
struct hh
{
    int x,y;
    hh(int xx,int yy)
    {
        x=xx,y=yy;
    }
};
vector<hh>hou;
vector<hh>man;

int real(int x)
{
    if(x<0)
        return -x;
    return x;
}
char x[110][110];
int main()
{
    int m,n;
    while(~scanf("%d%d",&n,&m)&&n+m)
    {
        hou.clear();
        man.clear();
        for(int i=0; i<n; i++)
            for(int j=0; j<m; j++)
            {
                scanf(" %c",&x[i][j]);
                if(x[i][j]=='H')
                {
                    hh t(i,j);
                    hou.push_back(t);
                }
                else if(x[i][j]=='m')
                {
                    hh t(i,j);
                    man.push_back(t);
                }
            }
        int l=hou.size();
        prepare(2*l+2,2*l,2*l+1);
        for(int i=0; i<l; i++)
            for(int j=0; j<l; j++)
                addedge(i,j+l,1,real(hou[i].x-man[j].x)+real(hou[i].y-man[j].y));
        for(int i=0; i<l; i++)
            addedge(2*l,i,1,0),addedge(i+l,2*l+1,1,0);
        printf("%d\n",SpfaFlow());
    }
    return 0;
}
