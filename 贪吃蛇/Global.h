#pragma once
#include<Windows.h>
#include"EasyWindow.h"



//Program Name
#define szAppName TEXT("Snake")


//Windows
EZWND MainWnd;
EZWND GameWnd;
EZWND ControlWnd;


//Default Color
#define DefColor RGB(137,186,4)


//GDI Object
HBRUSH DefBrush;
HPEN DefPen;
LOGFONT FontForm;

HBRUSH BrickBrush;



//Window Proc
EZWNDPROC MainProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC ControlPanelProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC GameProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC QuitMessageBox(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

EZWNDPROC BlockProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);





//About the gam
int Block[128][128]; //0 empty   1 brick   2 snake(Use SnakeHead to judge whether it is head or not)   3 food
EZWND BlkWnd[128][128];
int BlkNum;

typedef struct __NodeSnake SNAKE, *pSNAKE;
typedef struct __NodeSnake
{
	int x, y;
	pSNAKE h, t;//Head,Tail
}SNAKE, *pSNAKE;

pSNAKE SnakeHead, SnakeTail;

int Direction;// < 0 - 3 >  ->  <left up right down>



//Game Engine Functions
int GameInit();
int GameStart();
int GamePause();
int GameContinue();
int GameEnd();
int GameTimer();

int SnakeResetDirection(int d);

//Game States
int GameStates;//0 Not Start yet    1 Playing    2 Pause


pSNAKE SnakeAlloc(int x, int y);



//Global Vars
BOOL bQuitMsgBox;



