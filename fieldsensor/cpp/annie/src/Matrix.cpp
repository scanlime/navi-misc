#include "../include/Matrix.h"
#include "../include/Exception.h"

#include <cstdio>

using namespace std;

namespace annie
{

Matrix::Matrix(int m,int n)
{
	_m = m;
	_n = n;
	_M = NULL;
	_allocateMatrix(m,n);
}

Matrix::Matrix(Matrix &M)
{
	_M = NULL;
	_allocateMatrix(M._m,M._n);
	int i,j;
	for (i=0;i<_m;i++)
		for (j=0;j<_n;j++)
			_M[i][j]=M._M[i][j];
}

Matrix::Matrix(Matrix *M)
{	
	_M = NULL;
	_allocateMatrix(M->_m,M->_n);
	int i,j;
	for (i=0;i<_m;i++)
		for (j=0;j<_n;j++)
			_M[i][j]=M->_M[i][j];
}

Matrix::~Matrix()
{	_freeMatrix();	}

void Matrix::_allocateMatrix(int m, int n)
{
	if (_M)
		_freeMatrix();
	_m = m;
	_n = n;
	int i,j;
	_M = new real*[_m];
	for (i=0;i<m;i++)
	{
		for (j=0;j<n;j++)
			_M[i]=new real[n];
	}
}

void Matrix::_freeMatrix()
{
	int i;
	real *temp;
	if (_M)
	{
		for (i=0;i<_m;i++)
		{
			temp = _M[i];
			delete temp;
		}
		delete _M;
	}
	_M = NULL;

}

void Matrix::print()
{
	int i,j;
	if (_M)
	{
		for (i=0;i<_m;i++)
		{
			for (j=0;j<_n;j++)
				printf("%.2f ",_M[i][j]);
			printf("\n");
		}
	}
	else
		printf("NULL\n");
}

real&
Matrix::elementAt(int i,int j)
{
	if (i<0 || i>=_m || j<0 || j>=_n)
		throw Exception("Matrix::elementAt() - Invalid position in matrix.");
	if (!_M)
		_allocateMatrix(_m,_n);
	return _M[i][j];
}

Matrix*
Matrix::inverse()
{
	if (_m != _n)
		throw Exception("Matrix::inverse() - Not a square matrix.");
	int i,j,k;
	real anchor,factor;
	int n = _m;
	Matrix *I = Matrix::Identity(n);
	Matrix *M = new Matrix(this);

	for (i=0;i<n-1;i++)
	{
		
		for (k=i; k<n && M->_M[k][i]==0.0;k++); // EMPTY LOOP IS INTENDED!
		if (k==n)
		{
			delete I;
			delete M;
			throw Exception("Matrix::inverse() - Inverse doesn't exist.");
		} 
		else if (k!=i)
		{
			M->swapRows(k,i);
			I->swapRows(k,i);
		}
		anchor = M->_M[i][i];
		for (j=i+1;j<n;j++)
		{
			factor = M->_M[j][i]/anchor;
			for (k=0;k<n;k++)
			{
				M->_M[j][k] -= M->_M[i][k]*factor;
				I->_M[j][k] -= I->_M[i][k]*factor;
			}
		}
	}
	for (i=n-1;i>=0;i--)
	{
		if (M->_M[i][i]!=1.0)
		{
			factor = M->_M[i][i];
			for (k=0;k<n;k++)
			{
				M->_M[i][k]/=factor;
				I->_M[i][k]/=factor;
			}
		}
		for (j=n-1;j>i;j--)
		{
			if (M->_M[i][j]==0)
				continue;
			factor = M->_M[i][j] / M->_M[j][j];
			M->_M[i][j]=0;
			for (k=0;k<n;k++)
			{
				I->_M[i][k] -= I->_M[j][k]*factor;
			}
		}
	}
	delete M; return I;
}

Matrix*
Matrix::transpose()
{
	int i,j;
	Matrix *T = new Matrix(_n,_m);
	for (i=0;i<_m;i++)
		for (j=0;j<_n;j++)
			T->_M[j][i] = _M[i][j];
	return T;
}

Matrix*
Matrix::multiply(Matrix &M2)
{
	if (_n != M2._m)
	{
		string error("Matrix::multiply() - Supplied matrix does not have dimensions valid for multiplication with this matrix.");
		throw Exception(error);
	}

	Matrix *P = new Matrix(_m,M2._n);
	int i,j,k;

	for (i=0;i<_m;i++)
		for (j=0;j<M2._n;j++)
		{
			P->_M[i][j]=0.0;
			for (k=0;k<_n;k++)
				P->_M[i][j] += _M[i][k] * M2._M[k][j];
		}
	return P;
}

Matrix *
Matrix::multiply(Matrix *other)
{	return multiply(*other);	}

Matrix *
Matrix::Identity(int n)
{
	Matrix *I = new Matrix(n,n);
	int i,j;
	for (i=0;i<n;i++)
		for (j=0;j<n;j++)
			I->_M[i][j] = (i==j)?(real)1.0:(real)0.0;
	return I;
}

void
Matrix::multiply(real f)
{
	int i,j;
	for (i=0;i<_m;i++)
		for (j=0;j<_n;j++)
			_M[i][j]*=f;
}

void
Matrix::swapRows(int i, int j)
{
	real temp;
	int p;
	for (p=0;p<_n;p++)
	{
		temp = _M[i][p];
		_M[i][p] = _M[j][p];
		_M[j][p] = temp;
	}
}

ostream& operator << (ostream& s, Matrix &M)
{
	int i,j;
	char temp[4096];
	if (M._M)
	{
		for (i=0;i<M._m;i++)
		{
			for (j=0;j<M._n;j++)
			{
				sprintf(temp,"%.2f ",M._M[i][j]);
				s<<temp;
			}
			s<<endl;
		}
	}
	else
		s<<"NULL";
	s<<endl;
	return s;
}

}; //namespace annie

