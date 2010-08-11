varying vec3 normal;
varying vec2 texcoord0;
varying vec3 tangent;
varying vec3 bitangent;

void main() {
    gl_FragColor = vec4(texcoord0.x, texcoord0.y, 0, 1.0); 
}
