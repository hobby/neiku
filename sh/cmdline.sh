#!/bin/bash

#
# File:   neiku/sh/cmdline.sh
# Brief:  cmdline tools for you script.
# Author: Hobby <ku7d@qq.com>
# Date:   2015/04/26
# ChangeLog:
# --- 2015/04/26
# * long options only
# 
# Example:
# +----------- test_cmdline.sh ------------------
# | source cmdline.sh
# | cmdline_init "id:,name:" "$@" || exit $?
# | echo "id   ===> [$cmdline_getopt '--id'   'default-id')]"
# | echo "name ===> [$cmdline_getopt '--name' 'default-name')]"
# +----------------------------------------------
# 
# $./test_cmdline.sh --id="this is id" --name="this is name" 
# id   ===> [this is id]
# name ===> [this is name]
#

declare -A cmdline_m_opts

# cmdline_init optstr args
function cmdline_init()
{
    TEMP=$(getopt -o "" --long "$1" -n "$0" -- "$@") || return $?
    eval set -- "$TEMP"
    while true
    do
        if [ "$1" = "--" ]
        then
             break
        else
            cmdline_m_opts["$1"]="$2"
            shift 2
        fi
    done
    return 0
}

# cmdline_getopt optname defopt
function cmdline_getopt()
{
    value="${cmdline_m_opts[$1]}"
    if [ -z "$value" ] 
    then
        value="$2"
    fi
    echo "$value"
    return 0
}
