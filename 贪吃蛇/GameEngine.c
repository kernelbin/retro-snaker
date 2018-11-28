#include<Windows.h>
#include"EasyWindow.h"
#include"Global.h"

int drctX[4] = { -1, 0, 1, 0 };
int drctY[4] = { 0, -1, 0, 1 };



int GameInit()
{
	SnakeHead = 0;
	GameStates = 0;
	srand(time(0));
	return 0;
}



int GameStart()
{
	GameStates = 1;

	//Clean The Map
	for (int py = 0; py < BlkNum; py++)
		for (int px = 0; px < BlkNum; px++)
			Block[py][px] = 0;

	//Clean The Snake (if exist)
	for (pSNAKE CleanNode = SnakeHead; CleanNode;)
	{
		pSNAKE Next = CleanNode->t;
		free(CleanNode);
		CleanNode = Next;
	}

	//Temporary map
	for (int y = 0; y < BlkNum; y++)
		Block[y][0] = 1;

	//Create The Snake (Temporary)
	SnakeHead = SnakeTail = SnakeAlloc(3, 3);
	SnakeAddToHead(3, 4);
	SnakeAddToHead(3, 5);
	
	Direction = 2;


	FoodGenerate();

	//SetTimer
	EZSendMessage(GameWnd, EZWM_USER_NOTIFY, 101, 0);
	return 0;
}


int GamePause()
{
	GameStates = 2;
	//KillTimer
	EZSendMessage(GameWnd, EZWM_USER_NOTIFY, 102, 0);
	return 0;
}


int GameContinue()
{
	GameStates = 1;
	//SetTimer
	EZSendMessage(GameWnd, EZWM_USER_NOTIFY, 101, 0);
	return 0;
}


int GameEnd()
{
	GameStates = 0;
	//KillTimer
	EZSendMessage(GameWnd, EZWM_USER_NOTIFY, 102, 0);


	return 0;
}


int GameTimer()
{
	//Add an extra BlkNum to ensure the answer is positive
	int px = (SnakeHead->x + drctX[Direction] + BlkNum) % BlkNum;
	int py = (SnakeHead->y + drctY[Direction] + BlkNum) % BlkNum;


	//check block / snake itself
	if (Block[py][px] == 1 || Block[py][px] == 2)
	{
		if (!(px == SnakeTail->x && py == SnakeTail->y))
		{
			//Died
			GameEnd();
			return 0;
		}
	}

	//check food
	if (Block[py][px] == 3)
	{
		SnakeAddToHead(px, py);
		FoodGenerate();
		return 0;
	}

	//just simplly move
	SnakeMove(px, py);
	return 0;
}


int SnakeAddToHead(int x, int y)
{
	//Allocate a node and add to the head
	pSNAKE n = SnakeAlloc(x, y);
	SnakeHead->h = n;
	n->t = SnakeHead;

	SnakeHead = n;
	return 0;
}


int SnakeMove(int x,int y)
{
	//Get the origin tail of snake ,move it to the head
	Block[SnakeTail->y][SnakeTail->x] = 0;
	pSNAKE NewTail = SnakeTail->h;
	SnakeTail->h->t = 0;
	
	memset(SnakeTail, 0, sizeof(SNAKE));
	SnakeTail->x = x;
	SnakeTail->y = y;
	SnakeHead->h = SnakeTail;
	SnakeTail->t = SnakeHead;
	SnakeHead = SnakeTail;

	SnakeTail = NewTail;

	Block[SnakeHead->y][SnakeHead->x] = 2;
	return 0;
}


int FoodGenerate()
{ 
	int cx, cy;
	while (Block[cx = rand() % BlkNum][cy = rand() % BlkNum] != 0);
	Block[cx][cy] = 3;
	return 0;
}


int SnakeResetDirection(int d)
{
	//set the direction of snake
	if (SnakeHead->x + drctX[d] == SnakeHead->t->x && SnakeHead->y + drctY[d] == SnakeHead->t->y)
	{
		return 0;//not accpeted
	}

	Direction = d;
	return 0;
}

pSNAKE SnakeAlloc(int x, int y)
{
	pSNAKE node;
	node = malloc(sizeof(SNAKE));
	memset(node, 0, sizeof(SNAKE));
	node->x = x;
	node->y = y;
	Block[y][x] = 2;
	return node;
}

