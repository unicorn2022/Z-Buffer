// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once

#include "Fragment.h"

// 最基础的ZBuffer
class NaiveZBuffer
{
public:
	NaiveZBuffer() = delete;
	NaiveZBuffer(int w, int h);
	~NaiveZBuffer();
	void initialize(); // 初始化及分配空间
	void renderPolygons(Polygons& fragments); // 遍历绘制所有面片
	void renderPolygon(Fragment& fragment); // 深度测试并绘制单个面片
	Color3f* getFrameBuffer() { return frameBuffer; }
	void release();
private:
	int width, height;
	Color3f* frameBuffer;
	float* zBuffer;
};