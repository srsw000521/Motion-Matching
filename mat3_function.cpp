//#include "stdafx.h"
#include "mat3_function.h"


void mat3_SetIdentity(float mat[][3])
{
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
		{
			if(i == j)	mat[i][j] = 1;
			else		mat[i][j] = 0;
		}
}

void mat3_SetTranslation(float mat[][3], float x, float y)
{
	mat[0][2] = x;
	mat[1][2] = y;
}

void mat3_SetScale(float mat[][3], float x, float y)
{
	mat[0][0] = x;
	mat[1][1] = y;
}

void mat3_Multiply(float mat[][3], float A[][3], float B[][3])
{
	for(int i=0; i<3; i ++)
		for(int j=0; j<3; j++)
		{
			mat[i][j] = 0.0f;
			for(int k=0; k<3; k++)
				mat[i][j] += A[i][k]*B[k][j];
		}
}

Vector3f mat3_Multiply(float mat[][3], Vector3f in)
{
	Vector3f out;
	out.x = mat[0][0]*in.x + mat[0][1]*in.y + mat[0][2];
	out.y = mat[1][0]*in.x + mat[1][1]*in.y + mat[1][2];
	out.z = 0.0f;
	return out;

}