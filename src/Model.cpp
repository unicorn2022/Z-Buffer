// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once
#include<sstream>
#include<iostream>
#include<fstream>
#include<ctime>
#include<string>
#include<regex>
#include "Model.h"


Model::Model(const std::string& path)
{
	if(this->load(path))
	{
		std::cout << "Successfully load the Model: " << path << std::endl;
		std::cout << "vertices: " << vertices.size() << " faces: " << faces.size() << std::endl;
	}
	else
	{
		std::cout << "Failed to load the Model: " << path << std::endl;
	}
}


bool Model::load(const std::string& path)
{
	clock_t t = clock();
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "Cannot open the file: " << path << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line))
	{
		//std::cout << line << std::endl;
		std::istringstream sline(line);
		std::string type;
		sline >> type;
		//std::cout << type << std::endl;

		if (type == "v")
		{
			Vertex v;
			sline >> v.p.x >> v.p.y >> v.p.z;
			vertices.push_back(v);
		}
		else if (type == "vn")
		{
			Vec3f vn;
			sline >> vn.x >> vn.y >> vn.z;
			normals.push_back(vn);
		}
		else if (type == "vt")
		{
			Vec2f vt;
			sline >> vt.x >> vt.y;
			texcoords.push_back(vt);
		}
		else if (type == "f")
		{	
			if (pattern == UNKNOWN)
				detectPattern(line);
			Face face;
			char slash;
			int vidx, vtidx, vnidx;
			if (pattern == ALL_INFO) {
				int times = 3; if (!istriangle)times = 4;
				for (int i = 0; i < times; i++) {
					sline >> vidx >> slash >> vtidx >> slash >> vnidx;
					face.vIdx.push_back(vidx - 1);
					face.vtIdx.push_back(vtidx - 1);
					face.vnIdx.push_back(vnidx - 1);
				}
			}	
			else if (pattern == VERTEX_AND_TEXCOORD) {
				int times = 3; if (!istriangle)times = 4;
				for (int i = 0; i < times; i++) {
					sline >> vidx >> slash >> vtidx;
					face.vIdx.push_back(vidx - 1);
					face.vtIdx.push_back(vtidx - 1);
				}

			}			
			else if (pattern == ONLY_VERTEX) {
				int times = 3; if (!istriangle)times = 4;
				for (int i = 0; i < times; i++) {
					sline >> vidx;
					face.vIdx.push_back(vidx - 1);
				}
			}

			// 通过叉乘计算面法向量，在obj文件不提供顶点法向量时使用
			if (face.vIdx.size() > 2)
			{
				Point3f& a = vertices[face.vIdx[0]].p;
				Point3f& b = vertices[face.vIdx[1]].p;
				Point3f& c = vertices[face.vIdx[2]].p;
				face.faceNormal = normalize(cross(b - a, c - b));
			}
			faces.push_back(face);
		}
	}
}


bool Model::detectPattern(std::string line)
{

	if (std::regex_match(line, std::regex("f(\\s\\d+/\\d+/\\d+)+(\\s)*")))
	{
		pattern = ALL_INFO;
		if (std::regex_match(line, std::regex("f(\\s\\d+/\\d+/\\d+){4}(\\s)*")))
			istriangle = false;		
	}	
	else if (std::regex_match(line, std::regex("f(\\s\\d+//\\d+//\\d+)+(\\s)*")))
	{
		pattern = ALL_INFO;
		if (std::regex_match(line, std::regex("f(\\s\\d+//\\d+//\\d+){4}(\\s)*")))
			istriangle = false;
	}
	else if (std::regex_match(line, std::regex("f(\\s\\d+/\\d+)+(\\s)*")))
	{
		pattern = VERTEX_AND_TEXCOORD;
		if (std::regex_match(line, std::regex("f(\\s\\d+/\\d+){4}(\\s)*")))
			istriangle = false;
	}
	else if (std::regex_match(line, std::regex("f(\\s\\d+//\\d+)+(\\s)*")))
	{
		pattern = VERTEX_AND_TEXCOORD;
		if (std::regex_match(line, std::regex("f(\\s\\d+//\\d+){4}(\\s)*")))
			istriangle = false;
	}
	else if (std::regex_match(line, std::regex("f(\\s\\d+)+(\\s)*")))
	{
		pattern = ONLY_VERTEX;
		if (std::regex_match(line, std::regex("f(\\s\\d+){4}(\\s)*")))
			istriangle = false;
	}
	else
	{
		std::cout << "Unknown mesh face format." << std::endl;
		return false;
	}
	return true;
}