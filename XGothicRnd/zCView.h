#pragma once
#include "zSTRING.h"
#include "oCGame.h"

class zCView
{
public:
	
	/** Prints the given line of text to the view */
	void Print(const INT2& position, const std::string& str)
	{
		// Make linebreaks
		std::string lines = str;
		int y = position.y;
		do
		{
			// Only take the single line
			std::string l = lines.substr(0, lines.find_first_of('\n'));

			// Print and move to next line
			__Print(position.x, y, zSTRING(l.c_str()));
			y += FontY();

			if (lines.find_first_of('\n') == std::string::npos)
				break;

			// Remove from string which contains the other lines
			lines = lines.substr(lines.find_first_of('\n')+1);
		} while (true);		
	}

	/** Returns the main session view */
	static zCView* GetSessionView()
	{
		if (!oCGame::GetGame())
			return nullptr;

		return oCGame::GetGame()->GetSessionView();
	}

	/** Returns the height of the current font in pixels */
	int FontY()
	{
		XCALL(MemoryLocations::Gothic::zCView__FontY_void);
	}

protected:
	/** Prints the given line of text to the view */
	void __Print(int x, int y, const zSTRING& str)
	{
		XCALL(MemoryLocations::Gothic::zCView__Print_int_int_zSTRING_const_r)
	}
};