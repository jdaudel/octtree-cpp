//
//  vec3.m
//  Game1
//
//  Created by Daudel, Jeffrey L on 4/26/14.
//  Copyright (c) 2014 Daudel, Jeffrey L. All rights reserved.
//

#include "vec3.h"

#include "Platform.h"
//#include "Orientation.h"
//#include "Matrix.h"


vec3 addVec( const vec3& left, const vec3& right )
{
    vec3 r;
    r.mX = left.mX + right.mX;
    r.mY = left.mY + right.mY;
    r.mZ = left.mZ + right.mZ;
    return( r );
}

vec3 subVec( const vec3& left, const vec3& right )
{
    vec3 r;
    r.mX = left.mX - right.mX;
    r.mY = left.mY - right.mY;
    r.mZ = left.mZ - right.mZ;
    return( r );
}

vec3 multiplyVec( float32 left, const vec3& right )
{
    vec3 r;
    r.mX = left * right.mX;
    r.mY = left * right.mY;
    r.mZ = left * right.mZ;
    return( r );
    
}

float64 lengthVec( const vec3& v )
{
    float64 r = sqrt( ((float64)(v.mX * v.mX)) + (v.mY * v.mY) + (v.mZ * v.mZ) );
    return( r );
}

vec3 normalizeVec( const vec3& v )
{
    float64 l = lengthVec( v );
	errorCheck( l >= .000001 );
	//errorCheck( l != 0 );
    
    return( multiplyVec( 1.0 / l, v ));
    
}

bool isNormalized( const vec3& v )
{
    return( fabs( lengthVec( v ) - 1.0f ) < .001 );
}

bool isParallel( const vec3& left, const vec3& right )
{
    // its parallel if the cross is zero length
    vec3 cross1 = cross( left, right );
    return( isEqualVec( kOrigin3, cross1 ) );
}

bool isEqualVec( const vec3& left, const vec3& right )
{
    return( left.mX == right.mX
           && left.mY == right.mY
           && left.mZ == right.mZ );
}

bool isEqualVec( const vec3& left, const vec3& right, float64 tolerance )
{
    return( fabs( left.mX - right.mX ) <= tolerance
                && fabs( left.mY - right.mY ) <= tolerance
                && fabs( left.mZ - right.mZ ) <= tolerance );
}

float64 dot( const vec3& left, const vec3& right )
{
    float64 result = ((float64)(left.mX * right.mX)) + (left.mY *right.mY)
        + (left.mZ * right.mZ);
    
    return( result );
}

vec3 div( const vec3& left, const vec3& right )
{
	vec3 result( left.mX / right.mX, left.mY / right.mY, left.mZ / right.mZ );
	return( result );
}

vec3 cross( const vec3& left, const vec3& right )
{
    vec3 r;
    
    // r.x = bycz - bzcy
    r.mX = left.mY * right.mZ - left.mZ * right.mY;
    r.mY = left.mZ * right.mX - left.mX * right.mZ;
    r.mZ = left.mX * right.mY - left.mY * right.mX;
    return( r );
}

vec3 project( const vec3& v, const vec3& onto )
{
    vec3 unitOnto = normalizeVec( onto );
    float32 scalar = dot( v, unitOnto );
    vec3 result = multiplyVec( scalar, unitOnto );
    return( result );
    
}

float32 projectScalar( const vec3& v, const vec3& onto )
{
    vec3 unitOnto = normalizeVec( onto );
    float32 scalar = dot( v, unitOnto );
    return( scalar );
}

float64 degreesToRadians( float64 degrees )
{
    return( degrees / 360 * (2 * kPi) );
}

float64 radiansToDegrees( float64 radians )
{
    return( radians / (2*kPi) * 360 );
}

bool isEndDirectional( float64 pos, float64 dir, float64 end )
{
    if (dir > 0)
    {
        return( pos >= end);
    }
    else if (dir < 0)
    {
        return( pos <= end);
    }
    else
    {
        errorMsg( "Invalid dir" );
    }
    
    return( false );
}

//vec3 rotateVec( const vec3& v, const vec3& axis, float32 degrees )
//{
//	Matrix4 m = RotateMatrix( IdentityMatrix(), degreesToRadians( degrees ),
//		axis.mX, axis.mY, axis.mZ );
//	vec3 result = MultiplyMatrix( m, v );
//	return( result );
//}

// t [0, 1]
vec3 interpolate( const vec3& v1, const vec3& v2, float32 t )
{
	vec3 v = v2 - v1;
	vec3 p = v1 + (t * v);
	return( p );
}

vec4 interpolate( const vec4& v1, const vec4& v2, float32 t )
{
	vec4 v = v2 - v1;
	vec4 p = v1 + (t * v);
	return( p );
}

vec3 interpolateRotation( const vec3& v1, const vec3& v2, float32 t )
{
	vec3 cross1 = cross( v1, v2 );
	
	if (lengthVec( cross1 ) < .01)
	{
		cross1 = cross( v1, kZUnit );
		
		if (lengthVec( cross1 ) < .01)
		{
			cross1 = cross( v1, kXUnit );
		}
	}
	
	errorCheck( lengthVec( cross1 ) >= .00001 );

	vec3 cross2 = cross( cross1, v1 );
	errorCheck( lengthVec( cross2 ) >= .00001 );
	
	cross2 = normalizeVec( cross2 );
	
	float64 dp = dot( v1, v2 );
	// if its just a precision error, dp should not be much above 1.0 maximum
	errorCheck( dp < 1.001 );
	if (dp > 1.0)
	{
		dp = 1.0;
	}
	
    errorCheck( dp > -1.001 );
    if (dp < -1)
    {
        dp = -1;
    }
    
	float64 angle = acos( dp );
	float64 angle2 = angle * t;
	
	vec3 result = v1 * cos( angle2 ) + cross2 * sin( angle2 );

	errorCheck( isnan( result.mX ) == false );
	
	return( result );
}

vec3 randNormalVec()
{
    vec3 v( randFloat( -1, 1 ), randFloat( -1, 1 ), randFloat( -1, 1 ));
    v = normalizeVec( v );
    return( v );
}

float64 getAngle( const vec3& v1, const vec3& v2 )
{
	float64 dp = dot( v1, v2 ) / (lengthVec( v1 ) * lengthVec( v2 ));
	// take care of floating precision
	dp = cap( dp, -1, 1 );
	float64 result = acos( dp );
	return( radiansToDegrees( result ) );
}

float64 getCollision( const vec3& p1, const vec3& v1, const vec3& p2, const vec3& v2, float64 dist )
{
	// make p2, v2 relative to p1
	// cancel out v2 to make just one vector
	vec3 v3 = v2 + (-1 * v1);
	vec3 p3 = p2 - p1;
	
	vec3_64 p3_64( p3 );
	vec3_64 v3_64( v3 );
	
	// solve quadratic
	float64 a = (v3_64.mX * v3_64.mX) + (v3_64.mY * v3_64.mY) + (v3_64.mZ * v3_64.mZ);
	float64 b = (2.0 * p3_64.mX * v3_64.mX) + (2 * p3_64.mY * v3.mY) + (2 * p3_64.mZ * v3_64.mZ);
	float64 c = (p3_64.mX * p3_64.mX) + (p3_64.mY * p3_64.mY) + (p3_64.mZ * p3_64.mZ)
		- (dist * dist);
	
	int32 numSols;
	float64 sol1;
	float64 sol2;
	quadratic( a, b, c, numSols, sol1, sol2 );
	float32 collisionT = -1;
	
    // use precision?
	//if (lengthVec( v3 ) < .001)
    if (v3 == kOrigin3)
	{
		// these objects are static with their relative velocity
		// will only collide if initially colliding
		errorCheck( numSols == 0 );
		vec3 distVec = subVec( p1, p2 );
		float64 dist2 = lengthVec( distVec );
		if (dist2 <= dist)
		{
			collisionT = 0;
		}
        else
        {
            // keep as not colliding (-1)
        }
		 
	}
	
	if (numSols >= 1)
	{
		// verify
		// p3 is relative to origin
		vec3 p4 = p3 + (sol1 * v3);
		float64 f2 = lengthVec( p4 );
		
		// todo: use percentages instead to verify?
		// make it relative to the point differences or vectors?
		//errorCheck( fuzzyCompare( f2, dist, 1.0 ) );
		errorCheck( fuzzyCompare( f2, dist, .1 ) );
		//errorCheck( fuzzyCompare( f2, d, .01 ) );
		//errorCheck( fuzzyCompare( f2, d, .001 ) );
		//errorCheck( fuzzyCompare( f2, d, .002 ) );
		
		if (numSols == 1)
		{
			if (sol1 >= 0
				&& sol1 <= 1)
			{
				collisionT = sol1;
			}
		}
		else
		{
			errorCheck( sol1 <= sol2 );
			if (sol2 < 0
				|| sol1 > 1)
			{
				// doesn't collide
			}
			else
			{
				if (sol1 < 0)
				{
					// object is already colliding at initial time 0
					collisionT = 0;
				}
				else
				{
					collisionT = sol1;
				}
			}
		}
	}
	
    // test - debug
    //vec3 p = v1 * collisionT;
    //errorCheck( p.mX >= -100000 && p.mX <= 100000 );
    //errorCheck( p.mY >= -100000 && p.mY <= 100000 );
    //errorCheck( p.mZ >= -100000 && p.mZ <= 100000 );
    
	return( collisionT );
}

float64 getCollision( const vec3& p1, const vec3& v1, const vec3& p2, float64 dist )
{
	return( getCollision( p1, v1, p2, kOrigin3, dist ));
}

//float64 getPathCollision( const vec3& rayP0, const vec3& rayP1, const float64 rayRadius,
//						 const vec3& path0, const vec3& path1, const vec3& zDirIn, float64 r0, float64 r1,
//						 Plane* reflectPlane, float64* closestPathT, float64* closestRayT )
//{
//	float64 result = -1;
//	
//	vec3 pathVec = path1 - path0;
//	
//	if (closestPathT != nullptr)
//	{
//		*closestPathT = -1;
//		*closestRayT = -1;
//	}
//	
//	// todo: handle ray as a point
//	if (lengthVec( rayP1 - rayP0 ) < .001 )
//	{
//		return( -1 );
//	}
//	
//	errorCheck( lengthVec( pathVec ) >= .001 );
//	
//	vec3 zDir = zDirIn;
//	//vec3 zDir = path1 - path0;
//	
//	Plane startPlane( path0, zDir );
//	Plane endPlane( path1, zDir );
//	
//	vec3 zNormal = normalizeVec( zDir );
//	
//	vec3 zVec = project( pathVec, zNormal );
//	vec3 pathShift = pathVec - zVec;
//	
//	//errorCheck( isEqualVec( pathShift, kOrigin3, .001 ) );
//	
//	vec3 rayP1Shift = rayP1;
//	vec3 rayVec = rayP1Shift - rayP0;
//	
////	// debug
////	vec3 vv1 = rayP1Shift - path0;
////	vec3 vv2 = project( vv1, (path1 - path0) );
////	vec3 vv3 = vv1 - vv2;
////	float64 d1 = lengthVec( vv3 );
////
////	vec3 vv4 = rayP0 - path0;
////	vec3 vv5 = project( vv4, (path1 - path0 ));
////	vec3 vv6 = vv4 - vv5;
////	float64 d2 = lengthVec( vv6 );
////
////	float64 d3 = r0 - rayRadius;
////	if (d1 >= d3)
////	{
////		NSLog( @"debug" );
////	}
//	
//	float64 t0 = startPlane.rayIntersects( rayP0, rayVec);
//	float64 t1 = endPlane.rayIntersects( rayP0, rayVec );
//	
//	// handle ray perpindicular to path dir case
//	// entire ray is either inside path z bounds or outside path z bounds
//	if (fabs( dot( rayVec, zDir ) ) < .001)
//	{
//		if (startPlane.containsInHalfSpace( rayP0 ) == false)
//		{
//			// before z start plane
//			return( -1 );
//		}
//		if (endPlane.containsInHalfSpace( rayP0 ) == true
//			&& endPlane.containsOnPlane( rayP0 ) == false)
//		{
//			// after end z plane
//			return( -1 );
//		}
//		
//		t0 = 0;
//		t1 = 1;
//		// no shifting distance
//		pathShift = kOrigin3;
//	}
//	
//	vec3 p0 = rayP0 + (t0 * rayVec);
//	vec3 p1 = rayP0 + (t1 * rayVec);
//	p1 = p1 - pathShift;
//	
//	// zvec can be close to zero
//	Orientation o1( path0, zNormal );
//	//Orientation o1( path0, zVec );
//	
//	vec3 local0 = o1.localize( p0 );
//	vec3 local1 = o1.localize( p1 );
//
//	
//	// rp = radius of path
//	// r0 = start of path radius
//	// r1 = end of path radius
//	// r2 = radius of ray
//	
//	// initial equation:
//	// sqrt( x^2 + y^2 ) - r2 = rp
//	// rp = r0 + rdiff * t
//	// x = x0 + xdiff * t
//	// y = y0 + ydiff * t
//	
//	//t^2 * (b^2 + c^2 - a^2) + t * (2*x0*b + 2*y0*c - 2*a*t - 2*r0*a-2*r2*a) + x0^2 + y0^2
//	// -r2^2 - r0^2 - a^2-2*r2*r0 = 0;
//	float64 x0 = local0.mX;
//	float64 y0 = local0.mY;
//	float64 xDiff = local1.mX - local0.mX;
//	float64 yDiff = local1.mY - local0.mY;
//	float64 rDiff = r1 - r0;
//	
//	float64 qa = (xDiff * xDiff) + (yDiff * yDiff) - (rDiff * rDiff);
//	float64 qb = (2 * x0 * xDiff) + (2 * y0 * yDiff)
//		- (2 * r0 * rDiff) + (2 * rayRadius * rDiff);
//	float64 qc =(x0 * x0) + (y0 * y0) - (rayRadius * rayRadius) - (r0 * r0) + (2 * rayRadius * r0);
//	int32 numSolutions;
//	float64 sol1, sol2;
//	
//	quadratic( qa, qb, qc, numSolutions, sol1, sol2 );
//	
//	float64 sols[ 2 ];
//	sols[ 0 ] = sol1;
//	sols[ 1 ] = sol2;
//	
//	// convert each solution relative to the ray equation and then use the closest positive solution
//	// for the result
//	for( int32 i=0; i<numSolutions; ++i )
//	{
//		float64 t = sols[ i ];
//		
//		// convert t relative to original ray
//		vec3 rayV = p1 - p0;
//		vec3 intersectPoint = p0 + (t * rayV);
//		
//		// shift back to original ray
//		vec3 intersectPoint2 = intersectPoint + (pathShift * t);
//		
//		vec3 v1 = intersectPoint2 - rayP0;
//		int32 sign;
//		if (dot( v1, rayVec ) < 0)
//		{
//			sign = -1;
//		}
//		else
//		{
//			sign = 1;
//		}
//		
//		float64 l = lengthVec( v1 );
//		float64 l2 = lengthVec( rayVec );
//		float64 rayT = l / l2 * sign;
//		
//		// ray must be within path segment where t = [0, 1]
//		// and within ray where rayT = [0, 1 ]
//		if (rayT >= 0
//			&& rayT <= 1)
////		if (rayT >= 0
////			&& rayT <= 1
////			&& t >= 0
////			&& t <= 1)
//		{
//			// valid
//			
//			// generate a reflection plane
//			// don't do this if intersection point is far off - can have precision errors?
//			Plane plane;
//			
//			// calc reflection plane
//			vec3 pathPoint = path0 + (t * pathVec);
//			vec3 pathRadius = intersectPoint2 - pathPoint;
//			
//			// handle case of a collision on in the center of tunnel (too small of radius)
//			if (isEqualVec( pathRadius, kOrigin3, .001 ))
//			{
//				// use path dir instead
//				plane.set( intersectPoint, -1 * pathVec );
//			}
//			else
//			{
//				pathRadius = normalizeVec( pathRadius );
//				vec3 pathTangent = cross( pathRadius, pathVec );
//				
//				vec3 planePoint0 = intersectPoint2;
//				vec3 planePoint1 = intersectPoint2 + pathVec + (rDiff * pathRadius);
//				vec3 planePoint2 = intersectPoint2 + pathTangent;
//				plane.set( planePoint0, planePoint2, planePoint1 );
//				// have normal point toward center
//				errorCheck( dot( pathRadius, plane.getNormal() ) < 0 );
//			}
//			
//			if (closestPathT != nullptr)
//			{
//				if (*closestPathT == -1
//					|| fabs( *closestPathT - .5 ) < fabs( t - .5 ))
//				{
//					*closestPathT = t;
//					*closestRayT = rayT;
//					if (reflectPlane != nullptr)
//					{
//						*reflectPlane = plane;
//					}
//				}
//			}
//			
//			if (t >= 0
//				&& t <= 1)
//			{
//				if (result == -1
//					|| rayT < result)
//				{
//					result = rayT;
//					
//					if (reflectPlane != nullptr)
//					{
//						*reflectPlane = plane;
//					}
//				}
//			}
//		}
//	}
//	
//	return( result );
//}

///////////////////
// class Plane
//////////////////

Plane::Plane( const vec3& p1, const vec3& p2, const vec3& p3 )
{
	set( p1, p2, p3 );
}

Plane::Plane()
{
	mPos = kOrigin3;
	mNormal = kOrigin3;
}

Plane::Plane( const vec3& p1, const vec3& normal )
{
	set( p1, normal );
}

void Plane::set( const vec3& p1, const vec3& normal )
{
	mPos = p1;
	mNormal = normal;
	
	errorCheck( isEqualVec( mNormal, kOrigin3, .001 ) == false );
	mNormal = normalizeVec( mNormal );
}

void Plane::set( const vec3& p1, const vec3& p2, const vec3& p3 )
{
	vec3 v1 = p2 - p1;
	vec3 v2 = p3 - p1;
	vec3 normal = cross( v1, v2 );
	
	set( p1, normal );
}

vec3 Plane::getPos() const
{
	return( mPos );
}

vec3 Plane::getNormal() const
{
	return( mNormal );
}

bool Plane::containsOnPlane( const vec3& p ) const
{
	vec3 v = p - mPos;
	float32 dp = dot( v, mNormal );
	return( fabs( dp ) < .001 );
}

// point is on or in the half space pointed to by the normal
bool Plane::containsInHalfSpace( const vec3& p ) const
{
	vec3 v = p - mPos;
	float32 dp = dot( v, mNormal );
	return( dp >= -.001 );
}

vec3 Plane::project( const vec3& p ) const
{
	vec3 v = p - mPos;
	vec3 proj = ::project( v, mNormal );
	vec3 result = p - proj;
	return( result );
}

float32 Plane::dist( const vec3& p ) const
{
	vec3 v = p - mPos;
	float32 result = projectScalar( v, mNormal );
	return( result );
}

float64 Plane::rayIntersects( const vec3& line0, const vec3& dir ) const
{
	// equation: d = dp ( (plane0 - line0), planeNorml ) / dp (line, normal)
	float64 result;
	vec3 v = mPos - line0;
	float64 numerator = dot( v, mNormal );
	float64 denominator = dot( dir, mNormal );
	
	if (fabs( denominator ) < .001)
	{
		if (fabs( numerator ) < .001)
		{
			// ray is in plane
			result = 0;
		}
		else
		{
			// ray never intersects plane
			result = -1;
		}
	}
	else
	{
		result = numerator / denominator;
	}
	
	return( result );
}

///////////////////////
// Frustum
///////////////////////

Frustum::Frustum( const Plane& left, const Plane& right, const Plane& top, const Plane& bottom )
{
	mPlanes[ 0 ] = left;
	mPlanes[ 1 ] = right;
	mPlanes[ 2 ] = top;
	mPlanes[ 3 ] = bottom;
}

float32 Frustum::dist( const vec3& p ) const
{
	float32 dist = std::numeric_limits<float>::max();
	
	for( auto& plane : mPlanes )
	{
		float32 d = plane.dist( p );
		if (d < dist)
		{
			dist = d;
		}
	}
	
	return( dist );
}

float32 getPerspectiveAspect( float32 xDegs, float32 yDegs )
{    
    float32 aspect = tan( degreesToRadians( xDegs / 2 ) ) / tan( degreesToRadians( yDegs / 2 ));
    return( aspect);
}

//vec4 clipView( const Orientation& camera, const vec4& viewAngles, const Orientation& object,
//			  float64 radius )
//{
//	vec3 points[ 4 ];
//	points[ 0 ] = object.Pos() + (object.Right() * -radius);
//	points[ 1 ] = object.Pos() + (object.Right() * radius);
//	points[ 2 ] = object.Pos() + (object.Top() * -radius);
//	points[ 3 ] = object.Pos() + (object.Top() * radius);
//	
//	// x, y, z, bounds
//	// [0] - min, [1] - max
//	vec2 bounds[ 3 ];
//	
//	for( int32 i=0; i<4; ++i )
//	{
//		vec3 v = points[ i ] - camera.Pos();
//		// dist - x, y, z
//		float64 dist[ 3 ];
//		dist[ 0 ] = projectScalar( v, camera.Right() );
//		dist[ 1 ] = projectScalar( v, camera.Top() );
//		dist[ 2 ] = projectScalar( v, camera.Front() );
//
//		// go through each bounds (x, y, z) and find min[ 0 ] and max[ 1 ]
//		for( int32 j=0; j<3; ++j )
//		{
//			vec2& b = bounds[ j ];
//			float64 d;
//			if (j == 2)
//			{
//				// z dist - no divide
//				d = dist[ j ];
//			}
//			else
//			{
//				// ratio respect to z distance from camera
//				d = radiansToDegrees( atan( dist[ j ] / dist[ 2 ] ) );
//			}
//			
//			// check for less min
//			if (i == 0
//				|| d < b[ 0 ])
//			{
//				b[ 0 ] = d;
//			}
//			
//			// check for bigger max
//			if (i == 0
//				|| d > b[ 1 ])
//			{
//				b[ 1 ] = d;
//			}
//		}
//	}
//	
//	vec2& xBounds = bounds[ 0 ];
//	vec2& yBounds = bounds[ 1 ];
//	vec2& zBounds = bounds[ 2 ];
//	
//	vec4 result = viewAngles;
//	
//	vec2& viewX = reinterpret_cast< vec2& >( result[ 0 ] );
//	vec2& viewY = reinterpret_cast< vec2& >( result[ 2 ] );
//	
//	// check if max z bounds is behind
//	if (zBounds[ 1 ] <= 0)
//	{
//		// all points are behind camera, - not visible
//		result = kOrigin4;
//	}
//	else if (zBounds[ 0 ] <= 0)
//	{
//		// some point are behind, and some are in front
//		// don't do any additional clipping
//		//result = viewAngles;
//	}
//	else
//	{		
//		if (xBounds[ 0 ] > viewX[ 0 ])
//		{
//			// move x to the right
//			viewX[ 0 ] = xBounds[ 0 ];
//		}
//		if (xBounds[ 1 ] < viewX[ 1 ])
//		{
//			viewX[ 1 ] = xBounds[ 1 ];
//		}
//		if (yBounds[ 0 ] > viewY[ 0 ])
//		{
//			viewY[ 0 ] = yBounds[ 0 ];
//		}
//		if (yBounds[ 1 ] < viewY[ 1 ])
//		{
//			viewY[ 1 ] = yBounds[ 1 ];
//		}
//	}
//	
//	return( result );
//}
