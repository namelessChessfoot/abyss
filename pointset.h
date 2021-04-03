#define MMAAXX 500
#include"LinListForPackage.h"
typedef struct
{
    int point[MMAAXX],num;
} PointSet;
void PSInitiate(PointSet *p)
{
    p->num=0;
}
int PSInsert(PointSet *p,int point)
{
    int i;
    if(p->num==MMAAXX) return 2;
    for(i=0; i<p->num; i++)
    {
        if(p->point[i]==point) return 1;
    }
    p->point[p->num]=point;
    p->num++;
    return 0;
}
int PSIntersect(PointSet a,PointSet b,PointSet *new_set)
{
    int n=0,i,j;
    PSInitiate(new_set);
    for(i=0;i<a.num;i++)
    {
        for(j=0;j<b.num;j++)
        {
            if(a.point[i]==b.point[j])
            {
                PSInsert(new_set,a.point[i]);
                break;
            }
        }
    }
    return 0;
}
int PSMerge(PointSet getter,PointSet giver,PointSet *newset)
{
    int i;
    *newset=getter;
    for(i=0; i<giver.num; i++)
    {
        if(PSInsert(newset,giver.point[i])==2) return 1;
    }
    return 0;
}
int PSCombine(PointSet A,PointSet B,Package *p)
{
    Package help;
    int i,j,k,n=0;
    for(i=0; i<A.num; i++)
    {
        for(j=0; j<B.num; j++)
        {
            if(A.point[i]!=B.point[j])
            {
                int tag=0;
                for(k=0; k<n; k++)
                {
                    if((A.point[i]==((p+k)->move1))&&(B.point[j]==((p+k)->move2)))
                    {
                        tag=1;
                        break;
                    }
                    if((A.point[i]==((p+k)->move2))&&(B.point[j]==((p+k)->move1)))
                    {
                        tag=1;
                        break;
                    }
                }
                if(tag==0)
                {
                    help.move1=A.point[i];
                    help.move2=B.point[j];
                    *(p+n)=help;
                    n++;
                }
            }
        }
    }
    return n;
}
