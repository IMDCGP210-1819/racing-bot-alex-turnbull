#pragma once

#include <tgf.h>
#include <track.h>
#include <car.h>
#include <raceman.h>
#include <robottools.h>
#include <robot.h>

class blackboard
{
private:
	static blackboard* _instance;

public:	
	static blackboard* Instance()
	{
		if (!_instance)
		{
			_instance = new blackboard;
		}
		return _instance;
	}

	blackboard();
	~blackboard();


	tCarElt* car;

};

