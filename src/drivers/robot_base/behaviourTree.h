#include <iostream>
#include <list>

#include "blackboard.h"

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

class Accelerate : public Node {
public:
	virtual bool run() override {
		std::cout << "In Accelerate State" << std::endl;
		auto car = blackboard::Instance()->car;
		
		car->ctrl.accelCmd = 0.5;

		return true;
	}
};

class Brake : public Node {
public:
	virtual bool run() override {
		std::cout << "In Brake State" << std::endl;

		auto car = blackboard::Instance()->car;

		car->ctrl.brakeCmd = 0.5;

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