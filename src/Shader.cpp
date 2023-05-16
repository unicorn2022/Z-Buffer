// Author: Peiyao Li
// Date:   Jan 8 2023
#include "Shader.h"
#include <cstdlib>
#define random(x) rand()%(x)

void Shader::shaderModel(Model& model, ShaderPattern shaderpattern)
{
	int faceSize = model.faces.size();
	int vertexSizePerFace = model.faces[0].vIdx.size();
	for (int i = 0; i < faceSize; i++)
	{
		Face& tmpFace = model.faces[i];
		tmpFace.faceColor = Color3f(0, 0, 0);

		if (shaderpattern == WHITE)
		{
			tmpFace.faceColor = Color3f(1, 1, 1);
		}
		else if (shaderpattern == RANDOM_COLOR)
		{		
			tmpFace.faceColor.r = float(random(100)) / 100.0;
			tmpFace.faceColor.g = float(random(100)) / 100.0;
			tmpFace.faceColor.b = float(random(100)) / 100.0;
		}
		else if (shaderpattern == LIGHT_DIFFUSE)
		{
			for (int j = 0; j < vertexSizePerFace; j++)
			{
				Vertex& tmpVertex = model.vertices[tmpFace.vIdx[j]];
				Vec3f lightDir = normalize(lightPos - tmpVertex.p);
				Vec3f normal = (tmpFace.vnIdx.size() > 0&& tmpFace.vnIdx[j] >= 0) ? model.normals[tmpFace.vnIdx[j]] : tmpFace.faceNormal;
				float cosine = dot(lightDir, normal);
				if (cosine > 0.0) tmpFace.faceColor += kd * cosine * lightCol;
				tmpFace.faceColor += ambientCol;
			}
			tmpFace.faceColor /= vertexSizePerFace;
		}
		
		tmpFace.faceColor.clamp(0.0, 1.0);
		model.faces[i].faceColor = tmpFace.faceColor;
	}
}