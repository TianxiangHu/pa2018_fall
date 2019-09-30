#include "cpu/instr.h"
#include "device/port_io.h"

make_instr_func(in_b)
{
    int len=1;
    uint16_t port_addr=cpu.edx & 0xFFFF;
    size_t data_len= 1;
    uint32_t data= pio_read(port_addr,data_len);
    cpu.eax=data;
    return len;
}

make_instr_func(in_v)
{
    int len=1;
    uint16_t port_addr=cpu.edx & 0xFFFF;
    size_t data_len= data_size/8;
    uint32_t data= pio_read(port_addr,data_len);
    cpu.eax=data;
    return len;
}

make_instr_func(out_b)
{
    int len=1;
    uint16_t port_addr=cpu.edx & 0xFFFF;
    size_t data_len=  1;
    uint32_t data=cpu.eax;
    pio_write(port_addr,data_len,data);
    return len;
}

make_instr_func(out_v)
{
    int len=1;
    uint16_t port_addr=cpu.edx & 0xFFFF;
    size_t data_len=  data_size/8;
    uint32_t data=cpu.eax;
    pio_write(port_addr,data_len,data);
    return len;
}