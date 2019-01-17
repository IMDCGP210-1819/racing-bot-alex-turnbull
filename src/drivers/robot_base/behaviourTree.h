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
		return true;
	}
};

class Brake : public Node {
public:
	virtual bool run() override {
		std::cout << "In Brake State" << std::endl;
		return true;
	}
};

class Turn : public Node {
public:
	virtual bool run() override {
		std::cout << "In Turn State" << std::endl;
		return true;
	}
};

class ChangeGear : public Node {
public:
	virtual bool run() override {
		std::cout << "In ChangeGear State" << std::endl;
		return true;
	}
};