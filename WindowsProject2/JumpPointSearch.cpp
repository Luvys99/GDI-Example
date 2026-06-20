#include "JumpPointSearch.h"

bool JPS::FindPathStep()
{

	// 오픈 리스트에 노드가 없다면 탐색 종료
	if (OpenList.empty())
	{
		return false;
	}

	// 오픈 리스트에서 맨 앞에서 빼낼 때마다 정렬하는 오버헤드를 줄이기 위해서 
	// 정렬 할 때 뒤에서부터 정렬을 함으로써 마지막 원소만 빼낼 때 O(1)의 비용으로 처리할 수 있게 함
	Node* currentNode = OpenList.back();
	OpenList.pop_back();

	// 현재 노드의 좌표 저장
	int currentX = currentNode->xPos;
	int currentY = currentNode->yPos;

	// Close List에 저장
	currentNode->isOpen = false;
	CloseList[currentY][currentX] = 1;

	// 목적지에 도달했다면 최단 거리 부모 포인터로 따라가서 ispath 플래그 표시
	if (currentNode->xPos == m_destX && currentNode->yPos == m_destY)
	{
		Node* traceNode = currentNode;
		while (traceNode != nullptr)
		{
			traceNode->isPath = true;
			traceNode = traceNode->parentNode;
		}
		return true;
	}

	// 출발지에서 8 방향으로 탐색 시작.
	// 매번 한 스텝씩 길을 찾을 때 8방향 모두 탐색함수를 호출하게 하는 것은 낭비 ( 8방향이 아니라 3방향만 탐색해야 할 경우가 생기기 때문에 분기문으로 걸러줘야 됨 )
	// 현재 노드는 오픈 리스트에 존재하는 F가 가장 낮은 노드로 그 노드에서 탐색할 수 있는 모든 방향을 탐색하는 것이 1 step

	// 오른쪽 방향
	if (currentX + 1 < GRID_WIDTH && g_Tile[currentY][currentX + 1] != 1 && CloseList[currentY][currentX + 1] != 1)
	{
		JumpToRR(currentX, currentY,currentNode);
	}

	// 왼쪽 방향
	if (currentX - 1 < GRID_WIDTH && g_Tile[currentY][currentX - 1] != 1 && CloseList[currentY][currentX - 1] != 1)
	{
		JumpToLL(currentX, currentY, currentNode);
	}

	// 윗쪽 방향
	if (currentY - 1 < GRID_HEIGHT && g_Tile[currentY-1][currentX] != 1 && CloseList[currentY-1][currentX] != 1)
	{
		JumpToUU(currentX, currentY, currentNode);
	}

	// 아래쪽 방향
	if (currentY + 1 < GRID_HEIGHT && g_Tile[currentY+1][currentX] != 1 && CloseList[currentY+1][currentX] != 1)
	{
		JumpToDD(currentX, currentY, currentNode);
	}

	// 우측 상단 방향
	

}

void JPS::CreateStartNode(int startX, int startY) noexcept
{
	// 시작 노드 생성
	startNode = new Node();
	startNode->xPos = startX;
	startNode->yPos = startY;

	m_nodeMap[startY][startX] = startNode;

	m_startX = startX;
	m_startY = startY;
}

void JPS::CreateNode(int curX, int curY, Node* currentNode) noexcept
{
	Node* node = new Node();
	node->xPos = curX;
	node->yPos = curY;
	node->CalculateHeuristic(curX, curY);

	node->parentNode = currentNode;
	node->isOpen = true;

	// 오픈리스트에 저장
	OpenList.push_back(node);
}

Node* JPS::GetNodeFromMap(int xPos, int yPos) const noexcept
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

// 8방향으로 탐색하는 함수들
// 수직, 수평

void JPS::JumpToLL(int curx, int cury, Node* currentNode)
{
	int nextX = curx;
	int nextY = cury;

	// 벽을 만날 때까지 반복
	while (1)
	{
		// 다음 왼쪽 방향 타일 좌표
		nextX--;

		// 벽 만나면 종료
		if (g_Tile[nextY][nextX] == 1) break;

		// 목적지 만나면 리턴
		if (nextX == m_destX && nextY == m_destY)
		{
			return CreateNode(nextX, nextY, currentNode);
		}

		// 코너체크 4군데
		bool isTopWall = (g_Tile[nextY - 1][nextX] == 1);
		bool isTopLeftWallEmpty = (g_Tile[nextY - 1][nextX - 1] != 1);
		bool isBottomWall = (g_Tile[nextY + 1][nextX] == 1);
		bool isBottomLeftWallEmpty = (g_Tile[nextY + 1][nextX - 1] != 1);

		// Top, BottomWall이 존재하고 TopRightWall, BottomRightWall이 존재하지 않으면 노드 생성
		// TopWall이 있고 TopWallRight가 비어있다면 코너를 찾은 것 또는 BottomWall이 있고 BottomWallRight가 비어있다면 코너를 찾은 것
		if ((isTopWall && isTopLeftWallEmpty) || (isBottomWall && isBottomLeftWallEmpty))
		{
			// 노드를 생성하면 생성한 노드부터
			return CreateNode(nextX, nextY, currentNode);
		}
	}
}

void JPS::JumpToDD(int curx, int cury, Node* currentNode)
{
	int nextX = curx;
	int nextY = cury;

	// 벽을 만날 때까지 반복
	while (1)
	{
		// 다음 왼쪽 방향 타일 좌표
		nextY++;

		// 벽 만나면 종료
		if (g_Tile[nextY][nextX] == 1) break;

		// 목적지 만나면 리턴
		if (nextX == m_destX && nextY == m_destY)
		{
			return CreateNode(nextX, nextY, currentNode);
		}

		// 코너체크 4군데
		bool isRightWall = (g_Tile[nextY][nextX + 1] == 1);
		bool isRightDownWallEmpty = (g_Tile[nextY + 1][nextX + 1] != 1);
		bool isLeftWall = (g_Tile[nextY][nextX - 1] == 1);
		bool isLeftDownWallEmpty = (g_Tile[nextY + 1][nextX - 1] != 1);

		// Top, BottomWall이 존재하고 TopRightWall, BottomRightWall이 존재하지 않으면 노드 생성
		// TopWall이 있고 TopWallRight가 비어있다면 코너를 찾은 것 또는 BottomWall이 있고 BottomWallRight가 비어있다면 코너를 찾은 것
		if ((isRightWall && isRightDownWallEmpty) || (isLeftWall && isLeftDownWallEmpty))
		{
			// 노드를 생성하면 생성한 노드부터
			return CreateNode(nextX, nextY, currentNode);
		}
	}
}

void JPS::JumpToRR(int curx, int cury, Node* currentNode)
{
	int nextX = curx;
	int nextY = cury;

	// 벽을 만날 때까지 반복
	while (1)
	{
		// 다음 오른쪽 방향 타일 좌표
		nextX++;

		// 벽 만나면 종료
		if (g_Tile[nextY][nextX] == 1) break;

		// 목적지 만나면 리턴
		if (nextX == m_destX && nextY == m_destY)
		{
			return CreateNode(nextX, nextY, currentNode);
		}
		
		// 코너체크 4군데
		bool isTopWall = (g_Tile[nextY - 1][nextX] == 1);
		bool isTopRightWallEmpty = (g_Tile[nextY - 1][nextX + 1] != 1);
		bool isBottomWall = (g_Tile[nextY + 1][nextX] == 1);
		bool isBottomRightWallEmpty = (g_Tile[nextY + 1][nextX + 1] != 1);

		// Top, BottomWall이 존재하고 TopRightWall, BottomRightWall이 존재하지 않으면 노드 생성
		// TopWall이 있고 TopWallRight가 비어있다면 코너를 찾은 것 또는 BottomWall이 있고 BottomWallRight가 비어있다면 코너를 찾은 것
		if ((isTopWall && isTopRightWallEmpty) || (isBottomWall && isBottomRightWallEmpty))
		{
			// 노드를 생성하면 생성한 노드부터
			return CreateNode(nextX, nextY, currentNode);
		}
	}
}

void JPS::JumpToUU(int curx, int cury, Node* currentNode)
{
	int nextX = curx;
	int nextY = cury;

	// 벽을 만날 때까지 반복
	while (1)
	{
		// 다음 왼쪽 방향 타일 좌표
		nextY--;

		// 벽 만나면 종료
		if (g_Tile[nextY][nextX] == 1) break;

		// 목적지 만나면 리턴
		if (nextX == m_destX && nextY == m_destY)
		{
			return CreateNode(nextX, nextY, currentNode);
		}

		// 코너체크 4군데
		bool isRightWall = (g_Tile[nextY][nextX + 1] == 1);
		bool isRightUpWallEmpty = (g_Tile[nextY - 1][nextX + 1] != 1);
		bool isLeftWall = (g_Tile[nextY][nextX - 1] == 1);
		bool isLeftUpWallEmpty = (g_Tile[nextY - 1][nextX - 1] != 1);

		// Top, BottomWall이 존재하고 TopRightWall, BottomRightWall이 존재하지 않으면 노드 생성
		// TopWall이 있고 TopWallRight가 비어있다면 코너를 찾은 것 또는 BottomWall이 있고 BottomWallRight가 비어있다면 코너를 찾은 것
		if ((isRightWall && isRightUpWallEmpty) || (isLeftWall && isLeftUpWallEmpty))
		{
			// 노드를 생성하면 생성한 노드부터
			return CreateNode(nextX, nextY, currentNode);
		}
	}
}

// 대각선
void JPS::JumpToRU(int curx, int cury, Node* currentNode)
{




}

void JPS::JumpToRD(int curx, int cury, Node* currentNode)
{

}

void JPS::JumpToLU(int curx, int cury, Node* currentNode)
{

}

void JPS::JumpToLD(int curx, int cury, Node* currentNode)
{

}

void JPS::JumpToLL(int curx, int cury, Node* currentNode)
{

}