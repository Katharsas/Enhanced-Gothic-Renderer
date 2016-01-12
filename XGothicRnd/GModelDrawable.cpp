#include "pch.h"
#include "GModelDrawable.h"


GModelDrawable::GModelDrawable(GVisual* sourceVisual) : GBaseDrawable(sourceVisual, true, true) // Inform the visual about drawcalls
{
}


GModelDrawable::~GModelDrawable()
{
}
