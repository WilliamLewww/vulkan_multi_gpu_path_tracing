#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_ray_query : enable

#define M_PI 3.1415926535897932384626433832795

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  vec3 emission;

  float shininess;
  float dissolve;
  float ior;
};

layout(early_fragment_tests) in;

layout(location = 0) in vec3 interpolatedPosition;
flat layout(location = 1) in uint rasterInstanceIndex;
flat layout(location = 2) in mat4 cameraMatrix;

layout(location = 0) out vec4 outColor;

layout(binding = 0, set = 0) uniform Camera {
  vec4 position;
  vec4 right;
  vec4 up;
  vec4 forward;

  float pitch;
  float yaw;
  uint frameCount;
} camera;

layout(binding = 1, set = 0) uniform SceneInstanceDescriptionContainer {
  uint instanceCount;
  uint vertexOffsets[8];
  uint normalOffsets[8];
  uint textureCoordinateOffsets[8];
  uint indexOffsets[8];
  uint materialIndexOffsets[8];
  uint materialOffsets[8];
  mat4 transformMatrix[8];
} sceneInstanceDescriptionContainer;

layout(binding = 2, set = 0) uniform InstanceDescriptionContainer {
  uint instanceCount;
  uint vertexOffsets[8];
  uint normalOffsets[8];
  uint textureCoordinateOffsets[8];
  uint indexOffsets[8];
  uint materialIndexOffsets[8];
  uint materialOffsets[8];
  mat4 transformMatrix[8];
} instanceDescriptionContainer;

layout(binding = 4, set = 0) uniform accelerationStructureEXT topLevelAS;

layout(binding = 7, set = 0) buffer LensProperties {
  uint apertureInstanceIndex;
  uint aperturePrimitiveCount;
  uint aperturePrimitiveOffset;
  uint lastLensElementInstanceIndex;
  uint lastLensElementPrimitiveCount;
  uint filmInstanceIndex;
} lensProperties;

layout(binding = 10, set = 0) buffer FlareBuffer { float data[]; } flareBuffer;

layout(binding = 0, set = 1) buffer SceneIndexBuffer { uint data[]; } sceneIndexBuffer;
layout(binding = 1, set = 1) buffer SceneVertexBuffer { float data[]; } sceneVertexBuffer;
layout(binding = 2, set = 1) buffer SceneNormalIndexBuffer { uint data[]; } sceneNormalIndexBuffer;
layout(binding = 3, set = 1) buffer SceneNormalBuffer { float data[]; } sceneNormalBuffer;
layout(binding = 6, set = 1) buffer SceneMaterialIndexBuffer { uint data[]; } sceneMaterialIndexBuffer;
layout(binding = 7, set = 1) buffer SceneMaterialBuffer { Material data[]; } sceneMaterialBuffer;
layout(binding = 8, set = 1) buffer SceneMaterialLightBuffer { 
  uint count; 
  uint indicesPrimitive[64];
  uint indicesInstance[64];
} sceneMaterialLightBuffer;

layout(binding = 0, set = 2) buffer IndexBuffer { uint data[]; } indexBuffer;
layout(binding = 1, set = 2) buffer VertexBuffer { float data[]; } vertexBuffer;
layout(binding = 2, set = 2) buffer NormalIndexBuffer { uint data[]; } normalIndexBuffer;
layout(binding = 3, set = 2) buffer NormalBuffer { float data[]; } normalBuffer;
layout(binding = 4, set = 2) buffer TextureCoordinateIndexBuffer { uint data[]; } textureCoordinateIndexBuffer;
layout(binding = 5, set = 2) buffer TextureCoordinateBuffer { float data[]; } textureCoordinateBuffer;
layout(binding = 6, set = 2) buffer MaterialIndexBuffer { uint data[]; } materialIndexBuffer;
layout(binding = 7, set = 2) buffer MaterialBuffer { Material data[]; } materialBuffer;
layout(binding = 8, set = 2) buffer MaterialLightBuffer { 
  uint count; 
  uint indicesPrimitive[64];
  uint indicesInstance[64];
} materialLightBuffer;

float random(vec2 uv, float seed) {
  return fract(sin(mod(dot(uv, vec2(12.9898, 78.233)) + 1113.1 * seed, M_PI)) * 43758.5453);;
}

void main() {
  outColor = vec4(flareBuffer.data[int(int(gl_FragCoord.y) * 800 + gl_FragCoord.x)]);
  flareBuffer.data[int(int(gl_FragCoord.y) * 800 + gl_FragCoord.x)] = 0.0;
}