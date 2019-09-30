#include "cpu/instr.h"
#include"memory/memory.h"

make_instr_func(cli)
{
    int len=1;
    cpu.eflags.IF=0;
    return len;
}

make_instr_func(sti)
{
    int len=1;
    cpu.eflags.IF=1;
    return len;
}