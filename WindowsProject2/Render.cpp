#include "Render.h"

void RenderGrid(HDC hdc)
{
	int iX = 0;
	int iY = 0;

	HPEN hOldPen = (HPEN)SelectObject(hdc, g_hGridPen);

	// 그리드의 마지막 선을 추가로 그리기 위해 <= 의 반복 조건
	for (int iCntW = 0; iCntW <= GRID_WIDTH; iCntW++)
	{
		MoveToEx(hdc, iX, 0, NULL);
		LineTo(hdc, iX, GRID_HEIGHT * GRID_SIZE);
		iX += GRID_SIZE;
	}
	for (int iCntH = 0; iCntH <= GRID_HEIGHT; iCntH++)
	{
		MoveToEx(hdc, 0, iY, NULL);
		LineTo(hdc, GRID_WIDTH * GRID_SIZE, iY);
		iY += GRID_SIZE;
	}
	SelectObject(hdc, hOldPen);
}

void RenderObstacle(HDC hdc)
{
	int iX = 0;
	int iY = 0;
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, g_hTileBrush);
	SelectObject(hdc, GetStockObject(NULL_PEN));

	// 사각형의 테두리를 안 보이도록하기 위해 NULL Pen을 지정한다
	// CreatePen 으로 NULL PEN을 생성해도 되지만, GetStockObject를 사용하여 이미 시스템에 만들어져 있는 고정 GDI Object를 사용해본다.
	// GetStockObject 는 시스템의 고정적인 범용 GDI Object로서 삭제가 필요 없다. ( 생성된걸 가져가 사용하기 때문에 )
	// 시스템 전역적인 GDI Object를 얻어서 사용한다는 개념
	for (int iCntW = 0; iCntW < GRID_WIDTH; iCntW++)
	{
		for (int iCntH = 0; iCntH < GRID_HEIGHT; iCntH++)
		{
			if (g_Tile[iCntH][iCntW])
			{
				iX = iCntW * GRID_SIZE;
				iY = iCntH * GRID_SIZE;

				// 테두리 크기가 있으므로 + 2 한다.
				Rectangle(hdc, iX, iY, iX + GRID_SIZE + 2, iY + GRID_SIZE + 2);
			}
		}
	}
	SelectObject(hdc, hOldBrush);
}