#pragma once
#include "ClientObject.h"
#include <queue>
class ResMge
{
private:
	std::queue<int>exist;
	static ResMge* instance;
public:
	int getfreeName();
	void recycle(int roomid);
	ResMge();
	static ResMge* getInstance();
	~ResMge();

};

