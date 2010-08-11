varying vec3 normal;

void main() {
    vec3 n = normalize(normal);
    gl_FragColor = vec4((n.x+1.0)/2.0, (n.y+1.0)/2.0, (n.z+1.0)/2.0, a); 
}
