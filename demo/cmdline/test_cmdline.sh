#!/bin/bash

#
# test script for cmdline.
# 

if [ -n "$debug" ]
then
     set -x
fi

source ../../sh/cmdline.sh

cmdline_init "id:,name:" "$@" || exit $?

echo "id   ===> [$(cmdline_getopt '--id'   'default-id')]"
echo "name ===> [$(cmdline_getopt '--name' 'default-name')]"
