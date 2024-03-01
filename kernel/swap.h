
#include "types.h"
#include "buddy.h"

#define MAX_SWAPFILES 32



struct swap_info_struct {
    unsigned long flags; // 交换分区的状态标识
    signed short prio; // 交换分区的优先
    signed char type; // 交换分区的类型或在数组中的位置
    unsigned int max; // 交换分区包含的最大页数
    unsigned char *swap_map; // 每个页的引用计数数组
    unsigned int lowest_bit; // 第一个可用的槽位
    unsigned int highest_bit; // 最后一个可用的槽位
    unsigned int pages; // 交换分区中可用的总页数
    unsigned int inuse_pages; // 当前被占用的页数
};

typedef struct {
	unsigned long val;
} swp_entry_t;