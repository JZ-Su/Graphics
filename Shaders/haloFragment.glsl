#version 330 core

uniform sampler2D diffuseTex;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)	{
	fragColour = texture(diffuseTex, IN.texCoord);
	float alpha = 0.1;
	float r = sqrt((IN.texCoord.x-0.5) * (IN.texCoord.x-0.5) + (IN.texCoord.y-0.5) * (IN.texCoord.y-0.5));
	float radius = sqrt(3) / 4.0;
	float dStep = (2.0 - sqrt(3)) / 28.0;
	if(fragColour.x!=0 || fragColour.y!=0  || fragColour.z!=0 )
	if (r < radius) {
		fragColour = vec4(0,0,0,0);
	} else if (r <= radius + 2.0 * dStep) {
		fragColour = vec4(1.0, (r - radius) / dStep / 2.0, 0.0, fragColour.w * alpha);
	} else if (r <= radius + 3.0 * dStep) {
		fragColour = vec4((radius - r) / dStep + 3.0, 1.0, 0.0, fragColour.w * alpha);
	} else if (r <= radius + 4.0 * dStep) {
		fragColour = vec4(0.0, 1.0, (r - radius) / dStep - 3.0, fragColour.w * alpha);
	} else if (r <= radius + 5.0 * dStep) {
		fragColour = vec4(0.0, (radius - r) / dStep + 5.0, 1.0, fragColour.w * alpha);
	} else if (r <= radius + 6.0 * dStep) {
		fragColour = vec4((r - radius) / dStep / 2 - 2.5, 0.0, 1.0, fragColour.w * alpha);
	} else {
		fragColour = vec4((r - radius) / dStep / 2 - 2.5, (r - radius) / dStep - 6, 1.0, fragColour.w * alpha);
	}
}
