#include <stdio.h>
#include <string.h>

#include "debug.h"

extern unsigned int _end;

static int verbose = 0;

#define BUFFER_SIZE 1024
static char gdb_recv_buffer[BUFFER_SIZE];
static char gdb_send_buffer[BUFFER_SIZE];

static const char hexchars[] = "0123456789abcdef";

static int restore_breakpoint_after_step = -1;
static int restore_breakpoint_after_continue = -1;

static int hex (char ch) {
  if ((ch >= 'a') && (ch <= 'f'))
    return (ch - 'a' + 10);
  if ((ch >= '0') && (ch <= '9'))
    return (ch - '0');
  if ((ch >= 'A') && (ch <= 'F'))
    return (ch - 'A' + 10);
  return -1;
}

static char * getpacket(void) {
  unsigned char checksum;
  unsigned char xmitcsum;
  int count;
  char ch;

  while (1) {
    /* wait around for the start character, ignore all other characters */
    while ((ch = dbg_recv()) != '$')
      ;

    retry: checksum = 0;
    xmitcsum = -1;
    count = 0;

    /* now, read until a # or end of buffer is found */
    while (count < BUFFER_SIZE - 1) {
      ch = dbg_recv();
      if (ch == '$')
        goto retry;
      if (ch == '#')
        break;
      checksum = checksum + ch;
      gdb_recv_buffer[count] = ch;
      count = count + 1;
    }
    gdb_recv_buffer[count] = 0;

    if (ch == '#') {
      ch = dbg_recv();
      xmitcsum = hex(ch) << 4;
      ch = dbg_recv();
      xmitcsum += hex(ch);

      if (checksum != xmitcsum) {
        printf("gdb: wrong checksum !\n");
        dbg_send('-'); /* failed checksum */
      } else {
        dbg_send('+'); /* successful transfer */

        /* if a sequence char is present, reply the sequence ID */
        if (gdb_recv_buffer[2] == ':') {
          dbg_send(gdb_recv_buffer[0]);
          dbg_send(gdb_recv_buffer[1]);

          return &gdb_recv_buffer[3];
        }

        return gdb_recv_buffer;
      }
    }
  }
}

static void putpacket(char * buffer) {
  unsigned char checksum;
  int count;
  char ch;

  if (verbose)
    printf("gdb: sending : %s\n", buffer);
  do {
    dbg_send('$');
    checksum = 0;
    count = 0;

    while ((ch = buffer[count])) {
      dbg_send(ch);
      checksum += ch;
      count += 1;
    }

    dbg_send('#');
    dbg_send(hexchars[checksum >> 4]);
    dbg_send(hexchars[checksum % 16]);
    dbg_flush();
  } while (dbg_recv() != '+');
}

char * write_register(char * buffer, int reg) {
  int i;
  for(i=0; i<4; i++) {
    *buffer++ = hexchars[(reg >> (8*i + 4)) & 0xf];
    *buffer++ = hexchars[(reg >> (8*i)) & 0xf];
  }
  return buffer;
}

int hex2int(char **ptr, int *intValue) {
  int numChars = 0;
  int hexValue;

  *intValue = 0;

  while (**ptr) {
    hexValue = hex(**ptr);
    if (hexValue >= 0) {
      *intValue = (*intValue << 4) | hexValue;
      numChars++;
    } else
      break;

    (*ptr)++;
  }

  return (numChars);
}

static volatile int phase = 0;

char * mem2hex(char *mem, char *buf, int count, int may_fault) {
  int i, j;
  unsigned char ch;

  if(mem == 0 || mem >= &_end) {
    if(count == 4) {
      j = *((unsigned int*)mem);
      for(i = 0; i < 4; i++) {
        *buf++ = hexchars[(j >> 4) & 0xf];
        *buf++ = hexchars[j & 0xf];
        j >>= 8;
      }
    } else {
      for (i = 0; i < count; i++) {
        ch = 0;
        *buf++ = hexchars[ch >> 4];
        *buf++ = hexchars[ch % 16];
      }
    }
  }
  else {
    for (i = 0; i < count; i++) {
      ch = *mem++;
      *buf++ = hexchars[ch >> 4];
      *buf++ = hexchars[ch % 16];
    }
  }
  *buf = 0;
  
  //dna_printf("answer: %s\n", buf - (count * 2));

  return (buf);
}

void gdb_converse(unsigned int trapno, struct ia32_regs *t) {
  char * buf;
  int addr, length;

  if(trapno == EXC_BP) {
    t->eip -= 1; // restoring return address for iret (because breakpoint instruction (0xCC) execution incremented it by 1)
    if (verbose)
      printf("gdb: trapped on breakpoint @ 0x%08x\n", t->eip);
  }
  else if(trapno == EXC_DB) {
    if(restore_breakpoint_after_continue != -1) {
      // this is a special step, to avoid infinite loop when resuming on a breakpoint
      unmask_breakpoint(restore_breakpoint_after_continue);
      restore_breakpoint_after_continue = -1;
      t->eflags &= ~0x100; // continue
      goto end;
    }
    if(restore_breakpoint_after_step != -1) {
      unmask_breakpoint(restore_breakpoint_after_step);
      restore_breakpoint_after_step = -1;
    }
  }

  if(phase == 0) {
    t->eip += 1; // jumping over breakpoint
    phase = 1;
  }

  if (verbose)
    printf("gdb: current eip is 0x%08x\n", t->eip);

  sti();
  putpacket("T05thread:01;");

  for(;;) {
    char * ptr = getpacket();
    if (verbose)
      dna_printf("gdb: received : %s\n", ptr);
    gdb_send_buffer[0] = '\0';
    switch(*ptr++) {
    case 'q':
      if(strncmp(ptr, "Supported:", 10) == 0) {
        strcpy(gdb_send_buffer, "PacketSize=1000");
      }
      else if(strncmp(ptr, "fThreadInfo", 11) == 0) {
        strcpy(gdb_send_buffer, "m1");
      }
      else if(strncmp(ptr, "sThreadInfo", 11) == 0) {
        strcpy(gdb_send_buffer, "l");
      }
      else if(strncmp(ptr, "ThreadExtraInfo,1", 17) == 0) {
        strcpy(gdb_send_buffer, "4350552330205b72756e6e696e675d");
      }
      else if(*ptr == 'C') {
        strcpy(gdb_send_buffer, "QC1");
      }
      break;
    case 'H':
      if(strncmp(ptr, "g0", 2) == 0) {
        strcpy(gdb_send_buffer, "OK");
      } else if(strncmp(ptr, "c-1", 2) == 0) {
        strcpy(gdb_send_buffer, "OK");
      }
      break;
    case '?':
      strcpy(gdb_send_buffer, "T05thread:01;");
      break;
    case 'g':
      buf = gdb_send_buffer;
      // order found in linux kernel sources : arch/ia32/include/asm/kgdb.h
      buf = write_register(buf, t->eax);
      buf = write_register(buf, t->ecx);
      buf = write_register(buf, t->edx);
      buf = write_register(buf, t->ebx);
      buf = write_register(buf, t->esp);
      buf = write_register(buf, t->ebp);
      buf = write_register(buf, t->esi);
      buf = write_register(buf, t->edi);
      buf = write_register(buf, t->eip);
      buf = write_register(buf, t->eflags);
      buf = write_register(buf, t->cs);
      buf = write_register(buf, t->ss);
      buf = write_register(buf, t->ds);
      buf = write_register(buf, t->es);
      buf = write_register(buf, t->fs);
      buf = write_register(buf, t->gs);

      break;
    case 'm':
      if (hex2int(&ptr, &addr))
        if (*(ptr++) == ',')
          if (hex2int(&ptr, &length)) {
            ptr = 0;
            mask_all_breakpoints();
            mem2hex((char *) addr, gdb_send_buffer, length, 1);
            unmask_all_breakpoints();
          }

      if (ptr) {
        strcpy(gdb_send_buffer, "E01");
      }
      break;
    case 'Z':
      if(*ptr++ == '0') {
        if(*ptr++ == ',') {
          hex2int(&ptr, &addr);
          if(*ptr++ == ',') {
            hex2int(&ptr, &length);
            if(length != 1) {
              dna_printf("panic: length != 1 ...");
              while(1);
            }
            add_breakpoint(addr, NULL);
            strcpy(gdb_send_buffer, "OK");
          }
        }
      }
      break;
    case 'z':
      if(*ptr++ == '0') {
        if(*ptr++ == ',') {
          hex2int(&ptr, &addr);
          if(*ptr++ == ',') {
            hex2int(&ptr, &length);
            if(length != 1) {
              dna_printf("gdb: length != 1 ...");
              while(1);
            }
            remove_breakpoint(addr);
            strcpy(gdb_send_buffer, "OK");
          }
        }
      }
      break;
    case 'v':
      if(strncmp(ptr, "Cont", 4) == 0) {
        ptr += 4;
        if(*ptr == '?') {
          strcpy(gdb_send_buffer, "vCont;c;C;s;S");
        }
        else if(*ptr == ';') {
          ptr++;
          if(*ptr == 's') {
            // if instruction is a breakpoint, we have to remove it, step, and restore it when step is done
            if(*((unsigned char *)t->eip) == INSN_BREAKPOINT) {
              mask_breakpoint(t->eip);
              restore_breakpoint_after_step = t->eip;
            }
            t->eflags |= 0x100; // step
            goto end;
          }
          else if(*ptr == 'c') {
            // if instruction is a breakpoint, we have to remove it, step, restore it, and then continue
            if(*((unsigned char *)t->eip) == INSN_BREAKPOINT) {
              mask_breakpoint(t->eip);
              restore_breakpoint_after_continue = t->eip;
              t->eflags |= 0x100; // step
            }
            else {
              t->eflags &= ~0x100; // continue
            }
            phase = 2;
            goto end;
          }
        }
      }
      break;
    case 'T':
      if(*ptr == '1') {
        strcpy(gdb_send_buffer, "OK");
      }
      break;
    }
    putpacket(gdb_send_buffer);
  }

  end:

  cli();

  return;
}
