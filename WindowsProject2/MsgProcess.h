#pragma once
#include "framework.h"
#include "ASTAR.h"

#define MAX_LOADSTRING 100
#define TIMER_ASTAR_STEP 1

inline HINSTANCE hInst;                                // 현재 인스턴스입니다.
inline WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
inline WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
inline ASTAR g_astar;
inline bool g_isrunning = false;

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// 메시지 처리기
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void OnLButtonDown(HWND hWnd, LPARAM lParam); // 좌클릭 누를 때 메시지
void OnLButtonUp(); // 좌클릭 땔 때 메세지
void OnRButtonDown(HWND hWnd, LPARAM lParam); // 우클릭 누를 때 메시지
void OnRButtonUp(); // 우클릭 땔 때 메시지
void OnMouseMove(HWND hWnd, LPARAM lParam); // 마우스 이동 메시지
void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam); // 키다운 메시지
void OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnTimer(HWND hWnd, WPARAM wParam);
void OnLButtonBLCLK(HWND hWnd, WPARAM wParam, LPARAM lParam); // 더블 좌클릭 메시지
void OnRButtonBLCLK(HWND hWnd, WPARAM wParam, LPARAM lParam); // 더블 좌클릭 메시지
void OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam); // 마우스 휠 메시지
void OnCreate(HWND hWnd);  // 초기화 메시지
void OnPaint(HWND hWnd); // 랜더링
void OnSize(HWND hWnd); // 윈도우 창 사이즈 조절 메시지
LRESULT OnSysCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); // 윈도우 창 제어 메시지
void OnDestroy(); // 윈도우 창 꺼졌을 떄 정리하는 메시지