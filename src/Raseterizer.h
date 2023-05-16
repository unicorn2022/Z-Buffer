// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once

#include "Model.h"

// 光栅化
class Rasterizer
{
public:
	Rasterizer(int w, int h) { width = w; height = h; }
	inline void setSize(int w, int h) { width = w; height = h; }
	void rasterize(Model& model); // 对单个Model进行光栅化，计算结果直接储存在原来的Model类中
private:
	int width, height;
};