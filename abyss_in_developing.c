#include"abyss.h"
int main()
{
    board b,*bp;
    short i;
    bp=&b;
    BoardInitiate(bp,BTime*1000,WTime*1000);
    show(bp);
    while(1)
    {
        short help,record;
        scanf("%hd",&help);
        record=putonepiece(bp,help);
        system("cls");
        show(bp);
        if(record==-2) printf("error\n");
        if(record==0)
        {
            printf("white win\n");
            break;
        }
        if(record==1)
        {
            printf("black win\n");
            break;
        }
        if(record==2)
        {
            printf("continue......\n");
        }
    }
    return 0;
}
