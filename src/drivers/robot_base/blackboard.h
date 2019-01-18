#pragma once

#include <tgf.h>
#include <track.h>
#include <car.h>
#include <raceman.h>
#include <robottools.h>
#include <robot.h>

#include <list>

//basic blackboard implementation, not proper design

//define and create a single blackboard instance that the behaviour tree can access for values
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

	//define a variable to store the car/robot and hence it's controls
	tCarElt* car;

	int ticksSpentStuck = 0;
};