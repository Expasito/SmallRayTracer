#define _CRT_SECURE_NO_WARNINGS
/*
* 
* Goal: Make a simple ray tracer in as few lines of code as possible.
* 
*/
#include <stdlib.h>
#include <stdio.h>

const int width = 500;
const int height = 200;
const int hWidth = width / 2;
const int hHeight = height / 2;

struct vec3{
	float x,y,z;

	vec3 operator-(vec3 b) {
		return {x-b.x,y-b.y,z-b.z};
	}
};

float dot(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;

}

vec3 cross(vec3 a, vec3 b) {
	return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,a.x * b.y - a.y * b.x };
}

struct Triangle{
	// we have the point vector and its normal
	vec3 p;
	vec3 n;

	vec3 p1;
	vec3 p2;
	vec3 p3;

	Triangle(vec3 p1_, vec3 p2_, vec3 p3_) {
		p1 = p1_; p2 = p2_; p3 = p3_;
		// default to p1 here
		p = p1;
		n = cross(p1_ - p2_, p1_ - p3_);
	}

};

Triangle triangle{ {-1,-1,3},{0,1,3},{1,-1,3} };


// i is width, j is height
vec3 castRay(unsigned char* data, int i, int j) {
	// casting towards the z direction

	vec3 dir = { (i-hWidth)/(float)width,-(j-hHeight)/(float)height,1};
	float t = dot(triangle.p, triangle.n) / dot(dir, triangle.n);
	vec3 I = { t * dir.x,t * dir.y,t * dir.z };

	vec3 edge0 = triangle.p2-triangle.p1; vec3 edge1 = triangle.p3-triangle.p2; vec3 edge2 = triangle.p1-triangle.p3;

	int index = j * width * 3 + i * 3;
	if (dot(triangle.n, cross(edge0, { I - triangle.p1})) > 0.0 &&
		dot(triangle.n, cross(edge1, { I - triangle.p2 })) > 0.0 &&
		dot(triangle.n, cross(edge2, { I - triangle.p3 })) > 0.0
	) {
		data[index + 0] = 255;
		data[index + 1] = 0;
		data[index + 2] = 0;
		return {255,0,0};
	}
	else {
		data[index + 0] = 0;
		data[index + 1] = 0;
		data[index + 2] = 0;
		return {0,0,0};
	}
}

int main() {
	
	// 3 channels
	unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char) * width * height*3);
	FILE* file = fopen("Image.ppm", "w");
	fprintf(file,"P3\n");
	fprintf(file, "%d %d\n255\n", width, height);

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int index = i * height * 3 + j * 3;
			vec3 color = castRay(data, i, j);
			fprintf(file,"%d %d %d\n", (int)color.x, (int)color.y, (int)color.z);
		}
	}

	fclose(file);
	free(data);

}