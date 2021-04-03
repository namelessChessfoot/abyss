typedef struct
{
    int move1,move2;
}Package;
typedef struct Node
{
    Package data;
    struct Node *next;
} SLNode;
void PackListInitiate(SLNode **head)
{
    if((*head = (SLNode *)malloc(sizeof(SLNode)))==NULL) exit(1);
    (*head)->next = NULL;
    (*head)->data.move1=0;
}
int PackListInsert(SLNode *head,Package x)
{
    SLNode *q;
    if((q=(SLNode *)malloc(sizeof(SLNode)))==NULL) exit(1);
    q->data=x;
    q->next=head->next;
    head->next=q;
    head->data.move1++;
    return 1;
}
void PackListDestroy(SLNode **head)
{
    SLNode *p,*p1;
    p=*head;
    while(p!=NULL)
    {
        p1=p;
        p=p->next;
        free(p1);
    }
    *head=NULL;
}
