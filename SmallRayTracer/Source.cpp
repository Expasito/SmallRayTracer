#define _CRT_SECURE_NO_WARNINGS
/*
* 
* Goal: Make a simple ray tracer in as few lines of code as possible.
* 
*/


// Added colors to triangles, working on some weird issues

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <chrono>

const int width = 500;
const int height = 500;
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

	vec3 edge0, edge1, edge2;

	vec3 color;

	Triangle(vec3 p1_, vec3 p2_, vec3 p3_,vec3 color_) {
		p1 = p1_; p2 = p2_; p3 = p3_;
		// default to p1 here
		p = p1;
		n = cross(p1_ - p2_, p1_ - p3_);

		// precalculate these since our triangles do not move
		edge0 = p2 - p1;
		edge1 = p3 - p2;
		edge2 = p1 - p3;
		color = color_;
	}

};
std::vector<Triangle> triangles;

struct Light {
	vec3 position;
	float intensity;
};
std::vector<Light> lights;

struct PayLoad {
	vec3 point;
	vec3 color;
	float distance;
	Triangle* cur;
	bool didHit;
};

PayLoad castRay(vec3 orgin, vec3 dir,int log, Triangle* curr) {
	if (log) {
		printf("%p\n", curr);
		printf("Origin: %f %f %f\n", orgin.x, orgin.y, orgin.z);
		printf("Dir: %f %f %f\n", dir.x, dir.y, dir.z);
	}
	
	PayLoad closest = { {0,0,0},{0,0,0},100000000,NULL ,false};
	bool found=false;
	for (Triangle& triangle : triangles) {
		if (&triangle == curr) {
			// skip the matched one if curr is a Triangle*
			continue;
		}
		if (log) {
			printf("   %p\n", &triangle);
		}
		float t = dot(triangle.p1-orgin, triangle.n) / dot(dir, triangle.n);
		// negative t values get filtered out already so do it now
		if (t < 0) {
			continue;
		}
		if (log) {
			printf("t: %f\n", t);
			printf("Top: %f\n", dot(triangle.p1 - orgin, triangle.n));
			printf("Bottom: %f\n", dot(dir, triangle.n));
		}

		vec3 I = { orgin.x+ t * dir.x,orgin.y+t * dir.y,orgin.z + t * dir.z };
		if (log) {
			printf("I: %f %f %f\n", I.x, I.y, I.z);
		}

		if (dot(triangle.n, cross(triangle.edge0, { I - triangle.p1 })) > 0.0 &&
			dot(triangle.n, cross(triangle.edge1, { I - triangle.p2 })) > 0.0 &&
			dot(triangle.n, cross(triangle.edge2, { I - triangle.p3 })) > 0.0
			) {
			if (t < closest.distance && t > 0.00001) {
				//printf("here\n");
				closest = { I,triangle.color,t ,&triangle,true};
				found = true;
			}
		}
	}
	return closest;
}

int main() {
	// 3 channels
	unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 3);
	FILE* file = fopen("Image.ppm", "w");
	fprintf(file, "P3\n%d %d\n255\n", width, height);

	triangles.push_back(
		{ {-1,-1,4},{0,1,4},{1,-1,4}, {255,0,0} }
	);

	triangles.push_back(
		{ {-.5,0,4},{0,4,5},{.5,2,4}, {0,255,0} }
	);

	triangles.push_back(
		{ {-5,-2,-10},{0,-2,30},{5,-2,-10}, {0,0,255} }
	);

	triangles.push_back(
		{ {-1,-1,7},{0,-1,9},{1,-1,7},{255,255,0} }
	);

	triangles.push_back(
		{ {1,-1,2},{2,0,4},{3,0,2}, {255,0,255} }
	);

	triangles.push_back(
		{ {1,-1,8},{2,1,8},{3,-1,8}, {0,255,255} }
	);

	triangles.push_back(
		{ {-1,-1,3},{0,1,3},{1,-1,3}, {255,255,255} }
	);
	triangles.push_back(
		{ {-1,-1,3.0001},{0,1,3.0001},{1,-1,3.0001} ,{128,128,128} }
	);

	lights.push_back(
		{ {1,4,5},255 }
	);
	lights.push_back(
		{ {0,6,4},255 }
	);
	lights.push_back(
		{ {0,0,4},255 }
	);

	lights.push_back(
		{ {-4,4,-4},160 }
	);

	lights.push_back(
		{ {0,0,0},240 }
	);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int index = i * height * 3 + j * 3;
			
			vec3 dir;
			vec3 origin;
			// for first pass
			dir = normalize({ (i - hWidth) / (float)width, -(j - hHeight) / (float)height, 1 });
			origin = { 0,1,-5 };

			PayLoad hit = castRay(origin, dir,0,NULL);
			vec3 color = { 0,0,0 };
			// we only care if it hit
			if (hit.didHit == true) {
				color = { 0,0,0 };
				// now check for if the area is in a shadow or not
				for (Light& light : lights) {
					origin = hit.point;
					dir = normalize({ light.position.x - hit.point.x, light.position.y - hit.point.y, light.position.z - hit.point.z });
					PayLoad hit_ = castRay(origin, dir, 0, hit.cur);
					float dist = magnitude(light.position - hit.point);
					dist *= dist;
					// no collisions at all OR collision but after hitting the light
					if (hit_.didHit == false || (hit_.didHit == true && hit_.distance > dist)) {
						color.x += light.intensity/dist * hit.color.x/255.0;
						color.y += light.intensity/dist * hit.color.y/255.0;
						color.z += light.intensity/dist * hit.color.z/255.0;
					}

				}
			}
			// we divide by 255 because we multiply by upto 255, so we are scaling it back down
			//color.x /= 255.0;
			//color.y /= 255.0;
			//color.z /= 255.0;


			if (color.x > 255) {
				color.x = 255;
			}
			if (color.y > 255) {
				color.y = 255;
			}
			if (color.z > 255) {
				color.z = 255;
			}



			// now save color
			fprintf(file, "%d %d %d\n", (int)color.x, (int)color.y, (int)color.z);
			
		}
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	float milis = (end - begin).count()/1000000;
	printf("Time taken: %f[ms], FPS: %f", milis,1000.0/milis);

	fclose(file);
	free(data);

}

// Still under 250 lines with comments and logging