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

layout(location = 0) out vec4 outColor;

layout(binding = 0, set = 0) uniform Camera {
  vec4 position;
  vec4 right;
  vec4 up;
  vec4 forward;

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

layout(binding = 2, set = 0) buffer CollectionIndexBuffer { uint data[]; } collectionIndexBuffer;

layout(binding = 3, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 4, set = 0, rgba32f) uniform image2D image;

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

vec3 uniformSampleHemisphere(vec2 uv) {
  float z = uv.x;
  float r = sqrt(max(0, 1.0 - z * z));
  float phi = 2.0 * M_PI * uv.y;

  return vec3(r * cos(phi), z, r * sin(phi));
}

vec3 alignHemisphereWithCoordinateSystem(vec3 hemisphere, vec3 up) {
  vec3 right = normalize(cross(up, vec3(0.0072f, 1.0f, 0.0034f)));
  vec3 forward = cross(right, up);

  return hemisphere.x * right + hemisphere.y * up + hemisphere.z * forward;
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

vec3 shade(uint instanceIndex, uint primitiveIndex, vec3 position, vec3 normal, Material material) {
  vec3 color = vec3(0.0, 0.0, 0.0);

  if (dot(material.emission, material.emission) > 0) {
    color = material.emission;
  }
  else {
    for (int x = 0; x < materialLightBuffer.count; x++) {
      uint lightInstanceIndex = materialLightBuffer.indicesInstance[x];
      uint lightPrimitiveIndex = materialLightBuffer.indicesPrimitive[x];

      vec3 lightVertexA, lightVertexB, lightVertexC;
      getVertexFromIndices(lightInstanceIndex, lightPrimitiveIndex, lightVertexA, lightVertexB, lightVertexC);

      vec2 uv = vec2(random(gl_FragCoord.xy, camera.frameCount), random(gl_FragCoord.xy, camera.frameCount + 1));
      if (uv.x + uv.y > 1.0f) {
        uv.x = 1.0f - uv.x;
        uv.y = 1.0f - uv.y;
      }

      vec3 lightBarycentric = vec3(1.0 - uv.x - uv.y, uv.x, uv.y);
      vec3 lightPosition = lightVertexA * lightBarycentric.x + lightVertexB * lightBarycentric.y + lightVertexC * lightBarycentric.z;
      vec3 positionToLightDirection = normalize(lightPosition - position);
      
      float shadowRayDistance = length(lightPosition - position) - 0.0001f;

      rayQueryEXT rayQuery;
      rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, position, 0.0001f, positionToLightDirection, shadowRayDistance);
  
      while (rayQueryProceedEXT(rayQuery));

      int intersectionInstanceIndex = rayQueryGetIntersectionInstanceIdEXT(rayQuery, true);
      int intersectionPrimitiveIndex = rayQueryGetIntersectionPrimitiveIndexEXT(rayQuery, true);

      Material intersectionMaterial = getMaterialFromPrimitive(intersectionInstanceIndex, intersectionPrimitiveIndex);

      if (rayQueryGetIntersectionTypeEXT(rayQuery, true) == gl_RayQueryCommittedIntersectionNoneEXT || intersectionMaterial.dissolve < 1.0) {
        Material lightMaterial = getMaterialFromPrimitive(lightInstanceIndex, lightPrimitiveIndex);
        float lightArea = length(cross(lightVertexB - lightVertexA, lightVertexC - lightVertexA)) * 0.5;
        float lightIntensity = sqrt(lightArea);
        float lightAttenuation = getLightAttenuation(length(lightPosition - position), 1, 0.05, 0.03);

        vec3 surfaceToCamera = normalize(camera.position.xyz - position);
        vec3 reflectedPositionToLightDirection = reflect(-positionToLightDirection, normal);

        vec3 diffuse = vec3(material.diffuse * lightMaterial.diffuse * dot(normal, positionToLightDirection));
        vec3 specular = vec3(material.specular * lightMaterial.specular * max(0, pow(dot(reflectedPositionToLightDirection, surfaceToCamera), material.shininess)));
        
        if (dot(positionToLightDirection, normal) > 0.0001) {
          if (intersectionMaterial.dissolve < 1.0) {
            vec3 directColor = lightMaterial.emission * lightAttenuation * lightIntensity * (diffuse + specular);
            color = (intersectionMaterial.diffuse * directColor);
          }
          else {
            color += lightMaterial.emission * lightAttenuation * lightIntensity * (diffuse + specular);
          }
        }
        else {
          if (dot(positionToLightDirection, normal) < -0.0001 && intersectionMaterial.dissolve < 1.0) {
            diffuse = vec3(material.diffuse * lightMaterial.diffuse * dot(-normal, positionToLightDirection));
            color += lightMaterial.emission * lightAttenuation * lightIntensity * diffuse;
          }
        }
      }
      else {
        bool intersectionIsLight = false;
        for (int y = 0; y < materialLightBuffer.count; y++) {
          if (intersectionInstanceIndex == materialLightBuffer.indicesInstance[y] && intersectionPrimitiveIndex == materialLightBuffer.indicesPrimitive[y]) {
            intersectionIsLight = true;
          }
        }

        if (intersectionIsLight) {
          getVertexFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, lightVertexA, lightVertexB, lightVertexC);

          uv = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);
          
          lightBarycentric = vec3(1.0 - uv.x - uv.y, uv.x, uv.y);
          lightPosition = lightVertexA * lightBarycentric.x + lightVertexB * lightBarycentric.y + lightVertexC * lightBarycentric.z;
          positionToLightDirection = normalize(lightPosition - position);

          if (dot(positionToLightDirection, normal) > 0.0001) {
            Material lightMaterial = getMaterialFromPrimitive(lightInstanceIndex, lightPrimitiveIndex);
            float lightArea = length(cross(lightVertexB - lightVertexA, lightVertexC - lightVertexA)) * 0.5;
            float lightIntensity = sqrt(lightArea);
            float lightAttenuation = getLightAttenuation(length(lightPosition - position), 1, 0.05, 0.03);
            
            vec3 surfaceToCamera = normalize(camera.position.xyz - position);
            vec3 reflectedPositionToLightDirection = (2 * normal * dot(positionToLightDirection, normal)) - positionToLightDirection;

            vec3 diffuse = vec3(material.diffuse * lightMaterial.diffuse * dot(normal, positionToLightDirection));
            vec3 specular = vec3(material.specular * lightMaterial.specular * max(0, pow(dot(reflectedPositionToLightDirection, surfaceToCamera), material.shininess)));
            color += lightMaterial.emission * lightAttenuation * lightIntensity * (diffuse + specular);
          }
        }
      }
    }
  }

  return color;
}

vec3 shadeRefraction(vec3 position, vec3 normal, Material material) {
  vec3 cameraToSurface = normalize(position - camera.position.xyz);

  float transmittance = 1.0 - reflectance(cameraToSurface, normal, 1.0, material.ior);
  vec3 transmissionDirection = refract(cameraToSurface, normal, 1.0, material.ior);

  rayQueryEXT rayQuery;
  rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, position, 0.0001f, transmissionDirection, 1000.0f);

  while (rayQueryProceedEXT(rayQuery));

  int intersectionInstanceIndex = rayQueryGetIntersectionInstanceIdEXT(rayQuery, true);
  int intersectionPrimitiveIndex = rayQueryGetIntersectionPrimitiveIndexEXT(rayQuery, true);

  vec3 intersectionVertexA, intersectionVertexB, intersectionVertexC;
  getVertexFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionVertexA, intersectionVertexB, intersectionVertexC);

  vec3 intersectionNormalA, intersectionNormalB, intersectionNormalC;
  getNormalFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionNormalA, intersectionNormalB, intersectionNormalC);

  vec2 intersectionUV = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);
  vec3 intersectionBarycentrics = vec3(1.0 - intersectionUV.x - intersectionUV.y, intersectionUV.x, intersectionUV.y);
  vec3 intersectionPosition = intersectionVertexA * intersectionBarycentrics.x + intersectionVertexB * intersectionBarycentrics.y + intersectionVertexC * intersectionBarycentrics.z;
  vec3 intersectionNormal = intersectionNormalA * intersectionBarycentrics.x + intersectionNormalB * intersectionBarycentrics.y + intersectionNormalC * intersectionBarycentrics.z;

  transmissionDirection = refract(transmissionDirection, -intersectionNormal, material.ior, 1.0);

  rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, intersectionPosition, 0.0001f, transmissionDirection, 1000.0f);

  while (rayQueryProceedEXT(rayQuery));

  bool isIntersection = rayQueryGetIntersectionTypeEXT(rayQuery, true) != gl_RayQueryCommittedIntersectionNoneEXT;
  if (!isIntersection) {
    return vec3(0.0, 0.0, 0.0);
  }
  while (isIntersection) {
    intersectionInstanceIndex = rayQueryGetIntersectionInstanceIdEXT(rayQuery, true);
    intersectionPrimitiveIndex = rayQueryGetIntersectionPrimitiveIndexEXT(rayQuery, true);

    Material intersectionMaterial = getMaterialFromPrimitive(intersectionInstanceIndex, intersectionPrimitiveIndex);

    getVertexFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionVertexA, intersectionVertexB, intersectionVertexC);
    getNormalFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionNormalA, intersectionNormalB, intersectionNormalC);

    intersectionUV = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);
    intersectionBarycentrics = vec3(1.0 - intersectionUV.x - intersectionUV.y, intersectionUV.x, intersectionUV.y);
    intersectionPosition = intersectionVertexA * intersectionBarycentrics.x + intersectionVertexB * intersectionBarycentrics.y + intersectionVertexC * intersectionBarycentrics.z;
    intersectionNormal = intersectionNormalA * intersectionBarycentrics.x + intersectionNormalB * intersectionBarycentrics.y + intersectionNormalC * intersectionBarycentrics.z;

    if (intersectionMaterial.dissolve < 1.0) {
      transmissionDirection = refract(transmissionDirection, intersectionNormal, 1.0, intersectionMaterial.ior);

      rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, intersectionPosition, 0.0001f, transmissionDirection, 1000.0f);

      while (rayQueryProceedEXT(rayQuery));

      intersectionInstanceIndex = rayQueryGetIntersectionInstanceIdEXT(rayQuery, true);
      intersectionPrimitiveIndex = rayQueryGetIntersectionPrimitiveIndexEXT(rayQuery, true);

      getVertexFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionVertexA, intersectionVertexB, intersectionVertexC);
      getNormalFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionNormalA, intersectionNormalB, intersectionNormalC);

      intersectionUV = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);
      intersectionBarycentrics = vec3(1.0 - intersectionUV.x - intersectionUV.y, intersectionUV.x, intersectionUV.y);
      intersectionPosition = intersectionVertexA * intersectionBarycentrics.x + intersectionVertexB * intersectionBarycentrics.y + intersectionVertexC * intersectionBarycentrics.z;
      intersectionNormal = intersectionNormalA * intersectionBarycentrics.x + intersectionNormalB * intersectionBarycentrics.y + intersectionNormalC * intersectionBarycentrics.z;

      transmissionDirection = refract(transmissionDirection, -intersectionNormal, intersectionMaterial.ior, 1.0);

      rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, intersectionPosition, 0.0001f, transmissionDirection, 1000.0f);

      while (rayQueryProceedEXT(rayQuery));
    }
    else {
      return transmittance * shade(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionPosition, intersectionNormal, intersectionMaterial);
    }
  }
}

vec3 shadeReflection(vec3 position, vec3 normal, Material material) {
  vec3 color = vec3(0.0, 0.0, 0.0);

  vec3 cameraToSurface = normalize(position - camera.position.xyz);

  float reflectivity = reflectance(cameraToSurface, normal, 1.0, material.ior);
  vec3 reflectionDirection = vec3(reflect(cameraToSurface, normal));

  rayQueryEXT rayQuery;
  rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsNoneEXT, 0xFF, position, 0.0001f, reflectionDirection, 1000.0f);

  while (rayQueryProceedEXT(rayQuery));

  if (rayQueryGetIntersectionTypeEXT(rayQuery, true) != gl_RayQueryCommittedIntersectionNoneEXT) {
    int intersectionInstanceIndex = rayQueryGetIntersectionInstanceIdEXT(rayQuery, true);
    int intersectionPrimitiveIndex = rayQueryGetIntersectionPrimitiveIndexEXT(rayQuery, true);

    Material intersectionMaterial = getMaterialFromPrimitive(intersectionInstanceIndex, intersectionPrimitiveIndex);

    vec3 intersectionVertexA, intersectionVertexB, intersectionVertexC;
    getVertexFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionVertexA, intersectionVertexB, intersectionVertexC);

    vec3 intersectionNormalA, intersectionNormalB, intersectionNormalC;
    getNormalFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionNormalA, intersectionNormalB, intersectionNormalC);

    vec2 intersectionUV = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);
    vec3 intersectionBarycentrics = vec3(1.0 - intersectionUV.x - intersectionUV.y, intersectionUV.x, intersectionUV.y);
    vec3 intersectionPosition = intersectionVertexA * intersectionBarycentrics.x + intersectionVertexB * intersectionBarycentrics.y + intersectionVertexC * intersectionBarycentrics.z;
    vec3 intersectionNormal = intersectionNormalA * intersectionBarycentrics.x + intersectionNormalB * intersectionBarycentrics.y + intersectionNormalC * intersectionBarycentrics.z;

    color = reflectivity * shade(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionPosition, intersectionNormal, intersectionMaterial);
  }

  return color;
}

void main() {
  vec3 directColor = vec3(0.0, 0.0, 0.0);

  Material rasterMaterial = getMaterialFromPrimitive(rasterInstanceIndex, gl_PrimitiveID);

  vec3 vertexA, vertexB, vertexC;
  getVertexFromIndices(rasterInstanceIndex, gl_PrimitiveID, vertexA, vertexB, vertexC);

  float u, v, w;
  getBarycentricFromPoints(interpolatedPosition, vertexA, vertexB, vertexC, u, v, w);

  vec3 normalA, normalB, normalC;
  getNormalFromIndices(rasterInstanceIndex, gl_PrimitiveID, normalA, normalB, normalC);

  vec3 interpolatedNormal = normalA * u + normalB * v + normalC * w;

  directColor = shade(rasterInstanceIndex, gl_PrimitiveID, interpolatedPosition, interpolatedNormal, rasterMaterial);

  if (rasterMaterial.dissolve < 1.0) {
    vec3 refractedColor = shadeRefraction(interpolatedPosition, interpolatedNormal, rasterMaterial);
    vec3 reflectedColor = shadeReflection(interpolatedPosition, interpolatedNormal, rasterMaterial);

    directColor = directColor + refractedColor + reflectedColor;
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