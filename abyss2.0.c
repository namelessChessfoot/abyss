#include"abyss.h"
#include"math.h"
#include <stdio.h>
#include<string.h>
int strtoint(char *string,int *btime,int *wtime,int *a,int *b)
{

    int i,count=0;
    for(i=0; i<strlen(string); i++)
        if(string[i]==' ')
        {
            char str[15];
            int j=i,k;
            while(string[j]==' ')
                j++;
            for(k=j; k<strlen(string); k++)
                str[k-j]=string[k];
            str[k-j]='\0';
            count++;
            if(count==1)
                *btime=600-atoi(str);
            else if(count==2)
                *wtime=600-atoi(str);
            else if(count==3)
                *a=atoi(str);
            else if(count==4)
                *b=atoi(str);
            return 1;
        }
    *btime=-1;
    *wtime=-1;
    *a=-1;
    *b=-1;
    return 0;
}
int main()
{
    board bb;
    int i,j,a,b,btime,wtime,best,besteva,who,lsc;
    char str[100];
    SLNode *head,*pp;
    gets(str);
    strtoint(str,&btime,&wtime,&a,&b);
    if(a==b&&a==1) who=0;//i am black
    else if(a==b&&a==0) who=1;//i am white
    else return 0;
    BoardInitiate(&bb,0,0);
    if(who==0)
    {
        putonepiece(&bb,len*len/2);
        fflush(stdout);
        fprintf(stdout,"%d %d\n",len*len/2,-1);
        fflush(stdout);
    }
    while(1)
    {
        if(bb.turnx%2==who)
        {
            gets(str);
            strtoint(str,&btime,&wtime,&a,&b);
            lsc=putonepiece(&bb,a);
            if(lsc==1-who)
            {
                return 0;
            }
            lsc=putonepiece(&bb,b);
            if(lsc==1-who)
            {
                return 0;
            }
        }
        else
        {
            lsc=GenerateMoves(&bb,&head);
            if(lsc==666) return 0;
            best=0;
            besteva=(int)pow(-1,bb.turnx)*99999;
            pp=head->next;
            for(i=0; i<head->data.move1; i++)
            {
                board newb;
                newb=bb;
                putonepiece(&newb,pp->data.move1);
                putonepiece(&newb,pp->data.move2);
                int ee;
                ee=Evaluate(&newb);
                if(bb.turnx%2==1&&ee>besteva)
                {
                    besteva=ee;
                    best=i;
                }
                if(bb.turnx%2==0&&ee<besteva)
                {
                    besteva=ee;
                    best=i;
                }
                pp=pp->next;
            }
            pp=head->next;
            for(i=0; i<best; i++)
            {
                pp=pp->next;
            }
            putonepiece(&bb,pp->data.move1);
            putonepiece(&bb,pp->data.move2);
            fprintf(stdout,"%d %d\n",pp->data.move1,pp->data.move2);
            fflush(stdout);
            PackListDestroy(&head);
            if(lsc==PositiveInfinity)
            {
                return 0;
            }
        }
    }
    return 0;
}
