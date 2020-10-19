#include <iostream>
#include <windows.h>
#include "d3d12.h"

typedef struct D3D12_FEATURE_DATA_FEATURE_LEVELS {
    UINT NumFeatureLevels;
    const D3D_FEATURE_LEVEL *pFeatureLevelsRequested;
    D3D_FEATURE_LEVEL MaxSupportedFeatureLevel;
};

D3D_FEATURE_LEVEL featureLevels[3] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3
};

D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelsInfo;


int main()
{
    std::cout << "Hello World!\n";
}

