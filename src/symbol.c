#include "symbol.h"
#include "operate.h"
#include <stdlib.h>
#include <string.h>

static Symbol root_symbol = {NULL, 0, 0, ""};
static Symbol* symbol_table = &root_symbol;

Symbol* add_symbol(ULong name_offset, ULong name_length, ULong address,
                   UWord type) {
  Symbol* new_symbol = (Symbol*)malloc(sizeof(Symbol) + name_length);
  if (new_symbol == NULL) {
    return NULL;  // メモリ確保失敗
  }

  new_symbol->address = address;
  new_symbol->type = type;
  char* name_buf = malloc(name_length + 1);
  if (name_buf == NULL) {
    free(new_symbol);
    return NULL;  // メモリ確保失敗
  }
  memcpy(name_buf,
         (char*)GetReadableMemorySuper(name_offset, name_length).bufptr,
         name_length);
  name_buf[name_length] = '\0';
  strcpy(new_symbol->name, name_buf);
  free(name_buf);

  new_symbol->next = symbol_table->next;
  symbol_table->next = new_symbol;

  return new_symbol;
}

Symbol* find_symbol_by_address(ULong address) {
  Symbol* current = symbol_table->next;
  while (current != NULL) {
    if (current->address == address) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

Symbol* find_symbol_by_name(const char* name) {
  Symbol* current = symbol_table->next;
  while (current != NULL) {
    if (strcmp(current->name, name) == 0) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

void dump_symbol_table(void) {
  Symbol* current = symbol_table->next;
  while (current != NULL) {
    printFmt("%08X %04X %s\n", current->address, current->type, current->name);
    current = current->next;
  }
}
