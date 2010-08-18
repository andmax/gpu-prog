varying vec4 front_color;

void main() {
    vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
    vec3 light_dir = normalize(vec3(gl_LightSource[0].position));
    float NdotL = max(dot(normal, light_dir), 0.0);
    vec4 diffuse = gl_FrontMaterial.diffuse*gl_LightSource[0].diffuse;
    front_color =  NdotL * diffuse;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
}
