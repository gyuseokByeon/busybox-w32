/* vi: set sw=4 ts=4: */
/*
 * Mini ps implementation(s) for busybox
 *
 * Copyright (C) 1999,2000 by Lineo, inc. and Erik Andersen  
 * Copyright (C) 1999,2000,2001 by Erik Andersen <andersee@debian.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "busybox.h"

static const int TERMINAL_WIDTH = 79;      /* not 80 in case terminal has linefold bug */



extern int ps_main(int argc, char **argv)
{
	procps_status_t * p;
	int i, len;
#ifdef CONFIG_FEATURE_AUTOWIDTH
	struct winsize win = { 0, 0, 0, 0 };
	int terminal_width = TERMINAL_WIDTH;
#else
#define terminal_width  TERMINAL_WIDTH
#endif


#ifdef CONFIG_FEATURE_AUTOWIDTH
		ioctl(fileno(stdout), TIOCGWINSZ, &win);
		if (win.ws_col > 0)
			terminal_width = win.ws_col - 1;
#endif

	printf("  PID  Uid     VmSize Stat Command\n");
	while ((p = procps_scan(1)) != 0) {
		char *namecmd = p->cmd;

		if(p->rss == 0)
			len = printf("%5d %-8s        %s ", p->pid, p->user, p->state);
		else
			len = printf("%5d %-8s %6ld %s ", p->pid, p->user, p->rss, p->state);
		i = terminal_width-len;

		if(namecmd != 0 && namecmd[0] != 0) {
			if(i < 0)
		i = 0;
			if(strlen(namecmd) > i)
				namecmd[i] = 0;
			printf("%s\n", namecmd);
		} else {
			namecmd = p->short_cmd;
			if(i < 2)
				i = 2;
			if(strlen(namecmd) > (i-2))
				namecmd[i-2] = 0;
			printf("[%s]\n", namecmd);
		}
		free(p->cmd);
	}
	return EXIT_SUCCESS;
}

