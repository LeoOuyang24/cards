#version 330 core

out vec4 fragColor;
in vec2 texCoord;
in vec4 shade;
uniform sampler2D sprite;

void main()
{
    vec4 text = texture(sprite,texCoord);
    fragColor = mix(vec4(shade.rgb,text.a),text,(shade == vec4(1,1,1,1)));//vec4(1,0,0,1);

}
