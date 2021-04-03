#include"abyss.h"
int main()
{
    board b;
    BoardInitiate(&b,0,0);
    putonepiece(&b,40);
    putonepiece(&b,0);//
    putonepiece(&b,1);//
    putonepiece(&b,3);
    putonepiece(&b,4);
    putonepiece(&b,8);//
    putonepiece(&b,9);//
    putonepiece(&b,13);
    show(&b);
    return 0;
}
