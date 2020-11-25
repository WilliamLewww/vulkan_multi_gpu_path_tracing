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
};

layout(location = 0) in vec3 interpolatedPosition;
flat layout(location = 1) in uint instanceIndex;

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
layout(binding = 4, set = 1) buffer MaterialLightBuffer { int count; int indices[]; } materialLightBuffer;

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

void main() {
  vec3 directColor = vec3(0.0, 0.0, 0.0);

  uint indexOffset = instanceDescriptionContainer.indexOffsets[instanceIndex];
  uint vertexOffset = instanceDescriptionContainer.vertexOffsets[instanceIndex];
  uint materialIndexOffset = instanceDescriptionContainer.materialIndexOffsets[instanceIndex];
  uint materialOffset = instanceDescriptionContainer.materialOffsets[instanceIndex];
  mat4 transformMatrix = instanceDescriptionContainer.transformMatrix[instanceIndex];

  ivec3 indices = ivec3(indexBuffer.data[3 * gl_PrimitiveID + 0 + indexOffset], indexBuffer.data[3 * gl_PrimitiveID + 1 + indexOffset], indexBuffer.data[3 * gl_PrimitiveID + 2 + indexOffset]);

  vec3 vertexA = (transformMatrix * vec4(vertexBuffer.data[3 * indices.x + 0 + vertexOffset], vertexBuffer.data[3 * indices.x + 1 + vertexOffset], vertexBuffer.data[3 * indices.x + 2 + vertexOffset], 1.0)).xyz;
  vec3 vertexB = (transformMatrix * vec4(vertexBuffer.data[3 * indices.y + 0 + vertexOffset], vertexBuffer.data[3 * indices.y + 1 + vertexOffset], vertexBuffer.data[3 * indices.y + 2 + vertexOffset], 1.0)).xyz;
  vec3 vertexC = (transformMatrix * vec4(vertexBuffer.data[3 * indices.z + 0 + vertexOffset], vertexBuffer.data[3 * indices.z + 1 + vertexOffset], vertexBuffer.data[3 * indices.z + 2 + vertexOffset], 1.0)).xyz;
  
  vec3 geometricNormal = normalize(cross(vertexB - vertexA, vertexC - vertexA));

  vec3 surfaceColor = materialBuffer.data[materialIndexBuffer.data[gl_PrimitiveID + materialIndexOffset] + materialOffset].diffuse;

  vec4 color = vec4(surfaceColor, 1.0);
  if (camera.frameCount > 0) {
    vec4 previousColor = imageLoad(image, ivec2(gl_FragCoord.xy));
    previousColor *= camera.frameCount;

    color += previousColor;
    color /= (camera.frameCount + 1);
  }
  outColor = color;
}
