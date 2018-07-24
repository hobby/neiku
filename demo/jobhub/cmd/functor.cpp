#include <cstdio>
#include "cmdhub.h"

class Object;
typedef int (Object::*Routine)();

class Object
{
public:
	int print()
	{
		CMDHUB_INFO("not in Object::print(), this:[%p]", this);
		return 0;
	}
};

CMDHUB_CMD_TYPE cmd()
{
	Object obj, obj2;
	obj.print();;

	Object* pObj = &obj;
	Routine routine = &Object::print;
	(*pObj.*routine)(); // 写法1
	(pObj->*routine)(); // 写法2

	pObj = &obj2;
	(*pObj.*routine)();

    return 0;
}

CMDHUB_INIT_TYPE init()
{
	cmdhub::addCmd({exec: cmd, desc: "functor command"});

    return 0;
}