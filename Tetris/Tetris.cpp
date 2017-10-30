// Tetris.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Tetris.h"

#define MAX_LOADSTRING 100

// Размеры окна
const int WND_WIDTH = 500;
const int WND_HEIGHT = 480;

const int BORDER_PADDING = 10;

// Интервалы таймера
const int TIMER_INTERVAL = 800;
const int TIMER_INPUT_INTERVAL = TIMER_INTERVAL / 3;

// Клавиши
const int N_KEYS_TOTAL = 256;
const int KEY_LEFT = 37;
const int KEY_UP = 38;
const int KEY_RIGHT = 39;
const int KEY_DOWN = 40;

// Глобальные переменные:
HINSTANCE hInst;
HWND hMainWnd, hNewBtn;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
UINT hTimer = NULL;
int ticks;

// Нажатые клавиши
bool pressedKeys[N_KEYS_TOTAL];
bool keyHandled;

// Ресурсы
HPEN cellPen;
HBRUSH backgroundBrush, figureBrush, fallingBrush;

// Определения функций
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

void UpdateState(HWND, UINT, UINT_PTR, DWORD);
void GameOver();

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                       LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TETRIS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TETRIS));

	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

// Зарегистрировать класс окна
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TETRIS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TETRIS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

// Инициализировать окно
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    // Разместить окно в центре
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int windowX = (screenWidth - WND_WIDTH) / 2;
    int windowY = (screenHeight - WND_HEIGHT) / 2;

    DWORD windowStyle = WS_OVERLAPPEDWINDOW & (~WS_MAXIMIZEBOX);

    hMainWnd = CreateWindow(szWindowClass, szTitle, windowStyle,
        windowX, windowY, WND_WIDTH, WND_HEIGHT, NULL, NULL, hInstance, NULL);

    if (!hMainWnd)
    {
        return FALSE;
    }

    hNewBtn = CreateWindow(TEXT("BUTTON"), TEXT("Начать новую игру"),
        WS_CHILD | BS_DEFPUSHBUTTON,
        WND_WIDTH / 2, 150, 200, 32,
        hMainWnd, NULL, hInst, NULL);

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

    return TRUE;
}

// Инициализировать ресурсы
void InitializeResources(HWND hWnd)
{
    srand(GetTickCount());
    cellPen = CreatePen(PS_SOLID, 1, 0x888888);
    backgroundBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    figureBrush = CreateSolidBrush(0xee44aa);
    fallingBrush = CreateSolidBrush(0x8844ee);
}

// Отобразить очки и справочную информацию
void PaintHUD(HDC hdc)
{
    const int PREVIEW_SIZE = 10;
    const int SPACE = 30;

    TCHAR scoreMsg[32];
    wsprintf((LPWSTR)scoreMsg, TEXT("Счет: %d"), score);

    RECT textRect = {
        WND_WIDTH / 2, BORDER_PADDING,
        WND_WIDTH / 2 + 200, BORDER_PADDING + SPACE
    };

    DrawText(hdc, scoreMsg, wcslen(scoreMsg), &textRect, DT_LEFT);

    textRect.top += SPACE;
    textRect.bottom += SPACE;

    TCHAR* nextMsg = TEXT("Следующая фигура:");
    DrawText(hdc, nextMsg, wcslen(nextMsg), &textRect, DT_LEFT);

    // Отобразить следующую фигуру
    int previewX = textRect.left;
    int previewY = textRect.top + SPACE;

    RECT previewBack = { 
        previewX, previewY,
        previewX + PREVIEW_SIZE * FIG_SIZE, previewY + PREVIEW_SIZE * FIG_SIZE
    };
    FillRect(hdc, &previewBack, backgroundBrush);

    for (int i = 0; i < FIG_SIZE; i++)
    {
        for (int j = 0; j < FIG_SIZE; j++)
        {
            if (nextFigure[j][i] == 0)
                continue;

            int pcx = previewX + PREVIEW_SIZE * i;
            int pcy = previewY + PREVIEW_SIZE * j;

            RECT cellRect = { pcx, pcy, pcx + PREVIEW_SIZE, pcy + PREVIEW_SIZE };
            FillRect(hdc, &cellRect, fallingBrush);
        }
    }
}

// Отрисовать всю графику
void PaintWindow(HWND hWnd, HDC hdc)
{
    RECT client;
    GetClientRect(hWnd, &client);

    int fieldHeight = client.bottom - BORDER_PADDING * 2;
    int cellSize = fieldHeight / GAME_ROWS;
    int fieldWidth = cellSize * GAME_COLS;

    // Фон
    SelectObject(hdc, backgroundBrush);
    RECT fieldRect = {
        BORDER_PADDING, BORDER_PADDING,
        BORDER_PADDING + fieldWidth, BORDER_PADDING + fieldHeight
    };
    FillRect(hdc, &fieldRect, backgroundBrush);

    // Сетка
    SelectObject(hdc, cellPen);

    for (int i = 0; i < GAME_ROWS + 1; i++)
    {
        int yPoint = BORDER_PADDING + cellSize * i;
        MoveToEx(hdc, BORDER_PADDING, yPoint, NULL);
        LineTo(hdc, BORDER_PADDING + fieldWidth, yPoint);
    }

    for (int i = 0; i < GAME_COLS + 1; i++)
    {
        int xPoint = BORDER_PADDING + cellSize * i;
        MoveToEx(hdc, xPoint, BORDER_PADDING, NULL);
        LineTo(hdc, xPoint, BORDER_PADDING + fieldHeight);
    }

    // Фигуры
    SelectObject(hdc, figureBrush);

    for (int i = 0; i < GAME_ROWS; i++)
    {
        for (int j = 0; j < GAME_COLS; j++)
        {
            bool inFalling = false;

            if ((j >= figOffsetX && j < figOffsetX + FIG_SIZE) &&
                (i >= figOffsetY && i < figOffsetY + FIG_SIZE))
            {
                int figX = j - figOffsetX;
                int figY = i - figOffsetY;
                inFalling = figure[figY][figX] != 0;
            }

            if (field[i][j] != 0 || inFalling)
            {
                int cellX = BORDER_PADDING + j * cellSize;
                int cellY = BORDER_PADDING + i * cellSize;

                RECT block = {
                    cellX + 2, cellY + 2,
                    cellX + cellSize - 1, cellY + cellSize - 1
                };
                FillRect(hdc, &block, inFalling ? fallingBrush : figureBrush);
            }
        }
    }

    // Интерфейс
    PaintHUD(hdc);
}

// Вызвать перерисовку окна
void ForceRepaint()
{
    LPRECT client = NULL;
    GetClientRect(hMainWnd, client);
    InvalidateRect(hMainWnd, client, TRUE);
}

// Обработчик нажатых клавиш 
bool HandleKeys()
{
    if (pressedKeys[KEY_LEFT])
    {
        MoveFigure(-1);
        return true;
    }
    else if (pressedKeys[KEY_RIGHT])
    {
        MoveFigure(1);
        return true;
    }
    else if (pressedKeys[KEY_UP])
    {
        RotateFigure();
        return true;
    }
    else if (pressedKeys[KEY_DOWN])
    {
        DropFigure();
        ticks = TIMER_INTERVAL - TIMER_INPUT_INTERVAL;
        return true;
    }

    return false;
}

// Обработать нажатие клавиши сразу после нажатия
void TryHandleKeyImmediately(int keyCode)
{
    if (!pressedKeys[keyCode] && !keyHandled)
    {
        pressedKeys[keyCode] = true;
        HandleKeys();
        keyHandled = true;
    }
}

// Событие таймера
void UpdateState(HWND hWnd, UINT id, UINT_PTR msg, DWORD param)
{
    ticks += TIMER_INPUT_INTERVAL;

    bool updated = keyHandled;

    if (!keyHandled)
    {
        updated = HandleKeys();
    }
    keyHandled = false;

    if (ticks > TIMER_INTERVAL)
    {
        UpdateGame();
        updated = true;
        ticks = 0;
    }

    if (updated)
    {
        ForceRepaint();
    }
}

// Начать новую игру
void StartNewGame()
{
    // Таймер
    if (hTimer != NULL)
    {
        KillTimer(NULL, hTimer);
    }
    hTimer = SetTimer(NULL, NULL, TIMER_INPUT_INTERVAL, (TIMERPROC)UpdateState);
    ticks = 0;

    // Клавиши
    for (int i = 0; i < N_KEYS_TOTAL; i++)
    {
        pressedKeys[i] = false;
    }
    keyHandled = false;

    InitializeGame((ENDGAMEPROC)GameOver);
    ShowWindow(hNewBtn, SW_HIDE);
    ForceRepaint();
}

// Конец игры
void GameOver()
{
    ForceRepaint();
    KillTimer(NULL, hTimer);
    hTimer = NULL;
    
    TCHAR* text[32];
    wsprintf((LPWSTR)text, TEXT("Игра окончена. Счет: %d"), score);

    MessageBox(hMainWnd, (LPCWSTR)text, szTitle, MB_OK | MB_ICONINFORMATION);

    ShowWindow(hNewBtn, SW_SHOW);
    SetFocus(hNewBtn);
}

// Обработка оконных сообщений
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
    case WM_CREATE:
        InitializeResources(hWnd);
        StartNewGame();
        break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);

        // Кнопки
        if ((HWND)lParam == hNewBtn)
        {
            StartNewGame();
            return true;
        }

		// Меню:
		switch (wmId)
		{
        case IDM_NEWGAME:
            StartNewGame();
            break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
        PaintWindow(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;
    case WM_KEYDOWN:
        TryHandleKeyImmediately((int)wParam);
        break;
    case WM_KEYUP:
        pressedKeys[(int)wParam] = false;
        break;
	case WM_DESTROY:
        DeleteObject(cellPen);
        DeleteObject(figureBrush);
        DeleteObject(fallingBrush);
        DeleteObject(backgroundBrush);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
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
