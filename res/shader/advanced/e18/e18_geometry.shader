#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

in VS_OUT {
    vec3 normal;
    vec3 color;
} gs_in[];

out vec3 fColor;

const float MAGNITUDE = 0.4;

void GenerateLine(int index)
{
    fColor = gs_in[index].color;
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    fColor = gs_in[index].color;
    gl_Position = projection * (gl_in[index].gl_Position + 
                                vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // 第一个顶点法线
    GenerateLine(1); // 第二个顶点法线
    GenerateLine(2); // 第三个顶点法线
}