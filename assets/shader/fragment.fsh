#version 330 core
uniform sampler2D tex_image;
uniform sampler2D tex_lut;


in vec2 TexCoord;
out vec4 FragColor;

void main(void) {
    vec4 rgbColor = texture(tex_image, TexCoord);

    float blueColor = rgbColor.b * 63.;

    vec2 quad1;
    quad1.y = floor(floor(blueColor) / 8.0);
    quad1.x = floor(blueColor) - (quad1.y * 8.0);

    vec2 quad2;
    quad2.y = floor(ceil(blueColor) / 8.0);
    quad2.x = ceil(blueColor) - (quad2.y * 8.0);

    vec2 texPos1;
    texPos1.x = (quad1.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * rgbColor.r);
    texPos1.y = (quad1.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * rgbColor.g);

    vec2 texPos2;
    texPos2.x = (quad2.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * rgbColor.r);
    texPos2.y = (quad2.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * rgbColor.g);

    vec4 newColor1 = texture(tex_lut, texPos1);
    vec4 newColor2 = texture(tex_lut, texPos2);

    FragColor = mix(newColor1, newColor2, fract(blueColor));
}