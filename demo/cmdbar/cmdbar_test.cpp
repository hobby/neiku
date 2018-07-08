// vim:ts=4:sw=4:expandtab
#include "neiku/log.h"
#include "cmdbar.h"

int main(int argc, char* argv[])
{
    LOG_MSG("cmdbar test");

    return cmdbar::launch(argc, argv);
}
