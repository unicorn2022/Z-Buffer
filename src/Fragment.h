// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once

#include "Model.h"
#include <vector>
#include <cmath>
#include <cfloat>
#include <cinttypes>
class Fragment
{
// 管理光栅化之后的面片（支持三角形面片和四边形面片）
// 可以通过重心坐标插值的方式计算当前像素的Z值
// 对于四边形面片，处理方法是采样三次，每次采样三个点组成一个三角形计算，并选取重心坐标符合要求的结果
public:
	Fragment(Model& model, int faceid);
	void addPoint(Point3f p);
	std::vector<Point3f> vPos;
	Color3f faceColor;
	int id;
	float caculateZ(Vec2f pixel);
	float caculateZTriangle(Vec2f pixel, int aIdx, int bIdx, int cIdx);	

	int getMinX() { return minX; }
	int getMaxX() { return maxX; }
	int getMinY() { return minY; }
	int getMaxY() { return maxY; }
	float getMinZ() { return minZ; }
	float getMaxZ() { return maxZ; }
	int getDeltaX() { return maxX - minX; }
	int getDeltaY() { return maxY - minY; }
	float getDeltaZ() { return maxZ - minZ; }
private:
	int minX = INT_MAX, maxX = INT_MIN;
	int minY = INT_MAX, maxY = INT_MIN;
	float minZ = FLT_MAX, maxZ = -FLT_MAX;
};


class Polygons
{
// 包含一个vector，管理场景中所有的三角形面片Fragment
public:
	std::vector<Fragment> fragments;
	float minZ = FLT_MAX, maxZ = -FLT_MAX;

	void addModel(Model& model);
	int getSize() { return fragments.size(); }
};