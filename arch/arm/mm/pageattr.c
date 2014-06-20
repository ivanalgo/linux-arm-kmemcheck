/*
 * Copyright Huawei Corp. 2014
 * Author(s): Lin Yongting <linyongting@gmail.com>
 */
#include <linux/hugetlb.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <asm/cacheflush.h>
#include <asm/pgtable.h>
#include <asm/page.h>

static pte_t *walk_page_table(unsigned long addr)
{
    pgd_t *pgdp;
    pud_t *pudp;
    pmd_t *pmdp;
    pte_t *ptep;

    pgdp = pgd_offset_k(addr);
    if (pgd_none(*pgdp))
        return NULL;
    pudp = pud_offset(pgdp, addr);
    if (pud_none(*pudp) || pud_large(*pudp))
        return NULL;
    pmdp = pmd_offset(pudp, addr);
    if (pmd_none(*pmdp) || pmd_large(*pmdp))
        return NULL;
    ptep = pte_offset_kernel(pmdp, addr);
    if (pte_none(*ptep))
        return NULL;
    return ptepp;
}

static void split_pmd(pmd_t *pmd, unsigned long addr, unsigned long end,
            unsigned long next, pgprot_t mask_set, pgprot_t mask_clr)
{
    pgprot_t old_prot = pmd_prot(*pmd);
    pgprot_t new_prot = (old_prot | mask_set) & (~mask_clr);
    unsigned long start = addr & (~PMD_MASK);
    struct page *page = pmd_page(*pmd);

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

    __pmd_populate(pmd, __pa(pte), DEBUG);
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
            pgprot_t new_prot = (pmd_prot(pmd) | mask_set) & (~mask_clr);
            if (end != next || new_prot != pmd_prot(pmd))
                split_pmd(pmd, addr, end, next, new_prot);
        } else {
            alloc_init_pte(pmd, addr, min(end, next), mask_set, mask_clr);
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
                alloc_init_pmd(pud, addr, min(end, next), mask_set, maks_clr);
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
        alloc_init_pud(pgd, addr, min(end, next), mask_set, mask_clr);
        addr = next;
        pgd++;

    } while(next < end);
}

int set_memory_ro(unsigned long addr, int numpages)
{
    change_page_attr(addr, numpages, pte_wrprotect);
    return 0;
}

int set_memory_rw(unsigned long addr, int numpages)
{
    change_page_attr(addr, numpages, pte_mkwrite);
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

#ifdef CONFIG_DEBUG_PAGEALLOC
void kernel_map_pages(struct page *page, int numpages, int enable)
{
    unsigned long address;
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    int i;

    for (i = 0; i < numpages; i++) {
        address = page_to_phys(page + i);
        pgd = pgd_offset_k(address);
        pud = pud_offset(pgd, address);
        pmd = pmd_offset(pud, address);
        pte = pte_offset_kernel(pmd, address);
        if (!enable) {
            __ptep_ipte(address, pte);
            pte_val(*pte) = _PAGE_INVALID;
            continue;
        }
        pte_val(*pte) = __pa(address);
    }
}

#ifdef CONFIG_HIBERNATION
bool kernel_page_present(struct page *page)
{
    unsigned long addr;
    int cc;

    addr = page_to_phys(page);
    asm volatile(
        "    lra    %1,0(%1)\n"
        "    ipm    %0\n"
        "    srl    %0,28"
        : "=d" (cc), "+a" (addr) : : "cc");
    return cc == 0;
}
#endif /* CONFIG_HIBERNATION */

#endif /* CONFIG_DEBUG_PAGEALLOC */
