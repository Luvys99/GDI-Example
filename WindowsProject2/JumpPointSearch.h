#pragma once
#include <vector>
#include <queue>
#include "Render.h"

// 방향


struct JPSNode
{
	int xPos;
	int yPos;
	int h;
	int g;
	int f;
	bool isOpen;
	bool isPath;
	bool isClose;
	JPSNode* parentNode;
	JPSNode()
	{
		parentNode = nullptr;
		h = 0, g = 0;
		f = 0;
		xPos = -1, yPos = -1; 
		isOpen = false;
		isPath = false;
		isClose = false;
	}
	// h, f 계산하는 함수( 맨하튼 방식으로 H 값 계산 )
	inline void CalculateHeuristic(int destX, int destY) noexcept
	{
		h = (abs(destX - xPos) + abs(destY - yPos)) * 10;
		f = g + h;
	}
};

// priority queue 비교용 구조체 ( 람다 함수랑 같은 원리 )
struct CompareNode
{
	bool operator()(const JPSNode* a, const JPSNode* b) const
	{
		// f값이 같다면 h가 더 작은 노드가 우선순위가 낮다.
		// 기본적으로 최대힙이기 때문에 최소힙으로 올르려면 부등호를 반대로 해줘야 함
		if (a->f == b->f)
		{
			return a->h > b->h;
		}

		// a의 f값이 더 크면 true를 반환해서 a를 큐 아래로 내려보냄
		return a->f > b->f;
	}
};

class JPS
{
public:
	JPS() : startNode(nullptr), m_startX(-1), m_startY(-1), m_destX(-1), m_destY(-1), CountOfDirections(0){ }

	inline JPSNode* GetStartNode() const noexcept { return startNode; }
	inline int GetStartX() const noexcept { return m_startX; }
	inline int GetStartY() const noexcept { return m_startY; }
	inline int GetDestX() const noexcept { return m_destX; }
	inline int GetDestY() const noexcept { return m_destY; }

	inline void PushOpenList(JPSNode* node) noexcept
	{
		OpenList.push(node);
	}

	// 시작 노드 생성
	void CreateStartNode(int startX, int startY);

	// 노드 생성, 가중치 갱신 함수
	void CreateNodeorUpateNode(int targetxpos, int targetypos, JPSNode* currentNode);

	// 두 좌표 사이의 G 비용을 계산하는 함수
	int CalculateJumpDistance(int startX, int startY, int endX, int endY) noexcept;

	// 목적지 좌표 저장
	void SetDestPos(int destX, int destY) noexcept;

	// 길찾기 할 때마다 초기화함
	void InitAStarPos() noexcept;

	void ClearPathData() noexcept;

	// JPS 알고리즘으로 길 찾기
	bool FindPathStep();

	// 8방향으로 탐색하는 함수들
	// 수직, 수평
	std::pair<int, int> JumpToLL(int curx, int cury, JPSNode* parentNode);
	std::pair<int, int> JumpToDD(int curx, int cury, JPSNode* parentNode);
	std::pair<int, int> JumpToRR(int curx, int cury, JPSNode* parentNode);
	std::pair<int, int> JumpToUU(int curx, int cury, JPSNode* parentNode);
	// 대각선
	std::pair<int, int> JumpToRU(int curx, int cury, JPSNode* parentNode);
	std::pair<int, int> JumpToRD(int curx, int cury, JPSNode* parentNode);
	std::pair<int, int> JumpToLU(int curx, int cury, JPSNode* parentNode);
	std::pair<int, int> JumpToLD(int curx, int cury, JPSNode* parentNode);

	JPSNode* GetNodeFromMap(int xPos, int yPos) const noexcept;
private:
	int m_startX;
	int m_startY;
	int m_destX;
	int m_destY;

	JPSNode* startNode;
	// 우선순위 큐로 구현한 오픈 리스트( 최소 힙 )
	std::priority_queue<JPSNode*,std::vector<JPSNode*>, CompareNode> OpenList;
	std::vector<JPSNode*> CloseList;

	JPSNode* m_nodeMap[GRID_HEIGHT][GRID_WIDTH];
	int CountOfDirections; // 탐색 횟수 ( 횟수 따라 색 변경 )
};