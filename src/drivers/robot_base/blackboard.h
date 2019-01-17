#pragma once

#include <tgf.h>
#include <track.h>
#include <car.h>
#include <raceman.h>
#include <robottools.h>
#include <robot.h>

class blackboard
{
public:
	blackboard();
	~blackboard();

	static blackboard* _instance;

	static blackboard* Instance()
	{
		if (!_instance)
		{
			_instance = new blackboard;
		}
		return _instance;
	}	

	tCarElt* car;

};

