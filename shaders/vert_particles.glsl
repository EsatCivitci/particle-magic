#version 430 core

layout(location = 0) in vec4 aPos;

//uniform samplerBuffer positions;  
uniform mat4 mvp;
uniform int pointSize;

out float intensity;

void main() {
    //vec4 pos = texelFetch(positions, gl_VertexID); 
    intensity = aPos.w;
    gl_Position = mvp * aPos;
    gl_PointSize = pointSize;
}