#version 330 core

layout (location = 0) in vec2 values;
layout (location = 1) in vec4 rect;
layout (location = 2) in int depth;
layout (location = 3) in float radians;
layout (location = 4) in float flip; //radians along the y axis

#include "${resources_dir}/shaders/common/uniforms.h"

out vec2 texCoord;

void main()
{


    vec2 transformed = vec2(values.xy * rect.zw*.5);
    //same as the usual vertex shader except we also rotate around the y axis.
    //this is the naive approach where we simply rotate around the z axis first then y.
    //gimbal lock these nuts lmao gottem
    transformed = vec2(cos(flip)*transformed.x + sin(flip)*depth,transformed.y );
    transformed = vec2(cos(radians)*transformed.x - sin(radians)*transformed.y,sin(radians)*transformed.x + cos(radians)*transformed.y);
    transformed += rect.xy + rect.zw*.5;

    gl_Position = projection*view*vec4(transformed,depth,1);

    texCoord = vec2((values.x + 1)/2.0f, (values.y + 1)/2.0f);


   // gl_Position.z = depth;

}
