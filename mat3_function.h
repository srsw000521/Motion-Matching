#pragma once

#include "vector3d.h"

void mat3_SetIdentity(float mat[][3]);
void mat3_SetTranslation(float mat[][3], float x, float y);
void mat3_SetScale(float mat[][3], float x, float y);
void mat3_Multiply(float mat[][3], float A[][3], float B[][3]);
Vector3f mat3_Multiply(float mat[][3], Vector3f in);