#include <iostream>
using namespace std;

struct BiTNode{
 int times;//赢的次数
 int probability;//赢的概率
 struct BiTNode *lchild, *rchild;//左右孩子
};

BiTNode *T;
BiTNode *BiTree;

BiTNode *CreateBiTree(int times, int probability);
void Inorder(BiTNode* &T);
void PreOrderTraverse(BiTNode* &T);
void Posorder(BiTNode* &T);

int p,q,n;
int result =0;

int main(void){
    while(cin >> p >> q >> n){//程序代码
      //根据已知的参数创建二叉树
    BiTree = CreateBiTree(0,p);//递归创建二叉树
    Inorder(BiTree);
    cout<<result<< endl;
    }

    return 0;
}
BiTNode *CreateBiTree(int times, int probability){
  //按先序输入二叉树中结点的值（一个字符），空格字符代表空树
  //构造二叉树表表示二叉树T。
  //创建左子树
  T = new BiTNode;

  T->times = times;
  if(probability >= 100) T->probability = 100;
  else  T->probability = probability;

  int lp = p/(2^(times+1));
  int rp = 1-lp+q;

  cout<<T->times<<","<<T->probability<<endl;//访问参数

  if(times == 2) T->lchild = NULL;
  else T->lchild = CreateBiTree(times+1,lp);

  if(lp == 100 ) T->rchild = NULL;
  else T->rchild = CreateBiTree(times,rp);
  return(T);

}//CreateTree


void PreOrderTraverse(BiTNode* &T){
 //先序递归遍历二叉树
 if(T){//当结点不为空的时候执行
  cout<<T->times<<","<<T->probability<<endl;//访问参数
  PreOrderTraverse(T->lchild);//
  PreOrderTraverse(T->rchild);
 }
 else cout<<"";
}//PreOrderTraverse

void Inorder(BiTNode* &T){//中序递归遍历二叉树
 if(T){//bt=null退层
  Inorder(T->lchild);//中序遍历左子树
  cout<<T->times<<","<<T->probability<<endl;//访问参数
  Inorder(T->rchild);//中序遍历右子树
 }
 else cout<<"";
 }//Inorder

void Posorder(BiTNode* &T){
 if(T){
  Posorder(T->lchild);//后序递归遍历左子树
  Posorder(T->rchild);//后序递归遍历右子树
  cout<<T->times<<","<<T->probability<<endl;//访问参数
 }
 else cout<<"";
}