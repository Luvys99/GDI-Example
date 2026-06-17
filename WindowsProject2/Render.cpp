#include "Render.h"
#include "ASTAR.h"
#include "MsgProcess.h"

void RenderGrid(HDC hdc)
{
	// 펜 빌리기
	HPEN hOldPen = (HPEN)SelectObject(hdc, g_hGridPen);

	// 확대된 그리드 간격 계산
	float currentDrawSize = GRID_SIZE * scale;

	// 세로선 그리기 ( x값이 0부터 시작해서 카메라 배율이 적용된 그리드 크기만큼씩 세로만 그림
	// 그리드의 마지막 선을 추가로 그리기 위해 <= 의 반복 조건
	for (int iCntW = 0; iCntW <= GRID_WIDTH; iCntW++)
	{
		int x = (int)(camX + (iCntW * currentDrawSize));
		int yStart = (int)camY;
		int yEnd = (int)(camY + (GRID_HEIGHT * currentDrawSize));

		// 선 긋기 
		MoveToEx(hdc, x, yStart, NULL);
		LineTo(hdc, x, yEnd);
		
	}
	for (int iCntH = 0; iCntH <= GRID_HEIGHT; iCntH++)
	{
		int xStart = (int)camX;
		int xEnd = (int)(camX + (GRID_WIDTH * currentDrawSize));
		int y = (int)(camY + (iCntH * currentDrawSize));

		MoveToEx(hdc, xStart, y, NULL);
		LineTo(hdc, xEnd, y);
		
	}
	// 펜 반납하기
	SelectObject(hdc, hOldPen);
}

void RenderObstacle(HDC hdc)
{
	// 확대한 그리드 간격 계산
	float currentDrawSize = GRID_SIZE * scale;

	for (int iCntW = 0; iCntW < GRID_WIDTH; iCntW++)
	{
		for (int iCntH = 0; iCntH < GRID_HEIGHT; iCntH++)
		{
			
			RECT rcTile;
			rcTile.left = (int)(camX + (iCntW * currentDrawSize));
			rcTile.top = (int)(camY + (iCntH * currentDrawSize));
			rcTile.right = (int)(rcTile.left + currentDrawSize) + 1;
			rcTile.bottom = (int)(rcTile.top + currentDrawSize) + 1;

			HBRUSH hTargetBrush = nullptr;
			Node* pNode = g_astar.GetNodeFromMap(iCntW, iCntH);

			// 출발지
			if (iCntW == g_astar.GetStartX() && iCntH == g_astar.GetStartY())
			{
				hTargetBrush = g_hBrushStart;
			}
			// 목적지
			else if (iCntW == g_astar.GetDestX() && iCntH == g_astar.GetDestY())
			{
				hTargetBrush = g_hBrushDest;
			}
			else if (pNode != nullptr && pNode->isPath)
			{
				hTargetBrush = g_hBrushPath; // 최종 경로
			}
			// 벽
			else if (g_Tile[iCntH][iCntW] == 1)
			{
				hTargetBrush = g_hTileBrush;
			}
			// 오픈 리스트, 클로즈 리스트 색칠
			else if (pNode != nullptr)
			{
				if (pNode->isOpen) hTargetBrush = g_hBrushOpen;
				else if (pNode->isClose) hTargetBrush = g_hBrushClosed;
			}

			if (hTargetBrush != nullptr)
			{
				// 지정된 브러시로 사각형 내부 채우기 (경계선 없음)
				FillRect(hdc, &rcTile, hTargetBrush);
			}

		}
	}

	// 색칠이 끝난 후에 부모를 향하는 선 그리기
	// 바탕이 다 칠해진 후, 그 위에 부모를 향하는 선(Line)을 덧그립니다.
	HPEN hDirPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); // 부모 방향 지시선 (회색)
	HPEN hPathPen = CreatePen(PS_SOLID, 4, RGB(255, 0, 0));    // 최종 경로 굵은 선 (빨간색)
	HPEN hOldPen = (HPEN)SelectObject(hdc, hDirPen);

	for (int x = 0; x < GRID_WIDTH; x++)
	{
		for (int y = 0; y < GRID_HEIGHT; y++)
		{
			Node* pNode = g_astar.GetNodeFromMap(x, y);

			// 노드가 존재하고, 부모 포인터가 연결되어 있다면
			if (pNode != nullptr && pNode->parentNode != nullptr)
			{
				int centerX = (int)(camX + (x * currentDrawSize) + (currentDrawSize / 2.0f));
				int centerY = (int)(camY + (y * currentDrawSize) + (currentDrawSize / 2.0f));

				int parentCenterX = (int)(camX + (pNode->parentNode->xPos * currentDrawSize) + (currentDrawSize / 2.0f));
				int parentCenterY = (int)(camY + (pNode->parentNode->yPos * currentDrawSize) + (currentDrawSize / 2.0f));

				// 분기 처리: 최종 경로(빨간색)는 부모 타일 중앙까지 완벽하게 이어지도록 그림
				if (pNode->isPath && pNode->parentNode->isPath)
				{
					SelectObject(hdc, hPathPen);
					MoveToEx(hdc, centerX, centerY, NULL);
					LineTo(hdc, parentCenterX, parentCenterY); // 부모 중앙까지 Full Line
				}
				// 일반 탐색 방향 지시선(회색)은 타일 경계선(절반)까지만 그림
				else
				{
					SelectObject(hdc, hDirPen);

					float dirX = (float)(pNode->parentNode->xPos - x);
					float dirY = (float)(pNode->parentNode->yPos - y);

					int lineEndX = (int)(centerX + dirX * (currentDrawSize / 2.0f));
					int lineEndY = (int)(centerY + dirY * (currentDrawSize / 2.0f));

					MoveToEx(hdc, centerX, centerY, NULL);
					LineTo(hdc, lineEndX, lineEndY); // 타일 가장자리까지만 Half Line
				}
			}
		}
	}

	// 자원 해제
	SelectObject(hdc, hOldPen);
	DeleteObject(hDirPen);
	DeleteObject(hPathPen);

	// 노드에 G, H, F 값 그리기
	// 글자 배경을 투명하게 설정 (타일 색을 가리지 않기 위함)
	const float TEXT_VISIBLE_THRESHOLD = 40.0f;
	if (currentDrawSize >= TEXT_VISIBLE_THRESHOLD)
	{
		int oldBkMode = SetBkMode(hdc, TRANSPARENT);
		COLORREF oldTextColor = SetTextColor(hdc, RGB(0, 0, 0)); // 글자색을 검정색으로 설정

		for (int x = 0; x < GRID_WIDTH; x++)
		{
			for (int y = 0; y < GRID_HEIGHT; y++)
			{
				Node* pNode = g_astar.GetNodeFromMap(x, y);

				// 값이 계산된 노드(오픈/클로즈 리스트 및 경로)들만 출력
				if (pNode != nullptr && (pNode->isOpen || pNode->isClose || pNode->isPath))
				{
					RECT rcText;
					rcText.left = (int)(camX + (x * currentDrawSize));
					rcText.top = (int)(camY + (y * currentDrawSize));
					rcText.right = (int)(rcText.left + currentDrawSize);
					rcText.bottom = (int)(rcText.top + currentDrawSize);

					// 유니코드 전용 자료형(wchar_t)과 버퍼 크기 지정
					wchar_t szText[64];

					// swprintf_s를 사용하여 유니코드(L) 문자열 포맷팅
					swprintf_s(szText, _countof(szText), L"G:%d\nH:%d\nF:%d", pNode->g, pNode->h, pNode->f);

					// 사각형 영역(rcText)의 중앙에 맞춰서 텍스트 출력 (유니코드 전용 함수 DrawTextW 사용)
					DrawTextW(hdc, szText, -1, &rcText, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
				}
			}
		}

		// GDI 설정 원상 복구
		SetBkMode(hdc, oldBkMode);
		SetTextColor(hdc, oldTextColor);
	}

}