#pragma once

namespace Window
{
const char basic2DVertexShaderSource[] =
"#version 330 core\n\
\n\
layout (location = 0) in vec2 position;\n\
\n\
out vec2 textureCoord;\n\
\n\
void main()\n\
{\n\
	gl_Position = vec4(position,0,1.0);\n\
	textureCoord = position*0.5 + vec2(0.5);\n\
}\n\n";

const char gammaCorrectionFragmantShaderSource[] =
"#version 330 core\n\
out vec4 outColor;\n\
\n\
in vec2 textureCoord;\n\
uniform sampler2D textureSampler;\n\
uniform float gamma;\n\
\n\
void main()\n\
{\n\
	vec4 uncorrectedColor = texture(textureSampler,vec2(textureCoord.x,-textureCoord.y));\n\
	float inverseGamma = 1.0f/gamma;\n\
	vec4 correctedColor = pow(uncorrectedColor,vec4(inverseGamma,inverseGamma,inverseGamma,0));\n\
	outColor = correctedColor;\n\
}\n\n";

}
