varying vec3 normal;
varying vec2 texcoord0;
varying vec3 tangent;
varying vec3 bitangent;

void main() {
    vec3 n = normalize(tangent);
    gl_FragColor = vec4((n.x+1.0)/2.0, (n.y+1.0)/2.0, (n.z+1.0)/2.0, 1.0); 
}
