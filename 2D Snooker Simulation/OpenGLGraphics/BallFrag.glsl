#version 150 core

in Vertex	{
	vec2 texCoord;
	vec4 colour;
	vec3 outPos;
} IN;

out vec4 gl_FragColor;

void main(void)	{
	if (((IN.outPos.x * IN.outPos.x) + (IN.outPos.y * IN.outPos.y)) > 0.00008)
	{
		gl_FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	} else {
		gl_FragColor = IN.colour;
	}
}