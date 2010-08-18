varying vec4 diffuse, ambient;
varying vec3 normal, light_dir, half_vector;

void main() {
    vec3 n = normalize(normal);
    vec4 color = ambient;
    float NdotL = max(dot(n, light_dir), 0.0);
    if (NdotL > 0.0) {
        color +=  diffuse * NdotL;
        vec3 hv = normalize(half_vector);
        float NdotHV = max(dot(n, hv), 0.0);
        vec4 specular = gl_FrontMaterial.specular * gl_LightSource[0].specular 
                * pow(NdotHV, gl_FrontMaterial.shininess);
        color += specular;
    }

    gl_FragColor = color;
}

