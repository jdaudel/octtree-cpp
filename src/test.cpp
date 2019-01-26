
#include "octtree.h"

void testBasicOctTree();
void testOctTreeSpan();
void testBigOctTree();

int main()
{
	testBasicOctTree();
	testOctTreeSpan();
	testBigOctTree();
}

class OctItem
{
public:

	OctItem( const vec3& pos, float32 radius )
	{
		mPos = pos;
		mRadius = radius;
	}
	
	vec3 mPos;
	float32 mRadius;
};

bool verifyVoxels( const std::vector< Box3 >& set, const std::vector< Box3 >& subSet )
{
    std::vector< Box3 > subSet2( subSet );
    
    for( const Box3& b1 : set )
    {
        for( auto iter = subSet2.begin(); ; ++iter )
        {
            if (iter == subSet2.end())
            {
                // not found
                return( false );
            }
            
            Box3& b2 = *iter;
            if (b1 == b2)
            {
                subSet2.erase( iter );
                break;
            }
        }
    }
    
    return( true );
}



void testBasicOctTree()
{
	// make space power of 2, so voxels are interger size
	// divides in half each time
	vec3 minSize( -8, -8, -8 );
	vec3 maxSize( 8, 8, 8 );
	
	octTree< OctItem* > tree( minSize, maxSize, 1 );
    std::set< OctItem* > items;
	
	
	std::vector< Box3 > treeVoxels;
    std::vector< Box3 > voxels;
    
	tree.getItems( vec3( 0, 0, 0 ), 1, items);
	errorCheck( items.size() == 0 );
	
	Box3 maxBox( kOrigin3, 8 );
	voxels.push_back( maxBox );
    tree.getVoxels( maxBox, treeVoxels );
	verifyVoxels( treeVoxels, voxels );
	
	OctItem i1( vec3( 2, 2, 2 ), 1);
	OctItem i2( vec3( -4, -4, -4 ), 1);
	
	tree.add( &i1, i1.mPos, i1.mRadius );

	// 1 item should not split space
	treeVoxels.clear();
	errorCheck( voxels.size() == 1 );
	tree.getVoxels( maxBox, treeVoxels );
	verifyVoxels( treeVoxels, voxels );
	
	// test two items in separate 1/8
	tree.add( &i2, i2.mPos, i2.mRadius );
	
	// check space is split evenly into 8
	voxels.clear();
	split8( maxBox, voxels );
	errorCheck( voxels.size() == 8 );
    for( Box3& b : voxels )
    {
        errorCheck( isEqualVec( b.getSize(), vec3( 8, 8, 8 )));
    }
	
	treeVoxels.clear();
    tree.getVoxels( maxBox, treeVoxels );
	errorCheck( verifyVoxels( treeVoxels, voxels ) );
	
	items.clear();
	tree.getItems( vec3( 4, 4, 4 ), 2, items );
	errorCheck( items.size() == 1 );
	errorCheck( *(items.begin()) == &i1 );
	
	items.clear();
    tree.getItems( vec3( -4, -4, -4 ), 2, items );
	errorCheck( items.size() == 1 );
	errorCheck( *(items.begin()) == &i2 );
	
	items.clear();
	tree.getItems( vec3( -4, 4, 4 ), 2, items );
	errorCheck( items.size() == 0 );
	
	//items.clear();
	//tree.getItems( vec3( 0, 0, 0 ), 3, items );
	//errorCheck( items.size() == 0 );
	
	items.clear();
	tree.getItems( vec3( 0, 0, 0 ), 3.5, items );
	errorCheck( items.size() == 2 );
	
	// test ray intersect
	items.clear();
	tree.getItems( vec3( 6, 6, 6 ), vec3( 0, 0, 0 ), .25, items );
	errorCheck( items.size() == 1 );
	errorCheck( *(items.begin()) == &i1 );
	
	items.clear();
	tree.getItems( vec3( 6, 6, 6 ), vec3( -6, -6, -6 ), .25, items );
	errorCheck( items.size() == 2 );
	
	// test a ray miss
	items.clear();
	tree.getItems( vec3( 6 + 2, 6, 6 ), vec3( -6 + 2, -6, -6 ), .25, items );
	errorCheck( items.size() == 0 );
	
	// test ray hit due to ray radius
	items.clear();
	tree.getItems( vec3( 6 + 2, 6, 6 ), vec3( -6 + 2, -6, -6 ), 2.0, items );
	errorCheck( items.size() == 2 );
	
    // test remove an item
    tree.remove( &i1 );
	
	// verify back to single voxel
	treeVoxels.clear();
	tree.getVoxels( maxBox, treeVoxels );
	errorCheck( treeVoxels.size() == 1 );
	errorCheck( treeVoxels[ 0 ] == Box3( kOrigin3, 8 ));
	
   	items.clear();
	tree.getItems( vec3( 4, 4, 4 ), 4, items );
    errorCheck( items.size() == 0 );
	
	treeVoxels.clear();
	bool result = tree.getVoxels( &i1, treeVoxels );
	errorCheck( result == false );
	errorCheck( treeVoxels.size() == 0 );
	
	// remaining one should be back to just one voxel - root
	voxels.clear();
	tree.getVoxels( &i2, voxels );
	errorCheck( voxels.size() == 1 );
	errorCheck( voxels[ 0 ] == Box3( kOrigin3, 8 ));
	
	// test 3 items
	i1.mPos = vec3( 2, 2, 2 );
	tree.add( &i1, i1.mPos, i2.mRadius );
	
	OctItem i3( vec3( 6, 6, 6 ), 1 );
	tree.add( &i3, i3.mPos, i3.mRadius );
	
	treeVoxels.clear();
	tree.getVoxels( maxBox, treeVoxels );
	errorCheck( treeVoxels.size() == 7 + 8 );
	// must have subdivided
	errorCheck( verifyVoxels( treeVoxels, voxels) == false );
	
	voxels.clear();
	tree.getVoxels( &i2, voxels );
	errorCheck( voxels.size() == 1 );
	errorCheck( voxels[ 0 ] == Box3( i2.mPos, 4 ));
	
	voxels.clear();
	tree.getVoxels( &i1, voxels );
	errorCheck( voxels.size() == 1 );
	errorCheck( voxels[ 0 ] == Box3( i1.mPos, 2 ));
	
	voxels.clear();
	tree.getVoxels( &i3, voxels );
	errorCheck( voxels.size() == 1 );
	errorCheck( voxels[ 0 ] == Box3( i3.mPos, 2 ));
	
	// test recombine
	tree.remove( &i3 );
	voxels.clear();
	tree.getVoxels( &i1, voxels );
	errorCheck( voxels.size() == 1 );
	errorCheck( voxels[ 0 ] == Box3( vec3( 4, 4, 4 ), 4 ));
	
	// recombine whole space
	tree.remove( &i2 );
	voxels.clear();
	tree.getVoxels( &i1, voxels );
	errorCheck( voxels.size() == 1 );
	errorCheck( voxels[ 0 ] == Box3( kOrigin3, 8 ));
	
	// verify back to single voxel
	treeVoxels.clear();
	tree.getVoxels( maxBox, treeVoxels );
	errorCheck( treeVoxels.size() == 1 );
	errorCheck( treeVoxels[ 0 ] == Box3( kOrigin3, 8 ));
	
	// test clear
	tree.clear();
	
	treeVoxels.clear();
	result = tree.getVoxels( &i1, treeVoxels );
	errorCheck( result == false );
	errorCheck( treeVoxels.size() == 0 );
	
	treeVoxels.clear();
	tree.getVoxels( maxBox, treeVoxels );
	errorCheck( treeVoxels.size() == 1 );
	errorCheck( treeVoxels[ 0 ] == Box3( kOrigin3, 8 ));
	
}

// test octtree with items overlapping multiple voxels
void testOctTreeSpan()
{
	vec3 minSize( -8, -8, -8 );
	vec3 maxSize( 8, 8, 8 );
	
	octTree< OctItem* > tree( minSize, maxSize, 1 );
	std::set< OctItem* > items;
	
	OctItem i1( vec3( 0, 0, 0 ), .5 );
	OctItem i2( vec3( 3, 3, 3 ), .5 );
	OctItem i3( vec3( 0, 0, 0 ), 1 );
	
	// put overlap first so it will need to split by second object
	tree.add( &i1, i1.mPos, i1.mRadius );
	tree.add( &i2, i2.mPos, i2.mRadius );
	
	std::vector< Box3 > voxels;
	voxels.clear();
	// on a border
	tree.getVoxels( &i1, voxels );
	errorCheck( voxels.size() == 8 );
	
	// not on a border
	voxels.clear();
	tree.getVoxels( &i2, voxels );
	errorCheck( voxels.size() == 1 );
	
	// try recombining the spanning object
	tree.remove( &i2 );
	voxels.clear();
	bool result = tree.getVoxels( &i2, voxels );
	errorCheck( result == false );
	errorCheck( voxels.size() == 0 );
	
	// must be root voxel only
	voxels.clear();
	result = tree.getVoxels( &i1, voxels );
	errorCheck( voxels.size() == 1 );
	errorCheck( voxels[ 0 ] == Box3( kOrigin3, 8 ));
	
	// add in item at same location
	tree.add( &i3, i3.mPos, i3.mRadius );
	
	// no dist between, should not divide
	voxels.clear();
	result = tree.getVoxels( &i1, voxels );
	errorCheck( voxels.size() == 1 );
	errorCheck( voxels[ 0 ] == Box3( kOrigin3, 8 ));
	
	items.clear();
	tree.getItems( kOrigin3, .25, items );
	errorCheck( items.size() == 2 );
	
}

void testBigOctTree()
{
	float32 space = 1.0;
	float32 radius = .25;
	vec3 minSize( -8, -8, -8 );
	vec3 maxSize( 8, 8, 8 );
	
	octTree< OctItem* > tree( minSize, maxSize, 1.0 );
	
	std::vector< OctItem* > items2;
	
    // test large data set
	// put them in center of potential voxels
    tree.clear();
    for( float32 x = minSize.mX + .5; x <= maxSize.mX; x += space )
    {
		for( float32 y = minSize.mY + .5; y <= maxSize.mY; y += space )
        {
            for( float32 z = minSize.mZ + .5; z <= maxSize.mZ; z += space )
            {
				OctItem* item = new OctItem( vec3( x, y, z ), .25 );
				items2.push_back( item );
                tree.add( item, item->mPos, item->mRadius );
            }
        }
    }

	std::set< OctItem* > items;
	items.clear();
    tree.getItems( vec3( .5, .5, .5 ), .5, items );
    errorCheck( items.size() == 1 );
	OctItem* item0 = *(items.begin());
	errorCheck( item0->mPos == vec3( .5, .5, .5 ) );
	
	std::vector< Box3 > voxels;
	voxels.clear();
	
	// only sizes of 2 voxels are guareenteed to reduce
	// so it won't get to size 1
	tree.getVoxels( Box3( vec3( .5, .5, .5 ), .25 ), voxels );
	errorCheck( voxels.size() == 1 );
	errorCheck( voxels[ 0 ] == Box3( vec3( 1, 1, 1 ), 1 ));
	
	items.clear();
	tree.getItems( vec3( 2.5, 2.5, 2.5 ), .25, items );
	errorCheck( items.size() == 1 );
	item0 = *(items.begin());
	errorCheck( item0->mPos == vec3( 2.5, 2.5, 2.5 ) );
	
	// test a miss
	items.clear();
	tree.getItems( vec3( 0, 0, 0 ), .1, items );
	errorCheck( items.size() == 0 );

	// get a group
	items.clear();
	tree.getItems( vec3( 0, 0, 0 ), .5, items );
	errorCheck( items.size() == 8 );

	voxels.clear();
	tree.getVoxels( Box3( vec3( 0, 0, 0 ), .5 ), voxels );
	errorCheck( voxels.size() == 8 );
	
    // test ray collisions
    items.clear();
    tree.getItems( vec3( 1, 1, 1 ), vec3( .9, .9, .9 ), .1, items );
    errorCheck( items.size() == 0 );
    
    items.clear();
    tree.getItems( vec3( 1, 1, 1 ), vec3( 0, 0, 0 ), .25, items );
    errorCheck( items.size() == 1 );
    errorCheck( (*(items.begin()))->mPos == vec3( .5 , .5, .5 ) );
    
    items.clear();
    tree.getItems( vec3( 1, 0, 0 ), vec3( 0, 0, 0 ), .1, items );
    errorCheck( items.size() == 0 );
    
    // test a long ray with no collisions
    items.clear();
    tree.getItems( vec3( 8, 0, 0 ), vec3( -8, 0, 0 ), .1, items );
    errorCheck( items.size() == 0 );
        
	// test removing all the items
	errorCheck( tree.getNumItems() == items2.size() );
	int32 removeCount = 0;
	for( OctItem* item : items2 )
	{
		++removeCount;
	
		// test still exists - not removed incorrectly
		items.clear();
		tree.getItems( item->mPos, item->mRadius, items );
		errorCheck( items.size() == 1 );
		OctItem* item2 = *(items.begin());
		errorCheck( item2 == item );
		
		tree.remove( item );
		
		// test removed
		items.clear();
		tree.getItems( item->mPos, item->mRadius, items );
		errorCheck( items.size() == 0 );
		
		delete item;
	}
	
	items2.clear();
	
	errorCheck( tree.getNumItems() == 0 );
	
	// verify single root voxel remains
	voxels.clear();
	tree.getVoxels( Box3( vec3( .5, .5, .5 ), .25 ), voxels );
	errorCheck( voxels.size() == 1 );
	errorCheck( voxels[ 0 ] == Box3( kOrigin3, 8 ) );
}
