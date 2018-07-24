#include <cstdio>
#include <vector>
#include <functional>
#include "cmdhub.h"

enum JobStatus
{
    JOB_STOP    = -1,
    JOB_DONE    = 0,
    JOB_SUSPEND = 1,
};

class JobWorker: public std::unary_function<void, JobStatus>
{
public:
    virtual JobStatus operator()(void) = 0;
};

class Foo2: public JobWorker
{
public:
	JobStatus operator()()
	{
		CMDHUB_INFO("now in Foo2::operator(void):int, this:[%p]", this);
		return JOB_DONE;
	}
};

class Bar2: public JobWorker
{
public:
	JobStatus operator()()
	{
		CMDHUB_INFO("now in Bar2::operator(void):int, this:[%p]", this);
		return JOB_DONE;
	}
};

class Temp2
{
public:
    Temp2(): cnt(0) {}

    JobStatus doSth()
    {
        cnt++;
        CMDHUB_INFO("now in Temp2::doSth(void):int, this:[%p], cnt:[%d]", this, cnt);
        return JOB_DONE;
    }

    int cnt;
};

template<typename ClassName = JobWorker>
class JobWorkerWrapper: public JobWorker
{
public:
    typedef JobStatus (*FunctionPointer)();
    typedef JobStatus (ClassName::*MemberFunction)();

    JobWorkerWrapper(FunctionPointer fn):  _objPtr(NULL), _objFn(NULL), _fn(fn)
    {}

    JobWorkerWrapper(ClassName* ptr, MemberFunction fn): _objPtr(ptr), _objFn(fn), _fn(NULL)
    {}

public:
    JobStatus operator()()
    {
        if (_fn != NULL)
        {
            return _fn();
        }
        return (*_objPtr.*_objFn)();
    }

private:
    // for 成员函数
    ClassName*      _objPtr;
    MemberFunction  _objFn;

    // for 函数指针
    FunctionPointer _fn;
};

static JobStatus func()
{
    CMDHUB_INFO("now in func(void):JobStatus, func:[%p], &func:[%p]", func, &func);
    return JOB_SUSPEND;
}

/*
 * 一个job有名字、负责人(worker)；每个worker抽象为一个函数对象的地址（相比函数指针多了状态，更符合现实场景）
 * 函数对象可以是继承Functor并并实现JobStatus operator()方法的对象。
 * 对于成员函数怎么转成Functor的子类，可以加一个包装器。
 * 对于普通的函数指针，也提供包装器。
 */

class HelloworldWorker: public JobWorker
{
public:
    HelloworldWorker(): helloWrapper(this, &HelloworldWorker::hello)
                      , hahaWrapper(this, &HelloworldWorker::haha)
    {}

    JobStatus operator()()
    {
        CMDHUB_INFO("now in HelloworldWorker::operator(void):int, this:[%p]", this);
        return JOB_DONE;
    }

    JobStatus hello()
    {
        CMDHUB_INFO("now in HelloworldWorker::hello(void):int, this:[%p]", this);
        return JOB_DONE;
    }

    JobStatus haha()
    {
        CMDHUB_INFO("now in HelloworldWorker::haha(void):int, this:[%p]", this);
        return JOB_DONE;
    }

    JobStatus helloworld()
    {
        CMDHUB_INFO("now in HelloworldWorker::helloworld(void):int, this:[%p]", this);
        return JOB_DONE;
    }

    void run()
    {
        // 成员函数经过包装后可以使用
        Temp2 tmp;
        JobWorkerWrapper<Temp2> tmpWrapper(&tmp, &Temp2::doSth);

        // 函数指针经过包装后可以使用
        JobWorkerWrapper<> funcWrapper(&func);

        // 实现Functor接口的对象可以使用

        std::vector< JobWorker* > list;
        list.push_back(&foo);
        list.push_back(&bar);
        list.push_back(&bar);
        list.push_back(&bar);
        list.push_back(&tmpWrapper);
        list.push_back(&funcWrapper);
        list.push_back(&helloWrapper);
        list.push_back(&hahaWrapper);
        list.push_back(new JobWorkerWrapper<HelloworldWorker>(this, &HelloworldWorker::helloworld));

        for (std::vector< JobWorker* >::iterator it = list.begin(); it != list.end(); ++it)
        {
            JobStatus status = (*(*it))();
            CMDHUB_INFO("status:[%d]", status);
        }

        tmp.doSth();
    }

private:
    Foo2 foo;
    Bar2 bar;
    JobWorkerWrapper<HelloworldWorker> helloWrapper;
    JobWorkerWrapper<HelloworldWorker> hahaWrapper;
};

CMDHUB_CMD_TYPE cmd()
{
    HelloworldWorker worker;
    worker.run();
    return 0;
}

CMDHUB_INIT_TYPE init()
{
	cmdhub::addCmd({exec: cmd, desc: "funobj command"});

    return 0;
}
