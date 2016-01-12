#pragma once
#include "pch.h"

class oCGame
{
public:

	/** Returns the current game-session */
	static oCGame* GetGame() { return *(oCGame**)MemoryLocations::Gothic::oCGame_p_ogame; };

	/** Returns the currently loaded world */
	class zCWorld* GetWorld(){return zCSession_world;}

	/** Returns the current main camera */
	class zCCamera* GetSessionCamera(){return zCSession_camera;}

	/** Returns the current session view */
	class zCView* GetSessionView() { return zCSession_viewport; }
private:
	int _vtbl;
    class zCCSManager* zCSession_csMan;     
    class zCWorld* zCSession_world;
    class zCCamera* zCSession_camera;
    class zCAICamera* zCSession_aiCam;
    class zCVob* zCSession_camVob; 
    class zCView* zCSession_viewport;



};