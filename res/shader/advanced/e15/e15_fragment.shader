#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int post_process;

const float offset = 1.0 / 300.0;

void main()
{ 
    if (post_process == 0 || post_process > 5) {
        FragColor = texture(screenTexture, TexCoords);
    }
    else if (post_process == 1) {
        FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
    }
    else if (post_process == 2) {
        FragColor = texture(screenTexture, TexCoords);
        // float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
        float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
        FragColor = vec4(average, average, average, 1.0);
    }
    else if (post_process >= 3 && post_process <=5) {
        vec2 offsets[9] = vec2[](
            vec2(-offset,  offset), // 左上
            vec2( 0.0f,    offset), // 正上
            vec2( offset,  offset), // 右上
            vec2(-offset,  0.0f),   // 左
            vec2( 0.0f,    0.0f),   // 中
            vec2( offset,  0.0f),   // 右
            vec2(-offset, -offset), // 左下
            vec2( 0.0f,   -offset), // 正下
            vec2( offset, -offset)  // 右下
        );

        float sharpen_kernel[9] = float[](
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1
        );
        float edge_t_kernel[9] = float[](
            1, 1, 1,
            1, -8, 1,
            1, 1, 1
        );
        float blur_kernel[9] = float[](
            1.0 / 16, 2.0 / 16, 1.0 / 16,
            2.0 / 16, 4.0 / 16, 2.0 / 16,
            1.0 / 16, 2.0 / 16, 1.0 / 16  
        );

        float kernel[9];
        if (post_process == 3) {
            kernel = sharpen_kernel;
        }
        else if (post_process == 4) {
            kernel = edge_t_kernel;
        }
        else {
            kernel = blur_kernel;
        }

        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture(screenTexture, TexCoords.xy + offsets[i]));
        }
        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i];

        FragColor = vec4(col, 1.0);
    }
    
}