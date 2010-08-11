uniform float time;
varying vec3 normal;
varying vec2 texcoord0;

void main() {
    float s = sin(radians(time*45.0));
    s *= s * 0.05;
    normal = normalize(gl_NormalMatrix * gl_Normal);
    vec4 v = gl_ModelViewMatrix * gl_Vertex;
    v = vec4(v.xyz + s*normal, v.w);
    texcoord0 = vec2(gl_MultiTexCoord0);
    gl_Position = gl_ProjectionMatrix*v; 
}
