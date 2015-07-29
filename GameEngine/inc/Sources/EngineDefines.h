
#pragma once

#ifndef _WIN64
#define GAMEENGINE_STATIC
#define GAMEENGINE_OPENGL
#endif

#ifndef GAMEENGINE_OPENGL
#define GAMEENGINE_DIRECTX
#endif


#ifdef GAMEENGINE_STATIC
#define GAMEENGINE_API
#elif defined(GAMEENGINE_EXPORTS)
#define GAMEENGINE_API __declspec(dllexport)
#define STL_API 
#else
#define GAMEENGINE_API __declspec(dllimport)
#define STL_API extern
#endif

