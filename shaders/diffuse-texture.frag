uniform sampler2D diffuse_texture;
varying vec2 texcoord0;

void main() {
    gl_FragColor = texture2D(diffuse_texture, texcoord0);
}
