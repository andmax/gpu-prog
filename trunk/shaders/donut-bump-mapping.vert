varying vec4 diffuse, ambient;
varying vec3 half_vector, normal, light_dir;

void main(void) {
	gl_TexCoord[0] = gl_MultiTexCoord0; 
	vec3 tangent = normalize(gl_NormalMatrix * vec3(gl_MultiTexCoord1)); 
	vec3 bitangent = normalize(gl_NormalMatrix * vec3(gl_MultiTexCoord2));
	normal = normalize(gl_NormalMatrix * gl_Normal); 
    mat3 tbn = mat3(tangent, bitangent, normal);
    normal = normal * tbn;
    diffuse = gl_FrontMaterial.diffuse*gl_LightSource[0].diffuse;
    ambient = gl_FrontMaterial.ambient*gl_LightSource[0].ambient;
    ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;
	light_dir = vec3(gl_LightSource[0].position) * tbn;
	vec3 eye_dir = -vec3(gl_ModelViewMatrix * gl_Vertex); 
    half_vector = (normalize(vec3(gl_LightSource[0].position)) + 
        normalize(eye_dir)) * tbn;;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
