#include<iostream>
#include<cstdio>
#include<cstring>
using namespace std;
#define inf 0x3fffffff
#define nMax 1000
#define eMax 40000
#define abs(a) ((a)>0?(a):-(a))

struct enode     //前向星，用来存边
{
    int v, cap, cost, next, re;    //  re记录逆边的下标。
}edge[eMax];

int n,m;
int k,edgeHead[nMax]; //用来找邻接边
int que[nMax], pre[nMax], dis[nMax];//que:spfa队列；pre前一个点，dis距离
bool vis[nMax];
 
void addEdge(int u, int v, int ca, int co) //ca容量，co花费（添加边）
{
    edge[k].v = v;
    edge[k].cap = ca;
    edge[k].cost = co;
    edge[k].next = edgeHead[u];
    edge[k].re = k + 1;
    edgeHead[u] = k ++;
    edge[k].v = u;
    edge[k].cap = 0;
    edge[k].cost = -co;
    edge[k].next = edgeHead[v];
    edge[k].re = k - 1;
    edgeHead[v] = k ++;
}
 
bool spfa()        //最短路spfa
{                  //源点为0，汇点为n+1
    int i, head = 0, tail = 1;
    for(i=0;i<=n+1;i++)
{
        dis[i] = inf;
        vis[i] = false;
pre[i] = 0;
    }
    dis[0] = 0;
    que[0] = 0;
    vis[0] = true;
    while(tail>head)
{     
        int u = que[head++];
        for(i = edgeHead[u]; i != -1; i = edge[i].next)
{
            int v=edge[i].v;
            if(edge[i].cap && dis[v] > dis[u] + edge[i].cost)
{
                dis[v]=dis[u]+edge[i].cost;
                pre[v]=i;
                if(!vis[v])
{
                    vis[v]=true;
                    que[tail++]=v;
                }
            }
        }
        vis[u] = false;
    }
    if(dis[n]==inf) return false;
    return true;
}
 
int compute()
{
int u,p,sum=inf;
    for(u=n;u!=0;u=edge[edge[p].re].v)
{
        p=pre[u];
        if(sum>edge[p].cap)
sum=edge[p].cap;
    }
int ans=0;
    for(u = n; u != 0; u = edge[edge[p].re].v){
        p = pre[u];
        edge[p].cap -= sum;
        edge[edge[p].re].cap += sum;
        ans+=sum * edge[p].cost;     //  cost记录的为单位流量费用，必须得乘以流量。
    }
return ans;
}

int main(){
    n=3;   //点数
    m=3;   //边数
    k=0;     //记录edge里的边数  
    memset(edgeHead,-1,sizeof(edgeHead));
    //create graph
    addEdge(0,1,50,0);
    addEdge(0,3,20,0);
    addEdge(1,2,30,3);
    addEdge(1,3,20,1);
    addEdge(2,3,20,1);
    //addEdge(1,4,inf,0);
    //addEdge(2,4,inf,0);
    //addEdge(3,4,10,4);

    int sum=0;
    while(spfa()) sum+=compute();
    printf("%d\n",sum);

    return 0;
}
