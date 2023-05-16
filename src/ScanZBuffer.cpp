// Author: Peiyao Li
// Date:   Jan 8 2023
#include "ScanZBuffer.h"
#include <omp.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <assert.h>
#include "Timer.h"
//#define NUM_THREADS 8


ScanLineZBuffer::ScanLineZBuffer(int w, int h)
{
	width = w;
	height = h;

	initialize();
}

ScanLineZBuffer::~ScanLineZBuffer()
{
	release();
}

void ScanLineZBuffer::initialize()
{
	release();

	zBuffer = new float[width];
	frameBuffer = new Color3f[width * height];
	for (int i = 0; i < width * height; i++)
	{
		frameBuffer[i] = Color3f(0, 0, 0);
	}
	idBuffer = new int* [height];
	for (int i = 0; i < height; ++i)
		idBuffer[i] = new int[width];

}

void ScanLineZBuffer::release()
{
	if (zBuffer != NULL)
	{
		delete[] zBuffer;
		zBuffer = NULL;
	}
	if (idBuffer != NULL)
	{
		for (int i = 0; i < height; ++i)
		{
			delete[] idBuffer[i];
			idBuffer[i] = NULL;
		}
	}
	delete[] idBuffer;
	idBuffer = NULL;
}

// 对提取出的当前多边形中的分类边的比较函数，递增
static bool classifiedEdgeSortCmp(const ClassifiedEdge& lEdge, const ClassifiedEdge& rEdge)
{
	if (lEdge.x < rEdge.x)return true;
	else if (lEdge.x == rEdge.x)
	{
		if (lEdge.dx < rEdge.dx)
			return true;
	}
	return false;
}

bool ScanLineZBuffer::buildTable(const Model& model)
{
	Timer timer;
	timer.start();

	classifiedPolygonTable.clear();
	classifiedPolygonTable.resize(height);
	classifiedEdgeTable.clear();
	classifiedEdgeTable.resize(height);

	omp_lock_t mylock;
	omp_init_lock(&mylock);

	int faces_size = model.faces.size();
	for (int i = 0; i < faces_size; i++)
	{
		const Face& face = model.faces[i];

		float max_y = -0xfffffff, min_y = 0xfffffff;
		//构建分类边表
		const std::vector<int>& vertexIdx = model.faces[i].vIdx;
		for (int j = 0, vsize = vertexIdx.size(); j < vsize; j++)
		{
			Point3f p1 = model.vertices[vertexIdx[j]].p;
			Point3f p2 = model.vertices[vertexIdx[(j + 1) % vsize]].p;
			
			if (p1.y < p2.y)
			{
				Point3f tmp = p1;
				p1 = p2;
				p2 = tmp;
			}

			max_y = std::max(max_y, p1.y);
			min_y = std::min(min_y, p2.y);

			// problem:
			// 试试在一开始就用round舍弃浮点会不会好一点
			// 改用floor和ceil
			ClassifiedEdge edge;
			edge.x = floor(p1.x);
			edge.id = i;
			edge.dy = floor(p1.y) - floor(p2.y);
			if (isEqualf(edge.dy,0)) continue; //在图像空间平行于x轴

			//edge.dx = -(floor(p1.x) - floor(p2.x)) / (floor(p1.y) - floor(p2.y));
			edge.dx = -(floor(p1.x) - floor(p2.x)) / edge.dy;

			// problem: 上面一条边几乎平行于x轴
			//if (edge.dx > 10)
			//{
			//	std::cout << "error" << std::endl;
			//	std::cout << "test" << edge.dx << std::endl;
			//}

			omp_set_lock(&mylock);
			classifiedEdgeTable[floor(p1.y)].push_back(edge);
			omp_unset_lock(&mylock);
		}


		ClassifiedPolygon polygon;
		polygon.id = i;
		polygon.dy = floor(max_y) - floor(min_y);
		Point3f pos = model.vertices[face.vIdx[0]].p;
		polygon.a = face.faceNormal.x;
		polygon.b = face.faceNormal.y;
		polygon.c = face.faceNormal.z;
		polygon.d = -(polygon.a * pos.x + polygon.b * pos.y + polygon.c * pos.z);
		polygon.color = face.faceColor;

		omp_set_lock(&mylock);
		classifiedPolygonTable[floor(max_y)].push_back(polygon);
		omp_unset_lock(&mylock);
	}

	timer.end();
	timer.printTimeCost("ScanLine build");
	return true;
}

void ScanLineZBuffer::addActiveEdgeFromActivePolygon(int y, ClassifiedPolygon& activePolygon)
{
	if (isEqualf(activePolygon.c, 0))return;

	relatedEdge.clear();
	// 把该多边形在oxy平面上的投影和扫描线相交的边加入到活化边表中
	for (std::vector<ClassifiedEdge>::iterator it = classifiedEdgeTable[y].begin(),
		end = classifiedEdgeTable[y].end(); it != end; it++)
	{
		if (it->id != activePolygon.id) continue;

		relatedEdge.push_back(*it);
		// classifiedEdgeTable[y].erase(it); // problem：这样删去会导致it迭代器出错，无法遍历全部变量
		it->id = -1;
	}
	// 对当前活化多边形的活化边表按照x递增排序(x相等时，使用dx)
	if (relatedEdge.size() % 2 != 0)
	{
		std::cout << "Error to find even number of edges intersecting the current scan line" << std::endl;
		assert(relatedEdge.size() % 2 == 0);
	}
	// problem: 有一条边平行于x轴 且这条边在第一次就被检测出来
	if (relatedEdge.size() == 3)
	{
		std::cout << "There is an edge parallel to the x-axis." << std::endl;
	}
	sort(relatedEdge.begin(), relatedEdge.end(), classifiedEdgeSortCmp);

	ActiveEdge activeEdge;
	activeEdge.id = activePolygon.id;
	activeEdge.dzx = -(activePolygon.a / activePolygon.c);
	activeEdge.dzy = activePolygon.b / activePolygon.c;
	activeEdge.color = activePolygon.color;
	for (std::vector<ClassifiedEdge>::iterator it = relatedEdge.begin(),
		end = relatedEdge.end(); it != end; it++)
	{
		activeEdge.xl = it->x;
		activeEdge.dxl = it->dx;
		activeEdge.dyl = it->dy;
		activeEdge.zl = -(activePolygon.d + activePolygon.a * it->x + activePolygon.b * y) / activePolygon.c;
		it++;
		activeEdge.xr = it->x;
		activeEdge.dxr = it->dx;
		activeEdge.dyr = it->dy;

		activeEdgeTable.push_back(activeEdge);
	}
}

bool ScanLineZBuffer::findReplaceEdgeFromActivePolygon(int y, ActiveEdge& activeEdge)
{
	std::vector<ClassifiedEdge> relatedEdge;

	for (std::vector<ClassifiedEdge>::iterator it = classifiedEdgeTable[y].begin(),
		end = classifiedEdgeTable[y].end(); it != end; it++)
	{
		if (it->id == activeEdge.id)
		{
			relatedEdge.push_back(*it);
		}
	}
	if (relatedEdge.size() == 0)
	{
		return false; // 该活化多边形中没有该删除该活化边
	}
	else
	{
		if (activeEdge.dyl == 0 && activeEdge.dyr == 0)
		{
			// 最后一条边平行于x轴，不需要操作，因为下一轮迭代完会自动结束
			// 需要操作。
			if (relatedEdge.size() == 1) {
				activeEdge.dxl = activeEdge.dxr = 0;
				activeEdge.dyl = activeEdge.dyr = 1;
			}
			else if (relatedEdge.size() == 2) // 出现在四边形中，两条边同时取代现在的两条边
			{
				sort(relatedEdge.begin(), relatedEdge.end(), classifiedEdgeSortCmp);
				for (std::vector<ClassifiedEdge>::iterator it = relatedEdge.begin(),
					end = relatedEdge.end(); it != end; it++)
				{
					activeEdge.xl = it->x;
					activeEdge.dxl = it->dx;
					activeEdge.dyl = it->dy;

					it++;
					activeEdge.xr = it->x;
					activeEdge.dxr = it->dx;
					activeEdge.dyr = it->dy;
				}
			}
		}
		else if (activeEdge.dyl == 0)
		{
			activeEdge.xl = relatedEdge[0].x;
			activeEdge.dxl = relatedEdge[0].dx;
			activeEdge.dyl = relatedEdge[0].dy;
		}
		else
		{
			activeEdge.xr = relatedEdge[0].x;
			activeEdge.dxr = relatedEdge[0].dx;
			activeEdge.dyr = relatedEdge[0].dy;
		}
	}


}

void ScanLineZBuffer::scan()
{
	Timer timer;
	timer.start();

	for (int y = height - 1; y >= 0; y--)
	{
		memset(idBuffer[y], -1, sizeof(int) * width);
		std::fill(zBuffer, zBuffer + width, -0xfffffff);

		for (std::vector<ClassifiedPolygon>::iterator it = classifiedPolygonTable[y].begin(),
			end = classifiedPolygonTable[y].end(); it != end; it++)
		{
			activePolygonTable.push_back(*it);
			addActiveEdgeFromActivePolygon(y, *it);
		}

		for (std::vector<ActiveEdge>::iterator it = activeEdgeTable.begin(),
			end = activeEdgeTable.end(); it != end; it++)
		{
			ActiveEdge tmpActiveEdge = *it;
			float zx = tmpActiveEdge.zl;
			for (int x = round(tmpActiveEdge.xl); x <= round(tmpActiveEdge.xr); x++)
			{
				if (zx > zBuffer[x])
				{
					zBuffer[x] = zx;
					idBuffer[y][x] = tmpActiveEdge.id;
					frameBuffer[y * width + x] = tmpActiveEdge.color;
				}
				zx += tmpActiveEdge.dzx;
			}
			
			if (tmpActiveEdge.dyl == 0 || tmpActiveEdge.dyr == 0)
			{
				bool flag = findReplaceEdgeFromActivePolygon(y, tmpActiveEdge);
			}
			tmpActiveEdge.dyl--; tmpActiveEdge.dyr--;
			
			tmpActiveEdge.xl += tmpActiveEdge.dxl;
			tmpActiveEdge.xr += tmpActiveEdge.dxr;
			tmpActiveEdge.zl += tmpActiveEdge.dzx * tmpActiveEdge.dxl + tmpActiveEdge.dzy;
			// problem: 开始这里写成dyl了
			
			*it = tmpActiveEdge; // 开始没加这一句导致计算错误
		}

		// 还是erase的问题
		//for (std::vector<ClassifiedPolygon>::iterator it = activePolygonTable.begin(),
		//	end = activePolygonTable.end(); it != end; it++)
		//{
		//	it->dy--;
		//	if (it->dy < 0)
		//		activePolygonTable.erase(it);
		//}

		// 没有更新ActivePolygon的dy会导致
		for (std::vector<ClassifiedPolygon>::iterator it = activePolygonTable.begin(),
			end = activePolygonTable.end(); it != end; it++)
		{
			it->dy--;
		}
		//for (std::vector<ActiveEdge>::iterator it = activeEdgeTable.begin(),
		//	end = activeEdgeTable.end(); it != end; it++)
		//{
		//	it->dyl--; it->dyr--;
		//}

		// 迭代dy 并移除 dy < 0 的活化多边形
		int last = 0;
		int actPolySize = activePolygonTable.size();
		for (int i = 0; i < actPolySize; ++i, ++last) {
			while (activePolygonTable[i].dy < 0) {
				++i;
				if (i >= actPolySize)break;
			}
			if (i >= actPolySize)break;
			activePolygonTable[last] = activePolygonTable[i];
		}
		activePolygonTable.resize(last);

		// 迭代dy 并移除 dy < 0 的活化边
		last = 0;
		int actEdgeSize = activeEdgeTable.size();
		for (int i = 0; i < actEdgeSize; ++i, ++last) {
			// problem: 边界问题，卡了很久
			// 如果用<0 会有一些情况整个三角形太小只占了一个元素
			// 如果用<=0 可能画不到最下面的边界
			// 思考后认为应该用<0 只占一个元素的情况应该初始dy=0
			while (activeEdgeTable[i].dyl < 0 || activeEdgeTable[i].dyr < 0){
				//||floor(activeEdgeTable[i].xl) > floor(activeEdgeTable[i].xr) ) {
				++i;
				if (i >= actEdgeSize)break;
			}
			if (i >= actEdgeSize)break;
			activeEdgeTable[last] = activeEdgeTable[i];
		}
		activeEdgeTable.resize(last);
	}
	timer.end();
	timer.printTimeCost("ScanLine");
}

