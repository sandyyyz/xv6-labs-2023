
#include "types.h"
#include "buddy.h"

#define MAX_SWAPFILES      32
#define VM_FAULT_OOM       0x0001
#define FAULT_FLAG_WRITE   0x01     /* Fault was a write access */
#define VM_FAULT_MAJOR     0x0004


struct swap_info_struct {
    unsigned long flags;        // 交换分区的状态标识
    signed short prio;          // 交换分区的优先
    signed char type;           // 交换分区的类型或在数组中的位置
    unsigned int max;           // 交换分区包含的最大页数
    unsigned char *swap_map;    // 每个页的引用计数数组
    unsigned int lowest_bit;    // 第一个可用的槽位
    unsigned int highest_bit;   // 最后一个可用的槽位
    unsigned int pages;         // 交换分区中可用的总页数
    unsigned int inuse_pages;   // 当前被占用的页数
};

typedef struct {
	unsigned long val;
} swp_entry_t;

struct vm_fault {
    struct vm_area_struct *vma;
	unsigned int flags;
	//gfp_t gfp_mask;
	//pgoff_t pgoff;
	unsigned long address;
	//pmd_t *pmd;
	//pud_t *pud;	
	pte_t orig_pte;
	//struct page *cow_page;
	//struct mem_cgroup *memcg;
	//struct page *page;
	pte_t *pte;	
	//spinlock_t *ptl;
	//pgtable_t prealloc_pte;
};


struct vm_area_struct {
    pgprot_t vm_page_prot;
    struct mm_struct *vm_mm; /* The address space we belong to. */
};


typedef struct { 
    unsigned long pte; 
} pte_t;


typedef struct { 
    unsigned long pgprot; 
} pgprot_t;

struct mm_struct {
    struct vm_area_struct *mmp;
};