#include<iostream>
#include<algorithm>
#include<string>
#include<sstream>
#include<set>
#include<vector>
#include<stack>
#include<map>
#include<queue>
#include<deque>
#include<cstdlib>
#include<cstdio>
#include<cstring>
#include<cmath>
#include<ctime>
#include<functional>
using namespace std;

#define N 20010
#define INF 100000000
struct Edge
{
    int from,to,cap,flow;
    Edge(int u,int v,int c,int f):from(u),to(v),cap(c),flow(f){}
};
struct Dinic
{
    int n,m,s,t;//结点数，边数（包括反向弧），源点编号，汇点编号
    vector<Edge>edges;//边表，dges[e]和dges[e^1]互为反向弧
    vector<int>G[N];//邻接表，G[i][j]表示结点i的第j条边在e数组中的编号
    bool vis[N]; //BFS的使用
    int d[N]; //从起点到i的距离
    int cur[N]; //当前弧下标

    void addedge(int from,int to,int cap)
    {
        edges.push_back(Edge(from,to,cap,0));
        edges.push_back(Edge(to,from,0,0));
        int  m=edges.size();
        G[from].push_back(m-2);
        G[to].push_back(m-1);
    }

    bool bfs()
    {
        memset(vis,0,sizeof(vis));
        queue<int>Q;
        Q.push(s);
        d[s]=0;
        vis[s]=1;
        while(!Q.empty())
        {
            int x=Q.front();Q.pop();
            for(int i=0;i<G[x].size();i++)
            {
                Edge&e=edges[G[x][i]];
                if(!vis[e.to]&&e.cap>e.flow)//只考虑残量网络中的弧
                {
                    vis[e.to]=1;
                    d[e.to]=d[x]+1;
                    Q.push(e.to);
                }
            }

        }
        return vis[t];
    }

    int dfs(int x,int a)//x表示当前结点，a表示目前为止的最小残量
    {
        if(x==t||a==0)return a;//a等于0时及时退出，此时相当于断路了
        int flow=0,f;
        for(int&i=cur[x];i<G[x].size();i++)//从上次考虑的弧开始，注意要使用引用，同时修改cur[x]
        {
            Edge&e=edges[G[x][i]];//e是一条边
            if(d[x]+1==d[e.to]&&(f=dfs(e.to,min(a,e.cap-e.flow)))>0)
            {
                e.flow+=f;
                edges[G[x][i]^1].flow-=f;
                flow+=f;
                a-=f;
                if(!a)break;//a等于0及时退出，当a!=0,说明当前节点还存在另一个曾广路分支。

            }
        }
        return flow;
    }

    int Maxflow(int s,int t)//主过程
    {
        this->s=s,this->t=t;
        int flow=0;
        while(bfs())//不停地用bfs构造分层网络，然后用dfs沿着阻塞流增广
        {
            memset(cur,0,sizeof(cur));
            flow+=dfs(s,INF);
        }
        return flow;
    }
  };

  int main()
  {
      //freopen("in.txt","r",stdin);
      int n,m;
      while(scanf("%d%d",&n,&m)!=EOF)
      {
          int a,b,c;
          Dinic dinic;
          //dinic.n=n+1;dinic.m=m;
          for(int i=1;i<=n;i++)
          {
              scanf("%d%d",&a,&b);
              dinic.addedge(0,i,a);
              dinic.addedge(i,n+1,b);
          }
          for(int i=0;i<m;i++)
          {
              scanf("%d%d%d",&a,&b,&c);
              dinic.addedge(a,b,c);
              dinic.addedge(b,a,c);
          }
          printf("%d\n",dinic.Maxflow(0,n+1));
      }
  }
