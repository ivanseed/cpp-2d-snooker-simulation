#version 150 core

in Vertex	{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 gl_FragColor;

void main(void)	{	
	gl_FragColor = vec4(0.6f,0.2f,0.1f,1.0f);
}