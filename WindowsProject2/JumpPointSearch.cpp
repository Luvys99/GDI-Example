#include "JumpPointSearch.h"
#include <algorithm>
#include <cmath>

bool JPS::FindPathStep()
{

	// 오픈 리스트에 노드가 없다면 탐색 종료
	if (OpenList.empty())
	{
		return false;
	}

	// 오픈 리스트에서 노드 꺼냄
	JPSNode* currentNode = OpenList.top();
	OpenList.pop();

	// 클로즈 리스트 플래그 표시
	currentNode->isOpen = false;
	currentNode->isClose = true;

	// 길찾기 횟수마다 1씩 증가 ( 길찾기 스텝마다 다른 색으로 칠하기 위함 )
	m_currentStepCount++;

	// 목적지에 도달했다면 최단 거리 부모 포인터로 따라가서 ispath 플래그 표시
	if (currentNode->xPos == m_destX && currentNode->yPos == m_destY)
	{
		JPSNode* traceNode = currentNode;
		while (traceNode != nullptr)
		{
			traceNode->isPath = true;
			traceNode = traceNode->parentNode;
		}
		return true;
	}

	// 탐색으로 반환되는 좌표 저장할 변수
	std::pair<int, int> jumppos = { -1, -1 };

	// 진행 방향 저장 변수
	int dx = 0;
	int dy = 0;

	if (currentNode->parentNode != nullptr)
	{
		// 현재 노드와 부모노드의 좌표를 뺌으로써 방향이 생김
		int diffX = currentNode->xPos - currentNode->parentNode->xPos;
		int diffY = currentNode->yPos - currentNode->parentNode->yPos;

		// 방향을 -1, 0, 1로 값을 맞춰줌
		dx = std::clamp(diffX, -1, 1);
		dy = std::clamp(diffY, -1, 1);
	}

	int curX = currentNode->xPos;
	int curY = currentNode->yPos;

	// 출발지 노드 ( 부모노드가 없으면 )
	if (dx == 0 && dy == 0)
	{
		jumppos = JumpToDD(curX, curY, currentNode);
		if (jumppos.first != -1) CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);

		jumppos = JumpToLL(curX, curY, currentNode);
		if (jumppos.first != -1) CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);

		jumppos = JumpToUU(curX, curY, currentNode);
		if (jumppos.first != -1) CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);

		jumppos = JumpToRR(curX, curY, currentNode);
		if (jumppos.first != -1) CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);

		jumppos = JumpToRU(curX, curY, currentNode);
		if (jumppos.first != -1) CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);

		jumppos = JumpToRD(curX, curY, currentNode);
		if (jumppos.first != -1) CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);

		jumppos = JumpToLD(curX, curY, currentNode);
		if (jumppos.first != -1) CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);

		jumppos = JumpToLU(curX, curY, currentNode);
		if (jumppos.first != -1) CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);

	}
	// --------------------------------------------
	// 수평, 수직 이동
	// --------------------------------------------
	// 오른쪽 RR 방향
	else if (dx == 1 && dy == 0)
	{
		// RR 방향 탐색
		jumppos = JumpToRR(curX, curY, currentNode);
		if (jumppos.first != -1) CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);

		// RU 방향 코너 탐색
		if (curY - 1 >= 0 && g_Tile[curY - 1][curX] == 1 && curX + 1 < GRID_WIDTH && g_Tile[curY - 1][curX + 1] != 1)
		{
			jumppos = JumpToRU(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}

		// RD 방향 코너 탐색
		if (curY + 1 < GRID_HEIGHT && g_Tile[curY + 1][curX] == 1 && curX + 1 < GRID_WIDTH && g_Tile[curY + 1][curX + 1] != 1)
		{
			jumppos = JumpToRD(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}

	}
	// 윈쪽 LL 방향
	else if (dx == -1 && dy == 0)
	{
		// LL 방향 탐색
		jumppos = JumpToLL(curX, curY, currentNode);
		if (jumppos.first != -1) CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);

		// LU 방향 코너 탐색
		if (curY - 1 >= 0 && g_Tile[curY - 1][curX] == 1 && curX - 1 >= 0 && g_Tile[curY - 1][curX - 1] != 1)
		{
			jumppos = JumpToLU(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}

		// LD 방향 코너 탐색
		if (curY + 1 < GRID_HEIGHT && g_Tile[curY + 1][curX] == 1 && curX - 1 >= 0 && g_Tile[curY + 1][curX - 1] != 1)
		{
			jumppos = JumpToLD(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}

	}
	// 위쪽 UU 방향
	else if (dx == 0 && dy == -1)
	{
		// UU 방향 탐색
		jumppos = JumpToUU(curX, curY, currentNode);
		if (jumppos.first != -1) CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);

		// LU 방향 코너 탐색
		if ( curX - 1 >= 0 && g_Tile[curY][curX - 1] == 1  &&  curY - 1 >= 0 && g_Tile[curY - 1][curX - 1] != 1 ) 
		{
			jumppos = JumpToLU(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}

		// RU 방향 코너 탐색
		if (curX + 1 < GRID_WIDTH && g_Tile[curY][curX + 1] == 1 && curY - 1 >= 0 && g_Tile[curY - 1][curX + 1] != 1) 
		{
			jumppos = JumpToRU(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}
	}
	// 아래쪽 DD 방향
	else if (dx == 0 && dy == 1)
	{
		// DD 방향 탐색
		jumppos = JumpToDD(curX, curY, currentNode);
		if (jumppos.first != -1) CreateNodeorUpateNode(jumppos.first, jumppos .second, currentNode);

		// LD 코너 탐색
		if (curX - 1 >= 0 && g_Tile[curY][curX - 1] == 1 && curY + 1 < GRID_HEIGHT && g_Tile[curY + 1][curX - 1] != 1)
		{
			jumppos = JumpToLD(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}
		// RD 코너 탐색
		if (curX + 1 < GRID_WIDTH && g_Tile[curY][curX + 1] == 1 && curY + 1 < GRID_HEIGHT && g_Tile[curY + 1][curX + 1] != 1)
		{
			jumppos = JumpToRD(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}
	}
	// --------------------------------------------
	// 대각선 이동
	// --------------------------------------------
	// 우상단 RU 방향
	else if (dx == 1 && dy == -1)
	{
		// RU 방향 탐색
		jumppos = JumpToRU(curX, curY, currentNode); 
		if (jumppos.first != -1)
		{
			CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
		}
		// RR 방향 탐색
		jumppos = JumpToRR(curX, curY, currentNode); 
		if (jumppos.first != -1)
		{
			CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
		}
		// UU 방향 탐색
		jumppos = JumpToUU(curX, curY, currentNode); 
		if (jumppos.first != -1)
		{
			CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
		}

		// LU 좌상단 코너 체크
		if (curX - 1 >= 0 && g_Tile[curY][curX - 1] == 1 && curY - 1 >= 0 && g_Tile[curY - 1][curX - 1] != 1) {
			jumppos = JumpToLU(curX, curY, currentNode); 
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}
		// RD 우하단 코너 체크
		if (curY + 1 < GRID_HEIGHT && g_Tile[curY + 1][curX] == 1 && curX + 1 < GRID_WIDTH && g_Tile[curY + 1][curX + 1] != 1) {
			jumppos = JumpToRD(curX, curY, currentNode); 
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}
	}
	// 좌상단 LU 방향
	else if (dx == -1 && dy == -1)
	{
		// LU 방향 탐색
		jumppos = JumpToLU(curX, curY, currentNode);
		if (jumppos.first != -1)
		{
			CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
		}
		// LL 방향 탐색
		jumppos = JumpToLL(curX, curY, currentNode);
		if (jumppos.first != -1)
		{
			CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
		}
		// UU 방향 탐색
		jumppos = JumpToUU(curX, curY, currentNode);
		if (jumppos.first != -1)
		{
			CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
		}

		// RU 우상단 코너 체크 
		if (curX + 1 < GRID_WIDTH && g_Tile[curY][curX + 1] == 1 && curY - 1 >= 0 && g_Tile[curY - 1][curX + 1] != 1) {
			jumppos = JumpToRU(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}
		// LD 좌하단 코너 체크 
		if (curY + 1 < GRID_HEIGHT && g_Tile[curY + 1][curX] == 1 && curX - 1 >= 0 && g_Tile[curY + 1][curX - 1] != 1) {
			jumppos = JumpToLD(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}
	}
	// 우하단 RD 방향
	else if (dx == 1 && dy == 1)
	{
		// RD 방향 탐색
		jumppos = JumpToRD(curX, curY, currentNode);
		if (jumppos.first != -1)
		{
			CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
		}
		// RR 방향 탐색
		jumppos = JumpToRR(curX, curY, currentNode);
		if (jumppos.first != -1)
		{
			CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
		}
		// DD 방향 탐색
		jumppos = JumpToDD(curX, curY, currentNode);
		if (jumppos.first != -1)
		{
			CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
		}

		// LD 좌하단 코너 체크 
		if (curX - 1 >= 0 && g_Tile[curY][curX - 1] == 1 && curY + 1 < GRID_HEIGHT && g_Tile[curY + 1][curX - 1] != 1) {
			jumppos = JumpToLD(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}
		// RU 우상단 코너 체크 
		if (curY - 1 >= 0 && g_Tile[curY - 1][curX] == 1 && curX + 1 < GRID_WIDTH && g_Tile[curY - 1][curX + 1] != 1) {
			jumppos = JumpToRU(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}
	}
	// 좌하단 LD 방향
	else if (dx == -1 && dy == 1)
	{
		// LD 방향 탐색
		jumppos = JumpToLD(curX, curY, currentNode);
		if (jumppos.first != -1)
		{
			CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
		}
		// LL 방향 탐색
		jumppos = JumpToLL(curX, curY, currentNode);
		if (jumppos.first != -1)
		{
			CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
		}
		// DD 방향 탐색
		jumppos = JumpToDD(curX, curY, currentNode);
		if (jumppos.first != -1)
		{
			CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
		}

		// RD 우하단 코너 체크
		if (curX + 1 < GRID_WIDTH && g_Tile[curY][curX + 1] == 1 && curY + 1 < GRID_HEIGHT && g_Tile[curY + 1][curX + 1] != 1) {
			jumppos = JumpToRD(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}
		// LU 좌상단 코너 체크
		if (curY - 1 >= 0 && g_Tile[curY - 1][curX] == 1 && curX - 1 >= 0 && g_Tile[curY - 1][curX - 1] != 1) {
			jumppos = JumpToLU(curX, curY, currentNode);
			if (jumppos.first != -1)
			{
				CreateNodeorUpateNode(jumppos.first, jumppos.second, currentNode);
			}
		}
	}

	// 8방향 탐색 시도 후 목적지를 발견하지 못했으면 false
	return false;
}

void JPS::CreateStartNode(int startX, int startY)
{
	// 시작 노드 생성
	startNode = new JPSNode();
	startNode->xPos = startX;
	startNode->yPos = startY;

	m_nodeMap[startY][startX] = startNode;

	m_startX = startX;
	m_startY = startY;
}

// 좌표와 좌표 사이의 G 값 계산
int JPS::CalculateJumpDistance(int startX, int startY, int endX, int endY) noexcept
{

	int dx = std::abs(endX - startX);
	int dy = std::abs(endY - startY);

	// 대각선은 x, y축 1칸씩 전진하는 것으로 x, y축 좌표의 차이 중에 작은 차이만큼 대각선 전진이 가능
	int diagonalsteps = min(dx, dy);
	int straightsteps = std::abs(dx - dy); 

	return (15 * diagonalsteps) + (10 * straightsteps);
}

void JPS::CreateNodeorUpateNode(int targetxpos, int targetypos, JPSNode* parentNode)
{
	
	int xpos = targetxpos;
	int ypos = targetypos;

	// 유효하지 않은 좌표면 무시하고 종료
	if (xpos == -1 || ypos == -1) return;

	// G값 계산
	int jumpDist = CalculateJumpDistance(parentNode->xPos, parentNode->yPos, xpos, ypos);
	int newG = parentNode->g + jumpDist;

	// 해당 좌표에 이미 노드가 있는지 확인
	JPSNode* targetNode = GetNodeFromMap(xpos, ypos);

	if (targetNode == nullptr)
	{
		// [새로 발견한 길] 노드 생성
		targetNode = new JPSNode();
		targetNode->xPos = xpos;
		targetNode->yPos = ypos;
		targetNode->parentNode = parentNode;
		targetNode->g = newG;
		targetNode->CalculateHeuristic(m_destX, m_destY); // 내부에서 f = g + h 계산

		targetNode->isOpen = true;
		m_nodeMap[ypos][xpos] = targetNode;
		OpenList.push(targetNode); // 우선순위 큐에 삽입
	}
	else if (targetNode->isOpen)
	{
		// [이미 오픈 리스트에 있는 길] G값 비교 후 더 빠른 길이면 갱신
		if (newG < targetNode->g)
		{
			targetNode->parentNode = parentNode;
			targetNode->g = newG;
			targetNode->CalculateHeuristic(m_destX, m_destY);

			// 기존 노드를 수정했으므로, 갱신된(더 작아진) F값으로 큐에 한 번 더 밀어 넣음 (Lazy Deletion)
			OpenList.push(targetNode);
		}
	}


}

JPSNode* JPS::GetNodeFromMap(int xPos, int yPos) const noexcept
{
	if (xPos < 0 || xPos >= GRID_WIDTH || yPos < 0 || yPos >= GRID_HEIGHT) return nullptr;

	// 2D 배열에서 해당 좌표의 노드 포인터를 즉시 반환
	return m_nodeMap[yPos][xPos];
}

void JPS::SetDestPos(int destX, int destY) noexcept
{
	m_destX = destX;
	m_destY = destY;
}

void JPS::InitAStarPos() noexcept
{

	m_startX = -1;
	m_startY = -1;
	m_destX = -1;
	m_destY = -1;

}

void JPS::ClearPathData() noexcept
{

	// 2D 노드 맵 순회해서 메모리 해제
	for (int y = 0; y < GRID_HEIGHT; y++)
	{
		for (int x = 0; x < GRID_WIDTH; x++)
		{
			// startNode에 해당하는 맵은 제외하고 전부 삭제
			if (m_nodeMap[y][x] != nullptr)
			{
				delete m_nodeMap[y][x];
				m_nodeMap[y][x] = nullptr;
			}
		}
	}

	// 오픈 리스트도 비워주기 
	while (!OpenList.empty())
	{
		OpenList.pop();
	}

}

// 8방향으로 탐색하는 함수들
// 수직, 수평

std::pair<int, int> JPS::JumpToLL(int curx, int cury, JPSNode* parentNode)
{
	int nextX = curx;
	int nextY = cury;

	while (1)
	{
		// 다음 왼쪽 방향 타일 좌표
		nextX--;

		// 그리드를 벗어나거나 벽 만나면 종료
		if (nextX < 0 || g_Tile[nextY][nextX] == 1) return { -1, -1 };

		// 처음 방문하는 타일이면 현제 길찾기 스텝 번호 저장
		if (m_visitedOrder[nextY][nextX] == 0)
		{
			m_visitedOrder[nextY][nextX] = m_currentStepCount;
		}

		// 목적지 만나면 리턴
		if (nextX == m_destX && nextY == m_destY) return { nextX, nextY };

		// 코너체크 4군데
		bool isTopWall = (nextY - 1 >= 0 ) && (g_Tile[nextY - 1][nextX] == 1);
		bool isTopLeftWallEmpty = (nextY - 1 >= 0) && (nextX - 1 >= 0) && (g_Tile[nextY - 1][nextX - 1] != 1);
		bool isBottomWall = (nextY + 1 < GRID_HEIGHT) && (g_Tile[nextY + 1][nextX] == 1);
		bool isBottomLeftWallEmpty = (nextY + 1 < GRID_HEIGHT) && (nextX - 1 >= 0) &&(g_Tile[nextY + 1][nextX - 1] != 1);

		// Top, BottomWall이 존재하고 TopRightWall, BottomRightWall이 존재하지 않으면 노드 생성
		// TopWall이 있고 TopWallRight가 비어있다면 코너를 찾은 것 또는 BottomWall이 있고 BottomWallRight가 비어있다면 코너를 찾은 것
		if ((isTopWall && isTopLeftWallEmpty) || (isBottomWall && isBottomLeftWallEmpty)) return { nextX, nextY };

	}
}

std::pair<int, int> JPS::JumpToDD(int curx, int cury, JPSNode* parentNode)
{
	int nextX = curx;
	int nextY = cury;

	while ( 1 )
	{
		// 다음 왼쪽 방향 타일 좌표
		nextY++;

		// 그리드를 벗어나거나 벽 만나면 종료
		if (nextY >= GRID_HEIGHT || g_Tile[nextY][nextX] == 1) return { -1, -1 };

		// 처음 방문하는 타일이면 현제 길찾기 스텝 번호 저장
		if (m_visitedOrder[nextY][nextX] == 0)
		{
			m_visitedOrder[nextY][nextX] = m_currentStepCount;
		}

		// 목적지 만나면 리턴
		if (nextX == m_destX && nextY == m_destY) return { nextX, nextY };

		// 코너체크 4군데
		bool isRightWall = (nextX + 1 < GRID_WIDTH) && (g_Tile[nextY][nextX + 1] == 1);
		bool isRightDownWallEmpty = (nextX + 1 < GRID_WIDTH) && (nextY + 1 < GRID_HEIGHT) && (g_Tile[nextY + 1][nextX + 1] != 1);
		bool isLeftWall = (nextX - 1 >= 0) && (g_Tile[nextY][nextX - 1] == 1);
		bool isLeftBottomWallEmpty = (nextY + 1 < GRID_HEIGHT) && (nextX - 1 >= 0) && (g_Tile[nextY + 1][nextX - 1] != 1);

		// Top, BottomWall이 존재하고 TopRightWall, BottomRightWall이 존재하지 않으면 노드 생성
		// TopWall이 있고 TopWallRight가 비어있다면 코너를 찾은 것 또는 BottomWall이 있고 BottomWallRight가 비어있다면 코너를 찾은 것
		if ((isRightWall && isRightDownWallEmpty) || (isLeftWall && isLeftBottomWallEmpty)) return { nextX, nextY };

	}
}

std::pair<int, int> JPS::JumpToRR(int curx, int cury, JPSNode* parentNode)
{
	int nextX = curx;
	int nextY = cury;

	// 그리드 범위를 벗어나지 않도록
	while ( 1 )
	{
		// 다음 오른쪽 방향 타일 좌표
		nextX++;

		// 벽 만나거나 그리드 범위를 벗어나면 종료
		if (nextX >= GRID_WIDTH || g_Tile[nextY][nextX] == 1) return { -1, -1 };

		// 처음 방문하는 타일이면 현제 길찾기 스텝 번호 저장
		if (m_visitedOrder[nextY][nextX] == 0)
		{
			m_visitedOrder[nextY][nextX] = m_currentStepCount;
		}

		// 목적지 만나면 리턴
		if (nextX == m_destX && nextY == m_destY) return { nextX, nextY };
		
		// 코너체크 4군데
		bool isTopWall = (nextY - 1 >= 0) && (g_Tile[nextY - 1][nextX] == 1);
		bool isTopRightWallEmpty = (nextY - 1 >= 0) && (nextX + 1 < GRID_WIDTH) && (g_Tile[nextY - 1][nextX + 1] != 1);
		bool isBottomWall = (nextY + 1 < GRID_HEIGHT) && (g_Tile[nextY + 1][nextX] == 1);
		bool isBottomRightWallEmpty = (nextY + 1 < GRID_HEIGHT) && (nextX + 1 <GRID_WIDTH) && (g_Tile[nextY + 1][nextX + 1] != 1);

		// Top, BottomWall이 존재하고 TopRightWall, BottomRightWall이 존재하지 않으면 노드 생성
		// TopWall이 있고 TopWallRight가 비어있다면 코너를 찾은 것 또는 BottomWall이 있고 BottomWallRight가 비어있다면 코너를 찾은 것
		if ((isTopWall && isTopRightWallEmpty) || (isBottomWall && isBottomRightWallEmpty)) return { nextX, nextY };
		
	}
}

std::pair<int, int> JPS::JumpToUU(int curx, int cury, JPSNode* parentNode)
{
	int nextX = curx;
	int nextY = cury;

	// 그리드 범위를 벗어나지 않도록
	while (1)
	{
		// 다음 왼쪽 방향 타일 좌표
		nextY--;

		// 벽 만나거나 그리드 범위를 벗어나면 종료
		if (nextY < 0 || g_Tile[nextY][nextX] == 1) return { -1, -1 };

		// 처음 방문하는 타일이면 현제 길찾기 스텝 번호 저장
		if (m_visitedOrder[nextY][nextX] == 0)
		{
			m_visitedOrder[nextY][nextX] = m_currentStepCount;
		}

		// 목적지 만나면 리턴
		if (nextX == m_destX && nextY == m_destY) return { nextX, nextY };
		
		// 코너체크 2군데
		bool isRightWall = (nextX + 1 < GRID_WIDTH) && (g_Tile[nextY][nextX + 1] == 1);
		bool isRightTopWallEmpty = (nextY - 1 >= 0) && (nextX + 1 <GRID_WIDTH) && (g_Tile[nextY - 1][nextX + 1] != 1);
		bool isLeftWall = (nextX - 1 >= 0 ) && (g_Tile[nextY][nextX - 1] == 1);
		bool isLeftTopWallEmpty = (nextY - 1 >= 0) && (nextX -1 >= 0 ) && (g_Tile[nextY - 1][nextX - 1] != 1);

		// Top, BottomWall이 존재하고 TopRightWall, BottomRightWall이 존재하지 않으면 노드 생성
		// TopWall이 있고 TopWallRight가 비어있다면 코너를 찾은 것 또는 BottomWall이 있고 BottomWallRight가 비어있다면 코너를 찾은 것
		if ((isRightWall && isRightTopWallEmpty) || (isLeftWall && isLeftTopWallEmpty)) return { nextX, nextY };

	}
}

// 대각선
std::pair<int, int> JPS::JumpToRU(int curx, int cury, JPSNode* parentNode)
{

	int nextX = curx;
	int nextY = cury;

	// 그리드 범위를 벗어나지 않도록
	while ( 1 )
	{
		nextX++;
		nextY--;

		// 벽 만나거나 그리드 범위를 벗어나면 종료
		if ( nextX >= GRID_WIDTH || nextY < 0 || g_Tile[nextY][nextX] == 1) return { -1, -1 };

		// 처음 방문하는 타일이면 현제 길찾기 스텝 번호 저장
		if (m_visitedOrder[nextY][nextX] == 0)
		{
			m_visitedOrder[nextY][nextX] = m_currentStepCount;
		}

		// 목적지 만나면 리턴
		if (nextX == m_destX && nextY == m_destY) return { nextX, nextY };

		// 대각선 탐색하기 전에 RR, UU 탐색
		std::pair<int, int> UUresult = JumpToUU(nextX, nextY, parentNode);
		std::pair<int, int> RRresult = JumpToRR(nextX, nextY, parentNode);

		// 두 방향 중에 하나라도 탐색 중에 코너를 발견되었다면 현재 대각선 위치가 좌표가 점프 포인트가 되서 노드를 생성해야 함
		if (UUresult.first != -1  || RRresult.first != -1) return { nextX, nextY };

		// 코너 체크 
		bool isLeftWall = (nextX - 1 >= 0) && (g_Tile[nextY][nextX - 1] == 1);
		bool isLeftTopWallEmpty = (nextY - 1 >= 0 ) && (nextX - 1 >= 0 ) && (g_Tile[nextY - 1][nextX - 1] != 1);
		bool isBottomWall = (nextY + 1 <GRID_HEIGHT) && (g_Tile[nextY + 1][nextX] == 1);
		bool isRightBottomWallEmpty = (nextY + 1 < GRID_HEIGHT) && ( nextX + 1 < GRID_WIDTH) && (g_Tile[nextY + 1][nextX + 1] != 1);

		if ((isLeftWall && isLeftTopWallEmpty) || (isBottomWall && isRightBottomWallEmpty)) return { nextX, nextY };

	}
}

std::pair<int, int> JPS::JumpToRD(int curx, int cury, JPSNode* parentNode)
{

	int nextX = curx;
	int nextY = cury;

	while (1)
	{
		nextX++;
		nextY++;

		// 벽 만나거나 그리드 범위를 벗어나면 종료
		if ( nextX >= GRID_WIDTH || nextY >= GRID_HEIGHT || g_Tile[nextY][nextX] == 1) return { -1, -1 };

		// 처음 방문하는 타일이면 현제 길찾기 스텝 번호 저장
		if (m_visitedOrder[nextY][nextX] == 0)
		{
			m_visitedOrder[nextY][nextX] = m_currentStepCount;
		}

		// 목적지 만나면 리턴
		if (nextX == m_destX && nextY == m_destY) return { nextX, nextY };

		// 대각선 탐색하기 전에 DD, RR 탐색
		std::pair<int, int> DDresult = JumpToDD(nextX, nextY, parentNode);
		std::pair<int, int> RRresult = JumpToRR(nextX, nextY, parentNode);

		// 두 방향 중에 하나라도 탐색 중에 코너를 발견되었다면 현재 대각선 위치가 좌표가 점프 포인트가 되서 노드를 생성해야 함
		if (DDresult.first != -1 || RRresult.first != -1 ) return { nextX, nextY };

		// 코너 체크 
		bool isLeftWall = (nextX - 1 >= 0 ) && (g_Tile[nextY][nextX - 1] == 1);
		bool isLeftBottomWallEmpty = (nextY + 1 < GRID_HEIGHT) && (nextX - 1 >= 0) && (g_Tile[nextY + 1][nextX - 1] != 1);
		bool isTopWall =  (nextY - 1 >= 0) && (g_Tile[nextY - 1][nextX] == 1);
		bool isRightTopWallEmpty = (nextY - 1 >= 0) && ( nextX + 1 < GRID_WIDTH ) && (g_Tile[nextY - 1][nextX + 1] != 1);

		if ((isLeftWall && isLeftBottomWallEmpty) || (isTopWall && isRightTopWallEmpty)) return { nextX, nextY };
	
	}
}

std::pair<int, int> JPS::JumpToLU(int curx, int cury, JPSNode* parentNode)
{
	int nextX = curx;
	int nextY = cury;

	while ( 1 )
	{
		nextX--;
		nextY--;

		// 벽 만나거나 그리드 범위를 벗어나면 종료
		if ( nextX < 0 || nextY < 0 || g_Tile[nextY][nextX] == 1) return { -1, -1 };

		// 처음 방문하는 타일이면 현제 길찾기 스텝 번호 저장
		if (m_visitedOrder[nextY][nextX] == 0)
		{
			m_visitedOrder[nextY][nextX] = m_currentStepCount;
		}

		// 목적지 만나면 리턴
		if (nextX == m_destX && nextY == m_destY) return { nextX, nextY };

		// 대각선 탐색하기 전에 DD, RR 탐색
		std::pair<int, int> LLresult = JumpToLL(nextX, nextY, parentNode);
		std::pair<int, int> UUresult = JumpToUU(nextX, nextY, parentNode);

		// 두 방향 중에 하나라도 탐색 중에 코너를 발견되었다면 현재 대각선 위치가 좌표가 점프 포인트가 되서 노드를 생성해야 함
		if (LLresult.first != -1 || UUresult.first != -1) return { nextX, nextY };

		// 코너 체크 
		bool isRightWall = (nextX + 1 < GRID_WIDTH) && (g_Tile[nextY][nextX + 1] == 1);
		bool isRightTopWallEmpty = (nextY - 1 >= 0 ) && (nextX + 1 < GRID_WIDTH) && (g_Tile[nextY - 1][nextX + 1] != 1);
		bool isBottomWall = (nextY + 1 < GRID_HEIGHT) && (g_Tile[nextY + 1][nextX] == 1);
		bool isLeftBottomWallEmpty = (nextY + 1 < GRID_HEIGHT) && ( nextX - 1 >= 0 ) && (g_Tile[nextY + 1][nextX - 1] != 1);

		if ((isRightWall && isRightTopWallEmpty) || (isBottomWall && isLeftBottomWallEmpty)) return { nextX, nextY };
		
	}
}

std::pair<int, int> JPS::JumpToLD(int curx, int cury, JPSNode* parentNode)
{
	int nextX = curx;
	int nextY = cury;

	while ( 1 )
	{
		nextX--;
		nextY++;

		// 벽 만나거나 그리드 범위를 벗어나면 종료
		if (nextX < 0 || nextY >= GRID_HEIGHT || g_Tile[nextY][nextX] == 1) return { -1, -1 };

		// 처음 방문하는 타일이면 현제 길찾기 스텝 번호 저장
		if (m_visitedOrder[nextY][nextX] == 0)
		{
			m_visitedOrder[nextY][nextX] = m_currentStepCount;
		}

		// 목적지 만나면 리턴
		if (nextX == m_destX && nextY == m_destY) return { nextX, nextY };

		// 대각선 탐색하기 전에 DD, RR 탐색
		std::pair<int, int> LLresult = JumpToLL(nextX, nextY, parentNode);
		std::pair<int, int> DDresult = JumpToDD(nextX, nextY, parentNode);

		// 두 방향 중에 하나라도 탐색 중에 코너를 발견되었다면 대각선 방향의 노드도 생성
		if (LLresult.first != -1 || DDresult.first != -1) return { nextX, nextY };

		// 코너 체크 
		bool isTopWall = (nextY - 1 >= 0) &&(g_Tile[nextY - 1][nextX] == 1);
		bool isRightTopWallEmpty = (nextY - 1 >= 0) && ( nextX + 1 < GRID_WIDTH) && (g_Tile[nextY - 1][nextX + 1] != 1);
		bool isRightWall = (nextX + 1 < GRID_WIDTH) && (g_Tile[nextY][nextX + 1] == 1);
		bool isRightBottomWallEmpty = (nextY + 1 < GRID_HEIGHT) && ( nextX + 1 < GRID_WIDTH) && (g_Tile[nextY + 1][nextX + 1] != 1);

		if ((isTopWall && isRightTopWallEmpty) || (isRightWall && isRightBottomWallEmpty)) return { nextX, nextY };
		
	}
}
