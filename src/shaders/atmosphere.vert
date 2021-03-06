#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	vec3 camera;
} ubo;

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoords;

layout(location = 0) out vec3 fNormal;
layout(location = 1) out vec4 fPosition;
layout(location = 2) out mat4 m;
layout(location = 6) out mat4 v;


out gl_PerVertex {
	vec4 gl_Position;
};

void main()
{
	fPosition = vec4(vPosition, 1.0);
	fNormal = vNormal;
	m = ubo.model;
	v = ubo.view;
	gl_Position = vec4(vPosition, 1.0) * ubo.model * ubo.view * ubo.proj;
}