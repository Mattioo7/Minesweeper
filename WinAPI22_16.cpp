// WinAPI22_16.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WinAPI22_16.h"
#include <random>
#include <time.h>
#include <synchapi.h>

#define MAX_LOADSTRING 100
#define MAKE_TILES_TIMER_ID 1	
#define CHILD_SIZE_IN_PX 25
#define MAX_HEIGHT 30
#define MAX_WIDTH 24

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
int xTilesNumber = 10;							// number of tiles in x 
int yTilesNumber = 10;							// number of tiles in y 
int howManyMines = 10;							// number of mines
BOOL isChecked = FALSE;							// Debug button functionality
double timer = 0;								// temporary timer
BOOL stopTimer = TRUE;							// for starting/stoping timer
BOOL gameStarted = FALSE;						// was game started?
HWND TilesTable[MAX_HEIGHT][MAX_WIDTH];			// table of handles to tiles
BOOL MinesTable[MAX_HEIGHT][MAX_WIDTH];			// table for mines
int enemiesAround[MAX_HEIGHT][MAX_WIDTH];		// table for number of mines around you
HWND hWnd;										// main window handle
int flagCounter = 10;							// flag counter
int minesFound;									// how many mines has been found
BOOL gameOver = FALSE;							// TRUE if stepped on mine
int tickCountInMiliseconds;						// stores miliseconds from system start on game start
BOOL gameEnded = FALSE;							// if TRUE it blocks mouse from clicking tiles
BOOL debugMode = FALSE;							// is debug mode on?
int uncoverdTiles = 100;						// uncovered tiles

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                MyRegisterChildClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
BOOL                InitTilesInstance(HINSTANCE hInstance, int nCmdShow, HWND hWnd, int x, int y, int i, int j);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndTileProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	CustomSize(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void MakeTiles(HWND hWnd);						// creates all tiles
void RandomNumberOfTiles();						// set new random value for 
void KillTiles();								// killing all tiles for new game
void NewGame(int random = 0);					// create new game
void DisplayFlagCounter();						// function for displaying flag counter
void DisplayTimer();							// function that displays timer
void ResetMinesPlacement();						// filling MinesTable with FALSE;
void PlaceMines();								// placing mines in MinesTable
void Draw_Flague(HWND hWnd);					// draws flag in window hWnd
void RestoreGrey(HWND hWnd);					// undo drawing flag
void PaintMine(HWND hWnd);						// paint mine
void Uncover(HWND hWnd);						// uncovers empty tile
void LookForEmptyTiles(int i, int j);			// seeks and uncovers tiles
void clearEnemies();							// clears table of mines around you
void detectEnemies();							// calculate number of mines aourd you
void PaintNumber(HWND hWnd);					// paint number of mines around you
void WinGame();									// does all the work related to win
void LoseGame();								// does all the work related to step on mine
void DebugON();									// function for debug mode ON
void DebugOFF();								// function for debug mode OFF

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINAPI2216, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	MyRegisterChildClass(hInstance); // register child

	// for randomize number of squares
	srand(time(NULL));

	//NewGame();

	//RandomNumberOfTiles();

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPI2216));

	MSG msg;


	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINAPI2216));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINAPI2216);
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

// Class for tiles
ATOM MyRegisterChildClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndTileProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_WINAPI2216)); // here ?....
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(0x6D, 0x6D, 0x6D)); //(HBRUSH)(COLOR_GRAYTEXT + 1); //
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINAPI2216); // ....
	wcex.lpszClassName = L"ChildClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   // calculate size of the window
   int xSize = (xTilesNumber * (CHILD_SIZE_IN_PX + 1)) + 15;
   int ySize = (yTilesNumber * (CHILD_SIZE_IN_PX + 1)) - 2 + 60 + 30;

   // get coorinates for centering window
   int xPos = GetSystemMetrics(SM_CXSCREEN) / 2 - xSize / 2;
   int yPos = GetSystemMetrics(SM_CYSCREEN) / 2 - ySize / 2;

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
	   xPos, yPos, xSize, ySize, nullptr, nullptr, hInstance, nullptr);


   if (!hWnd)
   {
	  return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   // making tiles
   MakeTiles(hWnd);

   DisplayFlagCounter();
   DisplayTimer();

   ResetMinesPlacement();
   PlaceMines();

   clearEnemies();
   detectEnemies();

   return TRUE;
}

// init child
BOOL InitTilesInstance(HINSTANCE hInstance, int nCmdShow, HWND hWnd, int x, int y, int i, int j)
{	
	// 
	RECT rc;
	GetClientRect(hWnd, &rc);

	HWND child = CreateWindowW(L"ChildClass", L"xyz", WS_CHILD | WS_VISIBLE,
		x, y, CHILD_SIZE_IN_PX, CHILD_SIZE_IN_PX, hWnd, nullptr, hInst, nullptr);

	if (!child)
	{
		return FALSE;
	}

	TilesTable[j][i] = child;
	SetProp(TilesTable[j][i], _T("i_idx"), (HANDLE)i);
	SetProp(TilesTable[j][i], _T("j_idx"), (HANDLE)j);
	SetProp(TilesTable[j][i], _T("isUncovered"), (HANDLE)false);
	SetProp(TilesTable[j][i], _T("hasFlag"), (HANDLE)false);

	ShowWindow(child, nCmdShow);
	UpdateWindow(child);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		SetTimer(hWnd, 1, 30, 0);
		tickCountInMiliseconds = GetTickCount64();
		break;
	}
	case WM_TIMER:
		TCHAR s[256];
		if (stopTimer == FALSE)
		{
			//int deltaTime = 100; // GetTickCount() - tickCountInMiliseconds;
			//timer += 0.032;// (deltaTime / 1000);
			timer = (GetTickCount64() - tickCountInMiliseconds)/1000.0;
			DisplayTimer();
			//tickCountInMiliseconds = GetTickCount64();
		}
		break;
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case ID_F2:
			case ID_GAME_NEWGAME:
			{
				//KillTiles();
				NewGame();
			}
				break;
			case ID_F11:
			case ID_F12:
			case ID_HELP_DEBUGF12:
				if (isChecked == FALSE)
				{
					CheckMenuItem(GetMenu(hWnd), ID_HELP_DEBUGF12, MF_CHECKED);
					// MessageBox(hWnd, L"Debug mode: ON", L"Debug mode", MB_OK | MB_APPLMODAL);
					isChecked = TRUE;
					debugMode = TRUE;
					DebugON();
				}
				else
				{
					CheckMenuItem(GetMenu(hWnd), ID_HELP_DEBUGF12, MF_UNCHECKED);
					// MessageBox(hWnd, L"Debug mode: OFF", L"Debug mode", MB_OK | MB_APPLMODAL);
					isChecked = FALSE;
					debugMode = FALSE;
					DebugOFF();
				}
				break;
			case ID_GAME_CUSTOMSIZE:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_BOARDSIZE), hWnd, CustomSize);
				break;
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case ID_GAME_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...

			if (debugMode == TRUE)
			{
				DebugON();
			}
			else
			{
				DebugOFF();
			}
			DisplayTimer();
			DisplayFlagCounter();

			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	/*case WM_ERASEBKGND:
		return -1;
		break;*/
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// init child
LRESULT CALLBACK WndTileProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		if (gameEnded == TRUE) break;
		
		// starting the timer
		if (stopTimer == TRUE && gameStarted == FALSE)
		{
			stopTimer = FALSE;
			gameStarted = TRUE;
			tickCountInMiliseconds = GetTickCount64();
		}

		int a = uncoverdTiles;

		int i = (int)GetProp(hWnd, _T("i_idx"));
		int j = (int)GetProp(hWnd, _T("j_idx"));
		
		// if tile is covered and doesn't has a flag and we have at least 1 flag
		if (!GetProp(hWnd, _T("isUncovered")) && !GetProp(hWnd, _T("hasFlag")) && flagCounter > 0)
		{			
			if (MinesTable[j][i] == TRUE)
			{
				minesFound++;
			}

			flagCounter--;
			DisplayFlagCounter();
			Draw_Flague(hWnd);
			
			if (minesFound >= howManyMines)
			{
				WinGame();
			}
		}
		else if (!GetProp(hWnd, _T("isUncovered")) && GetProp(hWnd, _T("hasFlag")))
		{
			RestoreGrey(hWnd);
			uncoverdTiles++;
			flagCounter++;
			DisplayFlagCounter();
			SetProp(TilesTable[j][i], _T("hasFlag"), (HANDLE)false);
			SetProp(TilesTable[j][i], _T("isUncovered"), (HANDLE)false);

			// delete gdi



			if (MinesTable[j][i] == TRUE)
			{
				minesFound--;
			}

			if (debugMode == TRUE)
			{
				if (MinesTable[j][i] == TRUE)
				{
					PaintMine(TilesTable[j][i]);
				}
				else if (enemiesAround[j][i] > 0)
				{
					PaintNumber(TilesTable[j][i]);
				}
			}
		}
	}
		break;
	case WM_LBUTTONDOWN:
	{
		if (gameEnded == TRUE) break;
		
		if (stopTimer == TRUE && gameStarted == FALSE)
		{
			stopTimer = FALSE;
			gameStarted = TRUE;
			tickCountInMiliseconds = GetTickCount64();
		}
		
		int i = (int)GetProp(hWnd, _T("i_idx"));
		int j = (int)GetProp(hWnd, _T("j_idx"));

		if (!GetProp(hWnd, _T("isUncovered")) && MinesTable[j][i] == TRUE && !GetProp(hWnd, _T("hasFlag")))
		{
			gameOver = TRUE;
			stopTimer = TRUE;
			
			SetProp(TilesTable[j][i], _T("isUncovered"), (HANDLE)true);
			Uncover(hWnd);
			PaintMine(hWnd);

			// end
			LoseGame();
		}
		else if (!GetProp(hWnd, _T("isUncovered")) && !GetProp(hWnd, _T("hasFlag")) && MinesTable[j][i] == FALSE)
		{
			SetProp(TilesTable[j][i], _T("isUncovered"), (HANDLE)true);
			
			Uncover(hWnd);
			uncoverdTiles--;

			int a = uncoverdTiles;


			if (uncoverdTiles <= howManyMines - minesFound)
			{
				WinGame();
			}

			if (enemiesAround[j][i] > 0)
			{
				PaintNumber(hWnd);
			}
			else
			{
				LookForEmptyTiles(i, j);
			}
		}
		else
		{
			//uncover(hWnd);
		}
	}
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...

		int i = (int)GetProp(hWnd, _T("i_idx"));
		int j = (int)GetProp(hWnd, _T("j_idx"));

		// if no flag and covered
		if (!GetProp(hWnd, _T("hasFlag")) && !GetProp(hWnd, _T("isUncovered")))
		{
			RestoreGrey(hWnd);
		}

		// if has flag and is uncovered
		if (GetProp(hWnd, _T("hasFlag")) && !GetProp(hWnd, _T("isUncovered")))
		{
			Draw_Flague(hWnd);
		}

		// if no flag and uncovered
		if (!GetProp(hWnd, _T("hasFlag")) && GetProp(hWnd, _T("isUncovered")))
		{
			Uncover(hWnd);
		}

		// if has mine and uncovered
		if (MinesTable[j][i] == TRUE && GetProp(hWnd, _T("isUncovered")))
		{
			Uncover(hWnd);
			PaintMine(hWnd);
		}

		// if uncovered and has number and no mine
		if (GetProp(hWnd, _T("isUncovered")) && enemiesAround[j][i] > 0 && MinesTable[j][i] == FALSE)
		{
			Uncover(hWnd);
			PaintNumber(hWnd);
		}

		if (debugMode == TRUE)
		{
			if (MinesTable[j][i] == FALSE && enemiesAround[j][i] > 0)
			{
				PaintNumber(hWnd);
			}
			if (MinesTable[j][i] == TRUE)
			{
				PaintMine(hWnd);
			}
		}

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		//PostQuitMessage(0);
		break;
	/*case WM_ERASEBKGND:
		return -1;
		break;*/
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Message handler for custom size box.
INT_PTR CALLBACK CustomSize(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_HEIGHT, yTilesNumber, 0);
		SetDlgItemInt(hDlg, IDC_WIDTH, xTilesNumber, 0);
		SetDlgItemInt(hDlg, IDC_MINES, howManyMines, 0);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			yTilesNumber = GetDlgItemInt(hDlg, IDC_HEIGHT, 0, 0);
			if (yTilesNumber > 30) yTilesNumber = 30;
			if (yTilesNumber < 5) yTilesNumber = 5;
			xTilesNumber = GetDlgItemInt(hDlg, IDC_WIDTH, 0, 0);
			if (xTilesNumber > 24) xTilesNumber = 24;
			if (xTilesNumber < 5) xTilesNumber = 5;
			howManyMines = GetDlgItemInt(hDlg, IDC_MINES, 0, 0);
			if (howManyMines < 10) howManyMines = 10;
			if (howManyMines > yTilesNumber * xTilesNumber) howManyMines = yTilesNumber * xTilesNumber;

			uncoverdTiles = yTilesNumber * xTilesNumber;

			EndDialog(hDlg, LOWORD(wParam));

			NewGame(1);

			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	case WM_GETTEXT:
		//TCHAR s[5];
		// _stprintf_s(s, 5, TEXT("%04d"), flagCounter);
		// int number = atoi((char*)lParam);

		// http://www.winprog.org/tutorial/controls.html
		//int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_HEIGHT));
		//if (len > 0)
		//{
			
			//LPWSTR* buf;

			//buf = (LPWSTR*)GlobalAlloc(GPTR, len + 1);
			int i = GetDlgItemInt(hDlg, IDC_HEIGHT, 0, 0);

			//... do stuff with text ...

			//GlobalFree((HANDLE)buf);
		//}

		break;
	}
	return (INT_PTR)FALSE;
}

void MakeTiles(HWND hWnd)
{
	int x;
	int y;

	for (int j = 0; j < yTilesNumber; ++j)
	{
		for (int i = 0; i < xTilesNumber; ++i)
		{
			x = i * (CHILD_SIZE_IN_PX + 1);
			y = j * (CHILD_SIZE_IN_PX + 1) + 30;
			InitTilesInstance(hInst, SW_NORMAL, hWnd, x, y, i, j);
		}
	}
	// KillTimer(hWnd, MAKE_TILES_TIMER_ID);
	uncoverdTiles = yTilesNumber * xTilesNumber;
}

void RandomNumberOfTiles()
{
	//yTilesNumber = rand() % 21 + 10;
	//xTilesNumber = rand() % 15 + 10;

	yTilesNumber = 10;
	xTilesNumber = 10;

	uncoverdTiles =  yTilesNumber * xTilesNumber;
}

void KillTiles()
{
	for (int j = 0; j < yTilesNumber; ++j)
	{
		for (int i = 0; i < xTilesNumber; ++i)
		{
			DestroyWindow(TilesTable[j][i]);
		}
	}
}

void NewGame(int random)
{
	KillTiles();
	
	/*if (random == 0)
		RandomNumberOfTiles();*/

	// timer reset
	timer = 0;
	stopTimer = TRUE;
	gameStarted = FALSE;
	gameEnded = FALSE;

	// reset flags
	flagCounter = 10;
	minesFound = 0;

	// calculate size of the window
	int xSize = (xTilesNumber * (CHILD_SIZE_IN_PX + 1)) + 15;
	int ySize = (yTilesNumber * (CHILD_SIZE_IN_PX + 1)) - 2 + 60 + 30;

	// get coorinates for centering window
	int xPos = GetSystemMetrics(SM_CXSCREEN) / 2 - xSize / 2;
	int yPos = GetSystemMetrics(SM_CYSCREEN) / 2 - ySize / 2;

	MoveWindow(hWnd, xPos, yPos, xSize, ySize, TRUE);	// maybe this should be at the end?

	MakeTiles(hWnd);
	DisplayFlagCounter();
	DisplayTimer();

	ResetMinesPlacement();
	PlaceMines();

	clearEnemies();
	detectEnemies();

	gameOver = FALSE;
	uncoverdTiles = yTilesNumber * xTilesNumber;

	isChecked = FALSE;
	debugMode = FALSE;
	CheckMenuItem(GetMenu(hWnd), ID_HELP_DEBUGF12, MF_UNCHECKED);
}

void DisplayFlagCounter()
{
	HDC hdc = GetDC(hWnd);

	SetTextColor(hdc, RGB(255, 0, 0));

	HFONT font = CreateFont(
		25,	// Height
		0,													// Width
		0,													// Escapement
		0,													// Orientation
		FW_BOLD,											// Weight
		false,												// Italic
		FALSE,												// Underline
		0,													// StrikeOut
		EASTEUROPE_CHARSET,									// CharSet
		OUT_DEFAULT_PRECIS,									// OutPrecision
		CLIP_DEFAULT_PRECIS,								// ClipPrecision
		DEFAULT_QUALITY,									// Quality
		DEFAULT_PITCH | FF_SWISS,							// PitchAndFamily
		_T("Arial"));										// Facename

	HFONT oldFont = (HFONT)SelectObject(hdc, font);
	
	RECT rc;
	GetClientRect(hWnd, &rc);
	TCHAR s[5];
	_stprintf_s(s, 5, TEXT("%04d"), flagCounter);
	TextOut(hdc, ((rc.right - rc.left) * 3 / 4) - 30, 5, s, (int)_tcslen(s));
	ReleaseDC(hWnd, hdc);
	SelectObject(hdc, oldFont);
	DeleteObject(font);

	SetTextColor(hdc, RGB(0, 0, 0));
}

void DisplayTimer()
{
	HDC hdc = GetDC(hWnd);

	SetTextColor(hdc, RGB(255, 0, 0));

	HFONT font = CreateFont(
		25,	// Height
		0,													// Width
		0,													// Escapement
		0,													// Orientation
		FW_BOLD,											// Weight
		false,												// Italic
		FALSE,												// Underline
		0,													// StrikeOut
		EASTEUROPE_CHARSET,									// CharSet
		OUT_DEFAULT_PRECIS,									// OutPrecision
		CLIP_DEFAULT_PRECIS,								// ClipPrecision
		DEFAULT_QUALITY,									// Quality
		DEFAULT_PITCH | FF_SWISS,							// PitchAndFamily
		_T("Arial"));										// Facename

	HFONT oldFont = (HFONT)SelectObject(hdc, font);
	
	RECT rc;
	GetClientRect(hWnd, &rc);
	TCHAR s[17];
	_stprintf_s(s, 17, TEXT("%06.1f"), timer);
	//HDC hdc = GetDC(hWnd);
	TextOut(hdc, ((rc.right - rc.left) / 4) - 25, 5, s, (double)_tcslen(s));
	ReleaseDC(hWnd, hdc);
	SelectObject(hdc, oldFont);
	DeleteObject(font);

	SetTextColor(hdc, RGB(0, 0, 0));
}

void ResetMinesPlacement()
{
	for (int j = 0; j < yTilesNumber; ++j)
	{
		for (int i = 0; i < xTilesNumber; ++i)
		{
			MinesTable[j][i] = FALSE;
		}
	}
}

void PlaceMines()
{
	int i = 0;
	int x, y;

	while (i++ < howManyMines)
	{
		do {
			y = rand() % yTilesNumber;
			x = rand() % xTilesNumber;
		} while (MinesTable[y][x] == TRUE);
		MinesTable[y][x] = TRUE;
	}
}

void Draw_Flague(HWND hWnd)
{
	HDC hdc = GetDC(hWnd);
	HBITMAP bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(129));
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);
	BITMAP bmInfo;
	GetObject(bitmap, sizeof(bmInfo), &bmInfo);
	int a = uncoverdTiles;//
	int b = a;//
	BitBlt(hdc, 0, 0, 20, 20, memDC, 0, 0, SRCCOPY);
	StretchBlt(hdc, 0, 0, 25, 25, memDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, SRCCOPY);
	// SetProp(hWnd, _T("isUncovered"), (HANDLE)true);
	SetProp(hWnd, _T("hasFlag"), (HANDLE)true);
	SelectObject(memDC, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(memDC);
	
	ReleaseDC(hWnd, hdc);
}

void RestoreGrey(HWND hWnd)
{
	HDC hdc = GetDC(hWnd);
	RECT rc;
	GetClientRect(hWnd, &rc);
	HBRUSH brush = CreateSolidBrush(RGB(0x6D, 0x6D, 0x6D)); // (HBRUSH)(COLOR_GRAYTEXT + 1);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
	Rectangle(hdc, -1, -1, rc.right - rc.left + 1, rc.bottom - rc.top + 1);
	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
	ReleaseDC(hWnd, hdc);

	SetProp(hWnd, _T("hasFlag"), (HANDLE)false);
	SetProp(hWnd, _T("isUncovered"), (HANDLE)false);
}

void PaintMine(HWND hWnd) ////
{
	HDC hdc = GetDC(hWnd);
	RECT rc;
	GetClientRect(hWnd, &rc);

	// https://xoax.net/cpp/crs/win32/lessons/Lesson4/
	HPEN hPenOld;

	// Draw a blue ellipse
	HPEN hEllipsePen;
	COLORREF qEllipseColor;

	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

	qEllipseColor = RGB(0, 0, 0);
	hEllipsePen = CreatePen(PS_SOLID, 3, qEllipseColor);
	hPenOld = (HPEN)SelectObject(hdc, hEllipsePen);

	Ellipse(hdc, 5, 5, 20, 20);

	SelectObject(hdc, hPenOld);
	DeleteObject(hEllipsePen);

	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
	ReleaseDC(hWnd, hdc);


	//
	// int a = uncoverdTiles;


}

void Uncover(HWND hWnd)
{
	HDC hdc = GetDC(hWnd);
	RECT rc;
	GetClientRect(hWnd, &rc);
	HBRUSH brush = CreateSolidBrush(RGB(0xAA, 0xAA, 0xAA));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
	Rectangle(hdc, -1, -1, rc.right - rc.left + 1, rc.bottom - rc.top + 1);
	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
	ReleaseDC(hWnd, hdc);

	//Sleep(1);
}

void LookForEmptyTiles(int i, int j)
{
	if (i - 1 >= 0 && !GetProp(TilesTable[j][i - 1], _T("isUncovered")) && MinesTable[j][i - 1] == FALSE)
	{
		SendMessage(TilesTable[j][i - 1], WM_LBUTTONDOWN, 0, 0);
	}

	if (j - 1 >= 0 && !GetProp(TilesTable[j - 1][i], _T("isUncovered")) && MinesTable[j - 1][i] == FALSE)
	{
		SendMessage(TilesTable[j - 1][i], WM_LBUTTONDOWN, 0, 0);
	}

	if (i + 1 < xTilesNumber && !GetProp(TilesTable[j][i + 1], _T("isUncovered")) && MinesTable[j][i + 1] == FALSE)
	{
		SendMessage(TilesTable[j][i + 1], WM_LBUTTONDOWN, 0, 0);
	}
				
	if (j + 1 < yTilesNumber && !GetProp(TilesTable[j + 1][i], _T("isUncovered")) && MinesTable[j + 1][i] == FALSE)
	{
		SendMessage(TilesTable[j + 1][i], WM_LBUTTONDOWN, 0, 0);
	}


	// up-left
	if (i - 1 >= 0 && j - 1 >= 0 && !GetProp(TilesTable[j - 1][i - 1], _T("isUncovered")) && MinesTable[j - 1][i - 1] == FALSE)
	{
		SendMessage(TilesTable[j - 1][i - 1], WM_LBUTTONDOWN, 0, 0);
	}

	// down-left
	if (i - 1 >= 0 && j + 1 < yTilesNumber && !GetProp(TilesTable[j + 1][i - 1], _T("isUncovered")) && MinesTable[j + 1][i - 1] == FALSE)
	{
		SendMessage(TilesTable[j + 1][i - 1], WM_LBUTTONDOWN, 0, 0);
	}

	// up-right
	if (i + 1 < xTilesNumber && j - 1 >= 0 && !GetProp(TilesTable[j - 1][i + 1], _T("isUncovered")) && MinesTable[j - 1][i + 1] == FALSE)
	{
		SendMessage(TilesTable[j - 1][i + 1], WM_LBUTTONDOWN, 0, 0);
	}

	// down-right
	if (i + 1 < xTilesNumber && j + 1 < yTilesNumber && !GetProp(TilesTable[j + 1][i + 1], _T("isUncovered")) && MinesTable[j + 1][i + 1] == FALSE)
	{
		SendMessage(TilesTable[j + 1][i + 1], WM_LBUTTONDOWN, 0, 0);
	}


}

void clearEnemies()
{
	for (int j = 0; j < yTilesNumber; ++j)
	{
		for (int i = 0; i < xTilesNumber; ++i)
		{
			enemiesAround[j][i] = 0;
		}
	}
}

void detectEnemies()
{
	for (int j = 0; j < yTilesNumber; ++j)
	{
		for (int i = 0; i < xTilesNumber; ++i)
		{
			// left
			if (i - 1 >= 0 && MinesTable[j][i - 1] == TRUE)
			{
				enemiesAround[j][i] += 1;
			}

			// up
			if (j - 1 >= 0 && MinesTable[j - 1][i] == TRUE)
			{
				enemiesAround[j][i] += 1;
			}

			// right
			if (i + 1 < xTilesNumber && MinesTable[j][i + 1] == TRUE)
			{
				enemiesAround[j][i] += 1;
			}

			// down
			if (j + 1 < yTilesNumber && MinesTable[j + 1][i] == TRUE)
			{
				enemiesAround[j][i] += 1;
			}

			// up-left
			if (i - 1 >= 0 && j - 1 >= 0 && MinesTable[j - 1][i - 1] == TRUE)
			{
				enemiesAround[j][i] += 1;
			}

			// down-left
			if (i - 1 >= 0 && j + 1 < yTilesNumber && MinesTable[j + 1][i - 1] == TRUE)
			{
				enemiesAround[j][i] += 1;
			}

			// up-right
			if (i + 1 < xTilesNumber && j - 1 >= 0 && MinesTable[j - 1][i + 1] == TRUE)
			{
				enemiesAround[j][i] += 1;
			}

			// down-right
			if (i + 1 < xTilesNumber && j + 1 < yTilesNumber && MinesTable[j + 1][i + 1] == TRUE)
			{
				enemiesAround[j][i] += 1;
			}

		}
	}
}

void PaintNumber(HWND hWnd)
{
	int i = (int)GetProp(hWnd, _T("i_idx"));
	int j = (int)GetProp(hWnd, _T("j_idx"));

	HDC hdc = GetDC(hWnd);
	RECT rc;
	GetClientRect(hWnd, &rc);
	TCHAR s[2];
	_itow_s(enemiesAround[j][i], s, 16);

	if (enemiesAround[j][i] == 1)
	{
		SetTextColor(hdc, RGB(0, 102, 255));
	}
	else if (enemiesAround[j][i] == 2)
	{
		SetTextColor(hdc, RGB(0, 153, 0));
	}
	else if (enemiesAround[j][i] == 3)
	{
		SetTextColor(hdc, RGB(255, 26, 26));
	}
	else if (enemiesAround[j][i] == 4)
	{
		SetTextColor(hdc, RGB(204, 0, 153));
	}
	else if (enemiesAround[j][i] == 5)
	{
		SetTextColor(hdc, RGB(128, 0, 0));
	}
	else if (enemiesAround[j][i] == 6)
	{
		SetTextColor(hdc, RGB(0, 255, 255));
	}
	else if (enemiesAround[j][i] == 7)
	{
		SetTextColor(hdc, RGB(51, 0, 51));
	}
	else if (enemiesAround[j][i] == 8)
	{
		SetTextColor(hdc, RGB(179, 179, 204));
	}
	
	SetBkMode(hdc, TRANSPARENT);

	DrawText(hdc, s, (int)_tcslen(s), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	ReleaseDC(hWnd, hdc);
}

void WinGame()
{
	stopTimer = TRUE;
	gameEnded = TRUE;

	// show messgae box
	MessageBox(hWnd, L"Win", L"Minesweeper", MB_OK | MB_APPLMODAL);
}

void LoseGame()
{
	stopTimer = TRUE;
	gameEnded = TRUE;

	// show messgae box
	MessageBox(hWnd, L"Boom", L"Minesweeper", MB_OK | MB_APPLMODAL | MB_ICONERROR);
}

void DebugON()
{
	for (int j = 0; j < yTilesNumber; ++j)
	{
		for (int i = 0; i < xTilesNumber; ++i)
		{
			HWND hWnd_tile = TilesTable[j][i];
			
			//if (!GetProp(hWnd_tile, _T("hasFlag")))
			{
				if (enemiesAround[j][i] > 0)
				{
					PaintNumber(hWnd_tile);
				}

				if (MinesTable[j][i] == TRUE)
				{
					PaintMine(hWnd_tile);
				}
			}
		}
	}
}

void DebugOFF()
{
	for (int j = 0; j < yTilesNumber; ++j)
	{
		for (int i = 0; i < xTilesNumber; ++i)
		{
			HWND hWnd_tile = TilesTable[j][i];
			
			if (!GetProp(hWnd_tile, _T("hasFlag")) && !GetProp(hWnd_tile, _T("isUncovered")))
			{
				RestoreGrey(hWnd_tile);
			}

			if (GetProp(hWnd_tile, _T("hasFlag")) && !GetProp(hWnd_tile, _T("isUncovered")))
			{
				Draw_Flague(hWnd_tile);
			}

			/*if (!GetProp(hWnd_tile, _T("hasFlag")) && GetProp(hWnd_tile, _T("isUncovered")))
			{
				Uncover(hWnd_tile);
			}*/
		}
	}
}