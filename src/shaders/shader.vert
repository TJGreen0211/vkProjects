#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	vec3 camera;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fColor;
layout(location = 1) out vec2 fTexCoord;

out gl_PerVertex {
	vec4 gl_Position;
};

layout(location = 2) out vec3 fE;
layout(location = 3) out vec3 fN;
layout(location = 4) out vec3 fL;
layout(location = 5) out vec3 fH;

void main() {
	gl_Position = vec4(inPosition, 1.0) * ubo.model * ubo.view * ubo.proj;

	vec4 ray = normalize(ubo.model*vec4(inPosition, 1.0) - vec4(ubo.camera, 1.0));
	mat3 normalMatrix = transpose(inverse(mat3(ubo.model)));
	vec4 lightPos = vec4(10.0, 5.0, -4.0, 1.0);
	vec3 lightDir = normalize(vec4(inPosition, 1.0)*ubo.model - lightPos).xyz;

	fE = -normalize(vec4(inPosition, 1.0)*ubo.model).xyz;
	fN = normalize(inColor*normalMatrix);
	fL = -normalize(lightDir);
	fH = -normalize((vec4(inPosition, 1.0)*ubo.model - lightPos) + ray).xyz;

	fColor = inColor;//vec3(ubo.model[0][0], 0.0, 0.0);
	fTexCoord = inTexCoord;
}