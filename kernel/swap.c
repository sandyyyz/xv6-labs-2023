
#include "lru.h"
#include "types.h"
#include "list.h"
#include "page.h"
#include "swap.h"

static struct swap_info_struct swap_info[MAX_SWAPFILES];

/* use the shrink_page_list part to implement the swap mechanism */
static unsigned long shrink_page_list(struct list_head *page_list) {

    /* only LRU_INACTIVE_LIST lists are considered */
    while(!list_empty(page_list)) {
        struct page *page;

        if(page->in_swap_cache)
            goto reclaim;
        /* remove pages from the tail of the LRU list */
        page = lru_to_page(page_list);
        _list_del(&page->lru);

        /* add the page to slot */
        add_to_swap(page_list);

        reclaim:
            /* delete the page from swap cache and reclaim it from memory */
            page->in_swap_cache = 0;

    }
}


int add_to_swap(struct page *page)
{
    swp_entry_t entry;
    int err;

    /* check if lock is held */

    entry = get_swap_page(page);
    /* no swap space is allocated, the function returns 0 */
    if(!entry.val)
        return 0;

    err = add_to_swap_cache(page, entry);
    if(err)
        goto fail;

    /* Mark the page as dirty to ensure that changes to the page 
     * will eventually be written back to the swap space
     */
    set_page_dirty(page);
    return 1;

fail:
    /* to free the swap space previously allocated for the page */
    put_swap_page(page, entry);
    return 0;
}


static inline swp_entry_t get_swap_page(struct page *page)
{
	swp_entry_t entry;
    /* swap_info */
    struct swap_info_struct *si;
    unsigned long offset;
    int type;

    /* Try to find a swap space and corresponding slot for the page */
    type = get_swap_device(page, &si, &offset);
    if (type < 0) {
        entry.val = 0;
        return entry;
    }

    /* Write page contents to swap slot */
    if (swap_write_page(si, offset, page) < 0) {
        entry.val = 0;
        return entry;
    }

    /* Update entry to locate this slot
     * type is the exchange type obtained through the get_swap_device function
     * offset is the offset of the page in the swap area
     */
    entry.val = swp_entry(type, offset);

    page->PTE = entry.val;

	return entry;
}

int set_page_dirty(struct page *page)
{
    page->dirty = 1;
}



/* swap pages into slot */
inline int swap_write_page(struct swap_info_struct *si, unsigned long offset, struct page *page)
{
    /* the slot is currently occupied */
    si->swap_map[offset] = 1;

    /* The kernel writes the contents of the page to the selected slot in the swap area. 
     * This involves writing to disk
     */

}


inline int swp_entry(int type, unsigned long offset)
{
    return 0 + (type << 2) + (offset << 8);
}


int add_to_swap_cache(struct page *page, swp_entry_t entry)
{
    page->in_swap_cache = 1;

    return 0;
}


static unsigned long do_swap_page()
{
    struct page *page;
    swp_entry_t entry;
    unsigned long offset;
    int type;

    /* reassign new pages */

    /* read back the data corresponding to this virtual address from the swap partition */
    entry = pte_to_swp_entry();
}