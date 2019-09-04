//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School.
//
// File Name	: 
// Description	: 
// Author		: Your Name
// Mail			: your.name@mediadesign.school.nz
//

//Library Includes
#include <windows.h>
//#include <vld.h>
#include <windowsx.h>

//Local Includes
#include "Game.h"
#include "Clock.h"
#include "utils.h"
#include "level.h"
#include "playership.h"
#include "resource.h"

const int kiWidth = 1600;
const int kiHeight = 900;

int playerMoveSpeed = 2;
int playerShootSpeed = 2;
int pirateMoveSpeed = 2;
int pirateShootSpeed = 2;
bool setSpeeds = false;

#define WINDOW_CLASS_NAME L"BSENGGFRAMEWORK"

HWND optionsDialog, nameDialog;

LRESULT CALLBACK WindowProc(HWND _hWnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam)
{
	switch (_uiMsg)
	{
	case WM_MOUSEMOVE:
	{
		int iMouseX = LOWORD(_lParam);
		//CGame::GetInstance().GetLevel()->GetPaddle()->SetX(static_cast<float>(iMouseX));
		return (0);
	}
	break;
	case WM_DESTROY:
	{
		//CGame& rGame = CGame::GetInstance();
		CGame::DestroyInstance();
		PostQuitMessage(0);
		return(0);
	}
	break;
	case WM_KEYUP:
	{
		switch (_wParam)
		{
		case VK_F1:
		{
			ShowWindow(optionsDialog, SW_SHOWNORMAL);
			break;
		}
		default:
			break;
		}
		return(0);
	}
	default:
		break;
	}

	return (DefWindowProc(_hWnd, _uiMsg, _wParam, _lParam));
}


BOOL CALLBACK OptionsDlgProc(HWND _hwnd,
	UINT _msg,
	WPARAM _wparam,
	LPARAM _lparam)
{
	switch (_msg)
	{
	case WM_CREATE:
	{
		return(0);
	}
	case WM_COMMAND:
	{
		switch (LOWORD(_wparam))
		{
		case IDC_SLOW1:
		{
			playerMoveSpeed = 1;
			break;
		}
		case IDC_SLOW2:
		{
			playerShootSpeed = 1;
			break;
		}
		case IDC_SLOW3:
		{
			pirateMoveSpeed = 1;
			break;
		}
		case IDC_SLOW4:
		{
			pirateShootSpeed = 1;
			break;
		}
		case IDC_NORMAL1:
		{
			playerMoveSpeed = 2;
			break;
		}
		case IDC_NORMAL2:
		{
			playerShootSpeed = 2;
			break;
		}
		case IDC_NORMAL3:
		{
			pirateMoveSpeed = 2;
			break;
		}
		case IDC_NORMAL4:
		{
			pirateShootSpeed = 2;
			break;
		}
		case IDC_FAST1:
		{
			playerMoveSpeed = 3;
			break;
		}
		case IDC_FAST2:
		{
			playerShootSpeed = 3;
			break;
		}
		case IDC_FAST3:
		{
			pirateMoveSpeed = 3;
			break;
		}
		case IDC_FAST4:
		{
			pirateShootSpeed = 3;
			break;
		}
		case IDBT_CANCEL:
		{
			setSpeeds = false;
			SendMessage(_hwnd, WM_CLOSE, _wparam, _lparam);
			break;
		}
		case IDBT_YARR:
		{
			setSpeeds = true;
			SendMessage(_hwnd, WM_CLOSE, _wparam, _lparam);
			break;
		}
		default:
			break;
		}
		return(0);
	}
	case WM_CLOSE:
	{
		ShowWindow(_hwnd, SW_HIDE);
		return TRUE;
		break;
	}
	default:
		break;
	}
	return FALSE;
}


BOOL CALLBACK NameDlgProc(HWND _hwnd,
	UINT _msg,
	WPARAM _wparam,
	LPARAM _lparam)
{


	switch (_msg)
	{
	case WM_CREATE:
	{
		return(0);
	}
	case WM_COMMAND:
	{
		switch (LOWORD(_wparam))
		{
		case ID_NAMEOK:
		{
			wchar_t _pcValue[20];
			ZeroMemory(_pcValue, 20);
			GetDlgItemText(_hwnd, IDC_EDITNAME, _pcValue, 20);
			std::wstring ws(_pcValue);

			std::string tempString(ws.begin(), ws.end());

			CGame& rGame = CGame::GetInstance();
			rGame.SetCurrentName(tempString);

			SendMessage(_hwnd, WM_CLOSE, _wparam, _lparam);
			break;
		}
		default:
			break;
		}
		return(0);
	}
	case WM_CLOSE:
	{
		ShowWindow(_hwnd, SW_HIDE);

		CGame& rGame = CGame::GetInstance();
		rGame.SaveHighScore();
		return TRUE;
		break;
	}
	default:
		break;
	}
	return FALSE;
}


HWND CreateAndRegisterWindow(HINSTANCE _hInstance, int _iWidth, int _iHeight, const wchar_t* _pcTitle)
{
	WNDCLASSEX winclass;

	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = _hInstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = static_cast<HBRUSH> (GetStockObject(NULL_BRUSH));
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&winclass))
	{
		// Failed to register.
		return (0);
	}

	HWND hwnd;
	hwnd = CreateWindowEx(NULL,
		WINDOW_CLASS_NAME,
		_pcTitle,
		WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		_iWidth, _iHeight,
		NULL,
		NULL,
		_hInstance,
		NULL);

	if (!hwnd)
	{
		// Failed to create.
		return (0);
	}

	return (hwnd);
}

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdline, int _iCmdshow)
{
	MSG msg;
	RECT _rect;
	ZeroMemory(&msg, sizeof(MSG));

	HWND hwnd = CreateAndRegisterWindow(_hInstance, kiWidth, kiHeight, L"Cargo Pirates");

	CGame& rGame = CGame::GetInstance();
	
	GetClientRect(hwnd, &_rect);

	//if (!rGame.Initialise(_hInstance, hwnd, kiWidth, kiHeight))
	if (!rGame.Initialise(_hInstance, hwnd, _rect.right, _rect.bottom))
	{
		// Failed
		return (0);
	}

	//Create modeless dialog box
	ShowCursor(true);
	optionsDialog = CreateDialog(_hInstance, MAKEINTRESOURCE(IDD_OPTIONS), hwnd, (DLGPROC)OptionsDlgProc);
	nameDialog = CreateDialog(_hInstance, MAKEINTRESOURCE(IDD_ENTERNAME), hwnd, (DLGPROC)NameDlgProc);

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (setSpeeds)
			{
				rGame.SetSpeeds(playerMoveSpeed, playerShootSpeed, pirateMoveSpeed, pirateShootSpeed);
				setSpeeds = false;
			}

			if (rGame.GetOpenName() == true)
			{
				ShowWindow(nameDialog, SW_SHOWNORMAL);
				rGame.SetOpenName(false);
			}

			rGame.ExecuteOneFrame();
		}
	}

	//rGame.DestroyInstance();

	return (static_cast<int>(msg.wParam));
}
