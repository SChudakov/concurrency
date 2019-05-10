//
// Created by semen on 25.04.19.
//

#include <linux/module.h>
//#include <linux/init.h>

//int main() {
//    asm(
//    "push %eax\n\t"
//    "movl %cr0,%eax\n\t"
//    "orl $0x40000000,%eax\n\t"  /* Set CD */
//    "movl %eax,%cr0\n\t"
//    "wbinvd\n\t"
//    "pop  %eax\n\t");
//    return 0;
//}