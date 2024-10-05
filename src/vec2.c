#include "vec2.h"

#include <stddef.h>
#include <stdio.h>
#include <math.h>

Vec2 Vec2_Create(double x, double y){
	Vec2 ret_value = {x, y};
	return ret_value;
}

Vec2 Vec2_Add(Vec2 arg1, Vec2 arg2){
	Vec2 ret_value = {
		arg1.x + arg2.x,
		arg1.y + arg2.y
	};
	return ret_value;
}

Vec2 Vec2_Sub(Vec2 arg1, Vec2 arg2){
	Vec2 ret_value = {
		arg1.x - arg2.x,
		arg1.y - arg2.y
	};
	return ret_value;
}

Vec2 Vec2_Mul(Vec2 arg1, double value){
	Vec2 ret_value = {
		arg1.x * value,
		arg1.y * value
	};
	return ret_value;
}

Vec2 Vec2_MultiplyByMatrix(Vec2 arg1, double *matrix){
	if(matrix == NULL) return Vec2_Create(0, 0);

	Vec2 ret_value = {
		arg1.x*matrix[0] + matrix[1]*arg1.y,
		arg1.x*matrix[2] + matrix[3]*arg1.y
	};

	return ret_value;
}

double Vec2_Length(Vec2 arg1){
	return sqrt(Vec2_LengthSqr(arg1));
}

double Vec2_LengthSqr(Vec2 arg1){
	return (arg1.x*arg1.x + arg1.y*arg1.y);
}

Vec2 Vec2_Normalize(Vec2 arg1){
	double length = Vec2_Length(arg1);

	if(length == 0) return Vec2_Create(0, 0);

	Vec2 ret_value = {
		arg1.x / length,
		arg1.y / length
	};

	return ret_value;
}
