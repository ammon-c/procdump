//--------------------------------------------------------------------
//
// Program to print a list of Windows processes to stdout.
//
// If run with no command line arguments, prints all processes.
// If command line arguments are given, only processes with
// matching module names are printed (any substring counts as
// a match).
//
//--------------------------------------------------------------------
//
// (C) Copyright 2019,2024 Ammon R. Campbell.
//
// I wrote this code for use in my own educational and experimental
// programs, but you may also freely use it in yours as long as you
// abide by the following terms and conditions:
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above
//     copyright notice, this list of conditions and the following
//     disclaimer in the documentation and/or other materials
//     provided with the distribution.
//   * The name(s) of the author(s) and contributors (if any) may not
//     be used to endorse or promote products derived from this
//     software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
// OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.  IN OTHER WORDS, USE AT YOUR OWN RISK, NOT OURS.  
//
//--------------------------------------------------------------------

#include "winproclist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static bool dump_processes(const wchar_t *match_pattern)
{
    PROCLIST_ENTRY *list = NULL;
    size_t count = 0;
    size_t printed = 0;

    if (!proclist_get(&list, &count))
    {
        printf("ERROR: Failed getting process list!\n");
        return false;
    }

    if (match_pattern)
    {
        printf("\nLooking for processes matching \"%S\"\n", match_pattern);
        fflush(stdout);
    }

    for (size_t i = 0; i < count; ++i)
    {
        if (match_pattern && match_pattern[0] &&
            wcsstr(list[i].module_name, match_pattern) == NULL)
        {
            continue;
        }

        printf("%zu: \"%S\"", i, list[i].module_name);
        printf("  pid:%u", list[i].process_id);
        printf("  parent:%u", list[i].parent_id);
        printf("  children:%zu", list[i].num_children);
        printf("  threads:%zu", list[i].num_threads);
        if (list[i].filename[0])
        {
            printf("\n");
            printf("          filename: \"%S\"", list[i].filename);
        }
        if (list[i].commandline[0])
        {
            printf("\n");
            printf("          command: \"%S\"", list[i].commandline);
        }
        printf("\n");

        printed++;
    }

    printf("\nNumber of matching processes:  %zu of %zu\n\n", printed, count);

    proclist_release(&list);
    return true;
}

int wmain(int argc, wchar_t **argv)
{
    if (argc < 2)
    {
        if (!dump_processes(NULL))
            return EXIT_FAILURE;
        return EXIT_SUCCESS;
    }

    for (int iarg = 1; iarg < argc; iarg++)
    {
        if (!dump_processes(argv[iarg]))
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

