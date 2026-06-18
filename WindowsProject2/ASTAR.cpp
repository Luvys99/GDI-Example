#include "ASTAR.h"
#include "Render.h"
#include <algorithm>
#include "MsgProcess.h"

void ASTAR::CreateStartNode(int startX, int startY) noexcept
{
	// 시작 노드 생성
	startNode = new Node();
	startNode->xPos = startX;
	startNode->yPos = startY;

	m_nodeMap[startY][startX] = startNode;

	m_startX = startX;
	m_startY = startY;
}

void ASTAR::SetDestPos(int destX, int destY) noexcept
{
	m_destX = destX;
	m_destY = destY;
}

bool ASTAR::FindPathStep()
{
	// 오픈 리스트에 꺼낼 노드가 없다면 실패
	if (OpenList.empty())
	{
		return false;
	}

	// 오픈 리스트에서 F 값이 가장 낮은 노드를 맨 뒤로 정렬 ( 내림차순 )
	// vector에서 맨 앞 노드를 꺼내면 앞으로 원소들을 끌어와야 하기 때문에 오버헤드가 발생함
	Node* currentNode = OpenList.back();
	OpenList.pop_back(); // 뒤에서 빼면 원소의 이동이 없어짐

	currentNode->isOpen = false;
	currentNode->isClose = true;

	// 목적지에 도착했다면 종료
	if (currentNode->xPos == m_destX && currentNode->yPos == m_destY)
	{
		// 역주적 로직 실행해서 최단 경로에 대한 플래그를 찍어주기
		Node* traceNode = currentNode;
		while (traceNode != nullptr)
		{
			traceNode->isPath = true;
			traceNode = traceNode->parentNode;
		}
		return true;
	}

	// 북서, 북, 북동, 서, 동, 남서, 남, 남동 순
	int dx[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	int dy[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	int diagonal[] = { 15, 10, 15, 10, 10, 15, 10, 15 }; // 대각선은 15, 직선 10

	for (int i = 0; i < 8; i++)
	{
		int nx = currentNode->xPos + dx[i];
		int ny = currentNode->yPos + dy[i];

		Node* neighborNode = GetNodeFromMap(nx, ny);

		// 벽이거나 맵 밖이거나 클로드 노드라면 다음 방향으로 넘어감
		if (IsValidNode(currentNode->xPos, currentNode->yPos, nx, ny, neighborNode) == false) continue;

		// 새로운 G 갱신
		int newG = currentNode->g + diagonal[i];

		// 유효한 노드가 오픈 리스트에 없다면? 한 번도 탐색하지 않은 길인 것 
		if (neighborNode == nullptr)
		{
			neighborNode = new Node();
			neighborNode->xPos = nx;
			neighborNode->yPos = ny;

			// 새로 만든 노드를 2D 배열에 등록
			m_nodeMap[ny][nx] = neighborNode;

			// g, h, f 값 계산해서 갱신
			neighborNode->g = newG;
			neighborNode->CalculateHeuristic(m_destX, m_destY);

			// 이웃의 부모를 현재 노드로 연결 후 오픈 리스트에 저장
			neighborNode->parentNode = currentNode;
			neighborNode->isOpen = true;

			OpenList.push_back(neighborNode);
		}
		else if (neighborNode->isOpen) // 이웃노드가 이미 오픈 리스트에 존재하는 노드였다면
		{
			// 이번에 새로 계산한 길이 더 짧다면 갱신
			if (newG < neighborNode->g)
			{
				neighborNode->g = newG;
				neighborNode->CalculateHeuristic(m_destX, m_destY); // 더 짧아진 g 값으로 다시 h, f 계산

				// 더 짧은 길을 찾았기 때문에 부모를 현재 노드로 변경
				neighborNode->parentNode = currentNode;
			}
		}
	}

	// 오픈 리스트 f 값을 기준으로 내림차순
	// sort는 시작점, 끝점, 정렬 규칙을 인자로 받고 람다 함수를 통해서 정렬의 기준을 정의함
	// OpenList의 두 개의 노드를 꺼내서 비교 했을 때 f값이 더 작은 노드가 앞으로 오게끔 정렬하라는 것
	std::sort(OpenList.begin(), OpenList.end(), [](Node* a, Node* b) { 
		if (a->f == b->f)
		{
			// F가 같으면 h가 작은 노드를 뒤로 보냄 ( 내림 차순 )
			// f값이 같다면 h목적지 거리가 더 가까운 노드가 우선순위로 탐색이 되어야 한다. 
			return a->h > b->h;
		}
		// F가 작은 노드를 맨뒤로
		return a->f > b->f; 
	});

	// 아직 찾는 중
	return false;

}

bool ASTAR::IsValidNode(int curX, int curY, int xPos, int yPos, Node* node) const noexcept
{

	// 맵 밖인지 검사
	if (xPos < 0 || xPos >= GRID_WIDTH || yPos < 0 || yPos >= GRID_HEIGHT) return false;
	// 벽인지 검사
	if (g_Tile[yPos][xPos] == 1) return false;
	// 클로즈 리스트에 있는지 검사
	if (node != nullptr && node->isClose == true) return false;

	/*// 대각선으로 벽을 뚫고 이동하는 경우 차단
	if (curX != xPos && curY != yPos)
	{
		// 이동하려는 대각선 경로의 양 옆 타일이 하나
		if (g_Tile[curY][xPos] == 1 || g_Tile[yPos][curX] == 1)
		{
			return false;
		}
	}
	*/
	return true;

}

Node* ASTAR::GetNodeFromMap(int xPos, int yPos)
{
	if (xPos < 0 || xPos >= GRID_WIDTH || yPos < 0 || yPos >= GRID_HEIGHT) return nullptr;

	// 2D 배열에서 해당 좌표의 노드 포인터를 즉시 반환
	return m_nodeMap[yPos][xPos];

}

void ASTAR::ClearPathData() noexcept
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
	OpenList.clear();

}

void ASTAR::InitAStarPos() noexcept
{
	
	m_startX = -1;
	m_startY = -1;
	m_destX = -1;
	m_destY = -1;

}