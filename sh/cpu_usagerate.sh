#!/bin/bash

#
# File:   neiku/sh/cpu_usagerate.sh
# Brief:  show cpu usage rate.
# Author: Hobby <ku7d@qq.com>
# Date:   2015/11/10
# Usage:  cpu_usagerate.sh [--delay-ms=DELAYMS]
# Algorithm: cpu usage rate = (user+nice+sys+iowait+irq+softird) / (user+nice+sys+iowait+irq+softird + idle) * 100%
# More:   /proc/stat
# Require: bash >= 4.0.0
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
declare -A cpu_usagerate
declare -A cpu_name
declare -A cpu_stat_user
declare -A cpu_stat_nice
declare -A cpu_stat_sys
declare -A cpu_stat_idle
declare -A cpu_stat_iowait
declare -A cpu_stat_irq
declare -A cpu_stat_softirq
#declare -A cpu_stat_steal
#declare -A cpu_stat_guest

# 初始记录当前cpu数据
i=0
while read name user nice sys idle iowait irq softirq steal guest
do
    # 只使用cpu数据
    if ! expr match $name "^cpu" >/dev/null 2>&1; then
        break;
    fi
    
    # 记录每个cpu的当前状态
    let i+=1;
    cpu_name[$i]=$name
    cpu_usagerate[$name]=0    
    cpu_stat_user[$name]=$user
    cpu_stat_nice[$name]=$nice
    cpu_stat_sys[$name]=$sys
    cpu_stat_idle[$name]=$idle
    cpu_stat_iowait[$name]=$iowait
    cpu_stat_irq[$name]=$irq
    cpu_stat_softirq[$name]=$softirq
done < /proc/stat

# 每隔delayms毫秒，计算每个cpu的使用率
while usleep $((show_delay_ms*1000))
do
    while read name user nice sys idle iowait irq softirq steal guest
    do
        # 只使用cpu数据
        if ! expr match $name "^cpu" >/dev/null 2>&1; then
            break;
        fi

        # 上次cpu使用情况
        preused=0
        let preused+=cpu_stat_user[$name]
        let preused+=cpu_stat_nice[$name]
        let preused+=cpu_stat_sys[$name]
        let preused+=cpu_stat_iowait[$name]
        let preused+=cpu_stat_irq[$name]
        let preused+=cpu_stat_softirq[$name]
        pretotal=0
        let pretotal+=cpu_stat_user[$name]
        let pretotal+=cpu_stat_nice[$name]
        let pretotal+=cpu_stat_sys[$name]
        let pretotal+=cpu_stat_idle[$name]
        let pretotal+=cpu_stat_iowait[$name]
        let pretotal+=cpu_stat_irq[$name]
        let pretotal+=cpu_stat_softirq[$name]

        # 当前cpu使用情况
        curused=0
        let curused+=$user
        let curused+=$nice
        let curused+=$sys
        let curused+=$iowait
        let curused+=$irq
        let curused+=$softirq
        curtotal=0
        let curtotal+=$user
        let curtotal+=$nice
        let curtotal+=$sys
        let curtotal+=$idle
        let curtotal+=$iowait
        let curtotal+=$irq
        let curtotal+=$softirq

        # 计算delayms毫秒时间内的cpu平均使用率
        delta_used=0
        delta_total=0
        let delta_used=curused-preused
        let delta_total=curtotal-pretotal
        let usagerate=100*delta_used/delta_total

        # 记录当前cpu的使用率
        cpu_usagerate[$name]=$usagerate
        cpu_stat_user[$name]=$user
        cpu_stat_nice[$name]=$nice
        cpu_stat_sys[$name]=$sys
        cpu_stat_idle[$name]=$idle
        cpu_stat_iowait[$name]=$iowait
        cpu_stat_irq[$name]=$irq
        cpu_stat_softirq[$name]=$softirq
    done < /proc/stat

    # 打印各个cpu的使用率
    echo -n "[`date +'%F %T'`] "
    for name in ${cpu_name[@]}
    do
        echo -n "$name=${cpu_usagerate[$name]}% "
    done
    echo
done
