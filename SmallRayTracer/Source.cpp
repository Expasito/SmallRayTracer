#define _CRT_SECURE_NO_WARNINGS
/*
* 
* Goal: Make a simple ray tracer in as few lines of code as possible.
* 
*/
#include <stdlib.h>
#include <stdio.h>

const int width = 400;
const int height = 400;
const int hWidth = width / 2;
const int hHeight = height / 2;

// a triangle is a point(x,y,z) and a normal(n1,n2,n3)

struct vec3{
	float x,y,z;

	vec3 operator-(vec3 b) {
		return {x-b.x,y-b.y,z-b.z};
	}
};


typedef struct {
	vec3 p;
	vec3 n;

} Triangle;
// the points we will use are -1,-1,1   0,1,1   1,-1,1
// edge1 = 1,2,0,  edge2 = 1,-2,0, edge3 = -2,0,0
// this will be 
const Triangle triangle{ {0,0,2},{0,0,-1} };

float dot(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;

}

vec3 cross(vec3 a, vec3 b) {
	return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z,a.x*b.y-a.y*b.x};
}

void castRay(unsigned char* data, int i, int j) {
	// casting towards the z direction
	vec3 dir = {(i-hWidth)/(float)width,(j - hHeight)/(float)height,1};
	float t = dot(triangle.p, triangle.n) / dot(dir, triangle.n);
	//printf("%f\n", t);
	vec3 I = { t * dir.x,t * dir.y,t * dir.z };
	vec3 edge0 = { 1,2,0 }; vec3 edge1 = { 1,-2,0 }; vec3 edge2 = { -2,0,0 };
	int index = i * width * 3 + j * 3;
	if (dot(triangle.n, cross(edge0, { I - vec3{1,2,0} }))>0.0 &&
		dot(triangle.n, cross(edge1, { I - vec3{1,-2,0} })) > 0.0 &&
		dot(triangle.n, cross(edge2, { I - vec3{-2,0,0} })) > 0.0
	
	) {
		printf("here");
		data[index + 0] = 255;
		data[index + 1] = 0;
		data[index + 2] = 0;
	}
	else {
		data[index + 0] = 0;
		data[index + 1] = 0;
		data[index + 2] = 0;
	}
	
}

int main() {
	
	// 3 channels
	unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char) * width * height*3);
	FILE* file = fopen("Image.ppm", "w");
	fprintf(file,"P3\n");
	fprintf(file, "%d %d\n255\n", width, height);
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			castRay(data, i, j);
		}
	}
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			int index = i * width * 3 + j * 3;
			fprintf(file, "%d %d %d\n", data[index], data[index + 1], data[index + 2]);
		}
	}



	fclose(file);


}