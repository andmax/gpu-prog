varying vec3 normal;
uniform float time;

void main() {
    vec3 n = normalize(normal);
    float s = sin(radians(time*45.0));
    gl_FragColor = vec4(step(n.x,0.0), step(n.y,0.0), s*s, 1.0); 
}
