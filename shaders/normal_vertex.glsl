#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightpos;
uniform vec3 lightintensity;
uniform vec3 eyepos;

out vec4 vertex_color;

void main() {
    // First, convert to world coordinates. This is where
	// lighting computations must be performed. inVertex
	// is NOT in homogeneous coordinates. inNormal has three
	// components. For computing the normal transformation
	// matrix we use the upper 3x3 part of the modeling
	// matrix.
	 
	vec4 pWorld = model * vec4(pos, 1);
	vec3 nWorld = inverse(transpose(mat3x3(model))) * normal;

	// Compute lighting. We assume lightPos and eyePos are in world
	// coordinates.

	vec3 L = normalize(lightpos - vec3(pWorld));
	vec3 V = normalize(eyepos - vec3(pWorld));
	vec3 H = normalize(L + V);
	vec3 N = normalize(nWorld);

	float NdotL = dot(N, L); // for diffuse component
	float NdotH = dot(N, H); // for specular component

	vec3 diffuseColor = lightintensity * max(0, NdotL);
	vec3 specularColor = lightintensity * pow(max(0, NdotH), 100);
	vec3 ambientColor = vec3(0.1, 0.1, 0.1);

	// We update the front color of the vertex. This value will be sent
	// to the fragment shader after it is interpolated at every fragment.
	// Front color specifies the color of a vertex for a front facing
	// primitive.

	vertex_color = vec4(diffuseColor + specularColor + ambientColor, 1);

	// Transform the vertex with the product of the projection, viewing, and
	// modeling matrices.

    gl_Position = projection * view * model * vec4(pos, 1);
}   
