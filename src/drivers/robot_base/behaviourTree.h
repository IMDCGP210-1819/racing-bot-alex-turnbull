#include <iostream>
#include <list>


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

int main() {
	Sequence *root = new Sequence, *sequence1 = new Sequence;
	Selector* selector1 = new Selector;
	root->addChild(selector1);
	selector1->addChild(sequence1);

	while (!root->run())
		std::cout << "--------------------" << std::endl;
	std::cout << std::endl << "Operation complete.  Behaviour tree exited." << std::endl;
	std::cin.get();
}