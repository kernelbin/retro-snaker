#include<Windows.h>
#include"EasyWindow.h"
#include"Global.h"

int drctX[4] = { -1, 0, 1, 0 };
int drctY[4] = { 0, -1, 0, 1 };



int GameInit()
{
	SnakeHead = 0;
	return 0;
}



int GameStart()
{
	//Temporary map
	for (int y = 0; y < BlkNum; y++)
		Block[y][0] = 1;

	//Create The Snake (Temporary)
	SnakeHead = SnakeTail = SnakeAlloc(3, 3);
	SnakeAddToHead(3, 4);
	SnakeAddToHead(3, 5);
	
	Direction = 2;
	return 0;
}


int GameTimer()
{
	int px = (SnakeHead->x + drctX[Direction]) % BlkNum;
	int py = (SnakeHead->y + drctY[Direction]) % BlkNum;
	//check block / snake itself
	if (Block[py][px] == 1 || Block[py][px] == 2)
	{
		if (!(px == SnakeTail->x && py == SnakeTail->y))
		{
			//Died

			return 0;
		}
	}

	//check food
	if (Block[py][px] == 3)
	{
		
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

