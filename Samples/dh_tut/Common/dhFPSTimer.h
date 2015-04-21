//
// vertex_buffers.cpp - Vertex Buffers
//
// Copyright 2004 by Ken Paulson
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the Drunken Hyena License.  If a copy of the license was
// not included with this software, you may get a copy from:
// http://www.drunkenhyena.com/docs/DHLicense.txt
//
#ifndef DHFPSTIMER_H
#define DHFPSTIMER_H

//
// Version: 2.1 - January 1, 2007
//
// Change Log:
// - VS2005 update - January 1, 2007
// - Now API independent - September 6, 2003
//

#include <atlstr.h>
#include <fstream>

extern "C"{
   unsigned long __stdcall timeGetTime(void);
}

#pragma comment(lib,"winmm.lib") //Required for timeGetTime


class dhFPSTimer{
protected:

   DWORD m_frames_one_second;
   DWORD m_frames;
   DWORD m_min_one_second;
   DWORD m_max_one_second;
   DWORD m_current_fps;
   DWORD m_start_time;
   DWORD m_one_second_start_time;

   CString m_app_name;

   CString m_fps_string_buffer;

public:
   inline void StartTimer(void);
   inline void LogTimer(void);
   inline void LogTimer(const char *p_file_name);
   inline void CheckTimer(float *p_fps,DWORD *p_min_1_sec,DWORD *p_max_1_sec);
   inline float GetFPS(void);
   inline float GetCurrentFPS(void);
   inline CString GetFPSString(void);

   inline void Flip(void);
   inline void Present(void);

   inline dhFPSTimer(const char *p_app_name);
   inline dhFPSTimer(void);
};

//********************************************************************
// Function:dhFPSTimer::dhFPSTimer
// Whazzit:Constructor.  The log file requires the app name, so pass
//         it in here.
//********************************************************************
inline dhFPSTimer::dhFPSTimer(const char *p_app_name):
	m_app_name(p_app_name)
{

}
//********************************************************************
// Function:dhFPSTimer::dhFPSTimer
// Whazzit:Constructor.  The log file requires the app name, so pass
//         it in here.
//********************************************************************
inline dhFPSTimer::dhFPSTimer(void):
	m_app_name("dhFPSTimer")
{

}
//********************************************************************
// Function:dhFPSTimer::StartTimer
// Whazzit:Initializes all variables & records current time.  You can
//         safely call this function multiple times, it will just
//         reset the timer.
//********************************************************************
inline void dhFPSTimer::StartTimer(void){

   m_frames_one_second = 0;
   m_frames = 0;
   m_min_one_second = 0x7fffffff;
   m_max_one_second = 0;

   m_start_time = timeGetTime();
   m_one_second_start_time = m_start_time;

}
//********************************************************************
// Function:dhFPSTimer::LogTimer
// Whazzit:Calls LogTimer with the default file name.
//********************************************************************
inline void dhFPSTimer::LogTimer(void){

   LogTimer("FPS.txt");

}
//********************************************************************
// Function:dhFPSTimer::LogTimer
// Whazzit:Writes a GameGauge 2.0 FPS log.
//********************************************************************
inline void dhFPSTimer::LogTimer(const char *p_file_name){
float fps;
std::ofstream fps_file;
CString buffer;

   fps=GetFPS();

	fps_file.open(p_file_name,std::ios::trunc);
   if(fps_file.is_open())
	{
		buffer.Format("%.2f %s\n%d min\n%d max\n",fps,m_app_name,m_min_one_second,m_max_one_second);

		fps_file << (LPCTSTR)buffer;

      fps_file.close();
   }
}
//********************************************************************
// Function:dhFPSTimer::CheckTimer
// Whazzit:Fills parameters with FPS timings.  This is useful if you'd
//         like to display the FPS on the screen rather than (or in
//         addition to) the log file.
//********************************************************************
inline void dhFPSTimer::CheckTimer(float *p_fps,DWORD *p_min_1_sec,DWORD *p_max_1_sec){

   *p_fps=GetFPS();
   *p_min_1_sec=m_min_one_second;
   *p_max_1_sec=m_max_one_second;

}
//********************************************************************
// Function:dhFPSTimer::GetFPS
// Whazzit:Returns average FPS.
//********************************************************************
inline float dhFPSTimer::GetFPS(void){
DWORD end_time=0,total_time=0;
float fps;

   end_time=timeGetTime();
   total_time=end_time-m_start_time;
   fps=((float)m_frames/(float)total_time)*1000.0f;

   return fps;
}
//********************************************************************
// Function:dhFPSTimer::GetCurrentFPS
// Whazzit:Returns FPS for the current second.  Since framerates can
//         vary significantly from second to second, this value will
//         tend to jump around
//********************************************************************
inline float dhFPSTimer::GetCurrentFPS(void){

   return (float)m_current_fps;
}
//********************************************************************
// Function:dhFPSTimer::GetFPSString
// Whazzit:Returns a pointer to a pre-formatted FPS string.
//********************************************************************
inline CString dhFPSTimer::GetFPSString(void){

   m_fps_string_buffer.Format("FPS:%.2f",GetFPS());

   return m_fps_string_buffer;
}
//********************************************************************
// Function:dhFPSTimer::Present
// Whazzit:Alias for the Flip call.  Use whichever one you like.
//********************************************************************
inline void dhFPSTimer::Present(void){

   Flip();
}
//********************************************************************
// Function:dhFPSTimer::Flip
// Whazzit:Increment the frame counters.
//********************************************************************
inline void dhFPSTimer::Flip(void){


   m_frames_one_second++;
   m_frames++;


   if((timeGetTime() - m_one_second_start_time) > 1000){
      if(m_frames_one_second < m_min_one_second)
         m_min_one_second=m_frames_one_second;
      if(m_frames_one_second > m_max_one_second)
         m_max_one_second=m_frames_one_second;

      m_current_fps=m_frames_one_second;
      m_frames_one_second=0;

      m_one_second_start_time = timeGetTime();
   }

};



#endif //#ifndef DHFPSTIMER_H
