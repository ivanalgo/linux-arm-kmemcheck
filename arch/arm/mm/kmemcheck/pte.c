#include <linux/mm.h>
#include <asm/pgalloc.h>

#include "pte.h"

/*
 * Lookup the page table entry for a virtual address. Return a pointer
 * to the entry and the level of the mapping.
 *
 * Note: We return pud and pmd either when the entry is marked large
 * or when the present bit is not set. Otherwise we would return a
 * pointer to a nonexisting mapping.
 */
pte_t *lookup_address(unsigned long address)
{
	pgd_t *pgd = pgd_offset_k(address);
	pud_t *pud;
	pmd_t *pmd;

	if (pgd_none(*pgd))
		return NULL;

	pud = pud_offset(pgd, address);
	if (pud_none(*pud))
		return NULL;

	pmd = pmd_offset(pud, address);
	if (pmd_none(*pmd) || !pmd_present(*pmd) || pmd_large(*pmd))
		return NULL;

	return pte_offset_kernel(pmd, address);
}
EXPORT_SYMBOL_GPL(lookup_address);

pte_t *kmemcheck_pte_lookup(unsigned long address)
{
	pte_t *pte;

	pte = lookup_address(address);
	if (!pte)
		return NULL;

	if (!pte_hidden(*pte))
		return NULL;

	return pte;
}

