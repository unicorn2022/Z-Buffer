#include "Fragment.h"

Fragment::Fragment(Model& model, int faceid)
{
	Face& face = model.faces[faceid];
	int vertexSizePerFace = face.vIdx.size();
	for (int j = 0; j < vertexSizePerFace; j++)
		addPoint(model.vertices[face.vIdx[j]].p);
	faceColor = face.faceColor;
}

void Fragment::addPoint(Point3f p)
{
	vPos.push_back(p);
	minX = std::min(minX, int(floor(p.x)));
	maxX = std::max(maxX, int(floor(p.x)));
	minY = std::min(minY, int(floor(p.y)));
	maxY = std::max(maxY, int(floor(p.y)));
	minZ = std::fmin(minZ, p.z);
	maxZ = std::fmax(maxZ, p.z);
}

float Fragment::caculateZ(Vec2f pixel) {
	if (vPos.size() == 3) {
		return caculateZTriangle(pixel, 0, 1, 2);
	}
	else {
		// 四边形怎么重心插值 //或许可以用遍历三个三角形的方法 
		float z1 = caculateZTriangle(pixel, 0, 1, 2);
		float z2 = caculateZTriangle(pixel, 1, 2, 3);
		float z3 = caculateZTriangle(pixel, 0, 1, 3);

		return std::fmax(z1, std::fmax(z2, z3));
	}
}

float Fragment::caculateZTriangle(Vec2f pixel, int aIdx, int bIdx, int cIdx) {
	float z = -FLT_MAX;
	Point3f A = vPos[aIdx];
	Point3f B = vPos[bIdx];
	Point3f C = vPos[cIdx];
	float alpha = (-(pixel.x - B.x) * (C.y - B.y) + (pixel.y - B.y) * (C.x - B.x)) / (-(A.x - B.x) * (C.y - B.y) + (A.y - B.y) * (C.x - B.x));
	float beta = (-(pixel.x - C.x) * (A.y - C.y) + (pixel.y - C.y) * (A.x - C.x)) / (-(B.x - C.x) * (A.y - C.y) + (B.y - C.y) * (A.x - C.x));
	float gamma = 1 - alpha - beta;
	if (alpha >= 0 && beta >= 0 && gamma >= 0) {
		z = A.z * alpha + B.z * beta + C.z * gamma;
	}
	return z;
}


void Polygons::addModel(Model& model)
{
	for (int i = 0; i < model.faces.size(); i++)
	{
		Fragment fragment = Fragment(model, i);
		minZ = std::fmin(minZ, fragment.getMinZ());
		maxZ = std::fmax(maxZ, fragment.getMaxZ());
		fragment.id = i;
		fragments.push_back(fragment);
	}

}