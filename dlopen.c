/*
dlopen.c - DLL support code

written by 2002 dbjh


This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifdef __APPLE__  // macosx supports dlopen directly.
#define __unix__
#endif

#ifdef  DJGPP
#include <sys/dxe.h>
#elif   defined __unix__                       // also defined under Cygwin (and DJGPP)
#include <dlfcn.h>
#elif   defined _WIN32
#include <windows.h>
#elif   defined __BEOS__
#include <image.h>
#include <Errors.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef  DJGPP
#include "dxedll_pub.h"
#include "map.h"


#define INITIAL_HANDLE 1
static st_map_t *dxe_map;

void
uninit_func (void)
{
  fprintf (stderr, "An uninitialized member of the import/export structure was called!\n"
                   "Update dlopen.c/open_module().\n");
  exit (1);
}
#endif


void *
open_module (char *module_name)
{
#ifdef  DJGPP
  static int new_handle = INITIAL_HANDLE;
  void *handle;
  int n, m;
  st_symbol_t *sym = _dxe_load (module_name);
  /*
    _dxe_load() doesn't really return a handle. It returns a pointer to the one
    symbol a DXE module can export.
  */
  if (sym == 0)
    {
      fprintf (stderr, "Error while loading DXE module: %s\n", module_name);
      exit (1);
    }

  // initialize the import/export structure

  /*
    Catch calls to uninitialized members in case a new function was added to
    st_symbol_t, but forgotten to initialize here.
  */
  m = sizeof (st_symbol_t) / sizeof (void (*) (void));
  for (n = 0; n < m && ((void (**) (void)) sym)[n] != 0; n++) // Don't overwrite values initialized by DXE
    ;
  for (; n < m; n++)
    ((void (**) (void)) sym)[n] = uninit_func;

  // initialize functions
  sym->printf = printf;
  sym->fprintf = fprintf;
  sym->sprintf = sprintf;
  sym->fputs = fputs;
  sym->fopen = fopen;
  sym->fclose = fclose;
  sym->fseek = fseek;
  sym->ftell = ftell;
  sym->fread = fread;
  sym->fwrite = fwrite;
  sym->fflush = fflush;

  sym->free = free;
  sym->malloc = malloc;
  sym->exit = exit;
  sym->strtol = strtol;

  sym->memcpy = memcpy;
  sym->memset = memset;
  sym->strcmp = strcmp;
  sym->strcpy = strcpy;
  sym->strcat = strcat;
  sym->strcasecmp = strcasecmp;
  sym->strncasecmp = strncasecmp;
  sym->strrchr = strrchr;

  sym->stat = stat;

  sym->time = time;

  // initialize variables
  sym->__dj_stdin = __dj_stdin;
  sym->__dj_stdout = __dj_stdout;
  sym->__dj_stderr = __dj_stderr;

  // initialize the DXE module
  sym->dxe_init ();

  if (new_handle == INITIAL_HANDLE)
    dxe_map = map_create (10);
  dxe_map = map_put (dxe_map, (void *) new_handle, sym);
  handle = (void *) new_handle++;
#elif   defined __unix__
  void *handle;

  handle = dlopen (module_name, RTLD_LAZY);
  if (handle == NULL)
    {
      fputs (dlerror (), stderr);
      fputs ("\n", stderr);
      exit (1);
    }
#elif   defined _WIN32
  void *handle = LoadLibrary (module_name);
  if (handle == NULL)
    {
      LPTSTR strptr;

      FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |
                     FORMAT_MESSAGE_FROM_SYSTEM |
                     FORMAT_MESSAGE_IGNORE_INSERTS,
                     NULL, GetLastError (),
                     MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                     (LPTSTR) &strptr, 0, NULL);
      // Note the construct with strptr. You wouldn't believe what a bunch of
      //  fucking morons those guys at Microsoft are!
      fputs (strptr, stderr);
      LocalFree (strptr);
      exit (1);
    }
#elif   defined __BEOS__
  void *handle = (void *) load_add_on (module_name);
  if ((int) handle < B_NO_ERROR)
    {
      fprintf (stderr, "Error while loading add-on image: %s\n", module_name);
      exit (1);
    }
#endif

  return handle;
}


void *
get_symbol (void *handle, char *symbol_name)
{
  void *symptr;
#ifdef  DJGPP
  st_symbol_t *sym = map_get (dxe_map, handle);
  if (sym == NULL)
    {
      fprintf (stderr, "Invalid handle: %x\n", (int) handle);
      exit (1);
    }

  symptr = sym->dxe_symbol (symbol_name);
  if (symptr == NULL)
    {
      fprintf (stderr, "Could not find symbol: %s\n", symbol_name);
      exit (1);
    }
#elif   defined __unix__
  char *strptr;

  symptr = dlsym (handle, symbol_name);
  if ((strptr = dlerror ()) != NULL)            // this is "the correct way"
    {                                           //  according to the info page
      fputs (strptr, stderr);
      fputs ("\n", stderr);
      exit (1);
    }
#elif   defined _WIN32
  symptr = GetProcAddress (handle, symbol_name); // actually only function names
  if (symptr == NULL)
    {
      LPTSTR strptr;

      FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER |
                     FORMAT_MESSAGE_FROM_SYSTEM |
                     FORMAT_MESSAGE_IGNORE_INSERTS,
                     NULL, GetLastError (),
                     MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                     (LPTSTR) &strptr, 0, NULL);
      fputs (strptr, stderr);
      LocalFree (strptr);
      exit (1);
    }
#elif   defined __BEOS__
  int status = get_image_symbol ((int) handle, symbol_name,
                                 B_SYMBOL_TYPE_TEXT, &symptr);
  if (status != B_OK)
    {
      fprintf (stderr, "Could not find symbol: %s\n", symbol_name);
      exit (1);
    }
#endif

  return symptr;
}
