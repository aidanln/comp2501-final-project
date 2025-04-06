// Source code of fragment shader
#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;

// Texture sampler
uniform sampler2D onetex;

// Ghost mode declaration
uniform bool ghost;

void main() {

    // Sample texture
    vec4 color = texture2D(onetex, uv_interp);

    // Apply ghost mode corrections
    if (ghost) {
        float gray = (color.r + color.g + color.b) / 3.0;
        color.rgb = vec3(gray);
    }

    // Assign color to fragment
    gl_FragColor = color;

    // Discard objects that are too transparent
    if (color.a < 0.01) {
         discard;
    }
}
