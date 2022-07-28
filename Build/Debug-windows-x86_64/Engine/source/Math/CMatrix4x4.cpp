//--------------------------------------------------------------------------------------
// Matrix4x4 class (cut down version) to hold matrices for 3D
//--------------------------------------------------------------------------------------
#include "ppch.h"
#include "CMatrix4x4.h"


/*-----------------------------------------------------------------------------------------
    Member functions
-----------------------------------------------------------------------------------------*/

CMatrix4x4::CMatrix4x4(const CVector3& quat, const float& w, const CVector3& pos, const CVector3& scale)
{
	// Code from the Tank assignment
    // Efficiently precalculate some values from the quaternion
    float xx = 2 * quat.x;
    float yy = 2 * quat.y;
    float zz = 2 * quat.z;
    float xy = xx * quat.y;
    float yz = yy * quat.z;
    float zx = zz * quat.x;
    float wx = w * xx;
    float wy = w * yy;
    float wz = w * zz;
    xx *= quat.x;
    yy *= quat.y;
    zz *= quat.z;

    // Fill upper 3x3 matrix, combining scaling with rotation values from the quaternion
    e00 = scale.x * (1 - yy - zz);
    e01 = scale.x * (xy + wz);
    e02 = scale.x * (zx - wy);
    e03 = 0.0f; // Add 0's in fourth column

    e10 = scale.y * (xy - wz);
    e11 = scale.y * (1 - xx - zz);
    e12 = scale.y * (yz + wx);
    e13 = 0.0f;

    e20 = scale.z * (zx + wy);
    e21 = scale.z * (yz - wx);
    e22 = scale.z * (1 - xx - yy);
    e23 = 0.0f;

    // Put pos (translation) in bottom row
    e30 = pos.x;
    e31 = pos.y;
    e32 = pos.z;
    e33 = 1.0f;
}


CMatrix4x4::CMatrix4x4(const float elt00, const float elt01, const float elt02, const float elt03, const float elt10, const float elt11, const float elt12, const float elt13, const float elt20, const float elt21, const float elt22, const float elt23, const float elt30, const float elt31, const float elt32, const float elt33)
{
    e00 = elt00;
    e01 = elt01;
    e02 = elt02;
    e03 = elt03;

    e10 = elt10;
    e11 = elt11;
    e12 = elt12;
    e13 = elt13;

    e20 = elt20;
    e21 = elt21;
    e22 = elt22;
    e23 = elt23;

    e30 = elt30;
    e31 = elt31;
    e32 = elt32;
    e33 = elt33;
}

CMatrix4x4::CMatrix4x4(const CVector3& position, const CVector3& angles, const CVector3& scale)
{
    // First build rotation matrix
    MakeRotation(angles);

    // Scale matrix
    e00 *= scale.x;
    e01 *= scale.x;
    e02 *= scale.x;

    e10 *= scale.y;
    e11 *= scale.y;
    e12 *= scale.y;

    e20 *= scale.z;
    e21 *= scale.z;
    e22 *= scale.z;

    // Put position (translation) in bottom row
    e30 = position.x;
    e31 = position.y;
    e32 = position.z;
}

CMatrix4x4::CMatrix4x4(const CVector3& position)
{
    // Take most elements from identity
    e00 = 1.0f;
    e01 = 0.0f;
    e02 = 0.0f;
    e03 = 0.0f;

    e10 = 0.0f;
    e11 = 1.0f;
    e12 = 0.0f;
    e13 = 0.0f;

    e20 = 0.0f;
    e21 = 0.0f;
    e22 = 1.0f;
    e23 = 0.0f;

    // Put position (translation) in bottom row
    e30 = position.x;
    e31 = position.y;
    e32 = position.z;
    e33 = 1.0f;
}

CMatrix4x4::CMatrix4x4(const CVector3& v0, const CVector3& v1, const CVector3& v2, const CVector3& v3, const bool bRows)
{
    if (bRows)
    {
        e00 = v0.x;
        e01 = v0.y;
        e02 = v0.z;
        e03 = 0.0f;

        e10 = v1.x;
        e11 = v1.y;
        e12 = v1.z;
        e13 = 0.0f;

        e20 = v2.x;
        e21 = v2.y;
        e22 = v2.z;
        e23 = 0.0f;

        e30 = v3.x;
        e31 = v3.y;
        e32 = v3.z;
        e33 = 1.0f;
    }
    else
    {
        e00 = v0.x;
        e10 = v0.y;
        e20 = v0.z;

        e01 = v1.x;
        e11 = v1.y;
        e21 = v1.z;

        e02 = v2.x;
        e12 = v2.y;
        e22 = v2.z;

        e03 = v3.x;
        e13 = v3.y;
        e23 = v3.z;

        e30 = 0.0f;
        e31 = 0.0f;
        e32 = 0.0f;
        e33 = 1.0f;
    }
}

void CMatrix4x4::SetRotation(CVector3 rotation)
{
    MatrixRotationX(GetEulerAngles().x * rotation.x);
    MatrixRotationY(GetEulerAngles().y * rotation.y);
    MatrixRotationZ(GetEulerAngles().z * rotation.z);
}

// Set a single row (range 0-3) of the matrix using a CVector3. Fourth element left unchanged
// Can be used to set pos or x,y,z axes in a matrix
void CMatrix4x4::SetRow(int iRow, const CVector3& v)
{
    float* pfElts = &e00 + iRow * 4;
    pfElts[0] = v.x;
    pfElts[1] = v.y;
    pfElts[2] = v.z;
}

// Get a single row (range 0-3) of the matrix into a CVector3. Fourth element is ignored
// Can be used to access pos or x,y,z axes from a matrix
CVector3 CMatrix4x4::GetRow(int iRow) const
{
    const float* pfElts = &e00 + iRow * 4;
    return CVector3(pfElts[0], pfElts[1], pfElts[2]);
}


// Post-multiply this matrix by the given one
CMatrix4x4& CMatrix4x4::operator*=(const CMatrix4x4& m)
{
    if (this == &m)
    {
        // Special case of multiplying by self - no copy optimisations so use binary version
        *this = m * m;
    }
    else
    {
        float t0, t1, t2;

        t0 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20 + e03 * m.e30;
        t1 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21 + e03 * m.e31;
        t2 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22 + e03 * m.e32;
        e03 = e00 * m.e03 + e01 * m.e13 + e02 * m.e23 + e03 * m.e33;
        e00 = t0;
        e01 = t1;
        e02 = t2;

        t0 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20 + e13 * m.e30;
        t1 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21 + e13 * m.e31;
        t2 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22 + e13 * m.e32;
        e13 = e10 * m.e03 + e11 * m.e13 + e12 * m.e23 + e13 * m.e33;
        e10 = t0;
        e11 = t1;
        e12 = t2;

        t0 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20 + e23 * m.e30;
        t1 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21 + e23 * m.e31;
        t2 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22 + e23 * m.e32;
        e23 = e20 * m.e03 + e21 * m.e13 + e22 * m.e23 + e23 * m.e33;
        e20 = t0;
        e21 = t1;
        e22 = t2;

        t0 = e30 * m.e00 + e31 * m.e10 + e32 * m.e20 + e33 * m.e30;
        t1 = e30 * m.e01 + e31 * m.e11 + e32 * m.e21 + e33 * m.e31;
        t2 = e30 * m.e02 + e31 * m.e12 + e32 * m.e22 + e33 * m.e32;
        e33 = e30 * m.e03 + e31 * m.e13 + e32 * m.e23 + e33 * m.e33;
        e30 = t0;
        e31 = t1;
        e32 = t2;
    }
    return *this;
}


/*-----------------------------------------------------------------------------------------
    Operators
-----------------------------------------------------------------------------------------*/

// Matrix-matrix multiplication
CMatrix4x4 operator*(const CMatrix4x4& m1, const CMatrix4x4& m2)
{
    CMatrix4x4 mOut;

    mOut.e00 = m1.e00*m2.e00 + m1.e01*m2.e10 + m1.e02*m2.e20 + m1.e03*m2.e30;
    mOut.e01 = m1.e00*m2.e01 + m1.e01*m2.e11 + m1.e02*m2.e21 + m1.e03*m2.e31;
    mOut.e02 = m1.e00*m2.e02 + m1.e01*m2.e12 + m1.e02*m2.e22 + m1.e03*m2.e32;
    mOut.e03 = m1.e00*m2.e03 + m1.e01*m2.e13 + m1.e02*m2.e23 + m1.e03*m2.e33;

    mOut.e10 = m1.e10*m2.e00 + m1.e11*m2.e10 + m1.e12*m2.e20 + m1.e13*m2.e30;
    mOut.e11 = m1.e10*m2.e01 + m1.e11*m2.e11 + m1.e12*m2.e21 + m1.e13*m2.e31;
    mOut.e12 = m1.e10*m2.e02 + m1.e11*m2.e12 + m1.e12*m2.e22 + m1.e13*m2.e32;
    mOut.e13 = m1.e10*m2.e03 + m1.e11*m2.e13 + m1.e12*m2.e23 + m1.e13*m2.e33;

    mOut.e20 = m1.e20*m2.e00 + m1.e21*m2.e10 + m1.e22*m2.e20 + m1.e23*m2.e30;
    mOut.e21 = m1.e20*m2.e01 + m1.e21*m2.e11 + m1.e22*m2.e21 + m1.e23*m2.e31;
    mOut.e22 = m1.e20*m2.e02 + m1.e21*m2.e12 + m1.e22*m2.e22 + m1.e23*m2.e32;
    mOut.e23 = m1.e20*m2.e03 + m1.e21*m2.e13 + m1.e22*m2.e23 + m1.e23*m2.e33;

    mOut.e30 = m1.e30*m2.e00 + m1.e31*m2.e10 + m1.e32*m2.e20 + m1.e33*m2.e30;
    mOut.e31 = m1.e30*m2.e01 + m1.e31*m2.e11 + m1.e32*m2.e21 + m1.e33*m2.e31;
    mOut.e32 = m1.e30*m2.e02 + m1.e31*m2.e12 + m1.e32*m2.e22 + m1.e33*m2.e32;
    mOut.e33 = m1.e30*m2.e03 + m1.e31*m2.e13 + m1.e32*m2.e23 + m1.e33*m2.e33;

    return mOut;
}



/*-----------------------------------------------------------------------------------------
    Non-member functions
-----------------------------------------------------------------------------------------*/

// The following functions create a new matrix holding a particular transformation
// They can be used as temporaries in calculations, e.g.
//     CMatrix4x4 m = MatrixScaling( 3.0f ) * MatrixTranslation( CVector3(10.0f, -10.0f, 20.0f) );

// Return an identity matrix
CMatrix4x4 MatrixIdentity()
{
    return CMatrix4x4{ 1, 0, 0, 0,
                       0, 1, 0, 0,
                       0, 0, 1, 0,
                       0, 0, 0, 1 };
}

// Return a translation matrix of the given vector
CMatrix4x4 MatrixTranslation(const CVector3& t)
{
    return CMatrix4x4  { 1,   0,   0,  0,
                         0,   1,   0,  0,
                         0,   0,   1,  0,
                       t.x, t.y, t.z,  1 };
}

void CMatrix4x4::MakeRotation(CVector3 angles)
{
    float sX, cX, sY, cY, sZ, cZ;
    SinCos(angles.x, &sX, &cX);
    SinCos(angles.y, &sY, &cY);
    SinCos(angles.z, &sZ, &cZ);

    e00 = cZ * cY + sZ * sX * sY;
    e01 = sZ * cX;
    e02 = -cZ * sY + sZ * sX * cY;
    e03 = 0.0f;

    e10 = -sZ * cY + cZ * sX * sY;
    e11 = cZ * cX;
    e12 = sZ * sY + cZ * sX * cY;
    e13 = 0.0f;

    e20 = cX * sY;
    e21 = -sX;
    e22 = cX * cY;
}


// Return an X-axis rotation matrix of the given angle (in radians)
CMatrix4x4 MatrixRotationX(float x)
{
    float sX = std::sin(x);
    float cX = std::cos(x);

    return CMatrix4x4{ 1,   0,   0,  0,
                       0,  cX,  sX,  0,
                       0, -sX,  cX,  0,
                       0,   0,   0,  1 };
}

// Return a Y-axis rotation matrix of the given angle (in radians)
CMatrix4x4 MatrixRotationY(float y)
{
    float sY = std::sin(y);
    float cY = std::cos(y);

    return CMatrix4x4{ cY,   0, -sY,  0,
                        0,   1,   0,  0,
                       sY,   0,  cY,  0,
                        0,   0,   0,  1 };
}

// Return a Z-axis rotation matrix of the given angle (in radians)
CMatrix4x4 MatrixRotationZ(float z)
{
    float sZ = std::sin(z);
    float cZ = std::cos(z);

    return CMatrix4x4{ cZ,  sZ,  0,  0,
                      -sZ,  cZ,  0,  0,
                        0,   0,  1,  0,
                        0,   0,  0,  1 };
}


// Return a matrix that is a scaling in X,Y and Z of the values in the given vector
CMatrix4x4 MatrixScaling(const CVector3& s)
{
    return CMatrix4x4{ s.x,   0,   0,  0,
                       0,   s.y,   0,  0,
                       0,     0, s.z,  0,
                       0,     0,   0,  1 };
}

// Return a matrix that is a uniform scaling of the given amount
CMatrix4x4 MatrixScaling(const float s)
{
    return CMatrix4x4{ s, 0, 0, 0,
                       0, s, 0, 0,
                       0, 0, s, 0,
                       0, 0, 0, 1 };
}


// Return the inverse of given matrix assuming that it is an affine matrix
// Advanced calulation needed to get the view matrix from the camera's posing matrix
CMatrix4x4 InverseAffine(const CMatrix4x4& m)
{
    CMatrix4x4 mOut;

    // Calculate determinant of upper left 3x3
    float det0 = m.e11*m.e22 - m.e12*m.e21;
    float det1 = m.e12*m.e20 - m.e10*m.e22;
    float det2 = m.e10*m.e21 - m.e11*m.e20;
    float det = m.e00*det0 + m.e01*det1 + m.e02*det2;

    // Calculate inverse of upper left 3x3
    float invDet = 1.0f / det;
    mOut.e00 = invDet * det0;
    mOut.e10 = invDet * det1;
    mOut.e20 = invDet * det2;

    mOut.e01 = invDet * (m.e21*m.e02 - m.e22*m.e01);
    mOut.e11 = invDet * (m.e22*m.e00 - m.e20*m.e02);
    mOut.e21 = invDet * (m.e20*m.e01 - m.e21*m.e00);

    mOut.e02 = invDet * (m.e01*m.e12 - m.e02*m.e11);
    mOut.e12 = invDet * (m.e02*m.e10 - m.e00*m.e12);
    mOut.e22 = invDet * (m.e00*m.e11 - m.e01*m.e10);

    // Transform negative translation by inverted 3x3 to get inverse
    mOut.e30 = -m.e30*mOut.e00 - m.e31*mOut.e10 - m.e32*mOut.e20;
    mOut.e31 = -m.e30*mOut.e01 - m.e31*mOut.e11 - m.e32*mOut.e21;
    mOut.e32 = -m.e30*mOut.e02 - m.e31*mOut.e12 - m.e32*mOut.e22;

    // Fill in right column for affine matrix
    mOut.e03 = 0.0f;
    mOut.e13 = 0.0f;
    mOut.e23 = 0.0f;
    mOut.e33 = 1.0f;

    return mOut;
}


// Make this matrix an affine 3D transformation matrix to face from current pos to given target (in the Z direction)
// Will retain the matrix's current scaling
void CMatrix4x4::FaceTarget(const CVector3& target)
{
    // Use cross product of target direction and up vector to give third axis, then orthogonalise
    CVector3 axisX, axisY, axisZ;
    axisZ = Normalise(target - GetPosition());
    if (IsZero(Length(axisZ))) return;
    axisX = Normalise(Cross({0, 1, 0}, axisZ));
    if (IsZero(Length(axisX))) return;
    axisY = Cross(axisZ, axisX); // Will already be normalised

    // Set rows of matrix, restoring existing scale. pos will be unchanged, 4th column
    // taken from unit matrix
    CVector3 scale = GetScale();
    SetRow(0, axisX * scale.x);
    SetRow(1, axisY * scale.y);
    SetRow(2, axisZ * scale.z);
}


// Return the rotation stored in this matrix as Euler angles
CVector3 CMatrix4x4::GetEulerAngles()
{
	// Calculate matrix scaling
	float scaleX = sqrt( e00*e00 + e01*e01 + e02*e02 );
	float scaleY = sqrt( e10*e10 + e11*e11 + e12*e12 );
	float scaleZ = sqrt( e20*e20 + e21*e21 + e22*e22 );

	// Calculate inverse scaling to extract rotational values only
	float invScaleX = 1.0f / scaleX;
	float invScaleY = 1.0f / scaleY;
	float invScaleZ = 1.0f / scaleZ;

	float sX, cX, sY, cY, sZ, cZ;

    sX = -e21 * invScaleZ;
    cX = sqrt( 1.0f - sX*sX );

    // If no gimbal lock...
    if (abs(cX) > 0.001f)
    {
	    float invCX = 1.0f / cX;
	    sZ = e01 * invCX * invScaleX;
	    cZ = e11 * invCX * invScaleY;
	    sY = e20 * invCX * invScaleZ;
	    cY = e22 * invCX * invScaleZ;
    }
    else
    {
	    // Gimbal lock - force Z angle to 0
	    sZ = 0.0f;
	    cZ = 1.0f;
	    sY = -e02 * invScaleX;
	    cY =  e00 * invScaleX;
    }

	return { atan2(sX, cX), atan2(sY, cY), atan2(sZ, cZ) };
}



// Transpose the matrix (rows become columns). There are two ways to store a matrix, by rows or by columns.
// Different apps use different methods. Use Transpose to swap when necessary.
void CMatrix4x4::Transpose()
{
    std::swap(e01, e10);
    std::swap(e02, e20);
    std::swap(e03, e30);
    std::swap(e12, e21);
    std::swap(e13, e31);
    std::swap(e23, e32);
}

