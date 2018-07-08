#include "neiku/log.h"
#include "neiku/serialize.h"

using namespace neiku;

class CPerson
{
public:
    int32_t id;
    std::string name;

    CPerson(): id(0)
    {}

    template <typename T>
    T& serialize(T& t)
    {
        SERIALIZE(t, id);
        SERIALIZE(t, name);
        return ar;
    }
};

int main(int argc, char* argv[])
{
    CPerson person;
    person.id = 123;
    person.name = "YIF";

    CObjDumper dumper;
    dumper << person;

    return 0;
}
