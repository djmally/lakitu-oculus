//
// dhWindow.h - Part of the Drunken Hyena Common Library for DirectX9
//
// Copyright 2004 by Ken Paulson
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the Drunken Hyena License.  If a copy of the license was
// not included with this software, you may get a copy from:
// http://www.drunkenhyena.com/docs/DHLicense.txt
//
#ifndef DH_COMMON_WINDOW_H
#define DH_COMMON_WINDOW_H
#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

HRESULT dhInitWindow(bool p_fullscreen,const char *p_name,DWORD p_width,DWORD p_height,
                     WNDPROC p_wndproc,HWND *p_window);

void dhKillWindow(HWND *p_window);

#endif //#ifndef DH_COMMON_WINDOW_H


