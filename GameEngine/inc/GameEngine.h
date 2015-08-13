#pragma once
#if !defined(_GAMEENGINE_H_) && defined(_WIN64)
#define _GAMEENGINE_H_

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
#include <queue>

#include <Windows.h>
#include <mfapi.h>
#include <mfmediaengine.h>
#include <mfreadwrite.h>
#include <XAudio2.h>

#include <d3d11.h>
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include "Sources\EngineDefines.h"

#include "Sources\property.h"
#include "Sources\MathLib.h"
#include "Sources\Object.h"
#include "Sources\Component.h"
#include "Sources\DebugClass.h"

#include "Sources\Texture2D.h"
#include "Sources\GraphicDevice.h"
#include "Sources\GameWindow.h"
#include "Sources\GameTime.h"
#include "Sources\Transform.h"
#include "Sources\GameObject.h"
#include "Sources\Camera.h"
#include "Sources\SkyBox.h"
#include "Sources\Animation.h"
//
#include "Sources\Shader.h"
#include "Sources\Material.h"
#include "Sources\Font.h"
#include "Sources\Mesh.h"
#include "Sources\GlobalSetting.h"
#include "Sources\Input.h"
#include "Sources\Light.h"
#include "Sources\Physics.h"

#include "Sources\Renderer.h"
#include "Sources\Resource.h"
#include "Sources\Scene.h"
#include "Sources\UI.h"
#endif