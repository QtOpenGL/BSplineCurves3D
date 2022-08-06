#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 256) out;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 controlPoints[16];
uniform int controlPointsCount;
uniform float dt; //  dt = t_(n) - t(n-1) where t_(n) and t_(n-1) is in [0,...,1], i.e., it is the difference between two consecutive number in [0,...,1].
uniform float r; // radius, distance to core path
uniform float sectorAngle0;
uniform float sectorAngle1;

in float gs_t[]; // Coming from vertex shader. This contains exactly one element because our primitive is points.

out vec3 fs_normal;
out vec3 fs_position;

#define PI 3.1415926538

float customPow(float x, float y)
{
    if(x == 0 && y == 0)
        return 1;

    return pow(x, y);
}

float factorial(int n)
{
    float result = 1;

    for(int i = 2; i <= n; ++i)
        result *= i;

    return result;
}


float choose(int n, int k)
{
    return factorial(n) / (factorial(k) * factorial(n - k));
}


vec3 valueAt(float t)
{
    vec3 value = vec3(0, 0, 0);
    int degree = controlPointsCount - 1;

    for(int i = 0; i <= degree; ++i)
    {
        float c0 = choose(degree, i);
        float c1 = customPow(t, i);
        float c2 = customPow(1 - t, degree - i);

        value += c0 * c1 * c2 * controlPoints[i];
    }

    return value;
}


vec3 tangentAt(float t)
{
    vec3 tangent = vec3(0,0,0);
    int degree = controlPointsCount - 1;

    for(int i = 0; i <= degree - 1; i++)
    {
        float c0 = choose(degree - 1, i);
        float c1 = customPow(t, i);
        float c2 = customPow(1 - t, degree - 1 - i);
        tangent += degree * c0 * c1 * c2 * (controlPoints[i+1] - controlPoints[i]);
    }

   return normalize(tangent);
}

// Rotation around axis by angle radians
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

// Rotation matrix around the X axis.
mat3 rotateX(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(1, 0, 0),
        vec3(0, c, -s),
        vec3(0, s, c)
    );
}

// Rotation matrix around the Y axis.
mat3 rotateY(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(c, 0, s),
        vec3(0, 1, 0),
        vec3(-s, 0, c)
    );
}

// Rotation matrix around the Z axis.
mat3 rotateZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(c, -s, 0),
        vec3(s, c, 0),
        vec3(0, 0, 1)
    );
}

// Identity matrix.
mat3 identity() {
    return mat3(
        vec3(1, 0, 0),
        vec3(0, 1, 0),
        vec3(0, 0, 1)
    );
}

float atan2(float num, float denum) {

    if(denum != 0) {
        return atan(num, denum);
    }

    if(num >= 0) {
        return -PI / 2;
    }
    else {
        return PI / 2;
    }

}


void main()
{
    float t0 = gs_t[0];
    float t1 = t0 + dt;
    float t2 = t1 + dt;

    vec3 value0 = valueAt(t0);
    vec3 value1 = valueAt(t1);

    vec3 tangent0 = tangentAt(t0);
    vec3 tangent1 = tangentAt(t1);

    float theta0 = atan2(-tangent0.z, tangent0.x);
    float theta1 = atan2(-tangent1.z, tangent1.x);

    float sqrt0 = sqrt(tangent0.x * tangent0.x + tangent0.z * tangent0.z);
    float sqrt1 =  sqrt(tangent1.x * tangent1.x + tangent1.z * tangent1.z);

    // What if sqrt = 0? We may use atan2 here.
    // Check for singular cases
    float phi0 = atan(tangent0.y / sqrt0);
    float phi1 = atan(tangent1.y / sqrt1);

    mat3 rotation0 = rotateY(-theta0) * rotateZ(-phi0);
    mat3 rotation1 = rotateY(-theta1) * rotateZ(-phi1);

    mat4 pm = projectionMatrix * viewMatrix;

    vec3 position00 = value0 + rotation0 * vec3(0, r * cos(sectorAngle0), r * sin(sectorAngle0));
    vec3 position10 = value1 + rotation1 * vec3(0, r * cos(sectorAngle0), r * sin(sectorAngle0));
    vec3 position01 = value0 + rotation0 * vec3(0, r * cos(sectorAngle1), r * sin(sectorAngle1));
    vec3 position11 = value1 + rotation1 * vec3(0, r * cos(sectorAngle1), r * sin(sectorAngle1));

    vec3 normal = normalize(cross(normalize(position00 - position10), normalize(position01 - position10)));

    fs_position = position00;
    fs_normal = normal;
    gl_Position = pm * vec4(position00, 1.0);
    EmitVertex();

    fs_position = position10;
    fs_normal = normal;
    gl_Position = pm * vec4(position10, 1.0);
    EmitVertex();

    fs_position = position01;
    fs_normal = normal;
    gl_Position = pm * vec4(position01, 1.0);
    EmitVertex();

    fs_position = position11;
    fs_normal = normal;
    gl_Position = pm * vec4(position11, 1.0);
    EmitVertex();

    EndPrimitive();

}
