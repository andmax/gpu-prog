uniform sampler2D diffuse_texture; 
uniform sampler2D normalmap_texture;

varying vec4 diffuse, ambient;
varying vec3 half_vector, normal, light_dir;

void main (void) {
    vec2 st = gl_TexCoord[0].st;
    vec4 decal = texture2D(diffuse_texture, 2.0*st);
    vec4 color = ambient * decal;
    vec3 l = normalize(light_dir);
	vec3 n = normalize(normal);
    float NdotL = dot(n, l);
    // macro-structure self-shadowing
    if (NdotL > 0.0) {
        vec3 normal_bump = texture2D(normalmap_texture, 3.0*st).xyz * 2.0 - 1.0;
        vec3 nb = normalize(normal_bump);
        float NBdotL = dot(nb, l);
        // meso-structure self-shadowing
        if (NBdotL > 0.0) {
            color += diffuse * NBdotL * decal;
            vec3 hv = normalize(half_vector);
            float NBdotHV = max(dot(nb, hv), 0.0);
            vec4 specular = gl_FrontMaterial.specular * 
                            gl_LightSource[0].specular * 
                            pow(NBdotHV, gl_FrontMaterial.shininess);
            color += specular;
        }
    }
	gl_FragColor = color; 
}			
