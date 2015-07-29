
#pragma once

#include <unordered_map>
#include <map>
#include <vector>
#include <list>
#include <atlmem.h>
#include <memory>
#include <string>
#include <stack>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <array>
#include <functional>
#include <fstream>
#include <sstream>
#include <tchar.h>
#include <set>


#ifdef _WIN64
#include <Windows.h>
#include <mfapi.h>
#include <mfmediaengine.h>
#include <mfreadwrite.h>
#include <XAudio2.h>

#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "xaudio2.lib")

#include <d3d11.h>
#include <D3DX11.h>
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <d3dx11effect.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dx11.lib")

#pragma comment(lib, "Effects11.lib")

#define FBXSDK_SHARED
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H

#pragma comment(lib, "freetype26d.lib")

#endif

//engine
#include "EngineDefines.h"
#include "MathLib.h"
#include "property.h"
#include "Object.h"
#include "Component.h"


