// vim:ts=4:sw=4:expandtab
#include "cmdhub.h"

int init()
{
	cmdhub::getRef<std::string>() = "cmdbar v0.1";
    return 0;
}

int main(int argc, char* argv[])
{
    return cmdhub::ready(init).launch(argc, argv);
}
