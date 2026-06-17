#pragma once
#include <wtypes.h>

#define GRID_SIZE 16
#define GRID_WIDTH 150
#define GRID_HEIGHT 100

inline HBRUSH g_hTileBrush;
inline HPEN g_hGridPen;
inline char g_Tile[GRID_HEIGHT][GRID_WIDTH];	// 0 장애물 없음, 1 장애물 있음

inline bool g_bErase = false;
inline bool g_bDrag = false;

inline float scale = 1.0f;
inline float camX = 0.0f;
inline float camY = 0.0f;

inline bool g_bPanDrag = false;
inline int g_LastMouseX = 0;
inline int g_LastMouseY= 0;

// 더블 버퍼링에 필요한 전역 변수:
inline HBITMAP g_hMemDCBitmap;
inline HBITMAP g_hMemDcBitmap_old;
inline HDC g_hMemDC;
inline RECT g_MemDCRect;

inline HBRUSH g_hBrushStart; // 출발지 색 ( 녹색 )
inline HBRUSH g_hBrushDest; // 목적지 색 ( 적색 )
inline HBRUSH g_hBrushOpen; // 오픈리스트 색 ( 파란색 )
inline HBRUSH g_hBrushClosed; // 클로즈 리스트 색 ( 노란색 )
inline HBRUSH g_hBrushPath;

// 그리드 타일 그리는 함수
void RenderGrid(HDC hdc);

// 장애물 그리는 함수
void RenderObstacle(HDC hdc);