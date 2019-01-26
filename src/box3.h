//
// Box3
//

#ifndef _BOX_H
#define _BOX_H

#include "Types.h"
#include "vec3.h"

class Box3
{
public:
	
	Box3();
	Box3( const vec3& p1, const vec3& p2 );
    
    // box containing center point and radius
    Box3( const vec3& center, float64 radius );
	
	void add( const vec3& p );
	void add( const Box3 box );
	void addSphere( const vec3& p, float32 radius );
	
	vec3 getCenter() const;
	vec3 getSize() const;
	float32 getMaxSize() const;
	
	bool intersects( const Box3& box ) const;
	
	// ray intersection
	bool intersects( const vec3& p1, const vec3& p2, float64 radius ) const;
	
	bool contains( const Box3& box ) const;
	
	void getBoundingSphere( vec3& posOut, float32 radiusOut ) const;
	
	vec3 getMin() const;
	vec3 getMax() const;
    
    bool operator==( const Box3& right ) const;
	
	// todo:
	//bool getDist( const vec3& p ) const;
	
	// zero of positive area
	bool valid() const;
	
private:
	
	vec3 mMin;
	vec3 mMax;
};



#endif
