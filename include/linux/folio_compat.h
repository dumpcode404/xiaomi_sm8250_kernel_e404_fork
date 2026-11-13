/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_FOLIO_COMPAT_H
#define _LINUX_FOLIO_COMPAT_H

#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/swap.h>
#include <linux/memcontrol.h>
#include <linux/pagemap.h>

/*
 * Folio compat shim: 4.19 backport of the mainline stable 6.6 folio API
 * subset used by the folio-struct PoC (zram first, core MM next).
 *
 * This tree (4.19) has no struct folio yet. For the order-0 pages the
 * PoC deals with, folio == page 1:1, with struct page as the first
 * (only) member, so page_folio() is a plain cast. kmap_local_folio() /
 * kunmap_local() map to kmap_atomic() / kunmap_atomic() here; callers
 * must use offset 0 / full-page mappings only and must not nest beyond
 * the existing bounce-buffer discipline.
 *
 * LRU/memcg wrapper names use a folio_* prefix and are PoC-local;
 * mainline 6.6 spells several of these differently (e.g. folio_add_lru()
 * exists there, while charging goes through mem_cgroup_charge() /
 * folio_memcg()). The mapping underneath is what matters for the PoC.
 */

struct folio {
	struct page page;
};

static inline struct folio *page_folio(struct page *page)
{
	return (struct folio *)page;
}

static inline struct page *folio_page(struct folio *folio, unsigned long n)
{
	return &folio->page + n;
}

static inline struct folio *folio_alloc(gfp_t gfp, unsigned int order)
{
	struct page *page = alloc_pages(gfp, order);

	return page ? page_folio(page) : NULL;
}

static inline void folio_put(struct folio *folio)
{
	put_page(&folio->page);
}

static inline void *folio_address(struct folio *folio)
{
	return page_address(&folio->page);
}

static inline void *kmap_local_folio(struct folio *folio, size_t offset)
{
	return kmap_atomic(&folio->page) + offset;
}

static inline void kunmap_local(const void *addr)
{
	kunmap_atomic((void *)addr);
}

static inline void flush_dcache_folio(struct folio *folio)
{
	flush_dcache_page(&folio->page);
}

/*
 * LRU wrappers: 6.6 folio API spelling over the 4.19 page-based
 * mm/swap.c primitives.
 */
static inline void folio_add_lru(struct folio *folio)
{
	lru_cache_add(&folio->page);
}

static inline void folio_add_lru_vma(struct folio *folio,
				     struct vm_area_struct *vma)
{
	lru_cache_add_active_or_unevictable(&folio->page, vma);
}

static inline void folio_activate(struct folio *folio)
{
	activate_page(&folio->page);
}

static inline void folio_mark_accessed(struct folio *folio)
{
	mark_page_accessed(&folio->page);
}

static inline void folio_mark_lazyfree(struct folio *folio)
{
	mark_page_lazyfree(&folio->page);
}

static inline void folio_rotate_reclaimable(struct folio *folio)
{
	rotate_reclaimable_page(&folio->page);
}

static inline void folio_deactivate(struct folio *folio)
{
	deactivate_page(&folio->page);
}

static inline void folio_deactivate_file(struct folio *folio)
{
	deactivate_file_page(&folio->page);
}

static inline void folio_add_lru_anon(struct folio *folio)
{
	lru_cache_add_anon(&folio->page);
}

/*
 * XArray-folio API: 6.6 looks swap/page cache slots up as folios.
 * find_get_page() performs the XArray lookup under RCU (xa_load path)
 * and returns the resident page with a ref held (NULL for miss or for
 * shadow-only slots); page_folio() then presents it as a folio.
 * xa_lock-protected stores/erases stay at the converted call sites in
 * mm/swap_state.c, where the xas locking discipline is preserved.
 */
static inline struct folio *filemap_get_folio(struct address_space *mapping,
					      pgoff_t index)
{
	struct page *page = find_get_page(mapping, index);

	if (!page)
		return NULL;
	return page_folio(page);
}

/*
 * Memcg wrappers: 6.6 charges folios; here the charge still lands on
 * the underlying page. Signatures mirror the 4.19 page-based
 * mem_cgroup_*() calls (which also carry static-inline stubs for
 * !CONFIG_MEMCG, so these wrappers compile in both configurations).
 */
static inline int folio_memcg_try_charge(struct folio *folio,
					 struct mm_struct *mm,
					 gfp_t gfp_mask,
					 struct mem_cgroup **memcgp,
					 bool compound)
{
	return mem_cgroup_try_charge(&folio->page, mm, gfp_mask,
				     memcgp, compound);
}

static inline void folio_memcg_commit_charge(struct folio *folio,
					     struct mem_cgroup *memcg,
					     bool lrucare, bool compound)
{
	mem_cgroup_commit_charge(&folio->page, memcg, lrucare, compound);
}

static inline void folio_memcg_cancel_charge(struct folio *folio,
					      struct mem_cgroup *memcg,
					      bool compound)
{
	mem_cgroup_cancel_charge(&folio->page, memcg, compound);
}

static inline void folio_memcg_uncharge(struct folio *folio)
{
	mem_cgroup_uncharge(&folio->page);
}

#endif /* _LINUX_FOLIO_COMPAT_H */
