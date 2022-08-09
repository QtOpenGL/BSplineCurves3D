#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 256) out;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform vec3 control_points[16];
uniform int control_points_count;
uniform float dt; //  dt = t_(n) - t(n-1) where t_(n) and t_(n-1) is in [0,...,1], i.e., it is the difference between two consecutive number in [0,...,1].
uniform float r; // radius, distance to core path
uniform float sector_angle_0;
uniform float sector_angle_1;

in float gs_t[]; // Coming from vertex shader. This contains exactly one element because our primitive is "points".

out vec3 fs_normal;
out vec3 fs_position;

const int degree = control_points_count - 1;

#define PI 3.1415926538

float custom_pow(float x, float y)
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


vec3 value_at(float t)
{
    vec3 value = vec3(0, 0, 0);

    for(int i = 0; i <= degree; ++i)
    {
        float c0 = choose(degree, i);
        float c1 = custom_pow(t, i);
        float c2 = custom_pow(1 - t, degree - i);

        value += c0 * c1 * c2 * control_points[i];
    }

    return value;
}


vec3 tangent_at(float t)
{
    vec3 tangent = vec3(0,0,0);

    for(int i = 0; i <= degree - 1; i++)
    {
        float c0 = choose(degree - 1, i);
        float c1 = custom_pow(t, i);
        float c2 = custom_pow(1 - t, degree - 1 - i);
        tangent += degree * c0 * c1 * c2 * (control_points[i+1] - control_points[i]);
    }

   return normalize(tangent);
}

// Rotation around axis by angle radians
mat4 rotation_matrix(vec3 axis, float angle)
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
mat3 rotate_x(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(1, 0, 0),
        vec3(0, c, -s),
        vec3(0, s, c)
    );
}

// Rotation matrix around the Y axis.
mat3 rotate_y(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(c, 0, s),
        vec3(0, 1, 0),
        vec3(-s, 0, c)
    );
}

// Rotation matrix around the Z axis.
mat3 rotate_z(float theta) {
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

    vec3 value0 = value_at(t0);
    vec3 value1 = value_at(t1);

    vec3 tangent0 = tangent_at(t0);
    vec3 tangent1 = tangent_at(t1);

    float theta0 = atan2(-tangent0.z, tangent0.x);
    float theta1 = atan2(-tangent1.z, tangent1.x);

    float sqrt0 = sqrt(tangent0.x * tangent0.x + tangent0.z * tangent0.z);
    float sqrt1 =  sqrt(tangent1.x * tangent1.x + tangent1.z * tangent1.z);

    // What if sqrt = 0? We may use atan2 here.
    // Check for singular cases.
    float phi0 = atan(tangent0.y / sqrt0);
    float phi1 = atan(tangent1.y / sqrt1);

    mat3 rotation0 = rotate_y(-theta0) * rotate_z(-phi0);
    mat3 rotation1 = rotate_y(-theta1) * rotate_z(-phi1);

    mat4 pm = projection_matrix * view_matrix;

    vec3 position00 = value0 + rotation0 * vec3(0, r * cos(sector_angle_0), r * sin(sector_angle_0));
    vec3 position10 = value1 + rotation1 * vec3(0, r * cos(sector_angle_0), r * sin(sector_angle_0));
    vec3 position01 = value0 + rotation0 * vec3(0, r * cos(sector_angle_1), r * sin(sector_angle_1));
    vec3 position11 = value1 + rotation1 * vec3(0, r * cos(sector_angle_1), r * sin(sector_angle_1));

    vec3 normal = cross(normalize(position00 - position10), normalize(position01 - position10));

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
