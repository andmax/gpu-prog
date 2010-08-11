uniform sampler2D diffuse_texture;
uniform sampler2D normalmap_texture;
varying vec2 texcoord0;

void main() {
    gl_FragColor = texture2D(normalmap_texture, texcoord0);
}
