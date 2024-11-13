#include <easyx.h>
#include <iostream>
int main()
{
    initgraph(640, 480);
    setlinecolor(RGB(255, 0, 0));
    line(0, 0, 640, 480);
    // _getch();
    double speed = 300;
    int x, y, r;
    x = 300;
    y = 10;
    r = 10;
    while (true)
    {
        if (y + r + speed * 0.01 > 480)
        {
            speed = -speed;
        }
        if (y - r + speed * 0.01 < 0)
        {
            speed = -speed;
        }
        y += speed * 0.01;
        cleardevice();
        setfillcolor(RGB(255, 0, 0));
        fillcircle(x, y, r);
        Sleep(10);
    }
    getchar();
    closegraph();
    return 0;
}