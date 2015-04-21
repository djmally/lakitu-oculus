//
// dhD3D.h - Part of the Drunken Hyena Common Library for DirectX9
//
// Copyright 2004 by Ken Paulson
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the Drunken Hyena License.  If a copy of the license was
// not included with this software, you may get a copy from:
// http://www.drunkenhyena.com/docs/DHLicense.txt
//
#ifndef DH_COMMON_D3D_H
#define DH_COMMON_D3D_H
#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <d3d9.h>


HRESULT dhInitD3D(IDirect3D9 **p_d3d);
void dhInitPresentParameters(bool p_fullscreen,HWND p_window,DWORD p_width,DWORD p_height,D3DFORMAT p_format,D3DFORMAT p_depth,D3DPRESENT_PARAMETERS *p_pp);
HRESULT dhInitDevice(IDirect3D9 *p_d3d,DWORD p_adapter,D3DDEVTYPE p_dev_type,HWND p_window,D3DPRESENT_PARAMETERS *p_pp,IDirect3DDevice9 **p_device);

void dhKillD3D(IDirect3D9 **p_d3d,IDirect3DDevice9 **p_device);

HRESULT SetupDirect3D(const char *p_app_name,DWORD p_width,DWORD p_height,WNDPROC p_wndproc,
                      UCHAR p_depth,bool p_use_z,
                      HWND *p_window,IDirect3D9 **p_d3d,IDirect3DDevice9 **p_device,
                      D3DPRESENT_PARAMETERS *p_pp);

#endif //#ifndef DH_COMMON_D3D_H


