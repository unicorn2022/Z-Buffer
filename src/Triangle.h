#pragma once
// Author: Peiyao Li
// Date:   Jan 8 2023, 12:53

#include "Vector.h"
#include <vector>

class Triangle
{
public:
	Vec3f pos[3];
	Vec3f nrm[3];
	Vec2f tex[3];
	int modelID;
	int faceID;
private:
};

class RasterizedModel
{
public:
	std::vector<Triangle> triangles;
};