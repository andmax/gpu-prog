varying vec4 diffuse, ambient;
varying vec3 half_vector, normal, light_dir;

void main() {
    normal = gl_NormalMatrix * gl_Normal;
    light_dir = vec3(gl_LightSource[0].position);
    diffuse = gl_FrontMaterial.diffuse*gl_LightSource[0].diffuse;
    ambient = gl_FrontMaterial.ambient*gl_LightSource[0].ambient;
    ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;
    vec3 eye_dir = -vec3(gl_ModelViewMatrix * gl_Vertex); 
    half_vector = normalize(eye_dir) + normalize(light_dir);
    gl_Position = ftransform();
}
