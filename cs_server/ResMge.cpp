#include "ResMge.h"
ResMge* ResMge::instance = NULL;
int ResMge::getfreeName() {
	if (exist.empty())return -1;
	else {
		int fr = exist.front();
		exist.pop();
		return fr;
	}
}
void ResMge::recycle(int roomid) {
	exist.push(roomid);
}
ResMge::ResMge() {
	for (int i = 1; i <= 100; i++)exist.push(i);

}
ResMge* ResMge::getInstance() {
	if (instance == NULL) {
		instance = new ResMge;
	}
     return instance;
}
ResMge::~ResMge() {
	if (instance != NULL)delete instance;
}