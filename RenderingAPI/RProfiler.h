//=================================================================================================
//
//	Query Profiling Sample
//  by MJP
//  http://mynameismjp.wordpress.com/
//
//  All code and content licensed under Microsoft Public License (Ms-PL)
//
//=================================================================================================

#pragma once

#include "pch.h"
#include "RTimer.h"

#ifdef RND_D3D11

class RProfiler
{

public:

	// Constants
	static const UINT64 QueryLatency = 5;

    static RProfiler GlobalProfiler;

	struct ProfileData
	{
		ID3D11Query* DisjointQuery[QueryLatency];
		ID3D11Query* TimestampStartQuery[QueryLatency];
		ID3D11Query* TimestampEndQuery[QueryLatency];
		BOOL QueryStarted;
		BOOL QueryFinished;

		double GPUDelta[QueryLatency];
		double CPUStart[QueryLatency];
		double CPUEnd[QueryLatency];

		ProfileData() : QueryStarted(FALSE), QueryFinished(FALSE) {}
	};


    void Initialize(ID3D11Device* device, ID3D11DeviceContext* immContext);

    void StartProfile(const std::string& name);
    void EndProfile(const std::string& name);

    void EndFrame(std::string& output);

	const ProfileData& GetProfileData(const std::string& name)
	{
		return profiles[name];
	}

protected:

    typedef std::map<std::string, ProfileData> ProfileMap;

    ProfileMap profiles;
    UINT64 currFrame;

    ID3D11Device* device;
    ID3D11DeviceContext* context;

    RTimer timer;
};

class RProfileBlock
{
public:

	RProfileBlock(const std::string& name);
    ~RProfileBlock();

	

protected:

    std::string name;
};

#endif