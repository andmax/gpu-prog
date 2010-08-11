uniform float time;
varying vec3 normal;
varying vec2 texcoord0;
varying vec3 tangent;
varying vec3 bitangent;

void main() {
    normal = normalize(gl_NormalMatrix * gl_Normal);
    tangent = normalize(gl_NormalMatrix * gl_MultiTexCoord1.xyz); 
    bitangent = gl_MultiTexCoord1.w * cross(normal, tangent);
    texcoord0 = vec2(gl_MultiTexCoord0);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
