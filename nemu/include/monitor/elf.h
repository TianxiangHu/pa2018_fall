#ifndef __ELF_H__
#define __ELF_H__

#include "nemu.h"

void load_elf_tables(char * exec_file);
uint32_t look_up_fun_symtab(char *sym, bool *success);
uint32_t look_up_symtab(char *sym, bool *success);
const char* find_fun_name(uint32_t eip);

#endif
