#!/bin/bash

echo 0 > /sys/kernel/debug/tracing/tracing_on
sleep 1

echo "tracing_off"

echo 0 > /sys/kernel/debug/tracing/events/enable
sleep 1
echo "events disabled"

echo secondary_start_kernel > /sys/kernel/debug/tracing/set_ftrace_filter
sleep 1
echo "set_ftrace_filter init"

echo function_graph > /sys/kernel/debug/tracing/current_tracer
sleep 1
echo "function_graph tracer enabled"

echo bcm2835_mmc_irq > /sys/kernel/debug/tracing/set_ftrace_filter
sleep 1
echo bcm2835_mmc_tasklet_finish > /sys/kernel/debug/tracing/set_ftrace_filter
sleep 1
echo "set_ftrace_filter enabled"

echo 1 > /sys/kernel/debug/tracing/events/sched/sched_switch/enable
echo 1 > /sys/kernel/debug/tracing/events/sched/sched_wakeup/enable
sleep 1

echo 1 > /sys/kernel/debug/tracing/events/irq/irq_handler_entry/enable
echo 1 > /sys/kernel/debug/tracing/events/irq/irq_handler_exit/enable
echo "event enabled"

echo 1 > /sys/kernel/debug/tracing/tracing_on
echo "tracing_on"
