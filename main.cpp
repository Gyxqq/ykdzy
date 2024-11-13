#include <windows.h>
#include <graphics.h> // 假设你在使用 EasyX 库
#include <stdio.h>
#include <conio.h>
#include <ctime>
#include <iostream>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 400;
const int BALL_RADIUS = 20;
const int BALL_INIT_X = 200;
const int BALL_INIT_Y = 380;
const int GRAVITY = 1;
const int RECT_INIT_LEFT = 570;
const int RECT_INIT_RIGHT = 600;
const int RECT_INIT_BOTTOM = 400;
const int RECT_MIN_HEIGHT = 270;
const int RECT_MAX_HEIGHT = 350;
const int RECT_MIN_SPEED = 5;
const int RECT_MAX_SPEED = 10;
const int TEXT_SIZE = 20;
const int SCORE_X = 50;
const int SCORE_Y = 30;
const int SLEEP_TIME = 10;

void initGame(int &x, int &y, int &vy, int &left, int &top, int &right, int &bottom, int &vx, int &count, bool &counter)
{
    srand((int)time(NULL));
    x = BALL_INIT_X;
    y = BALL_INIT_Y;
    vy = 0;
    left = RECT_INIT_LEFT;
    top = RECT_MAX_HEIGHT - rand() % (RECT_MAX_HEIGHT - RECT_MIN_HEIGHT);
    right = RECT_INIT_RIGHT;
    bottom = RECT_INIT_BOTTOM;
    vx = rand() % (RECT_MAX_SPEED - RECT_MIN_SPEED) + RECT_MIN_SPEED;
    count = 0;
    counter = true;
    initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
    settextstyle(TEXT_SIZE, 0, "Arial");
    setfillcolor(RED);
    setlinecolor(BLUE);
}

void updateRectangle(int &left, int &top, int &right, int &bottom, int &vx, bool &counter)
{
    left -= vx;
    right -= vx;
    if (right <= 0)
    {
        left = RECT_INIT_LEFT;
        top = RECT_MAX_HEIGHT - rand() % (RECT_MAX_HEIGHT - RECT_MIN_HEIGHT);
        right = RECT_INIT_RIGHT;
        bottom = RECT_INIT_BOTTOM;
        vx = rand() % (RECT_MAX_SPEED - RECT_MIN_SPEED) + RECT_MIN_SPEED;
        counter = true;
    }
}

void handleInput(int &vy, int y)
{
    if (_kbhit())
    {
        char input = _getch();
        if (input == ' ' && y == BALL_INIT_Y)
        {
            vy = -20;
        }
    }
}

void updateBall(int &y, int &vy)
{
    vy += GRAVITY;
    y += vy;
    if (y >= SCREEN_HEIGHT - BALL_RADIUS)
    {
        vy = 0;
        y = SCREEN_HEIGHT - BALL_RADIUS;
    }
}

bool checkCollision(int x, int y, int r, int left, int right, int top)
{
    return left <= x + r && right >= x - r && top <= y + r;
}

void drawGame(int x, int y, int r, int left, int top, int right, int bottom, int count)
{
    TCHAR count_char[100];
    cleardevice();
    sprintf(count_char, "%d", count);
    outtextxy(SCORE_X, SCORE_Y, "SCORE: ");
    outtextxy(SCORE_X + 70, SCORE_Y + 1, count_char);
    fillcircle(x, y, r);
    fillrectangle(left, top, right, bottom);
    FlushBatchDraw();
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    int x, y, vy, left, top, right, bottom, vx, count;
    bool counter;
    initGame(x, y, vy, left, top, right, bottom, vx, count, counter);

    BeginBatchDraw();
    while (true)
    {
        updateRectangle(left, top, right, bottom, vx, counter);
        handleInput(vy, y);
        updateBall(y, vy);

        if (checkCollision(x, y, BALL_RADIUS, left, right, top))
        {
            count = 0;
            left += vx;
            if (left >= x + BALL_RADIUS)
            {
                left = x + BALL_RADIUS;
                right = left + 30;
            }
            else
            {
                left -= vx;
                vy = 0;
                y = top - BALL_RADIUS;
            }
        }

        if (right < x - BALL_RADIUS && counter)
        {
            count++;
            counter = false;
        }

        drawGame(x, y, BALL_RADIUS, left, top, right, bottom, count);
        Sleep(SLEEP_TIME);
    }
    EndBatchDraw();
    closegraph();
    _getch();
    return 0;
}