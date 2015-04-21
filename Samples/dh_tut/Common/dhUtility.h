//
// dhUtility.h - Part of the Drunken Hyena Common Library for DirectX9
//
// Copyright 2004 by Ken Paulson
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the Drunken Hyena License.  If a copy of the license was
// not included with this software, you may get a copy from:
// http://www.drunkenhyena.com/docs/DHLicense.txt
//
#ifndef DH_COMMON_UTILTY_H
#define DH_COMMON_UTILTY_H
#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include<d3d9.h>



bool dhAskFullscreen(const char *p_title);
void dhMessagePump(void);

D3DFORMAT dhFind16BitMode(IDirect3D9 *p_d3d);
D3DFORMAT dhFind32BitMode(IDirect3D9 *p_d3d);
HRESULT dhGetFormat(IDirect3D9 *p_d3d,bool p_fullscreen,UCHAR p_depth,D3DFORMAT *p_format);

void dhLog(const char *p_msg,HRESULT p_hr=D3D_OK);
void dhLogErase(void);


HRESULT dhHandleLostDevice(IDirect3DDevice9 *p_device,D3DPRESENT_PARAMETERS *p_pp,HRESULT p_hr);



#endif //#ifndef DH_COMMON_UTILTY_H
