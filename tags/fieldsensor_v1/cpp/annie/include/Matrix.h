#ifndef _MATRIX_H
#define _MATRIX_H

#include "defines.h"

namespace annie
{

/** A class for 2-dimensional matrices.
  * This probably isn't the most efficient way to do matrix operations,
  * use of optimized matrix libraries would be preferable, but this should
  * suffice for most applications
  */
class Matrix
{
private:
	int _m,_n;
	real **_M;

	void _allocateMatrix(int m, int n);
	void _freeMatrix();
public:
	/** Create a matrix
	  * Note that the elements of the matrix will be random, they have
	  * NOT been initialized to zero or any other value
	  * @param m Number of rows
	  * @param n Number of columns
	  */
	Matrix(int m,int n);

	/// Copy constructor
	Matrix(Matrix &M);

	/// Similar to the copy constructor
	Matrix(Matrix *M);

	virtual ~Matrix();

	/// Prints the matrix on the screen
	virtual void print();

	/** Value of a given cell in the matrix
	  * @param i The row in the matrix
	  * @param j The column in the matrix
	  * @return Value of the element at the ith row and jth column
	  */
	virtual real& elementAt(int i,int j);

	/** Multiplies the matrix with the given matrix and returns
	  * a new matrix.
	  * You will have to deallocate the product matrix with delete.
	  * @param other The matrix to multiply with
	  * @return The product matrix
	  * @throws Exception if the matrices cannot be multiplied
	  */
	virtual Matrix *multiply(Matrix &other);

	/** Multiplies the matrix with the given matrix and returns
	  * a new matrix.
	  * You will have to deallocate the product matrix with delete.
	  * @param other The matrix to multiply with
	  * @return The product matrix
	  * @throws Exception if the matrices cannot be multiplied
	  */
	virtual Matrix *multiply(Matrix *other);

	/// Multiplies the matrix with the scalar quantity provided
	virtual void multiply(real f);

	/// Swaps the ith and jth row of the matrix
	virtual void swapRows(int i, int j);

	/** Finds the transpose of the matrix
	  * You will have to deallocate the tranpose matrix with delete.
	  * @return A new matrix which is the transpose of the current matrix
	  */
	virtual Matrix *transpose();

	/** Finds the inverse of the matrix
	  * You will have to deallocate the inverse matrix with delete.
	  * Uses the gaussian elmination technique to calculate the inverse
	  * @return A new matrix which is the inverse of this matrix
	  * @throws Exception if the matrix is not invertible.
	  */
	virtual Matrix *inverse();

	/** Returns the identity matrix of size nxn.
	  * You will have to deallocate the identity matrix with delete
	  * @param n The number of rows/columns in the matrix
	  * @return The square identity matrix of nxn
	  */
	static Matrix *Identity(int n);

	/// Prints matrix to the provided output stream
	friend std::ostream& operator << (std::ostream& s, Matrix &M);
};
}; //namespace annie
#endif // define _MATRIX_H

