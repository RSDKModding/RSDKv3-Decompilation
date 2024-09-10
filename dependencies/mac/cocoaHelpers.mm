#ifdef __APPLE__

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#include "cocoaHelpers.hpp"
#include <sys/stat.h>

void getResourcesPath(char* buffer, int bufferSize)
{
    @autoreleasepool
    {
        NSString* appFolder = [NSBundle.mainBundle.bundlePath stringByDeletingLastPathComponent];
        NSString* dataFile = [appFolder stringByAppendingString:@"/Data.rsdk"];
        NSString* dataFolder = [appFolder stringByAppendingString:@"/Data"];
        NSString* settingsFile = [appFolder stringByAppendingString:@"/settings.ini"];
        if([NSFileManager.defaultManager fileExistsAtPath:dataFile] ||
           [NSFileManager.defaultManager fileExistsAtPath:settingsFile] ||
           [NSFileManager.defaultManager fileExistsAtPath:dataFolder]){
            [appFolder getCString:buffer maxLength:bufferSize encoding:NSUTF8StringEncoding];
            return;
        }
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSString *applicationSupportDirectory = [paths firstObject];
        NSString* gameData = [applicationSupportDirectory stringByAppendingString:@"/RSDKv3"];
        if(![NSFileManager.defaultManager fileExistsAtPath:gameData]){
            mkdir([gameData cStringUsingEncoding:NSUTF8StringEncoding], 0777);
        }
        [gameData getCString:buffer maxLength:bufferSize encoding:NSUTF8StringEncoding];
        return;
    }
}
#endif
