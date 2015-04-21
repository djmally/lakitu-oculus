//
// dhUtility.cpp - Part of the Drunken Hyena Common Library for DirectX9
//
// Copyright 2007 by Ken Paulson
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the Drunken Hyena License.  If a copy of the license was
// not included with this software, you may get a copy from:
// http://www.drunkenhyena.com/docs/DHLicense.txt
//

#include <atlstr.h>

#include <fstream>
//#include <stdio.h>

#include "dhUtility.h"

#pragma comment(lib,"dxerr9.lib")

static const TCHAR dhlog_name[]="log.txt";

//These are defined in your app
extern void InitVolatileResources(void);
extern void FreeVolatileResources(void);


//******************************************************************************************
// Function:dhAskFullscreen
// Whazzit:Ask the user if they would like to run in full-screen or windowed mode or if they
//         would like to Cancel (abort).
// NOTE:This function will cause your program to exit if the user selects 'CANCEL' or if
//      the MessageBox fails
//******************************************************************************************
bool dhAskFullscreen(const TCHAR *p_title){
int full_result;
bool full_screen=true;

   full_result=MessageBox(NULL,"Would you like to run in fullscreen mode?",p_title,
                          MB_YESNOCANCEL|MB_ICONQUESTION);
   switch(full_result){
      case IDCANCEL: //User hit 'Cancel' button, so we quit
         MessageBox(NULL,"User Abort",p_title,MB_OK);
         dhLog("User Abort");
         exit(EXIT_SUCCESS);
         break;
      case IDNO:     //User hit 'No' button, run in a window
         full_screen=false;
         dhLog("Running in windowed mode");
         break;
      case IDYES:    //User hit 'Yes' button, run full-screen
         dhLog("Running in full-screen");
         full_screen=true;
         break;
      case 0:        //Error!  Couldn't open dialog box
         dhLog("Couldn't open MessageBox, dying");
         exit(EXIT_FAILURE);
         break;
   }

   return full_screen;

}

//******************************************************************************************
// Procedure: dhMessagePump
// Whazzit:Checks the message queue to see if any windows messages
//         (window is closing, window needs repainting, etc)
//         are waiting. If there are, the messages are dispatched
//         to our message handler (defined when the window was created).
//******************************************************************************************
void dhMessagePump(void){
MSG msg;

   while(PeekMessage(&msg, NULL, 0, 0,PM_REMOVE)){
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

}

//******************************************************************************************
// Function:dhFind16BitMode
// Whazzit:Tests a couple of 16-bit modes to see if they are supported.  Virtually every
//         graphics card in existance will support one of these 2 formats:
//         D3DFMT_R5G6B5,D3DFMT_X1R5G5B5
//         Returns D3DFMT_UNKNOWN if it can't find a valid mode
//******************************************************************************************
D3DFORMAT dhFind16BitMode(IDirect3D9 *p_d3d){
const D3DFORMAT cBufferFormats[]={D3DFMT_R5G6B5,D3DFMT_X1R5G5B5};
const int cFormatCount=sizeof(cBufferFormats)/sizeof(D3DFORMAT);
HRESULT hr=D3D_OK;
int count;


   for(count=0;count<cFormatCount;count++){

      //CheckDeviceType() is used to verify that a Device can support a particular display mode.
      hr=p_d3d->CheckDeviceType(D3DADAPTER_DEFAULT, //Test the primary display device, this is
                                                 //necessary because systems can have add-on cards
                                                 //or multi-monitor setups
                                D3DDEVTYPE_HAL,  //This states that we want support for this mode
                                              //in hardware rather than emulated in software
                                cBufferFormats[count],   //The is the primary (viewable) buffer format
                                cBufferFormats[count],   //This is the back (drawable) buffer format
                                FALSE);   //Is this windowed mode?  Nope

      if(SUCCEEDED(hr)){
         return cBufferFormats[count];
      }

   }

   dhLog("dhFind16BitMode failed to find a usable mode");
   return D3DFMT_UNKNOWN;
}

//******************************************************************************************
// Function:dhFind32BitMode
// Whazzit:Tests a couple of 32-bit modes to see if they are supported.  Virtually every
//         graphics card in existance will support one of these 2 formats:
//         D3DFMT_X8R8G8B8,D3DFMT_A8R8G8B8
//         Returns D3DFMT_UNKNOWN if it can't find a valid mode
//******************************************************************************************
D3DFORMAT dhFind32BitMode(IDirect3D9 *p_d3d){
const D3DFORMAT cBufferFormats[]={D3DFMT_X8R8G8B8,D3DFMT_A8R8G8B8};
const int cFormatCount=sizeof(cBufferFormats)/sizeof(D3DFORMAT);
HRESULT hr=D3D_OK;
int count;


   for(count=0;count<cFormatCount;count++){

      //CheckDeviceType() is used to verify that a Device can support a particular display mode.
      hr=p_d3d->CheckDeviceType(D3DADAPTER_DEFAULT, //Test the primary display device, this is
                                                 //necessary because systems can have add-on cards
                                                 //or multi-monitor setups
                                D3DDEVTYPE_HAL,  //This states that we want support for this mode
                                              //in hardware rather than emulated in software
                                cBufferFormats[count],   //The is the primary (viewable) buffer format
                                cBufferFormats[count],   //This is the back (drawable) buffer format
                                FALSE);   //Is this windowed mode?  Nope

      if(SUCCEEDED(hr)){
         return cBufferFormats[count];
      }

   }

   dhLog("dhFind32BitMode failed to find a usable mode");
   return D3DFMT_UNKNOWN;
}

//******************************************************************************************
// Function: dhGetFormat
// Whazzit: Finds an appropriate display mode for our app
//******************************************************************************************
HRESULT dhGetFormat(IDirect3D9 *p_d3d,bool p_fullscreen,UCHAR p_depth,D3DFORMAT *p_format){
HRESULT hr=D3D_OK;

   if(p_fullscreen)
	{
      switch(p_depth)
		{
         case 16:
            (*p_format)=dhFind16BitMode(p_d3d);
      	   break;
         case 32:
            (*p_format)=dhFind32BitMode(p_d3d);
      	   break;
         default:
            dhLog("dhGetFormat: called with invalid depth");
            (*p_format)=D3DFMT_UNKNOWN;
            hr = E_FAIL;
            break;
      }
   }
	else
	{
      (*p_format) = D3DFMT_UNKNOWN; //D3D will automatically use the Desktop's format
   }

   return hr;
}
//******************************************************************************************
// Procedure: dhLog
// Whazzit: Prints the a message to the debugger and to a log file. If an HRESULT
//          is provided, it prints the associated error string
//******************************************************************************************
void dhLog(const TCHAR *p_msg,HRESULT p_hr){
CString buffer;
std::ofstream file;

   if(p_hr != D3D_OK)
	{
      buffer.Format("%s : %s\n",p_msg,"DXGetErrorString9 cannot be imported :(");
   }
	else
	{
      buffer.Format("%s\n",p_msg);
   }

   OutputDebugString(buffer);

	file.open(dhlog_name,std::ios::app);
   if(file.is_open())
	{
      file << buffer;
      file.close();
   }
	else
	{
      OutputDebugString("Error writing to log file\n");
   }

}
void dhLogErase(void){

	std::ofstream file(dhlog_name,std::ios::trunc);

}
//******************************************************************************************
// Function:dhHandleLostDevice
// Whazzit:Frees resources that need to be freed so the device can be reset and
//         Resets the device when it can be.  Also re-initializes resources that
//         need to be after the Reset
//******************************************************************************************
HRESULT dhHandleLostDevice(IDirect3DDevice9 *p_device,D3DPRESENT_PARAMETERS *p_pp,HRESULT p_hr){
HRESULT hr=p_hr;

   //We've lost our device and it cannot be Reset yet.
   if(hr == D3DERR_DEVICELOST){

      FreeVolatileResources();

      // Since we can't render anyway, we might as well give up some of the CPU
      // rather than spinning constantly
      Sleep(500);

   }else if(hr == D3DERR_DEVICENOTRESET){ //The device is ready to be Reset

      hr=p_device->Reset(p_pp);

      //If the device was successfully Reset, re-Initialize the Volatile resources
      if(SUCCEEDED(hr)){
         InitVolatileResources();
      }

   }

   return hr;
}
