#version 150 

uniform vec3 matColor;
uniform vec3 eyeDir;
uniform vec3 lightPos;
uniform vec3 eyePos;
uniform vec3 lightCol;

//these are the interpolated values out of the rasterizer, so you can't know
//their specific values without knowing the vertices that contributed to them
in vec3 fs_normal;
in vec3 fs_color;

out vec4 out_Color;

void main() {
    vec3 bigE = normalize(eyePos);
    vec3 bigN = normalize(fs_normal);
    vec3 bigL = normalize(lightPos);
    //vec3 bigH = normalize(bigL + eyePos);
    vec3 bigR = -normalize(reflect(bigE, bigL));

    float diffuseTerm = clamp(dot(bigN, bigL) * 0.7, 0.0, 1.0);
    float specularTerm = clamp(pow(clamp(dot(bigN, bigR), 0.0, 1.0), 25) * diffuseTerm, 0.0, 1.0); 

    vec4 diffuseColor = vec4(fs_color, 1.0);

    out_Color = clamp((
          (vec4(lightCol, 0.0) * diffuseColor * diffuseTerm * vec4(matColor, 0.0))
        + (vec4(lightCol, 0.0) * specularTerm * vec4(1.0, 1.0, 1.0, 0.0) * vec4(matColor, 0.0))
        + (vec4(0.02, 0.02, 0.02, 1.0))
      ), 0.0, 1.0);
}
