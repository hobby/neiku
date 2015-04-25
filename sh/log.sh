#!/bin/bash
#
# log.sh - log for scripts (neiku)
#
# auther: neiku
# date  : 2014-07-31
#
######################################
# usage :
# +-------test_log.sh-------
# |#!/bin/bash
# |
# |source /path/to/log.sh
# |
# |log_init "/path/to/$0" [log_level]
# |
# |log_msg this is msg log.
# |log_err this is err log.
# |log_dbg this is dbg log.
# |
# +--------------------------
# $./test_log.sh
# $cat /path/to/test_log.sh.log
######################################

# define
_LOG_LEVEL_ERR=1
_LOG_LEVEL_MSG=2
_LOG_LEVEL_DBG=3

_LOG_VERBOSE_OFF=0
_LOG_VERBOSE_ON=1

# config
_log_level=0
_log_file=""
_log_maxnum=0
_log_maxsize=0
_log_verbose=0

function log_log()
{
    # 日志优先级不够高，啥也不做
    [ "$_log_level" -lt "$1" ] && return 0;

    pid=$2;
    [ ! -z "$3" ] && filename=$(basename $3);
    lineno=$4;
    funcname=$5;
    logleveltag=$6;
    shift 6;
    echo "[`date '+%F %T.%N'`][pid:$pid][$filename:$lineno][$funcname] $logleveltag: $*" >> ${_log_file};

    [ $_log_verbose -eq $_LOG_VERBOSE_ON ] &&
    {
        echo "[`date '+%F %T.%N'`][pid:$pid][$filename:$lineno][$funcname] $logleveltag: $*"
    }

    # TODO: 文件滚动
}

function log_init()
{
    [ ! -z "$1" ] && _log_file="$1.log"
    [ ! -z "$2" ] && _log_level=$2 || _log_level=$_LOG_LEVEL_MSG;

    # 创建日志目录
    dirname="$(dirname $_log_file)";
    ls "$dirname" 1>/dev/null 2>&1 || mkdir -p "$dirname";
}

function log_verbose_on()
{
    _log_verbose=$_LOG_VERBOSE_ON;
}

function log_verbose_off()
{
    _log_verbose=$_LOG_VERBOSE_OFF;
}

function log_level_err()
{
    echo 1;
}
function log_level_msg()
{
    echo 2;
}
function log_level_dbg()
{
    echo 3;
}

# alias
shopt -s expand_aliases
alias log_err='log_log "$_LOG_LEVEL_ERR" "$$" "$BASH_SOURCE" "$LINENO" "$FUNCNAME" ERR';
alias log_msg='log_log "$_LOG_LEVEL_MSG" "$$" "$BASH_SOURCE" "$LINENO" "$FUNCNAME" MSG';
alias log_dbg='log_log "$_LOG_LEVEL_DBG" "$$" "$BASH_SOURCE" "$LINENO" "$FUNCNAME" DBG';
# log_msg this is msg log.

# eval
log_err='eval log_log "$_LOG_LEVEL_ERR" "$$" "$BASH_SOURCE" "$LINENO" "$FUNCNAME" ERR';
log_msg='eval log_log "$_LOG_LEVEL_MSG" "$$" "$BASH_SOURCE" "$LINENO" "$FUNCNAME" MSG';
log_dbg='eval log_log "$_LOG_LEVEL_DBG" "$$" "$BASH_SOURCE" "$LINENO" "$FUNCNAME" DBG';
# $log_msg this is msg log.

