#include <stdio.h>
#include <dpmi.h>

void get_memory_stats() {
    unsigned long dpmi_page_size;

    __dpmi_free_mem_info dpmi_free_mem;

    __dpmi_get_page_size(&dpmi_page_size);
    __dpmi_get_free_memory_information(&dpmi_free_mem);

    unsigned int available = (dpmi_free_mem.total_number_of_free_pages * dpmi_page_size) / 1024;
    unsigned int total = (dpmi_free_mem.total_number_of_physical_pages * dpmi_page_size) / 1024;

    printf("Available Memory ..... %uKB\n", available);
    printf("Total Memory ......... %uKB\n", total);
}

int main() {
    get_memory_stats();
    return 0;
}