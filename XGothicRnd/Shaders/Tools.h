
/** Performs the rotation of a vector by a quaternion */
float3 RotateVectorByQuat(float4 quat, float3 vec)
{
	return vec + 2.0 * cross(cross(vec, quat.xyz) + quat.w * vec, quat.xyz);
}

float4 DWORDToFloat4(uint color)
{
	float a = (color >> 24) / 255.0f;
	float r = ((color >> 16) & 0xFF) / 255.0f;
	float g = ((color >> 8 ) & 0xFF) / 255.0f;
	float b = (color & 0xFF) / 255.0f;

	return float4(r,g,b,a);
}
