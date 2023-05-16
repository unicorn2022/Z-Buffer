// Author: Peiyao Li
// Date:   Jan 8 2023
#include "OctreeHieraZBuffer.h"
#include "Quadtree.h"
#include "Timer.h"

OctreeHieraZBuffer::OctreeHieraZBuffer(int w, int h)
{
	width = w;
	height = h;
	initialize();
}

OctreeHieraZBuffer::~OctreeHieraZBuffer()
{
	release();
}

void OctreeHieraZBuffer::initialize()
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

void OctreeHieraZBuffer::buildScene(Polygons& polygons)
{
	Timer timer;
	timer.start();
	scene = buildOctree(0, width - 1, 0, height - 1, polygons.minZ, polygons.maxZ, polygons);
	timer.end();
	timer.printTimeCost("Build Scene Octree");
}

OctoNode* OctreeHieraZBuffer::buildOctree(int minX, int maxX, int minY, int maxY, float minZ, float maxZ, Polygons& polygons)
{
	if (minX > maxX || minY > maxY || minZ > maxZ)
	{
		return NULL;
	}

	// 包含的多边形小于设置好的容量
	if (polygons.getSize() <= this->capacity || minX == maxX || minY == maxY)
	{
		OctoNode* octoNode = new OctoNode(minX, maxX, minY, maxY, minZ, maxZ, polygons);
		octoNode->z = maxZ;
		octoNode->polygons = polygons;
		return octoNode;
	}

	// 仍然需要分割
	OctoNode* node = new OctoNode(minX, maxX, minY, maxY, minZ, maxZ, polygons);

	int middleX = (minX + maxX) / 2, middleY = (minY + maxY) / 2;
	float middleZ = (minZ + maxZ) / 2;

	Polygons childPolygons[8];
	for (Fragment& fragment : polygons.fragments)
		if (fragment.getMaxX() <= middleX && fragment.getMaxY() <= middleY && fragment.getMinZ() >= middleZ)
			childPolygons[0].fragments.push_back(fragment);
		else if (fragment.getMaxX() <= middleX && fragment.getMaxY() <= middleY && fragment.getMaxZ() <= middleZ)
			childPolygons[1].fragments.push_back(fragment);
		else if (fragment.getMinX() >= middleX && fragment.getMaxY() <= middleY && fragment.getMinZ() >= middleZ)
			childPolygons[2].fragments.push_back(fragment);
		else if (fragment.getMinX() >= middleX && fragment.getMaxY() <= middleY && fragment.getMaxZ() <= middleZ)
			childPolygons[3].fragments.push_back(fragment);
		else if (fragment.getMaxX() <= middleX && fragment.getMinY() >= middleY && fragment.getMinZ() >= middleZ)
			childPolygons[4].fragments.push_back(fragment);
		else if (fragment.getMaxX() <= middleX && fragment.getMinY() >= middleY && fragment.getMaxZ() <= middleZ)
			childPolygons[5].fragments.push_back(fragment);
		else if (fragment.getMinX() >= middleX && fragment.getMinY() >= middleY && fragment.getMinZ() >= middleZ)
			childPolygons[6].fragments.push_back(fragment);
		else if (fragment.getMinX() >= middleX && fragment.getMinY() >= middleY && fragment.getMaxZ() <= middleZ)
			childPolygons[7].fragments.push_back(fragment);
		else
			node->polygons.fragments.push_back(fragment);


	node->children.push_back(buildOctree(minX, middleX, minY, middleY, minZ, middleZ, childPolygons[0]));
	node->children.push_back(buildOctree(minX, middleX, minY, middleY, middleZ, maxZ, childPolygons[1]));
	node->children.push_back(buildOctree(middleX + 1, maxX, minY, middleY, minZ, middleZ, childPolygons[2]));
	node->children.push_back(buildOctree(middleX + 1, maxX, minY, middleY, middleZ, maxZ, childPolygons[3]));
	node->children.push_back(buildOctree(minX, middleX, middleY + 1, maxY, minZ, middleZ, childPolygons[4]));
	node->children.push_back(buildOctree(minX, middleX, middleY + 1, maxY, middleZ, maxZ, childPolygons[5]));
	node->children.push_back(buildOctree(middleX + 1, maxX, middleY + 1, maxY, minZ, middleZ, childPolygons[6]));
	node->children.push_back(buildOctree(middleX + 1, maxX, middleY + 1, maxY, middleZ, maxZ, childPolygons[7]));
	for (OctoNode* child : node->children)
		node->z = std::fmax(node->z, child->z);

	return node;
}


QuadNode* OctreeHieraZBuffer::buildZPyramid(int minX, int maxX, int minY, int maxY)
{

	if (minX > maxX || minY > maxY)
	{
		return NULL;
	}

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

	//if (middleX + 1 <= maxX)
	//{
		child = buildZPyramid(middleX + 1, maxX, minY, middleY);
		if (child != NULL)child->father = node;
		node->children.push_back(child);
	//}
	//if (middleY + 1 <= maxY)
	//{
		child = buildZPyramid(minX, middleX, middleY + 1, maxY);
		if (child != NULL)child->father = node;
		node->children.push_back(child);
	//}
	//if (middleX + 1 <= maxX && middleY + 1 <= maxY)
	//{
		child = buildZPyramid(middleX + 1, maxX, middleY + 1, maxY);
		if (child != NULL)child->father = node;
		node->children.push_back(child);
	//}
	return node;
}



bool OctreeHieraZBuffer::rejectOctoNode(OctoNode* oNode, QuadNode* qNode)
{
	if (oNode->z <= qNode->z)
		return true;
	else return false;
}


void OctreeHieraZBuffer::renderOctree()
{
	Timer timer;
	timer.start();
	renderOctreeNode(scene, zPyramid);
	timer.end();
	timer.printTimeCost("Octree Hierarchical ZBuffer");
}

void OctreeHieraZBuffer::renderOctreeNode(OctoNode* oNode, QuadNode* qNode)
{
	if (rejectOctoNode(oNode, qNode))
	{
		return;
	}
	else
	{
		// 绘制当前网格内的三角形
		for (int i = 0; i < oNode->polygons.getSize(); i++)
		{
			renderPyramidRange(oNode->polygons.fragments[i], qNode);
		}
		int oChild = oNode->children.size();
		int qChild = qNode->children.size();
		if (oChild > 0 && qChild > 0)
		{
			for (int i = 0; i < oChild; i++)
			{
				renderOctreeNode(oNode->children[i], qNode->children[i / 2]);
			}
		}
	}
}

bool OctreeHieraZBuffer::rejectPolygon(Fragment& fragment, QuadNode* qNode)
{
	if (fragment.getMaxZ() <= qNode->z)
		return true;
	else return false;
}


bool OctreeHieraZBuffer::containPolygon(Fragment& fragment, QuadNode* qNode)
{
	if (fragment.getMinX() >= qNode->minX && fragment.getMaxX() <= qNode->maxX
		&& fragment.getMinY() >= qNode->minY && fragment.getMaxY() <= qNode->maxY)
		return true;
	else return false;
}



void OctreeHieraZBuffer::renderPyramidRange(Fragment& fragment, QuadNode* qNode)
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
			if (qNode->children[idx] == NULL)continue;
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

float OctreeHieraZBuffer::renderNodeRange(Fragment& fragment, QuadNode* qNode)
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
			if (child != NULL)
			{
				float tmpZ = renderNodeRange(fragment, child);
				minZ = std::fmin(minZ, tmpZ);
			}
		}
		qNode->z = minZ;
		return qNode->z;
	}
}


void OctreeHieraZBuffer::release()
{
	if (frameBuffer != NULL)
		delete[] frameBuffer;
	zPyramid->release();
	if (zBuffer != NULL)
		delete[] zBuffer;
}

