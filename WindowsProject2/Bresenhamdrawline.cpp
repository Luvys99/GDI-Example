#include <iostream>
#include "Bresenhamdrawline.h"


std::vector<POINT> Bresenhamdrawline::drawline(int startX, int startY, int endX, int endY)
{

	std::vector<POINT> point;

	// 직선의 방정식
	int dx = endX - startX;
	int dy = endY - startY;

	// dx가 양수고 dy가 양수면 4사분면쪽으로 솓은 직선
	// dx가 음수고 dy가 양수면 3사분면
	// dx가 음수고 dy가 음수면 2사분면
	// dx가 양수고 dy가 음수면 1사분면
	int vx = (dx > 0) ? 1 : -1;
	if (dx == 0) vx = 0; // 변화가 없으면 y축만 변화되는 것
	int vy = (dy > 0) ? 1 : -1;
	if (dy == 0) vy = 0; // x축만 변화되는 것

	dx = abs(dx);
	dy = abs(dy);

	POINT pointStart;
	pointStart.x = startX;
	pointStart.y = startY;

	POINT currentpoints;
	int error = 0;

	if (dx > dy)
	{

		for (int i = 1; i <= dx; i++)
		{
			error += 2 * dy;
			if (error > dx)
			{
				error -= 2 * dx;
				pointStart.y += vy;
			}
			pointStart.x += vx;

			currentpoints.x = pointStart.x;
			currentpoints.y = pointStart.y;
			point.push_back(currentpoints);
		}

	}
	else
	{

		for (int j = 1; j <= dy; j++)
		{
			error += 2 * dx;
			if (error > dy)
			{
				error -= 2 * dy;
				pointStart.x += vx;
			}
			pointStart.y += vy;

			currentpoints.x = pointStart.x;
			currentpoints.y = pointStart.y;
			point.push_back(currentpoints);
		}

	}


	return point;

}
