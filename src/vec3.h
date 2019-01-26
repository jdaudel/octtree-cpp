//
//  vec3.h
//

#ifndef _VEC3_H
#define _VEC3_H

#include "Types.h"
#include <math.h>
#include <assert.h>

class Plane;
class Orientation;

class vec3
{
public:
    
    vec3( float32 x, float32 y, float32 z )
    {
        set( x, y, z );
    }
    
    vec3()
    {}
    
    void set( float32 x, float32 y, float32 z )
    {
        mX = x;
        mY = y;
        mZ = z;
    }
    
    void clear()
    {
        mX = 0;
        mY = 0;
        mZ = 0;
    }
    
    float32& operator[]( int32 index )
    {
        float32* f;
        
        if (index == 0)
        {
            f = &mX;
        }
        else if (index == 1)
        {
            f = &mY;
        }
        else if (index == 2)
        {
            f = &mZ;
        }
        else
        {
			f = nullptr;
            assert( "Bad index" );
        }
        
        return *f;
    }

	float32 operator[]( int32 index ) const
	{
		float32 result = (const_cast< vec3& >( *this )[ index ]);
		return( result );
	}
	
    float32 mX;
    float32 mY;
    float32 mZ;
};

class vec3_64
{
public:
	
	vec3_64( const vec3& v )
	{
		mX = v.mX;
		mY = v.mY;
		mZ = v.mZ;
	}
	
	float64 mX;
	float64 mY;
	float64 mZ;
	
};

constexpr float64 kPi = 3.14159265358979323846;
const vec3 kOrigin3( 0, 0, 0 );

const vec3 kXUnit( 1, 0, 0 );
const vec3 kYUnit( 0, 1, 0 );
const vec3 kZUnit( 0, 0, 1 );
const vec3 kUnit[ 3 ] = { kXUnit, kYUnit, kZUnit };

bool isEqualVec( const vec3& left, const vec3& right );
bool isEqualVec( const vec3& left, const vec3& right, float64 tolerance );
vec3 addVec( const vec3& left, const vec3& right );
vec3 subVec( const vec3& left, const vec3& right );
vec3 multiplyVec( float32 left, const vec3& right );

inline vec3 operator+( const vec3& left, const vec3& right )
{
    return( addVec( left, right ));
}

inline vec3 operator-( const vec3& left, const vec3& right )
{
    return( subVec( left, right ));
}

inline vec3 operator*( double s, const vec3& right )
{
    return( multiplyVec( s, right ));
}

inline vec3 operator*( const vec3& left, float64 right )
{
	return( multiplyVec( right, left ));
}

inline vec3 operator*( const vec3& left, const vec3& right )
{
    return( vec3( left.mX * right.mX, left.mY * right.mY, left.mZ * right.mZ ));
}

inline vec3 operator/( const vec3& left, float64 right )
{
	return( vec3( left.mX / right, left.mY / right, left.mZ / right ));
}

inline bool operator==( const vec3& left, const vec3& right )
{
    return( isEqualVec( left, right ) );
}

inline bool operator!=( const vec3& left, const vec3& right )
{
	return( isEqualVec( left, right ) == false );
}

float64 lengthVec( const vec3& v );
vec3 normalizeVec( const vec3& v );
bool isNormalized( const vec3& v );
float64 dot( const vec3& left, const vec3& right );
vec3 div( const vec3& left, const vec3& right );

bool isParallel( const vec3& left, const vec3& right );
vec3 cross( const vec3& left, const vec3& right );
vec3 project( const vec3& v, const vec3& onto );
// just get the scalar result of the project
float32 projectScalar( const vec3& v, const vec3& onto );

// misc math functions
float64 degreesToRadians( float64 degrees );
float64 radiansToDegrees( float64 radians );
// check if a value being incremented in a direction (positive or negative)
// is at or past the end
bool isEndDirectional( float64 pos, float64 dir, float64 end );

vec3 rotateVec( const vec3& v, const vec3& axis, float32 degrees );

// linear interpolation
//  equivalent to interpolatePoly with poly = 0
// t [0, 1]
vec3 interpolate( const vec3& v1, const vec3& v2, float32 t );

// rotate a vector (keeping vector length constant)
vec3 interpolateRotation( const vec3& v1, const vec3& v2, float32 t );

// make a random direction
// result is normalized
vec3 randNormalVec();

// get angle of two vectors (vectors do not have to be normalized)
// returns in degrees
float64 getAngle( const vec3& v1, const vec3& v2 );

// two vector collisions
// will return [0, 1] if collision
// return < 0 if no collision
float64 getCollision( const vec3& p1, const vec3& v1, const vec3& p2, const vec3& v2, float64 dist );

// ray / sphere collision (i.e. v2 = 0)
// same return as above
float64 getCollision( const vec3& p1, const vec3& v1, const vec3& p2, float64 dist );

// ray / path seg collision
// path seg has a direction, shift, and beginning (r0) and ending radius (r1)
// rayP0 = ray start point, rayP1 = ray end point
// path0 = path start point
// path1 = path end point
// zVec = any length vector length of where the z path would end
// reflectPlane = generate plane that can be used for reflection. Plane normal will always point inward of path.
float64 getPathCollision( const vec3& rayP0, const vec3& rayP1, float64 rayRadius,
	const vec3& path0, const vec3& path1, const vec3& zDir, float64 r0, float64 r1,
	Plane* reflectPlane = nullptr, float64* closestPathT = nullptr, float64* closestRayT = nullptr );

class vec2
{
public:
    vec2( float32 x, float32 y )
    {
        mX = x;
        mY = y;
    }
    vec2()
    {}
    
    void set( float32 x, float32 y )
    {
        mX = x;
        mY = y;
    }
	
	float32& operator[]( size_t size )
	{
		return( reinterpret_cast< float32* >( this )[ size ]);
	}
	
	// temp?
	// sort by x, then y
	bool operator<( const vec2& right ) const
	{
		if (mX != right.mX)
		{
			return( mX < right.mX );
		}
		
		return( mY < right.mY );
	}
	
    float32 mX;
    float32 mY;
    
};

const vec2 kOrigin2( 0, 0 );

inline vec2 operator+( const vec2& left, const vec2& right )
{
	vec2 result( left.mX + right.mX, left.mY + right.mY );
	return( result );
}

inline vec2 operator-( const vec2& left, const vec2& right )
{
	vec2 result( left.mX - right.mX, left.mY - right.mY );
	return( result );
}

inline vec2 operator*( float32 left, const vec2& right )
{
	vec2 result( left * right.mX, left * right.mY );
	return( result );
}

inline bool operator==( const vec2& left, const vec2& right )
{
    return( left.mX == right.mX
           && left.mY == right.mY );
}

class vec4
{
public:
    
    vec4( float32 x, float32 y, float32 z, float32 w )
    {
        set( x, y, z, w );
    }
    
    vec4()
    {}
    
    void set( float32 x, float32 y, float32 z, float32 w )
    {
        mX = x;
        mY = y;
        mZ = z;
        mW = w;
    }
    
    void clear()
    {
        mX = 0;
        mY = 0;
        mZ = 0;
        mW = 0;
    }

	float32 operator[]( size_t index ) const
	{
		return reinterpret_cast< const float32* >( this )[ index ];
	}
	
	float32& operator[]( size_t index )
	{
		return reinterpret_cast< float32* >( this )[ index ];
	}
    
    float32 mX;
    float32 mY;
    float32 mZ;
    float32 mW;
};

const vec4 kOrigin4( 0, 0, 0, 0 );

inline vec4 operator+( const vec4& left, const vec4& right )
{
	vec4 result( left.mX + right.mX, left.mY + right.mY,
				left.mZ + right.mZ, left.mW + right.mW );
	return( result );
}

inline vec4 operator-( const vec4& left, const vec4& right )
{
	vec4 result( left.mX - right.mX, left.mY - right.mY,
				left.mZ - right.mZ, left.mW - right.mW );
	return( result );
}

inline vec4 operator*( float64 left, const vec4& right )
{
	vec4 result( left * right.mX, left * right.mY,
				left * right.mZ, left * right.mW );
	return( result );
}

vec4 interpolate( const vec4& v1, const vec4& v2, float32 t );


inline bool isEqualVec( const vec4& left, const vec4& right, float64 tolerance = 0 )
{
	return( fabs( left.mX - right.mX ) <= tolerance
		   && fabs( left.mY - right.mY ) <= tolerance
		   && fabs( left.mZ - right.mZ ) <= tolerance
		   && fabs( left.mW - right.mW ) <= tolerance );
}

inline bool operator==( const vec4& left, const vec4& right )
{
	return( isEqualVec( left, right, 0 ));
}

inline bool operator!=( const vec4& left, const vec4& right )
{
	return( !(left == right));
}

// class Plane
// plane is defined by 3 points
class Plane
{
public:
	
	Plane();
	// create a plane from 3 points
	Plane( const vec3& p1, const vec3& p2, const vec3& p3 );
	
	// create a plane from a point and its normal
	Plane( const vec3& p1, const vec3& normal );
	
	void set( const vec3& p1, const vec3& p2, const vec3& p3 );
	void set( const vec3& p1, const vec3& normal );
	
	vec3 getNormal() const;
	vec3 getPos() const;
	
	// point is on the 2d plane
	bool containsOnPlane( const vec3& p ) const;
	
	// point is on or in the half space pointed to by the normal
	bool containsInHalfSpace( const vec3& p ) const;
	
	vec3 project( const vec3& p ) const;
	
	float32 dist( const vec3& p ) const;
	
	// check when ray intersection
	// returns < 0 if does not intersect in positive line direction
	// will return 0, if ray is in plane
	// line0 = starting point of ray, dir = ray dir vector
	float64 rayIntersects( const vec3& line0, const vec3& dir ) const;

private:
	vec3 mPos;
	vec3 mNormal;
	
};

// Frustum is made of 4 3d planes
class Frustum
{
public:
	
	Frustum( const Plane& left, const Plane& right, const Plane& top, const Plane& bottom );
	float32 dist( const vec3& p ) const;
	
private:
	Plane mPlanes[ 4 ];
};

// convert viewing angles into openGL perspective aspect
float32 getPerspectiveAspect( float32 xDegs, float32 yDegs );

// clip a view / portal for an object
// i.e. tunnel interior view
// view vec is - (-x, +x), (-y, + y) view angles
vec4 clipView( const Orientation& camera, const vec4& viewAngles, const Orientation& object,
	float64 radius );


#endif // _VEC3_H
