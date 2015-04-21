//
// dhD3D.cpp - Part of the Drunken Hyena Common Library for DirectX9
//
// Copyright 2007 by Ken Paulson
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the Drunken Hyena License.  If a copy of the license was
// not included with this software, you may get a copy from:
// http://www.drunkenhyena.com/docs/DHLicense.txt
//
#include "dhD3D.h"
//The following 2 includes are only needed for SetupDirect3D
#include "dhUtility.h"
#include "dhWindow.h"

//******************************************************************************************
// Function:dhInitD3D
// Whazzit:Initializes the D3D object
//******************************************************************************************
HRESULT dhInitD3D(IDirect3D9 **p_d3d)
{
	
	//Create Direct3D9, this is the first thing you have to do in any D3D9 program
	//Always pass D3D_SDK_VERSION to the function.
	(*p_d3d) = Direct3DCreate9( D3D_SDK_VERSION);
	if(!(*p_d3d))
	{
		return E_FAIL;
	}

	return D3D_OK;
}

//******************************************************************************************
// Function:dhInitPresentParameters
// Whazzit:Initializes PresentParameters so they can be passed to CreateDevice
//******************************************************************************************
void dhInitPresentParameters(bool p_fullscreen,HWND p_window,DWORD p_width,DWORD p_height,D3DFORMAT p_format,D3DFORMAT p_depth,D3DPRESENT_PARAMETERS *p_pp)
{

	//Clear out our D3DPRESENT_PARAMETERS structure.  Even though we're going
	//to set virtually all of its members, it's good practice to zero it out first.
	ZeroMemory(p_pp,sizeof(D3DPRESENT_PARAMETERS));
	
	//Whether we're full-screen or windowed these are the same.
	p_pp->BackBufferCount	= 1;  //We only need a single back buffer
	p_pp->MultiSampleType	= D3DMULTISAMPLE_NONE; //No multi-sampling
	p_pp->MultiSampleQuality	= 0; //No multi-sampling
	p_pp->SwapEffect			= D3DSWAPEFFECT_DISCARD; // Throw away previous frames, we don't need them
	p_pp->hDeviceWindow		= p_window;  //This is our main (and only) window
	p_pp->Flags				= 0;  //No flags to set
	p_pp->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; //Default Refresh Rate
	p_pp->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; //Default Presentation rate
	p_pp->BackBufferFormat  = p_format; //Display format

	if(p_depth==D3DFMT_UNKNOWN)
	{
		p_pp->EnableAutoDepthStencil = FALSE;
	}
	else
	{
		p_pp->EnableAutoDepthStencil = TRUE;
	}

	p_pp->AutoDepthStencilFormat = p_depth; //This is ignored if EnableAutoDepthStencil is FALSE

	//BackBufferWidth/Height have to be set for full-screen applications, these values are
	//used (along with BackBufferFormat) to determine the display mode.
	//They aren't needed in windowed mode since the size of the window will be used.
	if(p_fullscreen)
	{
		p_pp->Windowed				= FALSE;
		p_pp->BackBufferWidth	= p_width;
		p_pp->BackBufferHeight  = p_height;
	}
	else
	{
		p_pp->Windowed = TRUE;
	}

}

//******************************************************************************************
// Function:dhInitDevice
// Whazzit:Creates the Direct3D device.
//******************************************************************************************
HRESULT dhInitDevice(IDirect3D9 *p_d3d,DWORD p_adapter,D3DDEVTYPE p_dev_type,HWND p_window,D3DPRESENT_PARAMETERS *p_pp,IDirect3DDevice9 **p_device)
{
HRESULT hr = S_OK;

	//After filling in our D3DPRESENT_PARAMETERS structure, we're ready to create our device.
	//Most of the options in how the device is created are set in the D3DPRESENT_PARAMETERS
	//structure.
	hr=p_d3d->CreateDevice(p_adapter, //User specified adapter, on a multi-monitor system
												 //there can be more than one.
								//User specified device type, Usually HAL
								p_dev_type,
								//Our Window
								p_window,
								//Process vertices in software. This is slower than in hardware,
								//But will work on all graphics cards.
								D3DCREATE_SOFTWARE_VERTEXPROCESSING,
								//Our D3DPRESENT_PARAMETERS structure, so it knows what we want to build
								p_pp,
								//This will be set to point to the new device
								p_device);
	

	return hr;
}



//******************************************************************************************
// Function:dhKillD3D
// Whazzit:Releases all of our D3D resources in the opposite order from their creation.
//******************************************************************************************
void dhKillD3D(IDirect3D9 **p_d3d,IDirect3DDevice9 **p_device)
{
	
	if(*p_device)
	{
		(*p_device)->Release();
		(*p_device) = NULL;
	}
	
	if(*p_d3d)
	{
		(*p_d3d)->Release();
		(*p_d3d) = NULL;
	}
	
}
//******************************************************************************************
// Function:SetupDirect3D
// Whazzit: A quickie setup function that does all the prep work for us
//******************************************************************************************
HRESULT SetupDirect3D(const char *p_app_name,DWORD p_width,DWORD p_height,WNDPROC p_wndproc,
							 UCHAR p_depth,bool p_use_z,
							 HWND *p_window,IDirect3D9 **p_d3d,IDirect3DDevice9 **p_device,
							 D3DPRESENT_PARAMETERS *p_pp)
{
HRESULT hr = D3D_OK;
bool fullscreen;
D3DFORMAT format;
D3DFORMAT depth_format = D3DFMT_UNKNOWN;

	(*p_window) = NULL;
	(*p_d3d) = NULL;
	(*p_device) = NULL;

	// Prompt the user, Full Screen?  Windowed?  Cancel?
	fullscreen=dhAskFullscreen(p_app_name);


	// Build our window.
	hr=dhInitWindow(fullscreen,p_app_name,p_width,p_height,p_wndproc,p_window);
	if(FAILED(hr))
	{
		return hr;
	}

	//Build the D3D object
	hr=dhInitD3D(p_d3d);
	if(FAILED(hr))
	{
		dhKillWindow(p_window);
		return hr;
	}



	hr=dhGetFormat((*p_d3d),fullscreen,p_depth,&format);
	if(FAILED(hr))
	{
		dhKillD3D(p_d3d,p_device);
		dhKillWindow(p_window);
		return hr;
	}

	if(p_use_z)
	{
		if(p_depth == 16)
		{
			depth_format = D3DFMT_D16;
		}
		else if(p_depth == 32)
		{
			depth_format = D3DFMT_D32;
		}
		else
		{
			return E_FAIL;
		}
	}

	dhInitPresentParameters(fullscreen,(*p_window),p_width,p_height,format,depth_format,p_pp);


	hr=dhInitDevice((*p_d3d),0,D3DDEVTYPE_HAL,(*p_window),p_pp,p_device);
	if(FAILED(hr))
	{
		dhKillD3D(p_d3d,p_device);
		dhKillWindow(p_window);
		return hr;
	}

	return hr;

}