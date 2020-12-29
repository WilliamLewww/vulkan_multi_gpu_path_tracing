#version 460

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec3 interpolatedPosition[];
flat layout(location = 1) in uint rasterInstanceIndex[];

layout(location = 0) out vec3 interpolatedPositionOut;
layout(location = 1) out vec3 interpolatedNormalOut;
flat layout(location = 2) out uint rasterInstanceIndexOut;

void main() {
  for (int x = 0; x < gl_in.length(); x++) {
    interpolatedPositionOut = interpolatedPosition[x];
    interpolatedNormalOut = vec3(gl_PrimitiveIDIn);
    rasterInstanceIndexOut = rasterInstanceIndex[x];
    gl_Position = gl_in[x].gl_Position;
    EmitVertex();
  }
  EndPrimitive();
}