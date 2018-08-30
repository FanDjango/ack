#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include "emu.h"

#define RAM_BASE 0x10000000
#define RAM_TOP  0x10100000

#define BRK_TOP (RAM_TOP - 0x1000)

#define INIT_SP RAM_TOP
#define INIT_PC 0x08000054

#define EXIT_PC 0xdeaddead

/* Read/write macros */
#define READ_BYTE(BASE, ADDR) (BASE)[ADDR]
#define READ_WORD(BASE, ADDR) (((BASE)[ADDR]<<8) |			\
							  (BASE)[(ADDR)+1])
#define READ_LONG(BASE, ADDR) (((BASE)[ADDR]<<24) |			\
							  ((BASE)[(ADDR)+1]<<16) |		\
							  ((BASE)[(ADDR)+2]<<8) |		\
							  (BASE)[(ADDR)+3])

#define WRITE_BYTE(BASE, ADDR, VAL) (BASE)[ADDR] = (VAL)&0xff
#define WRITE_WORD(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>8) & 0xff;		\
									(BASE)[(ADDR)+1] = (VAL)&0xff
#define WRITE_LONG(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>24) & 0xff;		\
									(BASE)[(ADDR)+1] = ((VAL)>>16)&0xff;	\
									(BASE)[(ADDR)+2] = ((VAL)>>8)&0xff;		\
									(BASE)[(ADDR)+3] = (VAL)&0xff


static void emulated_syscall(void);

static unsigned char ram[RAM_TOP - RAM_BASE];
uint32_t brkbase = RAM_BASE;
uint32_t brkpos = RAM_BASE;
uint32_t entrypoint = RAM_BASE;

void fatal(char* fmt, ...)
{
	static bool guard = false;

	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "fatal: ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);

	if (!guard)
	{
		guard = true;
		dump_state(stderr);
	}

	exit(EXIT_FAILURE);
}

static uint32_t transform_address(uint32_t address)
{
	uint32_t a = address - RAM_BASE;
	if (a >= (RAM_TOP-RAM_BASE))
		fatal("address 0x%x out of bounds", address);
	return a;
}

uint64_t read_double(uint32_t address)
{
	return ((uint64_t)read_long(address+0) << 32) | read_long(address+4);
}

uint32_t read_long(uint32_t address)
{
	uint32_t v = READ_LONG(ram, transform_address(address));
	#if 0
	fprintf(stderr, "read 0x%08x: 0x%08x\n", address, v);
	#endif
	return v;
}

uint32_t read_word(uint32_t address)
{
	return READ_WORD(ram, transform_address(address));
}

uint32_t read_byte(uint32_t address)
{
	return READ_BYTE(ram, transform_address(address));
}

void write_byte(uint32_t address, uint32_t value)
{
	WRITE_BYTE(ram, transform_address(address), value);
}

void write_word(uint32_t address, uint32_t value)
{
	WRITE_WORD(ram, transform_address(address), value);
}

void write_long(uint32_t address, uint32_t value)
{
	#if 0
	fprintf(stderr, "write 0x%08x: 0x%08x\n", address, value);
	#endif
	WRITE_LONG(ram, transform_address(address), value);
}

void write_double(uint32_t address, uint64_t value)
{
	write_long(address+0, value>>32);
	write_long(address+4, value);
}

void system_call(uint8_t trapno)
{
	cpu.cr &= ~(1<<28); /* reset summary overflow (for success) */
	switch (cpu.gpr[0])
	{
		case 1: /* exit */
			exit(cpu.gpr[3]);

		case 4: /* write */
		{
			int fd = cpu.gpr[3];
			uint32_t address = cpu.gpr[4];
			uint32_t len = cpu.gpr[5];
			void* ptr = ram + transform_address(address);
			transform_address(address+len); /* bounds check */
			cpu.gpr[3] = write(fd, ptr, len);
			if (cpu.gpr[3] == -1)
				goto error;
			break;
		}

		case 45: /* brk */
		{
			uint32_t newpos = cpu.gpr[3];
			if (newpos == 0)
				cpu.gpr[3] = brkpos;
			else if ((newpos < brkbase) || (newpos >= BRK_TOP))
				cpu.gpr[3] = -ENOMEM;
			else
			{
				brkpos = newpos;
				cpu.gpr[3] = 0;
			}
			break;
		}

		case 20: /* getpid */
		case 48: /* signal */
		case 54: /* ioctl */
		case 67: /* sigaction */
		case 78: /* gettimeofday */
		case 126: /* sigprocmask */
			cpu.gpr[3] = 0;
			break;

		error:
			cpu.gpr[3] = errno;
			cpu.cr |= (1<<28); /* set summary overflow (for failure) */
			return;

		default:
			fatal("unimplemented system call %d", cpu.gpr[0]);
	}
}

static void load_program(FILE* fd)
{
	fseek(fd, 0, SEEK_SET);
	if (fread(ram, 1, 0x34, fd) != 0x34)
		fatal("couldn't read ELF header");
	
	uint32_t phoff = READ_LONG(ram, 0x1c);
	uint16_t phentsize = READ_WORD(ram, 0x2a);
	uint16_t phnum = READ_WORD(ram, 0x2c);
	entrypoint = READ_LONG(ram, 0x18);
	if ((phentsize != 0x20) || (phnum != 1))
		fatal("unsupported ELF file");

	fseek(fd, phoff, SEEK_SET);
	if (fread(ram, 1, phentsize, fd) != phentsize)
		fatal("couldn't read program header");

	uint32_t offset = READ_LONG(ram, 0x04);
	uint32_t vaddr = READ_LONG(ram, 0x08);
	uint32_t filesz = READ_LONG(ram, 0x10);
	uint32_t memsz = READ_LONG(ram, 0x14);
	brkbase = brkpos = vaddr + memsz;

	uint32_t vaddroffset = transform_address(vaddr);
	transform_address(vaddr + memsz); /* bounds check */
	memset(ram+vaddroffset, 0, memsz);
	fseek(fd, offset, SEEK_SET);
	if (fread(ram+vaddroffset, 1, filesz, fd) != filesz)
		fatal("couldn't read program data");
}

/* The main loop */
int main(int argc, char* argv[])
{
	if(argc != 2)
		fatal("syntax: emuppc <program file>");

	FILE* fd = fopen(argv[1], "rb");
	if (!fd)
		fatal("Unable to open %s", argv[1]);
	load_program(fd);
	fclose(fd);

	/* On entry, the Linux stack looks like this.
	 * 
	 * sp+..           NULL
     * sp+8+(4*argc)   env (X quads)
     * sp+4+(4*argc)   NULL
     * sp+4            argv (argc quads)
     * sp              argc
	 *
	 * We'll set it up with a bodgy stack frame with argc=0 just to keep the
	 * startup code happy.
	 */

	{
		uint32_t sp = INIT_SP;
		write_long(sp -= 4, 0);
		uint32_t envp = sp;
		write_long(sp -= 4, envp);
		write_long(sp -= 4, 0);
		unsigned long argv = sp;
		write_long(sp -= 4, argv);
		write_long(sp -= 4, 0);
		cpu.gpr[1] = sp;
		cpu.cia = entrypoint;
	}

	cpu.lr = EXIT_PC;
	while (cpu.cia != EXIT_PC)
	{
		#if 0
		dump_state(stderr);
		#endif
		single_step();
	}

	return 0;
}

