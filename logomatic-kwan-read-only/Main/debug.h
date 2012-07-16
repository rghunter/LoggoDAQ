#ifndef debug_h
#define debug_h

//#define DEBUG
#ifdef DEBUG

#include "fat16.h"

#define open_debug() open_debug_core() 
#define debug_print(s) debug_print_core(s)
#define debug_printDec(d) debug_printDec_core(d)
#define debug_printHex(d,l) debug_printHex_core(d,l)
#define debug_flush() debug_flush_core()
#define debug_printPartition(l,p) debug_printPartition_core((l),(p))
#define debug_printHeader(l,h) debug_printHeader_core((l),(h))
#define debug_printFS(l,f) debug_printFS_core((l),(f))
#define debug_printDE(l,d) debug_printDE_core((l),(d))
#define debug_printFD(t,f) debug_printFD_core((t),(f))

extern struct fat16_file_struct debug_ouf;

int open_debug_core(void);
int debug_print_core(char* s);
int debug_printDec_core(int d);
int debug_printHex_core(unsigned int d, int len);
void debug_flush_core(void);

//Fat16 debugging stuff
void debug_printPartition_core(char* lead, struct partition_struct* partition);
void debug_printHeader_core(char* lead, struct fat16_header_struct* header);
void debug_printFS_core(char* lead, struct fat16_fs_struct* fs);
void debug_printDE_core(char* lead, struct fat16_dir_entry_struct* de);
void debug_printFD_core(char* title, struct fat16_file_struct* fd);

#else

#define open_debug() 0
#define debug_print(s) 0
#define debug_printDec(d) 0
#define debug_printHex(d,l) 0
#define debug_flush()

#define debug_printPartition(l,p)
#define debug_printHeader(l,h)
#define debug_printFS(l,f)
#define debug_printDE(l,d)
#define debug_printFD(t,f)

#endif


#endif
