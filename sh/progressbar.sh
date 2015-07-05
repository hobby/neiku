#!/bin/bash

#
# File:   neiku/sh/progrssbar.sh
# Brief:  progressbar for script program.
# Author: Hobby <ku7d@qq.com>
# Date:   2015/07/05
# ChangeLog:
# 
# Example:
# $ source progressbar.sh
# $ progressbar_init bar_name;
# $ max=100; i=0; while((i++<max)); do usleep 80000 && progressbar_clean && echo you options log$i; progressbar_show $i $max $?; done; progressbar_close;
# 
# vim:ts=4;sw=4;expandtab
#

pb_beg=0;
pb_end=0;
pb_err=0;
pb_name="";

function progressbar_init() # name
{
    pb_beg=$(echo `date +%s`*1000000000+`date +%N` | bc);
    pb_end=0;
    pb_err=0;
    pb_name="$1"
    if [ -z "$pg_name" ] ; then
        pb_name="progress bar";
    fi;
}

function progressbar_close()
{
    echo;
}

# 1ms
function progressbar_clean()
{
    tput dl1;
}

# 18+ms
function progressbar_show() # curr total retcode
{
    tput sc;
    tput dl1;

    # 计算距离最后一次更新进度条的时间间隔(耗时)
    pb_end=$(echo `date +%s`*1000000000+`date +%N` | bc);
    local ms=$(((pb_end-pb_beg)/1000000));
    local us=$(((pb_end-pb_beg)%1000000));

    # 计算进度完成百分比
    local ps1=$(($1*100/$2));
    local ps2=$(($1*100%$2/100));

    # 预估剩余时间
    local te=$((ms*(100-ps1)));

    # 累计错误数量
    if [ $3 != 0 ] ; then let pb_err=pb_err+1; fi;

    # 显示进度条
    echo -n "$pb_name: progress: $ps1.$ps2% ($1/$2), time-remaining: ${te}ms, errors: ${pb_err}";

    tput rc;
    pb_beg=$pb_end;
}
