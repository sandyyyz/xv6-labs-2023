
#include "types.h"
#include "buddy.h"

#define MAX_SWAPFILES 32



struct swap_info_struct {
    unsigned long flags; // ����������״̬��ʶ
    signed short prio; // ��������������
    signed char type; // �������������ͻ��������е�λ��
    unsigned int max; // �����������������ҳ��
    unsigned char *swap_map; // ÿ��ҳ�����ü�������
    unsigned int lowest_bit; // ��һ�����õĲ�λ
    unsigned int highest_bit; // ���һ�����õĲ�λ
    unsigned int pages; // ���������п��õ���ҳ��
    unsigned int inuse_pages; // ��ǰ��ռ�õ�ҳ��
};

typedef struct {
	unsigned long val;
} swp_entry_t;