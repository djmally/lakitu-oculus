#version 150

//there's no rules about the prefixes but they do help
//keep track of things
uniform mat4 transMat;
uniform mat4 camMat;
uniform vec3 lightPos;

in vec4 vs_position;
in vec3 vs_color;
in vec4 vs_normal;

out vec3 fs_color;
out vec3 fs_normal;

void main() { 
    fs_color = vs_color;

    fs_normal = normalize((transpose(inverse(transMat)) * normalize(vs_normal)).xyz);
    
    gl_Position = camMat * transMat * vs_position;
}
