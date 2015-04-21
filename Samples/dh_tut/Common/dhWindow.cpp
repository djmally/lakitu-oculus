//
// dhWindow.cpp - Part of the Drunken Hyena Common Library for DirectX9
//
// Copyright 2004 by Ken Paulson
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the Drunken Hyena License.  If a copy of the license was
// not included with this software, you may get a copy from:
// http://www.drunkenhyena.com/docs/DHLicense.txt
//
#include "dhWindow.h"
#include "dhUtility.h"

static const char *wnd_class_name="DH Class";

//Forward declarations of private functions
void dh_adjust_window(HWND p_window);
HRESULT dh_register_window_class(WNDPROC p_wndproc);
HRESULT dh_create_window(bool p_fullscreen,DWORD p_width,DWORD p_height,const char *p_name,HWND *p_window);



//******************************************************************************************
// Function: dhInitWindow
// Whazzit:Registers a window class and then creates our window.  Returns NULL if the window
//         can't be created
//******************************************************************************************
HRESULT dhInitWindow(bool p_fullscreen,const char *p_name,DWORD p_width,DWORD p_height,WNDPROC p_wndproc,HWND *p_window){
HRESULT hr;

   hr = dh_register_window_class(p_wndproc);
   if (FAILED(hr)) {
      return hr;
   }

   hr = dh_create_window(p_fullscreen,p_width,p_height,p_name,p_window);
   if(FAILED(hr)){
      dhLog("Error opening window");
      dhKillWindow(p_window);
      return E_FAIL;
   }

   //Hide the mouse in full-screen
   if(p_fullscreen){
      ShowCursor(FALSE);
   }
   
   //When you request a window of a certain width, that width includes the
   // borders, so we adjust the window so the client (drawable) area is
   // as big as we requested
   // Also, by querying for the display size we can center the window
   if(!p_fullscreen){
      dh_adjust_window(*p_window);
   }

   //Now that all adjustments have been made, we can show the window
   ShowWindow(*p_window,SW_SHOW);

   return hr;
}


//******************************************************************************************
// Function: dh_register_window_class
// Whazzit:Registers the window class that we'll use to create our window
//******************************************************************************************
HRESULT dh_register_window_class(WNDPROC p_wndproc){
HINSTANCE instance=GetModuleHandle(NULL);
WNDCLASS window_class;

   //Fill in all the fields for the WNDCLASS structure.  Window classes
   //are a sort of template for window creation.  You could create many
   //windows using the same window class.
   window_class.style          = CS_OWNDC;
   window_class.cbClsExtra     = 0;
   window_class.cbWndExtra     = 0;
   window_class.hInstance      = instance;
   window_class.hIcon          = LoadIcon(NULL,IDI_APPLICATION);
   window_class.hCursor        = LoadCursor(NULL,IDC_ARROW);
   window_class.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
   window_class.lpszMenuName   = NULL;
   window_class.lpszClassName  = wnd_class_name;
   //Here we provide our default window handler, all windows messages
   //will be sent to this function.
   window_class.lpfnWndProc    = p_wndproc;

   //Register the class with windows
   if(!RegisterClass(&window_class)){
      dhLog("Error registering window class");
      return E_FAIL;
   }

   return S_OK;

}

//******************************************************************************************
// Function: dh_create_window
// Whazzit:Adjusts window so client area matches the size requested, also centers the window
//******************************************************************************************
HRESULT dh_create_window(bool p_fullscreen,DWORD p_width,DWORD p_height,const char *p_name,HWND *p_window){
ULONG window_width, window_height;
HINSTANCE instance=GetModuleHandle(NULL);
DWORD style;

   //If we're running full screen, we cover the desktop with our window.
   //This isn't necessary, but it provides a smoother transition for the
   //user, especially when we're going to change screen modes.
   if(p_fullscreen){
      window_width=GetSystemMetrics(SM_CXSCREEN);
      window_height=GetSystemMetrics(SM_CYSCREEN);
      style=WS_POPUP|WS_VISIBLE;
   }else{
      //In windowed mode, we just make the window whatever size we need.
      window_width=p_width;
      window_height=p_height;
      style=WS_OVERLAPPED|WS_SYSMENU;
   }

   //Here we actually create the window.  For more detail on the various
   //parameters please refer to the Win32 documentation.
   (*p_window)=CreateWindow(wnd_class_name, //name of our registered class
                            p_name, //Window name/title
                            style,      //Style flags
                            0,          //X position
                            0,          //Y position
                            window_width,//width of window
                            window_height,//height of window
                            NULL,       //Parent window
                            NULL,       //Menu
                            instance, //application instance handle
                            NULL);      //pointer to window-creation data


   return S_OK;
}

//******************************************************************************************
// Function: dh_adjust_window
// Whazzit:Adjusts window so client area matches the size requested, also centers the window
//******************************************************************************************
void dh_adjust_window(HWND p_window){
RECT client_rect;
RECT window_rect;
ULONG window_width,window_height;
ULONG screen_width,screen_height;
ULONG new_x, new_y;

   GetClientRect(p_window,&client_rect);
   GetWindowRect(p_window,&window_rect);

   window_width = window_rect.right - window_rect.left;
   window_height = window_rect.bottom - window_rect.top;

   window_width = window_width + (window_width - client_rect.right);
   window_height = window_height + (window_height - client_rect.bottom);

   screen_width=GetSystemMetrics(SM_CXSCREEN);
   screen_height=GetSystemMetrics(SM_CYSCREEN);

   new_x = (screen_width - window_width) / 2;
   new_y = (screen_height - window_height) / 2;

   MoveWindow(p_window,
              new_x, //Left edge
              new_y, //Top Edge
              window_width,    //New Width
              window_height, //New Height
              TRUE);

}

//******************************************************************************************
// Function: dhKillWindow
// Whazzit:Closes the window, clean up any waiting messages, and then unregister 
//         our window class.  Note - This is not the standard Win32 way of cleaning
//         up your window.  The standard way involves putting the clean-up code in
//         your window handler.  With this method, we destroy what we create.
//******************************************************************************************
void dhKillWindow(HWND *p_window){
HINSTANCE instance=GetModuleHandle(NULL);
   
   //Test if our window is valid
   if((*p_window)){
      if(!DestroyWindow((*p_window))){
         //We failed to destroy our window, this shouldn't ever happen
         dhLog("Destroy Window Failed");
      }else{
         MSG msg;
         //Clean up any pending messages
         while(PeekMessage(&msg, NULL, 0, 0,PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
         }
      }
   }

   (*p_window)=NULL;
   
   //Unregister our window, if we had opened multiple windows using this
   //class, we would have to close all of them before we unregistered the class.
   if(!UnregisterClass(wnd_class_name,instance)){
      dhLog("Error unregistering window class");
   }

}

