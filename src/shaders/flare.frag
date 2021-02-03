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
layout(binding = 11, set = 0) buffer RandomBuffer { float data[]; } randomBuffer;

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

float random(vec2 uv) {
  return fract(sin(mod(dot(uv, vec2(12.9898, 78.233)) + 1113.1, M_PI)) * 43758.5453);;
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

void main() {
  vec3 lightPosition = camera.position.xyz + vec3(0, 0, -10);
  
  int randomPrimitiveIndex = int(random(gl_FragCoord.xy + vec2(0, camera.frameCount)) * float(lensProperties.lastLensElementPrimitiveCount));

  vec3 vertexA, vertexB, vertexC;
  getVertexFromIndices(lensProperties.lastLensElementInstanceIndex, randomPrimitiveIndex, vertexA, vertexB, vertexC);

  vec2 uv = vec2(random(gl_FragCoord.xy + vec2(0, camera.frameCount + 1)), random(gl_FragCoord.xy + vec2(0, camera.frameCount + 2)));
  if (uv.x + uv.y > 1.0f) {
    uv.x = 1.0f - uv.x;
    uv.y = 1.0f - uv.y;
  }

  vec3 barycentric = vec3(1.0 - uv.x - uv.y, uv.x, uv.y);
  vec3 position = vertexA * barycentric.x + vertexB * barycentric.y + vertexC * barycentric.z;

  vec3 rayOrigin = lightPosition;
  vec3 rayDirection = position - lightPosition;

  // light -> lens L
  rayQueryEXT rayQuery;
  rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, rayOrigin, 0.0001f, rayDirection, 1000.0f);
  while (rayQueryProceedEXT(rayQuery));

  int intersectionInstanceIndex = rayQueryGetIntersectionInstanceIdEXT(rayQuery, true);
  int intersectionPrimitiveIndex = rayQueryGetIntersectionPrimitiveIndexEXT(rayQuery, true);
  Material intersectionMaterial;
  vec3 intersectionVertexA, intersectionVertexB, intersectionVertexC;
  vec3 intersectionNormalA, intersectionNormalB, intersectionNormalC;

  vec2 intersectionUV;
  vec3 intersectionBarycentrics, intersectionPosition, intersectionNormal;

  getVertexFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionVertexA, intersectionVertexB, intersectionVertexC);
  getNormalFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionNormalA, intersectionNormalB, intersectionNormalC);

  intersectionUV = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);
  intersectionBarycentrics = vec3(1.0 - intersectionUV.x - intersectionUV.y, intersectionUV.x, intersectionUV.y);
  intersectionPosition = intersectionVertexA * intersectionBarycentrics.x + intersectionVertexB * intersectionBarycentrics.y + intersectionVertexC * intersectionBarycentrics.z;

  if (intersectionInstanceIndex == lensProperties.lastLensElementInstanceIndex) {
    bool isActive = true;
    int reflectionCount = 0;
    float intensity = 1.0;

    // int coordinateX = int(((intersectionPosition.x + 1.0) / 2.0) * 800);
    // int coordinateY = int(((intersectionPosition.y + 1.0) / 2.0) * 600) * 800;
    // flareBuffer.data[coordinateY + coordinateX] = 1.0;

    while (isActive) {
      intersectionInstanceIndex = rayQueryGetIntersectionInstanceIdEXT(rayQuery, true);
      intersectionPrimitiveIndex = rayQueryGetIntersectionPrimitiveIndexEXT(rayQuery, true);
      intersectionMaterial = getMaterialFromPrimitive(intersectionInstanceIndex, intersectionPrimitiveIndex);

      bool isIntersection = rayQueryGetIntersectionTypeEXT(rayQuery, true) != gl_RayQueryCommittedIntersectionNoneEXT;
      bool hitAperture = (intersectionInstanceIndex == lensProperties.apertureInstanceIndex) && (intersectionPrimitiveIndex >= lensProperties.aperturePrimitiveOffset) && (intersectionPrimitiveIndex < lensProperties.aperturePrimitiveOffset + lensProperties.aperturePrimitiveCount);
      
      if (hitAperture) {
        getVertexFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionVertexA, intersectionVertexB, intersectionVertexC);
        getNormalFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionNormalA, intersectionNormalB, intersectionNormalC);

        intersectionUV = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);
        intersectionBarycentrics = vec3(1.0 - intersectionUV.x - intersectionUV.y, intersectionUV.x, intersectionUV.y);
        intersectionPosition = intersectionVertexA * intersectionBarycentrics.x + intersectionVertexB * intersectionBarycentrics.y + intersectionVertexC * intersectionBarycentrics.z;
      
        rayOrigin = intersectionPosition;

        // aperture -> lens L
        rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, rayOrigin, 0.0001f, rayDirection, 1000.0f);
        while (rayQueryProceedEXT(rayQuery));
      }
      else {
        if (intersectionMaterial.dissolve == 1.0 || !isIntersection) {
          if (intersectionInstanceIndex == lensProperties.filmInstanceIndex) {
            getVertexFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionVertexA, intersectionVertexB, intersectionVertexC);

            intersectionUV = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);
            intersectionBarycentrics = vec3(1.0 - intersectionUV.x - intersectionUV.y, intersectionUV.x, intersectionUV.y);
            intersectionPosition = intersectionVertexA * intersectionBarycentrics.x + intersectionVertexB * intersectionBarycentrics.y + intersectionVertexC * intersectionBarycentrics.z;

            int coordinateX = int(((intersectionPosition.x + 1.0) / 2.0) * 800);
            int coordinateY = int(((intersectionPosition.y + 1.0) / 2.0) * 600) * 800;
            flareBuffer.data[coordinateY + coordinateX] = intensity;
          }

          isActive = false;
        }
        else {
          getVertexFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionVertexA, intersectionVertexB, intersectionVertexC);
          getNormalFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionNormalA, intersectionNormalB, intersectionNormalC);

          intersectionUV = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);
          intersectionBarycentrics = vec3(1.0 - intersectionUV.x - intersectionUV.y, intersectionUV.x, intersectionUV.y);
          intersectionPosition = intersectionVertexA * intersectionBarycentrics.x + intersectionVertexB * intersectionBarycentrics.y + intersectionVertexC * intersectionBarycentrics.z;
          intersectionNormal = intersectionNormalA * intersectionBarycentrics.x + intersectionNormalB * intersectionBarycentrics.y + intersectionNormalC * intersectionBarycentrics.z;
        
          rayOrigin = intersectionPosition;
          rayDirection = refract(rayDirection, intersectionNormal, 1.0, intersectionMaterial.ior);

          if (reflectionCount < 2) {
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
            rayDirection = reflect(rayDirection, -intersectionNormal);
            intensity *= reflectance(rayDirection, -intersectionNormal, intersectionMaterial.ior, intersectionMaterial.ior);

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
            rayDirection = reflect(rayDirection, -intersectionNormal);
            intensity *= reflectance(rayDirection, -intersectionNormal, intersectionMaterial.ior, intersectionMaterial.ior);

            reflectionCount += 1;
          }

          // lens L -> lens R
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

          // lens R -> lens L | aperture
          rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, rayOrigin, 0.0001f, rayDirection, 1000.0f);
          while (rayQueryProceedEXT(rayQuery));
        }
      }
    }
  }
}