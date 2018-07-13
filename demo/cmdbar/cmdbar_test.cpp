// vim:ts=4:sw=4:expandtab
#include "cmdbar.h"

int main(int argc, char* argv[])
{
	cmdbar::getRef<std::string>() = "cmdbar v0.1";
    return cmdbar::launch(argc, argv);
}
