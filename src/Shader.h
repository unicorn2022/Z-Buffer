// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once
#include "Vector.h"
#include "Model.h"
const float kd = 0.8;

// 着色模式：全白色、面片随机颜色和点光源Diffuse三种
enum ShaderPattern {
	WHITE = 0,
	RANDOM_COLOR = 1,
	LIGHT_DIFFUSE = 2
};

// 需要说明的时，为了展示出不同面片的边界，着色时每个面片都是同样的颜色，而不是像平滑模式那样根据点的颜色插值
class Shader
{
public:
	Point3f lightPos = Point3f(400.0f, 300.0f, 500.0f);
	Color3f lightCol = Color3f(0.6, 0.6, 0.6);
	Color3f ambientCol = Color3f(0.3, 0.3, 0.3);

	void shaderModel(Model& model, ShaderPattern shaderpattern);
};