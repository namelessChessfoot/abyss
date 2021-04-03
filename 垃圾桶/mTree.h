typedef struct Node
{
    int data,n;
    struct Node *next[5];
} mTreeNode;
void treeInitiate(mTreeNode **head)
{
    if(((*head)=(mTreeNode *)malloc(sizeof(mTreeNode)))==NULL) exit(1);
    int i;
    for(i=0;i<5;i++)
    {
        (*head)->next[i]=NULL;
    }
    (*head)->n=0;
}
void SetInt(mTreeNode *p,int a)
{
    p->data=a;
}
int GrowLeave(mTreeNode *start,int newInt)
{
    int index;
    for(index=0;index<start->n;index++)
    {
        if(newInt>(start->next[index])->data) break;
    }
    if(index==5) return 0;//not in
    int j;
    for(j=start->n;j>index;j--)
    {
        if(j==5)
        {
            free(start->next[4]);
            j--;
            start->n--;
        }
        start->next[j]=start->next[j-1];
    }
    start->next[index]=(mTreeNode *)malloc(sizeof(mTreeNode));
    SetInt(start->next[index],newInt);
    (start->n)++;
    return 1;
}
void CutTree(mTreeNode *start,int KeepIndex)
{
    int i;
    for(i=0;i<start->n;i++)
    {
        if(i!=KeepIndex) CutTree(start->next[i],-1);
    }
    free(start);printf("i die\n");
}
