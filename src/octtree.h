//
//  vec3.h
//  Game1
//
//  Created by Daudel, Jeffrey L on 4/26/14.
//  Copyright (c) 2014 Daudel, Jeffrey L. All rights reserved.
//

#ifndef _OCTTREE_H
#define _OCTTREE_H

#include "Platform.h"

#include "vec3.h"
#include "box3.h"

#include <vector>
#include <map>
#include <set>
#include <algorithm>

template< typename T > class VoxelItem;
template< typename T > class Voxel;

// split a box into 8 equal parts
void split8( const Box3& box, std::vector< Box3 >& out );

template< typename T >
class VoxelItem
{
public:
	
	VoxelItem( T item, const vec3& p, float64 radius )
	{
		mItem = item;
		mPos = p;
		mRadius = radius;
	}
	
	T mItem;
	vec3 mPos;
	float64 mRadius;
	std::vector< Voxel< T >* > mVoxels;
};

template< typename T >
class Voxel
{
public:
	
	Voxel( Voxel* parent, const Box3& bounds )
	{
		mParent = parent;
		mBounds = bounds;
        mNumItems = 0;
	}
	
	~Voxel()
	{
		for( Voxel<T>* child : mChildren )
		{
			delete child;
		}
	}
	
	// not divisible add
    // for leafs only
	void add( VoxelItem<T>* item )
	{
        errorCheck( mChildren.size() == 0 );
		auto insertResult = mItems.insert( item );
		errorCheck( insertResult.second == true );
		item->mVoxels.push_back( this );
	}
	
	void add( VoxelItem<T>* item, const Box3& bounds, float64 minVoxelSize )
	{
        if (mBounds.intersects( bounds ) == false)
        {
            // not added to this voxel
        }
        else
        {
            mNumItems++;
            if (mChildren.size() == 0)
            {
                // root case - no children
                add( item );
                divide( minVoxelSize );
            }
            else
            {
                // add to children
                for( Voxel<T>* child : mChildren )
                {
					child->add( item, bounds, minVoxelSize );
				}
			}
		}
	}
	
    // for leafs only
	void remove( VoxelItem<T>* item )
	{
        //errorCheck( mChildren.size() == 0 );
		auto iter = std::find( item->mVoxels.begin(), item->mVoxels.end(), this );
		errorCheck( iter != item->mVoxels.end() );
		item->mVoxels.erase( iter );
		size_t num = this->mItems.erase( item );
		errorCheck( num == 1 );
        
        // voxels items must match
        //errorCheck( mNumItems == mItems.size() );
	}
	
    // todo: trivial collapse: only collapse voxels when ir has only 1 or 0 items.
	void remove( VoxelItem<T>* item, const Box3& bounds, float64 minVoxelSize, bool combineVoxels )
	{
		if (mBounds.intersects( bounds ) == false)
		{
			// item can't be in this voxel
			return;
		}
		
        errorCheck( mNumItems > 0 );
        --mNumItems;
        
		for( Voxel<T>* child : mChildren )
		{
			child->remove( item, bounds, minVoxelSize, combineVoxels );
		}
		
		if (mChildren.size() == 0)
		{
			// item must be attached to this leaf voxel
			auto iter = mItems.find( item );
			errorCheck( iter != mItems.end() );
			mItems.erase( iter );
			
			auto iter2 = std::find( item->mVoxels.begin(),
				item->mVoxels.end(), this );
			errorCheck( iter2 != item->mVoxels.end() );
			item->mVoxels.erase( iter2 );
		}
		 
        // do trival combines
        if (mNumItems == 0
            && mChildren.size() > 0)
        {
            for( Voxel<T>* child : mChildren )
            {
                // must be only 1 level deep
                errorCheck( child->mChildren.size() == 0 );
                delete child;
            }
            
            mChildren.clear();
        }
        
        if (mNumItems == 1
            && mChildren.size() > 0)
        {
            VoxelItem<T>* item2 = nullptr;
            for (Voxel<T>* child : mChildren )
            {
                errorCheck( child->mChildren.size() == 0 );
                if (child->mItems.size() > 0)
                {
                    VoxelItem<T>* item3 = *(child->mItems.begin());
                    if (item2 == nullptr)
                    {
                        item2 = item3;
                    }
                    else
                    {
                        errorCheck( item2 == item3 );
                    }

                    child->remove( item3 );
                    delete child;
                }
            }

            errorCheck( item2 != nullptr );
            mChildren.clear();
            this->add( item2 );
        }
        
        return;
        
//        // look to see if children need to be combined
//        // temp skip the recombine
//        if (combineVoxels == false)
//        {
//            return;
//        }
//
//        if (mChildren.size() > 0)
//        {
//            // see if all children are eligible for combine
//            // must be leafs
//            bool combine = true;
//            std::set< VoxelItem<T>* > items;
//            float64 maxDist;
//
//            // first check if all children are leafs
//            for( Voxel<T>* child : mChildren)
//            {
//                if (child->mChildren.size() > 0)
//                {
//                    combine = false;
//                    break;
//                }
//            }
//
//            // next check if dist is great enoug between furtherest objects
//            if (combine)
//            {
//                for( Voxel<T>* child : mChildren)
//                {
//                    if (child->isReducible( child->mItems, child->getVoxelSize(), minVoxelSize, maxDist ) == false)
//                    {
//                        // don't change
//                        for( VoxelItem<T>* item : child->mItems )
//                        {
//                            items.insert( item );
//                        }
//                    }
//                    else
//                    {
//                        combine = false;
//                        break;
//                    }
//                }
//            }
//
//            if (combine
//                && isReducible( items, getVoxelSize(), minVoxelSize, maxDist ) == false)
//            {
//                // combine all childen
//                for( Voxel<T>* child : mChildren )
//                {
//                    // must be leaf
//                    errorCheck( child->mChildren.size() == 0 );
//
//                    for( ; child->mItems.size() > 0; )
//                    {
//                        VoxelItem<T>* item = *(child->mItems.begin());
//                        child->remove( item );
//                    }
//
//                    delete child;
//                }
//
//                mChildren.clear();
//
//                errorCheck( mItems.size() == 0 );
//                for( VoxelItem<T>* item : items )
//                {
//                    add( item );
//                }
//            }
//        }
	}

	// check for combining a space of voxels
	void combine( const Box3& bounds, float64 minVoxelSize )
	{
		if (mBounds.intersects( bounds ) == false)
		{
			// item can't be in this voxel
			return;
		}
		
		for( Voxel<T>* child : mChildren )
		{
			child->combine( bounds, minVoxelSize );
		}
		
		if (mChildren.size() > 0)
		{
			// see if all children are eligible for combine
			// must be leafs
			bool combine = true;
			std::set< VoxelItem<T>* > items;
			float64 maxDist;
			
			// first check if all children are leafs
			for( Voxel<T>* child : mChildren)
			{
				if (child->mChildren.size() > 0)
				{
					combine = false;
					break;
				}
			}
			
			// next check if dist is great enoug between furtherest objects
			if (combine)
			{
				for( Voxel<T>* child : mChildren)
				{
					if (child->isReducible( child->mItems, child->getVoxelSize(), minVoxelSize, maxDist ) == false)
					{
						for( VoxelItem<T>* item : child->mItems )
						{
							items.insert( item );
						}
					}
					else
					{
						// don't change
						combine = false;
						break;
					}
				}
			}
			
			if (combine
				&& isReducible( items, getVoxelSize(), minVoxelSize, maxDist ) == false)
			{
				// combine all childen
				for( Voxel<T>* child : mChildren )
				{
					// must be leaf
					errorCheck( child->mChildren.size() == 0 );
					
					for( ; child->mItems.size() > 0; )
					{
						VoxelItem<T>* item = *(child->mItems.begin());
						child->remove( item );
					}
					
					delete child;
				}
				
				mChildren.clear();
				
				errorCheck( mItems.size() == 0 );
				for( VoxelItem<T>* item : items )
				{
					add( item );
				}
			}
		}
	}
	
	float64 getVoxelSize() const
	{
		return( mBounds.getSize().mX );
	}
	
	static bool isReducible( const std::set< VoxelItem<T>* >& items, float64 voxelSize, float64 minVoxelSize,
			float64& maxAlignedDistOut )
	{
		bool result;
		maxAlignedDistOut = 0;
		
		// assume this is a leaf node
		// have to iterate over all children check if it can combine children
		//errorCheck( mChildren.size() == 0 );

		if (items.size() <= 1)
		{
			// can't get below 1 in all blocks
			result = false;
		}
		else if (voxelSize <= minVoxelSize)
		{
			// can't go any smaller
			result = false;
		}
		else
		{
			// if smallest item is at least 2 min radii away from closest object,
			// then it can be split
			VoxelItem<T>* min = nullptr;
			for( VoxelItem<T>* item : items )
			{
				if (min == nullptr)
				{
					min = item;
				}
				else
				{
					if (item->mRadius < min->mRadius)
					{
						min = item;
					}
				}
			}
			
			// max aligned dist
			// since voxels will be aligned to axis
			float64 maxAlignedDist = 0;
			for( VoxelItem<T>* item : items )
			{
				if (item != min)
				{
					for( int i=0; i<3; ++i )
					{
						float64 dist = fabs( item->mPos[ 0 ] - min->mPos[ 0 ] )
							- item->mRadius - min->mRadius;
						if (dist > maxAlignedDist)
						{
							maxAlignedDist = dist;
						}
					}
				}
			}
			
			maxAlignedDistOut = maxAlignedDist;
			if (maxAlignedDist > minVoxelSize)
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}

		return( result );
	}
	
	void divide( float64 minVoxelSize )
	{
		// must not have been divided already
		errorCheck( mChildren.size() == 0 );
		
		float64 maxAlignedDist;
		bool reduce = isReducible( this->mItems, getVoxelSize(), minVoxelSize, maxAlignedDist );
		if (reduce == false)
		{
			return;
		}
		
		std::vector< Box3 > childBounds;
		split8( mBounds, childBounds );
		for( Box3& bounds : childBounds )
		{
			Voxel< T >* childVoxel = new Voxel( this, bounds );
			mChildren.push_back( childVoxel );
		}
		
		// migrate items to children
		for( VoxelItem<T>* item : mItems )
		{
			for( Voxel< T >* child : mChildren )
			{
				float64 radius = item->mRadius;
				Box3 box( item->mPos, radius );
				if (child->mBounds.intersects( box ))
				{
                    child->mNumItems++;
					child->add( item );
				}
			}
		}
		
		// check if a reduction was made
		int32 maxNumber = 0;
		
		for( Voxel< T >* child : mChildren )
		{
			int32 childNumItems = child->mItems.size();
			if (childNumItems > maxNumber)
			{
				maxNumber = childNumItems;
			}
		}
		
		float64 childVoxelSize = getVoxelSize() / 2 ;
		if (maxAlignedDist > childVoxelSize)
		{
			// must have been reduced
			errorCheck( maxNumber < static_cast< int64 >( mItems.size() ) );
		}
		
		// migrate items from parent to children
		errorCheck( mItems.size() > 0 );
		
		for( ; mItems.size() > 0; )
		{
			VoxelItem<T>* item = *(mItems.begin());
			this->remove( item );
		}
		
		// look to subdivide further
		for( Voxel<T>* child : mChildren )
		{
			child->divide( minVoxelSize );
		}
	}
	
	void getItems( const Box3& bounds, std::set< T >& out ) const
	{
		if (mBounds.intersects( bounds ))
		{
			if (mChildren.size() > 0)
			{
				// tree node
				// if children, all items should be in children
				errorCheck( mItems.size() == 0 );
				for( Voxel<T>* child : mChildren )
				{
					child->getItems( bounds, out );
				}
			}
			else
			{
				// leaf node
				
				for( VoxelItem<T>* item: mItems )
				{
					Box3 childBounds( item->mPos, item->mRadius );
					if (childBounds.intersects( bounds ))
					{
						out.insert( item->mItem );
					}
				}
			}
		}
	}
	
	void getItems( const vec3& p1, const vec3& p2, float64 radius, std::set< T >& out ) const
	{
		if (mBounds.intersects( p1, p2, radius ) == false)
		{
			// does not intersect this voxel
		}
		else if (mChildren.size() == 0 )
		{
			// leaf node
			vec3 v = p2 - p1;
			for( VoxelItem<T>* item : mItems )
			{
				if (getCollision( p1, v, item->mPos, radius + item->mRadius ) >= 0)
				{
					out.insert( item->mItem );
				}
			}
		}
		else
		{
			// tree node - recurse
			for( Voxel<T>* child : mChildren )
			{
				child->getItems( p1, p2, radius, out );
			}
		}
	}

    void getVoxels( const Box3& bounds, std::vector< Box3 >& result ) const
    {
        if (mBounds.intersects( bounds ) == false)
        {
            // none to add
        }
        else if (mChildren.size() == 0)
        {
            result.push_back( mBounds );
        }
        else
        {
            for( Voxel* child : mChildren )
            {
                child->getVoxels( bounds, result );
            }
        }
    }
	
	Box3 mBounds;
	
	std::set< VoxelItem<T>* > mItems;
	Voxel* mParent;
	std::vector< Voxel< T >* > mChildren;
    int32 mNumItems;
};

template< typename T >
class octTree
{
public:
	
	octTree( const vec3& minBounds, const vec3& maxBounds, float64 minVoxelSize, int32 splitThreshold = 2 )
	{
		mBounds.add( minBounds );
		mBounds.add( maxBounds );
		mMinVoxelSize = minVoxelSize;
        mSplitThreshold = splitThreshold;
		mRoot = nullptr;
		clear();
	}
	
	~octTree()
	{
		delete mRoot;
	}
	
	void clear()
	{
		delete mRoot;
		mRoot = new Voxel<T>( nullptr, mBounds );
		mItems.clear();
	}
	
	void add( T object, const vec3& p, float64 radius )
	{
		// must be unique
		auto itemIter = mItems.find( object );
		errorCheck( itemIter == mItems.end() );
		
        Box3 box( p, radius );
        
		// must be in bounds of root
		errorCheck( mRoot->mBounds.contains( box ) );
		
		VoxelItem<T>* item = new VoxelItem<T>( object, p, radius );
		typename std::map< T, VoxelItem< T >* >::value_type value( object, item );
		auto insertResult = mItems.insert( value );
		errorCheck( insertResult.second == true );
		
		mRoot->add( item, box, mMinVoxelSize );
		
		// must be in at least one voxel
		errorCheck( item->mVoxels.size() > 0 );
	}
	
	bool remove( T object, bool combineVoxels = true )
	{
		auto iter = mItems.find( object );
		errorCheck( iter != mItems.end() );
		
		VoxelItem<T>* item = iter->second;
		Box3 bounds( item->mPos, item->mRadius );
		mRoot->remove( item, bounds, mMinVoxelSize, combineVoxels );
		
		// verify removed from all voxels
		errorCheck( item->mVoxels.size() == 0 );
		
		mItems.erase( iter );
		delete item;
		
		return( true );
	}
	
	void combine( const Box3& bounds )
	{
		mRoot->combine( bounds, mMinVoxelSize );
	}
	
	void getItems( const vec3& p, float64 radius, std::set< T >& out ) const
	{
		Box3 bounds( p, radius );
		mRoot->getItems( bounds, out );
	}
	
	// get items from a beam (line with radius)
	void getItems( const vec3& p1, const vec3& p2, float64 radius, std::set< T >& out ) const
	{
		mRoot->getItems( p1, p2, radius, out );
	}
	
    // debug an item that should found
    void debugItem( const vec3& p1, const vec3& p2, float64 radius, T item )
    {
        auto iter = mItems.find( item );
        errorCheck( iter != mItems.end() );
        
        VoxelItem<T>* voxelItem = iter->second;
        int32 numVoxels = 0;
        for( Voxel<T>* voxel : voxelItem->mVoxels )
        {
            if (voxel->mBounds.intersects( p1, p2, radius ))
            {
                numVoxels++;
            }
        }
        
        // ray must intersect at least one voxel
        errorCheck( numVoxels > 0 );
        
        // verify ray collides with item
        float64 t = getCollision( p1, p2 - p1, voxelItem->mPos, radius + voxelItem->mRadius );
        errorCheck( t >= 0 );
    }
    
	using TBounds = std::vector< Box3 >;
	bool getVoxels( T object, TBounds& boundsOut ) const
	{
		auto iter = mItems.find( object );
		if (iter == mItems.end())
		{
			return( false );
		}
		
		VoxelItem<T>* item = iter->second;
		for( Voxel< T >* voxel : item->mVoxels )
		{
			boundsOut.push_back( voxel->mBounds );
		}
		
		return( true );
	}
	
	void getVoxels( const Box3& bounds, TBounds& out ) const
	{
        mRoot->getVoxels( bounds, out );
    }
	
	Box3 getBounds() const
	{
		return( mBounds );
	}
	
	size_t getNumItems() const
	{
		return( mItems.size() );
	}
	
private:
	
	Box3 mBounds;
    int32 mSplitThreshold;
	float64 mMinVoxelSize;
	Voxel< T > * mRoot = nullptr;
	std::map< T, VoxelItem< T >* > mItems;
	
};


#endif
