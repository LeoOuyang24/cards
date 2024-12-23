#version 330 core

out vec4 FragColor;

in vec2 texCoord;

flat in int ms; //time in milliseconds

//shamelessly stolen from https://thebookofshaders.com/10/
float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

void main()
{
    vec2 center = vec2(floor(texCoord.x/0.01),floor(texCoord.y/0.01));
    vec4 text = mix(vec4(0,0,0,0),vec4(0.3,0,0,1), min(1.0f,ms/1000.0f));
    //vec4 text = texture(sprite,texCoord);
    FragColor = text;
}
