//--------------------------------------------------------------------
//
// C functions for enumerating Windows list of processes.
// Uses Win32 API and Microsoft Toolhelp library.
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
#include <windows.h>
#include <tlhelp32.h>
#include <winternl.h>
#include <psapi.h>
#include <stdio.h>

//
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
//
size_t proclist_find_parent_by_name(PROCLIST_ENTRY *list_ptr, size_t list_count, const wchar_t *name)
{
    if (!list_ptr || list_count < 1 || !name)
        return 0;

    for (size_t iproc = 0; iproc < list_count; ++iproc)
    {
        if (_wcsicmp(name, list_ptr[iproc].module_name) != 0)
            continue;

        if (list_ptr[iproc].parent_has_same_module_name)
            continue;

        return iproc;
    }

    return 0;
}

//
// Searches the process list for a process with the specified PID.
// Returns the list index of the process if successful, or zero if
// no matching process was fund. 
//
size_t proclist_find_by_pid(PROCLIST_ENTRY *list_ptr, size_t list_count, uint32_t pid)
{
    for (int index = 0; index < list_count; ++index)
    {
        if (list_ptr[index].process_id == pid)
            return index;
    }

    return 0;
}

//
// Retrieves the command line text for the given process handle.
// Returns true if successful, false if data not available.
//
static bool GetProcessCommandLine(HANDLE hProcess, wchar_t *cmd, size_t cmd_max)
{
    if (!hProcess || !cmd || !cmd_max)
        return false;

    memset(cmd, 0, cmd_max * sizeof(wchar_t));

    // Find the process environment block (PEB).
    // The 2nd element from NtQueryInformationProcess contains this.
    PVOID pinfo[6];
    if (NtQueryInformationProcess(hProcess, ProcessBasicInformation, pinfo,
                                  sizeof(PVOID) * _countof(pinfo), NULL))
        return false;

    PPEB ppeb = (PPEB)(pinfo[1]);
    if (!ppeb)
        return false;

   // Extract the PEB data from the process's memory.
   PEB pebData;
   BOOL result = ReadProcessMemory(hProcess, ppeb, &pebData, sizeof(PEB), NULL);
   RTL_USER_PROCESS_PARAMETERS params;
   result = ReadProcessMemory(hProcess, pebData.ProcessParameters,
                              &params,
                              sizeof(RTL_USER_PROCESS_PARAMETERS), NULL);

   // Copy the command line.
   USHORT bytes = params.CommandLine.Length;
   const USHORT max_bytes = (USHORT)(cmd_max * sizeof(wchar_t) - 1);
   if (bytes > max_bytes)
        bytes = max_bytes;
   result = ReadProcessMemory(hProcess,
                              params.CommandLine.Buffer,
                              cmd, bytes, NULL);

   return (result != 0);
}

//
// For the given Windows process ID, attempts to retrieve the
// process's image path/filename and the command line that was
// used to start the process.  Note that this information is
// not available for all processes.  Returns true if successful.
//
static bool process_get_filename_and_command_line(uint32_t process_id,
    wchar_t *filename, wchar_t filename_max,
    wchar_t *commandline, wchar_t commandline_max)
{
    if (!process_id)
        return false;
    if (filename && filename_max < 1)
        return false;
    if (commandline && commandline_max < 1)
        return false;

    if (filename)
        *filename = '\0';
    if (commandline)
        *commandline = '\0';

    HANDLE hprocess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                  FALSE, process_id);
    if (!hprocess)
        return false;

    wchar_t text[512] = {0};
    if (GetProcessImageFileNameW(hprocess, text, _countof(text)) > 0)
    {
        if (filename)
            _snwprintf_s(filename, filename_max, filename_max, L"%s", text);
    }

    GetProcessCommandLine(hprocess, commandline, commandline_max);

    CloseHandle(hprocess);
    return true;
}

//
// Collects information about currently running Windows processes
// and places them in a list in memory.  If successful, populates
// list_ptr and list_count.  The caller should discard the list
// later by calling proclist_release.
//
bool proclist_get(PROCLIST_ENTRY **list_ptr, size_t *list_count)
{
    if (!list_ptr || !list_count)
        return false;

    PROCLIST_ENTRY *list = NULL;
    size_t count = 0;
    PROCESSENTRY32W process = {0};
    process.dwSize = sizeof(process);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return false;

    // First build the list of processes in memory.
    for (BOOL found = Process32FirstW(snapshot, &process); found; found = Process32NextW(snapshot, &process))
    {
        PROCLIST_ENTRY info = {0};
        info.process_id = process.th32ProcessID;
        info.parent_id = process.th32ParentProcessID;
        info.num_threads = process.cntThreads;
        for (size_t ichar = 0; process.szExeFile[ichar]; ++ichar)
            info.module_name[ichar] = (wchar_t)process.szExeFile[ichar];

        process_get_filename_and_command_line(info.process_id,
            info.filename, sizeof(info.filename) / sizeof(wchar_t),
            info.commandline, sizeof(info.commandline) / sizeof(wchar_t));

        if (list == NULL)
        {
            list = calloc(1, sizeof(PROCLIST_ENTRY));
            *list = info;

        }
        else
        {
            list = realloc(list, (count + 1) * sizeof(PROCLIST_ENTRY));
            list[count] = info;
        }

        ++count;
    }

    CloseHandle(snapshot);
    *list_ptr = list;
    *list_count = count;

    // Count how many children each process has.
    for (size_t iparent = 0; iparent < count; ++iparent) {
        for (size_t ichild = 0; ichild < count; ++ichild) {
            if (iparent == ichild) {
                continue;
            }

            if (list[iparent].process_id == list[ichild].parent_id) {
                ++list[iparent].num_children;
            }
        }
    }

    // Determine if each process's parent has the same module name.
    for (size_t ichild = 0; ichild < count; ++ichild) {
        size_t parent_index = proclist_find_by_pid(list, count, list[ichild].parent_id);
        if (!parent_index) {
            continue;
        }

        if (_wcsicmp(list[ichild].module_name, list[parent_index].module_name) == 0)
            list[ichild].parent_has_same_module_name = true;
    }

    return true;
}

//
// Discards the information collected by a prior call to 
// proclist_get (see above).
//
void proclist_release(PROCLIST_ENTRY **list_ptr)
{
    if (!list_ptr)
        return;
    if (!*list_ptr)
        return;
    free(*list_ptr);
    *list_ptr = NULL;
}

