
#include "types.h"
#include "buddy.h"

#define MAX_SWAPFILES      32
#define VM_FAULT_OOM       0x0001
#define FAULT_FLAG_WRITE   0x01     /* Fault was a write access */
#define VM_FAULT_MAJOR     0x0004


struct swap_info_struct {
    unsigned long flags;        // ����������״̬��ʶ
    signed short prio;          // ��������������
    signed char type;           // �������������ͻ��������е�λ��
    unsigned int max;           // �����������������ҳ��
    unsigned char *swap_map;    // ÿ��ҳ�����ü�������
    unsigned int lowest_bit;    // ��һ�����õĲ�λ
    unsigned int highest_bit;   // ���һ�����õĲ�λ
    unsigned int pages;         // ���������п��õ���ҳ��
    unsigned int inuse_pages;   // ��ǰ��ռ�õ�ҳ��
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