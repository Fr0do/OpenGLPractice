#version 330 core
in vec2 vTexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;

out vec4 fragColor;

void main()
{             
    const float gamma = 1.0;
    vec3 hdrColor = texture(scene, vTexCoords).rgb;          
    vec3 bloomColor = texture(bloomBlur, vTexCoords).rgb;
    if(bloom) {
        hdrColor += bloomColor; // additive blending
    }
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    fragColor = vec4(result, 1.0);
    //fragColor = vec4(hdrColor, 1.0);
}