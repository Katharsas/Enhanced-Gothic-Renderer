#include "pch.h"
#include "GCamera.h"
#include "zCCamera.h"

GCamera::GCamera(zCCamera* sourceObject) : GzObjectExtension<zCCamera, GCamera>(sourceObject)
{
}


GCamera::~GCamera(void)
{
}

/** Returns the currently active camera */
GCamera* GCamera::GetActiveCamera()
{
	return GCamera::GetFromSource(zCCamera::GetActiveCamera());
}

/** Returns the position of this camera */
const float3& GCamera::GetCameraPosition()
{
	return m_SourceObject->GetCameraWorldMatrix().TranslationT();
}

/** Returns the direction of this camera */
const float3& GCamera::GetDirection()
{
	return m_SourceObject->GetCameraDirection();
}

/** Returns the near and farplanes */
float GCamera::GetFarZ()
{
	return m_SourceObject->GetFarPlane();
}

float GCamera::GetNearZ()
{
	return m_SourceObject->GetNearPlane();
}