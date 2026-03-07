#pragma once
#include <wtypes.h>

#define GRID_SIZE 16
#define GRID_WIDTH 100
#define GRID_HEIGHT 50

extern HBRUSH g_hTileBrush;
extern HPEN g_hGridPen;
extern char g_Tile[GRID_HEIGHT][GRID_WIDTH];	// 0 장애물 없음, 1 장애물 있음

extern bool g_bErase;
extern bool g_bDrag;

void RenderGrid(HDC hdc);

void RenderObstacle(HDC hdc);