/*
 * (C) Copyright 2000-2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Misc boot support
 */
#include <common.h>
#include <command.h>
#include <net.h>

#ifdef CONFIG_CMD_GO

/* Allow ports to override the default behavior */
__attribute__((weak))
unsigned long do_go_exec(ulong (*entry)(int, char * const []), int argc,
				 char * const argv[])
{
	return entry (argc, argv);
}

unsigned long do_go_kernel(ulong (*entry)(int, int, int, int), int dtb_addr, int res_r1, int res_r2, int res_r3)
{
	return entry(dtb_addr, res_r1, res_r2, res_r3);
}

static int do_go(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	ulong	addr, rc;
	ulong	dtb_addr;
	int     rcode = 0;

	if (argc < 2)
		return CMD_RET_USAGE;

	addr = simple_strtoul(argv[1], NULL, 16);
	dtb_addr = simple_strtoul(argv[2], NULL, 16);
	printf ("switch to el2\n");
	dcache_disable();
	armv8_switch_to_el2();
	printf ("switch to el1\n");
	armv8_switch_to_el1();	

	printf ("## Starting application at 0x%08lX ...\n", addr);
	/*
	 * pass address parameter as argv[0] (aka command name),
	 * and all remaining args
	 */
	//rc = do_go_exec ((void *)addr, argc - 1, argv + 1);
	rc = do_go_kernel ((void *)addr, dtb_addr, 0, 0, 0);
	if (rc != 0) rcode = 1;

	printf ("## Application terminated, rc = 0x%lX\n", rc);
	return rcode;
}

/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	go, CONFIG_SYS_MAXARGS, 1,	do_go,
	"start application at address 'addr'",
	"addr [arg ...]\n    - start application at address 'addr'\n"
	"      passing 'arg' as arguments"
);

#endif

U_BOOT_CMD(
	reset, 1, 0,	do_reset,
	"Perform RESET of the CPU",
	""
);

#ifdef CONFIG_CMD_POWEROFF
U_BOOT_CMD(
	poweroff, 1, 0,	do_poweroff,
	"Perform POWEROFF of the device",
	""
);
#endif
