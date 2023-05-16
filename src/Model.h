// Author: Peiyao Li
// Date:   Jan 8 2023

#pragma once
#include<string>
#include<vector>
#include"Vector.h"

enum ModelPattern { 
	UNKNOWN = 0,
	ONLY_VERTEX = 1,  
	VERTEX_AND_TEXCOORD = 2, 
	ALL_INFO = 3 
};

class Vertex
{
public:
	Point3f p;
	Vec3f n;
};

class Face
{
public:
	std::vector<int> vIdx;
	std::vector<int> vnIdx;
	std::vector<int> vtIdx;
	Vec3f faceNormal;
	Color3f faceColor;
};


class Model
{
public:
	ModelPattern pattern = UNKNOWN;
	int istriangle = true;
	std::vector<Vertex> vertices;
	std::vector<Vec3f> normals;
	std::vector<Vec2f> texcoords;
	std::vector<Face> faces;
	Point3f center;

	Model() = delete;
	Model(const std::string& path);

	Model(Model const&) = delete;
	Model(Model &&) = delete;
	Model& operator=(Model const&) = delete;
	Model& operator=(Model &&) = delete;

	bool load(const std::string& path);
	bool detectPattern(std::string line);
};