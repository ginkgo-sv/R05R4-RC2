#include <stdio.h>
#include <user-offsets.h>

#define SHOW(name) printf("#define %s %d\n", #name, name)

int main(int argc, char **argv)
{
	printf("/* Automatically generated by "
	       "arch/um/kernel/skas/util/mk_ptregs */\n");
	printf("\n");
	printf("#ifndef __SKAS_PT_REGS_\n");
	printf("#define __SKAS_PT_REGS_\n");
	printf("\n");
	SHOW(HOST_FRAME_SIZE);
	SHOW(HOST_FP_SIZE);
	SHOW(HOST_XFP_SIZE);

	SHOW(HOST_IP);
	SHOW(HOST_SP);
	SHOW(HOST_EFLAGS);
	SHOW(HOST_EAX);
	SHOW(HOST_EBX);
	SHOW(HOST_ECX);
	SHOW(HOST_EDX);
	SHOW(HOST_ESI);
	SHOW(HOST_EDI);
	SHOW(HOST_EBP);
	SHOW(HOST_CS);
	SHOW(HOST_SS);
	SHOW(HOST_DS);
	SHOW(HOST_FS);
	SHOW(HOST_ES);
	SHOW(HOST_GS);

	printf("\n");
	printf("#endif\n");
	return(0);
}

/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * Emacs will notice this stuff at the end of the file and automatically
 * adjust the settings for this buffer only.  This must remain at the end
 * of the file.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-file-style: "linux"
 * End:
 */
