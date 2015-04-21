//
// dhUserPrefsDialog.cpp - Part of the Drunken Hyena Common Library for DirectX9
//
// Copyright 2007 by Ken Paulson
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the Drunken Hyena License.  If a copy of the license was
// not included with this software, you may get a copy from:
// http://www.drunkenhyena.com/docs/DHLicense.txt
//

#include <atlstr.h>
#include <stdio.h>

#include "dhUserPrefsDialog.h"
#include "dhUtility.h"

static const CString user_pref_wnd_class_name("dhUserPrefWndClass");
static const DWORD window_width = 420;
static const DWORD window_height = 300;
static const int left_edge = 80;
static const int right_edge = 340;


HWND dhUserPrefs::m_btn_fullscreen;
bool dhUserPrefs::m_fullscreen = false;

HWND dhUserPrefs::m_btn_device_hal;
D3DDEVTYPE dhUserPrefs::m_device_type = D3DDEVTYPE_HAL;

HWND dhUserPrefs::m_cmb_adapters;
DWORD dhUserPrefs::m_adapter = 0;

bool dhUserPrefs::m_exited_okay= false;


enum dhUserPrefsBtns{
   btn_Okay = 1,
   btn_Cancel
};


dhUserPrefs::dhUserPrefs(void){

   dhUserPrefs(NULL);

}

dhUserPrefs::dhUserPrefs(const char *p_app_name){

   m_adapter_count = 0;

   m_window = NULL;

   m_app_name = p_app_name;

}

dhUserPrefs::~dhUserPrefs(void){

}

bool dhUserPrefs::GetFullscreen(void){

   return m_fullscreen;

}
D3DDEVTYPE dhUserPrefs::GetDeviceType(void){

   return m_device_type;

}

DWORD dhUserPrefs::GetAdapter(void){

   return m_adapter;

}

bool dhUserPrefs::QueryUser(void){

   find_adapters();


   if(!create_window()){

      return false;

   }

   MSG msg;

   while(GetMessage( &msg, NULL, 0, 0 )){
      if (!IsDialogMessage(m_window,&msg)) { //This makes TAB work as it should
         TranslateMessage(&msg); 
         DispatchMessage(&msg); 
      }
   }


   UnregisterClass(user_pref_wnd_class_name,GetModuleHandle(NULL));

   return m_exited_okay;

}

bool dhUserPrefs::find_adapters(void){
IDirect3D9 *d3d;

   d3d = Direct3DCreate9( D3D_SDK_VERSION);

   m_adapter_count = d3d->GetAdapterCount();

   //HACK: We only support up to 9 adapters.
   if (m_adapter_count > 9) {
      m_adapter_count = 9;
   }


   for (DWORD i=0;i < m_adapter_count;i++) {
      d3d->GetAdapterIdentifier(i,0,&m_adapters[i]);
   }

   d3d->Release();

   return true;

}

bool dhUserPrefs::register_window_class(void){
WNDCLASSEX window_class;
ATOM success;

   window_class.cbSize         = sizeof(WNDCLASSEX);
   window_class.style          = 0;
   window_class.cbClsExtra     = 0;
   window_class.cbWndExtra     = 0;
   window_class.hInstance      = GetModuleHandle(NULL);
   window_class.hIcon          = LoadIcon(NULL,IDI_APPLICATION);
   window_class.hIconSm        = NULL;
   window_class.hCursor        = LoadCursor(NULL,IDC_ARROW);
   window_class.hbrBackground  = (HBRUSH)(COLOR_BTNFACE + 1);
   window_class.lpszMenuName   = NULL;
   window_class.lpszClassName  = user_pref_wnd_class_name;
   //Here we provide our default window handler, all windows messages
   //will be sent to this function.
   window_class.lpfnWndProc    = window_proc;



   //Register the class with windows
   success = RegisterClassEx(&window_class);
   if(!success){
      dhLog("Error registering window class");
   }

   return (success != 0);

}

bool dhUserPrefs::create_window(void){
DWORD screen_width;
DWORD screen_height;
CString window_name;

   if(!register_window_class()){

      return false;

   }

   if(m_app_name != NULL)
	{
		window_name.Format("%s : User Preferences",m_app_name);
   }
	else
	{
      window_name = "User Preferences";
   }

   screen_width=GetSystemMetrics(SM_CXSCREEN);
   screen_height=GetSystemMetrics(SM_CYSCREEN);

   m_window = CreateWindowEx(WS_EX_APPWINDOW|WS_EX_DLGMODALFRAME,
                             user_pref_wnd_class_name,
                             window_name,
                             WS_CAPTION|WS_VISIBLE,
                             (screen_width - window_width)/2,
                             (screen_height - window_height)/2,
                             window_width,
                             window_height,
                             NULL,                           //WindowParent
                             NULL,                           //Menu
                             GetModuleHandle(NULL),          //Instance
                             NULL);                          //lpParam


   add_widgets();

   return true;

}

bool dhUserPrefs::add_widgets(void){


   create_fullscreen_widgets();

   create_device_type_widgets();

   create_adapter_widgets();

   m_btn_okay = CreateWindow("button","Okay",WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|WS_TABSTOP|WS_GROUP,
                                   left_edge,240,100,25,m_window,(HMENU)btn_Okay,GetModuleHandle(NULL),NULL);

   m_btn_cancel = CreateWindow("button","Cancel",WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|WS_TABSTOP|WS_GROUP,
                                   right_edge - 100,240,100,25,m_window,(HMENU)btn_Cancel,GetModuleHandle(NULL),NULL);
   return true;

}

bool dhUserPrefs::create_fullscreen_widgets(void){

   m_grp_forw = CreateWindow("button","Full Screen?",WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP,
                             left_edge,20,120,70,m_window,NULL,GetModuleHandle(NULL),NULL);

   m_btn_fullscreen = CreateWindow("button","Full Screen",WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP,
                                   left_edge + 10,40,100,15,m_window,NULL,GetModuleHandle(NULL),NULL);

   m_btn_windowed = CreateWindow("button","Windowed",WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_TABSTOP,
                                 left_edge + 10,65,100,20,m_window,NULL,GetModuleHandle(NULL),NULL);

   if(m_fullscreen){
      SendMessage(m_btn_fullscreen,BM_SETCHECK,1,0);
   }else{
      SendMessage(m_btn_windowed,BM_SETCHECK,1,0); 
   }

   return true;

}

bool dhUserPrefs::create_device_type_widgets(void){

   m_grp_dev_type = CreateWindow("button","Device Type",WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP,
                             220,20,120,70,m_window,NULL,GetModuleHandle(NULL),NULL);

   m_btn_device_hal = CreateWindow("button","HAL",WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP,
                                   220 + 10,40,100,15,m_window,NULL,GetModuleHandle(NULL),NULL);

   m_btn_device_ref = CreateWindow("button","Reference",WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_TABSTOP,
                                 220 + 10,65,100,20,m_window,NULL,GetModuleHandle(NULL),NULL);

   if(m_device_type == D3DDEVTYPE_HAL){
      SendMessage(m_btn_device_hal,BM_SETCHECK,1,0);
   }else{
      SendMessage(m_btn_device_ref,BM_SETCHECK,1,0); 
   }


   return true;

}

bool dhUserPrefs::create_adapter_widgets(void){

   int adapter_left = left_edge - 35;

   m_grp_adapter = CreateWindow("button","Adapter",WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP,
                                adapter_left,105,320,70,m_window,NULL,GetModuleHandle(NULL),NULL);


   m_cmb_adapters = CreateWindow("COMBOBOX","Adapter",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_TABSTOP|WS_GROUP|WS_VSCROLL,
                                 adapter_left + 10,135,right_edge - left_edge+40,60,m_window,NULL,GetModuleHandle(NULL),NULL);

   CString buffer;
   for (DWORD i=0;i < m_adapter_count;i++) {

		buffer.Format("%s (%s)",m_adapters[i].Description,m_adapters[i].DeviceName);
      SendMessage(m_cmb_adapters,CB_ADDSTRING,0,(LPARAM)(LPCTSTR)buffer);
   }

   SendMessage(m_cmb_adapters,CB_SETCURSEL,0,0);

   return true;

}



//******************************************************************************************
// Function:window_proc
// Whazzit:This handles any incoming Windows messages and sends any that aren't handled to
//         DefWindowProc for Windows to handle.
//******************************************************************************************
LRESULT CALLBACK dhUserPrefs::window_proc(HWND p_hwnd,UINT p_msg,WPARAM p_wparam,LPARAM p_lparam){
DWORD id = LOWORD(p_wparam);
LRESULT result;

   switch(p_msg){

      case WM_CREATE:
         m_exited_okay = false;
         break;

      case WM_COMMAND:
         if(id == btn_Okay){
            result=SendMessage(m_btn_fullscreen,BM_GETCHECK,0,0);
            m_fullscreen = (result == BST_CHECKED);

            result=SendMessage(m_btn_device_hal,BM_GETCHECK,0,0);
            if (result) {
               m_device_type = D3DDEVTYPE_HAL;
            }else{
               m_device_type = D3DDEVTYPE_REF;
            }

            result = SendMessage(m_cmb_adapters,CB_GETCURSEL,0,0);

            m_adapter = (DWORD)result;


            m_exited_okay = true;

            DestroyWindow(p_hwnd);
            return 0;
         }else if (id == btn_Cancel) {
            DestroyWindow(p_hwnd);
            return 0;
         }

         break;
      case WM_DESTROY:
         PostQuitMessage(0);
         break;
   }
   
   return (DefWindowProc(p_hwnd,p_msg,p_wparam,p_lparam));
   
}
