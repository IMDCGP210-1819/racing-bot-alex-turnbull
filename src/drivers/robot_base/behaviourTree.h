#include <iostream>
#include <list>

#include "blackboard.h"

static float calculateBraking()
{
	auto car = blackboard::Instance()->car;
	float value = car->_trkPos.seg->next->arc + car->_trkPos.seg->next->next->arc + car->_trkPos.seg->next->next->next->arc + 0.1;
	if (car->pub.speed > 40)
	{
		value += 0.2;
	}
	return value;
}

class Node {
public:
	virtual bool run() = 0;
};

class CompositeNode : public Node {
private:
	std::list<Node*> children;
public:
	const std::list<Node*>& getChildren() const { return children; }
	void addChild(Node* child) { children.emplace_back(child); }
};

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

struct carStatus {
	bool isStuck;
};

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

class Turn : public Node {
public:
	virtual bool run() override {
		std::cout << "In Turn State" << std::endl;

		auto car = blackboard::Instance()->car;

		float angle;
		const float SC = 1.0;

		angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
		NORM_PI_PI(angle); // put the angle back in the range from -PI to PI
		angle -= SC*car->_trkPos.toMiddle/car->_trkPos.seg->width;

		car->ctrl.steer = angle / car->_steerLock;

		return true;
	}
};

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