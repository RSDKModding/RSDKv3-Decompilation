#ifdef __APPLE__
#include "cocoaHelpers.hpp"

#import <Foundation/Foundation.h>

const char* getResourcesPath(void)
{
    @autoreleasepool
    {
        NSString* resource_path = [[NSBundle mainBundle] resourcePath];
        
        return (char*)[resource_path UTF8String];
    }
}
const char* getDocumentsPath(void)
{
    @autoreleasepool
    {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* path = [paths objectAtIndex:0];
        
        return (char*)[path UTF8String];
    }
}
#endif
