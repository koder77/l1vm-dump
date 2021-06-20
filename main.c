/*
 * This file main.c is part of l1vm-dump-byte-data.
 *
 * (c) Copyright Stefan Pietzonke (jay-t@gmx.net), 2020
 *
 * L1vm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * L1vm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with L1vm.  If not, see <http://www.gnu.org/licenses/>.
 */

//  l1vm-dump-byte-data
//
//

#include "include/global.h"

// show host system type on compile time
#if __linux__
	#pragma message ("Linux host detected!")
#endif
#if _WIN32
	#pragma message ("Windows host detected!")
#endif

// protos
S2 load_object (U1 *name);
void free_modules (void);
size_t strlen_safe (const char * str, int maxlen);

S8 data_size ALIGN;
S8 code_size ALIGN;

// see global.h user settings on top
S8 max_code_size ALIGN = MAX_CODE_SIZE;
S8 max_data_size ALIGN = MAX_DATA_SIZE;

S8 data_mem_size ALIGN;
S8 stack_size ALIGN = STACKSIZE;		// stack size added to data size when dumped to object file

// code
U1 *code = NULL;

// data
U1 *data = NULL;


S8 code_ind ALIGN;
S8 data_ind ALIGN;
S8 modules_ind ALIGN = -1;    // no module loaded = -1

S8 cpu_ind ALIGN = 0;

S8 max_cpu ALIGN = MAXCPUCORES;    // number of threads that can be runned

U1 silent_run = 0;				// switch startup and status messages of: "-q" flag on shell

typedef U1* (*dll_func)(U1 *sp, U1 *sp_top, U1 *sp_bottom, U1 *data);

struct data_info data_info[MAXDATAINFO];
S8 data_info_ind ALIGN = -1;

// pthreads data mutex
pthread_mutex_t data_mutex;

// return code of main thread
S8 retcode ALIGN = 0;

// shell arguments
U1 shell_args[MAXSHELLARGS][MAXSHELLARGLEN];
S4 shell_args_ind = -1;

void cleanup (void)
{
	if (data) free (data);
	if (code) free (code);
}

void show_info (void)
{
	printf ("l1vm-dump-byte-data <program-bytecode>\n");
	printf (" (C) 2020 Stefan Pietzonke\n");
}

int main (int ac, char *av[])
{
	// do compilation time sense check on integer 64 bit and double 64 bit type!!
	S8 size_int64 ALIGN;
	S8 size_double64 ALIGN;
	size_int64 = sizeof (S8);
	if (size_int64 != 8)
	{
		printf ("FATAL compiler ERROR: size of S8 not 8 bytes (64 bit!): %lli bytes only!!\n", size_int64);
		cleanup ();
		exit (1);
	}

	size_double64 = sizeof (F8);
	if (size_double64 != 8)
	{
		printf ("FATAL compiler ERROR: size of F8 not 8 bytes (64 bit!): %lli bytes only!!\n", size_double64);
		cleanup ();
		exit (1);
	}

	if (ac < 2)
	{
		show_info ();
		cleanup ();
		exit (1);
	}
	else
 	{
		if (ac == 2)
		{
			if (strcmp (av[1], "-?") == 0)
			{
				show_info ();
				cleanup ();
				exit (1);
			}
			if (strcmp (av[1], "--help") == 0)
			{
				show_info ();
				cleanup ();
				exit (1);
			}
		}
	}

	if (load_object ((U1 *) av[1]))
	{
		cleanup ();
		exit (1);
	}

	cleanup ();
	exit (0);
}
