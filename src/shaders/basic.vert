#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 interpolatedPosition;
layout(location = 1) out uint instanceIndex;

layout(binding = 0, set = 0) uniform Camera {
  vec4 position;
  vec4 right;
  vec4 up;
  vec4 forward;

  uint frameCount;
} camera;

layout(binding = 1, set = 0) uniform InstanceTransformMatrix {
  mat4 transformMatrix[2];

} instanceTransformMatrix;

void main() {
  mat4 viewMatrix = {
    vec4(camera.right.x, camera.up.x, camera.forward.x, 0),
    vec4(camera.right.y, camera.up.y, camera.forward.y, 0),
    vec4(camera.right.z, camera.up.z, camera.forward.z, 0),
    vec4(-dot(camera.right, camera.position), -dot(camera.up, camera.position), -dot(camera.forward, camera.position), 1)
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

  gl_Position = projectionMatrix * viewMatrix * instanceTransformMatrix.transformMatrix[gl_InstanceIndex] * vec4(inPosition, 1.0);

  interpolatedPosition = (instanceTransformMatrix.transformMatrix[gl_InstanceIndex] * vec4(inPosition, 1.0)).xyz;
  instanceIndex = gl_InstanceIndex;
}
