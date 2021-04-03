#include"abyss.h"
#include"math.h"
#include <stdio.h>
#include<string.h>
int strtoint(char *string,int *btime,int *wtime,int *a,int *b)
{
    int i,pos=0,count=0;
    while(pos<strlen(string))
    {
        if(string[pos]!=' ')
        {
            char str[15];
            i=pos;
            while(string[i]!=' '&&string[i]!='\0')
            {
                str[i-pos]=string[i];
                i++;
            }
            str[i-pos]='\0';
            pos=i;
            count++;
            if(count==1)
                *btime=600-atoi(str);
            else if(count==2)
                *wtime=600-atoi(str);
            else if(count==3)
                *a=atoi(str);
            else if(count==4)
            {
                *b=atoi(str);
                return 1;
            }
        }
        else
            pos++;
    }
    *btime=-1,*wtime=-1,*a=-1,*b=-1;
    return 0;
}
int main()
{
    board bb;
    int i,j,a,b,btime,wtime,waitwho,lsc;
    char str[100];
    HashNode *HashHeads;
    CreateHashList(&HashHeads);
    SLNode *head,*pp;
    srand((unsigned)time(NULL));
    for(i=0; i<2*len*len; i++)
    {
        ZobristValue[i]=rand64();
    }
    gets(str);
    strtoint(str,&btime,&wtime,&a,&b);
    if(a==b&&a==1) waitwho=0;//i am black
    else if(a==b&&a==0) waitwho=1;//i am white
    else return 0;
    BoardInitiate(&bb,0,0);
    if(waitwho==0)
    {
        putonepiece(&bb,len*len/2);
        fflush(stdout);
        fprintf(stdout,"%d %d\n",len*len/2,-1);
        fflush(stdout);
    }
    while(1)
    {
        if(bb.turnx%2==waitwho)
        {
            gets(str);
            strtoint(str,&btime,&wtime,&a,&b);
            lsc=putonepiece(&bb,a);
            if(lsc==1-waitwho)
            {
                return 0;
            }
            lsc=putonepiece(&bb,b);
            if(lsc==1-waitwho)
            {
                return 0;
            }
        }
        else
        {
            Package pack;
            int tag,sa;
            mTreeNode *tn;
            pack=SearchTogether(&tn,waitwho,&bb,-1,HashHeads,&sa,&tag);
            fprintf(stdout,"%d %d\n",pack.move1,pack.move2);
            fflush(stdout);
            lsc=putonepiece(&bb,pack.move1);
            if(lsc!=1-waitwho) lsc=putonepiece(&bb,pack.move2);
            if(tag) CutTree(tn,-1);
            HashClear(HashHeads);
            if(lsc==1-waitwho)
            {
                show(&bb);
                return 0;
            }
        }
    }
    return 0;
}
