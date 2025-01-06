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

std::string root_directory = "E:/MyProject/Learn/CG/Z-Buffer/";

void writePic(std::string const& filename, int width, int height, Color3f*framedBuffer) {
    std::string path = root_directory + filename;
    std::ofstream f(path, std::ios_base::out | std::ios_base::binary);
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


int main() {
	int width = 800;
	int height = 600;
    std::cout << "Screen resolution:" << width << "*" << height << std::endl;

	std::string path = root_directory + "Model/teapot15k.obj";

    /* 加载模型 */
	Model model(path);
    /* 将模型坐标修改为屏幕空间 */
    Rasterizer rasterizer(width, height);
	rasterizer.rasterize(model);
    /* 计算面片颜色 */
    Shader shader;
    shader.shaderModel(model, LIGHT_DIFFUSE);
    /* 将模型的所有面片添加到全局面片管理中 */
    Polygons polygons;
    polygons.addModel(model);    
    
    std::cout << "Polygons Size:" << polygons.getSize() << std::endl;

	// ScanLineZBuffer slzBuffer(width, height);
	// slzBuffer.buildTable(model);
	// slzBuffer.scan();
    // writePic("result_scanline.ppm", width, height, slzBuffer.getFrameBuffer());
    
    NaiveZBuffer zBuffer(width, height);
    zBuffer.renderPolygons(polygons);
    writePic("result_naive.ppm", width, height, zBuffer.getFrameBuffer());

    // HieraZBuffer hzBuffer(width, height);
    // hzBuffer.renderPolygons(polygons);
    // writePic("result_hiera.ppm", width, height, hzBuffer.getFrameBuffer());

    // OctreeHieraZBuffer ohzBuffer(width, height);
    // ohzBuffer.buildScene(polygons);
    // ohzBuffer.renderOctree();
    // writePic("result_octree_hiera.ppm", width, height, ohzBuffer.getFrameBuffer());
    
	// system("pause");
	return 0;
}