varying vec4 diffuse, ambient;
varying vec3 half_vector, normal, light_dir;

void main() {
    normal = normalize(gl_NormalMatrix * gl_Normal);
    light_dir = normalize(vec3(gl_LightSource[0].position));
    float NdotL = max(dot(normal, light_dir), 0.0);
    diffuse = gl_FrontMaterial.diffuse*gl_LightSource[0].diffuse;
    ambient = gl_FrontMaterial.ambient*gl_LightSource[0].ambient;
    ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;
    vec3 eye_dir = -normalize(vec3(gl_ModelViewMatrix * gl_Vertex)); 
    half_vector = normalize(eye_dir + light_dir);
    gl_Position = ftransform();
}
