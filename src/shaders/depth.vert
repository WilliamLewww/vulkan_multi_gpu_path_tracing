#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;

layout(binding = 0, set = 0) uniform Camera {
  vec4 position;
  vec4 right;
  vec4 up;
  vec4 forward;

  float pitch;
  float yaw;
  uint frameCount;
} camera;

layout(binding = 2, set = 0) uniform InstanceDescriptionContainerLens {
  uint instanceCount;
  uint vertexOffsets[8];
  uint normalOffsets[8];
  uint textureCoordinateOffsets[8];
  uint indexOffsets[8];
  uint materialIndexOffsets[8];
  uint materialOffsets[8];
  mat4 transformMatrix[8];
} instanceDescriptionContainerLens;

void main() {
  mat4 viewMatrix = {
    vec4(1, 0, 0, 0),
    vec4(0, 1, 0, 0),
    vec4(0, 0, -1, 0),
    vec4(0, 0, 0, 1)
  };

  float farDist = 1000.0;
  float nearDist = 0.0001;
  float frustumDepth = farDist - nearDist;
  float oneOverDepth = 1.0 / frustumDepth;
  float fov = 1.0472;
  float aspect = 800.0 / 600.0;

  mat4 projectionMatrix = {
    vec4(1.0 / tan(0.5f * fov) / aspect, 0, 0, 0),
    vec4(0, 1.0 / tan(0.5f * fov), 0, 0),
    vec4(0, 0, farDist * oneOverDepth, 1),
    vec4(0, 0, (-farDist * nearDist) * oneOverDepth, 0)
  };

  gl_Position = projectionMatrix * viewMatrix * instanceDescriptionContainerLens.transformMatrix[gl_InstanceIndex] * vec4(inPosition, 1.0);
}
