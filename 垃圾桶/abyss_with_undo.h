#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<time.h>
#define  len (char)9
#define  BTime 60
#define  WTime 60
#define  black 'o'
#define  white 'x'
#define  vac '+'
#include"LinStackForRecord.h"
#include"SeqListForPoints.h"
#include"SeqListForShape.h"
SeqListForPoints closure;
typedef struct
{
    short turnx;//回合数
    char haveput;
    char fen[len][len],dicount[len][len][8];//表示棋盘以及每个点的“方向个数”
    int timeblack,timewhite;
    clock_t start,stop;
    LSNode *record;
    SeqListForShape blackconnect[4],whiteconnect[4];//0123 is for c2345
} board;
void BoardInitiate(board *bp,int blacktime,int whitetime)
{
    bp->turnx=1;
    bp->haveput=0;
    char i,j,k;
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
        SeqListInitiate(&(bp->blackconnect[i]));
        SeqListInitiate(&(bp->whiteconnect[i]));
    }
    bp->timeblack=blacktime;
    bp->timewhite=whitetime;
    StackInitiate(&bp->record);
    bp->start=clock();
}
void show(board *bp)
{
    char i,j;
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
        printf("%d(%d in all): ",i+2,SeqListLength(bp->blackconnect+i));
        for(j=0; j<SeqListLength(bp->blackconnect+i); j++)
        {
            printf("<%c%c%hd %hd%c%c> , ",bp->blackconnect[i].list[j].SmallStatus[1],bp->blackconnect[i].list[j].SmallStatus[0],bp->blackconnect[i].list[j].p1,bp->blackconnect[i].list[j].p2,bp->blackconnect[i].list[j].GreatStatus[0],bp->blackconnect[i].list[j].GreatStatus[1]);
        }
        printf("\n");
    }
    printf("WHITE HAS: \n");
    for(i=0; i<4; i++)
    {
        printf("%d(%d in all): ",i+2,SeqListLength(bp->whiteconnect+i));
        for(j=0; j<SeqListLength(bp->whiteconnect+i); j++)
        {
            printf("<%c%c%hd %hd%c%c> , ",bp->whiteconnect[i].list[j].SmallStatus[1],bp->whiteconnect[i].list[j].SmallStatus[0],bp->whiteconnect[i].list[j].p1,bp->whiteconnect[i].list[j].p2,bp->whiteconnect[i].list[j].GreatStatus[0],bp->whiteconnect[i].list[j].GreatStatus[1]);
        }
        printf("\n");
    }
    if(bp->turnx%2) printf("now it is black to play, has put %d piece\n",bp->haveput);
    else printf("now it is white to play, has put %d piece\n",bp->haveput);
}
short nextpiece(short a,char di)//an easy function to fix the piece which is on the di direction of piece a;
{
    switch(di)
    {
    case 0:
    {
        if(a/len!=0&&a%len!=0) return a-len-1;//left-up
        else return -1;
    }
    case 1:
    {
        if(a/len!=0) return a-len;//up
        else return -1;
    }
    case 2:
    {
        if(a/len!=0&&a%len!=len-1) return a-len+1;//right-up
        else return -1;
    }
    case 3:
    {
        if(a%len!=0) return a-1;//left
        else return -1;
    }
    case 4:
    {
        if(a%len!=len-1) return a+1;//right
        else return -1;
    }
    case 5:
    {
        if(a/len!=len-1&&a%len!=0) return a+len-1;//left-down
        else return -1;
    }
    case 6:
    {
        if(a/len!=len-1) return a+len;//down
        else return -1;
    }
    case 7:
    {
        if(a/len!=len-1&&a%len!=len-1) return a+len+1;//right-down
        else return -1;
    }
    }
}
char ShapeAdmin(board *bp,short a,char z)//z=1 means a is a new piece,0 means a is undone
{
    char n1,n2,di,who,n;
    short np1,np2,dp1,dp2,i,index,npout1,npout2,block1,block2;//np are the points of the new shape, dp are the points of the shape which will be deleted
    //npout are the points out of the new shape, used to determine active or die
    //block are the points of blocked shape
    shaperecord t;
    who=bp->fen[a/len][a%len];
    if(z)
    {
        for(di=0; di<4; di++)
        {
            n1=bp->dicount[a/len][a%len][di];
            n2=bp->dicount[a/len][a%len][7-di];
            if(n1+n2>=5) return 1;
            if(n1!=0)//on di, exist new shape
            {
                np1=a;
                for(i=0; i<n1; i++)
                {
                    np1=nextpiece(np1,di);
                }
                npout1=nextpiece(np1,di);
                if(n1>1)//this shape need to be deleted
                {
                    dp1=np1;
                    dp2=nextpiece(a,di);
                    t.p1=dp1;
                    t.p2=dp2;
                    if(who==black)
                    {
                        index=SeqListFind(bp->blackconnect+n1-2,&t);
                        SeqListDelete(bp->blackconnect+n1-2,index,&t);
                    }
                    else
                    {
                        index=SeqListFind(bp->whiteconnect+n1-2,&t);
                        SeqListDelete(bp->whiteconnect+n1-2,index,&t);
                    }
                }
            }
            else
            {
                npout1=block1=nextpiece(a,di);
                if(block1!=-1&&bp->fen[block1/len][block1%len]!=vac)//meet an opponent
                {
                    n=bp->dicount[block1/len][block1%len][di];
                    if(n!=0)
                    {
                        block2=block1;
                        for(i=0; i<n; i++)
                        {
                            block2=nextpiece(block2,di);
                        }
                        t.p1=block1;
                        t.p2=block2;
                        if(who==black)
                        {
                            index=SeqListFind(bp->whiteconnect+n-1,&t);
                            if(bp->whiteconnect[n-1].list[index].p1==block1) bp->whiteconnect[n-1].list[index].SmallStatus[1]=bp->whiteconnect[n-1].list[index].SmallStatus[0]='D';
                            else bp->whiteconnect[n-1].list[index].GreatStatus[1]=bp->whiteconnect[n-1].list[index].GreatStatus[0]='D';
                        }
                        else
                        {
                            index=SeqListFind(bp->blackconnect+n-1,&t);
                            if(bp->blackconnect[n-1].list[index].p1==block1) bp->blackconnect[n-1].list[index].SmallStatus[1]=bp->blackconnect[n-1].list[index].SmallStatus[0]='D';
                            else bp->blackconnect[n-1].list[index].GreatStatus[1]=bp->blackconnect[n-1].list[index].GreatStatus[0]='D';
                        }
                    }
                }
                else if(block1!=-1&&bp->fen[block1/len][block1%len]==vac)//the above step is empty
                {
                    block1=nextpiece(block1,di);
                    if(block1!=-1&&bp->fen[block1/len][block1%len]!=vac)
                    {
                        n=bp->dicount[block1/len][block1%len][di];
                        if(bp->fen[block1/len][block1%len]==who&&n>0)//meet a friend shape
                        {
                            block2=block1;
                            for(i=0; i<n; i++)
                            {
                                block2=nextpiece(block2,di);
                            }
                            t.p1=block1;
                            t.p2=block2;
                            if(who==white)
                            {
                                index=SeqListFind(bp->whiteconnect+n-1,&t);
                                if(bp->whiteconnect[n-1].list[index].p1==block1) bp->whiteconnect[n-1].list[index].SmallStatus[1]=bp->whiteconnect[n-1].list[index].SmallStatus[0]='W';
                                else bp->whiteconnect[n-1].list[index].GreatStatus[1]=bp->whiteconnect[n-1].list[index].GreatStatus[0]='W';
                            }
                            else
                            {
                                index=SeqListFind(bp->blackconnect+n-1,&t);
                                if(bp->blackconnect[n-1].list[index].p1==block1) bp->blackconnect[n-1].list[index].SmallStatus[1]=bp->blackconnect[n-1].list[index].SmallStatus[0]='W';
                                else bp->blackconnect[n-1].list[index].GreatStatus[1]=bp->blackconnect[n-1].list[index].GreatStatus[0]='W';
                            }
                        }
                        else if(bp->fen[block1/len][block1%len]!=who&&n>0)
                        {
                            block2=block1;
                            for(i=0; i<n; i++)
                            {
                                block2=nextpiece(block2,di);
                            }
                            t.p1=block1;
                            t.p2=block2;
                            if(who==black)
                            {
                                index=SeqListFind(bp->whiteconnect+n-1,&t);
                                if(bp->whiteconnect[n-1].list[index].p1==block1) bp->whiteconnect[n-1].list[index].SmallStatus[1]='D';
                                else bp->whiteconnect[n-1].list[index].GreatStatus[1]='D';
                            }
                            else
                            {
                                index=SeqListFind(bp->blackconnect+n-1,&t);
                                if(bp->blackconnect[n-1].list[index].p1==block1) bp->blackconnect[n-1].list[index].SmallStatus[1]='D';
                                else bp->blackconnect[n-1].list[index].GreatStatus[1]='D';
                            }
                        }
                    }
                }
            }
            if(n2!=0)//on 7-di, exist new shape
            {
                np2=a;
                for(i=0; i<n2; i++)
                {
                    np2=nextpiece(np2,7-di);
                }
                npout2=nextpiece(np2,7-di);
                if(n2>1)//this shape need to be deleted
                {
                    dp1=np2;
                    dp2=nextpiece(a,7-di);
                    t.p1=dp1;
                    t.p2=dp2;
                    if(who==black)
                    {
                        index=SeqListFind(bp->blackconnect+n2-2,&t);
                        SeqListDelete(bp->blackconnect+n2-2,index,&t);
                    }
                    else
                    {
                        index=SeqListFind(bp->whiteconnect+n2-2,&t);
                        SeqListDelete(bp->whiteconnect+n2-2,index,&t);
                    }
                }
            }
            else
            {
                npout2=block1=nextpiece(a,7-di);
                if(block1!=-1&&bp->fen[block1/len][block1%len]!=vac)
                {
                    n=bp->dicount[block1/len][block1%len][7-di];
                    if(n!=0)
                    {
                        block2=block1;
                        for(i=0; i<n; i++)
                        {
                            block2=nextpiece(block2,7-di);
                        }
                        t.p1=block1;
                        t.p2=block2;
                        if(who==black)
                        {
                            index=SeqListFind(bp->whiteconnect+n-1,&t);
                            if(bp->whiteconnect[n-1].list[index].p1==block1) bp->whiteconnect[n-1].list[index].SmallStatus[1]=bp->whiteconnect[n-1].list[index].SmallStatus[0]='D';
                            else bp->whiteconnect[n-1].list[index].GreatStatus[1]=bp->whiteconnect[n-1].list[index].GreatStatus[0]='D';
                        }
                        else
                        {
                            index=SeqListFind(bp->blackconnect+n-1,&t);
                            if(bp->blackconnect[n-1].list[index].p1==block1) bp->blackconnect[n-1].list[index].SmallStatus[1]=bp->blackconnect[n-1].list[index].SmallStatus[0]='D';
                            else bp->blackconnect[n-1].list[index].GreatStatus[1]=bp->blackconnect[n-1].list[index].GreatStatus[0]='D';
                        }
                    }
                    else if(block1!=-1&&bp->fen[block1/len][block1%len]==vac)//the above step is empty
                    {
                        block1=nextpiece(block1,7-di);
                        if(block1!=-1&&bp->fen[block1/len][block1%len]!=vac)
                        {
                            n=bp->dicount[block1/len][block1%len][7-di];
                            if(bp->fen[block1/len][block1%len]==who&&n>0)//meet a friend shape
                            {
                                block2=block1;
                                for(i=0; i<n; i++)
                                {
                                    block2=nextpiece(block2,7-di);
                                }
                                t.p1=block1;
                                t.p2=block2;
                                if(who==white)
                                {
                                    index=SeqListFind(bp->whiteconnect+n-1,&t);
                                    if(bp->whiteconnect[n-1].list[index].p1==block1) bp->whiteconnect[n-1].list[index].SmallStatus[1]=bp->whiteconnect[n-1].list[index].SmallStatus[0]='W';
                                    else bp->whiteconnect[n-1].list[index].GreatStatus[1]=bp->whiteconnect[n-1].list[index].GreatStatus[0]='W';
                                }
                                else
                                {
                                    index=SeqListFind(bp->blackconnect+n-1,&t);
                                    if(bp->blackconnect[n-1].list[index].p1==block1) bp->blackconnect[n-1].list[index].SmallStatus[1]=bp->blackconnect[n-1].list[index].SmallStatus[0]='W';
                                    else bp->blackconnect[n-1].list[index].GreatStatus[1]=bp->blackconnect[n-1].list[index].GreatStatus[0]='W';
                                }
                            }
                            else if(bp->fen[block1/len][block1%len]!=who&&n>0)
                            {
                                block2=block1;
                                for(i=0; i<n; i++)
                                {
                                    block2=nextpiece(block2,7-di);
                                }
                                t.p1=block1;
                                t.p2=block2;
                                if(who==black)
                                {
                                    index=SeqListFind(bp->whiteconnect+n-1,&t);
                                    if(bp->whiteconnect[n-1].list[index].p1==block1) bp->whiteconnect[n-1].list[index].SmallStatus[1]='D';
                                    else bp->whiteconnect[n-1].list[index].GreatStatus[1]='D';
                                }
                                else
                                {
                                    index=SeqListFind(bp->blackconnect+n-1,&t);
                                    if(bp->blackconnect[n-1].list[index].p1==block1) bp->blackconnect[n-1].list[index].SmallStatus[1]='D';
                                    else bp->blackconnect[n-1].list[index].GreatStatus[1]='D';
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
                if(npout1==-1||bp->fen[npout1/len][npout1%len]!=vac) t.SmallStatus[1]=t.SmallStatus[0]='D';
                else
                {
                    t.SmallStatus[0]='A';
                    n=nextpiece(npout1,di);
                    if(n!=-1)
                    {
                        if(bp->fen[n/len][n%len]==who) t.SmallStatus[1]='W';
                        else if(bp->fen[n/len][n%len]==vac) t.SmallStatus[1]='A';
                        else t.SmallStatus[1]='D';
                    }
                }
                if(npout2==-1||bp->fen[npout2/len][npout2%len]!=vac) t.GreatStatus[1]=t.GreatStatus[0]='D';
                else
                {
                    t.GreatStatus[0]='A';
                    n=nextpiece(npout2,7-di);
                    if(n!=-1)
                    {
                        if(bp->fen[n/len][n%len]==who) t.GreatStatus[1]='W';
                        else if(bp->fen[n/len][n%len]==vac) t.GreatStatus[1]='A';
                        else t.GreatStatus[1]='D';
                    }
                }
                if(who==black) SeqListInsert(bp->blackconnect+n1+n2-1,&t);
                else SeqListInsert(bp->whiteconnect+n1+n2-1,&t);
            }
        }
        return 0;
    }
}
void putInfluent(board *bp,short a)//update dicount[][][] when do a move
{
    char di;
    for(di=0; di<8; di++)//for each direction, 01234567 symbolize 8 different directions
    {
        short t;//t is the number of the next piece on that direction
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
        short p=a,q=a;
        while(bp->dicount[p/len][p%len][di]>0)
        {
            p=nextpiece(q,di);
            bp->dicount[p/len][p%len][7-di]=1+bp->dicount[q/len][q%len][7-di];
            q=p;
        }
    }
}
char putonepiece(board *bp,short a)//put one piece on a(a>=0 and a==-1 means finish this turn)
//return the result: black1 white0 fighting2 error-2 ok3
{
    bp->stop=clock();
    int duration=(int)1000*(bp->stop-bp->start)/CLK_TCK;
    if(bp->turnx%2) bp->timeblack-=duration;
    else bp->timewhite-=duration;
    bp->start=clock();
    if(a==-1)//try to finish this turn
    {
        if(bp->haveput)//have already put one, can finish
        {
            bp->turnx++;
            bp->haveput=0;
            return 3;
        }
        else return -2;//cannot finish
    }
    char who,tag;
    if(bp->fen[a/len][a%len]!=vac) return -2;
    who=(bp->turnx)%2;
    bp->fen[a/len][a%len]=who?black:white;
    putInfluent(bp,a);
    tag=ShapeAdmin(bp,a,1);
    if(tag==1) return who;
    StackPush(bp->record,a);
    if(bp->turnx==1)
    {
        bp->turnx++;
        return 2;
    }
    bp->haveput++;
    char di;
    if(bp->haveput==2)
    {
        bp->turnx++;
        bp->haveput=0;
    }
    return 2;
}
void undoInfluent(board *bp,short a)//update dicount[][][] when undo a move
{
    char di;
    for(di=0; di<8; di++) //handle the surroundings
    {
        short p,q;
        if(bp->dicount[a/len][a%len][di]>0)
        {
            p=nextpiece(a,di);
            if(p!=-1)
            {
                bp->dicount[p/len][p%len][7-di]=0;
                q=p;
                while(bp->dicount[p/len][p%len][di]>0)
                {
                    p=nextpiece(q,di);
                    bp->dicount[p/len][p%len][7-di]=1+bp->dicount[q/len][q%len][7-di];
                    q=p;
                }
            }
        }
    }
    for(di=0; di<8; di++)//for each direction, 01234567 symbolize 8 different directions
    {
        short t;
        t=nextpiece(a,di);
        if(t!=-1)
        {
            bp->dicount[a/len][a%len][di]=-1;
        }
    }
}
char undoonepiece(board *bp)//0 for error, 1 for success
{
    short t;
    char who;
    if(StackNotEmpty(bp->record)) StackPop(bp->record,&t);
    else return 0;
    who=bp->fen[t/len][t%len];
    bp->fen[t/len][t%len]=vac;
    undoInfluent(bp,t);
    if(StackTop(bp->record,&t)==0)//undo the first move
    {
        bp->haveput=0;
        bp->turnx=1;
        return 1;
    }
    if(who!=bp->fen[t/len][t%len])
    {
        bp->haveput=0;
        if((who==black&&(1-bp->turnx%2))||(who==white&&(bp->turnx%2))) bp->turnx--;
    }
    else
    {
        bp->haveput=1;
        bp->turnx--;
    }
    return 1;
}
void GetDoubleClosure(board *bp)
{
    short i,m,t1,t2;
    char di;
    ListInitiate(&closure);
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
                if((t1!=-1)&&(bp->fen[t1/len][t1%len]==vac)&&ListNotHave(&closure,1,t1))
                {
                    ListInsert(&closure,1,t1);
                }
            }
        }
    }
    while(StackNotEmpty(p))
    {
        StackPop(p,&i);
        for(di=0; di<8; di++)
        {
            t1=nextpiece(i,di);
            if(t1!=-1)
            {
                t2=nextpiece(t1,di);
                if((t2!=-1)&&(bp->fen[t2/len][t2%len]==vac)&&ListNotHave(&closure,-1,t2)) ListInsert(&closure,-1,t2);
                char di2;
                if(di==1||di==2) di2=di-1;
                else if(di==5||di==6) di2=di+1;
                else if(di==3) di2=5;
                else if(di==4) di2=2;
                else if(di==0) di2=3;
                else if(di==7) di2=4;
                t2=nextpiece(t1,di2);
                if((t2!=-1)&&(bp->fen[t2/len][t2%len]==vac)&&ListNotHave(&closure,-1,t2)) ListInsert(&closure,-1,t2);
            }
        }
    }
}
