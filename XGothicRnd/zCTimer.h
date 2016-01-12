#pragma once
#include "zDefinitions.h"
#include "MemoryLocations.h"

class zCTimer {
public:
	float factorMotion;        //zREAL        //nicht zu klein machen. Sonst: Freeze bei hoher Framerate!
	float frameTimeFloat;      //zREAL [msec] //Zeit der zwischen diesem und dem letzten Frame verstrichen ist
	float totalTimeFloat;      //zREAL [msec] //gesamte Zeit
#ifndef BUILD_GOTHIC_1_08k
	float frameTimeFloatSecs;  //zREAL  [s]
	float totalTimeFloatSecs;  //zREAL  [s]
#endif
	unsigned int lastTimer;           //zDWORD
	unsigned int frameTime;           //zDWORD [msec] //nochmal als Ganzahl
	unsigned int totalTime;           //zDWORD [msec]
	unsigned int minFrameTime;        //zDWORD       //antifreeze. Sonst wird die Framezeit auf 0 gerundet und nichts bewegt sich
	unsigned int forcedMaxFrameTime;  //zDWORD //länger als das darf ein Frame (in Spielzeit) nicht dauern. Um zu große Zeitsprünge für die Objekte zu vermeiden? Jedenfalls sort dies dafür, dass das Spiel langsamer läuft, wenn das Spiel mit rendern nicht hinterherkommt.

	static zCTimer* GetTimer()
	{
		return (zCTimer *)MemoryLocations::Gothic::zCTimer_ztimer;
	}
};