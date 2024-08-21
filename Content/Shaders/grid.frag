#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in float inNear;
layout(location = 1) in float inFar;
layout(location = 2) in vec3 inNearPoint;
layout(location = 3) in vec3 inFarPoint;
layout(location = 4) in mat4 inViewProjection;

vec4 grid(vec3 position, float scale, bool hasAxis) {
    vec2 coord      = position.xy * scale;
    vec2 derivative = fwidth(coord);
    vec2 uv         = fract(coord - 0.5) - 0.5;
    vec2 grid       = abs(uv) / derivative;

    float line     = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1.0);
    float minimumx = min(derivative.x, 1.0);

    vec4 color = vec4(0.34);
    color.a    = 1.0 - min(line, 1.0);

    if (!hasAxis) {
        return color;
    }

    if ((-1.0 * minimumx) < position.x && position.x < (0.1 * minimumx)) {
        color.rgb = vec3(0.28);
    }

    if ((-1.0 * minimumz) < position.y && position.y < (0.1 * minimumz)) {
        color.rgb = vec3(0.28);
    }

    return color;
}

float computeDepth(vec4 clipSpace) {
    return clipSpace.z / clipSpace.w;
}

float computeLinearDepth(vec4 clipSpace) {
    float clippedDepth = (clipSpace.z / clipSpace.w) * 2.0 - 1.0;
    float linearDepth  = (2.0 * inNear * inFar) / (inFar + inNear - clippedDepth * (inFar - inNear));

    return linearDepth / inFar;
}

float computeFade(vec4 clipSpace) {
    float linearDepth = computeLinearDepth(clipSpace);

    return linearDepth / inFar;
}

void main() {
    float floorDistance = -inNearPoint.z / (inFarPoint.z - inNearPoint.z);

    vec3 position  = inNearPoint + (floorDistance * (inFarPoint - inNearPoint));
    vec4 clipSpace = inViewProjection * vec4(position, 1.0);

    outColor  = grid(position, 1, true);
    outColor *= smoothstep(0.08, 0.0, computeFade(clipSpace));

    gl_FragDepth = computeDepth(clipSpace);

    if (floorDistance < 0 || outColor.a <= 0.0) {
        discard;
    }
}