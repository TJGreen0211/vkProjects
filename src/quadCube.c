#include "quadCube.h"

void subdivideFace(quadCube *qc, double reverse, int divisions, int order[3]) {
    vec3 start = {1.0, 1.0, reverse};
	double offset = 2.0/((double)divisions);

	for(int i = 0; i < divisions; i++) {
		start.x = 1.0;
		for(int j = 0; j < divisions; j++) {
            double faces[4][3];
            faces[0][order[0]] = start.x;   	   faces[0][order[1]] = start.y;          faces[0][order[2]] = start.z;
			faces[1][order[0]] = start.x-offset;   faces[1][order[1]] = start.y;          faces[1][order[2]] = start.z;
			faces[2][order[0]] = start.x-offset;   faces[2][order[1]] = start.y-offset;   faces[2][order[2]] = start.z;
            faces[3][order[0]] = start.x;          faces[3][order[1]] = start.y-offset;   faces[3][order[2]] = start.z;

			if(start.z == 1.0) {
                memcpy(&qc->points[qc->vertexNumber++], &faces[0], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[1], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[2], sizeof(faces[0]));

                memcpy(&qc->points[qc->vertexNumber++], &faces[0], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[2], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[3], sizeof(faces[0]));


			}
			else {
                memcpy(&qc->points[qc->vertexNumber++], &faces[0], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[3], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[2], sizeof(faces[0]));

                memcpy(&qc->points[qc->vertexNumber++], &faces[0], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[2], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[1], sizeof(faces[0]));
			}

			start.x = start.x - offset;
		}
		start.y -= offset;
	}
}

void generateSmoothNormals(quadCube *qc) {
	vec3 *vna = malloc(qc->vertexNumber*sizeof(vec3));
	vec3 vn;
	for(int i = 0; i < qc->vertexNumber; i++) {
		vec3 tempvn = {0.0, 0.0, 0.0};
		vn = qc->points[i];
		for(int j = 0; j < qc->vertexNumber; j++) {
			if(vn.x == qc->points[j].x && vn.y == qc->points[j].y && vn.z == qc->points[j].z) {
				tempvn = plusequalvec3(tempvn, qc->normals[j]);
			}
		}
		vna[i] = normalizevec3(tempvn);
	}

	for(int i = 0; i < qc->vertexNumber; i++) {
		qc->normals[i] = vna[i];
	}
	free(vna);
}

void createCube(int divisions, quadCube *newQuadCube) {
	/*
	a,b,c,d,e,f,g,h
			 (-1,1,1)e-------f(1,1,1)
				   / |	   / |
				 /	 |	 /	 |
	   (-1,1,-1)a-------b(1,1|,-1)
		   (-1,-|1,1)g--|----h(1,-1,1)
				|  /	|   /
				|/		| /
	  (-1,-1,-1)c-------d(1,-1,-1)
	*/

	newQuadCube->points = malloc(divisions*divisions*sizeof(vec3)*6*6);
	newQuadCube->normals = malloc(divisions*divisions*sizeof(vec3)*6*6);

    //int order[3] = {0, 1, 2};
    //int order[3] = {2, 0, 1};
    //int order[3] = {1, 2, 0};

	newQuadCube->vertexNumber = 0;

    int order[3] = {0, 1, 2};
	subdivideFace(newQuadCube, 1.0, divisions, order);
	subdivideFace(newQuadCube, -1.0, divisions, order);
    order[0] =2; order[1] = 0; order[2] = 1;
    subdivideFace(newQuadCube, 1.0, divisions, order);
	subdivideFace(newQuadCube, -1.0, divisions, order);
    order[0] = 1; order[1] = 2; order[2] = 0;
    subdivideFace(newQuadCube, 1.0, divisions, order);
	subdivideFace(newQuadCube, -1.0, divisions, order);

	newQuadCube->size = newQuadCube->vertexNumber*sizeof(vec3);
	newQuadCube->nsize = newQuadCube->vertexNumber*sizeof(vec3);

	for(int i = 0; i < newQuadCube->vertexNumber; i++) {
		//printf("x:%f, y:%f, z:%f\n", newQuadCube.points[i].x, newQuadCube.points[i].y, newQuadCube.points[i].z);
		newQuadCube->points[i] = normalizevec3(newQuadCube->points[i]);
	}

	for(int i = 0; i < newQuadCube->vertexNumber; i+=3)
	{
		vec3 one, two;

		one.x = newQuadCube->points[i+1].x - newQuadCube->points[i].x;
		one.y = newQuadCube->points[i+1].y - newQuadCube->points[i].y;
		one.z = newQuadCube->points[i+1].z - newQuadCube->points[i].z;

		two.x = newQuadCube->points[i+2].x - newQuadCube->points[i+1].x;
		two.y = newQuadCube->points[i+2].y - newQuadCube->points[i+1].y;
		two.z = newQuadCube->points[i+2].z - newQuadCube->points[i+1].z;

		vec3 normal = normalizevec3(crossvec3(one, two));

		newQuadCube->normals[i] = normal;
		newQuadCube->normals[i+1] = normal;
		newQuadCube->normals[i+2] = normal;
	}

	generateSmoothNormals(newQuadCube);
}

void destroyCube(quadCube *newQuadCube) {
    free(newQuadCube->points);
    newQuadCube->points = NULL;
    free(newQuadCube->normals);
    newQuadCube->points = NULL;
}