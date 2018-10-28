#include "neiku/log.h"
#include "neiku/serialize.h"
#include "neiku/json.h"

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
        return t;
    }
};

int main(int argc, char* argv[])
{
    CPerson person;
    person.id = 123;
    person.name = "YIF";

    LOG_MSG("json_encode:[%s]", json_encode(person));
    LOG_MSG("json_encode_ml:[%s]", json_encode_ml(person));

    return 0;
}
