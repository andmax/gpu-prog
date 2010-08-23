varying vec4 diffuse, ambient, specular;
varying vec3 half_vector, light_dir;

void main(void) {
	gl_TexCoord[0] = gl_MultiTexCoord0; 
	vec3 tangent = normalize(gl_NormalMatrix * vec3(gl_MultiTexCoord1)); 
	vec3 bitangent = normalize(gl_NormalMatrix * vec3(gl_MultiTexCoord2));
	vec3 normal = normalize(gl_NormalMatrix * gl_Normal); 
    mat3 tbn = mat3(tangent, bitangent, normal);
	light_dir = vec3(gl_LightSource[0].position)*tbn;
	vec3 eye_dir = -(vec3(gl_ModelViewMatrix * gl_Vertex)*tbn); 
    half_vector = (normalize(light_dir) + normalize(eye_dir));
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    diffuse = gl_FrontMaterial.diffuse*gl_LightSource[0].diffuse;
    ambient = gl_FrontMaterial.ambient*gl_LightSource[0].ambient;
    ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;
    specular = gl_FrontMaterial.specular * gl_LightSource[0].specular; 
}
