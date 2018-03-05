#version 450
#extension GL_ARB_separate_shader_objects : enable

// vertex attributes
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
// texCoord
// color

// uniforms
layout(set = 0, binding = 0) uniform UniformBufferObject 
{
  mat4 model;
  mat4 view;
  mat4 proj;
} ubo;

layout(location = 0) out vec3 vertPosition;
layout(location = 1) out vec3 vertNormal;

//out gl_PerVertex {
//    vec4 gl_Position;
//};

void main() {
	mat4 modelView = ubo.view * ubo.model;
	vec4 viewSpacePosition = modelView * vec4(inPosition, 1.0);
	vec3 viewSpaceNormal = mat3( modelView ) * inNormal;
	vertPosition = viewSpacePosition.xyz;
	vertNormal = viewSpaceNormal;
    gl_Position = ubo.proj * viewSpacePosition;
}