uniform sampler2D diffuse_texture; 
uniform sampler2D normalmap_texture;

varying vec4 diffuse, ambient, specular;
varying vec3 half_vector, light_dir;

void main (void) {
    vec2 st = gl_TexCoord[0].st;
    vec4 decal = texture2D(diffuse_texture, st);
    vec4 color = ambient * decal;
    vec3 l = normalize(light_dir);
    float NdotL = l.z; 
    // macro-structure self-shadowing
    if (NdotL > 0.0) {
        vec3 normal_bump = texture2D(normalmap_texture, st).xyz * 2.0 - 1.0;
        vec3 nb = normalize(normal_bump);
        float NBdotL = dot(nb, l);
        // meso-structure self-shadowing
        if (NBdotL > 0.0) {
            color += diffuse * NBdotL * decal;
            vec3 hv = normalize(half_vector);
            float NBdotHV = max(dot(nb, hv), 0.0);
            vec4 specular = specular * pow(NBdotHV, gl_FrontMaterial.shininess);
            color += specular * 2.0;
        }
    }
	gl_FragColor = color; 
}
