// Author: Peiyao Li
// Date:   Jan 8 2023
#include "NaiveZBuffer.h"
#include "Timer.h"
NaiveZBuffer::NaiveZBuffer(int w,int h)
{
	width = w;
	height = h;

	initialize();
}
NaiveZBuffer::~NaiveZBuffer()
{
	release();
}

void NaiveZBuffer::initialize()
{
	frameBuffer = new Color3f[width * height];
	for (int i = 0; i < width * height; i++)
	{
		frameBuffer[i] = Color3f(0, 0, 0);
	}
	zBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++)
	{
		zBuffer[i] = -FLT_MAX;
	}
}

void NaiveZBuffer::release()
{
	delete[] frameBuffer;
	delete[] zBuffer;
}

void NaiveZBuffer::renderPolygons(Polygons& polygons)
{
	Timer timer;
	timer.start();
	int polygonSize = polygons.getSize();
	for (int i = 0; i < polygonSize; i++)
	{
		renderPolygon(polygons.fragments[i]);
	}
	timer.end();
	timer.printTimeCost("Naive ZBuffer");
}

void NaiveZBuffer::renderPolygon(Fragment& fragment)
{
	for (int y = fragment.getMinY(); y <= fragment.getMaxY(); y++)
	{
		for (int x = fragment.getMinX(); x <= fragment.getMaxX(); x++)
		{
			int loc = y * width + x;
			float z = fragment.caculateZ(Vec2f(x+0.5, y+0.5));
			if (!isEqualf(z, -FLT_MAX) && z > zBuffer[loc])
			{
				zBuffer[loc] = z;
				frameBuffer[loc] = fragment.faceColor;
			}
		}
	}
}