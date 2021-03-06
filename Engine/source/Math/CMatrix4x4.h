//--------------------------------------------------------------------------------------
// Matrix4x4 class (cut down version) to hold matrices for 3D
//--------------------------------------------------------------------------------------
// Code in .cpp file
#pragma once

#include "CVector3.h"


// Matrix class
class CMatrix4x4
{
// Concrete class - public access
public:
    CMatrix4x4() {}
	
	explicit CMatrix4x4(const CVector3& quat, const float& w, const CVector3& pos = { 0.0f, 0.0f, 0.0f }, 
        const CVector3& scale = {1.0f, 1.0f, 1.0f});

	
    CMatrix4x4
    (
        const float elt00, const float elt01, const float elt02, const float elt03,
        const float elt10, const float elt11, const float elt12, const float elt13,
        const float elt20, const float elt21, const float elt22, const float elt23,
        const float elt30, const float elt31, const float elt32, const float elt33
    );
	
	// Matrix elements
	float e00, e01, e02, e03;
	float e10, e11, e12, e13;
	float e20, e21, e22, e23;
	float e30, e31, e32, e33;

    
    /*-----------------------------------------------------------------------------------------
        Member functions
    -----------------------------------------------------------------------------------------*/

	// Set a single row (range 0-3) of the matrix using a CVector3. Fourth element left unchanged
    // Can be used to set position or x,y,z axes in a matrix
    void SetRow(int iRow, const CVector3& v);

    // Get a single row (range 0-3) of the matrix into a CVector3. Fourth element is ignored
    // Can be used to access position or x,y,z axes from a matrix
    CVector3 GetRow(int iRow) const;

    // Initialise this matrix with a pointer to 16 floats 
    void SetValues(float* matrixValues)  { *this = *reinterpret_cast<CMatrix4x4*>(matrixValues); }

 
    // Helper functions
    CVector3 GetXAxis() const { return GetRow(0); }
    CVector3 GetYAxis() const { return GetRow(1); }
    CVector3 GetZAxis() const { return GetRow(2); }
    CVector3 GetPosition() const  { return GetRow(3); }
    CVector3 GetEulerAngles();
    CVector3 GetScale() const  { return { Length(GetXAxis()), Length(GetYAxis()) , Length(GetZAxis()) }; }

    // Post-multiply this matrix by the given one
    CMatrix4x4& operator*=(const CMatrix4x4& m);

    // Make this matrix an affine 3D transformation matrix to face from current position to given
    // target (in the Z direction). Can pass up vector for the constructed matrix and specify
    // handedness (right-handed Z axis will face away from target)
    // Will retain the matrix's current scaling
    void FaceTarget(const CVector3& target);


    // Transpose the matrix (rows become columns). There are two ways to store a matrix, by rows or by columns.
    // Different apps use different methods. Use Transpose to swap when necessary.
    void Transpose();
};


/*-----------------------------------------------------------------------------------------
    Operators
-----------------------------------------------------------------------------------------*/

// Matrix-matrix multiplication
CMatrix4x4 operator*(const CMatrix4x4& m1, const CMatrix4x4& m2);


/*-----------------------------------------------------------------------------------------
  Non-member functions
-----------------------------------------------------------------------------------------*/

// The following functions create a new matrix holding a particular transformation
// They can be used as temporaries in calculations, e.g.
//     CMatrix4x4 m = MatrixScaling( 3.0f ) * MatrixTranslation( CVector3(10.0f, -10.0f, 20.0f) );

// Return an identity matrix
CMatrix4x4 MatrixIdentity();

// Return a translation matrix of the given vector
CMatrix4x4 MatrixTranslation(const CVector3& t);


// Return an X-axis rotation matrix of the given angle (in radians)
CMatrix4x4 MatrixRotationX(float x);

// Return a Y-axis rotation matrix of the given angle (in radians)
CMatrix4x4 MatrixRotationY(float y);

// Return a Z-axis rotation matrix of the given angle (in radians)
CMatrix4x4 MatrixRotationZ(float z);


// Return a matrix that is a scaling in X,Y and Z of the values in the given vector
CMatrix4x4 MatrixScaling(const CVector3& s);

// Return a matrix that is a uniform scaling of the given amount
CMatrix4x4 MatrixScaling(const float s);



// Return the inverse of given matrix assuming that it is an affine matrix
// Advanced calulation needed to get the view matrix from the camera's positioning matrix
CMatrix4x4 InverseAffine(const CMatrix4x4& m);

