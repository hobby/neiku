#!/bin/bash

#
# File:   neiku/sh/cpu_usagerate_user.sh
# Brief:  show cpu usage rate for in user mode.
# Author: Hobby <ku7d@qq.com>
# Date:   2015/11/10
# Usage:  cpu_usagerate_user.sh [--delay-ms=DELAYMS]
# Algorithm: cpu usage rate = (user+nice) / (user+nice + sys+irq+softirq +idle+iowait) * 100%
# More:   /proc/stat
# 

# 默认刷新周期是5秒
show_delay_ms=5000
TEMP=$(getopt -o "d:h" --long delay-ms:,help -n "$0" -- "$@")
if [ $? != 0 ] ; then
    echo "Usage: $0 [--delay-ms=DELAYMS]"
    exit 1
fi
eval set -- "$TEMP"
while true
do
    case "$1" in
        -d|--delay-ms)  show_delay_ms="$2" ; shift 2 ;;
        -h|--help)      echo "Usage: $0 [--delay-ms=DELAYMS]"; exit 0 ; ;;
        --)             shift ; break ;;
        *)              echo "parse options error!" >&2 ; exit 1 ;;
    esac
done
echo "begin show cpu usagerate with ${show_delay_ms}ms delay, exit with crtl+c."

# cpu使用率、状态数据
declare -a cpu_index
declare -a cpu_usagerate
declare -a cpu_name
declare -a cpu_stat_user
declare -a cpu_stat_nice
declare -a cpu_stat_sys
declare -a cpu_stat_idle
declare -a cpu_stat_iowait
declare -a cpu_stat_irq
declare -a cpu_stat_softirq
#declare -a cpu_stat_steal
#declare -a cpu_stat_guest

# 初始记录当前cpu数据
i=0
while read name user nice sys idle iowait irq softirq steal guest
do
    # 只使用cpu数据
    if ! expr match $name "^cpu" >/dev/null 2>&1; then
        break;
    fi
    
    # 记录每个cpu的当前状态
    cpu_index[$i]=$i
    cpu_name[$i]=$name
    cpu_usagerate[$i]=0
    cpu_stat_user[$i]=$user
    cpu_stat_nice[$i]=$nice
    cpu_stat_sys[$i]=$sys
    cpu_stat_idle[$i]=$idle
    cpu_stat_iowait[$i]=$iowait
    cpu_stat_irq[$i]=$irq
    cpu_stat_softirq[$i]=$softirq
    let i+=1;
done < /proc/stat

# 每隔delayms毫秒，计算每个cpu的使用率
while usleep $((show_delay_ms*1000))
do
    i=0
    while read name user nice sys idle iowait irq softirq steal guest
    do
        # 只使用cpu数据
        if ! expr match $name "^cpu" >/dev/null 2>&1; then
            break;
        fi

        # 上次cpu使用情况
        preused=0
        let preused+=cpu_stat_user[$i]
        let preused+=cpu_stat_nice[$i]
        pretotal=$preused
        let pretotal+=cpu_stat_sys[$i]
        let pretotal+=cpu_stat_iowait[$i]
        let pretotal+=cpu_stat_irq[$i]
        let pretotal+=cpu_stat_softirq[$i]
        let pretotal+=cpu_stat_idle[$i]

        # 当前cpu使用情况
        curused=0
        let curused+=$user
        let curused+=$nice
        curtotal=$curused
        let curtotal+=$sys
        let curtotal+=$iowait
        let curtotal+=$irq
        let curtotal+=$softirq
        let curtotal+=$idle

        # 计算delayms毫秒时间内的cpu平均使用率
        delta_used=0
        delta_total=0
        let delta_used=curused-preused
        let delta_total=curtotal-pretotal
        let usagerate=100*delta_used/delta_total

        # 记录当前cpu的使用率
        cpu_usagerate[$i]=$usagerate
        cpu_stat_user[$i]=$user
        cpu_stat_nice[$i]=$nice
        cpu_stat_sys[$i]=$sys
        cpu_stat_idle[$i]=$idle
        cpu_stat_iowait[$i]=$iowait
        cpu_stat_irq[$i]=$irq
        cpu_stat_softirq[$i]=$softirq

        let i+=1;
    done < /proc/stat

    # 打印各个cpu的使用率
    echo -n "[`date +'%F %T'`] "
    for i in ${cpu_index[@]}
    do
        echo -n "${cpu_name[$i]}=${cpu_usagerate[$i]}% "
    done
    echo
done
