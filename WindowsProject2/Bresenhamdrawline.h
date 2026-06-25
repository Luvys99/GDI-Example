#pragma once
#include <utility>
#include <vector>
#include <windows.h>

class Bresenhamdrawline
{
public:
	// 정적 함수로 선언해서 인스턴스 없이 호출
	static std::vector<POINT> drawline(int startX, int startY, int endX, int endY);

private:
	// 객체가 없이 사용할 것이기 때문에 멤버 변수가 존재하면 안됨
};