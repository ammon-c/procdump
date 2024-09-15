//--------------------------------------------------------------------
//
// C functions for enumerating Windows list of processes.
// Uses Win32 API and Microsoft Toolhelp library.
//
// Usage:
// * Call proclist_get to collect the list of processes from Windows.
// * Access elements of the list by indexing them directly
//   (e.g. my_list[x]).
// * Call any of the proclist_find_XXX functions to search the list
//   for a particular process.
// * Call proclist_release to discard the list when done with it.
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

#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Details about a single process from the process list.
typedef struct
{
    uint32_t    process_id;
    uint32_t    parent_id;
    wchar_t     module_name[512];
    size_t      num_children;
    bool        parent_has_same_module_name;
    size_t      num_threads;
    wchar_t     filename[512];
    wchar_t     commandline[4096];
} PROCLIST_ENTRY;

// Collects information about currently running Windows processes
// and places them in a list in memory.  If successful, populates
// list_ptr and list_count.  The caller should discard the list
// later by calling proclist_release.
bool proclist_get(PROCLIST_ENTRY **list_ptr, size_t *list_count);

// Discards the information collected by a prior call to 
// proclist_get (see above).
void proclist_release(PROCLIST_ENTRY **list_ptr);

// Searches the process list for a process with the specified PID.
// Returns the list index of the process if successful, or zero if
// no matching process was fund. 
size_t proclist_find_by_pid(PROCLIST_ENTRY *list_ptr, size_t list_count, uint32_t pid);

// Searches the process list for a process that meets both of these
// conditions:
//
// 1.  The process's module name matches the given module name.
// 2.  The process's parent has a different module name (so if there
//     are child processes with the same name, the one returned is
//     the parent process).
//
// Returns the list index of the matching process if successful, or
// zero if no matching process was fund.
size_t proclist_find_parent_by_name(PROCLIST_ENTRY *list_ptr, size_t list_count, const wchar_t *name);

#ifdef __cplusplus
}
#endif

