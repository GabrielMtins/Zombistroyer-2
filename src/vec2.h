#ifndef VEC2_H
#define VEC2_H

/* Vector2 structure */

typedef struct{
	double x, y;
} Vec2;

/* Create a Vec2 with given x and y values */
Vec2 Vec2_Create(double x, double y);

/* Sums two vectors and returns its values */
Vec2 Vec2_Add(Vec2 arg1, Vec2 arg2);

/* Subtracts two vectors and returns its values */
Vec2 Vec2_Sub(Vec2 arg1, Vec2 arg2);

/* Multiply the x and y values of a vector by a number and returns its value */
Vec2 Vec2_Mul(Vec2 arg1, double value);

/* Multiply a vector by a 2x2 matrix
 * The matrix is an array that is organized like this:
 * | a  b | 
 * |      |  and its equivalent to:  double arr[4] = {a, b, c, d};
 * | c  d |
*/
Vec2 Vec2_MultiplyByMatrix(Vec2 arg1, double *matrix);

double Vec2_Length(Vec2 arg1);

double Vec2_LengthSqr(Vec2 arg1);

/* Normalize a vector and returns its value */
Vec2 Vec2_Normalize(Vec2 arg1);

#endif
