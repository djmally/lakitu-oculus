//
// dhUserPrefsDialog.h - Part of the Drunken Hyena Common Library for DirectX9
//
// Copyright 2005 by Ken Paulson
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the Drunken Hyena License.  If a copy of the license was
// not included with this software, you may get a copy from:
// http://www.drunkenhyena.com/docs/DHLicense.txt
//

#ifndef DH_COMMON_USER_PREFS_DIALOG_H
#define DH_COMMON_USER_PREFS_DIALOG_H
#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <d3d9.h>

class dhUserPrefs{

public:
   dhUserPrefs(void);
   dhUserPrefs(const char *p_app_name);
   ~dhUserPrefs(void);

   bool GetFullscreen(void);
   D3DDEVTYPE GetDeviceType(void);
   DWORD GetAdapter(void);

   bool QueryUser(void);

protected:

   bool register_window_class(void);
   bool create_window(void);
   bool add_widgets(void);
   bool create_fullscreen_widgets(void);
   bool create_device_type_widgets(void);
   bool create_adapter_widgets(void);
   bool find_adapters(void);
   static LRESULT CALLBACK window_proc(HWND p_hwnd,UINT p_msg,WPARAM p_wparam,LPARAM p_lparam);

   static bool m_fullscreen;
   static D3DDEVTYPE m_device_type;
   static DWORD m_adapter;

   const char *m_app_name;

   HWND m_window;

   HWND m_grp_forw;
   static HWND m_btn_fullscreen;
   HWND m_btn_windowed;

   HWND m_btn_okay;
   HWND m_btn_cancel;

   HWND m_grp_dev_type;
   static HWND m_btn_device_hal;
   HWND m_btn_device_ref;

   static bool m_exited_okay;

   HWND m_grp_adapter;
   static HWND m_cmb_adapters;
   DWORD m_adapter_count;
   D3DADAPTER_IDENTIFIER9 m_adapters[9];
};


#endif //#ifndef DH_COMMON_USER_PREFS_DIALOG_H


