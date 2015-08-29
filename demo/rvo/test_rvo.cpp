
#include <inttypes.h>
#include <neiku/log.h>

class CObject
{
    public:
        int i;

    public:
        CObject()
        {
            LOG_MSG("执行了默认构造函数, i=%d", i);
        }

        CObject(int _i): i(_i)
        {
            LOG_MSG("执行了初始构造函数, i=%d", i);
        }

        CObject(const CObject& obj)
        {
            LOG_MSG("执行了拷贝构造函数, curr i=%d, from i=%d", i, obj.i);
        }

        CObject& operator = (const CObject& obj)
        {
            LOG_MSG("执行了赋值构造函数, curr i=%d, from i=%d", i, obj.i);
            return *this;
        }
        

        ~CObject()
        {
            LOG_MSG("执行了析构函数, i=%d", i);
        }
};

// 有优化
CObject GetObj(int i)
{
    CObject obj(i);
    LOG_MSG("GetObj(%d) 返回之前", i);
    return obj;
}

// 有优化
CObject GetObjRVO(int i)
{
    LOG_MSG("GetObjRVO(%d) 返回之前", i);
    return CObject(i);
}

// 没有优化
CObject GetObjRVO2(int i)
{
    CObject obj(i);
    LOG_MSG("GetObj(%d) 返回之前", i);
    return CObject(obj.i);
}

// 没有优化
CObject GetObjRVO3(int i)
{
    CObject obj(i);
    LOG_MSG("GetObj(%d) 返回之前", i);
    return CObject(obj);
}

int main(int argc, char* argv[])
{
    LOG_MSG("测试RVO");

    //CObject obj1(1);

    //CObject obj2 = GetObj(2);

    // obj3 默认构造一次，因为不知道后面还会有赋值 
    // GetObj(3) 会初始构造一次
    // obj3 会赋值构造一次
    // 总体上，现代编译器已经减少临时变量 
    CObject obj3;
    obj3 = GetObj(3);
    LOG_MSG("");

    // 同obj3
    CObject obj4;
    obj4 = GetObjRVO(4);
    LOG_MSG("");

    // 只有一次初始化构造，没有赋值构造
    // 即编译器优化了
    CObject obj5 = GetObjRVO(5);

    // 同obj5
    CObject obj6 = GetObj(6);

    //CObject obj7 = GetObjRVO2(7);
    //LOG_MSG("");

    CObject obj8 = GetObjRVO2(8);
    //LOG_MSG("");
    return 0;
}
