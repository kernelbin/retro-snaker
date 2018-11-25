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

//Window Proc
EZWNDPROC MainProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC ControlPanelProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC GameProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC QuitMessageBox(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);


//Global Vars
BOOL bQuitMsgBox;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{

	InitEZWindow();

	CreateEZStyleParentWindow(szAppName, EZS_PARENT | EZS_OVERLAPPEDWINDOW,
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
	switch (message)
	{
	case EZWM_CREATE:

		return 0;
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
	static EZWND BtnYes, BtnNo;
	
	switch (message)
	{
	case EZWM_CREATE:
		
		return 0;
	case EZWM_DRAW:
		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, BLACKNESS);
		return 0;
	case EZWM_COMMAND:

		return 0;
	}
	return 0;
}



int GDIObjInit()
{
	DefBrush = CreateSolidBrush(DefColor);
	DefPen = CreatePen(PS_SOLID, 1, DefColor);
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