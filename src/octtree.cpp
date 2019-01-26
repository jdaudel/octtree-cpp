//
//
//  Created by Daudel, Jeffrey L on 4/26/14.
//  Copyright (c) 2014-2018 Daudel, Jeffrey L. All rights reserved.
//

#include "octtree.h"

void split8( const Box3& box, std::vector< Box3 >& out )
{
    vec3 childSize = box.getSize() / 2;
    vec3 offset = box.getSize() / 4;
    vec3 center = box.getCenter();
    
    for( int32 x = 0; x<2; ++x )
    {
        for( int32 y = 0; y<2; ++y )
        {
            for( int32 z = 0; z<2; ++z )
            {
                vec3 childPos = center;
                childPos.mX += -offset.mX + (childSize.mX * x);
                childPos.mY += -offset.mY + (childSize.mY * y);
                childPos.mZ += -offset.mZ + (childSize.mZ * z);
                
                Box3 childBounds( childPos - offset, childPos + offset );
                out.push_back( childBounds );
            }
        }
    }
}



