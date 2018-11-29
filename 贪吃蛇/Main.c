#include<Windows.h>
#include"EasyWindow.h"
#include"Global.h"
#include"resource.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{

	InitEZWindow();

	MainWnd = CreateEZStyleParentWindow(szAppName, EZS_PARENT | EZS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) - 841) / 2, (GetSystemMetrics(SM_CYSCREEN) - 527) / 2, 841, 527, 1, MainProc);

	return EZWndMessageLoop();
}



EZWNDPROC MainProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case EZWM_CREATE:
		GDIObjInit(ezWnd->hdc);
		memset(Block, 0, sizeof(Block));
		memset(BlkWnd, 0, sizeof(BlkWnd));
		bQuitMsgBox = FALSE;
		GameWnd = CreateEZWindow(ezWnd, 0, 0, 0, 0, GameProc);
		ControlWnd = CreateEZWindow(ezWnd, 0, 0, 0, 0, ControlPanelProc);
		break;
	case EZWM_SIZE:
		MoveEZWindow(GameWnd, 0, 0, ezWnd->Width - 250, ezWnd->Height, 0);
		MoveEZWindow(ControlWnd, ezWnd->Width - 250, 0, 250, ezWnd->Height, 0);
		return 0;
	case EZWM_USER_NOTIFY:
		//EZDialogBox(ezWnd, 0, 0, 460, 200, EZDLG_CENTER | EZDLG_MASK, RGB(0, 0, 0), QuitMessageBox);
		//There are several bugs in the function above,so i use MessageBox instead
		MessageBox(NULL, TEXT("Gamr over!"), szAppName, 0);
		return 0;
	case EZWM_CLOSE:
		if (!bQuitMsgBox)
		{
			bQuitMsgBox = TRUE;
			EZDialogBox(ezWnd, 0, 0, 370, 156, EZDLG_CENTER | EZDLG_MASK, RGB(0, 0, 0), QuitMessageBox);
		}
		return EZDO_NOT_CLOSE;
	case EZWM_DESTROY:
		GDIObjClean();
		PostQuitMessage(0);
		return 0;
	}
	return 0;
}


EZWNDPROC GameProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static int xpos, ypos, tlen;
	static int TimerID;
	switch (message)
	{
	case EZWM_CREATE:
		BlkNum = 23;
		for (int py = 0; py < BlkNum; py++)
			for (int px = 0; px < BlkNum; px++)
			{
				BlkWnd[py][px] = CreateEZWindow(ezWnd, 0, 0, 0, 0, BlockProc);
				BlkWnd[py][px]->ezID = py * BlkNum + px;
			}

		TimerID = -1;
		EZCaptureKeyboard(ezWnd);
		return 0;
	case EZWM_DRAW:
		//SelectObject(wParam, DefBrush);
		SetStretchBltMode(wParam, HALFTONE);
		StretchBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, hdcBk, 0, 0, BmpBk.bmWidth, BmpBk.bmHeight,SRCAND);
		//PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, PATCOPY);
		PatBlt(wParam, xpos, ypos, tlen, tlen, WHITENESS);
		return 0;

	case EZWM_USER_NOTIFY:
		switch (wParam)
		{
		case 1:
			GameStart();

			break;
		case 2:
			//GamePause / Continue
			switch (GameStates)
			{
			case 0:
				break;
			case 1:
				GamePause();
				break;
			case 2:
				GameContinue();
				break;
			}
			break;
		case 3:
			GameEnd();
			EZSendMessage(MainWnd, EZWM_USER_NOTIFY, 0, 0);
			break;
		case 101:
			//SetTimer
			if (TimerID != -1)
			{
				KillEZTimer(ezWnd, TimerID);
			}
			TimerID = SetEZTimer(ezWnd, TimerSpace);
			break;
		case 102:
			//KillTimer
			KillEZTimer(ezWnd, TimerID);
			TimerID = -1;
			break;
		}
		EZRepaint(ezWnd, 0);
		return 0;

	case EZWM_TIMER:
		if (bQuitMsgBox == 0)
		{
			GameTimer();
			EZRepaint(ezWnd, 0);
		}
		if (bResetTimer)
		{
			EZSendMessage(ezWnd, EZWM_USER_NOTIFY, 102, 0);
			EZSendMessage(ezWnd, EZWM_USER_NOTIFY, 101, 0);
			bResetTimer = FALSE;
		}
		return 0;
	case EZWM_KEYDOWN:
		if (37 <= wParam && wParam <= 40)//left
		{
			SnakeResetDirection(wParam - 37);
		}
		return 0;
	case EZWM_SIZE:
		//calculate zhe size of each block
	{
		int blocklen;
		if (ezWnd->Width > ezWnd->Height)
		{
			blocklen = (ezWnd->Height - 20) / BlkNum;//length per block
			tlen = blocklen * BlkNum;
		}
		else
		{
			blocklen = (ezWnd->Width - 20) / BlkNum;//length per block
			tlen = blocklen * BlkNum;
		}
		xpos = (ezWnd->Width - tlen) / 2;
		ypos = (ezWnd->Height - tlen) / 2;

		//move child windows
		for (int py = 0; py < BlkNum; py++)
			for (int px = 0; px < BlkNum; px++)
				MoveEZWindow(BlkWnd[py][px],
					xpos + px * blocklen,
					ypos + py * blocklen,
					blocklen, blocklen, 0);
		return 0;
	}
	case EZWM_DESTROY:
		if(TimerID!=-1)EZSendMessage(ezWnd, EZWM_USER_NOTIFY, 102, 0);
	}
	return 0;
}


EZWNDPROC ControlPanelProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static EZWND StartBtn, PauseContinueBtn, EndBtn, ScoreText, SpeedScrollBar;
	switch (message)
	{
	case EZWM_CREATE:
		FontForm.lfHeight = 22;
		StartBtn = CreateEZStyleWindow(ezWnd, TEXT("Start Game"), EZS_CHILD | EZS_BUTTON | EZBS_PUSHBUTTON, 0, 0, 0, 0);
		EZSendMessage(StartBtn, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(StartBtn, EZWM_SETCOLOR, RGB(0, 0, 0), RGB(0, 0, 0));
		PauseContinueBtn = CreateEZStyleWindow(ezWnd, TEXT("Pause"), EZS_CHILD | EZS_BUTTON | EZBS_PUSHBUTTON, 0, 0, 0, 0);
		EZSendMessage(PauseContinueBtn, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(PauseContinueBtn, EZWM_SETCOLOR, RGB(0, 0, 0), RGB(0, 0, 0));
		EndBtn = CreateEZStyleWindow(ezWnd, TEXT("End Game"), EZS_CHILD | EZS_BUTTON | EZBS_PUSHBUTTON, 0, 0, 0, 0);
		EZSendMessage(EndBtn, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(EndBtn, EZWM_SETCOLOR, RGB(0, 0, 0), RGB(0, 0, 0));

		SpeedScrollBar = CreateEZStyleWindow(ezWnd, TEXT(""), EZS_CHILD | EZS_CHILD_HSCROLL, 0, 0, 0, 0);
		
		FontForm.lfHeight = 30;
		ScoreText = CreateEZStyleWindow(ezWnd, TEXT("Score: 0"), EZS_CHILD | EZS_STATIC, 0, 0, 0, 0);
		EZSendMessage(ScoreText, EZWM_SETFONT, 0, &FontForm);
		return 0;
	case EZWM_USER_NOTIFY:
		//Update the score
		if(wParam == 0)
		{
		TCHAR Score[32];
		wsprintf(Score, TEXT("Score: %d"), ScoreNow);
		EZSendMessage(ScoreText, EZWM_SETTEXT, Score, 0);
		EZRepaint(ScoreText, 0);
		}
		else
		{
			EZSendMessage(PauseContinueBtn, EZWM_SETTEXT, GameStates == 2 ? TEXT("Continue") : TEXT("Pause"), 0);
			EZRepaint(PauseContinueBtn, 0);
		}
	return 0;
	case EZWM_COMMAND:
		if (lParam == StartBtn)
		{
			EZSendMessage(GameWnd, EZWM_USER_NOTIFY, 1, 0);
		}
		else if (lParam == PauseContinueBtn)
		{
			EZSendMessage(GameWnd, EZWM_USER_NOTIFY, 2, 0);
			EZSendMessage(PauseContinueBtn, EZWM_SETTEXT, GameStates == 2 ? TEXT("Continue") : TEXT("Pause"), 0);
			EZRepaint(PauseContinueBtn, 0);
		}
		else if (lParam == EndBtn)
		{
			EZSendMessage(GameWnd, EZWM_USER_NOTIFY, 3, 0);
		}

		return 0;
	case EZWM_SCROLLPOSCHANGE:
		TimerSpace =1000 /((wParam + 50) / 50);
		bResetTimer = TRUE;
		return 0;
	case EZWM_SIZE:
		MoveEZWindow(StartBtn, (ezWnd->Width - 120) / 2, 40, 120, 49, 0);
		MoveEZWindow(PauseContinueBtn, (ezWnd->Width - 120) / 2, 110, 120, 49, 0);
		MoveEZWindow(EndBtn, (ezWnd->Width - 120) / 2, 180, 120, 49, 0);
		MoveEZWindow(SpeedScrollBar, (ezWnd->Width - 200) / 2, 250, 200, 17, 0);
		EZSendMessage(SpeedScrollBar, EZWM_SETSCROLLRANGE,300, 0);
		EZSendMessage(SpeedScrollBar, EZWM_SETSCROLLPOS, 0, 50);
		MoveEZWindow(ScoreText, (ezWnd->Width - 200) / 2, 300, 200, 60, 0);
		return 0;
	}
	return 0;
}


EZWNDPROC QuitMessageBox(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static EZWND BtnYes, BtnNo, StaticText;

	switch (message)
	{
	case EZWM_CREATE:
		BtnYes = CreateEZStyleWindow(ezWnd, TEXT("Yes"), EZS_CHILD | EZS_BUTTON | EZBS_PUSHBUTTON, 85, 105, 74, 26);
		BtnNo = CreateEZStyleWindow(ezWnd, TEXT("No"), EZS_CHILD | EZS_BUTTON | EZBS_PUSHBUTTON, ezWnd->Width - 74 - 85, 105, 74, 26);
		StaticText = CreateEZStyleWindow(ezWnd, TEXT("Really want to quit ?"), EZS_CHILD | EZS_STATIC, 37, 30, ezWnd->Width - 37 * 2, 32);
		FontForm.lfHeight = StaticText->Height;
		EZSendMessage(StaticText, EZWM_SETFONT, 0, &FontForm);
		//EZSendMessage(StaticText, EZWM_SETCOLOR, RGB(40, 40, 40), RGB(0, 0, 0));
		FontForm.lfHeight = BtnYes->Height * 0.8;
		EZSendMessage(BtnYes, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(BtnYes, EZWM_SETCOLOR, RGB(40, 40, 40), RGB(0, 0, 0));
		EZSendMessage(BtnNo, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(BtnNo, EZWM_SETCOLOR, RGB(40, 40, 40), RGB(0, 0, 0));
		return 0;
	case EZWM_DRAW:
		//	PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, BLACKNESS);
		SelectObject(wParam, DefPen);
		Rectangle(wParam, 0, 0, ezWnd->Width, ezWnd->Height);
		return 0;
	case EZWM_COMMAND:
		if (lParam == BtnYes)
		{
			DestroyEZWindow(MainWnd);
		}
		else
		{
			bQuitMsgBox = FALSE;
			EZEndDialog(ezWnd);
		}
		return 0;
	}
	return 0;
}


EZWNDPROC BlockProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int px, py;
	py = ezWnd->ezID / BlkNum;
	px = ezWnd->ezID % BlkNum;
	switch (message)
	{
	case EZWM_DRAW:
		switch (Block[py][px])
		{
		case 0:
			//empty
			break;
		case 1:
			//brick
			PaintBrick(wParam, ezWnd->Width, ezWnd->Height);
			break;
		case 2:
			//snake
			if (SnakeHead->x == px && SnakeHead->y == py)
			{
				PaintSnakeHead(wParam, ezWnd->Width, ezWnd->Height);
			}
			else
			{
				PaintSnakeBody(wParam, ezWnd->Width, ezWnd->Height);
			}
			break;
		case 3:
			PaintFood(wParam, ezWnd->Width, ezWnd->Height);
			break;
		}
		return 0;
	}
	return 0;
}


EZWNDPROC GameoverMessageBox(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static EZWND BtnYes, BtnNo, StaticText;

	switch (message)
	{
	case EZWM_CREATE:
		BtnYes = CreateEZStyleWindow(ezWnd, TEXT("Yes"), EZS_CHILD | EZS_BUTTON | EZBS_PUSHBUTTON, 85, 105, 74, 26);
		BtnNo = CreateEZStyleWindow(ezWnd, TEXT("No"), EZS_CHILD | EZS_BUTTON | EZBS_PUSHBUTTON, ezWnd->Width - 74 - 85, 105, 74, 26);
		StaticText = CreateEZStyleWindow(ezWnd, TEXT("Game over!"), EZS_CHILD | EZS_STATIC, 37, 30, ezWnd->Width - 37 * 2, 32);
		FontForm.lfHeight = StaticText->Height;
		EZSendMessage(StaticText, EZWM_SETFONT, 0, &FontForm);
		//EZSendMessage(StaticText, EZWM_SETCOLOR, RGB(40, 40, 40), RGB(0, 0, 0));
		FontForm.lfHeight = BtnYes->Height * 0.8;
		EZSendMessage(BtnYes, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(BtnYes, EZWM_SETCOLOR, RGB(40, 40, 40), RGB(0, 0, 0));
		EZSendMessage(BtnNo, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(BtnNo, EZWM_SETCOLOR, RGB(40, 40, 40), RGB(0, 0, 0));
		return 0;
	case EZWM_DRAW:
		//	PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, BLACKNESS);
		SelectObject(wParam, DefPen);
		Rectangle(wParam, 0, 0, ezWnd->Width, ezWnd->Height);
		return 0;
	case EZWM_COMMAND:
		if (lParam == BtnYes)
		{
			DestroyEZWindow(MainWnd);
		}
		else
		{
			bQuitMsgBox = FALSE;
			EZEndDialog(ezWnd);
		}
		return 0;
	}
	return 0;
}



int GDIObjInit(HDC hdc)
{
	DefBrush = CreateSolidBrush(DefColor);
	DefPen = CreatePen(PS_SOLID, 1, DefColor);


	FontForm.lfHeight = 0;
	FontForm.lfWidth = 0;
	FontForm.lfEscapement = 0;
	FontForm.lfOrientation = 0;
	FontForm.lfWeight = 0;
	FontForm.lfItalic = 0;
	FontForm.lfUnderline = 0;
	FontForm.lfStrikeOut = 0;
	FontForm.lfCharSet = DEFAULT_CHARSET;
	FontForm.lfOutPrecision = 0;
	FontForm.lfClipPrecision = 0;
	FontForm.lfPitchAndFamily = 0;
	lstrcpy(FontForm.lfFaceName, TEXT("Î¢ÈíÑÅºÚ"));

	hBmpHead = LoadBitmap(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP1));
	GetObject(hBmpHead, sizeof(BITMAP), &BmpHead);
	hdcHead = CreateCompatibleDC(hdc);
	SelectObject(hdcHead, hBmpHead);

	hBmpFood = LoadBitmap(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP2));
	GetObject(hBmpFood, sizeof(BITMAP), &BmpFood);
	hdcFood = CreateCompatibleDC(hdc);
	SelectObject(hdcFood, hBmpFood);

	hBmpBk = LoadBitmap(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP3));
	GetObject(hBmpBk, sizeof(BITMAP), &BmpBk);
	hdcBk = CreateCompatibleDC(hdc);
	SelectObject(hdcBk, hBmpBk);
	return 0;
}


int GDIObjClean()
{
	if (DefBrush)
	{
		DeleteObject(DefBrush);
	}
	if (DefPen)
	{
		DeleteObject(DefPen);
	}

	if (hdcHead)DeleteDC(hdcHead);
	if (hBmpHead)DeleteObject(hBmpHead);

	if (hdcFood)DeleteDC(hdcFood);
	if (hBmpFood)DeleteObject(hBmpFood);

	if (hdcBk)DeleteDC(hdcBk);
	if (hBmpBk)DeleteObject(hBmpBk);
	return 0;
}



int PaintBrick(HDC hdc, int x, int y)
{
	HBRUSH hBrush = CreateSolidBrush(RGB(80, 80, 80));
	SelectObject(hdc, hBrush);

	PatBlt(hdc, 0, 0, x, y, PATCOPY);

	DeleteObject(hBrush);
	return 0;
}


int PaintSnakeBody(HDC hdc, int x, int y)
{
	HBRUSH hBrush = CreateSolidBrush(RGB(136, 0, 21));
	SelectObject(hdc, hBrush);

	PatBlt(hdc, 0, 0, x, y, PATCOPY);

	DeleteObject(hBrush);
	return 0;
}


int PaintSnakeHead(HDC hdc, int x, int y)
{
	SetStretchBltMode(hdc, HALFTONE);
	StretchBlt(hdc, 0, 0, x, y, hdcHead, 0, 0, BmpHead.bmWidth, BmpHead.bmHeight, SRCCOPY);
	return 0;
}


int PaintFood(HDC hdc, int x, int y)
{
	SetStretchBltMode(hdc, HALFTONE);
	StretchBlt(hdc, 0, 0, x, y, hdcFood, 0, 0, BmpFood.bmWidth, BmpFood.bmHeight, SRCCOPY);
	return 0;
}
