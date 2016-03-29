#!/bin/bash

#
# File:   neiku/sh/cpu_info.sh
# Brief:  show cpu info.
# Author: Hobby <ku7d@qq.com>
# Date:   2015/11/15
# Usage:  cpu_info.sh
# More:   /proc/cpuinfo
# 

cpu_name=`cat /proc/cpuinfo | grep "model name" | head -1 | cut -d: -f2 | cut -c 2-`
physical_cpu_num=`cat /proc/cpuinfo | grep "physical id" | sort -u | wc -l`
logical_cpu_num=`cat /proc/cpuinfo | grep "processor" | sort -u | wc -l`
cpu_core_num=`cat /proc/cpuinfo | grep "cpu cores" | head -1 | cut -f2 -d: | tr -d ' '`
cpu_siblings_num=`cat /proc/cpuinfo | grep "siblings" | head -1 | cut -f2 -d: | tr -d ' '`
cpu_cache_size_num=`cat /proc/cpuinfo | grep "cache size" | head -1 | cut -f2 -d: | tr -d ' '`
isenableht="unknow"


echo "           cpu name: $cpu_name"
echo "physical cpu number: $physical_cpu_num"
echo " logical cpu number: $logical_cpu_num"
echo "    cpu core number: $cpu_core_num"
echo "cpu siblings number: $cpu_siblings_num"
echo "     cpu cache size: $cpu_cache_size_num"
if ((physical_cpu_num*cpu_core_num < logical_cpu_num)) ; then
    isenableht="yes"
else
    isenableht="no"
fi
echo "      is enabled ht: $isenableht"
