#pragma once
#include <vector>
#include "Render.h"

// 방향


struct Node
{
	int xPos;
	int yPos;
	int h;
	int g;
	int f;
	bool isvisited;
	bool isOpen;
	bool isClose;
	bool isPath;
	Node* parentNode;
	Node()
	{
		parentNode = nullptr;
		h = 0, g = 0;
		xPos = -1, yPos = -1; 
		isvisited = false;
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

class JPS
{
public:
	JPS() : startNode(nullptr), m_startX(-1), m_startY(-1), m_destX(-1), m_destY(-1) { }

	inline Node* GetStartNode() const noexcept { return startNode; }
	inline int GetStartX() const noexcept { return m_startX; }
	inline int GetStartY() const noexcept { return m_startY; }
	inline int GetDestX() const noexcept { return m_destX; }
	inline int GetDestY() const noexcept { return m_destY; }

	inline void PushOpenList(Node* node) noexcept
	{
		OpenList.push_back(node);
	}

	// 시작 노드 생성
	void CreateStartNode(int startX, int startY) noexcept;

	// 노드 생성
	void CreateNode(int curX, int curY, Node* currentNode) noexcept;

	// 목적지 좌표 저장
	void SetDestPos(int destX, int destY) noexcept;

	// 길찾기 할 때마다 초기화함
	void InitAStarPos() noexcept;

	// JPS 알고리즘으로 길 찾기
	bool FindPathStep();

	// 8방향으로 탐색하는 함수들
	// 수직, 수평
	void JumpToLL(int curx, int cury, Node* currentNode);
	void JumpToDD(int curx, int cury, Node* currentNode);
	void JumpToRR(int curx, int cury, Node* currentNode);
	void JumpToUU(int curx, int cury, Node* currentNode);
	// 대각선
	void JumpToRU(int curx, int cury, Node* currentNode);
	void JumpToRD(int curx, int cury, Node* currentNode);
	void JumpToLU(int curx, int cury, Node* currentNode);
	void JumpToLD(int curx, int cury, Node* currentNode);
	void JumpToLL(int curx, int cury, Node* currentNode);


private:
	int m_startX;
	int m_startY;
	int m_destX;
	int m_destY;

	Node* startNode;
	std::vector<Node*> OpenList;
	bool CloseList[GRID_HEIGHT][GRID_WIDTH];
	
	Node* m_nodeMap[GRID_HEIGHT][GRID_WIDTH];
	Node* GetNodeFromMap(int xPos, int yPos) const noexcept;

};