#define  GoodAlive 50
#define  NormalAlive 20
#define  Die 0
#define  bonus4 3
#define  bonus5 4
int nextpiece(int a,int di);
typedef struct
{
    int p1,p2,Six[3],threats;
    char SmallStatus[2],GreatStatus[2],tag;//A=ACTIVE D=DIE W=WOW, tag is {n=new s=suspicion b=boring}
} shaperecord;
int sod(char c)
{
    if(c=='W')
        return 35;
    if(c=='A')
        return 10;
    else
        return 0;
}
int powforint(int x,int y)
{
    int i,j=1;
    for(i=0;i<y;i++)
    {
        j=j*x;
    }
    return j;
}
int myValue(shaperecord p,int l)
{
    int i=0,F,n,bonus;
    if(l==4) bonus=bonus4;
    else if(l==5) bonus=bonus5;
    if(p.GreatStatus[1]=='W')
        i++;
    if(p.SmallStatus[1]=='W')
        i++;
    n=p.Six[0]+p.Six[1]+p.Six[2];
    if(n==0) F=Die;
    else if(n==3) F=GoodAlive;
    else F=NormalAlive;
    return F*((l-1)*powforint(2,i)+l+bonus);
}
int FindDirection(shaperecord *p)
{
    int a,b,x1,x2,y1,y2;
    a=p->p1;
    b=p->p2;
    x1=a/len;
    y1=a%len;
    x2=b/len;
    y2=b%len;
    if(x1==x2)
        return 3;
    if(y1==y2)
        return 1;
    if((x1-x2)*(y1-y2)>0)
        return 0;
    return 2;
}
int FindLength(shaperecord *p)
{
    int di=FindDirection(p);
    int n=1,a=p->p1,b=p->p2;
    if(a>b)
        di=7-di;
    while(a!=b)
    {
        b=nextpiece(b,di);
        n++;
    }
    return n;
}
int smaller_Point(shaperecord *p)
{
    if(p->p1<p->p2)
        return p->p1;
    return p->p2;
}
int greater_Point(shaperecord *p)
{
    if(p->p1>p->p2)
        return p->p1;
    return p->p2;
}
int FindMoreADirection(shaperecord *p)//can deal with 20 30 45 55 65
{
    if(p->SmallStatus[0]=='D')
        return 7-FindDirection(p);
    if(p->GreatStatus[0]=='D')
        return FindDirection(p);
    if(p->SmallStatus[1]=='D')
        return 7-FindDirection(p);
    if(p->GreatStatus[1]=='D')
        return FindDirection(p);
    if(p->SmallStatus[1]=='A')
        return 7-FindDirection(p);
    if(p->GreatStatus[1]=='A')
        return FindDirection(p);
}
int StatusScore(shaperecord *p)
/*
WA AW 90
AA AW 65***
DA AW 55***
DD AW 45***
AA AA 40
DA AA 30***
DD AA 20***
DA AD 20
DD AD 10
DD DD 0
*/
{
    return sod(p->GreatStatus[1])+sod(p->GreatStatus[0])+sod(p->SmallStatus[0])+sod(p->SmallStatus[1]);
}
void Fill_A_Into(shaperecord *p,int *a)
{
    int di=FindDirection(p);
    if(p->SmallStatus[0]=='D')
        *a=nextpiece(p->p2,7-di);
    else *a=nextpiece(p->p1,di);
}
void Fill_2A_Into(shaperecord *p,int *a,int *b)
{
    int di=FindDirection(p);
    int n;
    if(p->SmallStatus[0]=='D')
    {
        *a=n=nextpiece(p->p2,7-di);
        *b=nextpiece(n,7-di);
        return;
    }
    if(p->GreatStatus[0]=='D')
    {
        *a=n=nextpiece(p->p1,di);
        *b=nextpiece(n,di);
        return;
    }
    *a=nextpiece(p->p1,di);
    *b=nextpiece(p->p2,7-di);
}
void Fill_2smallA_Into(shaperecord *p,int *a,int *b)
{
    int di=FindDirection(p);
    *a=nextpiece(p->p1,di);
    *b=nextpiece(*a,di);
}
void Fill_2GREATA_Into(shaperecord *p,int *a,int *b)
{
    int di=FindDirection(p);
    *a=nextpiece(p->p2,7-di);
    *b=nextpiece(*a,7-di);
}
typedef struct
{
    shaperecord  list[100];
    int  size;
} SeqListForShape;
void  SeqListInitiate(SeqListForShape  *L)
{
    L->size=0;
}
int SeqListLength(SeqListForShape  *L)
{
    return  L->size;
}
int SeqListInsert(SeqListForShape  *L,shaperecord  *x)
{
    int j;
    if(L->size>=100)
    {
        return  0;
    }
    else
    {
        if(x->p1<x->p2)
        {
            L->list[L->size]=*x;
            L->size++;
            return  1;
        }
        else
        {
            shaperecord nn;
            nn.GreatStatus[1]=x->SmallStatus[1];
            nn.GreatStatus[0]=x->SmallStatus[0];
            nn.SmallStatus[0]=x->GreatStatus[0];
            nn.SmallStatus[1]=x->GreatStatus[1];
            nn.p1=x->p2;
            nn.p2=x->p1;
            L->list[L->size]=nn;
            L->size++;
            return  1;
        }
    }
}
int SeqListDelete(SeqListForShape  *L,int  i,shaperecord *x)
{
    int j;
    if(L->size<=0)
    {
        return  0;
    }
    else if(i<0 || i>L->size-1)
    {
        return  0;
    }
    else
    {
        *x =L->list[i];
        for(j=i+1; j<=L->size-1; j++)
            L->list[j-1]=L->list[j];
        L->size--;
        return  1;
    }
}
int SeqListFind(SeqListForShape *L,shaperecord  *x)
{
    int i;
    for(i=0; i<L->size; i++)
    {
        if(((x->p1==L->list[i].p1)&&(x->p2==L->list[i].p2))||((x->p2==L->list[i].p1)&&(x->p1==L->list[i].p2)))
            return i;
    }
    return -1;
}
int SeqListGet(SeqListForShape *L,int  i,shaperecord  *x)
{
    if(L->size<=0)
    {
        return  0;
    }
    else if(i<0 || i>L->size-1)
    {
        return  0;
    }
    else
    {
        *x =L->list[i];
        return  1;
    }
}

