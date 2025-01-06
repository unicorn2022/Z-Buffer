// Author: Peiyao Li
// Date:   Jan 8 2023
#include "Raseterizer.h"
#include "Vector.h"
#include <iostream>


// 为了方便不同尺度不同空间坐标的Mesh都能完整地显示在屏幕中
// 此处的光栅化其实是根据模型的坐标值范围和窗口的大小进行了一个scale和transform
// 光栅化后Model中顶点的坐标变换为在屏幕窗口上的二维坐标值
void Rasterizer::rasterize(Model& model) {
	int vertices_size = model.vertices.size();

	/* 根据模型顶点位置, 计算 scale 值 */
	Point3f min_pos(0xfffffff, 0xfffffff, 0xfffffff), max_pos(-0xfffffff, -    0xfffffff, -0xfffffff);
	for (int i = 0; i < vertices_size; i++) {
		const Point3f& vertex = model.vertices[i].p;
		min_pos = min(min_pos, vertex);
		max_pos = max(max_pos, vertex);
	}
	Point3f center_pos = (min_pos + max_pos) / 2;
	float model_width = max_pos.x - min_pos.x;
	float model_height = max_pos.y - min_pos.y;
	float scale = (std::min(width, height) - 1) / std::max(model_width, model_height);
	scale *= 0.8;

	/* 修改模型顶点位置 */
	for (int i = 0; i < vertices_size; ++i) {
		Vec3f& vertex = model.vertices[i].p;
		//放置到窗口中间
		Vec3f rasterVertex;
		vertex.x = (vertex.x - center_pos.x) * scale + width / 2;
		vertex.y = (vertex.y - center_pos.y) * scale + height / 2;
		vertex.z = (vertex.z - center_pos.z) * scale;
	}
	model.center = Point3f(width / 2, height / 2, 0);
}