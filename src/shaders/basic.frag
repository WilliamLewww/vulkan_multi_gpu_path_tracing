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
flat layout(location = 2) in uint rasterCollectionIndex;

layout(location = 0) out vec4 outColor;

layout(binding = 0, set = 0) uniform Camera {
  vec4 position;
  vec4 right;
  vec4 up;
  vec4 forward;

  uint apertureInstanceIndex;
  uint frameCount;
} camera;

layout(binding = 1, set = 0) uniform InstanceDescriptionContainer {
  uint instanceCount;
  uint vertexOffsets[8];
  uint normalOffsets[8];
  uint indexOffsets[8];
  uint materialIndexOffsets[8];
  uint materialOffsets[8];
  mat4 transformMatrix[8];
} instanceDescriptionContainer;

layout(binding = 3, set = 0) buffer CollectionIndexBuffer { uint data[]; } collectionIndexBuffer;
layout(binding = 4, set = 0) buffer CollectionOffsetBuffer { uint data[]; } collectionOffsetBuffer;

layout(binding = 5, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 7, set = 0, rgba32f) uniform image2D image;
layout(binding = 8, set = 0) buffer RayDirectionBuffer { float data[]; } rayDirectionBuffer;

layout(binding = 0, set = 1) buffer IndexBuffer { uint data[]; } indexBuffer;
layout(binding = 1, set = 1) buffer VertexBuffer { float data[]; } vertexBuffer;
layout(binding = 2, set = 1) buffer NormalIndexBuffer { uint data[]; } normalIndexBuffer;
layout(binding = 3, set = 1) buffer NormalBuffer { float data[]; } normalBuffer;
layout(binding = 4, set = 1) buffer MaterialIndexBuffer { uint data[]; } materialIndexBuffer;
layout(binding = 5, set = 1) buffer MaterialBuffer { Material data[]; } materialBuffer;
layout(binding = 6, set = 1) buffer MaterialLightBuffer { 
  uint count; 
  uint indicesPrimitive[64];
  uint indicesInstance[64];
} materialLightBuffer;

float random(vec2 uv, float seed) {
  return fract(sin(mod(dot(uv, vec2(12.9898, 78.233)) + 1113.1 * seed, M_PI)) * 43758.5453);;
}

Material getMaterialFromPrimitive(uint instanceIndex, uint primitiveIndex) {
  uint materialIndexOffset = instanceDescriptionContainer.materialIndexOffsets[instanceIndex];
  uint materialOffset = instanceDescriptionContainer.materialOffsets[instanceIndex];

  Material material = materialBuffer.data[materialIndexBuffer.data[primitiveIndex + materialIndexOffset] + materialOffset];

  return material;
}

void main() {
  vec3 directColor = vec3(0.0, 0.0, 0.0);

  int rayDirectionCoordinate = (int(gl_FragCoord.y) * 800 + int(gl_FragCoord.x)) * 3;

  vec3 rayOrigin = vec3(((gl_FragCoord.x / 800.0) - 0.5), ((gl_FragCoord.y / 600.0) - 0.5), 0.0) + camera.position.xyz;
  vec3 rayDirection = vec3(rayDirectionBuffer.data[rayDirectionCoordinate], rayDirectionBuffer.data[rayDirectionCoordinate + 1], rayDirectionBuffer.data[rayDirectionCoordinate + 2]);

  rayQueryEXT rayQuery;
  rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, rayOrigin, 0.0001f, rayDirection, 1000.0f);

  while (rayQueryProceedEXT(rayQuery));

  if (rayQueryGetIntersectionTypeEXT(rayQuery, true) != gl_RayQueryCommittedIntersectionNoneEXT) {
    int intersectionInstanceIndex = rayQueryGetIntersectionInstanceIdEXT(rayQuery, true);
    int intersectionPrimitiveIndex = rayQueryGetIntersectionPrimitiveIndexEXT(rayQuery, true);

    Material intersectionMaterial = getMaterialFromPrimitive(intersectionInstanceIndex, intersectionPrimitiveIndex);

    directColor = intersectionMaterial.diffuse;
  }
  
  vec4 color = vec4(directColor, 1.0);
  if (camera.frameCount > 0) {
    vec4 previousColor = imageLoad(image, ivec2(gl_FragCoord.xy));
    previousColor *= camera.frameCount;

    color += previousColor;
    color /= (camera.frameCount + 1);
  }
  imageStore(image, ivec2(gl_FragCoord.xy), color);
  outColor = color;
}