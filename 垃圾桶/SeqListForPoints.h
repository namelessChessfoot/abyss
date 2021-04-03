typedef  struct
{
    int  list[500];
    int  top1,top2;
} SeqListForPoints;
void  ListInitiate(SeqListForPoints  *L)
{
    L->top1=0;
    L->top2=499;
}
int  ListLength(SeqListForPoints  *L)
{
    return  L->top1-L->top2+499;
}
int ListInsert(SeqListForPoints *L,int di,int  x)
{
    int  j;
    if(ListLength(L)>=500) return  0;
    else
    {
        if(di==1)
        {
            L->list[L->top1]=x;
            L->top1++;
            return  1;
        }
        if(di==-1)
        {
            L->list[L->top2]=x;
            L->top2--;
            return  -1;
        }
    }
}
int ListNotHave(SeqListForPoints *L,int di,int x)
{
    if(di==1)
    {
        int i;
        for(i=L->top1==0?499:L->top1-1; i!=L->top2;)
        {
            if(L->list[i]==x) return 0;
            i--;
            if(i==-1) i=499;
        }
        return 1;
    }
    if(di==-1)
    {
        int i;
        for(i=L->top2==499?0:L->top2+1; i!=L->top1;)
        {
            if(L->list[i]==x) return 0;
            i++;
            if(i==500) i=0;
        }
        return 1;
    }
}
