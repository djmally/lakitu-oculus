//
// textured_quad.cpp - Textured Quad
//
// Copyright 2007 by Kenneth Paulson
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the Drunken Hyena License.  If a copy of the license was
// not included with this software, you may get a copy from:
// http://www.drunkenhyena.com/docs/DHLicense.txt
//
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <D3d9.h>
#include <D3dx9tex.h>
#include <atlstr.h>
#include "../Common/dhWindow.h"
#include "../Common/dhD3D.h"
#include "../Common/dhUtility.h"
#include "../Common/dhUserPrefsDialog.h"

// Link required libraries
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"dxerr9.lib")
#ifdef _DEBUG
  #pragma comment(lib,"d3dx9d.lib")
#else
  #pragma comment(lib,"d3dx9.lib")
#endif

// Forward declarations for all of our functions, see their definitions for more detail
LRESULT CALLBACK default_window_proc(HWND p_hwnd,UINT p_msg,WPARAM p_wparam,LPARAM p_lparam);
HRESULT init_scene(void);
void kill_scene(void);
HRESULT render(void);
HRESULT init_vb(void);
HRESULT load_texture(void);
void set_device_states(void);
void check_device_caps(void);

// The name of our application.  Used for window and MessageBox titles and error reporting
const char *g_app_name="Textured Quad";

// Our screen/window sizes and bit depth.  A better app would allow the user to choose the
// sizes.  I'll do that in a later tutorial, for now this is good enough.
const int g_width=640;
const int g_height=480;
const int g_depth=16; //16-bit colour

// Our global flag to track whether we should quit or not.  When it becomes true, we clean
// up and exit.
bool g_app_done=false;

// Our main Direct3D interface, it doesn't do much on its own, but all the more commonly
// used interfaces are created by it.  It's the first D3D object you create, and the last
// one you release.
IDirect3D9 *g_D3D=NULL;

// The D3DDevice is your main rendering interface.  It represents the display and all of its
// capabilities.  When you create, modify, or render any type of resource, you will likely
// do it through this interface.
IDirect3DDevice9 *g_d3d_device=NULL;

//Our presentation parameters.  They get set in our call to dhInitDevice, and we need them
//in case we need to reset our application.
D3DPRESENT_PARAMETERS g_pp;

struct textured_vertex{
    float x, y, z, rhw;  // The transformed(screen space) position for the vertex.
    float tu,tv;         // Texture coordinates
};

//Transformed vertex with 1 set of texture coordinates
const DWORD tri_fvf=D3DFVF_XYZRHW|D3DFVF_TEX1;

IDirect3DVertexBuffer9 *g_list_vb=NULL;
int g_list_count;

IDirect3DTexture9 *g_texture=NULL;

//******************************************************************************************
// Function:WinMain
// Whazzit:The entry point of our application
//******************************************************************************************
int APIENTRY WinMain(HINSTANCE ,HINSTANCE ,LPSTR ,int ){
bool fullscreen;
HWND window=NULL;
D3DFORMAT format;
HRESULT hr;
dhUserPrefs user_prefs(g_app_name);

   dhLogErase();  //Erase the log file to start fresh
   
   // Prompt the user for their preferences
   if (!user_prefs.QueryUser()) {
      dhLog("Exiting\n");
      return 0;
   }

   fullscreen = user_prefs.GetFullscreen();


   // Build our window.
   hr=dhInitWindow(fullscreen,g_app_name,g_width,g_height,default_window_proc,&window);
   if(FAILED(hr)){
      dhLog("Failed to create Window",hr);
      return 0;
   }

   //Build the D3D object
   hr=dhInitD3D(&g_D3D);
   if(FAILED(hr)){
      dhKillWindow(&window);
      dhLog("Failed to create D3D",hr);
      return 0;
   }

   //Find a good display/pixel format
   hr=dhGetFormat(g_D3D,fullscreen,g_depth,&format);
   if(FAILED(hr)){
      dhKillWindow(&window);
      dhLog("Failed to get a display format",hr);
      return 0;
   }

   DWORD adapter = user_prefs.GetAdapter();
   D3DDEVTYPE dev_type = user_prefs.GetDeviceType();

   //Initialize our PresentParameters
   dhInitPresentParameters(fullscreen,window,g_width,g_height,format,D3DFMT_UNKNOWN,&g_pp);

   //Create our device
   hr=dhInitDevice(g_D3D,adapter,dev_type,window,&g_pp,&g_d3d_device);
   if(FAILED(hr)){
      dhKillD3D(&g_D3D,&g_d3d_device);
      dhKillWindow(&window);
      dhLog("Failed to create the device",hr);
      return 0;
   }

   check_device_caps();

   //One-time preparation of objects and other stuff required for rendering
   if(FAILED(init_scene())){
      dhLog("Error initializing scene, exiting");
      kill_scene();
      dhKillD3D(&g_D3D,&g_d3d_device);
      dhKillWindow(&window);
      return 0;
   }


   //Loop until the user aborts (closes the window,presses the left mouse button or hits a key)
   while(!g_app_done){
      
      dhMessagePump();   //Check for window messages

      hr=g_d3d_device->TestCooperativeLevel();

      if(SUCCEEDED(hr)){
         hr=render();   //Draw our incredibly cool graphics
      }

      //Our device is lost
      if(hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICENOTRESET){

         dhHandleLostDevice(g_d3d_device,&g_pp,hr);

      }else if(FAILED(hr)){ //Any other error
         g_app_done=true;
         dhLog("Error rendering",hr);
      }

   }

   //Free all of our objects and other resources
   kill_scene();

   //Clean up all of our Direct3D objects
   dhKillD3D(&g_D3D,&g_d3d_device);

   //Close down our window
   dhKillWindow(&window);

   //Exit happily
   return 0;
}
//******************************************************************************************
// Function:check_device_caps
// Whazzit:Tests for various device capabilities.  If your application depended on any of
//         these capabilities you'd want to take additional action based on your findings.
//         For this sample, we'll just log our findings.
//******************************************************************************************
void check_device_caps(void){
D3DCAPS9 caps;
CString buffer;
int max_tex_size;

   g_d3d_device->GetDeviceCaps(&caps);

   max_tex_size=caps.MaxTextureWidth;
   
   buffer.Format("Maximum texture size is:%d",max_tex_size);
   dhLog(buffer);

   if(caps.TextureCaps&D3DPTEXTURECAPS_SQUAREONLY){
      dhLog("Textures have to be square");
   }else{
      dhLog("Textures do not have to be square");
   }

   if(caps.TextureCaps&D3DPTEXTURECAPS_POW2){
      dhLog("Textures must be a power of 2 in size.");

      if(caps.TextureCaps&D3DPTEXTURECAPS_NONPOW2CONDITIONAL){
         dhLog("But, in certain cases textures can ignore the power of 2 limitation.");
      }
   }else{
      dhLog("Textures do not need to be a power of 2 in size.");
   }

}

//******************************************************************************************
// Function:InitVolatileResources
// Whazzit:Prepare any objects that will not survive a device Reset.  These are initialized
//         separately so they can easily be recreated when we Reset our device.
//******************************************************************************************
void InitVolatileResources(void){

   set_device_states();

}
//******************************************************************************************
// Function:FreeVolatileResources
// Whazzit:Free any of our resources that need to be freed so that we can Reset our device,
//         also used to free these resources at the end of the program run.
//******************************************************************************************
void FreeVolatileResources(void){

   //This sample has no resources that need to be freed here.

}
//******************************************************************************************
// Function:init_scene
// Whazzit:Prepare any objects required for rendering.
//******************************************************************************************
HRESULT init_scene(void){
HRESULT hr=D3D_OK;


   hr=init_vb();
   if(FAILED(hr)){
      return hr;
   }

   hr=load_texture();
   if(FAILED(hr)){
      return hr;
   }
   
   InitVolatileResources();

   return hr;

}
//******************************************************************************************
// Function:load_texture
// Whazzit:Loads our texture
//******************************************************************************************
HRESULT load_texture(void){
HRESULT hr=D3D_OK;

   hr=D3DXCreateTextureFromFile(g_d3d_device,   //Direct3D Device
                                "DH.png",       //File Name
                                &g_texture);    //Texture handle

   if(FAILED(hr)){
      dhLog("Error loading texture",hr);
   }

   return hr;
}
//******************************************************************************************
// Function:set_device_states
// Whazzit:Sets up required RenderStates, TextureStageStates and SamplerStates
//******************************************************************************************
void set_device_states(void){

   g_d3d_device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
   g_d3d_device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
   g_d3d_device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);   //Ignored

   g_d3d_device->SetTexture(0,g_texture);

}
//******************************************************************************************
// Function:kill_scene
// Whazzit:Clean up any objects we required for rendering.
//******************************************************************************************
void kill_scene(void){

   if(g_texture){
      g_texture->Release();
      g_texture=NULL;
   }

   if(g_list_vb){
      g_list_vb->Release();
      g_list_vb=NULL;
   }
}

//******************************************************************************************
// Function: render
// Whazzit:Clears the screen, renders our scene and then presents the results.
//******************************************************************************************
HRESULT render(void){
HRESULT hr;

   //Clear the buffer to our new colour.
   hr=g_d3d_device->Clear(0,  //Number of rectangles to clear, we're clearing everything so set it to 0
                          NULL, //Pointer to the rectangles to clear, NULL to clear whole display
                          D3DCLEAR_TARGET,   //What to clear.  We don't have a Z Buffer or Stencil Buffer
                          0x00000000, //Colour to clear to (AARRGGBB)
                          1.0f,  //Value to clear ZBuffer to, doesn't matter since we don't have one
                          0 );   //Stencil clear value, again, we don't have one, this value doesn't matter
   if(FAILED(hr)){
      return hr;
   }

   //Notify the device that we're ready to render
   hr=g_d3d_device->BeginScene();
   if(FAILED(hr)){
      return hr;
   }


   g_d3d_device->SetFVF(tri_fvf);

   //Bind our Vertex Buffer
   g_d3d_device->SetStreamSource(0,                   //StreamNumber
                                 g_list_vb,           //StreamData
                                 0,                   //OffsetInBytes
                                 sizeof(textured_vertex)); //Stride

   //Render from our Vertex Buffer
   g_d3d_device->DrawPrimitive(D3DPT_TRIANGLELIST, //PrimitiveType
                               0,                  //StartVertex
                               g_list_count);      //PrimitiveCount

   
   //Notify the device that we're finished rendering for this frame
   g_d3d_device->EndScene();

   //Show the results
   hr=g_d3d_device->Present(NULL,  //Source rectangle to display, NULL for all of it
                            NULL,  //Destination rectangle, NULL to fill whole display
                            NULL,  //Target window, if NULL uses device window set in CreateDevice
                            NULL );//Unused parameter, set it to NULL

   return hr;
}
//******************************************************************************************
// Function:init_vb
// Whazzit:Create and fill our Vertex Buffer
//******************************************************************************************
HRESULT init_vb(void){
textured_vertex data[]={

   {150,400,1,1,0,1},{150,100,1,1,0,0},{450,100,1,1,1,0},
   {150,400,1,1,0,1},{450,100,1,1,1,0},{450,400,1,1,1,1}

};
int vert_count=sizeof(data)/sizeof(textured_vertex);
int byte_count=vert_count*sizeof(textured_vertex);
void *vb_vertices;
HRESULT hr;

   g_list_count=vert_count/3;

   hr=g_d3d_device->CreateVertexBuffer(byte_count,        //Length
                                       D3DUSAGE_WRITEONLY,//Usage
                                       tri_fvf,           //FVF
                                       D3DPOOL_MANAGED,   //Pool
                                       &g_list_vb,        //ppVertexBuffer
                                       NULL);             //Handle
   if(FAILED(hr)){
      dhLog("Error Creating vertex buffer",hr);
      return hr;
   }

   hr=g_list_vb->Lock(0, //Offset
                      0, //SizeToLock
                      &vb_vertices, //Vertices
                      0);  //Flags
   if(FAILED(hr)){
      dhLog("Error Locking vertex buffer",hr);
      return hr;
   }

   memcpy( vb_vertices, data,byte_count);

   g_list_vb->Unlock();


   return D3D_OK;
}

//******************************************************************************************
// Function:default_window_proc
// Whazzit:This handles any incoming Windows messages and sends any that aren't handled to
//         DefWindowProc for Windows to handle.
//******************************************************************************************
LRESULT CALLBACK default_window_proc(HWND p_hwnd,UINT p_msg,WPARAM p_wparam,LPARAM p_lparam){
   
   switch(p_msg){
      case WM_KEYDOWN:
         if(p_wparam == VK_ESCAPE){ //User hit Escape, end the app
            g_app_done=true;
         }
         return 0;
      case WM_CLOSE:    //User hit the Close Window button, end the app
      case WM_LBUTTONDOWN: //user hit the left mouse button
         g_app_done=true;
         return 0;
   }
   
   return (DefWindowProc(p_hwnd,p_msg,p_wparam,p_lparam));
   
}