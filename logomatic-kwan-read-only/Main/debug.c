#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "fat16.h"
#include "partition.h"
#include "rootdir.h"
#include "sd_raw.h"
#include "stringex.h"

#ifdef DEBUG

struct fat16_file_struct debug_ouf;

int open_debug_core() {
  int result=root_open_new(&debug_ouf,"DEBUG.txt");
  if(result<0) return result;
  return debug_print("Logomatic Kwan v0.3 - build "__DATE__" "__TIME__"MDT\n");
}

int debug_print_core(char* s) {
  return fat16_write_file(&debug_ouf,s,strlen(s));
}

int debug_printDec_core(int d) {
  char buf[12];
  toDec(buf,d);
  return debug_print(buf);
}

int debug_printHex_core(unsigned int in, int len) {
  char buf[9];
  int hexit;
  for(int i=0;i<len;i++) {
    hexit=(in>>(4*(len-i-1))) & 0x0F;
    buf[i]=hexDigits[hexit];
  }
  buf[len]=0;
  return debug_print(buf);
}


void debug_flush_core() {
  sd_raw_sync();
}

void debug_printPartition_core(char* lead, struct partition_struct* partition) {
  debug_print(lead);debug_print("  device_read           = 0x");debug_printHex((unsigned int)partition->device_read,8);debug_print("\n");
  debug_print(lead);debug_print("  device_read_interval  = 0x");debug_printHex((unsigned int)partition->device_read_interval,8);debug_print("\n");
  debug_print(lead);debug_print("  device_write          = 0x");debug_printHex((unsigned int)partition->device_write,8);debug_print("\n");
  debug_print(lead);debug_print("  type                  = ");debug_printDec(partition->type);debug_print("\n");
  debug_print(lead);debug_print("  offset                = ");debug_printDec(partition->offset);debug_print("\n");
  debug_print(lead);debug_print("  length                = ");debug_printDec(partition->length);debug_print("\n");
}

void debug_printHeader_core(char* lead, struct fat16_header_struct* header) {
  debug_print(lead);debug_print("  size               = ");debug_printDec(header->size);debug_print("\n");
  debug_print(lead);debug_print("  fat_offset         = ");debug_printDec(header->fat_offset);debug_print("\n");
  debug_print(lead);debug_print("  fat_size           = ");debug_printDec(header->fat_size);debug_print("\n");
  debug_print(lead);debug_print("  sector_size        = ");debug_printDec(header->sector_size);debug_print("\n");
  debug_print(lead);debug_print("  cluster_size       = ");debug_printDec(header->cluster_size);debug_print("\n");
  debug_print(lead);debug_print("  root_dir_offset    = ");debug_printDec(header->root_dir_offset);debug_print("\n");
  debug_print(lead);debug_print("  cluster_zero_offset= ");debug_printDec(header->cluster_zero_offset);debug_print("\n");
}

void debug_printFS_core(char* lead, struct fat16_fs_struct* fs) {
  debug_print(lead);debug_print("  partition  = 0x");debug_printHex((unsigned int)fs->partition,8);debug_print("\n");
  char buf[20];
  strcpy(buf,lead);
  strcat(buf,"  ");
  debug_printPartition(buf,fs->partition);
  debug_printHeader(buf,&(fs->header));
}

void debug_printDE_core(char* lead, struct fat16_dir_entry_struct* de) {
  debug_print(lead);debug_print("  long_name          = ");debug_print(de->long_name);debug_print("\n");
  debug_print(lead);debug_print("  attr               = 0x");debug_printHex(de->attributes,2);debug_print("\n");
  debug_print(lead);debug_print("  cluster            = ");debug_printDec(de->cluster);debug_print("\n");
  debug_print(lead);debug_print("  file_size          = ");debug_printDec(de->file_size);debug_print("\n");
  debug_print(lead);debug_print("  entry_offset       = ");debug_printDec(de->entry_offset);debug_print("\n");
}

void debug_printFD_core(char* title, struct fat16_file_struct* fd) {
  debug_print(title);debug_print(" = 0x");debug_printHex((unsigned int)fd,8);debug_print("\n");
  debug_print("  magic_cookie = 0x");debug_printHex(fd->magic_cookie,8);debug_print("\n");
  debug_print("  fs           = 0x");debug_printHex((unsigned int)fd->fs,8);debug_print("\n");
  debug_printFS("  ",fd->fs);
  debug_print("  de           = 0x");debug_printHex((unsigned int)&fd->dir_entry,8);debug_print("\n");
  debug_printDE("  ",&fd->dir_entry);
  debug_print("  pos          = ");debug_printDec(fd->pos);debug_print("\n");
  debug_print("  pos_cluster  = ");debug_printDec(fd->pos_cluster);debug_print("\n");debug_flush();
}

#endif
