#include "arcballCamera.h"

const double toRadians = M_PI / 180.0;

static double Yaw = -90.0f;
static double Pitch = 0.0f;
static double MovementSpeed = 0.1f;
static double maxSpeed = 2000.0f;
static double MouseSensitivity = 0.6f;
static double mouseZoom = 1.0f;

static double lastx, lasty;

mat4 tr, rxry;
vec3 rotation = {0.0, 0.0, 0.0};
vec3 Position = {0.0, 1.0, 0.0};
vec3 Up = {0.0, 0.0, 1.0};
vec3 Front = {0.0, 0.0, -1.0};
vec3 Right = {1.0, 0.0, 0.0};

mat4 getViewMatrix()
{
	double rad = 180.0 / M_PI;
	double arcYaw, arcPitch;

	tr = translate(rotation.x+Position.x, rotation.y+Position.y, rotation.z+Position.z);
	vec3 d = {rotation.x - 0.0, rotation.y - 0.0, rotation.z - 0.0};
	d = normalizevec3(d);
	arcYaw = asin(-d.y) * rad;
	arcPitch = atan2(d.x, -d.z) * rad;
	//arcPitch = arcPitch - 180.0;
	rxry = multiplymat4(rotateX(arcYaw), rotateY(arcPitch));

	//printf("DeltaTime: %f\n", arcYaw);

	return multiplymat4(rxry, tr);
}

vec3 getCamera() {
	return Position;
}

vec4 getCameraPosition(mat4 position) {
	mat4 mvTranspose = transposemat4(multiplymat4(position, getViewPosition()));
	vec4 inverseCamera = {-mvTranspose.m[3][0], -mvTranspose.m[3][1], -mvTranspose.m[3][2], -mvTranspose.m[3][3]};
	vec4 camPosition = multiplymat4vec4(mvTranspose, inverseCamera);

	return camPosition;
}

mat4 getViewPosition()
{
	return tr;
}

mat4 getViewRotation()
{
	return rxry;
}

void updateCameraVectors()
{
	Front.x = rxry.m[2][0];
	Front.y = rxry.m[2][1];
	Front.z = rxry.m[2][2];
	Front = normalizevec3(Front);

	Right.x = rxry.m[0][0];
	Right.y = rxry.m[0][1];
	Right.z = rxry.m[0][2];
	Right = normalizevec3(Right);

	Up.x = rxry.m[1][0];
	Up.y = rxry.m[1][1];
	Up.z = rxry.m[1][2];
	Up = normalizevec3(Up);
}

void processKeyboard(enum Camera_Movement direction, double deltaTime, double deltaSpeed)
{
	if(deltaSpeed > maxSpeed)
		deltaSpeed = maxSpeed;
	double velocity = MovementSpeed * deltaTime;// + deltaSpeed;

	//printf("DeltaTime: %f\n", deltaTime);

    if (direction == FORWARD)
        Position = plusequalvec3(Position, scalarMultvec3(Front, velocity));
    if (direction == BACKWARD)
        Position = minusequalvec3(Position, scalarMultvec3(Front, velocity));
    if (direction == LEFT)
        Position = minusequalvec3(Position, scalarMultvec3(Right, velocity));
    if (direction == RIGHT)
        Position = plusequalvec3(Position, scalarMultvec3(Right, velocity));
}

int constrainPitch;
void processMouseMovement(double xpos, double ypos, int resetFlag)
{
	vec3 mouseArc = {xpos, ypos, 0.0};
	mouseArc.y = -mouseArc.y;
	double mouseArc2 = mouseArc.x * mouseArc.x + mouseArc.y * mouseArc.y;

	if(mouseArc2 <= 1*1)
		mouseArc.z = sqrt(1*1 - mouseArc2);
	else
		mouseArc = normalizevec3(mouseArc);

	if(resetFlag) {
		lastx = xpos;
		lasty = ypos;
	}
	else {
		double diffx = xpos - lastx;
		double diffy = ypos - lasty;
		lastx = xpos;
		lasty = ypos;
		diffx *= MouseSensitivity;
		diffy *= MouseSensitivity;

		Yaw += diffx;
		Pitch += diffy;

		if(Pitch > 159.0)
			Pitch = 159.0f;
		if(Pitch < 1.0)
			Pitch = 1.0f;

		rotation.x = mouseZoom * cos(Yaw/50.0) * sin(-Pitch/50.0);
		rotation.y = mouseZoom * cos(-Pitch/50.0);
		rotation.z = mouseZoom * sin(Yaw/50.0) * sin(-Pitch/50.0);

		updateCameraVectors();
	}
}

double processMouseScroll(double yoffset)
{
	if (mouseZoom >= 1.0f && mouseZoom <= 20.0f)
        mouseZoom -= yoffset;
    if (mouseZoom <= 1.0f)
        mouseZoom = 1.0f;
    if (mouseZoom >= 20.0f)
         mouseZoom = 20.0f;

    rotation.x = mouseZoom * cos(Yaw/50.0) * sin(-Pitch/50.0);
	rotation.y = mouseZoom * cos(-Pitch/50.0);
	rotation.z = mouseZoom * sin(Yaw/50.0) * sin(-Pitch/50.0);

    return mouseZoom;
}
