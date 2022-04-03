/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "Shared.h"

//#if USE_CLIENT || CGAME_INCLUDE

/*
======
vectoangles2 - this is duplicated in the game DLL, but I need it here.
======
*/
void vectoangles2(const vec3_t value1, vec3_t angles)
{
    float   forward;
    float   yaw, pitch;

    if (value1[1] == 0 && value1[0] == 0) {
        yaw = 0;
        if (value1[2] > 0) {
            pitch = 90;
        } else {
            pitch = 270;
        }
    }
    else {
        if (value1[0]) {
            yaw = std::atan2f(value1[1], value1[0]) * 180.f / M_PI;
        } else if (value1[1] > 0) {
            yaw = 90;
        } else {
            yaw = 270;
        }

        if (yaw < 0) {
            yaw += 360;
        }

        forward = std::sqrtf(value1[0] * value1[0] + value1[1] * value1[1]);
        pitch = std::atan2f(value1[2], forward) * 180.f / M_PI;
        if (pitch < 0) {
            pitch += 360;
        }
    }

    angles[vec3_t::Pitch] = -pitch;
    angles[vec3_t::Yaw] = yaw;
    angles[vec3_t::Roll] = 0;
}

void MakeNormalVectors(const vec3_t& forward, vec3_t& right, vec3_t& up)
{
    float       d;

    // this rotate and negate guarantees a vector
    // not colinear with the original
    right.xyz[1] = -forward.xyz[0];
    right.xyz[2] = forward.xyz[1];
    right.xyz[0] = forward.xyz[2];

    d = DotProduct(right, forward);
    VectorMA(right, -d, forward, right);
    VectorNormalize(right);
    CrossProduct(right, forward, up);
}

//#endif  // USE_CLIENT

const vec3_t bytedirs[NUMVERTEXNORMALS] = {
    {-0.525731, 0.000000, 0.850651},
    {-0.442863, 0.238856, 0.864188},
    {-0.295242, 0.000000, 0.955423},
    {-0.309017, 0.500000, 0.809017},
    {-0.162460, 0.262866, 0.951056},
    {0.000000, 0.000000, 1.000000},
    {0.000000, 0.850651, 0.525731},
    {-0.147621, 0.716567, 0.681718},
    {0.147621, 0.716567, 0.681718},
    {0.000000, 0.525731, 0.850651},
    {0.309017, 0.500000, 0.809017},
    {0.525731, 0.000000, 0.850651},
    {0.295242, 0.000000, 0.955423},
    {0.442863, 0.238856, 0.864188},
    {0.162460, 0.262866, 0.951056},
    {-0.681718, 0.147621, 0.716567},
    {-0.809017, 0.309017, 0.500000},
    {-0.587785, 0.425325, 0.688191},
    {-0.850651, 0.525731, 0.000000},
    {-0.864188, 0.442863, 0.238856},
    {-0.716567, 0.681718, 0.147621},
    {-0.688191, 0.587785, 0.425325},
    {-0.500000, 0.809017, 0.309017},
    {-0.238856, 0.864188, 0.442863},
    {-0.425325, 0.688191, 0.587785},
    {-0.716567, 0.681718, -0.147621},
    {-0.500000, 0.809017, -0.309017},
    {-0.525731, 0.850651, 0.000000},
    {0.000000, 0.850651, -0.525731},
    {-0.238856, 0.864188, -0.442863},
    {0.000000, 0.955423, -0.295242},
    {-0.262866, 0.951056, -0.162460},
    {0.000000, 1.000000, 0.000000},
    {0.000000, 0.955423, 0.295242},
    {-0.262866, 0.951056, 0.162460},
    {0.238856, 0.864188, 0.442863},
    {0.262866, 0.951056, 0.162460},
    {0.500000, 0.809017, 0.309017},
    {0.238856, 0.864188, -0.442863},
    {0.262866, 0.951056, -0.162460},
    {0.500000, 0.809017, -0.309017},
    {0.850651, 0.525731, 0.000000},
    {0.716567, 0.681718, 0.147621},
    {0.716567, 0.681718, -0.147621},
    {0.525731, 0.850651, 0.000000},
    {0.425325, 0.688191, 0.587785},
    {0.864188, 0.442863, 0.238856},
    {0.688191, 0.587785, 0.425325},
    {0.809017, 0.309017, 0.500000},
    {0.681718, 0.147621, 0.716567},
    {0.587785, 0.425325, 0.688191},
    {0.955423, 0.295242, 0.000000},
    {1.000000, 0.000000, 0.000000},
    {0.951056, 0.162460, 0.262866},
    {0.850651, -0.525731, 0.000000},
    {0.955423, -0.295242, 0.000000},
    {0.864188, -0.442863, 0.238856},
    {0.951056, -0.162460, 0.262866},
    {0.809017, -0.309017, 0.500000},
    {0.681718, -0.147621, 0.716567},
    {0.850651, 0.000000, 0.525731},
    {0.864188, 0.442863, -0.238856},
    {0.809017, 0.309017, -0.500000},
    {0.951056, 0.162460, -0.262866},
    {0.525731, 0.000000, -0.850651},
    {0.681718, 0.147621, -0.716567},
    {0.681718, -0.147621, -0.716567},
    {0.850651, 0.000000, -0.525731},
    {0.809017, -0.309017, -0.500000},
    {0.864188, -0.442863, -0.238856},
    {0.951056, -0.162460, -0.262866},
    {0.147621, 0.716567, -0.681718},
    {0.309017, 0.500000, -0.809017},
    {0.425325, 0.688191, -0.587785},
    {0.442863, 0.238856, -0.864188},
    {0.587785, 0.425325, -0.688191},
    {0.688191, 0.587785, -0.425325},
    {-0.147621, 0.716567, -0.681718},
    {-0.309017, 0.500000, -0.809017},
    {0.000000, 0.525731, -0.850651},
    {-0.525731, 0.000000, -0.850651},
    {-0.442863, 0.238856, -0.864188},
    {-0.295242, 0.000000, -0.955423},
    {-0.162460, 0.262866, -0.951056},
    {0.000000, 0.000000, -1.000000},
    {0.295242, 0.000000, -0.955423},
    {0.162460, 0.262866, -0.951056},
    {-0.442863, -0.238856, -0.864188},
    {-0.309017, -0.500000, -0.809017},
    {-0.162460, -0.262866, -0.951056},
    {0.000000, -0.850651, -0.525731},
    {-0.147621, -0.716567, -0.681718},
    {0.147621, -0.716567, -0.681718},
    {0.000000, -0.525731, -0.850651},
    {0.309017, -0.500000, -0.809017},
    {0.442863, -0.238856, -0.864188},
    {0.162460, -0.262866, -0.951056},
    {0.238856, -0.864188, -0.442863},
    {0.500000, -0.809017, -0.309017},
    {0.425325, -0.688191, -0.587785},
    {0.716567, -0.681718, -0.147621},
    {0.688191, -0.587785, -0.425325},
    {0.587785, -0.425325, -0.688191},
    {0.000000, -0.955423, -0.295242},
    {0.000000, -1.000000, 0.000000},
    {0.262866, -0.951056, -0.162460},
    {0.000000, -0.850651, 0.525731},
    {0.000000, -0.955423, 0.295242},
    {0.238856, -0.864188, 0.442863},
    {0.262866, -0.951056, 0.162460},
    {0.500000, -0.809017, 0.309017},
    {0.716567, -0.681718, 0.147621},
    {0.525731, -0.850651, 0.000000},
    {-0.238856, -0.864188, -0.442863},
    {-0.500000, -0.809017, -0.309017},
    {-0.262866, -0.951056, -0.162460},
    {-0.850651, -0.525731, 0.000000},
    {-0.716567, -0.681718, -0.147621},
    {-0.716567, -0.681718, 0.147621},
    {-0.525731, -0.850651, 0.000000},
    {-0.500000, -0.809017, 0.309017},
    {-0.238856, -0.864188, 0.442863},
    {-0.262866, -0.951056, 0.162460},
    {-0.864188, -0.442863, 0.238856},
    {-0.809017, -0.309017, 0.500000},
    {-0.688191, -0.587785, 0.425325},
    {-0.681718, -0.147621, 0.716567},
    {-0.442863, -0.238856, 0.864188},
    {-0.587785, -0.425325, 0.688191},
    {-0.309017, -0.500000, 0.809017},
    {-0.147621, -0.716567, 0.681718},
    {-0.425325, -0.688191, 0.587785},
    {-0.162460, -0.262866, 0.951056},
    {0.442863, -0.238856, 0.864188},
    {0.162460, -0.262866, 0.951056},
    {0.309017, -0.500000, 0.809017},
    {0.147621, -0.716567, 0.681718},
    {0.000000, -0.525731, 0.850651},
    {0.425325, -0.688191, 0.587785},
    {0.587785, -0.425325, 0.688191},
    {0.688191, -0.587785, 0.425325},
    {-0.955423, 0.295242, 0.000000},
    {-0.951056, 0.162460, 0.262866},
    {-1.000000, 0.000000, 0.000000},
    {-0.850651, 0.000000, 0.525731},
    {-0.955423, -0.295242, 0.000000},
    {-0.951056, -0.162460, 0.262866},
    {-0.864188, 0.442863, -0.238856},
    {-0.951056, 0.162460, -0.262866},
    {-0.809017, 0.309017, -0.500000},
    {-0.864188, -0.442863, -0.238856},
    {-0.951056, -0.162460, -0.262866},
    {-0.809017, -0.309017, -0.500000},
    {-0.681718, 0.147621, -0.716567},
    {-0.681718, -0.147621, -0.716567},
    {-0.850651, 0.000000, -0.525731},
    {-0.688191, 0.587785, -0.425325},
    {-0.587785, 0.425325, -0.688191},
    {-0.425325, 0.688191, -0.587785},
    {-0.425325, -0.688191, -0.587785},
    {-0.587785, -0.425325, -0.688191},
    {-0.688191, -0.587785, -0.425325},
};

int DirToByte(const vec3_t &dir)
{
    int     i, best;
    float   d, bestd;

    if (!dir) {
        return 0;
    }

    bestd = 0;
    best = 0;
    for (i = 0; i < NUMVERTEXNORMALS; i++) {
        d = DotProduct(dir, bytedirs[i]);
        if (d > bestd) {
            bestd = d;
            best = i;
        }
    }

    return best;
}

#if 0
void ByteToDir(int index, vec3_t dir)
{
    if (index < 0 || index >= NUMVERTEXNORMALS) {
        Com_Error(ErrorType::Fatal, "ByteToDir: illegal index");
    }

    VectorCopy(bytedirs[index], dir);
}
#endif