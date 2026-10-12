/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
 */

#undef TRACE_SYSTEM
#define TRACE_SYSTEM dcvsh

#if !defined(_TRACE_DCVSH_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_DCVSH_H

#include <linux/tracepoint.h>

TRACE_EVENT(dcvsh_freq,
	TP_PROTO(unsigned long cpu, unsigned long freq),

	TP_ARGS(cpu, freq),

	TP_STRUCT__entry(
		__field(unsigned long, cpu)
		__field(unsigned long, freq)
	),

	TP_fast_assign(
		__entry->cpu = cpu;
		__entry->freq = freq;
	),

	TP_printk("cpu:%lu max frequency:%lu", __entry->cpu, __entry->freq)
);

TRACE_EVENT(dcvsh_throttle,

	TP_PROTO(unsigned long cpu, bool state),

	TP_ARGS(cpu, state),

	TP_STRUCT__entry(
		__field(unsigned long, cpu)
		__field(bool, state)
	),

	TP_fast_assign(
		__entry->cpu = cpu;
		__entry->state = state;
	),

	TP_printk("cpu:%lu throttle_%s",
		__entry->cpu,
		__entry->state ? "begin" : "end")
);

#endif /* _TRACE_DCVSH_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
