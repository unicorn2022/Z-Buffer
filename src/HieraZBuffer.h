// Author: Peiyao Li
// Date:   Jan 8 2023

#pragma once
#include "Model.h"
#include "Fragment.h"
#include "Quadtree.h"
#include <cmath>

// 实现简单模式的层次ZBuffer
class HieraZBuffer
{
public:
	HieraZBuffer() = delete;
	HieraZBuffer(int w, int h);
	~HieraZBuffer();

	void initialize(); // 初始化
	QuadNode* buildZPyramid(int minX, int maxX, int minY, int maxY); // 分配储存空间并使用四叉树节点递归建立层次Zbuffer结构

	bool rejectPolygon(Fragment& fragment, QuadNode* qNode); // 深度测试
	bool containPolygon(Fragment& fragment, QuadNode* qNode); // 当前层次ZBuffer块范围是否包含整个面片（不做面片切割）
	
	void renderPolygons(Polygons& polygons); // 计算场景中所有面片
	void renderPyramidRange(Fragment& fragment, QuadNode* qNode);  // 从顶层开始分别深度测试，找到包含面片且不能拒绝的最小ZBuffer并在此层开始向下绘制
	float renderNodeRange(Fragment& fragment, QuadNode* qNode); // 在当前层次ZBuffer节点递归找到最底层，并渲染面片

	Color3f* getFrameBuffer() { return frameBuffer; }
	void release();

private:
	int width, height;
	Color3f* frameBuffer;

	QuadNode* zPyramid;
	QuadNode* zBuffer;

};