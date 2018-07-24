// vim:ts=4:sw=4:expandtab
#ifndef __NK_JOBHUB_H__
#define __NK_JOBHUB_H__

#include <vector>
#include <string>
#include <functional>

namespace jobhub
{

// 作业状态
enum JobStatus
{
    JOB_STOP    = -1,
    JOB_DONE    = 0,
    JOB_SUSPEND = 1,
};

// 作业接口
class JobWorker: public std::unary_function<void, JobStatus>
{
public:
    virtual JobStatus operator()(void) = 0;
};

// 作业负责人包装器
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

// 作业调度器
class JobHub
{
public:
    void start()
    {
        bool earlyStop = false;
        std::map< std::string, JobWorker* >::iterator itJob = _jobWorkerMap.begin();
        for (; itJob != _jobWorkerMap.end(); ++itJob)
        {       
            JobStatus status = (*itJob->second)();
            if (status == JOB_DONE)
            {
                continue;
            }    
            if (status == JOB_STOP)
            {
                earlyStop = true;
                break;
            }
        }
        if ((itJob == _jobWorkerMap.end()) || (earlyStop == true))
        {
            (*_job4Stop)();
        }
    }

    JobHub& run(const std::string& jobName, JobWorker* jobWorker)
    {
        _jobWorkerMap.insert(std::pair<std::string, JobWorker*>(jobName, jobWorker));
        return *this;
    }

    JobHub& finally(JobWorker* jobWorker)
    {
        _job4Stop = jobWorker;
        return *this;
    }

private:
    std::map< std::string, JobWorker* > _jobWorkerMap;
    JobWorker* _job4Stop;
};

};

#endif