// xll.h - Excel add-in library header file
// Copyright (c) KALX, LLC. All rights reserved. No warranty made.
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
//#define RC_INVOKED
#include <Windows.h>

// Module handle from DllMain
extern HINSTANCE xll_hModule;
extern HINSTANCE xll_Instance;

#include "fp.h"
#include "error.h"
#include "on.h"
#include "addin.h"
#include "handle.h"
#include "test.h"

#ifndef _LIB
//#pragma comment(linker, "/include:" XLL_DECORATE("xll_this", 0))
//#pragma comment(linker, "/include:" XLL_DECORATE("xll_trace", 4))
#pragma comment(linker, "/include:" XLL_DECORATE("xll_paste_function", 0))
//#pragma comment(linker, "/include:" XLL_DECORATE("xll_make_doc", 0))
#pragma comment(linker, "/include:" XLL_DECORATE("xll_make_shfb", 0))
//#pragma comment(linker, "/include:" XLL_DECORATE("xll_get_workbook", 4))
//#pragma comment(linker, "/include:" XLL_DECORATE("xll_get_workspace", 4))
#endif // _LIB
