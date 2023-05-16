// Author: Peiyao Li
// Date:   Jan 8 2023
#include "HieraZBuffer.h"
#include "Quadtree.h"
#include "Timer.h"

HieraZBuffer::HieraZBuffer(int w, int h)
{
	width = w;
	height = h;
	initialize();
}

HieraZBuffer::~HieraZBuffer()
{
	release();
}

void HieraZBuffer::initialize()
{
	frameBuffer = new Color3f[width * height];
	for (int i = 0; i < width * height; i++) {
		frameBuffer[i] = Color3f(0, 0, 0);
	}

	zBuffer = new QuadNode[width * height];
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int loc = y * width + x;
			zBuffer[loc].minX = zBuffer[loc].maxX = x;
			zBuffer[loc].minY = zBuffer[loc].maxY = y;
			zBuffer[loc].z = -FLT_MAX;
		}
	}
	zPyramid = buildZPyramid(0, width - 1, 0, height - 1);
}

QuadNode* HieraZBuffer::buildZPyramid(int minX, int maxX, int minY, int maxY)
{
	// 单个像素 直接返回一维的底层ZBuffer
	if (minX == maxX && minY == maxY)
	{
		int loc = minY * width + minX;
		return &(zBuffer[loc]);
	}
	// 非单个像素 新建Node
	QuadNode* node = new QuadNode(minX, maxX, minY, maxY);


	int middleX = (minX + maxX) / 2, middleY = (minY + maxY) / 2;

	QuadNode* child = buildZPyramid(minX, middleX, minY, middleY);
	child->father = node;
	node->children.push_back(child);

	if (middleX + 1 <= maxX)
	{
		child = buildZPyramid(middleX + 1, maxX, minY, middleY);
		child->father = node;
		node->children.push_back(child);
	}
	if (middleY + 1 <= maxY)
	{
		child = buildZPyramid(minX, middleX, middleY + 1, maxY);
		child->father = node;
		node->children.push_back(child);
	}
	if (middleX + 1 <= maxX && middleY + 1 <= maxY)
	{
		child = buildZPyramid(middleX + 1, maxX, middleY + 1, maxY);
		child->father = node;
		node->children.push_back(child);
	}
	return node;
}

void HieraZBuffer::renderPolygons(Polygons& polygons)
{
	Timer timer;
	timer.start();
	int polygonSize = polygons.getSize();
	for (int i = 0; i < polygonSize; i++)
	{
		renderPyramidRange(polygons.fragments[i], zPyramid);
	}
	timer.end();
	timer.printTimeCost("Hierarchical ZBuffer");
}

bool HieraZBuffer::rejectPolygon(Fragment& fragment, QuadNode* qNode)
{
	if (fragment.getMaxZ() <= qNode->z)
		return true;
	else return false;
}

bool HieraZBuffer::containPolygon(Fragment& fragment, QuadNode* qNode)
{
	if (fragment.getMinX() >= qNode->minX && fragment.getMaxX() <= qNode->maxX
		&& fragment.getMinY() >= qNode->minY && fragment.getMaxY() <= qNode->maxY)
		return true;
	else return false;
}

void HieraZBuffer::renderPyramidRange(Fragment& fragment, QuadNode* qNode)
{
	if (rejectPolygon(fragment, qNode))
	{
		return;
	}
	else
	{
		bool flag = false;
		int idx = 0;
		for (idx = 0; idx < qNode->children.size(); idx++)
		{
			if (containPolygon(fragment, qNode->children[idx]))
			{
				flag = true;
				break;
			}
		}
		if (flag) {
			renderPyramidRange(fragment, qNode->children[idx]);
		}
		else {
			renderNodeRange(fragment, qNode);
		}
	}
}


float HieraZBuffer::renderNodeRange(Fragment& fragment, QuadNode* qNode)
{
	// 被拒绝的多边形 不绘制
	if (qNode->minX > qNode->maxX || qNode->minY > qNode->maxY)
	{
		return -1;
	}
	// 提前剪枝一些没有覆盖多边形的zbuffer块
	if (qNode->maxX < fragment.getMinX() || qNode->minX > fragment.getMaxX()
		|| qNode->maxY < fragment.getMinY() || qNode->minY > fragment.getMaxY())
	{
		int loc = qNode->minY * width + qNode->minX;
		return zBuffer[loc].z;
	}
	if (qNode->minX == qNode->maxX && qNode->minY == qNode->maxY)
	{
		int loc = qNode->minY * width + qNode->minX;
		float z = fragment.caculateZ(Vec2f(qNode->minX + 0.5, qNode->minY + 0.5));
		if (!isEqualf(z, -FLT_MAX) && z > zBuffer[loc].z)
		{
			zBuffer[loc].z = z;
			frameBuffer[loc] = fragment.faceColor;
		}
		return zBuffer[loc].z;		
	}
	else
	{
		float minZ = FLT_MAX;
		for (QuadNode* child : qNode->children)
		{
			float tmpZ = renderNodeRange(fragment, child);
			minZ = std::fmin(minZ, tmpZ);
		}
		qNode->z = minZ;
		return qNode->z;
	}
}



void HieraZBuffer::release()
{
	if (frameBuffer != NULL)
		delete[] frameBuffer;
	zPyramid->release();
	if (zBuffer != NULL)
		delete[] zBuffer;
}