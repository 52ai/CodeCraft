#include <iostream>
using namespace std;
int first[10005];
struct edge{
	int point,next,len;
} e[10005];
void add(int i, int u, int v, int w){
		e[i].point = v;
		e[i].next = first[u];
		e[i].len = w;
		first[u] = i;
}
int n,m;
int main(){
	int u,v,w;
	cin >> n >> m;
	for (int i = 1; i <= m; i++){
		cin >> u >> v >> w;
		add(i,u,v,w);
	}  //这段是读入和加入
	for (int i = 0; i <= n; i++){
		cout << "from " << i << endl;
		for (int j = first[i]; j; j = e[j].next)  //这就是遍历边了
			cout << "to " << e[j].point << " length= " << e[j].len << endl;
	}
}