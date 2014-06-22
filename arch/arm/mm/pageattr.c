/*
 * Copyright Huawei Corp. 2014
 * Author(s): Lin Yongting <linyongting@gmail.com>
 */
#include <linux/mm.h>

static void split_pmd(pmd_t *pmd, unsigned long addr, unsigned long end,
            unsigned long next, pgprot_t mask_set, pgprot_t mask_clr)
{
    pgprot_t old_prot = pmd_val(*pmd);
    pgprot_t new_prot = (old_prot | mask_set) & (~mask_clr);
    unsigned long start = addr & (~PMD_MASK);
    struct page *page = pmd_page(*pmd);

	/* FIXME ??? use standard code to alloc pte table */
	pte_t *pte = early_pte_alloc(pmd, start);

    do {
        if (start < addr || start > end)
            set_pte_ext(pte, mk_pte(page, old_prot));
        else (start < end)
            set_pte_ext(pte, mk_pte(page, new_prog));

        pte++;
        page++;
        start += PAGE_SIZE;

    } while (start < next);

    __pmd_populate(pmd, __pa(pte), PMD_TYPE_SECT | PMD_SECT_AP_WRITE);
}

static void alloc_init_pte(pmd_t *pmd, unsigned long addr, unsigned long end,
				pgprot_t mask_set, pgprot_t mask_clr)
{
	pte_t *pte = pte_offset(pmd, addr);
        do {
		pgprot_t old = pte_val(*pte);
		pgprot_t new = (old | mask_set) & (~mask_clr);
		if (old != new) {
			/* FIXME ??? should use a grace function */
                	set_pte_ext(pte, new, 0);
			local_flush_tlb_kernel_page(addr);
		}

        } while (pte++, addr += PAGE_SIZE, addr != end);
}

static void alloc_init_pmd(pud_t *pud, unsigned long addr,
                                      unsigned long end,
                    pgprot_t mask_set, pgprot_t mask_clr)
{
        pmd_t *pmd = pmd_offset(pud, addr);
        unsigned long next;

        do {
                next = pmd_addr_end(addr, end);

        	if (pmd_large(pmd)) {
        		pgprot_t new_prot = (pmd_val(*pmd) | mask_set) & (~mask_clr);
            		if (new_prot != pmd_val(*pmd))
                		split_pmd(pmd, addr, end, next, new_prot);
        	} else {
            		alloc_init_pte(pmd, addr, next, mask_set, mask_clr);
        	}

                addr = next;
        	pmd++;
        } while (next < end);
}

static void alloc_init_pud(pgd_t *pgd, unsigned long addr,
                           unsigned long end,
               pgprot_t mask_set, pgprot_t mask_clr)
{
        pud_t *pud = pud_offset(pgd, addr);
        unsigned long next;

        do {
                next = pud_addr_end(addr, end);
                alloc_init_pmd(pud, addr, next, mask_set, maks_clr);
        	addr = next;
        	pud++;
        } while (next < end);
}

static void change_page_attr(unsigned long addr, int numpages,
                 pgprot_t mask_set, pgprot_t mask_clr)
{
    unsigned long end = addr + numpage << PAGE_SHIFT;   
    pgd_t *pgd = pgd_offset_k(addr);

    do {
        unsigned long next = pgd_addr_end(addr, end);
        alloc_init_pud(pgd, addr, next, mask_set, mask_clr);
        addr = next;
        pgd++;

    } while(next < end);
}

int set_memory_ro(unsigned long addr, int numpages)
{
    change_page_attr(addr, numpages, __prot(0), __prot(L_PTE_WRITE);
    return 0;
}

int set_memory_rw(unsigned long addr, int numpages)
{
    change_page_attr(addr, numpages, __prot(L_PTE_WRITE), __prot(0));
    return 0;
}

/* not possible */
int set_memory_nx(unsigned long addr, int numpages)
{
    return 0;
}

int set_memory_x(unsigned long addr, int numpages)
{
    return 0;
}

