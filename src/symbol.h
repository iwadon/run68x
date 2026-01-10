#if !defined(SYMBOL_H)
#define SYMBOL_H

#include "m68k.h"

typedef struct Symbol {
  struct Symbol* next;
  ULong address;
  UWord type;
  char name[1];
} Symbol;

Symbol* add_symbol(ULong name_offset, ULong name_length, ULong address, UWord type);
Symbol* find_symbol_by_address(ULong address);
Symbol* find_symbol_by_name(const char* name);
void dump_symbol_table(void);

#endif
