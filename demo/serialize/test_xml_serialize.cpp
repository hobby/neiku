#include "neiku/log.h"
#include "neiku/xml_serialize.h"

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
        //SERIALIZE(t, id);
        SERIALIZE(t, name);
        return t;
    }
};

int main(int argc, char* argv[])
{
    CPerson person;
    person.id = 123;
    person.name = "YIF";

    //XmlDumper dumper("xml");
    XmlDumper dumper;
    dumper << person;
    LOG_MSG("xml:[%s]", dumper.c_str());    

    if (1 < argc)
    {
        std::string xml = argv[1];
        CPerson obj;
        XmlParser parser(xml.data(), xml.size());
        if (!parser)
        {
            LOG_MSG("parse xml fail, xml:[%s]", xml.c_str());
            return 1;
        }

        parser >> obj;
        LOG_MSG("id:[%d], name:[%s], xml:[%s]", obj.id, obj.name.c_str(), xml.c_str());

    }

    return 0;
}
