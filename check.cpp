/* coded by karel */
// 使用说明，我们要写的cdn程序一样。输入两个文件的参数。检验产生的结果是否有效

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/timeb.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>


using namespace std;

const int MAX_EDGE_NUM = 40000;
#define MAX_LINE_LEN 55000
#define PRINT printf
const int INNER_MAX = 1002;
const int CONSUME_MAX = 500;
const int INT_MAX = 0x7fffff;
const int INT_MIN = -INT_MAX;


int read_file(char ** const buff, const unsigned int spec, const char * const filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
      PRINT("Fail to open file %s, %s.\n", filename, strerror(errno));
        return 0;
    }
    PRINT("Open file %s OK.\n", filename);

    char line[MAX_LINE_LEN + 2];
    unsigned int cnt = 0;
    while ((cnt < spec) && !feof(fp))
    {
        line[0] = 0;
        if (fgets(line, MAX_LINE_LEN + 2, fp) == NULL)  continue;
        if (line[0] == 0)   continue;
        buff[cnt] = (char *)malloc(MAX_LINE_LEN + 2);
        strncpy(buff[cnt], line, MAX_LINE_LEN + 2 - 1);
        buff[cnt][MAX_LINE_LEN + 1] = 0;
        cnt++;
    }
    fclose(fp);
    PRINT("There are %d lines in file %s.\n", cnt, filename);

    return cnt;
}

struct ConsumeNode
{
  int consume_node;
  int inner_node;
  int requirement;
  bool seen;
  ConsumeNode(int consume = -1, int inner = -1, int req = -1, bool s = false):consume_node(consume), inner_node(inner), requirement(req), seen(s){}
  bool operator< (const ConsumeNode& b) const {
    return requirement > b.requirement;
  }
};
ConsumeNode cnode[CONSUME_MAX];

int cost_table[INNER_MAX][INNER_MAX];
int capacity_table[INNER_MAX][INNER_MAX];
int inn, cnn, server_cost;

void init(char* topo[MAX_EDGE_NUM], int line_num){
  // init cost_table.
  for(int i = 0; i < INNER_MAX; ++i){
    fill(cost_table[i], cost_table[i] + INNER_MAX, INT_MAX);
    cost_table[i][i] = 0;
    fill(capacity_table[i], capacity_table[i] + INNER_MAX, 0);
  }

  int i = 0, link = 0;
  stringstream stream;
  stream << topo[i];
  stream >> inn >> link >> cnn;
  stream.clear();

  i += 2;
  stream << topo[i];
  stream >> server_cost;
  stream.clear();

  i += 2;
  int temp = i + link;
  int from, to, cap, cost;
  for(; i < temp; ++i){
    stream << topo[i];
    stream >> from >> to >> cap >> cost;
    stream.clear();

    cost_table[from][to] = cost;
    cost_table[to][from] = cost;

    capacity_table[from][to] = cap;
    capacity_table[to][from] = cap;

  }

  stream.str("");
  ++i;
  int consume, inner, req;
  for(int k = 0; k < cnn; ++k){
    stream << topo[i++];
    stream >> consume >> inner >> req;
    stream.clear();
    cnode[k] = ConsumeNode(consume, inner, req);
  }
  stream.str("");
}

int check(char* res[MAX_EDGE_NUM]){
  bool server[INNER_MAX];
  fill(server, server+INNER_MAX, false);
  stringstream stream;
  int i = 0;
  int line = 0;
  int cost = 0;
  int num;
  stream << res[i];
  stream >> line;
  stream.clear();
  i = 2;
  for(; i < 2 + line; ++i){
    vector<int> path;
    stream << res[i];
    while(stream >> num){
      path.push_back(num);
    }
    stream.clear();
    stream.str("");
    int flow = path.back();
    for(int i = 0; i < path.size() - 3; ++i){
      capacity_table[path[i]][path[i+1]] -= flow;
      if(capacity_table[path[i]][path[i+1]] < 0){
        cout << "The flow error:" << endl;
        for(int i = 0; i < path.size(); ++i){
          cout << path[i] << " ";
        }
        cout << endl;
        return -1;
      }
      cost += flow * cost_table[path[i]][path[i+1]];
    }

    if(server[path[0]] == false){
      server[path[0]] = true;
      cost += server_cost;
    }

    cnode[path[path.size() - 2]].requirement -= flow;
  }

  for(int i = 0; i < cnn; ++i){
    if(cnode[i].requirement != 0){
      cout << "consume node " << i << " error: " << cnode[i].requirement << endl;
      return -1;
    }
  }

  return cost;
}

int main(int argc, char * argv[]){

  char *topo[MAX_EDGE_NUM];

  int line_num;

  char* topo_file = argv[1];

  line_num = read_file(topo, MAX_EDGE_NUM, topo_file);

  char* result_file = argv[2];

  char* res[MAX_EDGE_NUM];
  read_file(res, MAX_EDGE_NUM, result_file);

  init(topo, line_num);

  int cost = check(res);
  if(cost > 0){
    cout << "Result corrent:\n The cost is: " << cost << endl;
  }else {
    cout << "Result error!" << endl;
  }
  return 0;
}
