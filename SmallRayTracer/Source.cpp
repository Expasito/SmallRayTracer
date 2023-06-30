#define _CRT_SECURE_NO_WARNINGS
/*
* 
* Goal: Make a simple ray tracer in as few lines of code as possible.
* 
*/
#include <stdlib.h>
#include <stdio.h>
#include <vector>

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

	Triangle(vec3 p1_, vec3 p2_, vec3 p3_) {
		p1 = p1_; p2 = p2_; p3 = p3_;
		// default to p1 here
		p = p1;
		n = cross(p1_ - p2_, p1_ - p3_);
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
			// skip the matched one
			continue;
		}
		if (log) {
			printf("   %p\n", &triangle);
		}
		float t = dot(triangle.p1-orgin, triangle.n) / dot(dir, triangle.n);
		if (log) {
			printf("t: %f\n", t);
			printf("Top: %f\n", dot(triangle.p1 - orgin, triangle.n));
			printf("Bottom: %f\n", dot(dir, triangle.n));
		}
		//printf("%f ", t);
		vec3 I = { orgin.x+ t * dir.x,orgin.y+t * dir.y,orgin.z + t * dir.z };
		if (log) {
			printf("I: %f %f %f\n", I.x, I.y, I.z);
		}


		vec3 edge0 = triangle.p2 - triangle.p1; vec3 edge1 = triangle.p3 - triangle.p2; vec3 edge2 = triangle.p1 - triangle.p3;
		if (dot(triangle.n, cross(edge0, { I - triangle.p1 })) > 0.0 &&
			dot(triangle.n, cross(edge1, { I - triangle.p2 })) > 0.0 &&
			dot(triangle.n, cross(edge2, { I - triangle.p3 })) > 0.0
			) {
			if (t < closest.distance && t > 0.00001) {
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
	//	{ {-1,-1,4},{0,1,4},{1,-1,4} }
	//);


	//triangles.push_back(
	//	{ {-.5,0,4},{0,4,5},{.5,2,4} }
	//);

	triangles.push_back(
		{ {-5,-2,-10},{0,-2,30},{5,-2,-10} }
	);

	//triangles.push_back(
	//	{ {-1,-1,7},{0,-1,9},{1,-1,7} }
	//);

	//triangles.push_back(
	//	{ {1,-1,2},{2,0,4},{3,0,2} }
	//);

	triangles.push_back(
		{ {1,-1,8},{2,1,8},{3,-1,8} }
	);

	triangles.push_back(
		{ {-1,-1,3},{0,1,3},{1,-1,3} }
	);
	triangles.push_back(
		{ {-1,-1,3.0001},{0,1,3.0001},{1,-1,3.0001} }
	);

	lights.push_back(
		{ {0,-1.9,5},255 }
	);
	//lights.push_back(
	//	{ {0,6,4},255 }
	//);
	//lights.push_back(
	//	{ {0,0,4},255 }
	//);

	printf("%f %f %f\n", triangles.at(0).n.x,triangles.at(0).n.y, triangles.at(0).n.z);
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
			if (hit.didHit == true) {
				color = { 0,0,0 };
				for (Light& light : lights) {
					origin = hit.point;
					dir = normalize({ light.position.x - hit.point.x, light.position.y - hit.point.y, light.position.z - hit.point.z });
					PayLoad hit_ = castRay(origin, dir, 0, hit.cur);
					if (i == 401 && j == 499) {
						hit_ = castRay(origin, dir, 1, hit.cur);
					}
					else {
						hit_ = castRay(origin, dir, 0, hit.cur);

					}
					if (i == 401 && j == 499) {
						printf("%f %f %f\n", dir.x, dir.y, dir.z);
						printf("%f\n", hit_.distance);
						printf("%f %f %f\n", hit_.point.x, hit_.point.y, hit_.point.z);
						

					}
					float dist = magnitude(light.position - hit.point);
					// no collisions at all
					if (hit_.didHit == false) {
						//printf("%f\n", dist);
						color.x += light.intensity/dist;
						color.y += light.intensity/dist;
						color.z += light.intensity/dist;
					} // colission but after light
					else if (hit_.didHit == true && hit_.distance > dist) {
						color.x += light.intensity / dist;
						color.y += light.intensity / dist;
						color.z += light.intensity / dist;
					}

				}
				//if (color.x==255 && color.y==100 && color.z==100) {
				//	printf("%d %d\n", i, j);
				//}
			}
			else {
				
			}
			if (color.x > 255) {
				color = { 255,255,255 };
			}

			// now save color
			fprintf(file, "%d %d %d\n", (int)color.x, (int)color.y, (int)color.z);
			
			//if (hit.didHit==true) {
	
			//	vec3 light = {0,4,0};
			//	vec3 dir2 = normalize({ light.x - hit.point.x, light.y - hit.point.y, light.z - hit.point.z });

			//	PayLoad hit2 = castRay(hit.point, dir2,0,hit.cur);
			//	//printf("     %d  \n", hit2.didHit);

			//	//printf("%f %f %f %f %f %f\n", hit.point.x, hit.point.y, hit.point.z, dir2.x, dir2.y, dir2.z);
			//	//printf("%f  ", hit2.distance);
			//	float dist = magnitude({ light.x - hit.point.x, light.y - hit.point.y, light.z - hit.point.z });
			//	dist = dist / 4;
			//	// prevent the color value from being over 255
			//	if (dist * dist < 1) {
			//		dist = 1;
			//	}


			//	// no obstructions
			//	if (hit2.didHit == false) {
			//		fprintf(file, "%d %d %d\n", (int)(255 / (dist * dist)), (int)(255 / (dist * dist)), (int)(255 / (dist * dist)));

			//	}
			//	else {
			//		fprintf(file, "%d %d %d\n", (int)(255 / (dist * dist)/2), (int)(255 / (dist * dist)/2), (int)(255 / (dist * dist)/2));
			//	}

			//	
			//}
			//else {
			//	fprintf(file, "%d %d %d\n", (int)0, (int)0, 0);
			//}
			
		}
	}

	fclose(file);
	free(data);

}
