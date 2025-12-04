// Copyright (C) 1999-2021
// Smithsonian Astrophysical Observatory, Cambridge, MA, USA
// For conditions of distribution and use, see copyright notice in "copyright"

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <sstream>
using namespace std;

#include <tcl.h>
#include <tk.h>

Tcl_Interp *global_interp =NULL;

extern "C" {
  int SAOAppInit(Tcl_Interp *interp);
  int SAOLocalMainHook(int* argc, char*** argv);

  void TclSetStartupScriptFileName(const char*);

  int TclZipfs_Init(Tcl_Interp*);
  int TclZipfs_Mount(Tcl_Interp*, const char*, const char *, const char*);

  int Tkblt_Init(Tcl_Interp*);
  int Tktable_Init(Tcl_Interp*);
  int Tksao_Init(Tcl_Interp*);
  int Tkhtml1_Init(Tcl_Interp*);
  int Tkmpeg_Init(Tcl_Interp*);
  int Tksvg_Init(Tcl_Interp*);
  int Tkagif_Init(Tcl_Interp*);

  int Tkimg_Init(Tcl_Interp*);
  int Zlibtcl_Init(Tcl_Interp*);
  int Jpegtcl_Init(Tcl_Interp*);
  int Tkimgjpeg_Init(Tcl_Interp*);
  int Tifftcl_Init(Tcl_Interp*);
  int Tkimgtiff_Init(Tcl_Interp*);
  int Tkimgwindow_Init(Tcl_Interp*);

  int Tclxpa_Init(Tcl_Interp*);
  int Tclfitsy_Init(Tcl_Interp*);

  int Tls_Init(Tcl_Interp*);
  int Tclxml_Init(Tcl_Interp*);
  int Tclxml_libxml2_Init(Tcl_Interp*);

  int Tclxmlrpc_Init(Tcl_Interp*);

  int Signal_ext_Init(Tcl_Interp*);
}

// currently use relative path
// using full path with spaces causes problems
// with htmwidget and tcl/tk

#define PATHSIZE 2048

char my_tcl_library_path[PATHSIZE+1];


int SAOLocalMainHook(int* argcPtr, char*** argvPtr)
{
  // sync C++ io calls with C io calls
  ios::sync_with_stdio();

  // do this first
  Tcl_FindExecutable((*argvPtr)[0]);


  // use exec path
  char** argv = *argvPtr;
  char ss[PATHSIZE+1];
  memset(ss, 0, PATHSIZE+1);
  strncpy(ss,argv[0],PATHSIZE);

  // now remove "/ds9.exe"
  char* ptr = ss+strlen(ss);
  while (*ptr != '/' && ptr != ss)
    ptr--;
  *ptr = '\0';

  // so that tcl and tk know where to find their libs
  // we do it here before InitLibraryPath is called
  char rr[PATHSIZE+1];
  memset(rr, 0, PATHSIZE+1);
  char rp[PATHSIZE+1];
  memset(rp, 0, PATHSIZE+1);

  strncpy(rr,ss,PATHSIZE);
  strncat(rr,"/../lib/ds9/tcl8.6",PATHSIZE);
  realpath(rr, rp);   // Absolute path
  ostringstream str;
  str << "TCL_LIBRARY=" << rp << ends;
  strcpy(my_tcl_library_path, (char*)str.str().c_str());

  // and add startup script
  memset(rr, 0, PATHSIZE+1);
  strncpy(rr,ss,PATHSIZE);
  strncat(rr,"/../lib/ds9/library/ds9.tcl",PATHSIZE);
  realpath(rr, rp);

  Tcl_Obj *path = Tcl_NewStringObj(rp,-1);

  Tcl_SetStartupScript(path, NULL);

  return TCL_OK;
}

int SAOAppInit(Tcl_Interp *interp)
{

  // This needs to happen in the Init code rather than the Main hook
  putenv(my_tcl_library_path);


  // save interp for cputs function
  global_interp = interp;

  // We have to initialize the virtual filesystem before calling
  // Tcl_Init().  Otherwise, Tcl_Init() will not be able to find
  // its startup script files.
  if (TclZipfs_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "zipfs", TclZipfs_Init,
		     (Tcl_PackageInitProc*)NULL);

  // find current working directory, and set as mount point
  //~ {
//~ #ifdef ZIPFILE
    //~ ostringstream str;
    //~ str << (char *)Tcl_GetNameOfExecutable()
	//~ << ".zip"
	//~ <<  ends;
    //~ if(TclZipfs_Mount(interp, "", (const char*)str.str().c_str(), NULL) != TCL_OK ){
      //~ cerr << "ERROR: Unable to open the auxiliary ds9 file 'ds9.zip'. If you moved the ds9 program from its original location, please also move the zip file to the same place." << endl;
      //~ exit(1);
    //~ }
//~ #else
    //~ TclZipfs_Mount(interp, "", (const char *)Tcl_GetNameOfExecutable(), NULL);
//~ #endif
  //~ }

  // Tcl
  if (Tcl_Init(interp) == TCL_ERROR)
    return TCL_ERROR;

  // Tk
  if (Tk_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage(interp,"Tk", Tk_Init, Tk_SafeInit);

  // Tkblt
  if (Tkblt_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage(interp, "tkblt", Tkblt_Init,
		    (Tcl_PackageInitProc*)NULL);

  // Tktable
  if (Tktable_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "Tktable", Tktable_Init,
		     (Tcl_PackageInitProc*)NULL);

  // Tls
  if (Tls_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "tls", Tls_Init,
		     (Tcl_PackageInitProc*)NULL);

  // Tksao
  if (Tksao_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "tksao", Tksao_Init,
  		     (Tcl_PackageInitProc*)NULL);

  // Tkhtml1
  if (Tkhtml1_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "tkhtml1", Tkhtml1_Init,
  		     (Tcl_PackageInitProc*)NULL);

  // Tclxpa
  if (Tclxpa_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "Tclxpa", Tclxpa_Init,
		     (Tcl_PackageInitProc*)NULL);

  // Tclfitsy
  if (Tclfitsy_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "Tclfitsy", Tclfitsy_Init,
		     (Tcl_PackageInitProc*)NULL);

  // Tkmpeg
  if (Tkmpeg_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "tkmpeg", Tkmpeg_Init,
		     (Tcl_PackageInitProc*)NULL);

  // Tksvg
  if (Tksvg_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "tksvg", Tksvg_Init,
		     (Tcl_PackageInitProc*)NULL);

  // Tkagif
  if (Tkagif_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "tkagif", Tkagif_Init,
		     (Tcl_PackageInitProc*)NULL);

  // Tclxml
  if (Tclxml_Init(interp) == TCL_ERROR)
    return TCL_ERROR;

  // Tclxmlrpc
  if (Tclxmlrpc_Init(interp) == TCL_ERROR)
    return TCL_ERROR;

  // Tkimg
  if (Tkimg_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "img", Tkimg_Init,
		     (Tcl_PackageInitProc*)NULL);

  // zlibtcl
  if (Zlibtcl_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "zlibtcl", Zlibtcl_Init,
		     (Tcl_PackageInitProc*)NULL);

  // jpegtcl
  if (Jpegtcl_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "jpegtcl", Jpegtcl_Init,
		     (Tcl_PackageInitProc*)NULL);

  // Tkimgjpeg
  if (Tkimgjpeg_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "jpeg", Tkimgjpeg_Init,
		     (Tcl_PackageInitProc*)NULL);

  // Tifftcl
  if (Tifftcl_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "tifftcl", Tifftcl_Init,
		     (Tcl_PackageInitProc*)NULL);

  // Tkimgtiff
  if (Tkimgtiff_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "tiff", Tkimgtiff_Init,
		     (Tcl_PackageInitProc*)NULL);

  // Tkimgwindow
  if (Tkimgwindow_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "window", Tkimgwindow_Init,
		     (Tcl_PackageInitProc*)NULL);

  // Signal_Ext
  if (Signal_ext_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
  Tcl_StaticPackage (interp, "signal", Signal_ext_Init,
		     (Tcl_PackageInitProc*)NULL);

  return TCL_OK;
}
