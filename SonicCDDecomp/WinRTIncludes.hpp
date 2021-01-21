#pragma once
#include <winrt/base.h>
#include <winrt/Windows.Storage.h>

inline char *getResourcesPath()
{
    static char resourcePath[256] = { 0 };

    if (strlen(resourcePath) == 0) {
        auto folder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
        auto path   = to_string(folder.Path());
        
        std::copy(path.begin(), path.end(), resourcePath);
    }

    return resourcePath;
}