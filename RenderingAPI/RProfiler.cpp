//=================================================================================================
//
//	Query Profiling Sample
//  by MJP
//  http://mynameismjp.wordpress.com/
//
//  All code and content licensed under Microsoft Public License (Ms-PL)
//
//=================================================================================================

#include "pch.h"
#include "RProfiler.h"
#include "../Shared/Logger.h"

using std::string;
using std::map;

// == Profiler ====================================================================================

RProfiler RProfiler::GlobalProfiler;

void RProfiler::Initialize(ID3D11Device* device, ID3D11DeviceContext* immContext)
{
    this->device = device;
    this->context = immContext;
}

void RProfiler::StartProfile(const string& name)
{
	HRESULT hr;

    ProfileData& profileData = profiles[name];
    _ASSERT(profileData.QueryStarted == FALSE);
    _ASSERT(profileData.QueryFinished == FALSE);
    
    if(profileData.DisjointQuery[currFrame] == NULL)
    {
        // Create the queries
        D3D11_QUERY_DESC desc;
        desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
        desc.MiscFlags = 0;
        LE(device->CreateQuery(&desc, &profileData.DisjointQuery[currFrame]));

        desc.Query = D3D11_QUERY_TIMESTAMP;
		LE(device->CreateQuery(&desc, &profileData.TimestampStartQuery[currFrame]));
		LE(device->CreateQuery(&desc, &profileData.TimestampEndQuery[currFrame]));

		profileData.CPUStart[currFrame] = 0.0;
		profileData.CPUEnd[currFrame] = 0.0;
		profileData.GPUDelta[currFrame] = 0.0;
    }

    // Start a disjoint query first
    context->Begin(profileData.DisjointQuery[currFrame]);

    // Insert the start timestamp    
    context->End(profileData.TimestampStartQuery[currFrame]);

    profileData.QueryStarted = TRUE;

	timer.Update();
	profileData.CPUStart[currFrame] = timer.GetTotal();

}

void RProfiler::EndProfile(const string& name)
{
    ProfileData& profileData = profiles[name];
    _ASSERT(profileData.QueryStarted == TRUE);
    _ASSERT(profileData.QueryFinished == FALSE);

	timer.Update();
	profileData.CPUEnd[currFrame] = timer.GetTotal();

    // Insert the end timestamp    
    context->End(profileData.TimestampEndQuery[currFrame]);

    // End the disjoint query
    context->End(profileData.DisjointQuery[currFrame]);

    profileData.QueryStarted = FALSE;
    profileData.QueryFinished = TRUE;
}

void RProfiler::EndFrame(std::string& output)
{
    currFrame = (currFrame + 1) % QueryLatency;    

    float queryTime = 0.0f;

    // Iterate over all of the profiles
    ProfileMap::iterator iter;
    for(iter = profiles.begin(); iter != profiles.end(); iter++)
    {
        ProfileData& profile = (*iter).second;
        if(profile.QueryFinished == FALSE)
            continue;

        profile.QueryFinished = FALSE;

        if(profile.DisjointQuery[currFrame] == NULL)
            continue;

        timer.Update();

        // Get the query data
        UINT64 startTime = 0;
        while(context->GetData(profile.TimestampStartQuery[currFrame], &startTime, sizeof(startTime), 0) != S_OK);

        UINT64 endTime = 0;
        while(context->GetData(profile.TimestampEndQuery[currFrame], &endTime, sizeof(endTime), 0) != S_OK);

        D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
        while(context->GetData(profile.DisjointQuery[currFrame], &disjointData, sizeof(disjointData), 0) != S_OK);

        timer.Update();
        queryTime += timer.GetDeltaAccurate();

		double time = 0.0f;
		if(disjointData.Disjoint == FALSE)
		{
			UINT64 delta = endTime - startTime;
			double frequency = static_cast<double>(disjointData.Frequency);
			time = (delta / frequency) * 1000.0f;

			profile.GPUDelta[currFrame] = time;
        }        

        output += (*iter).first + ": " + std::to_string(time) + "ms";
    }

    output += "Time spent waiting for queries: " + std::to_string(queryTime) + "ms";
}

// == ProfileBlock ================================================================================

RProfileBlock::RProfileBlock(const std::string& name) : name(name)
{
    RProfiler::GlobalProfiler.StartProfile(name);
}

RProfileBlock::~RProfileBlock()
{
    RProfiler::GlobalProfiler.EndProfile(name);
}