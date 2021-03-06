/* Any assmbly language/system dependent hacks needed to setup boot1.c so it
 * will work as expected and cope with whatever platform specific wierdness is
 * needed for this architecture.
 */

asm("" \
"	.section .text..SHmedia32,\"ax\"\n"				\
"	.globl _dl_boot\n"						\
"	.type _dl_boot, @function\n"					\
"	.align 5\n"							\
"_dl_boot:\n"								\
"	! Set r12 to point to GOT\n"					\
"	movi	(((datalabel _GLOBAL_OFFSET_TABLE_-(.LZZZ3-.)) >> 16) & 65535), r12\n"	\
"	shori	((datalabel _GLOBAL_OFFSET_TABLE_-(.LZZZ3-.)) & 65535), r12\n"		\
".LZZZ3:\n"								\
"	ptrel/u	r12, tr0\n"						\
"	gettr	tr0, r12	! GOT address\n"			\
"	add	r18, r63, r11	! save return address - needed?\n"	\
"	add	r15, r63, r2	! arg = stack pointer\n"		\
"	pt	_dl_boot2, tr0	! should work even if PIC\n"		\
"	blink	tr0, r18	! call _dl_boot2 - user EP is in r2\n"	\
);

#define DL_BOOT(X)   static void __attribute_used__ _dl_boot2 (X)

/*
 * Get a pointer to the argv array.  On many platforms this can be just
 * the address if the first argument, on other platforms we need to
 * do something a little more subtle here.
 */
#define GET_ARGV(ARGVP, ARGS) ARGVP = ((unsigned long *)ARGS)

/*
 * Here is a macro to perform a relocation.  This is only used when
 * bootstrapping the dynamic loader.  RELP is the relocation that we
 * are performing, REL is the pointer to the address we are relocating.
 * SYMBOL is the symbol involved in the relocation, and LOAD is the
 * load address.
 */

/*
 * We need to do this stupidity here as the preprocessor will choke when
 * SYMTAB is NULL if we do this in PERFORM_BOOTSTRAP_RELOC().
 */

#include <elf.h>

static inline int __extract_lsb_from_symtab(Elf32_Sym *symtab)
{
	static int lsb = 0;

	/* Check for SHmedia/SHcompact */
	if (symtab)
		lsb = symtab->st_other & 4;

	return lsb;
}

/*
 * While on the subject of stupidity, there appear to be some conflicts with
 * regards to several relocation types as far as binutils is concerned
 * (Barcelona and Madrid both appear to use an out of date elf.h, whereas
 * native Catalonia has all of the necessary definitions. As a workaround,
 * we'll just define them here for sanity..
 */
#ifndef R_SH_RELATIVE_LOW16
#  define R_SH_RELATIVE_LOW16		197
#  define R_SH_RELATIVE_MEDLOW16	198
#  define R_SH_IMM_LOW16		246
#  define R_SH_IMM_LOW16_PCREL		247
#  define R_SH_IMM_MEDLOW16		248
#  define R_SH_IMM_MEDLOW16_PCREL	249
#endif

#define PERFORM_BOOTSTRAP_RELOC(RELP,REL,SYMBOL,LOAD,SYMTAB)		\
	const unsigned int r_type = ELF32_R_TYPE((RELP)->r_info);	\
	int lsb = __extract_lsb_from_symtab(SYMTAB);			\
									\
	switch (r_type)	{						\
	case R_SH_REL32:						\
		*(REL)  = (SYMBOL) + (RELP)->r_addend			\
			    - (unsigned long)(REL);			\
		break;							\
	case R_SH_DIR32:						\
	case R_SH_GLOB_DAT:						\
	case R_SH_JMP_SLOT:						\
		*(REL)  = ((SYMBOL) + (RELP)->r_addend) | lsb;		\
		break;							\
	case R_SH_RELATIVE:						\
		*(REL)  = (LOAD) + (RELP)->r_addend;			\
		break;							\
	case R_SH_RELATIVE_LOW16:					\
	case R_SH_RELATIVE_MEDLOW16:					\
	{								\
		unsigned long word, value;				\
									\
		word = (unsigned long)(REL) & ~0x3fffc00;		\
		value = (LOAD) + (RELP)->r_addend;			\
									\
		if (r_type == R_SH_RELATIVE_MEDLOW16)			\
			value >>= 16;					\
									\
		word |= (value & 0xffff) << 10;				\
		*(REL)	= word;						\
		break;							\
	}								\
	case R_SH_IMM_LOW16:						\
	case R_SH_IMM_MEDLOW16:						\
	{								\
		unsigned long word, value;				\
									\
		word = (unsigned long)(REL) & ~0x3fffc00;		\
		value = ((SYMBOL) + (RELP)->r_addend) | lsb;		\
									\
		if (r_type == R_SH_IMM_MEDLOW16)			\
			value >>= 16;					\
									\
		word |= (value & 0xffff) << 10;				\
		*(REL)	= word;						\
		break;							\
	}								\
	case R_SH_IMM_LOW16_PCREL:					\
	case R_SH_IMM_MEDLOW16_PCREL:					\
	{								\
		unsigned long word, value;				\
									\
		word = (unsigned long)(REL) & ~0x3fffc00;		\
		value = (SYMBOL) + (RELP)->r_addend			\
			  - (unsigned long)(REL);			\
									\
		if (r_type == R_SH_IMM_MEDLOW16_PCREL)			\
			value >>= 16;					\
									\
		word |= (value & 0xffff) << 10;				\
		*(REL)	= word;						\
		break;							\
	}								\
	case R_SH_NONE:							\
		break;							\
	default:							\
		SEND_STDERR("BOOTSTRAP_RELOC: unhandled reloc type ");	\
		SEND_NUMBER_STDERR(ELF32_R_TYPE((RELP)->r_info), 1);	\
		SEND_STDERR("REL, SYMBOL, LOAD: ");			\
		SEND_ADDRESS_STDERR(REL, 0);				\
		SEND_STDERR(", ");					\
		SEND_ADDRESS_STDERR(SYMBOL, 0);				\
		SEND_STDERR(", ");					\
		SEND_ADDRESS_STDERR(LOAD, 1);				\
		_dl_exit(1);						\
	}

/*
 * Transfer control to the user's application, once the dynamic loader
 * is done.  This routine has to exit the current function, then
 * call the _dl_elf_main function.
 */

#define START()   return _dl_elf_main;


