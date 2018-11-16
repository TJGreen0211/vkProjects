#ifndef LINEARALG_H
#define LINEARALG_H
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct mat4 {
	double m[4][4];
} mat4;

typedef struct mat3 {
	double m[3][3];
} mat3;

typedef struct mat2 {
	double m[2][2];
} mat2;

typedef struct vec4 {
	double x, y, z, w;
} vec4;

typedef struct vec3 {
	double x, y, z;
} vec3;

typedef struct vec2 {
	double x, y;
} vec2;

typedef struct quaternion {
	double w, x, y, z;
} quaternion;

double quatLength(quaternion q);
quaternion quatNormalize(quaternion q);
quaternion quatConjugate(quaternion q);
quaternion quatMultiply(quaternion q, quaternion u);
mat4 quaternionToRotation(quaternion q);
quaternion angleAxis(double angle, vec3 axis, vec3 point);

vec4 addvec4(vec4 v, vec4 u);
vec3 addvec3(vec3 v, vec3 u);
vec2 addvec2(vec2 v, vec2 u);

vec4 scalarMultvec4(vec4 v, double u);
vec3 scalarMultvec3(vec3 v, double u);
vec2 scalarMultvec2(vec2 v, double u);

vec4 dividevec4(vec4 v, double u);
vec3 dividevec3(vec3 v, double u);
vec2 dividevec2(vec2 v, double u);

vec4 plusequalvec4(vec4 v, vec4 u);
vec3 plusequalvec3(vec3 v, vec3 u);
vec2 plusequalvec2(vec2 v, vec2 u);

vec4 minusequalvec4(vec4 v, vec4 u);
vec3 minusequalvec3(vec3 v, vec3 u);
vec2 minusequalvec2(vec2 v, vec2 u);

double dotvec4(vec4 u, vec4 v);
double dotvec3(vec3 u, vec3 v);
double dotvec2(vec2 u, vec2 v);

double lengthvec4(vec4 v);
double lengthvec3(vec3 v);
double lengthvec2(vec2 v);

vec4 normalizevec4(vec4 v);
vec3 normalizevec3(vec3 v);
vec2 normalizevec2(vec2 v);

//Not technically defined in R4 but  still useful
vec4 crossvec4(vec4 v, vec4 u);
vec3 crossvec3(vec3 v, vec3 u);

vec4 multiplyvec4(vec4 v, vec4 u);
vec3 multiplyvec3(vec3 v, vec3 u);
vec2 multiplyvec2(vec2 v, vec2 u);

mat4 identityMatrix();

mat4 rotateX(double theta);
mat4 rotateY(double theta);
mat4 rotateZ(double theta);

mat4 translate(double x, double y, double z);
mat4 translatevec4( vec4 v );
mat4 translatevec3( vec3 v );
mat4 translatevec2( vec2 v );

mat4 scalevec4(double x, double y, double z);
mat4 scalevec3(vec3 v);
mat4 scale(double value);

mat4 multiplymat4(mat4 c, mat4 d);
vec4 multiplymat4vec4(mat4 a, vec4 v);
vec3 multiplymat3vec3(mat3 a, vec3 v);
vec2 multiplymat2vec2(mat2 a, vec2 v);

mat4 transposemat4(mat4 a);
mat3 transposemat3(mat3 a);
mat2 transposemat2(mat2 a);

mat4 perspective(double fovy, double aspect, double zNear, double zFar);
mat4 frustum(double left, double right, double bottom, double top, double zNear, double zFar);
mat4 ortho(double left, double right, double bottom, double top, double zNear, double zFar );
mat4 lookAt(vec3 eye, vec3 at, vec3 up);

#endif
