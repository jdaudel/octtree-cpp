//
//  Game1
//
//  Copyright (c) 2014 Daudel, Jeffrey L. All rights reserved.
//


#include "box3.h"
#include "Platform.h"

Box3::Box3()
{
	// init to negative volume
	mMin.set( 0, 0, 0 );
	mMax.set( -1, 0, 0 );
}

Box3::Box3( const vec3& p1, const vec3& p2 ) : Box3()
{
	add( p1 );
	add( p2 );
}

Box3::Box3( const vec3& center, float64 radius ) : Box3()
{
    vec3 p( radius, radius, radius );
    add( center - p );
    add( center + p );
}

bool Box3::valid() const
{
	return( mMin.mX <= mMax.mX
		   && mMin.mY <= mMax.mY
		   && mMin.mZ <= mMax.mZ );
}

void Box3::add( const vec3& p )
{
	if (valid() == false)
	{
		mMin = mMax = p;
	}
	else
	{
		// update min
		if (p.mX < mMin.mX)
		{
			mMin.mX = p.mX;
		}
		if (p.mY < mMin.mY)
		{
			mMin.mY = p.mY;
		}
		if (p.mZ < mMin.mZ)
		{
			mMin.mZ = p.mZ;
		}
		
		// update max
		if (p.mX > mMax.mX)
		{
			mMax.mX = p.mX;
		}
		if (p.mY > mMax.mY)
		{
			mMax.mY = p.mY;
		}
		if (p.mZ > mMax.mZ)
		{
			mMax.mZ = p.mZ;
		}
	}
	
	errorCheck( valid() );
	
}

void Box3::add( const Box3 box )
{
	add( box.mMin );
	add( box.mMax );
}

void Box3::addSphere( const vec3& p, float32 radius )
{
	add( vec3( p.mX - radius, p.mY - radius, p.mZ - radius ));
	add( vec3( p.mX + radius, p.mY + radius, p.mZ + radius ));
}

vec3 Box3::getCenter() const
{
	vec3 p;
	
	for( int i=0; i<3; ++i )
	{
		p[ i ] = (mMin[ i ] + mMax[ i ]) / 2;
	}
	
	return( p );
}

vec3 Box3::getMin() const
{
	return( mMin );
}

vec3 Box3::getMax() const
{
	return( mMax );
}

vec3 Box3::getSize() const
{
	vec3 p;
	for( int i=0; i<3; ++i )
	{
		p[ i ] = mMax[ i ] - mMin[ i ];
	}
	
	return( p );
}

float32 Box3::getMaxSize() const
{
	float32 size = 0;
	for( int i=0; i<3; ++i )
	{
		float32 size2 = mMax[ i ] - mMin[ i ];
		if (i == 0
			|| size2 > size)
		{
			size = size2;
		}
	}
	
	return( size );
}

bool Box3::intersects( const Box3& box ) const
{
	bool result = true;
	if (mMin.mX > box.mMax.mX
		|| mMin.mY > box.mMax.mY
		|| mMin.mZ > box.mMax.mZ
		|| mMax.mX < box.mMin.mX
		|| mMax.mY < box.mMin.mY
		|| mMax.mZ < box.mMin.mZ)
	{
		result = false;
	}
	
	return( result );
}

//bool Box3::intersects( const vec3& p1, const vec3& p2, float64 radius ) const
//{
//    // use bounding sphere
//    vec3 v = p2 - p1;
//    vec3 size = getSize();
//
//    float64 boxRadius = sqrt( (size.mX * size.mX) + (size.mY * size.mY) + (size.mZ * size.mZ )) / 2;
//
//    float64 t = getCollision( p1, v, getCenter(), boxRadius + radius );
//    return( t >= 0 );
//}

bool Box3::intersects( const vec3& p1, const vec3& p2, float64 radius ) const
{
    vec3 v = p2 - p1;
    bool result = false;

    // all 6 planes
    for( int32 i=0; i<3; ++i )
    {
        int32 c0 = i%3;
        int32 c1 = (i+1)%3;
        int32 c2 = (i+2)%3;

        if (v[ c0 ] == 0)
        {
            // can't check
        }
        else
        {
            float64 t1 = (mMin[ c0 ] - radius - p1[ c0 ]) / v[ c0 ];
            float64 t2 = (mMax[ c0 ] + radius - p1[ c0 ]) / v[ c0 ];
            if (t1 < 0
                && t2 <0)
            {
                return( false );
            }

            if (t1 > 1
                && t2 > 1)
            {
                return( false );
            }

            vec3 q1 = p1 + (v * t1);
            vec3 q2 = p1 + (v * t2);

            if (q1[ c1 ] >= mMin[ c1 ] - radius
                && q1[ c1 ] <= mMax[ c1 ] + radius
                && q1[ c2 ] >= mMin[ c2 ] - radius
                && q1[ c2 ] <= mMax[ c2 ] + radius)
            {
                result = true;
            }

            if (q2[ c1 ] >= mMin[ c1 ] - radius
                && q2[ c1 ] <= mMax[ c1 ] + radius
                && q2[ c2 ] >= mMin[ c2 ] - radius
                && q2[ c2 ] <= mMax[ c2 ] + radius)
            {
                result = true;
            }

        }

        if (result == true)
        {
            return( true );
        }
    }

    return( result );
}

bool Box3::contains( const Box3& box ) const
{
	bool result = false;
	if (mMin.mX <= box.mMin.mX
		&& mMin.mY <= box.mMin.mY
		&& mMin.mZ <= box.mMin.mZ
		&& mMax.mX >= box.mMax.mX
		&& mMax.mY >= box.mMax.mY
		&& mMax.mZ >= box.mMax.mZ)
	{
		result = true;
	}
	
	return( result );
}

bool Box3::operator==( const Box3& right ) const
{
    if (valid() == false
        || right.valid() == false)
    {
        return( false );
    }
    
    // todo: take into account floating point precision?
    return( mMin == right.mMin
           && mMax == right.mMax );
}




