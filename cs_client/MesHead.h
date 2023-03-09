#pragma once
enum {
	MES_SHOW,
	MES_JOIN,
	MES_TALK,
	MES_CREATE,
	MES_EXIT,
	MES_LEAVE,
	MES_SHOW_REPLY
};
//exit talk join leave create show
#define MAX_ROOM 10
class MesHead
{
public:
	int MesType;
	int datalen;
	MesHead() = default;
	~MesHead() = default;

private:

};

class MesShow :public MesHead {//默认继承是private继承,必须改写才能访问到mesType
public:
	MesShow() {
		MesType = MES_SHOW;
		datalen = sizeof(MesShow);
	}
};
class MesShowReply :public MesHead {
public:
	int RoomCount;
	struct RoomInfo
	{
		int room_id;
		int room_max;
		int room_count;
	}roomlist[MAX_ROOM];
	MesShowReply() {
		MesType = MES_SHOW_REPLY;
		datalen = sizeof(MesShowReply);
	}
};
class MesJoin :public MesHead {
public:
	int roomid;
	/*MesJoin(int id):roomid(id) {
		MesType = MES_JOIN;
		datalen = sizeof(MesJoin);
	}*/
	MesJoin(int id) {
		roomid = id;
		MesType = MES_JOIN;
		datalen = sizeof(MesJoin);
	}
	MesJoin() {
		MesType = MES_JOIN;
		datalen = sizeof(MesJoin);
	}
};
class MesCreate :public MesHead {
public:
	MesCreate() {
		MesType = MES_CREATE;
		datalen = sizeof(MesCreate);
	}
};

class MesExit :public MesHead {
public:
	MesExit() {
		MesType = MES_EXIT;
		datalen = sizeof(MesExit);
	}
};
class MesLeave :public MesHead {
public:
	MesLeave() {
		MesType = MES_LEAVE;
		datalen = sizeof(MesLeave);
	}
};
class MesTalk :public MesHead {
	char buf[1000];
public:
	MesTalk() {
		MesType = MES_TALK;
		datalen = sizeof(MesTalk);
	}
	char* getbuf() {
		return buf;
	}
};