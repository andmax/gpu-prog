const vec4 lightModelAmbient = vec4(0., 0., 0., 0.);
const vec4 lightPosition = vec4(0.5, 0.25, 0.83, 0.);
const vec4 lightAmbient = vec4(0.25, 0.25, 0.25, 1.);
const vec4 lightDiffuse = vec4(0.85, 0.85, 0.85, 1.);
const vec4 lightSpecular = vec4(0.5, 0.5, 0.5, 1.);
const vec4 materialAmbient = vec4(0.8, 0.5, 0.4, 1.);
const vec4 materialDiffuse = vec4(0.8, 0.5, 0.4, 1.);
const vec4 materialSpecular = vec4(0.8, 0.8, 0.8, 1.);
const vec4 materialEmission = vec4(0., 0., 0., 1);
const float materialShininess = 16.;

varying vec4 color;

void main() {
    // move normal and view vector to eye space
    vec3 n = normalize(gl_NormalMatrix * gl_Normal);
    vec4 v = normalize(gl_ModelViewMatrix * gl_Vertex);
    vec3 h = normalize(n-v.xyz);
    color = materialEmission + lightAmbient*materialAmbient;
    float ds = clamp(dot(n, lightPosition.xyz),0.0,1.0);
    color += lightDiffuse*materialDiffuse*ds;
    float ss = pow(clamp(dot(n,h),0.0,1.0), materialShininess);
    color += lightSpecular*materialSpecular*ss;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
}
