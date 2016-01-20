#pragma once
#include <iostream>
#include <Windows.h>
#include <sstream>
#include <vector>
#include <string>
#include "Toolbox.h"
#include <mutex>

//#include <DxErr.h>
//#pragma comment(lib, "Dxerr.lib")
#define USE_LOG

const int MAX_LOG_MESSAGES_TO_CACHE = 200;
__declspec( selectany ) std::string LOGFILE;

//#ifdef BUILD_DESKTOP
#ifdef BLERGH__
//#if defined(DEBUG) || defined(_DEBUG)
/** Checks for errors and logs them, HRESULT hr needs to be declared */
#define LE(x) { hr = (x); if(FAILED(hr)){LogError() << L#x << L" failed: " << DXGetErrorDescription(hr); }/*else{ LogInfo() << L#x << L" Succeeded."; }*/ }

/** Returns hr if failed (HRESULT-function, hr needs to be declared)*/
#define LE_R(x) { hr = (x); if(FAILED(hr)){LogError() << L#x << L" failed: " << DXGetErrorDescription(hr); return hr;} }

/** Returns nothing if failed (void-function)*/
#define LE_RV(x) { hr = (x); if(FAILED(hr)){LogError() << L#x << L" failed: " << DXGetErrorDescription(hr); return;} }

/** Returns false if failed (bool-function) */
#define LE_RB(x) { hr = (x); if(FAILED(hr)){LogError() << L#x << L" failed: " << DXGetErrorDescription(hr); return false;} }

/** throws an exceptopn if failed (HRESULT-function) */
#define LE_THROW(x) { hr = (x); if(FAILED(hr)){LogError() << L#x << L" failed: " << DXGetErrorDescription(hr); UT::ThrowIfFailed(hr);} }

#define ErrorBox(Msg) MessageBox(NULL,Msg,L"Error!",MB_OK|MB_ICONERROR|MB_TOPMOST)
#define InfoBox(Msg) MessageBox(NULL,Msg,L"Info!",MB_OK|MB_ICONASTERISK|MB_TOPMOST)
#define WarnBox(Msg) MessageBox(NULL,Msg,L"Warning!",MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST)

#else

#define XLE(x) { XRESULT xr = (x); if(xr != XRESULT::XR_SUCCESS){ LogError() << ##x << " failed with code: " << xr << " (" + Toolbox::MakeErrorString(xr) + ")";}}

#define LEB(x) {if(!x){LogError() << #x " failed!";}}
#define LEB_R(x) {if(!x){LogError() << #x " failed!"; return false;}}

/** Checks for errors and logs them, HRESULT hr needs to be declared */
#define LE(x) { hr = (x); if(FAILED(hr)){LogError() << #x " failed with code: " << hr << "!"; }/*else{ LogInfo() << L#x << L" Succeeded."; }*/ }

/** Returns hr if failed (HRESULT-function, hr needs to be declared)*/
#define LE_R(x) { hr = (x); if(FAILED(hr)){LogError() << #x " failed with code: " << hr << "!"; return hr;} }

/** Returns nothing if failed (void-function)*/
#define LE_RV(x) { hr = (x); if(FAILED(hr)){LogError() << #x " failed with code: " << hr << "!"; return;} }

/** Returns false if failed (bool-function) */
#define LE_RB(x) { hr = (x); if(FAILED(hr)){LogError() << #x " failed with code: " << hr << "!"; return false;} }

#define ErrorBox(Msg) MessageBoxA(NULL,Msg,"Error!",MB_OK|MB_ICONERROR|MB_TOPMOST)
#define InfoBox(Msg) MessageBoxA(NULL,Msg,"Info!",MB_OK|MB_ICONASTERISK|MB_TOPMOST)
#define WarnBox(Msg) MessageBoxA(NULL,Msg,"Warning!",MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST)

#endif

/*#else
#define LE(x) { hr = (x); }
#endif
*/

/** Logging macros 
	Usage: LogInfo() << L"Loaded Texture: " << TextureName; 
	*/


#define LogInfo() Log("Info",__FILE__, __LINE__, __FUNCSIG__)
#define LogWarn() Log("Warning",__FILE__, __LINE__, __FUNCSIG__, true)
#define LogError() Log("Error",__FILE__, __LINE__, __FUNCSIG__, true)


/** Displays a messagebox and loggs its content */
#define LogInfoBox() Log("Info",__FILE__, __LINE__, __FUNCSIG__, false, 1)
#define LogWarnBox() Log("Warning",__FILE__, __LINE__, __FUNCSIG__, true, 2)
#define LogErrorBox() Log("Error",__FILE__, __LINE__, __FUNCSIG__, true, 3)

/** Stream logger */
#ifdef USE_LOG

#define STRINGX(x) #x
#define STRING(x) STRINGX(x)

// Builds a string that can be read by visual studio to jump directly to the sourcecode
#define VS_LOG(file, line, function, msg) (file"(" STRING(line) "): [" function "] ")

namespace LogCache
{
	__declspec(selectany) std::vector<std::string> Cache;
	__declspec(selectany) std::mutex LogMutex;

	struct LogFlush
	{
		~LogFlush()
		{
			LogMutex.lock();
			Cache.push_back("Flushed " +  std::to_string(Cache.size()) + " log messages at program termination!\n");
			FlushData();
			LogMutex.unlock();
		}

		static void FlushData()
		{
			FILE* f;
			fopen_s(&f,LOGFILE.c_str(),"a");

			if(!f)
				return;

			//if(MAX_LOG_MESSAGES_TO_CACHE > 5)
			//	fputs(" --- Log-Cache flush! --- \n", f);

			// Write down all the data we have
			for(UINT i=0;i<Cache.size();i++)
			{
				fputs(Cache[i].c_str(), f);

				//OutputDebugStringA(Cache[i].c_str());
			}

			fclose(f);

			// Clear the cache
			Cache.clear();
		}
	};

	__declspec(selectany) LogFlush fls; // This will be deleted by the CRT when the program ends, thus, calling the destructor, which flushes the remaining cache
}

class Log
{
public:
	Log(const char* Type,const  char* File, int Line,const  char* Function, bool bIncludeInfo=false, UINT MessageBox=0 )
	{
		if(bIncludeInfo)
		{
			// {0}({1}): <message here>
			Info << File << "(" << Line << "):" << "[" << Function << "] "; 
			//Info << Type << ": ["<< File << "("<<Line<<"), "<<Function<<"]: "; 
		}else
		{
			Info << Type << ": ";
		}

		MessageBoxStyle=MessageBox;
	}

	~Log()
	{
		Flush();
	}

	/** Clears the logfile */
	static void Clear()
	{
		char path[MAX_PATH + 1];
		GetModuleFileNameA(NULL, path, MAX_PATH);
		LOGFILE = std::string(path);
		LOGFILE = LOGFILE.substr(0, LOGFILE.find_last_of('\\')+1);
		LOGFILE += "Log.txt";

		FILE* f;
		f = fopen(LOGFILE.c_str(),"w");
		fclose(f);
	}

	/** STL stringstream feature */
	template< typename T >
	inline Log& operator << ( const T &obj )
	{
		Message << obj;
		return *this;
	}

	inline Log& operator << ( std::wostream& (*fn)(std::wostream&) )
	{
		Message << fn;
		return *this;
	}

	/** Called when the object is getting destroyed, which happens immediately if simply calling the constructor of this class */
	inline void Flush()
	{	
		FILE* f;
		f = fopen(LOGFILE.c_str(),"a");

		if(f)
		{
			
			fputs(Info.str().c_str(),f);

			fputs(Message.str().c_str(),f);
			fputs("\n",f);

			fclose(f);

			OutputDebugString((Info.str() + Message.str() + "\n").c_str());

			// Do callback
			if(LogCallback)
			{
				LogCallback(Message.str());
			}
		}

		/*if(strnicmp(Info.str().c_str(), "Error", sizeof("Error")) == 0)
		{
			LastErrorMessage = Info.str() + Message.str();
		}*/

		/*// Place the message into the cache
		LogCache::Cache.push_back(Info.str() + Message.str() + "\n");

		// Flush data if the cache is full
		if(LogCache::Cache.size() >= MAX_LOG_MESSAGES_TO_CACHE)
			LogCache::LogFlush::FlushData();*/

		switch(MessageBoxStyle)
		{
		case 1:
			InfoBox(Message.str().c_str());
			break;

		case 2:
			WarnBox(Message.str().c_str());
			break;

		case 3:
			ErrorBox(Message.str().c_str());
			break;
		}
	}

	/** Sets the function to be called when a log should be printed */
	static void SetLogCallback(std::function<void(const std::string&)> fn)
	{
		LogCallback = fn;
	}

private:

	static std::function<void(const std::string&)> LogCallback;

	std::stringstream Info; // Contains an information like "Info", "Warning" or "Error"
	std::stringstream Message; // Text to write into the logfile
	UINT MessageBoxStyle; // Style of the messagebox if needed

	//static std::string LastErrorMessage; // The last errormessage
};
#else

class Log
{
public:
	Log(const char* Type,const  char* File, int Line,const  char* Function, bool bIncludeInfo=false, UINT MessageBox=0 )
	{

	}

	~Log()
	{
	
	}

	/** Clears the logfile */
	static void Clear()
	{

	}

	/** STL stringstream feature */
	template< typename T >
	inline Log& operator << ( const T &obj )
	{
		return *this;
	}

	inline Log& operator << ( std::wostream& (*fn)(std::wostream&) )
	{
		return *this;
	}

	/** Called when the object is getting destroyed, which happens immediately if simply calling the constructor of this class */
	inline void Flush()
	{	

	}

private:

	static std::function<void(const std::string&)> LogCallback;
};

#endif

// Definition of the static log callback
__declspec(selectany) std::function<void(const std::string&)> Log::LogCallback;