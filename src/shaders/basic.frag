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
  uint indexOffsets[8];
  uint materialIndexOffsets[8];
  uint materialOffsets[8];
  mat4 transformMatrix[8];
} instanceDescriptionContainer;

layout(binding = 2, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 3, set = 0, rgba32f) uniform image2D image;

layout(binding = 0, set = 1) buffer IndexBuffer { uint data[]; } indexBuffer;
layout(binding = 1, set = 1) buffer VertexBuffer { float data[]; } vertexBuffer;
layout(binding = 2, set = 1) buffer MaterialIndexBuffer { uint data[]; } materialIndexBuffer;
layout(binding = 3, set = 1) buffer MaterialBuffer { Material data[]; } materialBuffer;
layout(binding = 4, set = 1) buffer MaterialLightBuffer { 
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
  float cosi = clamp(-1, 1, dot(incidentDirection, normal)); 
  float etai = firstIOR, etat = secondIOR; 
  vec3 n = normal; 
  if (cosi < 0) { 
    cosi = -cosi; 
  } 
  else { 
    etai = secondIOR;
    etat = firstIOR;
    n= -normal; 
  } 
  float eta = etai / etat; 
  float k = 1 - eta * eta * (1 - cosi * cosi); 
  return k < 0 ? vec3(0) : eta * incidentDirection + (eta * cosi - sqrt(k)) * n; 
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
      rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsTerminateOnFirstHitEXT, 0xFF, position, 0.0001f, positionToLightDirection, shadowRayDistance);
  
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
        vec3 reflectedPositionToLightDirection = (2 * normal * dot(positionToLightDirection, normal)) - positionToLightDirection;

        vec3 diffuse = vec3(material.diffuse * lightMaterial.diffuse * dot(normal, positionToLightDirection));
        vec3 specular = vec3(material.specular * lightMaterial.specular * max(0, pow(dot(reflectedPositionToLightDirection, surfaceToCamera), material.shininess)));
        
        if (dot(positionToLightDirection, normal) > 0.0001) {
          if (intersectionMaterial.dissolve < 1.0) {
            vec3 directColor = lightMaterial.emission * lightAttenuation * lightIntensity * (diffuse + specular);
            color = (intersectionMaterial.diffuse * intersectionMaterial.dissolve) + (directColor * (1.0 - intersectionMaterial.dissolve));
          }
          else {
            color += lightMaterial.emission * lightAttenuation * lightIntensity * (diffuse + specular);
          }
        }
        else {
          if (intersectionMaterial.dissolve < 1.0) {
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
  vec3 color = vec3(0.0, 0.0, 0.0);

  vec3 cameraToSurface = normalize(position - camera.position.xyz);

  vec3 transmissionDirection = refract(cameraToSurface, normal, 1.0, material.ior);

  rayQueryEXT rayQuery;
  rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsTerminateOnFirstHitEXT, 0xFF, position, 0.0001f, transmissionDirection, 1000.0f);

  while (rayQueryProceedEXT(rayQuery));

  int intersectionInstanceIndex = rayQueryGetIntersectionInstanceIdEXT(rayQuery, true);
  int intersectionPrimitiveIndex = rayQueryGetIntersectionPrimitiveIndexEXT(rayQuery, true);

  vec3 intersectionVertexA, intersectionVertexB, intersectionVertexC;
  getVertexFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionVertexA, intersectionVertexB, intersectionVertexC);
  vec3 intersectionGeometricNormal = normalize(cross(intersectionVertexB - intersectionVertexA, intersectionVertexC - intersectionVertexA));

  vec2 intersectionUV = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);
  
  vec3 intersectionBarycentrics = vec3(1.0 - intersectionUV.x - intersectionUV.y, intersectionUV.x, intersectionUV.y);
  vec3 intersectionPosition = intersectionVertexA * intersectionBarycentrics.x + intersectionVertexB * intersectionBarycentrics.y + intersectionVertexC * intersectionBarycentrics.z;

  transmissionDirection = refract(transmissionDirection, intersectionGeometricNormal, material.ior, 1.0);

  rayQueryInitializeEXT(rayQuery, topLevelAS, gl_RayFlagsTerminateOnFirstHitEXT, 0xFF, intersectionPosition, 0.0001f, transmissionDirection, 1000.0f);

  while (rayQueryProceedEXT(rayQuery));

  if (rayQueryGetIntersectionTypeEXT(rayQuery, true) != gl_RayQueryCommittedIntersectionNoneEXT) {
    intersectionInstanceIndex = rayQueryGetIntersectionInstanceIdEXT(rayQuery, true);
    intersectionPrimitiveIndex = rayQueryGetIntersectionPrimitiveIndexEXT(rayQuery, true);

    getVertexFromIndices(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionVertexA, intersectionVertexB, intersectionVertexC);
    intersectionGeometricNormal = normalize(cross(intersectionVertexB - intersectionVertexA, intersectionVertexC - intersectionVertexA));
    Material intersectionMaterial = getMaterialFromPrimitive(intersectionInstanceIndex, intersectionPrimitiveIndex);

    intersectionUV = rayQueryGetIntersectionBarycentricsEXT(rayQuery, true);
  
    intersectionBarycentrics = vec3(1.0 - intersectionUV.x - intersectionUV.y, intersectionUV.x, intersectionUV.y);
    intersectionPosition = intersectionVertexA * intersectionBarycentrics.x + intersectionVertexB * intersectionBarycentrics.y + intersectionVertexC * intersectionBarycentrics.z;
  
    color = shade(intersectionInstanceIndex, intersectionPrimitiveIndex, intersectionPosition, intersectionGeometricNormal, intersectionMaterial);
  }

  return color;
}

void main() {
  vec3 directColor = vec3(0.0, 0.0, 0.0);

  vec3 vertexA, vertexB, vertexC;
  getVertexFromIndices(rasterInstanceIndex, gl_PrimitiveID, vertexA, vertexB, vertexC);
  vec3 geometricNormal = normalize(cross(vertexB - vertexA, vertexC - vertexA));
  Material rasterMaterial = getMaterialFromPrimitive(rasterInstanceIndex, gl_PrimitiveID);

  directColor = shade(rasterInstanceIndex, gl_PrimitiveID, interpolatedPosition, geometricNormal, rasterMaterial);

  if (rasterMaterial.dissolve < 1.0) {
    vec3 refractedDirectColor = shadeRefraction(interpolatedPosition, geometricNormal, rasterMaterial);

    directColor = (directColor * rasterMaterial.dissolve) + (refractedDirectColor * (1.0 - rasterMaterial.dissolve));
  }

  vec4 color = vec4(directColor, 1.0);
  if (camera.frameCount > 0) {
    vec4 previousColor = imageLoad(image, ivec2(gl_FragCoord.xy));
    previousColor *= camera.frameCount;

    color += previousColor;
    color /= (camera.frameCount + 1);
  }
  outColor = color;
}