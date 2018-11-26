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



//Window Proc
EZWNDPROC MainProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC ControlPanelProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC GameProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC QuitMessageBox(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

EZWNDPROC BlockProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);


//Global Vars
BOOL bQuitMsgBox;



//About the gam
int Block[128][128] = { 0 };
EZWND BlkWnd[128][128] = { 0 };
int BlkNum;

typedef struct __NodeSnake SNAKE, *pSNAKE;
typedef struct __NodeSnake
{
	int x, y;
	pSNAKE n;//Next
}SNAKE, *pSNAKE;


pSNAKE Head;//Snake Head


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
		GDIObjInit();
		bQuitMsgBox = FALSE;
		GameWnd = CreateEZWindow(ezWnd, 0, 0, 0, 0, GameProc);
		ControlWnd = CreateEZWindow(ezWnd, 0, 0, 0, 0, ControlPanelProc);
		break;
	case EZWM_SIZE:
		MoveEZWindow(GameWnd, 0, 0, ezWnd->Width - 250, ezWnd->Height, 0);
		MoveEZWindow(ControlWnd, ezWnd->Width - 250, 0, 250, ezWnd->Height, 0);
		return 0;
	case EZWM_CLOSE:
		if (!bQuitMsgBox)
		{
			bQuitMsgBox = TRUE;
			EZDialogBox(ezWnd, 0, 0, 370, 157, EZDLG_CENTER | EZDLG_MASK, RGB(0, 0, 0), QuitMessageBox);
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
	switch (message)
	{
	case EZWM_CREATE:
		BlkNum = 16;
		for (int py = 0; py < BlkNum; py++)
			for (int px = 0; px < BlkNum; px++)
				BlkWnd[py][px] = CreateEZWindow(ezWnd, 0, 0, 0, 0, BlockProc);
		return 0;
	case EZWM_DRAW:
		SelectObject(wParam, DefBrush);
		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, PATCOPY);
		PatBlt(wParam, xpos, ypos, tlen, tlen, WHITENESS);
		return 0;
	case EZWM_SIZE:
		//calculate zhe size of each block
	{
		
		if (ezWnd->Width > ezWnd->Height)
		{
			xpos = (ezWnd->Width - ezWnd->Height) / 2 + 5;
			ypos = 5;
			tlen = ezWnd->Height - 10;
		}
		else
		{
			xpos = 5;
			ypos = (ezWnd->Height - ezWnd->Width) / 2 + 5;
			tlen = ezWnd->Width - 10;
		}
		//move child windows

		for (int py = 0; py < BlkNum; py++)
			for (int px = 0; px < BlkNum; px++)
				MoveEZWindow(BlkWnd[px][py], xpos + px * tlen / BlkNum, ypos + py * tlen / BlkNum, tlen / BlkNum, tlen / BlkNum, 0);
		return 0;
	}
		
	}
	return 0;
}


EZWNDPROC ControlPanelProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case EZWM_CREATE:

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
		BtnNo  = CreateEZStyleWindow(ezWnd, TEXT("No") , EZS_CHILD | EZS_BUTTON | EZBS_PUSHBUTTON, ezWnd->Width-74-85, 105, 74, 26);
		StaticText = CreateEZStyleWindow(ezWnd, TEXT("Really want to quit ?"), EZS_CHILD | EZS_STATIC, 37, 30, ezWnd->Width-37*2, 32);
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
	switch (message)
	{
	case EZWM_DRAW:
		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, BLACKNESS);
		return 0;
	}
	return 0;
}

int GDIObjInit()
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
	return 0;
}