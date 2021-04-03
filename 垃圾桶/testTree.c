#include<stdio.h>
#include<stdlib.h>
#include"mTree.h"
int main()
{
    mTreeNode *root;
    treeInitiate(&root);
    GrowLeave(root,6);
    GrowLeave(root,2);
    GrowLeave(root,4);
    GrowLeave(root,3);
    GrowLeave(root,1);
    GrowLeave(root,9);
    int i;
    printf("%d in all\n",root->n);
    for(i=0;i<root->n;i++)
    {
        printf("%d ",root->next[i]->data);
    }
    CutTree(root,-1);
}
