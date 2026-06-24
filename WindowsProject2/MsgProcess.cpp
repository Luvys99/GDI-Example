#include "MsgProcess.h"
#include "Render.h"
#include "WinMain.h"
#include "ASTAR.h"
#include <windowsx.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_LBUTTONDOWN:  OnLButtonDown(hWnd, lParam); break;
    case WM_LBUTTONUP:    OnLButtonUp(); break;
    case WM_RBUTTONDOWN:  OnRButtonDown(hWnd, lParam); break;
    case WM_RBUTTONUP:    OnRButtonUp(); break;
    case WM_MOUSEMOVE:    OnMouseMove(hWnd, lParam); break;
    case WM_KEYDOWN:      OnKeyDown(hWnd, wParam, lParam); break;
    case WM_KEYUP:        OnKeyUp(hWnd, wParam, lParam); break;
    case WM_TIMER:        OnTimer(hWnd, wParam); break;
    case WM_LBUTTONDBLCLK: OnLButtonBLCLK(hWnd, wParam, lParam); break;
    case WM_RBUTTONDBLCLK: OnRButtonBLCLK(hWnd, wParam, lParam); break;
    case WM_MOUSEWHEEL:   OnMouseWheel(hWnd, wParam, lParam); break;
    case WM_CREATE:       OnCreate(hWnd); break;
    case WM_PAINT:        OnPaint(hWnd); break;
    case WM_SIZE:         OnSize(hWnd); break;
    case WM_SYSCOMMAND:   return OnSysCommand(hWnd, message, wParam, lParam);
    case WM_DESTROY:      OnDestroy(); break;
    default:              return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

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

void OnLButtonDown(HWND hWnd, LPARAM lParam)
{
    g_bDrag = true;

    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);

    // 카메라 오프셋과 스케일 반영
    float currentDrawSize = GRID_SIZE * scale;
    int iTileX = (int)((xPos - camX) / currentDrawSize);
    int iTileY = (int)((yPos - camY) / currentDrawSize);

    // 배열 범위가 벗어나는 클릭 방지
    if (iTileX >= 0 && iTileX < GRID_WIDTH && iTileY >= 0 && iTileY < GRID_HEIGHT)
    {
        // 첫 선택 타일이 장애물이면 지우기 모드 아니면 장애물 넣기 모드
        if (g_Tile[iTileY][iTileX] == 1)
        {
            g_bErase = true;
        }
        else
        {
            g_bErase = false;
        }

        // 누르자마자 즉시 타일이 변하도록 데이터 갱신
        g_Tile[iTileY][iTileX] = !g_bErase;
        InvalidateRect(hWnd, NULL, false);
    }
}

void OnLButtonUp()
{
    g_bDrag = false;
}

void OnRButtonDown(HWND hWnd, LPARAM lParam)
{
    g_bPanDrag = true;
    g_LastMouseX = GET_X_LPARAM(lParam);
    g_LastMouseY = GET_Y_LPARAM(lParam);
    SetCapture(hWnd); // 마우스가 창 밖으로 나가도 캡처 유지
}

void OnRButtonUp()
{
    if (g_bPanDrag)
    {
        g_bPanDrag = false;
        ReleaseCapture(); // 마우스 캡처 해제
    }
}

void OnMouseMove(HWND hWnd, LPARAM lParam)
{
    // 모니터 좌표를 인자로 전달받음
    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);

    // 좌클릭 드래그
    if (g_bDrag)
    {
        // 카메라 오프셋과 스케일 반영
        float currentDrawSize = GRID_SIZE * scale;
        int iTileX = (int)((xPos - camX) / currentDrawSize);
        int iTileY = (int)((yPos - camY) / currentDrawSize);

        // 배열 범위가 벗어나는 클릭 방지
        if (iTileX >= 0 && iTileX < GRID_WIDTH && iTileY >= 0 && iTileY < GRID_HEIGHT)
        {
            g_Tile[iTileY][iTileX] = !g_bErase;
            // 마우스 드래그로 데이터가 변경되어 갱신을 요청할 시 마지막 Erase 플래그를 false로 하여 화면 깜빡임을 없앤다. 
            // WM_PAINT 에서는 윈도우 전체를 덮어 쓰기 때문에 지우지 않아도 된다.
            InvalidateRect(hWnd, NULL, false);
        }
    }
    else if (g_bPanDrag) // 우클릭 드래그
    {
        int deltaX = xPos - g_LastMouseX;
        int deltaY = yPos - g_LastMouseY;

        // 카메라 오프셋에 이동량 누적
        camX += deltaX;
        camY += deltaY;

        // 현재 마우스 위치 갱신
        g_LastMouseX = xPos;
        g_LastMouseY = yPos;

        InvalidateRect(hWnd, NULL, false);
    }
}

void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if (wParam == 'C' || wParam == 'c')
    {
        // 타일, 오픈 리스트, 노드맵 초기화
        memset(g_Tile, 0, sizeof(g_Tile));
        g_jps.ClearPathData();

        //출발지, 목적지 색도 없애기
        g_jps.InitAStarPos();
        g_isfinished = false;

        // 타일 색 초기화
        memset(g_jps.m_visitedOrder, 0, sizeof(g_jps.m_visitedOrder));

        InvalidateRect(hWnd, NULL, false);
    }

    // 스페이스바를 누르면 길찾기 시작
    if (wParam == VK_SPACE)
    {
        // !(lParam & (1 << 30))를 검사하면 키보드 꾹 누름으로 인한 KEYDOWN이 무제한 발생하는 것을 방지
        if (!g_isrunning && !(lParam & (1 << 30)))
        {
            // 중복호출안 되도록 막는 동안 간격을 10ms 정도로 줄임
            SetTimer(hWnd, TIMER_ASTAR_STEP, 100, NULL);
            g_isrunning = true;
        }
    }
}

void OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    // 스페이스바에서 손을 떼면 길찾기 일시정지 (타이머 종료)
    if (wParam == VK_SPACE)
    {
        if (g_isrunning)
        {
            KillTimer(hWnd, TIMER_ASTAR_STEP);
            g_isrunning = false;
        }
    }
}

void OnTimer(HWND hWnd, WPARAM wParam)
{

    if (wParam == TIMER_ASTAR_STEP)
    {
        /*
        // 1스탭 이동 ( 목적지에 도달했으면 true, 아직이면 false )
        bool isfinished = g_astar.FindPathStep();

        // 화면 갱신
        InvalidateRect(hWnd, NULL, TRUE);

        // isfinisged가 true거나 오픈 리스트가 비어있다면 실패 한 것으로 타이머 삭제
        if (isfinished || g_astar.IsEmptyList())
        {
            KillTimer(hWnd, TIMER_ASTAR_STEP);
            g_isrunning = false;
            g_isfinished = true;
        }
        */
        // JPS 1스탭 이동
        bool isfinished = g_jps.FindPathStep();

        // 화면 갱신
        InvalidateRect(hWnd, NULL, TRUE);

        // isfinisged가 true거나 오픈 리스트가 비어있다면 실패 한 것으로 타이머 삭제
        if (isfinished)
        {
            KillTimer(hWnd, TIMER_ASTAR_STEP);
            g_isrunning = false;
            g_isfinished = true;
        }
    }

}

void OnLButtonBLCLK(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

    // 모니터 좌표를 인자로 전달받음
    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);

    // 카메라 오프셋과 스케일 반영
    float currentDrawSize = GRID_SIZE * scale;
    int startX = (int)((xPos - camX) / currentDrawSize);
    int startY = (int)((yPos - camY) / currentDrawSize);

    // ASTAR 출발지 노드 생성
    //g_astar.CreateStartNode(startX, startY);

    // JPS 출발지 노드 생성
    g_jps.CreateStartNode(startX, startY);

    // 더블클릭하면 벽 없애고 출발지 설정
    g_Tile[startY][startX] = 0;

    // 출발지 노드 색 그리기
    InvalidateRect(hWnd, NULL, false);

}

void OnRButtonBLCLK(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

    if (g_jps.GetStartNode() == nullptr)
    {
        return;
    }

    // 모니터 좌표를 인자로 전달받음
    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);

    // 카메라 오프셋과 스케일 반영
    float currentDrawSize = GRID_SIZE * scale;
    int destX = (int)((xPos - camX) / currentDrawSize);
    int destY = (int)((yPos - camY) / currentDrawSize);

    // 더블클릭하면 벽 없애고 목적지 설정
    g_Tile[destY][destX] = 0;

    /*
    // ASTAR 목적지 업데이트
    g_astar.SetDestPos(destX, destY);

    // ASTAR 목적지 좌표를 통해서 H값 계산
    g_astar.GetStartNode()->CalculateHeuristic(destX, destY);

    // ASTAR 오픈 리스트에 노드 집어넣기
    g_astar.PushOpenList(g_astar.GetStartNode());
    */

    // JPS 목적지 업데이트
    g_jps.SetDestPos(destX, destY);

    // JPS 목적지 좌표 통해서 H값 계산
    g_jps.GetStartNode()->CalculateHeuristic(destX, destY);

    // JPS 오픈 리스트에 노드 집어넣기
    g_jps.PushOpenList(g_jps.GetStartNode());

    // 목적지 노드 색 그리기
    InvalidateRect(hWnd, NULL, false);

}

void OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    // WHLLE 방향 확인
    short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

    // wParam인자로 실제 모니터 좌표를 전달받기 때문에 스크린 좌표를 클라이언트 좌표로 변환
    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);
    ScreenToClient(hWnd, &pt);

    // 줌 전의 마우스가 가리키는 실제 맵 좌표 계산
    float worldX = (pt.x - camX) / scale;
    float worldY = (pt.y - camY) / scale;

    // 배율 업데이트
    if (zDelta > 0) {
        scale *= 1.1f;
        if (scale > 5.0f) scale = 5.0f; // 최대 확대 배율
    }
    else {
        scale /= 1.1f;
        if (scale < 0.2f) scale = 0.2f; // 최대 축소 배율
    }

    // 새로운 배율에 맞춰 마우스 위치가 변하지 않도록 카메라 오프셋 보정
    camX = pt.x - (worldX * scale);
    camY = pt.y - (worldY * scale);

    // 화면 갱신
    InvalidateRect(hWnd, NULL, false);
}

void OnCreate(HWND hWnd)
{
    g_hGridPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
    g_hTileBrush = CreateSolidBrush(RGB(100, 100, 100));

    g_hBrushStart = CreateSolidBrush(RGB(0, 255, 0));   // 녹색 ( 출발 )
    g_hBrushDest = CreateSolidBrush(RGB(255, 0, 0));   // 적색 ( 목적 )
    g_hBrushOpen = CreateSolidBrush(RGB(0, 0, 255));   // 청색 ( 오픈 )
    g_hBrushClosed = CreateSolidBrush(RGB(135, 206, 235)); // 하늘색 ( 클로즈 )
    g_hBrushPath = CreateSolidBrush(RGB(255, 255, 0)); // 노란색 ( 최종 경로 )

    //윈도우 생성 시 현 윈도우 크기와 동일한 메모리 DC 생성
    HDC hdc = GetDC(hWnd);
    GetClientRect(hWnd, &g_MemDCRect);
    g_hMemDCBitmap = CreateCompatibleBitmap(hdc, g_MemDCRect.right, g_MemDCRect.bottom);
    g_hMemDC = CreateCompatibleDC(hdc);
    ReleaseDC(hWnd, hdc);
    g_hMemDcBitmap_old = (HBITMAP)SelectObject(g_hMemDC, g_hMemDCBitmap);
}

void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    // 메모리 DC를 클리어 하고
    PatBlt(g_hMemDC, 0, 0, g_MemDCRect.right, g_MemDCRect.bottom, WHITENESS);

    // RenderObstacle, RenderGrid 를 메모리 DC에 출력
    RenderObstacle(g_hMemDC);
    RenderGrid(g_hMemDC);

    // --- 좌측 고정 UI 렌더링 (카메라 영향 받지 않음) ---
    SetTextColor(g_hMemDC, RGB(0, 0, 0));
    SetBkMode(g_hMemDC, TRANSPARENT); // 글자 배경 투명하게

    TextOut(g_hMemDC, 20, 20, L"======== CONTROLS ========", 26);
    TextOut(g_hMemDC, 20, 50, L"• 좌클릭 드래그 : 장애물 토글", 18);
    TextOut(g_hMemDC, 20, 80, L"• 우클릭 드래그 : 화면 이동 (패닝)", 22);
    TextOut(g_hMemDC, 20, 110, L"• 마우스 휠     : 확대 / 축소", 21);
    TextOut(g_hMemDC, 20, 140, L"• [C] 키        : 초기화", 20);
    TextOut(g_hMemDC, 20, 170, L"• 좌측 더블클릭   : 출발지 설정", 20);
    TextOut(g_hMemDC, 20, 200, L"• 우측 더블클릭   : 목적지 설정", 20);
    TextOut(g_hMemDC, 20, 230, L"• [Space] 키    : A* 길찾기 시작", 26);
    TextOut(g_hMemDC, 20, 260, L"==========================", 26);

    // 메모리 DC에 랜더링이 끝나면, 메모리 DC -> 윈도우 DC로의 출력
    HDC hdc = BeginPaint(hWnd, &ps);
    BitBlt(hdc, 0, 0, g_MemDCRect.right, g_MemDCRect.bottom, g_hMemDC, 0, 0, SRCCOPY);
    EndPaint(hWnd, &ps);
}

void OnSize(HWND hWnd)
{
    if (g_hMemDC == NULL) return; // 초기화 전 호출 방지

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

LRESULT OnSysCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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
}

void OnDestroy()
{
    SelectObject(g_hMemDC, g_hMemDcBitmap_old);
    DeleteObject(g_hTileBrush);
    DeleteObject(g_hGridPen);
    PostQuitMessage(0); // WM_QUIT 메시지를 발생시키는 함수로서 메시지 루프의 종료를 유도
}

