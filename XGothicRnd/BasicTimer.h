#pragma once

#include <wrl.h>
#include <exception>

const int NUM_FRAMES_TO_MIDDLE = 256;

class BasicTimer
{
private:
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_currentTime;
	LARGE_INTEGER m_startTime;
	LARGE_INTEGER m_lastTime;
	float m_total;
	float m_delta[NUM_FRAMES_TO_MIDDLE];
	float m_deltaAvg;
	float m_deltaAccurate;
	int currentMidFrame;

public:
	BasicTimer()
	{
		if (!QueryPerformanceFrequency(&m_frequency))
		{
			throw std::exception();
		}

		currentMidFrame = 0;

		Reset();
	}

	void Reset()
	{

		m_total = 0;

		for(int i=0; i<NUM_FRAMES_TO_MIDDLE; i++)
		{
			m_delta[i] = 1.0f / 60.0f;
			m_deltaAccurate = 1.0f / 60.0f;
		}

		for(int i=0; i<NUM_FRAMES_TO_MIDDLE; i++)
		{
			Update();
		}

		m_startTime = m_currentTime;
	}

	void Update()
	{
		currentMidFrame ++;
		if(currentMidFrame == NUM_FRAMES_TO_MIDDLE )
		{
			currentMidFrame = 0;
		}


		if (!QueryPerformanceCounter(&m_currentTime))
		{
			throw std::exception();
		}

		m_total = static_cast<float>(
			static_cast<double>(m_currentTime.QuadPart-m_startTime.QuadPart) /
			static_cast<double>(m_frequency.QuadPart)
			);

		if (m_lastTime.QuadPart==m_startTime.QuadPart)
		{
			// If the timer was just reset, report a time delta equivalent to 60Hz frame time.
			m_delta[currentMidFrame] = 1.0f / 60.0f;
			m_deltaAccurate = 1.0f / 60.0f;
		}
		else
		{
			float d = static_cast<float>(
				static_cast<double>(m_currentTime.QuadPart-m_lastTime.QuadPart) /
				static_cast<double>(m_frequency.QuadPart)
				);


			//for(int i=0; i<NUM_FRAMES_TO_MIDDLE; i++)
			m_delta[currentMidFrame] = d;
			m_deltaAccurate = d;
			
		}

		m_deltaAvg = 0;
		for(int i=0; i<NUM_FRAMES_TO_MIDDLE; i++)
			m_deltaAvg += m_delta[i];

		m_deltaAvg /= (float)NUM_FRAMES_TO_MIDDLE;

		m_lastTime = m_currentTime;
	}



	float GetTotal() 
	{  
		return m_total;
	}

	void SetTotal(float t) 
	{  
		m_total=t;
	}

	float GetDelta() 
	{ 
		//return m_delta[0];


		return m_deltaAvg;
	}

	float GetDeltaAccurate()
	{
		return m_deltaAccurate;
	}

};
