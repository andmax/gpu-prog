varying vec4 front_color;

void main() {
    vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
    vec3 light_dir = normalize(vec3(gl_LightSource[0].position));
    float NdotL = max(dot(normal, light_dir), 0.0);
    vec4 diffuse = gl_FrontMaterial.diffuse*gl_LightSource[0].diffuse;
    vec4 ambient = gl_FrontMaterial.ambient*gl_LightSource[0].ambient;
    vec4 global_ambient = gl_LightModel.ambient * gl_FrontMaterial.ambient;
    front_color =  NdotL * diffuse + global_ambient + ambient;
    gl_Position = ftransform();
}
