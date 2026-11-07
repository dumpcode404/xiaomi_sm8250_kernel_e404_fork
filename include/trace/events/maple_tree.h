/* SPDX-License-Identifier: GPL-2.0 */
/*
 * 4.19 backport stub for Maple Tree tracepoints.
 *
 * Upstream lib/maple_tree.c emits trace_ma_op() / trace_ma_write()
 * via <trace/events/maple_tree.h> (TRACE_EVENTs). A full TRACE_EVENT
 * port is future work; for the backport these expand to no-ops so the
 * tree compiles and runs without tracing support. All call sites in
 * lib/maple_tree.c are statement-position logging only.
 */
#ifndef _TRACE_MAPLE_TREE_H
#define _TRACE_MAPLE_TREE_H

#define trace_ma_op(...)		do { } while (0)
#define trace_ma_read(...)		do { } while (0)
#define trace_ma_write(...)		do { } while (0)

#endif /* _TRACE_MAPLE_TREE_H */
