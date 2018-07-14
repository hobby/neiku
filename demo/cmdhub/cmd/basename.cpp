#include <string.h>
#include <cstdio>
#include "cmdhub.h"

static std::string name;

std::string GetFileName(const std::string& path)
{
	size_t pos = path.find_last_of('/');
	std::string filename = path.substr(pos+1);

	pos = filename.find_last_of('.');
	std::string name = filename.substr(0, pos);
    return name;
};

CMDHUB_CMD_TYPE cmd()
{
    std::string val = GetFileName(name);
    printf("val:[%s], name:[%s]\n", val.c_str(), name.c_str());
    return 0;
}

CMDHUB_INIT_TYPE init()
{
    // cmdhub::registCmd("basename", cmd, "basename command");
    // cmdhub::registOpt("basename", name);

    return 0;
}
