#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#define  len 37
#define  BTime 60
#define  WTime 60
#define  black 'o'
#define  white 'x'
#define  vac '+'
#define  six_ok 1
#define  not_ok 0
#define  GoodAlive 50
#define  NormalAlive 20
#define  Die 0
#define  PositiveInfinity 2000000000
#define  MinusInfinity -2000000000
#define  Width 5
#define  Depth 6
#define  EndOfGame 1//end_tag
#define  Continueing 0//end_tag
#define  HashSize 41017//491 499 503 509 ,991 997 1009 1013, 1999 2003 2011
#include"LinStackForRecord.h"
#include"SeqListForShape.h"
#include"pointset.h"
long long ZobristValue[2*len*len];
typedef struct
{
    int turnx,last[2],haveput,timeblack,timewhite,End_Tag;//回合数
    char fen[len][len],dicount[len][len][8];//表示棋盘以及每个点的“方向个数”
    long long key;
    clock_t start,stop;
    SeqListForShape connect[2][4];//0123 is for c2345
} board;
int FenEqual(char (*p1)[len],char (*p2)[len])///return 1=equal, 0=not equal
{
    int i,n=len*len,tag=1;
    for(i=0; i<n; i++)
    {
        if(*(*(p1+i/len)+i%len)!=*(*(p2+i/len)+i%len))
        {
            tag=0;
            break;
        }
    }
    return tag;
}
void BoardInitiate(board *bp,int blacktime,int whitetime)
{
    bp->turnx=1;
    bp->haveput=0;
    bp->End_Tag=Continueing;
    bp->key=0;
    int i,j,k;
    for(i=0; i<len; i++)
    {
        for(j=0; j<len; j++)
        {
            bp->fen[i][j]=vac;
            for(k=0; k<8; k++)
            {
                bp->dicount[i][j][k]=-1;
            }
        }
    }
    for(i=0; i<4; i++)
    {
        SeqListInitiate(&(bp->connect[1][i]));
        SeqListInitiate(&(bp->connect[0][i]));
    }
    bp->timeblack=blacktime;
    bp->timewhite=whitetime;
    bp->last[0]=-1;
    bp->last[1]=-1;
    bp->start=clock();
}
long long rand64()
{
    return rand()^((long long)rand()<<15)^((long long)rand()<<30)^((long long)rand()<<45)^((long long)rand()<<60);
}
int Evaluate(board *bp)
{
    int score=0;
    int i,j,n;
    for(i=0; i<4; i++)
    {
        for(j=0; j<bp->connect[1][i].size; j++)
        {
            score+=myValue(bp->connect[1][i].list[j],i+2);
        }
        for(j=0; j<bp->connect[0][i].size; j++)
        {
            score-=myValue(bp->connect[0][i].list[j],i+2);
        }
    }
    return score;
}
/*int Evaluate(board *bp)
{
    int score=0;
    int i,j,n;
    for(i=0; i<4; i++)
    {
        for(j=0; j<bp->connect[1][i].size; j++)
        {
            n=StatusScore(&(bp->connect[1][i].list[j]));
            score+=n*(i+2)*(i+2);
        }
        for(j=0; j<bp->connect[0][i].size; j++)
        {
            n=StatusScore(&(bp->connect[0][i].list[j]));
            score-=n*(i+2)*(i+2);
        }
    }
    return score;
}*/
void show(board *bp)
{
    int i,j;
    printf("***WHITE TIME: %d mm***\n",bp->timewhite);
    for(i=0; i<len; i++)
    {
        for(j=0; j<len; j++)
        {
            printf("%c ",bp->fen[i][j]);
        }
        printf("\n");
    }
    printf("***BLACK TIME: %d mm***\nBLACK HAS:\n",bp->timeblack);
    for(i=0; i<4; i++)
    {
        int l=SeqListLength(&(bp->connect[1][i]));
        printf("%d(%d in all): ",i+2,l);
        for(j=0; j<l; j++)
        {
            printf("(%d%c%d)%d<%c%c%hd %hd%c%c> , ",bp->connect[1][i].list[j].Six[0],bp->connect[1][i].list[j].tag,bp->connect[1][i].list[j].Six[1],bp->connect[1][i].list[j].Six[2],bp->connect[1][i].list[j].SmallStatus[1],bp->connect[1][i].list[j].SmallStatus[0],bp->connect[1][i].list[j].p1,bp->connect[1][i].list[j].p2,bp->connect[1][i].list[j].GreatStatus[0],bp->connect[1][i].list[j].GreatStatus[1]);
        }
        printf("\n");
    }
    printf("WHITE HAS: \n");
    for(i=0; i<4; i++)
    {
        int l=SeqListLength(&(bp->connect[0][i]));
        printf("%d(%d in all): ",i+2,l);
        for(j=0; j<l; j++)
        {
            printf("(%d%c%d)%d<%c%c%hd %hd%c%c> , ",bp->connect[0][i].list[j].Six[0],bp->connect[0][i].list[j].tag,bp->connect[0][i].list[j].Six[1],bp->connect[0][i].list[j].Six[2],bp->connect[0][i].list[j].SmallStatus[1],bp->connect[0][i].list[j].SmallStatus[0],bp->connect[0][i].list[j].p1,bp->connect[0][i].list[j].p2,bp->connect[0][i].list[j].GreatStatus[0],bp->connect[0][i].list[j].GreatStatus[1]);
        }
        printf("\n");
    }
    if(bp->turnx%2)
        printf("now it is black to play, has put %d piece\n",bp->haveput);
    else
        printf("now it is white to play, has put %d piece\n",bp->haveput);
    printf("***EVALUATION:%d***\n<<Last 2 Move %d,%d>>\n",Evaluate(bp),bp->last[0],bp->last[1]);
}
int nextpiece(int a,int di)//an easy function to fix the piece which is on the di direction of piece a;
{
    switch(di)
    {
    case 0:
    {
        if(a/len!=0&&a%len!=0)
            return a-len-1;//left-up
        else
            return -1;
    }
    case 1:
    {
        if(a/len!=0)
            return a-len;//up
        else
            return -1;
    }
    case 2:
    {
        if(a/len!=0&&a%len!=len-1)
            return a-len+1;//right-up
        else
            return -1;
    }
    case 3:
    {
        if(a%len!=0)
            return a-1;//left
        else
            return -1;
    }
    case 4:
    {
        if(a%len!=len-1)
            return a+1;//right
        else
            return -1;
    }
    case 5:
    {
        if(a/len!=len-1&&a%len!=0)
            return a+len-1;//left-down
        else
            return -1;
    }
    case 6:
    {
        if(a/len!=len-1)
            return a+len;//down
        else
            return -1;
    }
    case 7:
    {
        if(a/len!=len-1&&a%len!=len-1)
            return a+len+1;//right-down
        else
            return -1;
    }
    }
}
int MultiMove(int a,int di,int n)///multi next piece
{
    int i,k=a;
    for(i=0; i<n; i++)
    {
        k=nextpiece(k,di);
        if(k==-1)
            break;
    }
    return k;
}
int howWhole(board *bp,shaperecord t)
{
    int e1,e2,i,c1,c2;
    shaperecord tt=t;
    int di=FindDirection(&tt);
    char the,enemy;
    c1=c2=-1;
    e1=t.p1;
    e2=t.p2;
    the=bp->fen[e1/len][e1%len];
    enemy=the==black?white:black;
    while(e1!=-1&&bp->fen[e1/len][e1%len]!=enemy)
    {
        e1=nextpiece(e1,di);
        c1++;
    }
    while(e2!=-1&&bp->fen[e2/len][e2%len]!=enemy)
    {
        e2=nextpiece(e2,7-di);
        c2++;
    }
    if(c1+c2+FindLength(&tt)>5)
        return 1;
    else
        return 0;
}
int ShapeAdmin(board *bp,int a)
{
    int n1,n2,di,who,n;
    int np1,np2,dp1,dp2,i,index,npout1,npout2,block1,block2;//np are the points of the new shape, dp are the points of the shape which will be deleted
    //npout are the points out of the new shape, used to determine active or die
    //block are the points of blocked shape
    char the,enemy;
    shaperecord t;
    who=bp->fen[a/len][a%len]==black?1:0;
    the=bp->fen[a/len][a%len];
    enemy=the==black?white:black;
    if(bp->haveput==0)///initiate tags
    {
        for(i=0; i<4; i++)
        {
            for(index=0; index<bp->connect[who][i].size; index++)
                bp->connect[who][i].list[index].tag='b';
        }
    }
    for(di=0; di<4; di++)
    {
        n1=bp->dicount[a/len][a%len][di];
        n2=bp->dicount[a/len][a%len][7-di];
        if(n1+n2>=5)
            return 1;
        if(n1!=0)//on di, exist new shape
        {
            np1=MultiMove(a,di,n1);
            npout1=nextpiece(np1,di);
            if(n1>1)//this shape need to be deleted
            {
                dp1=np1;
                dp2=nextpiece(a,di);
                t.p1=dp1;
                t.p2=dp2;
                index=SeqListFind(&(bp->connect[who][n1-2]),&t);
                SeqListDelete(&(bp->connect[who][n1-2]),index,&t);
            }
        }
        else
        {
            np1=a;
            npout1=block1=nextpiece(a,di);
            if(block1!=-1&&bp->fen[block1/len][block1%len]==enemy)///meet an enemy,empty:0
            {
                n=bp->dicount[block1/len][block1%len][di];
                if(n!=0)
                {
                    block2=MultiMove(block1,di,n);
                    t.p1=block1;
                    t.p2=block2;
                    index=SeqListFind(&(bp->connect[1-who][n-1]),&t);
                    if(bp->connect[1-who][n-1].list[index].p1==block1)
                    {
                        if(bp->connect[1-who][n-1].list[index].GreatStatus[0]=='D')
                            SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                        else
                        {
                            bp->connect[1-who][n-1].list[index].SmallStatus[1]=bp->connect[1-who][n-1].list[index].SmallStatus[0]='D';
                            bp->connect[1-who][n-1].list[index].Six[0]=not_ok;
                            if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                        }
                    }
                    else
                    {
                        if(bp->connect[1-who][n-1].list[index].SmallStatus[0]=='D')
                            SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                        else
                        {
                            bp->connect[1-who][n-1].list[index].GreatStatus[1]=bp->connect[1-who][n-1].list[index].GreatStatus[0]='D';
                            bp->connect[1-who][n-1].list[index].Six[1]=not_ok;
                            if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                        }
                    }
                }
            }
            else if(block1!=-1&&bp->fen[block1/len][block1%len]==vac)//the above step is empty
            {
                block1=nextpiece(block1,di);
                if(block1!=-1&&bp->fen[block1/len][block1%len]!=vac)///empty:1
                {
                    n=bp->dicount[block1/len][block1%len][di];
                    if(bp->fen[block1/len][block1%len]==the&&n>0)//meet a friend shape
                    {
                        block2=MultiMove(block1,di,n);
                        t.p1=block1;
                        t.p2=block2;
                        index=SeqListFind(&(bp->connect[who][n-1]),&t);
                        bp->connect[who][n-1].list[index].tag='s';
                        if(bp->connect[who][n-1].list[index].p1==block1)
                            bp->connect[who][n-1].list[index].SmallStatus[1]='W';
                        else
                            bp->connect[who][n-1].list[index].GreatStatus[1]='W';
                    }
                    else if(bp->fen[block1/len][block1%len]==enemy&&n>0)
                    {
                        block2=MultiMove(block1,di,n);
                        t.p1=block1;
                        t.p2=block2;
                        index=SeqListFind(&(bp->connect[1-who][n-1]),&t);
                        if(bp->connect[1-who][n-1].list[index].p1==block1)
                        {
                            bp->connect[1-who][n-1].list[index].SmallStatus[1]='D';
                            if(n<4)
                            {
                                bp->connect[1-who][n-1].list[index].Six[0]=not_ok;
                                if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                            }
                        }
                        else
                        {
                            bp->connect[1-who][n-1].list[index].GreatStatus[1]='D';
                            if(n<4)
                            {
                                bp->connect[1-who][n-1].list[index].Six[1]=not_ok;
                                if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                            }
                        }
                    }
                }
                else if(block1!=-1&&bp->fen[block1/len][block1%len]==vac)
                {
                    block1=nextpiece(block1,di);
                    if(block1!=-1&&bp->fen[block1/len][block1%len]!=vac)///empty:2
                    {
                        n=bp->dicount[block1/len][block1%len][di];
                        if(bp->fen[block1/len][block1%len]==the&&n>0)
                        {
                            block2=MultiMove(block1,di,n);
                            t.p1=block1;
                            t.p2=block2;
                            index=SeqListFind(&(bp->connect[who][n-1]),&t);
                            bp->connect[who][n-1].list[index].tag='s';
                        }
                        if(bp->fen[block1/len][block1%len]==enemy&&n>0)
                        {
                            block2=MultiMove(block1,di,n);
                            t.p1=block1;
                            t.p2=block2;
                            index=SeqListFind(&(bp->connect[1-who][n-1]),&t);
                            if(bp->connect[1-who][n-1].list[index].p1==block1&&n<3)
                            {
                                bp->connect[1-who][n-1].list[index].Six[0]=not_ok;
                                if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                            }
                            else if(bp->connect[1-who][n-1].list[index].p2==block1&&n<3)
                            {
                                bp->connect[1-who][n-1].list[index].Six[1]=not_ok;
                                if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                            }
                        }
                    }
                    else if(block1!=-1&&bp->fen[block1/len][block1%len]==vac)
                    {
                        block1=nextpiece(block1,di);
                        if(block1!=-1&&bp->fen[block1/len][block1%len]!=vac)///empty:3
                        {
                            n=bp->dicount[block1/len][block1%len][di];
                            if(bp->fen[block1/len][block1%len]==enemy&&n>0)
                            {
                                block2=MultiMove(block1,di,n);
                                t.p1=block1;
                                t.p2=block2;
                                index=SeqListFind(&(bp->connect[1-who][n-1]),&t);
                                if(bp->connect[1-who][n-1].list[index].p1==block1&&n<2)
                                {
                                    bp->connect[1-who][n-1].list[index].Six[0]=not_ok;
                                    if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                                }
                                else if(bp->connect[1-who][n-1].list[index].p2==block1&&n<2)
                                {
                                    bp->connect[1-who][n-1].list[index].Six[1]=not_ok;
                                    if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                                }
                            }
                        }
                    }
                }
            }
        }
        if(n2!=0)//on 7-di, exist new shape
        {
            np2=MultiMove(a,7-di,n2);
            npout2=nextpiece(np2,7-di);
            if(n2>1)//this shape need to be deleted
            {
                dp1=np2;
                dp2=nextpiece(a,7-di);
                t.p1=dp1;
                t.p2=dp2;
                index=SeqListFind(&(bp->connect[who][n2-2]),&t);
                SeqListDelete(&(bp->connect[who][n2-2]),index,&t);
            }
        }
        else
        {
            np2=a;
            npout2=block1=nextpiece(a,7-di);
            if(block1!=-1&&bp->fen[block1/len][block1%len]==enemy)///meet an enemy,empty:0
            {
                n=bp->dicount[block1/len][block1%len][7-di];
                if(n!=0)
                {
                    block2=MultiMove(block1,7-di,n);
                    t.p1=block1;
                    t.p2=block2;
                    index=SeqListFind(&(bp->connect[1-who][n-1]),&t);
                    if(bp->connect[1-who][n-1].list[index].p1==block1)
                    {
                        if(bp->connect[1-who][n-1].list[index].GreatStatus[0]=='D')
                            SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                        else
                        {
                            bp->connect[1-who][n-1].list[index].SmallStatus[1]=bp->connect[1-who][n-1].list[index].SmallStatus[0]='D';
                            bp->connect[1-who][n-1].list[index].Six[0]=not_ok;
                            if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                        }
                    }
                    else
                    {
                        if(bp->connect[1-who][n-1].list[index].SmallStatus[0]=='D')
                            SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                        else
                        {
                            bp->connect[1-who][n-1].list[index].GreatStatus[1]=bp->connect[1-who][n-1].list[index].GreatStatus[0]='D';
                            bp->connect[1-who][n-1].list[index].Six[1]=not_ok;
                            if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                        }
                    }
                }
            }
            else if(block1!=-1&&bp->fen[block1/len][block1%len]==vac)//the above step is empty
            {
                block1=nextpiece(block1,7-di);
                if(block1!=-1&&bp->fen[block1/len][block1%len]!=vac)///empty:1
                {
                    n=bp->dicount[block1/len][block1%len][7-di];
                    if(bp->fen[block1/len][block1%len]==the&&n>0)//meet a friend shape
                    {
                        block2=MultiMove(block1,7-di,n);
                        t.p1=block1;
                        t.p2=block2;
                        index=SeqListFind(&(bp->connect[who][n-1]),&t);
                        bp->connect[who][n-1].list[index].tag='s';
                        if(bp->connect[who][n-1].list[index].p1==block1)
                            bp->connect[who][n-1].list[index].SmallStatus[1]='W';
                        else
                            bp->connect[who][n-1].list[index].GreatStatus[1]='W';
                    }
                    else if(bp->fen[block1/len][block1%len]!=the&&n>0)
                    {
                        block2=MultiMove(block1,7-di,n);
                        t.p1=block1;
                        t.p2=block2;
                        index=SeqListFind(&(bp->connect[1-who][n-1]),&t);
                        if(bp->connect[1-who][n-1].list[index].p1==block1)
                        {
                            bp->connect[1-who][n-1].list[index].SmallStatus[1]='D';
                            if(n<4)
                            {
                                bp->connect[1-who][n-1].list[index].Six[0]=not_ok;
                                if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                            }
                        }
                        else
                        {
                            bp->connect[1-who][n-1].list[index].GreatStatus[1]='D';
                            if(n<4)
                            {
                                bp->connect[1-who][n-1].list[index].Six[1]=not_ok;
                                if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                            }
                        }
                    }
                }
                else if(block1!=-1&&bp->fen[block1/len][block1%len]==vac)
                {
                    block1=nextpiece(block1,7-di);
                    if(block1!=-1&&bp->fen[block1/len][block1%len]!=vac)///empty:2
                    {
                        n=bp->dicount[block1/len][block1%len][7-di];
                        if(bp->fen[block1/len][block1%len]==the&&n>0)
                        {
                            block2=MultiMove(block1,7-di,n);
                            t.p1=block1;
                            t.p2=block2;
                            index=SeqListFind(&(bp->connect[who][n-1]),&t);
                            bp->connect[who][n-1].list[index].tag='s';
                        }
                        if(bp->fen[block1/len][block1%len]==enemy&&n>0)
                        {
                            block2=MultiMove(block1,7-di,n);
                            t.p1=block1;
                            t.p2=block2;
                            index=SeqListFind(&(bp->connect[1-who][n-1]),&t);
                            if(bp->connect[1-who][n-1].list[index].p1==block1&&n<3)
                            {
                                bp->connect[1-who][n-1].list[index].Six[0]=not_ok;
                                if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                            }
                            else if(bp->connect[1-who][n-1].list[index].p2==block1&&n<3)
                            {
                                bp->connect[1-who][n-1].list[index].Six[1]=not_ok;
                                if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                            }
                        }
                    }
                    else if(block1!=-1&&bp->fen[block1/len][block1%len]==vac)
                    {
                        block1=nextpiece(block1,7-di);
                        if(block1!=-1&&bp->fen[block1/len][block1%len]!=vac)///empty:3
                        {
                            n=bp->dicount[block1/len][block1%len][7-di];
                            if(bp->fen[block1/len][block1%len]==enemy&&n>0)
                            {
                                block2=MultiMove(block1,7-di,n);
                                t.p1=block1;
                                t.p2=block2;
                                index=SeqListFind(&(bp->connect[1-who][n-1]),&t);
                                if(bp->connect[1-who][n-1].list[index].p1==block1&&n<2)
                                {
                                    bp->connect[1-who][n-1].list[index].Six[0]=not_ok;
                                    if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                                }
                                else if(bp->connect[1-who][n-1].list[index].p2==block1&&n<2)
                                {
                                    bp->connect[1-who][n-1].list[index].Six[1]=not_ok;
                                    if((bp->connect[1-who][n-1].list[index].Six[2]=howWhole(bp,bp->connect[1-who][n-1].list[index]))==0) SeqListDelete(&(bp->connect[1-who][n-1]),index,&t);
                                }
                            }
                        }
                    }
                }
            }
        }
        if(n1+n2)
        {
            t.p1=np1;
            t.p2=np2;
            if(npout1==-1||bp->fen[npout1/len][npout1%len]!=vac)
                t.SmallStatus[1]=t.SmallStatus[0]='D';
            else
            {
                t.SmallStatus[0]='A';
                n=nextpiece(npout1,di);
                if(n!=-1)
                {
                    if(bp->fen[n/len][n%len]==the)
                        t.SmallStatus[1]='W';
                    else if(bp->fen[n/len][n%len]==vac)
                        t.SmallStatus[1]='A';
                    else
                        t.SmallStatus[1]='D';
                }
                else
                    t.SmallStatus[1]='D';
            }
            if(n1+n2==4)
                t.Six[0]=t.SmallStatus[0]=='A'?six_ok:not_ok;///l=5
            else if(n1+n2==3)
                t.Six[0]=t.SmallStatus[1]=='D'?not_ok:six_ok;///l=4
            else if(n1+n2==2)///l=3
            {
                if(t.SmallStatus[1]=='D')
                    t.Six[0]=not_ok;
                else
                {
                    n=nextpiece(n,di);
                    if(n!=-1)
                    {
                        t.Six[0]=(bp->fen[n/len][n%len]==enemy)?not_ok:six_ok;
                    }
                    else
                        t.Six[0]=not_ok;
                }
            }
            else if(n1+n2==1)///l=2
            {
                if(t.SmallStatus[1]=='D')
                    t.Six[0]=not_ok;
                else
                {
                    n=nextpiece(n,di);
                    if(n==-1)
                        t.Six[0]=not_ok;
                    else
                    {
                        if(bp->fen[n/len][n%len]==enemy)
                            t.Six[0]=not_ok;
                        else
                        {
                            n=nextpiece(n,di);
                            if(n==-1)
                                t.Six[0]=not_ok;
                            else
                            {
                                if(bp->fen[n/len][n%len]==enemy)
                                    t.Six[0]=not_ok;
                                else
                                    t.Six[0]=six_ok;
                            }
                        }
                    }
                }
            }
            if(npout2==-1||bp->fen[npout2/len][npout2%len]!=vac)
                t.GreatStatus[1]=t.GreatStatus[0]='D';
            else
            {
                t.GreatStatus[0]='A';
                n=nextpiece(npout2,7-di);
                if(n!=-1)
                {
                    if(bp->fen[n/len][n%len]==the)
                        t.GreatStatus[1]='W';
                    else if(bp->fen[n/len][n%len]==vac)
                        t.GreatStatus[1]='A';
                    else
                        t.GreatStatus[1]='D';
                }
                else
                    t.GreatStatus[1]='D';
            }
            if(n1+n2==4)
                t.Six[1]=t.GreatStatus[0]=='A'?six_ok:not_ok;///l=5
            else if(n1+n2==3)
                t.Six[1]=t.GreatStatus[1]=='D'?not_ok:six_ok;///l=4
            else if(n1+n2==2)///l=3
            {
                if(t.GreatStatus[1]=='D')
                    t.Six[1]=not_ok;
                else
                {
                    n=nextpiece(n,7-di);
                    if(n!=-1)
                    {
                        t.Six[1]=(bp->fen[n/len][n%len]==enemy)?not_ok:six_ok;
                    }
                    else
                        t.Six[1]=not_ok;
                }
            }
            else if(n1+n2==1)///l=2
            {
                if(t.GreatStatus[1]=='D')
                    t.Six[1]=not_ok;
                else
                {
                    n=nextpiece(n,7-di);
                    if(n==-1)
                        t.Six[1]=not_ok;
                    else
                    {
                        if(bp->fen[n/len][n%len]==enemy)
                            t.Six[1]=not_ok;
                        else
                        {
                            n=nextpiece(n,7-di);
                            if(n==-1)
                                t.Six[1]=not_ok;
                            else
                            {
                                if(bp->fen[n/len][n%len]==enemy)
                                    t.Six[1]=not_ok;
                                else
                                    t.Six[1]=six_ok;
                            }
                        }
                    }
                }
            }
            if(t.Six[0]==1||t.Six[1]==1)
                t.Six[2]=1;
            else
                t.Six[2]=howWhole(bp,t);
            t.tag='n';
            if(t.Six[2])
                SeqListInsert(&(bp->connect[who][n1+n2-1]),&t);
        }
    }
    return 0;
}
void putInfluent(board *bp,int a)//update dicount[][][] when do a move
{
    int di;
    for(di=0; di<8; di++)//for each direction, 01234567 symbolize 8 different directions
    {
        int t;//t is the number of the next piece on that direction
        t=nextpiece(a,di);
        if((t==-1)||(bp->fen[t/len][t%len]!=bp->fen[a/len][a%len]))//reach the edge or different color or vacant
        {
            bp->dicount[a/len][a%len][di]=0;
        }
        else//same color
        {
            bp->dicount[a/len][a%len][di]=1+bp->dicount[t/len][t%len][di];
        }
    }
    for(di=0; di<8; di++) //handle the surroundings
    {
        int p=a,q=a;
        while(bp->dicount[p/len][p%len][di]>0)
        {
            p=nextpiece(q,di);
            bp->dicount[p/len][p%len][7-di]=1+bp->dicount[q/len][q%len][7-di];
            q=p;
        }
    }
}
int putonepiece(board *bp,int a)//put one piece on a, a is no less than 0 and a==-1 means finish this turn)
//return the result: black1 white0 fighting2 error-2 ok3
{
    bp->stop=clock();
    int duration=(int)1000*(bp->stop-bp->start)/CLK_TCK;
    if(bp->turnx%2)
        bp->timeblack-=duration;
    else
        bp->timewhite-=duration;
    bp->start=clock();
    if(a==-1)//try to finish this turn
    {
        if(bp->haveput)//have already put one, can finish
        {
            bp->turnx++;
            bp->last[1]=-1;
            bp->haveput=0;
            return 3;
        }
        else
            return -2;//cannot finish
    }
    int who,tag;
    if(bp->fen[a/len][a%len]!=vac)
        return -2;
    who=(bp->turnx)%2;
    bp->fen[a/len][a%len]=who?black:white;
    bp->key=bp->key^ZobristValue[who*len*len+a];
    putInfluent(bp,a);
    tag=ShapeAdmin(bp,a);
    if(bp->turnx==1)
    {
        bp->turnx++;
        bp->last[bp->haveput]=a;
        return 2;
    }
    if(bp->haveput==0)
        bp->last[1]=-1;
    bp->last[bp->haveput]=a;
    bp->haveput++;
    int di;
    if(bp->haveput==2)
    {
        bp->turnx++;
        bp->haveput=0;
    }
    if(tag==1)
    {
        bp->End_Tag=EndOfGame;
        return who;
    }
    return 2;
}
void GetDoubleClosure(board *bp,PointSet *ps,PointSet *ps1)
{
    int i,m,t1,t2;
    int di;
    PSInitiate(ps);
    if(ps1!=NULL)
        PSInitiate(ps1);
    m=len*len;
    LSNode *p;
    StackInitiate(&p);
    for(i=0; i<m; i++) //scan the whole board
    {
        if(bp->fen[i/len][i%len]!=vac)//if there is a piece
        {
            StackPush(p,i);
            for(di=0; di<8; di++) //let's see its 8 directions
            {
                t1=nextpiece(i,di);
                if((t1!=-1)&&(bp->fen[t1/len][t1%len]==vac))
                {
                    PSInsert(ps,t1);
                }
            }
        }
    }
    if(ps1!=NULL)
        *ps1=*ps;
    while(StackNotEmpty(p))
    {
        StackPop(p,&i);
        for(di=0; di<8; di++)
        {
            t1=nextpiece(i,di);
            if(t1!=-1)
            {
                t2=nextpiece(t1,di);
                if((t2!=-1)&&(bp->fen[t2/len][t2%len]==vac))
                    PSInsert(ps,t2);
                int di2;
                if(di==1||di==2)
                    di2=di-1;
                else if(di==5||di==6)
                    di2=di+1;
                else if(di==3)
                    di2=5;
                else if(di==4)
                    di2=2;
                else if(di==0)
                    di2=3;
                else if(di==7)
                    di2=4;
                t2=nextpiece(t1,di2);
                if((t2!=-1)&&(bp->fen[t2/len][t2%len]==vac))
                    PSInsert(ps,t2);
            }
        }
    }
    Destroy(p);
}
int MMax(int a,int b)
{
    return a>b?a:b;
}
int MMin(int a,int b)
{
    return a<b?a:b;
}
int Win_In_One(board *bp,SLNode *head,int ChangDu,int index,int SaoNe)
{
    int who=bp->turnx%2,bus;
    if(SaoNe==1)
        who=1-who;
    char the=who?black:white;
    Package help;
    if(ChangDu==3)//attack with 5
    {
        Fill_A_Into(&(bp->connect[who][3].list[index]),&(help.move1));
        help.move2=-1;
        PackListInsert(head,help);
        return 5;
    }
    if(ChangDu==2)//attack with 4
    {
        bus=StatusScore(&(bp->connect[who][2].list[index]));
        if(bus>=20)
        {
            if(bus==45)
            {
                Fill_A_Into(&(bp->connect[who][2].list[index]),&(help.move1));
                help.move2=-1;
                PackListInsert(head,help);
                return 4;
            }
            Fill_2A_Into(&(bp->connect[who][2].list[index]),&(help.move1),&(help.move2));
            PackListInsert(head,help);
            return 4;
        }
    }
    if(ChangDu==1) //attack with 3
    {
        bus=StatusScore(&(bp->connect[who][1].list[index]));
        if(bus>=20)//perhaps can win,20 30 40 45 should be tested
        {
            if(bus>=55)//can win
            {
                Fill_2A_Into(&(bp->connect[who][1].list[index]),&(help.move1),&(help.move2));
                PackListInsert(head,help);
                return 3;
            }
            if(bus==30)
            {
                int di=FindMoreADirection(&(bp->connect[who][1].list[index])),t;
                if(di>=4)
                    t=bp->connect[who][1].list[index].p2;
                else
                    t=bp->connect[who][1].list[index].p1;
                help.move1=t=nextpiece(t,di);
                help.move2=t=nextpiece(t,di);
                t=nextpiece(t,di);
                if(t!=-1&&bp->fen[t/len][t%len]==the)//meet a friend beyond 2 steps
                {
                    PackListInsert(head,help);
                    return 3;
                }
            }
            if(bus==40)
            {
                int di=FindDirection(&(bp->connect[who][1].list[index])),t=bp->connect[who][1].list[index].p1;
                help.move1=t=nextpiece(t,di);
                help.move2=t=nextpiece(t,di);
                t=nextpiece(t,di);
                if(t!=-1&&bp->fen[t/len][t%len]==the)//meet a friend beyond 2 steps
                {
                    PackListInsert(head,help);
                    return 3;
                }
                t=bp->connect[who][1].list[index].p2;
                di=7-di;
                help.move1=t=nextpiece(t,di);
                help.move2=t=nextpiece(t,di);
                t=nextpiece(t,7-di);
                if(t!=-1&&bp->fen[t/len][t%len]==the)//meet a friend beyond 2 steps
                {
                    PackListInsert(head,help);
                    return 3;
                }
            }
            if(bus==45)
            {
                int di=FindMoreADirection(&(bp->connect[who][1].list[index])),t;
                if(di>=4)
                    t=bp->connect[who][1].list[index].p2;
                else
                    t=bp->connect[who][1].list[index].p1;
                help.move1=t=nextpiece(t,di);
                t=nextpiece(t,di);//t is the 'W' friend
                if(bp->dicount[t/len][t%len][di]>=1)
                {
                    help.move2=-1;
                    PackListInsert(head,help);
                    return 3;
                }
                t=nextpiece(t,di);
                if(t!=-1&&bp->fen[t/len][t%len]==vac)
                {
                    help.move2=t;
                    PackListInsert(head,help);
                    return 3;
                }
            }
            if(bus==20)
            {
                int di=FindMoreADirection(&(bp->connect[who][1].list[index])),t;
                if(di>=4)
                    t=bp->connect[who][1].list[index].p2;
                else
                    t=bp->connect[who][1].list[index].p1;
                Fill_2A_Into(&(bp->connect[who][1].list[index]),&(help.move1),&(help.move2));
                t=MultiMove(t,di,3);
                if(t!=-1&&bp->fen[t/len][t%len]==the)
                {
                    PackListInsert(head,help);
                    return 3;
                }
            }
        }
    }
    if(ChangDu==0) //attack with 2, 90 can win,DDAA and upper maybe can win
    {
        bus=StatusScore(&(bp->connect[who][0].list[index]));
        if(bus==90)
        {
            Fill_2A_Into(&(bp->connect[who][0].list[index]),&(help.move1),&(help.move2));
            PackListInsert(head,help);
            return 2;
        }
        if(bus==30||(bus==20&&(bp->connect[who][0].list[index].GreatStatus[0]=='D'||bp->connect[who][0].list[index].SmallStatus[0]=='D')))
        {
            int di=FindMoreADirection(&(bp->connect[who][0].list[index])),t;
            if(di>=4)
                t=bp->connect[who][0].list[index].p2;
            else
                t=bp->connect[who][0].list[index].p1;
            help.move1=t=nextpiece(t,di);
            help.move2=t=nextpiece(t,di);
            t=nextpiece(t,di);
            if(t!=-1&&bp->fen[t/len][t%len]==the&&bp->dicount[t/len][t%len][di]>=1)
            {
                PackListInsert(head,help);
                return 2;
            }
        }
        if(bus==40)
        {
            int di=FindDirection(&(bp->connect[who][0].list[index])),t=bp->connect[who][0].list[index].p1;
            help.move1=t=nextpiece(t,di);
            help.move2=t=nextpiece(t,di);
            t=nextpiece(t,di);
            if(t!=-1&&bp->fen[t/len][t%len]==the&&bp->dicount[t/len][t%len][di]>=1)
            {
                PackListInsert(head,help);
                return 2;
            }
            t=bp->connect[who][0].list[index].p2;
            di=7-di;
            help.move1=t=nextpiece(t,di);
            help.move2=t=nextpiece(t,di);
            t=nextpiece(t,di);
            if(t!=-1&&bp->fen[t/len][t%len]==the&&bp->dicount[t/len][t%len][di]>=1)
            {
                PackListInsert(head,help);
                return 2;
            }
        }
        if(bus==45)
        {
            int di=FindMoreADirection(&(bp->connect[who][0].list[index])),t;
            if(di>=4)
                t=bp->connect[who][0].list[index].p2;
            else
                t=bp->connect[who][0].list[index].p1;
            help.move1=t=nextpiece(t,di);
            t=nextpiece(t,di);//t reach 'W'
            int n=bp->dicount[t/len][t%len][di];
            if(n>=2)
            {
                help.move2=-1;
                PackListInsert(head,help);
                return 2;
            }
            else if(n==1)
            {
                t=MultiMove(t,di,2);
                if(t!=-1&&bp->fen[t/len][t%len]==vac)
                {
                    help.move2=t;
                    PackListInsert(head,help);
                    return 2;
                }
            }
            else if(n==0)
            {
                t=nextpiece(t,di);
                if(t!=-1&&bp->fen[t/len][t%len]==vac)
                {
                    help.move2=t;
                    t=nextpiece(t,di);
                    if(t!=-1&&bp->fen[t/len][t%len]==the)
                    {
                        PackListInsert(head,help);
                        return 2;
                    }
                }
            }
        }
        if(bus==55)
        {
            int di=FindMoreADirection(&(bp->connect[who][0].list[index])),t;
            if(di>=4)
                t=bp->connect[who][0].list[index].p2;
            else
                t=bp->connect[who][0].list[index].p1;
            help.move1=t=nextpiece(t,di);
            t=nextpiece(t,di);
            int n=bp->dicount[t/len][t%len][di];
            if(n>=1)
            {
                Fill_2A_Into(&(bp->connect[who][0].list[index]),&(help.move1),&(help.move2));
                PackListInsert(head,help);
                return 2;
            }
            else if(n==0)
            {
                t=nextpiece(t,di);
                if(t!=-1&&bp->fen[t/len][t%len]==vac)
                {
                    help.move2=t;
                    t=nextpiece(t,di);
                    if(t!=-1&&bp->fen[t/len][t%len]==the)
                    {
                        PackListInsert(head,help);
                        return 2;
                    }
                }
            }
        }
        if(bus==65)
        {
            int di=FindMoreADirection(&(bp->connect[who][0].list[index])),t;
            if(di>=4)
                t=bp->connect[who][0].list[index].p2;
            else
                t=bp->connect[who][0].list[index].p1;
            help.move1=t=nextpiece(t,di);
            t=nextpiece(t,di);
            int n=bp->dicount[t/len][t%len][di];
            if(n>=1)
            {
                Fill_2A_Into(&(bp->connect[who][0].list[index]),&(help.move1),&(help.move2));
                PackListInsert(head,help);
                return 2;
            }
            else if(n==0)
            {
                t=nextpiece(t,di);
                if(t!=-1&&bp->fen[t/len][t%len]==vac)
                {
                    help.move2=t;
                    t=nextpiece(t,di);
                    if(t!=-1&&bp->fen[t/len][t%len]==the)
                    {
                        PackListInsert(head,help);
                        return 2;
                    }
                }
            }
            di=7-di;
            if(di>=4)
                t=bp->connect[who][0].list[index].p2;
            else
                t=bp->connect[who][0].list[index].p1;
            help.move1=t=nextpiece(t,di);
            help.move2=t=nextpiece(t,di);
            t=nextpiece(t,di);
            if(t!=-1&&bp->fen[t/len][t%len]==the&&bp->dicount[t/len][t%len][di]>=1)
            {
                PackListInsert(head,help);
                return 2;
            }
        }
    }
    return 0;
}
int Defense(board *bp,SLNode *head)///no_killer=-1,i_lose=-2,mustDefenseWith2=2,can?With1=1;
{
    int susp[20],n=0,i,j,k,who=bp->turnx%2,inlist=0,xkmnb;
    SeqListForShape killer;
    SeqListInitiate(&killer);
    SLNode *slnp;
    PackListInitiate(&slnp);
    for(i=3; i>-1; i--)//find suspicions
    {
        for(j=0; j<bp->connect[1-who][i].size; j++)
        {
            inlist=0;
            if(bp->connect[1-who][i].list[j].tag!='b'&&n<20)
            {
                int t=i+10*j;
                for(k=0; k<n; k++)
                {
                    if(t==susp[k])
                    {
                        inlist=1;
                        break;
                    }
                }
                if(inlist==0)
                {
                    susp[n]=t;
                    n++;
                }
            }
        }
    }
    for(i=0; i<n; i++)//find killers
    {
        xkmnb=Win_In_One(bp,slnp,susp[i]%10,susp[i]/10,1);
        if(xkmnb>0&&SeqListFind(&killer,&(bp->connect[1-who][susp[i]%10].list[susp[i]/10]))==-1)
            SeqListInsert(&killer,&(bp->connect[1-who][susp[i]%10].list[susp[i]/10]));
    }
    if(killer.size==0)// no killer
    {
        PackListDestroy(&slnp);
        return -1;
    }
    //exist killer
    PointSet ps;
    int *asdfgh;//dynamic int[killer.size][4]
    asdfgh=(int *)malloc(killer.size*4*sizeof(int));
    for(i=0; i<killer.size; i++)
    {
        for(j=0; j<4; j++)
        {
            *(asdfgh+4*i+j)=-1;
        }
    }
    for(i=0; i<killer.size; i++)
    {
        int l,index;
        PSInitiate(&ps);
        shaperecord *p;
        p=&(killer.list[i]);
        int di=FindDirection(p),scout;
        l=FindLength(p);//
        index=SeqListFind(&(bp->connect[1-who][l-2]),p);//
        scout=smaller_Point(p);///<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        if(p->SmallStatus[0]=='A')
        {
            scout=nextpiece(scout,di);
            PSInsert(&ps,scout);
            if(p->SmallStatus[1]=='A')
            {
                scout=nextpiece(scout,di);
                PSInsert(&ps,scout);
            }
            else if(p->SmallStatus[1]=='W')
            {
                scout=nextpiece(scout,di);
                int n=bp->dicount[scout/len][scout%len][di];
                scout=MultiMove(scout,di,n+1);
                if(scout!=-1&&bp->fen[scout/len][scout%len]==vac)
                {
                    PSInsert(&ps,scout);
                }
            }
        }
        scout=greater_Point(p);
        di=7-di;
        if(p->GreatStatus[0]=='A')
        {
            scout=nextpiece(scout,di);
            PSInsert(&ps,scout);
            if(p->GreatStatus[1]=='A')
            {
                scout=nextpiece(scout,di);
                PSInsert(&ps,scout);
            }
            else if(p->GreatStatus[1]=='W')
            {
                scout=nextpiece(scout,di);
                int n=bp->dicount[scout/len][scout%len][di];
                scout=MultiMove(scout,di,n+1);
                if(scout!=-1&&bp->fen[scout/len][scout%len]==vac)
                {
                    PSInsert(&ps,scout);
                }
            }
        }///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        int DanDu=0;
        for(j=0; j<ps.num; j++) //try to solve with 1 move
        {
            board newb;
            newb=*bp;
            putonepiece(&newb,ps.point[j]);
            if(Win_In_One(&newb,slnp,l-2,index,1)==0||(SeqListFind(&(newb.connect[1-newb.turnx%2][l-2]),&(killer.list[i]))==-1))
            {
                *(asdfgh+4*i+DanDu)=ps.point[j];
                DanDu++;
            }
        }///ha ha ha
        if(DanDu==0)//must use 2, which I like
        {
            Package *pack,*newpack;
            int ss=ps.num*(ps.num-1)/2,note[12]= {0},SVCounter=0;
            pack=(Package *)malloc(ss*sizeof(Package));
            PSCombine(ps,ps,pack);
            for(j=0; j<ss; j++)
            {
                board newb;
                newb=*bp;
                putonepiece(&newb,(pack+j)->move1);
                putonepiece(&newb,(pack+j)->move2);
                if(Win_In_One(&newb,slnp,l-2,index,0)==0||(SeqListFind(&(newb.connect[newb.turnx%2][l-2]),&(killer.list[i]))==-1))
                {
                    note[j]=1;
                    SVCounter++;
                }
            }
            newpack=(Package *)malloc(SVCounter*sizeof(Package));
            int ii=0;
            for(j=0; j<ss; j++)
            {
                if(note[j]==1)
                {
                    *(newpack+ii)=*(pack+j);
                    ii++;
                }
            }
            for(j=0; j<SVCounter; j++)
            {
                board newb;
                newb=*bp;
                putonepiece(&newb,(newpack+j)->move1);
                putonepiece(&newb,(newpack+j)->move2);
                int ij,ji,tt=0;
                for(ij=3; ij>-1; ij--)
                {
                    tt=0;
                    for(ji=0; ji<newb.connect[who][ij].size; ji++)
                    {
                        if(newb.connect[who][ij].list[ji].tag!='b')
                        {
                            xkmnb=Win_In_One(&newb,slnp,ij,ji,0);
                            if(xkmnb>0)
                            {
                                tt=1;
                                break;
                            }
                        }
                    }
                    if(tt==1)
                        break;
                }
                if(tt==0)
                {
                    PackListInsert(head,*(newpack+j));
                }
            }
            if(head->data.move1==0)//i lose
            {
                PackListInsert(head,*(newpack));
                free(pack);
                free(newpack);
                PackListDestroy(&slnp);
                free(asdfgh);
                return -2;
            }
            free(pack);
            free(newpack);
            PackListDestroy(&slnp);
            free(asdfgh);
            return 2;
        }
    }
    ///all killers can be solve with one piece
    PointSet *xkmtnbl;
    xkmtnbl=(PointSet *)malloc(killer.size*sizeof(PointSet));
    for(i=0; i<killer.size; i++)
    {
        PSInitiate(xkmtnbl+i);
        for(j=0; j<4; j++)
        {
            if(*(asdfgh+4*i+j)!=-1)
            {
                PSInsert(xkmtnbl+i,*(asdfgh+4*i+j));
            }
            else
                break;
        }
    }
    PointSet rbq;
    rbq=*(xkmtnbl);
    for(i=1; i<killer.size; i++)
    {
        PSIntersect(rbq,*(xkmtnbl+i),&rbq);
        if(xkmtnbl->num==0)
            break;
    }
    if(rbq.num>0)//one piece solve all problem
    {
        Package help;
        for(i=0; i<rbq.num; i++)
        {
            help.move1=rbq.point[i];
            help.move2=-1;
            PackListInsert(head,help);
        }
    }
    int *ddl,ddc=rbq.num;
    ddl=(int *)malloc(ddc*sizeof(int));
    for(i=0; i<ddc; i++)
    {
        *(ddl+i)=rbq.point[i];
    }
    rbq=*(xkmtnbl);
    for(i=1; i<killer.size; i++)
    {
        PSMerge(rbq,*(xkmtnbl+i),&rbq);
    }
    PointSet arbq;
    arbq=rbq;
    PSInitiate(&rbq);
    for(i=0; i<arbq.num; i++)
    {
        int taag=0;
        for(j=0; j<ddc; j++)
        {
            if(arbq.point[i]==*(ddl+j))
            {
                taag=1;
                break;
            }
        }
        if(taag==0)
            PSInsert(&rbq,arbq.point[i]);
    }
    Package *nbrbq;
    int rrsize=rbq.num*(rbq.num-1)/2;
    nbrbq=(Package *)malloc(rrsize*sizeof(Package));
    PSCombine(rbq,rbq,nbrbq);
    for(i=0; i<rrsize; i++)
    {
        board newb;
        newb=*bp;
        int hhh=1,k;
        putonepiece(&newb,(nbrbq+i)->move1);
        putonepiece(&newb,(nbrbq+i)->move2);
        for(j=0; j<killer.size; j++)
        {
            int l,index;
            l=FindLength(&(killer.list[j]));//
            index=SeqListFind(&(newb.connect[1-who][l-2]),&(killer.list[j]));
            if(SeqListFind(&(newb.connect[newb.turnx%2][l-2]),&(killer.list[j]))!=-1&&Win_In_One(&newb,slnp,l-2,index,0)!=0)
            {
                hhh=0;
                break;
            }
        }
        if(hhh)
        {
            Package help;
            help.move1=(nbrbq+i)->move1;
            help.move2=(nbrbq+i)->move2;
            PackListInsert(head,help);
        }
    }
    if(head->data.move1==0)
    {
        Package help;
        help.move1=nbrbq->move1;
        help.move2=nbrbq->move2;
        PackListInsert(head,help);
        PackListDestroy(&slnp);
        free(asdfgh);
        free(xkmtnbl);
        free(nbrbq);
        free(ddl);
        return -2;
    }
    PackListDestroy(&slnp);
    free(asdfgh);
    free(xkmtnbl);
    free(nbrbq);
    free(ddl);
    return 1;
}
int GenerateMoves(board *bp,SLNode **head)///win=PositiveInfinity,lose=-2,defend_ok=-10 or -20,or num of attack,no vac=666
{
    PackListInitiate(head);
    SLNode *raw;
    int who=bp->turnx%2,i,j,BiDef[2],tag=0,xkmnb;
    char the=who?black:white;
    for(i=3; i>-1; i--)
    {
        for(j=0; j<bp->connect[who][i].size; j++)
        {
            if(bp->connect[who][i].list[j].tag!='b')
            {
                xkmnb=Win_In_One(bp,*head,i,j,0);
                if(xkmnb>0)
                    return PositiveInfinity;
            }
        }
    }
    //@@@defense@@@//
    PackListInitiate(&raw);
    xkmnb=Defense(bp,raw);
    if(xkmnb==-2)//lose
    {
        PackListInsert(*head,raw->next->data);
        PackListDestroy(&raw);
        return -2;
    }
    else if(xkmnb==2)//defense with 2
    {
        SLNode *hhh;
        hhh=raw->next;
        for(i=0; i<raw->data.move1; i++)
        {
            PackListInsert(*head,hhh->data);
            hhh=hhh->next;
        }
        PackListDestroy(&raw);
        return -20;
    }
    else if(xkmnb==1)//defense with 1 0r 2
    {
        SLNode *hhh;
        hhh=raw->next;
        int ai=raw->data.move1;
        for(i=0; i<ai; i++)
        {
            if(hhh->data.move2==-1)
            {
                PointSet yo,ya;
                PSInitiate(&yo);
                PSInitiate(&ya);
                board newb;
                newb=*bp;
                putonepiece(&newb,hhh->data.move1);
                GetDoubleClosure(&newb,&yo,NULL);
                PSInsert(&ya,hhh->data.move1);
                Package *pack;
                pack=(Package *)malloc(yo.num*sizeof(Package));
                PSCombine(ya,yo,pack);
                for(j=0; j<yo.num; j++)
                    PackListInsert(*head,*(pack+j));
                free(pack);
            }
            else
            {
                PackListInsert(*head,hhh->data);
            }
            hhh=hhh->next;
        }
        PackListDestroy(&raw);
        return -10;
    }
    PackListDestroy(&raw);
    //no killer!!!
    PointSet aha,ahe;
    GetDoubleClosure(bp,&aha,&ahe);
    Package *pack;
    int all;
    if(aha.num==0)
        return 666;
    all=ahe.num*(ahe.num-1)/2+ahe.num*(aha.num-ahe.num);
    pack=(Package *)malloc(all*sizeof(Package));
    PSCombine(ahe,aha,pack);
    for(i=0; i<all; i++)
    {
        PackListInsert(*head,*(pack+i));
    }
    free(pack);
    return 0;
}
typedef struct
{
    int deep,value;
} Deep_and_Value;
typedef struct treeNode
{
    int NumOfLeaves;
    Deep_and_Value dv;
    board now;
    struct treeNode *next[Width];
} mTreeNode;
typedef struct
{
    int turnx;
    long long key;
    char fen[len][len];
} ConciseBoard;
typedef struct node
{
    ConciseBoard CB;
    Deep_and_Value dv;
    struct node *next;
} HashNode;
void CBInitiate(board *bp,ConciseBoard *cbp)
{
    cbp->turnx=bp->turnx;
    cbp->key=bp->key;
    int i,n=len*len;
    for(i=0; i<n; i++)
    {
        cbp->fen[i/len][i%len]=bp->fen[i/len][i%len];
    }
}
void HashNodeInitiate(mTreeNode *mtnp,HashNode **hnp)
{
    if((*hnp=(HashNode *)malloc(sizeof(HashNode)))==NULL)
        exit(1);
    CBInitiate(&(mtnp->now),&((*hnp)->CB));
    (*hnp)->dv.deep=mtnp->dv.deep;
    (*hnp)->dv.value=mtnp->dv.value;
    (*hnp)->next=NULL;
}
void CreateHashList(HashNode **HashHeads)
{
    if((*HashHeads=(HashNode *)malloc(Width*HashSize*sizeof(HashNode)))==NULL)
        exit(1);
    int i;
    for(i=0; i<HashSize; i++)
    {
        (*HashHeads+i)->next=NULL;
    }
}
int HashFunc(mTreeNode *mtnp)
{
    int n=mtnp->now.key%HashSize;
    while(n<0)
        n=n+HashSize;
    return n;
}
HashNode *InHashList(mTreeNode *mtnp,HashNode *HashHeads)
{
    int index=HashFunc(mtnp);
    HashNode *p;
    p=(HashHeads+index)->next;
    while(p!=NULL)
    {
        if(p->CB.key==mtnp->now.key&&p->CB.turnx%2==mtnp->now.turnx%2&&FenEqual(p->CB.fen,mtnp->now.fen))
            return p;
        p=p->next;
    }
    return NULL;
}
int HashInsert(mTreeNode *mtnp,HashNode *HashHeads)
{
    int index=HashFunc(mtnp);
    HashNode *n;
    HashNodeInitiate(mtnp,&n);
    n->next=(HashHeads+index)->next;
    (HashHeads+index)->next=n;
    return 0;
}
void HashClear(HashNode *HashHeads)
{
    int i;
    HashNode *p,*r;
    for(i=0; i<HashSize*Width; i++)
    {
        p=(HashHeads+i)->next;
        while(p!=NULL)
        {
            r=p->next;
            free(p);
            p=r;
        }
        (HashHeads+i)->next=NULL;
    }
}
void TreeNodeInitiate(mTreeNode **head,board *bp,Package pp)
{
    int xkmnb666;
    if(((*head)=(mTreeNode *)malloc(sizeof(mTreeNode)))==NULL)
        exit(1);
    int i;
    for(i=0; i<Width; i++)
    {
        (*head)->next[i]=NULL;
    }
    (*head)->NumOfLeaves=0;
    (*head)->now=(*bp);
    if(pp.move1==-1)
    {
        (*head)->dv.value=Evaluate(&((*head)->now));
        (*head)->dv.deep=0;
        return;
    }
    xkmnb666=putonepiece(&((*head)->now),pp.move1);
    (*head)->dv.deep=PositiveInfinity;
    if(xkmnb666==1)
        (*head)->dv.value=PositiveInfinity;
    else if(xkmnb666==0)
        (*head)->dv.value=MinusInfinity;
    xkmnb666=putonepiece(&((*head)->now),pp.move2);
    if(xkmnb666==1)
        (*head)->dv.value=PositiveInfinity;
    else if(xkmnb666==0)
        (*head)->dv.value=MinusInfinity;
    else
    {
        (*head)->dv.value=Evaluate(&((*head)->now));
        (*head)->dv.deep=0;
    }
}
int GrowLeave(mTreeNode *start,Package pp)
{
    mTreeNode *leave;
    TreeNodeInitiate(&leave,&(start->now),pp);
    int index,who=start->now.turnx%2;
    if(who==1)
    {
        for(index=0; index<start->NumOfLeaves; index++)
        {
            if((leave->dv.value)>(start->next[index]->dv.value))
                break;
        }
    }
    else if(who==0)
    {
        for(index=0; index<start->NumOfLeaves; index++)
        {
            if((leave->dv.value)<(start->next[index]->dv.value))
                break;
        }
    }
    if(index==Width)
    {
        free(leave);
        return 0;//not in
    }
    int j;
    for(j=start->NumOfLeaves; j>index; j--)
    {
        if(j==Width)
        {
            free(start->next[Width-1]);
            j--;
            (start->NumOfLeaves)--;
        }
        start->next[j]=start->next[j-1];
    }
    start->next[index]=leave;
    (start->NumOfLeaves)++;
    return 1;
}
mTreeNode* CutTree(mTreeNode *start,int KeepIndex)
{
    int i;
    mTreeNode *pppp=NULL;
    if(KeepIndex!=-1)
    {
        pppp=start->next[KeepIndex];
    }
    for(i=0; i<start->NumOfLeaves; i++)
    {
        if(i!=KeepIndex)
            CutTree(start->next[i],-1);
    }
    free(start);
    return pppp;
}
int AABB(mTreeNode *tn,int dd,int alpha,int beta,int *non,HashNode *HashHeads,int ThreadID,int cowork)
{
    int who=tn->now.turnx%2,aa,bb,i;
    HashNode *p,*start;
    if(dd==0||tn->now.End_Tag==EndOfGame)
    {
        ///HashInsert(tn,HashHeads);
        return tn->dv.value;
    }
    start=HashHeads-ThreadID*HashSize;
    if((p=InHashList(tn,HashHeads))!=NULL&&p->dv.deep==dd)
    {
        return p->dv.value;
    }
    else
    {
        for(i=0; i<cowork; i++)
        {
            if(i!=ThreadID)
            {
                p=InHashList(tn,start+HashSize*i);
                if(p!=NULL&&p->dv.deep==dd)
                {
                    return p->dv.value;
                }
            }
        }
    }
    aa=alpha;
    bb=beta;
    if(who==1)
    {
        SLNode *head,*ttt;
        int g,i;
        g=GenerateMoves(&(tn->now),&head);
        if(g==666)
            return 0;
        ttt=head->next;
        for(i=0; i<head->data.move1; i++)
        {
            GrowLeave(tn,ttt->data);
            ttt=ttt->next;
        }
        *non+=tn->NumOfLeaves;
        PackListDestroy(&head);
        for(i=0; i<tn->NumOfLeaves; i++)
        {
            aa=MMax(aa,AABB(tn->next[i],dd-1,aa,bb,non,HashHeads,ThreadID,cowork));
            if(bb<=aa)
                break;
        }
        tn->dv.deep=dd;
        tn->dv.value=aa;
        HashInsert(tn,HashHeads);
        return aa;
    }
    else if(who==0)
    {
        SLNode *head,*ttt;
        int g,i;
        g=GenerateMoves(&(tn->now),&head);
        if(g==666)
            return 0;
        ttt=head->next;
        for(i=0; i<head->data.move1; i++)
        {
            GrowLeave(tn,ttt->data);
            ttt=ttt->next;
        }
        *non+=tn->NumOfLeaves;
        PackListDestroy(&head);
        for(i=0; i<tn->NumOfLeaves; i++)
        {
            bb=MMin(bb,AABB(tn->next[i],dd-1,aa,bb,non,HashHeads,ThreadID,cowork));
            if(bb<=aa)
                break;
        }
        tn->dv.deep=dd;
        tn->dv.value=bb;
        HashInsert(tn,HashHeads);
        return bb;
    }
}
typedef struct
{
    mTreeNode *tn;
    int dd,TemporaryCounter,MyThreadID,retValue,coWork;
    HashNode *HashHeads;
} ArgsForAB;
void *ABHelper(void *args)
{
    ArgsForAB *p;
    p=(ArgsForAB *)args;
    p->TemporaryCounter=0;
    p->retValue=AABB(p->tn,p->dd,MinusInfinity,PositiveInfinity,&(p->TemporaryCounter),p->HashHeads+p->MyThreadID*HashSize,p->MyThreadID,p->coWork);
    return NULL;
}
Package SearchTogether(mTreeNode **tn,int waitwho,board *bp,int MyDepth,HashNode *HashHeads,int *non,int *tag)
{
    int d,TemporaryCounter=0,i,j,ABindex,n,bbb;
    pthread_t *thread;
    Package MyChoices,aya;
    ArgsForAB *args;
    aya.move1=aya.move2=-1;
    if(MyDepth==-1)
        d=Depth;
    else
        d=MyDepth;
    SLNode *head,*pp;
    GenerateMoves(bp,&head);
    j=head->data.move1;
    pp=head->next;
    if(j!=1)
    {
        TreeNodeInitiate(tn,bp,aya);
        for(i=0; i<j; i++)
        {
            GrowLeave(*tn,pp->data);
            pp=pp->next;
        }
        n=(*tn)->NumOfLeaves;
        if((thread=(pthread_t *)malloc(n*sizeof(pthread_t)))==NULL)
            exit(1);
        if((args=(ArgsForAB *)malloc(n*sizeof(ArgsForAB)))==NULL)
            exit(1);
        for(i=0; i<n; i++)
        {
            (args+i)->dd=d;
            (args+i)->MyThreadID=i;
            (args+i)->tn=(*tn)->next[i];
            (args+i)->HashHeads=HashHeads;
            (args+i)->coWork=n;
            pthread_create(thread+i,NULL,ABHelper,(void *)(args+i));
        }
        for(i=0; i<n; i++)
        {
            pthread_join(*(thread+i),NULL);
        }
        bbb=0;
        for(i=0; i<n; i++)
        {
            if(waitwho==1&&(args+bbb)->retValue>(args+i)->retValue)
                bbb=i;
            if(waitwho==0&&(args+bbb)->retValue<(args+i)->retValue)
                bbb=i;
        }
        MyChoices.move1=(*tn)->next[bbb]->now.last[0];
        MyChoices.move2=(*tn)->next[bbb]->now.last[1];
        *tag=1;
        *non=0;
        for(i=0; i<n; i++)
        {
            *non=*non+(args+i)->TemporaryCounter;
        }
        free(thread);
        free(args);
    }
    else
    {
        MyChoices.move1=pp->data.move1;
        MyChoices.move2=pp->data.move2;
        *tag=0;
        *non=0;
    }
    PackListDestroy(&head);
    return MyChoices;
}
