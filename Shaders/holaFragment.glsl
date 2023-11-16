#version 330 core

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)	{
	float r = sqrt(IN.texCoord.x * IN.texCoord.x + IN.texCoord.y * IN.texCoord.y);
	float radius = sqrt(3) / 2.0;
	float dStep = (2.0 - sqrt(3)) / 14.0;
	if (r < radius) {
		fragColour = vec4(IN.colour.xyz, 0.05 / radius);
	} else if (r <= radius + 2.0 * dStep) {
		fragColour = vec4(1.0, (r - radius) / dStep / 2.0, 0.0, IN.colour.w);
	} else if (r <= radius + 3.0 * dStep) {
		fragColour = vec4((radius - r) / dStep + 3.0, 1.0, 0.0, IN.colour.w);
	} else if (r <= radius + 4.0 * dStep) {
		fragColour = vec4(0.0, 1.0, (r - radius) / dStep - 3.0, IN.colour.w);
	} else if (r <= radius + 5.0 * dStep) {
		fragColour = vec4(0.0, (radius - r) / dStep + 5.0, 1.0, IN.colour.w);
	} else if (r <= radius + 6.0 * dStep) {
		fragColour = vec4((r - radius) / dStep / 2 - 2.5, 0.0, 1.0, IN.colour.w);
	} else {
		fragColour = vec4((r - radius) / dStep / 2 - 2.5, (r - radius) / dStep - 6, 1.0, IN.colour.w);
	}
	//fragColour = IN.colour;	
}
