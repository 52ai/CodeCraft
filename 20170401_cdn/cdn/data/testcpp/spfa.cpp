#include<cstdio>  
#include<iostream>  
#include<cstring>  
#include<cmath>  
#include<cstdlib>  
#include<queue>  
#include<vector>  
#define inf 0x3f3f3f3f   
using namespace std;  
int n, m;  
struct node  
{  
    int to, distance;  
};  
vector<node>g[105];  
bool book[105];  
int dis[105];  
void myinit()  
{  
    memset(book, 0, sizeof(book));  
    for(int i = 0; i < 105; i++)  
    {  
        g[i].clear();  
    }  
    memset(dis, inf, sizeof(dis));  
}  
void SPFA(int s)  
{  
    queue<int> q;  
    q.push(s);  
    book[s] = 1;//加入队列并标记   
    while(!q.empty())  
    {  
        int point = q.front();  
        q.pop();  
        book[point] = 0;//弹出队列并取消标记   
        for(int i = 0; i < g[point].size(); i++)  
        {  
            if(g[point][i].distance + dis[point] < dis[g[point][i].to])//比较   
            {  
                dis[g[point][i].to] = g[point][i].distance + dis[point];//更新路径   
                if(!book[g[point][i].to])//若该点不在队列   
                {  
                    q.push(g[point][i].to);  
                    book[g[point][i].to] = 1;  
                }  
            }  
              
        }  
    }  
}  
int main()  
{  
    int i, j, x, y, d;  
    while(scanf("%d%d", &n, &m)!=EOF &&(n || m))  
    {  
        myinit();  
        for(i = 0; i < m; i++)  
        {  
            scanf("%d%d%d", &x, &y, &d);  
            node n1, n2;  
            n1.to = x; n1.distance = d;  
            n2.to = y; n2.distance = d;  
            g[x].push_back(n2);  
            g[y].push_back(n1);  
        }  
        dis[1] = 0;  
        SPFA(1);  
        cout<<dis[n]<<endl;  
    }  
    return 0;  
}  