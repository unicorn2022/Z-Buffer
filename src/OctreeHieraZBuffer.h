// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once
#include <cmath>
#include <iostream>
#include "Model.h"
#include "Fragment.h"
#include "Octotree.h"
#include "Quadtree.h"

// 完整版本的层次ZBuffer
class OctreeHieraZBuffer
{
public:
	OctreeHieraZBuffer() = delete;
	OctreeHieraZBuffer(int w, int h);
	~OctreeHieraZBuffer();

	void initialize();

	void buildScene(Polygons& polygons);
	OctoNode* buildOctree(int minX, int maxX, int minY, int maxY, float minZ, float maxZ, Polygons& polygons);
	QuadNode* buildZPyramid(int minX, int maxX, int minY, int maxY);

	bool rejectOctoNode(OctoNode* oNode, QuadNode* qNode);
	bool rejectPolygon(Fragment& fragment, QuadNode* qNode);
	bool containPolygon(Fragment& fragment, QuadNode* qNode);

	void renderOctree();
	void renderOctreeNode(OctoNode* octoNode, QuadNode* qNode);
	
	void renderPyramidRange(Fragment& fragment, QuadNode* qNode);
	float renderNodeRange(Fragment& fragment, QuadNode* qNode);


	Color3f* getFrameBuffer() { return frameBuffer; }
	void release();

	int capacity = 2;
private:
	int width, height;

	Color3f* frameBuffer;

	QuadNode* zPyramid;
	QuadNode* zBuffer;

	OctoNode* scene;
};
