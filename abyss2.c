
#include"abyss.h"
#include"math.h"
#include <stdio.h>
#include<string.h>
int strtoint(char *string,int *a,int *b)
{
    *a=atoi(string);
    int i;
    for(i=0; i<strlen(string); i++)
        if(string[i]==' ')
        {
            char str[10];
            int j=i,k;
            while(string[j]==' ')
                j++;
            for(k=j; k<strlen(string); k++)
                str[k-j]=string[k];
            str[k-j]='\0';
            *b=atoi(str);
            return 1;
        }
    *b=0;
    return 0;
}
int main()
{
    board bb;
    int i,j,a,b,best,besteva;
    char str[100];
    SLNode *head,*pp;
    BoardInitiate(&bb,0,0);
    fflush(stdout);
    while(1)
    {
        if(bb.turnx%2==1)
        {
            gets(str);
            strtoint(str,&a,&b);
            putonepiece(&bb,a);
            if(bb.turnx!=1) putonepiece(&bb,b);
        }
        else
        {
            GenerateMoves(&bb,&head);
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
        }
    }
}
