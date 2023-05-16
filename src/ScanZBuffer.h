// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once

#include<vector>
#include"Model.h"


// 分类多边形
struct ClassifiedPolygon
{
	float a, b, c, d; // 多边形所在平面的方程系数
	int id; // 多边形的编号
	int dy; // 多边形跨越的扫描线数目
	Color3f color; // 多边形的颜色
};

// 分类边
struct ClassifiedEdge
{
	float x; // 边上端点的x坐标
	float dx; // 相邻两条扫描线交点的x坐标差
	int dy; // 边跨越的扫描线数目
	int id; // 边所属多边形的编号
};

// 活化边
struct ActiveEdge
{
	float xl, xr; //左右交点的x坐标
	float dxl, dxr; // (左右交点边上)两相邻扫描线交点的x坐标之差
	float dyl, dyr; // 以和左右交点所在边相交的扫描线数为初值，向下每处理一条扫描线
	float zl; // 左右交点处多边形所在平面的深度值；
	float dzx; // 沿扫描线向右走过一个像素时，多边形所在平面的深度增量。
	float dzy; // 沿y方向向下移过一根扫描线时，多边形所在平面的深度增量。
	int id; // 交点对所在的多边形的编号
	Color3f color; // 多边形的颜色
};


// 扫描线ZBuffer算法
// 因为这个算法是根据一开始建立好的表计算的，所以不会用到Fragment类
class ScanLineZBuffer
{
public:
	ScanLineZBuffer() = delete;
	ScanLineZBuffer(int w, int h);
	~ScanLineZBuffer();
	void initialize();
	bool buildTable(const Model& model); // 建立分类多边形表和分类边表
	void scan(); // 根据建立的表依次进行扫描线的计算
	int** idBuffer;
	
	Color3f* getFrameBuffer()
	{
		return frameBuffer;
	}

private:
	int width;
	int height;
	float* zBuffer;
	Color3f* frameBuffer;
	std::vector<ClassifiedEdge> relatedEdge; // 记录分类边表中和当前活化边或者活化多边形id相同的
	std::vector<std::vector<ClassifiedPolygon> > classifiedPolygonTable;
	std::vector<std::vector<ClassifiedEdge> > classifiedEdgeTable;
	std::vector<ClassifiedPolygon> activePolygonTable;
	std::vector<ActiveEdge> activeEdgeTable;	
	
	void addActiveEdgeFromActivePolygon(int y, ClassifiedPolygon& activePolygon); // 对当前的活化多边形，计算需要加入的活化边
	bool findReplaceEdgeFromActivePolygon(int y, ActiveEdge& activeEdge); // 活化边中某一段的dy降到0时寻找一个替代边，并更新活化边
	void release();
};



