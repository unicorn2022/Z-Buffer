// Author: Peiyao Li
// Date:   Jan 8 2023

#include <iostream>
#include <fstream>
#include "Model.h"
#include "Shader.h"
#include "Raseterizer.h"
#include "ScanZBuffer.h"
#include "NaiveZBuffer.h"
#include "HieraZBuffer.h"
#include "OctreeHieraZBuffer.h"


void writePic(std::string const& filename, int width, int height, Color3f*framedBuffer)
{
    std::ofstream f(filename, std::ios_base::out | std::ios_base::binary);
    char ppm_head[50] = { 0 };
    sprintf_s(ppm_head, "P6\n%d %d\n255\n", width, height);
    f << ppm_head;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int loc = (height - 1 - y) * width + x;
            Color3f col = framedBuffer[loc];
            f << (char)(255.0 * col.r) << (char)(255.0 * col.g) << (char)(255.0 * col.b);
        }
    }
    f.close();
}


int main(int argc, char** argv)
{
	int width = 800;
	int height = 600;
    std::cout << "Screen resolution:" << width << "*" << height << std::endl;
    // obj path
    std::vector<std::string>  objpath;

    for (int i = 1; i < argc; ++i) {
        objpath.push_back(argv[i]);
    }
    if (objpath.size() == 0) {
        std::cout << "Please input the path of mesh..." << std::endl;
        return 0;
    }

	//std::string path = "./Model/torus1k.obj";
	Model model(objpath[0]);
    Rasterizer rasterizer(width, height);
	rasterizer.rasterize(model);
    Shader shader;
    shader.shaderModel(model, RANDOM_COLOR);
    Polygons polygons;
    polygons.addModel(model);    
    
    for (int i = 1; i < objpath.size(); i++)
    {
        Model tmpModel(objpath[i]);
        rasterizer.rasterize(tmpModel);
        shader.shaderModel(tmpModel, RANDOM_COLOR);
        polygons.addModel(tmpModel);
    }
    std::cout << "Polygons Size:" << polygons.getSize() << std::endl;

	ScanLineZBuffer slzBuffer(width, height);
	slzBuffer.buildTable(model);
	slzBuffer.scan();
    writePic("result_scanline.ppm", width, height, slzBuffer.getFrameBuffer());
    
    NaiveZBuffer zBuffer(width, height);
    zBuffer.renderPolygons(polygons);
    writePic("result_naive.ppm", width, height, zBuffer.getFrameBuffer());

    HieraZBuffer hzBuffer(width, height);
    hzBuffer.renderPolygons(polygons);
    writePic("result_hiera.ppm", width, height, hzBuffer.getFrameBuffer());

    OctreeHieraZBuffer ohzBuffer(width, height);
    ohzBuffer.buildScene(polygons);
    ohzBuffer.renderOctree();
    writePic("result_octree_hiera.ppm", width, height, ohzBuffer.getFrameBuffer());
    
	system("pause");
	return 0;
}