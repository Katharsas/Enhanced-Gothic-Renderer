#pragma once
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include "Types.h"

#include <Windows.h>

/** Misc. tools */
struct zTBBox3D;
enum zTCam_ClipType;
namespace Toolbox
{
	/** Prints the current callstack */
	void PrintStack();

	/** Checks if one of a series of strings is found within the input-string */
	bool StringContainsOneOf(const std::string& string, const std::string* checkStrings, int numStrings);

	/** Hashes the data of the given type */
	template<typename T> size_t HashObject(const T& object)
	{
		byte* data = (byte *)&object;
		size_t hash = 0;
		for(int i=0;i<sizeof(T);i+=4)
		{
			DWORD d;
			((char *)&d)[0] = data[i];
			((char *)&d)[1] = data[i+1];
			((char *)&d)[2] = data[i+2];
			((char *)&d)[3] = data[i+3];

			hash_combine(hash, d);
		}

		return hash;
	}

	/** Erases an element by value from a vector */
	template<typename T> void EraseByElement(std::vector<T>& vector, T value)
	{
		auto it = std::find(vector.begin(), vector.end(), value);

		if(it != vector.end())
			vector.erase(it);
	}

	/** Erases an element by value from a vector */
	template<typename T, typename S> void EraseByElement(std::map<T, S>& map, S value)
	{
		for(auto it = map.begin();it!=map.end();it++)
		{
			if((*it).second == value)
				it = map.erase(it);
		}
	}

	/** Deletes all elements of the given std::vector */
	template<typename T> void DeleteElements(std::vector<T>& vector)
	{
		for(auto it = vector.begin(); it != vector.end(); it++)
		{
			delete (*it);
		}

		vector.clear();
	}

	/** Deletes all elements of the given std::list */
	template<typename T> void DeleteElements(std::list<T>& list)
	{
		for(auto it = list.begin(); it != list.end(); it++)
		{
			delete (*it);
		}

		list.clear();
	}

	/** Deletes all elements of the given std::set */
	template<typename T> void DeleteElements(std::set<T>& set)
	{
		for(auto it = set.begin(); it != set.end(); it++)
		{
			delete (*it);
		}

		set.clear();
	}

	/** Deletes all (second) elements of the given std::map */
	template<typename T, typename S> void DeleteElements(std::map<T, S>& map)
	{
		for(auto it = map.begin(); it != map.end(); it++)
		{
			delete (*it).second;
		}

		map.clear();
	}

	/** Deletes all (second) elements of the given std::unordered_map */
	template<typename T, typename S> void DeleteElements(std::unordered_map<T, S>& map)
	{
		for(auto it = map.begin(); it != map.end(); it++)
		{
			delete (*it).second;
		}

		map.clear();
	}

	/** Sorts the vector and removes all doubles */
	template<typename T> void RemoveDoubles(std::vector<T>& vector)
	{
		std::sort(vector.begin(),vector.end());
		std::unique(vector.begin(),vector.end());
	}

	/** Sums all bytes of the given data and packs it into a byte*/
	template<typename T> byte SumBytes(const T& data)
	{
		byte s = 0;
		for(int i=0;i<sizeof(T);i++)
		{
			s += ((byte *)&data)[i];
		}

		return s;
	}

	byte SumBytes(const unsigned char* data, unsigned int numBytes);

	/** Checks whether a given boundingbox is inside the given frustum. The index in "cache" is tested first, if it isn't set to -1 */
	zTCam_ClipType BBox3DInFrustumCached(const zTBBox3D& bbox3D, Plane* frustumPlanes, byte* signbits, int& cache);

	/** Checks if a folder exists */
	bool FolderExists(const std::string& dirName_in);

	/** Hashes the given float value */
	void hash_combine(std::size_t& seed, float value);

	/** Hashes the given DWORD value */
	void hash_combine(std::size_t& seed, DWORD value);

	/** Returns true if the given position is inside the box */
	bool PositionInsideBox(const Vector3& p, const Vector3& min, const Vector3& max);

	/** Converts an errorcode into a string */
	std::string MakeErrorString(XRESULT code);

	/** Returns the number of bits inside a bitmask */
	WORD GetNumberOfBits( DWORD dwMask );

	/** Returns the size of a DDS-Image in bytes */
	unsigned int GetDDSStorageRequirements(unsigned int width, unsigned int height, bool dxt1);

	/** Returns the RowPitch-Size of a DDS-Image */
	unsigned int GetDDSRowPitchSize(unsigned int width, bool dxt1);

	/** Returns a random number between 0 and 1 */
	float frand();

	/** Linear interpolation */
	float lerp(float a, float b, float w);

	/** Converts a multi-byte-string to wide-char */
	std::wstring ToWideChar(const std::string& str);
	
	/** Converts a wide-char-string to  multi-byte*/
	std::string ToMultiByte(const std::wstring& str);

	/** Returns whether two AABBs are intersecting or not */
	bool AABBsOverlapping(const Vector3& minA, const Vector3& maxA, const Vector3& minB, const Vector3& maxB); 

	/** Does a ray vs aabb test */
	bool IntersectBox(const Vector3& min, const Vector3& max, const Vector3& origin, const Vector3& direction, float& t);

	/** Does a ray vs aabb test */
	bool IntersectTri(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& origin, const Vector3& direction, float& u, float& v, float& t);

	/** Computes the normal of a triangle */
	Vector3 ComputeNormal(const Vector3& v0, const Vector3& v1, const Vector3& v2);

	/** Computes the distance of a point to an AABB */
	float ComputePointAABBDistance(const Vector3& p, const Vector3& min, const Vector3& max);

	/** Returns whether the given file exists */
	bool FileExists(const std::string& file);

	/** Saves a std::string to a FILE* */
	void SaveStringToFILE(FILE* f, const std::string& str);

	/** sse2 memcpy implementation by William Chan and Google */
	void X_aligned_memcpy_sse2(void* dest, const void* src, const unsigned long size_t);

	/** Loads a std::string from a FILE* */
	std::string LoadStringFromFILE(FILE* f);
};