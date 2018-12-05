#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 fColor;
layout (location = 1) in vec2 fTexCoords;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 FragColor;

layout(location = 2) in vec3 fE;
layout(location = 3) in vec3 fN;
layout(location = 4) in vec3 fL;
layout(location = 5) in vec3 fH;


void main() {
	//FragColor = vec4(fColor, 0.0, 1.0);
	vec4 ambientProduct = vec4(0.2, 0.2, 0.2, 1.0)*vec4(0.2, 0.2, 1.0, 1.0);
	vec4 diffuseProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.8, 0.8, 0.8, 1.0);
	vec4 specularProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.5, 0.5, 0.5, 1.0);
	float shininess = 2.0;

	float Kd = max(dot(fL, fN), 0.0);
	float Ks = pow(max(dot(fN, fH), 0.0), shininess);
	vec4 color = vec4(texture(texSampler, fTexCoords)).rgba;
	vec3 ambient = ambientProduct.xyz;// * color.rgb;
	vec3 diffuse = Kd * diffuseProduct.xyz;// * color.rgb;
	vec3 specular = Ks * specularProduct.xyz;
	if(dot(fL, fN) < 0.0) {
		specular = vec3(0.0, 0.0, 0.0);
	}

	FragColor = vec4(ambient+diffuse+specular, 1.0);
	//FragColor = texture(texSampler, fTexCoords);
}