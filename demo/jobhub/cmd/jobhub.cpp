#include <cstdio>
#include <vector>
#include <functional>
#include "cmdhub.h"
#include "jobhub.h"

class Foo: public jobhub::JobWorker
{
public:
	jobhub::JobStatus operator()()
	{
		CMDHUB_INFO("now in Foo::operator(void):int, this:[%p]", this);
		return jobhub::JOB_DONE;
	}
};

class Bar: public jobhub::JobWorker
{
public:
	jobhub::JobStatus operator()()
	{
		CMDHUB_INFO("now in Bar::operator(void):int, this:[%p]", this);
		return jobhub::JOB_DONE;
	}
};

class Temp
{
public:
    Temp(): cnt(0) {}

    jobhub::JobStatus doSth()
    {
        cnt++;
        CMDHUB_INFO("now in Temp::doSth(void):int, this:[%p], cnt:[%d]", this, cnt);
        return jobhub::JOB_DONE;
    }

    int cnt;
};

static jobhub::JobStatus stop()
{
    CMDHUB_INFO("now in stop(void):JobStatus, stop:[%p]", &stop);
    return jobhub::JOB_SUSPEND;
}

CMDHUB_CMD_TYPE cmd()
{
    Foo foo;
    Bar bar;
    jobhub::JobWorkerWrapper<> stopWrapper(&stop);

    Temp temp;
    jobhub::JobWorkerWrapper<Temp> tempWrapper(&temp, &Temp::doSth);

    jobhub::JobHub hub;
    hub.run("foo", &foo); // .then("xxx", &xxx).then("yyy", &yyy); // 串行调用：foo -> xxx --> yyy
    hub.run("bar", &bar); // 并行调用: 无法保证foo/bar的调用顺序
    hub.run("temp", &tempWrapper); // .after("bar", "foo");  // 并行到串行调用：bar & foo => temp
    hub.finally(&stopWrapper);
    hub.start();
    return 0;
}

CMDHUB_INIT_TYPE init()
{
	cmdhub::addCmd({exec: cmd, desc: "jobhub test command"});

    return 0;
}
