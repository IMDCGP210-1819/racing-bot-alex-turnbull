#include <iostream>
#include <list>

#include "blackboard.h"

//basic implementation of a behaviour tree, not fully working as intended or as a BT typically would

//function called by braking node to determine how much power to apply when approaching a corner
static float calculateBraking()
{
	auto car = blackboard::Instance()->car;

	//my method involved grabbing the arc of the next 3 segments adding them up directly and using the float with a constant to produce amount of power
	float value = car->_trkPos.seg->next->arc + car->_trkPos.seg->next->next->arc + car->_trkPos.seg->next->next->next->arc + 0.1;
	
	//little check to see if the car is going quite fast and in turn need a bit more power for the turn ahead
	if (car->pub.speed > 40)
	{
		value += 0.2;
	}
	return value;
}

//basic node class that all in the tree will be derived from
//nodes will require a defintion for what happens when they are run
class Node {
public:
	virtual bool run() = 0;
};

//This is a base class that the Selector and Sequence parts will use, it's composite and contains a list of other nodes
class CompositeNode : public Node {
private:
	std::list<Node*> children;
public:
	const std::list<Node*>& getChildren() const { return children; }
	void addChild(Node* child) { children.emplace_back(child); }
};

//class for the selector implementation
//if one child is successful then the entire node is run
//else will fail if all of it's child fail
class Selector : public CompositeNode {
public:
	virtual bool run() override {
		for (Node* child : getChildren()) {
			if (child->run())
				return true;
		}
		return false;
	}
};


//class for sequence implementation
//needs all of it's children to run successfully else this node will fail
class Sequence : public CompositeNode {
public:
	virtual bool run() override {
		for (Node* child : getChildren()) {
			if (!child->run())
				return false;
		}
		return true;
	}
};

//very basic struct that certain nodes will need to use to check/write if the car is stuck
struct carStatus {
	bool isStuck;
};

//accelerate node
//checks if the car isn't stuck and if possible start to drive forward
class Accelerate : public Node {
private:
	carStatus* status;
public:
	Accelerate(carStatus* status) : status(status) {}
	virtual bool run() override {
		std::cout << "In Accelerate State" << std::endl;
		auto car = blackboard::Instance()->car;

		if(!status->isStuck)
		{
			car->ctrl.accelCmd = 0.5;
			car->ctrl.brakeCmd = 0.0;
		}		

		return true;
	}
};


//braking node
//checks the current speed of the car and if a corner is approaching and will call the function above to calculate how much braking force to apply
class Brake : public Node {
public:
	virtual bool run() override {
		std::cout << "In Brake State" << std::endl;

		auto car = blackboard::Instance()->car;
		float speed = car->pub.speed;

		std::cout << calculateBraking() << std::endl;
		if (speed > 30 && car->_trkPos.seg->next->type != TR_STR)
		{
			car->ctrl.accelCmd = 0.0;
			car->ctrl.brakeCmd = calculateBraking();
		}

		return true;
	}
};

//steering node
//this car aims to keep to the middle of the track for maximum room for error either side
//calcuates the angle needed to reach the middle using the current yaw of the car in comparison to the midline of the segment
//the angle is applied to the steering command, dividing by the cars steerLock  as that defines the angle of 100% steering
class Turn : public Node {
public:
	virtual bool run() override {
		std::cout << "In Turn State" << std::endl;

		auto car = blackboard::Instance()->car;

		float angle;
		const float SC = 1.0;

		angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
		NORM_PI_PI(angle); //once calculated, convert back in terms of PI as how the robot operates
		angle -= SC*car->_trkPos.toMiddle/car->_trkPos.seg->width;

		car->ctrl.steer = angle / car->_steerLock;

		return true;
	}
};

//gear handling node
//simply checks the current speed of the car and increase the gear to the appropriate level for the speed
class ChangeGear : public Node {
public:
	virtual bool run() override {
		std::cout << "In ChangeGear State" << std::endl;

		auto car = blackboard::Instance()->car;

		float speed = car->pub.speed;

		car->ctrl.gear = 1; // first gear
		if (speed > 15)
		{
		car->ctrl.gear = 2;
		}if (speed > 30)
		{
		car->ctrl.gear = 3;
		}if (speed > 40)
		{
		car->ctrl.gear = 4;
		}if(speed > 50)
		{
		car->ctrl.gear = 5;
		}if(speed > 60)
		{
		car->ctrl.gear = 6;
		}

		return true;
	}
};

//node to check if the car is stuck
//checks if the car has gone off course and collided with a wall by checking if the angle of the car over 30 degrees what it should be and if it is moving very/almost stopped
//start recording how long the car has been stuck for to see if it's long enough a change needs to be made
class CheckIfStuck : public Node {
private:
	carStatus* status;
public:
	CheckIfStuck(carStatus* status) : status(status) {}
	virtual bool run() override {
		std::cout << "In CheckIfStuck State" << std::endl;

		auto car = blackboard::Instance()->car;
		float timeEntered = car->race.curLapTime;
		float distance = car->race.distRaced;
		
		float angle;

		angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
		NORM_PI_PI(angle);

		if (car->race.curLapTime > 3)
		{
			if (fabs(angle) > 30.0 / 180.0*PI & (car->pub.speed < 0.2))
			{
				status->isStuck = true;
				std::cout << "Car is Stuck" << std::endl;
				blackboard::Instance()->ticksSpentStuck += 1;
			}
			else
			{
				status->isStuck = false;
			}
		}

		return status->isStuck;
	}
};

//node to follow up being stuck
//if stuck for long enough try to reverse out a little bit and steer the car away from the wall
//reset the stuck counter and run the other drive functions as normal
class AttemptToUnstick : public Node {
private:
	carStatus* status;
public:
	AttemptToUnstick(carStatus* status) : status(status) {}
	virtual bool run() override {
		std::cout << "In AttemptToUnstick State" << std::endl;

		std::cout << blackboard::Instance()->ticksSpentStuck << std::endl;

		auto car = blackboard::Instance()->car;
		
		if(blackboard::Instance()->ticksSpentStuck < 50)
		{
			float angle;
			const float SC = 1.0;

			angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
			NORM_PI_PI(angle); // put the angle back in the range from -PI to PI
			angle -= SC * car->_trkPos.toMiddle / car->_trkPos.seg->width;

			car->ctrl.gear = -1;
			car->ctrl.accelCmd = 1.0;
			car->ctrl.steer = -(angle / car->_steerLock);

			status->isStuck = true;
		}else
		{
			blackboard::Instance()->ticksSpentStuck = 0;
			status->isStuck = false;
		}		

		return status->isStuck;
	}
};