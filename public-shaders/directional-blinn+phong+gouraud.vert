varying vec4 front_color;

void main() {
    vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
    vec3 light_dir = normalize(vec3(gl_LightSource[0].position));
    float NdotL = max(dot(normal, light_dir), 0.0);
    vec4 diffuse = gl_FrontMaterial.diffuse*gl_LightSource[0].diffuse;
    vec4 ambient = gl_FrontMaterial.ambient*gl_LightSource[0].ambient;
    vec4 global_ambient = gl_LightModel.ambient * gl_FrontMaterial.ambient;
    front_color = global_ambient + ambient;
    if (NdotL > 0.0) {
        front_color += NdotL * diffuse;
        vec3 eye_dir = -normalize(vec3(gl_ModelViewMatrix * gl_Vertex)); 
        vec3 half_vector = normalize(eye_dir + light_dir);
        float NdotHV = max(dot(normal, half_vector),0.0);
        vec4 specular = gl_FrontMaterial.specular * gl_LightSource[0].specular 
                * pow(NdotHV, gl_FrontMaterial.shininess);
        front_color += specular;
    }
    gl_Position = ftransform();
}
