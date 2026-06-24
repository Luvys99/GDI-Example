#pragma once
#include <vector>
#include "Render.h"

struct ASTARNode
{
	int xPos;
	int yPos;
	int h;
	int g;
	int f;
	bool isOpen;
	bool isClose;
	bool isPath;
	ASTARNode* parentNode;
	ASTARNode()
	{
		parentNode = nullptr;
		h = 0, g = 0;
		xPos = -1, yPos = -1; // -1이라면 아직 노드가 만들어지지 않음
		isOpen = false;
		isClose = false;
		isPath = false;
		f = 0;
	}
	// h, f 계산하는 함수( 맨하튼 방식으로 H 값 계산 )
	inline void CalculateHeuristic(int destX, int destY) noexcept
	{
		h = (abs(destX - xPos) + abs(destY - yPos)) * 10;
		f = g + h;
	}
};

class ASTAR
{
public:
	ASTAR() : startNode(nullptr), m_startX(-1), m_startY(-1), m_destX(-1), m_destY(-1) { }
	~ASTAR() {}

	inline ASTARNode* GetStartNode() const noexcept { return startNode; }
	inline int GetStartX() const noexcept { return m_startX; }
	inline int GetStartY() const noexcept { return m_startY; }
	inline int GetDestX() const noexcept { return m_destX; }
	inline int GetDestY() const noexcept { return m_destY; }

	inline void PushOpenList(ASTARNode* node) noexcept
	{
		OpenList.push_back(node);
	}

	void CreateStartNode(int startX, int startY) noexcept;

	// 오픈 리스트가 비어있는지 확인
	inline bool IsEmptyList() const noexcept { return OpenList.empty(); }

	// 목적지 좌표 저장
	void SetDestPos(int destX, int destY) noexcept; 

	// A* 알고리즘으로 길 찾기
	bool FindPathStep();

	// 유효한 노드인지 검사하는 함수
	bool IsValidNode(int curX, int curY, int xPos, int yPos, ASTARNode* node) const noexcept;

	// 길찾기 할 때마다 초기화함
	void InitAStarPos() noexcept;

	// 2D 노드맵 순회하면서 메모리 해체 ( 초기화 )
	void ClearPathData() noexcept;

	// 클로즈 리스트를 순회해서 좌표를 찾는 것은 탐색이 진행될수록
	// 순회해야 하는 리스트가 너무 커지기 때문에 검사 속도가 굉장히 느려진다.
	ASTARNode* m_nodeMap[GRID_HEIGHT][GRID_WIDTH];
	ASTARNode* GetNodeFromMap(int xPos, int yPos);

private:
	// GDI 그리기 위해 저장하는 출발점, 목적지 좌표
	int m_startX;
	int m_startY;
	int m_destX;
	int m_destY;

	ASTARNode* startNode;
	std::vector<ASTARNode*> OpenList;
};