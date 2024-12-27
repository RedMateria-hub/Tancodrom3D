#pragma once
#include <detail/type_int.hpp>
#include "Types.h"

#ifdef _WIN64
#define SNPRINTF _snprintf_s
#define VSNPRINTF vsnprintf_s
#define RANDOM rand
#define SRANDOM srand((unsigned)time(NULL))
#pragma warning (disable: 4566)
#else
#define SNPRINTF snprintf
#define VSNPRINTF vsnprintf
#define RANDOM random
#define SRANDOM srandom(getpid())
#endif

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ZERO_MEM_VAR(var) memset(&var, 0, sizeof(var))

#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

void Message(const char* pFileName, uint line, const char* msg, ...);

#define ERROR_MESSAGE0(msg) Message(__FILE__, __LINE__, msg)
#define ERROR_MESSAGE(msg, ...) Message(__FILE__, __LINE__, msg, __VA_ARGS__)
#define MESSAGE(msg, ...) Message(__FILE__, __LINE__, msg, __VA_ARGS__)

void glDebugOutput(GLenum source,
   GLenum type,
   unsigned int id,
   GLenum severity,
   GLsizei length,
   const char* message,
   const void* userParam);

char* ReadBinaryFile(const char* pFilename, int& size);