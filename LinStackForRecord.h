typedef struct snode
{
    int data;
    struct snode *next;
} LSNode;
void StackInitiate(LSNode **head)
{
    if((*head=(LSNode *)malloc(sizeof(LSNode)))==NULL) exit(1);
    (* head)->next=NULL;
}
int StackNotEmpty(LSNode *head)
{
    if(head->next==NULL)
        return 0;
    else
        return 1;
}
int StackPush(LSNode *head,int x)
{
    LSNode *p;
    if((p=(LSNode *)malloc(sizeof(LSNode)))==NULL) return 0;//�ڴ�ռ䲻���޷�����
    p->data=x;
    p->next=head->next;
    head->next=p;
    return 1;
}
int StackPop(LSNode *head,int *d)
{
    LSNode *p=head->next;
    if(p==NULL) return 0;//��ջ�ѿճ���
    head->next=p->next;
    *d=p->data;
    free(p);
    return 1;
}
int StackTop(LSNode *head,int *d)
{
    LSNode *p=head->next;
    if(p==NULL) return 0;//��ջ�ѿճ���
    *d=p->data;
    return 1;
}
void Destroy(LSNode *head)
{
    LSNode *p,*q;
    p=head;
    while(p!=NULL)
    {
        q=p;
        p=p->next;
        free(q);
    }
}




