#pragma once

class GVisual;
class IVisualChangedObserver
{
public:
	/**
	 * Called when the given visual changed and the pipeline-states need to be recreated
	 */
	virtual void OnVisualChanged(GVisual* visual) = 0;
};