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

  int diffuseTextureIndex;
  int type;
};

layout(early_fragment_tests) in;

layout(location = 0) in vec3 interpolatedPosition;
flat layout(location = 1) in uint rasterInstanceIndex;

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
layout(binding = 6, set = 0) buffer RayDirectionBuffer { float data[]; } rayDirectionBuffer;

layout(binding = 7, set = 0) buffer LensProperties {
  uint apertureInstanceIndex;
  uint aperturePrimitiveCount;
  uint aperturePrimitiveOffset;
} lensProperties;

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

void getBarycentricFromPoints(vec3 p, vec3 a, vec3 b, vec3 c, out float u, out float v, out float w) {
  vec3 v0 = b - a; 
  vec3 v1 = c - a; 
  vec3 v2 = p - a;
  float d00 = dot(v0, v0);
  float d01 = dot(v0, v1);
  float d11 = dot(v1, v1);
  float d20 = dot(v2, v0);
  float d21 = dot(v2, v1);
  float denom = d00 * d11 - d01 * d01;
  v = (d11 * d20 - d01 * d21) / denom;
  w = (d00 * d21 - d01 * d20) / denom;
  u = 1.0f - v - w;
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

Material getMaterialFromPrimitive(uint instanceIndex, uint primitiveIndex) {
  uint materialIndexOffset = instanceDescriptionContainer.materialIndexOffsets[instanceIndex];
  uint materialOffset = instanceDescriptionContainer.materialOffsets[instanceIndex];

  Material material = materialBuffer.data[materialIndexBuffer.data[primitiveIndex + materialIndexOffset] + materialOffset];

  return material;
}

float getLightAttenuation(float distance, float c, float l, float q) {
  return 1.0 / (c + (l * distance) + (q * distance * distance));
}

vec3 refract(vec3 incidentDirection, vec3 normal, float firstIOR, float secondIOR) {
  float n = firstIOR / secondIOR;
  float cosI = -dot(normal, incidentDirection);
  float sinT2 = n * n * (1.0 - cosI * cosI);
  float cosT = sqrt(1.0 - sinT2);
  return n * incidentDirection + (n * cosI - cosT) * normal;
}

float reflectance(vec3 incidentDirection, vec3 normal, float firstIOR, float secondIOR) {
  float n = firstIOR / secondIOR;
  float cosI = -dot(normal, incidentDirection);
  float sinT2 = n * n * (1.0 - cosI * cosI);
  if (sinT2 > 1.0) {
    return 1.0;
  }
  float cosT = sqrt(1.0 - sinT2);
  float rOrth = (firstIOR * cosI - secondIOR * cosT) / (firstIOR * cosI + secondIOR * cosT);
  float rPar = (secondIOR * cosI - firstIOR * cosT) / (secondIOR * cosI + firstIOR * cosT);
  return (rOrth * rOrth + rPar * rPar) / 2.0;
}

vec3 getRayDirectionFromLens(vec3 filmPosition) {
  int randomPrimitiveIndex = int(random(gl_FragCoord.xy, camera.frameCount) * float(lensProperties.aperturePrimitiveCount + lensProperties.aperturePrimitiveOffset));

  vec3 aperatureVertexA, aperatureVertexB, aperatureVertexC;
  getVertexFromIndices(lensProperties.apertureInstanceIndex, randomPrimitiveIndex, aperatureVertexA, aperatureVertexB, aperatureVertexC);

  vec2 uv = vec2(random(gl_FragCoord.xy, camera.frameCount), random(gl_FragCoord.xy, camera.frameCount + 1));
  if (uv.x + uv.y > 1.0f) {
    uv.x = 1.0f - uv.x;
    uv.y = 1.0f - uv.y;
  }
  vec3 aperatureBarycentric = vec3(1.0 - uv.x - uv.y, uv.x, uv.y);
  vec3 aperaturePosition = aperatureVertexA * aperatureBarycentric.x + aperatureVertexB * aperatureBarycentric.y + aperatureVertexC * aperatureBarycentric.z;

  bool isFilmIntersection = true;
  vec3 rayOrigin = filmPosition;
  vec3 rayDirection = normalize(aperaturePosition - filmPosition);

  rayQueryEXT rayQuery;
  rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, rayOrigin, 0.0001f, rayDirection, 1000.0f);

  while (rayQueryProceedEXT(rayQuery));

  int intersectionInstanceIndex, intersectionPrimitiveIndex;
  Material intersectionMaterial;
  vec3 intersectionVertexA, intersectionVertexB, intersectionVertexC;
  vec3 intersectionNormalA, intersectionNormalB, intersectionNormalC;

  vec2 intersectionUV;
  vec3 intersectionBarycentrics, intersectionPosition, intersectionNormal;

  bool isIntersection = rayQueryGetIntersectionTypeEXT(rayQuery, true) != gl_RayQueryCommittedIntersectionNoneEXT;
  while (isIntersection) {
    intersectionInstanceIndex = rayQueryGetIntersectionInstanceIdEXT(rayQuery, true);
    intersectionPrimitiveIndex = rayQueryGetIntersectionPrimitiveIndexEXT(rayQuery, true);

    intersectionMaterial = getMaterialFromPrimitive(intersectionInstanceIndex, intersectionPrimitiveIndex);
    
    getVertexFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionVertexA, intersectionVertexB, intersectionVertexC);
    getNormalFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionNormalA, intersectionNormalB, intersectionNormalC);

    intersectionUV = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);
    intersectionBarycentrics = vec3(1.0 - intersectionUV.x - intersectionUV.y, intersectionUV.x, intersectionUV.y);
    intersectionPosition = intersectionVertexA * intersectionBarycentrics.x + intersectionVertexB * intersectionBarycentrics.y + intersectionVertexC * intersectionBarycentrics.z;
    intersectionNormal = intersectionNormalA * intersectionBarycentrics.x + intersectionNormalB * intersectionBarycentrics.y + intersectionNormalC * intersectionBarycentrics.z;

    rayOrigin = intersectionPosition;

    bool hitAperture = (intersectionInstanceIndex == lensProperties.apertureInstanceIndex) && (intersectionPrimitiveIndex >= lensProperties.aperturePrimitiveOffset) && (intersectionPrimitiveIndex < lensProperties.aperturePrimitiveOffset + lensProperties.aperturePrimitiveCount);

    if (intersectionMaterial.dissolve < 1.0 && !hitAperture) {
      rayDirection = refract(rayDirection, intersectionNormal, 1.0, intersectionMaterial.ior);

      rayQueryEXT rayQuery;
      rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, rayOrigin, 0.0001f, rayDirection, 1000.0f);

      while (rayQueryProceedEXT(rayQuery));

      intersectionInstanceIndex = rayQueryGetIntersectionInstanceIdEXT(rayQuery, true);
      intersectionPrimitiveIndex = rayQueryGetIntersectionPrimitiveIndexEXT(rayQuery, true);

      getVertexFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionVertexA, intersectionVertexB, intersectionVertexC);
      getNormalFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionNormalA, intersectionNormalB, intersectionNormalC);

      intersectionUV = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);
      intersectionBarycentrics = vec3(1.0 - intersectionUV.x - intersectionUV.y, intersectionUV.x, intersectionUV.y);
      intersectionPosition = intersectionVertexA * intersectionBarycentrics.x + intersectionVertexB * intersectionBarycentrics.y + intersectionVertexC * intersectionBarycentrics.z;
      intersectionNormal = intersectionNormalA * intersectionBarycentrics.x + intersectionNormalB * intersectionBarycentrics.y + intersectionNormalC * intersectionBarycentrics.z;

      rayOrigin = intersectionPosition;
      rayDirection = refract(rayDirection, -intersectionNormal, intersectionMaterial.ior, 1.0);
    }
    else {
      if (!hitAperture && !isFilmIntersection) {
        return vec3(0, 0, 0);
      }
    }

    isFilmIntersection = false;

    rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, rayOrigin, 0.0001f, rayDirection, 1000.0f);
    while (rayQueryProceedEXT(rayQuery));
    isIntersection = rayQueryGetIntersectionTypeEXT(rayQuery, true) != gl_RayQueryCommittedIntersectionNoneEXT;
  }

  return rayDirection;
}

void main() {
  vec3 filmPosition = vec3(-((gl_FragCoord.x / 800.0) - 0.5), ((gl_FragCoord.y / 600.0) - 0.5), 0.0);
  int rayDirectionCoordinate = (int(gl_FragCoord.y) * 800 + int(gl_FragCoord.x)) * 3;
  vec3 lensDirection = getRayDirectionFromLens(filmPosition);

  rayDirectionBuffer.data[rayDirectionCoordinate + 0] = lensDirection.x;
  rayDirectionBuffer.data[rayDirectionCoordinate + 1] = lensDirection.y;
  rayDirectionBuffer.data[rayDirectionCoordinate + 2] = lensDirection.z;
}