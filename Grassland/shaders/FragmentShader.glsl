#version 330 core

in vec4 vColor;
in vec3 originPos;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragColor2;

void main()
{
    FragColor = vColor;
    if (!gl_FrontFacing)
        FragColor2 = vec4(1.0 - FragColor) * 0.5 + 0.5* FragColor;
    else
        FragColor2 = FragColor;
    //FragColor2 = vec4(vec3(gl_FragCoord.z*2.0 - 1.0), 1.0);
    //float oz = originPos.z + 5.0;
    //if (gl_FrontFacing)
    //    discard;
    FragColor2 = vec4(originPos, 1.0);
    /*if (FragColor2.x < 0.0)FragColor2.x = 0.0;
    if (FragColor2.y < 0.0)FragColor2.y = 0.0;
    if (FragColor2.z < 0.0)FragColor2.z = 0.0;*/
    //FragColor2 = vec4((FragColor2.xyz + 1.0) * 0.5, 1.0);
    //if (gl_FragCoord.x < 400 && gl_FragCoord.y<300)
    //    FragColor2 = vec4(0.5, 0.5, 0.5, 1.0);
    //FragColor2 = vec4(FragColor.xy, oz, FragColor.w);
} 