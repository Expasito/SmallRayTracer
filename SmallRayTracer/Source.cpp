#define _CRT_SECURE_NO_WARNINGS
/*
* 
* Goal: Make a simple ray tracer in as few lines of code as possible.
* 
*/
#include <stdlib.h>
#include <stdio.h>
#include <vector>

const int width = 255;
const int height = 255;
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

float magnitude(vec3 a) {
	return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}
vec3 normalize(vec3 a) {
	float mag = magnitude(a);
	return {a.x/mag,a.y/mag,a.z/mag};
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
std::vector<Triangle> triangles;


// i is width, j is height
//vec3 castRay(unsigned char* data, int i, int j) {
//	// casting towards the z direction
//
//	vec3 dir = { (i-hWidth)/(float)width,-(j-hHeight)/(float)height,1};
//	float t = dot(triangle.p, triangle.n) / dot(dir, triangle.n);
//	vec3 I = { t * dir.x,t * dir.y,t * dir.z };
//
//	vec3 edge0 = triangle.p2-triangle.p1; vec3 edge1 = triangle.p3-triangle.p2; vec3 edge2 = triangle.p1-triangle.p3;
//
//	int index = j * width * 3 + i * 3;
//	if (dot(triangle.n, cross(edge0, { I - triangle.p1})) > 0.0 &&
//		dot(triangle.n, cross(edge1, { I - triangle.p2 })) > 0.0 &&
//		dot(triangle.n, cross(edge2, { I - triangle.p3 })) > 0.0
//	) {
//		data[index + 0] = 255;
//		data[index + 1] = 0;
//		data[index + 2] = 0;
//		return {255,0,0};
//	}
//	else {
//		data[index + 0] = 0;
//		data[index + 1] = 0;
//		data[index + 2] = 0;
//		return {0,0,0};
//	}
//}

struct PayLoad {
	vec3 point;
	vec3 color;
	float distance;
	Triangle* cur;
	bool didHit;
};

PayLoad castRay(vec3 orgin, vec3 dir,int log, Triangle* curr) {
	//printf("%p\n", curr);
	PayLoad closest = { {0,0,0},{0,0,0},100000000,NULL ,false};
	bool found=false;
	for (Triangle& triangle : triangles) {
		if (&triangle == curr) {
			// skip the matched one
			continue;
		}
		//printf("   %p\n", &triangle);
		float t = dot(triangle.p1-orgin, triangle.n) / dot(dir, triangle.n);
		if (log) {
			printf("%f\n", t);
		}
		//printf("%f ", t);
		vec3 I = { t * dir.x,t * dir.y,t * dir.z };


		vec3 edge0 = triangle.p2 - triangle.p1; vec3 edge1 = triangle.p3 - triangle.p2; vec3 edge2 = triangle.p1 - triangle.p3;
		if (dot(triangle.n, cross(edge0, { I - triangle.p1 })) > 0.0 &&
			dot(triangle.n, cross(edge1, { I - triangle.p2 })) > 0.0 &&
			dot(triangle.n, cross(edge2, { I - triangle.p3 })) > 0.0
			) {
			if (t < closest.distance && t > 0) {
				//printf("here\n");
				closest = { I,{100,0,0},t ,&triangle,true};
				found = true;
			}
		}
		else {

		}
	}
	return closest;
	if (found == true) {
		return closest;
	}
	else {
		return { {0,0,0},{0,0,0},-1 ,NULL,false};
	}
	
}

int main() {

	// 3 channels
	unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 3);
	FILE* file = fopen("Image.ppm", "w");
	fprintf(file, "P3\n%d %d\n255\n", width, height);

	//triangles.push_back(
	//	{ {-1,-1,3},{0,1,3},{1,-1,3} }
	//);
	//triangles.push_back(
	//	{ {-.5,-.5,-5},{0,.5,-5},{.5,-.5,-5} }
	//);
	// this is not a triangle...
	triangles.push_back(
		{ {-10,-2,10},{0,2,10},{10,-2,10} }
	);
	// this will cast the shadow
	//triangles.push_back(

	//	{ {-.5,1,4},{0,4,10},{.5,1,4} }
	//);


	//printf("Initial cast\n");
	//vec3 origin = { 0,0,0 };
	//vec3 dir = normalize({ 0, 0, 5 });
	//printf("starting at: %f %f %f towards %f %f %f\n", origin.x, origin.y, origin.z, dir.x, dir.y,dir.z);
	//PayLoad hit = castRay(origin, dir, 1,NULL);
	//printf("%f %f %f %f\n", hit.point.x, hit.point.y, hit.point.z, hit.distance);
	//
	//vec3 light = { 0,0,-10 };
	//dir = normalize({ light.x - hit.point.x, light.y - hit.point.y, light.z - hit.point.z });
	//origin = hit.point;
	//printf("\nstarting at: %f %f %f towards %f %f %f\n", origin.x, origin.y, origin.z, dir.x, dir.y, dir.z);
	////printf("%f %f %f\n", dir.x, dir.y, dir.z);
	//hit = castRay(hit.point, dir,1,hit.cur);
	//printf("%f %f %f %f\n", hit.point.x, hit.point.y, hit.point.z, hit.distance);

//}

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int index = i * height * 3 + j * 3;
			vec3 dir = normalize({ (i - hWidth) / (float)width, -(j - hHeight) / (float)height, 1 });
			
			vec3 origin = { 0,0,-10 };
			PayLoad hit = castRay(origin, dir,0,NULL);
			// we now need to see if the pixel is visible to the light source(which is 0,0,10)
			//printf("%d %d\n", i, j);
			if (hit.didHit==true) {
				//fprintf(file, "%d %d %d\n", 255, 255, 255);
				//exit(1);
				vec3 light = {0,0,-10};
				vec3 dir2 = normalize({ light.x - hit.point.x, light.y - hit.point.y, light.z - hit.point.z });
				//dir2.x *= -1;
				//dir2.y *= -1;
				//dir2.z *= -1;
				//vec3 dir2 = {};
				PayLoad hit2 = castRay(hit.point, dir2,0,hit.cur);
				//printf("     %d  \n", hit2.didHit);

				//printf("%f %f %f %f %f %f\n", hit.point.x, hit.point.y, hit.point.z, dir2.x, dir2.y, dir2.z);
				//printf("%f  ", hit2.distance);
				// no obstructions
				if (hit2.didHit == false) {
					fprintf(file, "%d %d %d\n", 255, 255, 255);
					//printf("Hit\n");
				}
				else {
					fprintf(file, "%d %d %d\n", (int)10, (int)10, (int)10);
				}
				float dist = magnitude(dir2);
				//fprintf(file, "%d %d %d\n", (int)(255 * 1 / dist), (int)(255 * 1 / dist), (int)(255 * 1 / dist));
				//fprintf(file, "%d %d %d\n", i, j, 0);
				
			}
			else {
				fprintf(file, "%d %d %d\n", (int)0, (int)0, 0);
			}
			
		}
	}

	fclose(file);
	free(data);

}


//int main() {
//	
//	// 3 channels
//	unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char) * width * height*3);
//	FILE* file = fopen("Image.ppm", "w");
//	fprintf(file,"P3\n %d %d\n255\n",width,height);
//
//	for (int j = 0; j < height; j++) {
//		for (int i = 0; i < width; i++) {
//			int index = i * height * 3 + j * 3;
//			vec3 color = castRay(data, i, j);
//			fprintf(file,"%d %d %d\n", (int)color.x, (int)color.y, (int)color.z);
//		}
//	}
//
//	fclose(file);
//	free(data);
//
//}

