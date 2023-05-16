// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once

#include<vector>
#include "Vector.h"
#include "Fragment.h"

// ËÄ²æÊ÷½Úµã
class QuadNode
{
public:
	QuadNode() {}
	QuadNode(int minX, int maxX, int minY, int maxY)
	{
		this->minX = minX;
		this->maxX = maxX;
		this->minY = minY;
		this->maxY = maxY;
		this->z = -FLT_MAX;
	}

	float z = -FLT_MAX;
	int minX, maxX;
	int minY, maxY;
	
	std::vector<QuadNode*> children;
	QuadNode* father = NULL;

	void release()
	{
		for (QuadNode* child : children)
		{
			if (child != NULL) child->release();
		}
		children.clear();
	}
private:
};