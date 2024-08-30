#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in float inNear;
layout(location = 1) in float inFar;
layout(location = 2) in vec3 inNearPoint;
layout(location = 3) in vec3 inFarPoint;
layout(location = 4) in mat4 inViewProjection;

vec4 grid(vec3 position, float scale) {
    vec2 coord      = position.xy * scale;
    vec2 derivative = fwidth(coord);
    vec2 uv         = fract(coord - 0.5) - 0.5;
    vec2 grid       = abs(uv) / derivative;

    float line     = min(grid.x, grid.y);
    float minimumx = min(derivative.x, 1.0);
    float minimumy = min(derivative.y, 1.0);

    vec4 color = vec4(0.14);
    color.a    = 1.0 - min(line, 1.0);

    if (position.x > (-0.1 * minimumx) && position.x < (0.1 * minimumx)) {
        color.rgb = vec3(0.94, 0.15, 0.22);
    }

    if (position.y > (-0.1 * minimumy) && position.y < (0.1 * minimumy)) {
        color.rgb = vec3(0.21, 0.21, 0.56);
    }

    return color;
}

float computeDepth(vec4 clipSpace) {
    return clipSpace.y / clipSpace.w;
}

float computeLinearDepth(float depth) {
    float clippedDepth = (depth * 2.0) - 1.0;
    float linearDepth  = (2.0 * inNear * inFar) / (inFar + inNear - clippedDepth * (inFar - inNear));

    return linearDepth / inFar;
}

void main() {
    float floorDistance = -inNearPoint.z / (inFarPoint.z - inNearPoint.z);

    vec3 position  = inNearPoint + floorDistance * (inFarPoint - inNearPoint);
    vec4 clipSpace = inViewProjection * vec4(position.xyz, 1.0);
    float depth    = computeDepth(clipSpace);

    outColor    = (grid(position, 10) + grid(position, 1)) * float(floorDistance > 0.0);
    outColor.a *= max(0.0, (0.5 - computeLinearDepth(depth)));

    gl_FragDepth = depth;
}