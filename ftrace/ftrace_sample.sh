#!/bin/bash
echo 0 > /sys/kernel/debug/tracing/tracing_on
sleep 1
echo "tracing_off" 



echo 0 > /sys/kernel/debug/tracing/events/enable
sleep 1
echo "events disabled"



echo  my_fb_irq_handler > /sys/kernel/debug/tracing/set_ftrace_filter	
sleep 1
echo "set_ftrace_filter init"



echo function > /sys/kernel/debug/tracing/current_tracer
sleep 1
echo "function tracer enabled"



echo 1 > /sys/kernel/debug/tracing/events/sched/sched_switch/enable
sleep 1
echo 1 > /sys/kernel/debug/tracing/events/sched/sched_wakeup/enable
sleep 1
echo 1 > /sys/kernel/debug/tracing/events/irq/irq_handler_entry/enable
sleep 1
echo 1 > /sys/kernel/debug/tracing/events/irq/irq_handler_exit/enable
sleep 1
echo "event enabled"



echo 1 > /sys/kernel/debug/tracing/options/func_stack_trace
sleep 1
echo 1 > /sys/kernel/debug/tracing/options/sym-offset
echo "function stack trace enabled"



echo 1 > /sys/kernel/debug/tracing/tracing_on
echo "tracing_on"
