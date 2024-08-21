#version 450

layout(set = 0, binding = 0) uniform CameraUBO {
    vec4 clip;

    mat4 view;
    mat4 projection;
    mat4 viewProjection;

    vec4 forward;
    vec4 right;
    vec4 up;
} camera;

layout(location = 0) out float outNear;
layout(location = 1) out float outFar;
layout(location = 2) out vec3 outNearPoint;
layout(location = 3) out vec3 outFarPoint;
layout(location = 4) out mat4 outViewProjection;

const vec3 positions[6] = vec3[](
	vec3( 1.0,  1.0, 0.0),
	vec3(-1.0, -1.0, 0.0),
	vec3(-1.0,  1.0, 0.0),
	vec3(-1.0, -1.0, 0.0),
	vec3( 1.0,  1.0, 0.0),
	vec3( 1.0, -1.0, 0.0)
);

vec3 unprojectPoint(float x, float y, float z) {
    vec4 unprojectedPoint =  inverse(camera.viewProjection) * vec4(x, y, z, 1.0);

    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec3 point = positions[gl_VertexIndex];

    // Values
    outNear = camera.clip.x;
    outFar  = camera.clip.y;

    // Points
    outNearPoint = unprojectPoint(
        point.x,
        point.y,
        -1.0
    ).xyz;
    outFarPoint = unprojectPoint(
        point.x,
        point.y,
        1.0
    ).xyz;

    // Camera
    outViewProjection = camera.viewProjection;

    gl_Position = vec4(point, 1.0);
}