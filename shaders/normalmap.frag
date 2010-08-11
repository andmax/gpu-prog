uniform sampler2D diffuse_texture;
uniform sampler2D normalmap_texture;
varying vec2 texcoord0;
varying vec3 normal;
varying vec3 tangent;
varying vec3 bitangent;

void main() {
    vec3 n = texture2D(normalmap_texture, texcoord0).xyz;
    n = vec3(n.x*2.0-1.0, n.y*2.0-1.0, n.z*2.0-1.0);
    vec3 bump = vec3(dot(n,tangent),dot(n,bitangent), dot(n,normal));
    gl_FragColor = vec4((bump.x+1.0)/2.0, (bump.y+1.0)/2.0, (bump.z+1.0)/2.0, 1.0);
    //float c = (bump.z+1.0)/2.0;
    //gl_FragColor = vec4(c,c,c,1.0);
}
