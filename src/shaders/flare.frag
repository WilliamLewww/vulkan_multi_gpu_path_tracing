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

layout(binding = 3, set = 0) uniform accelerationStructureEXT sceneTopLevelAS;
layout(binding = 4, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 6, set = 0) buffer RayDirectionBuffer { float data[]; } rayDirectionBuffer;

layout(binding = 7, set = 0) buffer LensProperties {
  uint apertureInstanceIndex;
  uint aperturePrimitiveCount;
  uint aperturePrimitiveOffset;
  uint lastLensElementInstanceIndex;
  uint lastLensElementPrimitiveCount;
} lensProperties;

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

ivec3 getIndicesFromPrimitive(uint instanceIndex, uint primitiveIndex) {
  uint indexOffset = instanceDescriptionContainer.indexOffsets[instanceIndex];

  ivec3 indices = ivec3(indexBuffer.data[3 * primitiveIndex + 0 + indexOffset], 
                        indexBuffer.data[3 * primitiveIndex + 1 + indexOffset], 
                        indexBuffer.data[3 * primitiveIndex + 2 + indexOffset]);

  return indices;
};

void getVertexFromIndices(uint instanceIndex, uint primitiveIndex, out vec3 vertexA, out vec3 vertexB, out vec3 vertexC) {
  uint vertexOffset = instanceDescriptionContainer.vertexOffsets[instanceIndex];
  mat4 transformMatrix = instanceDescriptionContainer.transformMatrix[instanceIndex];

  ivec3 indices = getIndicesFromPrimitive(instanceIndex, primitiveIndex);

  vertexA = (transformMatrix * vec4(vertexBuffer.data[3 * indices.x + 0 + vertexOffset], 
             vertexBuffer.data[3 * indices.x + 1 + vertexOffset], 
             vertexBuffer.data[3 * indices.x + 2 + vertexOffset], 
             1.0)).xyz;

  vertexB = (transformMatrix * vec4(vertexBuffer.data[3 * indices.y + 0 + vertexOffset], 
             vertexBuffer.data[3 * indices.y + 1 + vertexOffset], 
             vertexBuffer.data[3 * indices.y + 2 + vertexOffset], 
             1.0)).xyz;
  
  vertexC = (transformMatrix * vec4(vertexBuffer.data[3 * indices.z + 0 + vertexOffset], 
             vertexBuffer.data[3 * indices.z + 1 + vertexOffset], 
             vertexBuffer.data[3 * indices.z + 2 + vertexOffset], 
             1.0)).xyz;
}

ivec3 getNormalIndicesFromPrimitive(uint instanceIndex, uint primitiveIndex) {
  uint indexOffset = instanceDescriptionContainer.indexOffsets[instanceIndex];

  ivec3 indices = ivec3(normalIndexBuffer.data[3 * primitiveIndex + 0 + indexOffset], 
                        normalIndexBuffer.data[3 * primitiveIndex + 1 + indexOffset], 
                        normalIndexBuffer.data[3 * primitiveIndex + 2 + indexOffset]);

  return indices;
};

void getNormalFromIndices(uint instanceIndex, uint primitiveIndex, out vec3 normalA, out vec3 normalB, out vec3 normalC) {
  uint normalOffset = instanceDescriptionContainer.normalOffsets[instanceIndex];

  ivec3 indices = getNormalIndicesFromPrimitive(instanceIndex, primitiveIndex);

  normalA = vec4(normalBuffer.data[3 * indices.x + 0 + normalOffset], 
             normalBuffer.data[3 * indices.x + 1 + normalOffset], 
             normalBuffer.data[3 * indices.x + 2 + normalOffset], 
             1.0).xyz;

  normalB =  vec4(normalBuffer.data[3 * indices.y + 0 + normalOffset], 
             normalBuffer.data[3 * indices.y + 1 + normalOffset], 
             normalBuffer.data[3 * indices.y + 2 + normalOffset], 
             1.0).xyz;
  
  normalC =  vec4(normalBuffer.data[3 * indices.z + 0 + normalOffset], 
             normalBuffer.data[3 * indices.z + 1 + normalOffset], 
             normalBuffer.data[3 * indices.z + 2 + normalOffset], 
             1.0).xyz;
}

void main() {
  vec3 lightPosition = camera.position.xyz + vec3(0, 0, -5);
  
  int randomPrimitiveIndex = int(random(gl_FragCoord.xy, camera.frameCount) * float(lensProperties.lastLensElementPrimitiveCount));

  vec3 vertexA, vertexB, vertexC;
  getVertexFromIndices(lensProperties.lastLensElementInstanceIndex, randomPrimitiveIndex, vertexA, vertexB, vertexC);

  vec2 uv = vec2(random(gl_FragCoord.xy, camera.frameCount), random(gl_FragCoord.xy, camera.frameCount + 1));
  if (uv.x + uv.y > 1.0f) {
    uv.x = 1.0f - uv.x;
    uv.y = 1.0f - uv.y;
  }

  vec3 barycentric = vec3(1.0 - uv.x - uv.y, uv.x, uv.y);
  vec3 position = vertexA * barycentric.x + vertexB * barycentric.y + vertexC * barycentric.z;

  vec3 rayOrigin = lightPosition;
  vec3 rayDirection = position - lightPosition;

  outColor = vec4(rayDirection, 1.0);
}