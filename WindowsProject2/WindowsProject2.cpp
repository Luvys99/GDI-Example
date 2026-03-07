// WindowsProject2.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "WindowsProject2.h"
#include "Render.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HBRUSH g_hTileBrush;                            // 타일에 사용할 브러쉬
HPEN g_hGridPen;                                // 타일에 사용할 펜
char g_Tile[GRID_HEIGHT][GRID_WIDTH];	// 0 장애물 없음, 1 장애물 있음

bool g_bErase = false;
bool g_bDrag = false;

// 더블 버퍼링에 필요한 전역 변수:
HBITMAP g_hMemDCBitmap;
HBITMAP g_hMemDcBitmap_old;
HDC g_hMemDC;
RECT g_MemDCRect;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT2));

    MSG msg;

    // 기본 메시지 루프입니다:
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
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    // WNDCLASSEXW 구조체 포인터를 받아서 WNDCLASSEXW 구조체에 원하는 정보를 설정해서 인자로 넘겨주면 됨
    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

BOOL g_bClick; // 마우스 클릭 여부 판단
HPEN g_hPen;   // 현재 생성된 펜, 휠 버튼 클릭시 새롭게 생성
int g_iOldX;   // 마우스 드래그 표현을 위한 마우스 이동시 이전 좌표
int g_iOldY;   // 마우스 드래그 표현을 위한 마우스 이동시 이전 좌표

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
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
    case WM_LBUTTONDOWN:
        g_bDrag = true;
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            int iTileX = xPos / GRID_SIZE;
            int iTileY = yPos / GRID_SIZE;

            // 첫 선택 타일이 장애물이면 지우기 모드 아니면 장애물 넣기 모드
            if (g_Tile[iTileY][iTileX] == 1)
            {
                g_bErase = true;
            }
            else
            {
                g_bErase = false;
            }

        }
        break;
    case WM_LBUTTONUP:
        g_bDrag = false;
        break;
    case WM_MOUSEMOVE:
    {
        if (g_bDrag)
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            int iTileX = xPos / GRID_SIZE;
            int iTileY = yPos / GRID_SIZE;

            g_Tile[iTileY][iTileX] = !g_bErase;

            // 마우스 드래그로 데이터가 변경되어 갱신을 요청할 시 마지막 Erase 플래그를 false로 하여 화면 깜빡임을 없앤다. 
            // WM_PAINT 에서는 윈도우 전체를 덮어 쓰기 때문에 지우지 않아도 된다.
            InvalidateRect(hWnd, NULL, false);

        }

    }
    break;
    case WM_CREATE:
    {
        g_hGridPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
        g_hTileBrush = CreateSolidBrush(RGB(100, 100, 100));

        //윈도우 생성 시 현 윈도우 크기와 동일한 메모리 DC 생성
        HDC hdc = GetDC(hWnd);
        GetClientRect(hWnd, &g_MemDCRect);
        g_hMemDCBitmap = CreateCompatibleBitmap(hdc, g_MemDCRect.right, g_MemDCRect.bottom);
        g_hMemDC = CreateCompatibleDC(hdc);
        ReleaseDC(hWnd, hdc);
        g_hMemDcBitmap_old = (HBITMAP)SelectObject(g_hMemDC, g_hMemDCBitmap);
    }
    break;
    // 기존에는 윈도우 DC, hdc를 대상으로 출력하였으나 이제는 메모리 DC 를 대상으로 출력한다.
    case WM_PAINT:
    {
        // 메모리 DC를 클리어 하고
        PatBlt(g_hMemDC, 0, 0, g_MemDCRect.right, g_MemDCRect.bottom, WHITENESS);

        // RenderObstacle, RenderGrid 를 메모리 DC에 출력
        RenderObstacle(g_hMemDC);
        RenderGrid(g_hMemDC);

        // 메모리 DC에 랜더링이 끝나면, 메모리 DC -> 윈도우 DC로의 출력
        hdc = BeginPaint(hWnd, &ps);
        BitBlt(hdc, 0, 0, g_MemDCRect.right, g_MemDCRect.bottom, g_hMemDC, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        break;
    }
    break;
    // 윈도우 크기가 변경될 때마다 기존 메모리 DC를 지우고 새 크기의 메모리 DC를 만들어 주면 됨
    case WM_SIZE:
    {
        // 기존 메모리 DC 지우기
        SelectObject(g_hMemDC, g_hMemDcBitmap_old);
        DeleteObject(g_hMemDC);
        DeleteObject(g_hMemDCBitmap);

        // 새 크기의 메모리 DC 생성
        HDC hdc = GetDC(hWnd);
        GetClientRect(hWnd, &g_MemDCRect);
        g_hMemDCBitmap = CreateCompatibleBitmap(hdc, g_MemDCRect.right, g_MemDCRect.bottom);
        g_hMemDC = CreateCompatibleDC(hdc);
        ReleaseDC(hWnd, hdc);
        g_hMemDcBitmap_old = (HBITMAP)SelectObject(g_hMemDC, g_hMemDCBitmap);
     
    }
    break;
    case WM_SYSCOMMAND: // 종료시 확인 메시지 받아서 죵료 할지 말지 결정하는 메시지
        switch (wParam)
        {
        case SC_CLOSE:
            if (IDOK == MessageBox(hWnd, L"종료하시겠습니까?", L"종료 메시지", MB_OKCANCEL))
            {
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_DESTROY: // 윈도우 종료시 발생하는 메시지 ( 필 수 )
        SelectObject(g_hMemDC, g_hMemDcBitmap_old);
        DeleteObject(g_hTileBrush);
        DeleteObject(g_hGridPen);
        PostQuitMessage(0); // WM_QUIT 메시지를 발생시키는 함수로서 메시지 루프의 종료를 유도
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
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
