// This code is hereby placed in the public domain.
// This code comes with ABSOLUTELY NO WARRANTY, EITHER EXPRESSED OR IMPLIED.
// USE AT YOUR OWN RISK!
//
// shlink.c - create Microsoft Windows shell links (aka shortcuts).
//
// Originally from:
// http://www.metathink.com/shlink/shlink.c

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>

#include <stdio.h>
#include <stdlib.h>

#if !defined(OPTIONAL)
# define OPTIONAL /*param is optional, pass as NULL or 0 if not used*/
#endif

char *_prgm = "shlink";

// Create Shortcuts with IShellLink and IPersistFile.
// FYI: this function is based on a trivial example in Microsoft VC++ doc's.

HRESULT
CreateShellLink(
		LPCSTR lpszTargetPath,       // existing object shortcut will point to
		LPSTR lpszLinkPath,          // path to shortcut (MUST END IN .lnk)
		LPSTR lpszCWD OPTIONAL,      // working directory when launching shortcut
		LPSTR lpszDesc OPTIONAL,     // description (not apparent how this is used)
		LPSTR lpszIconPath OPTIONAL, // path to file containing icon resource
		int   iIconIndex OPTIONAL,   // index of icon in above file
		LPSTR lpszArguments OPTIONAL // command-line arguments
		)
{
    HRESULT hres;
    IShellLink* psl;

    // Get a pointer to the IShellLink interface.

    hres = CoCreateInstance(&CLSID_ShellLink, NULL,
                            CLSCTX_INPROC_SERVER, &IID_IShellLink, (void**) &psl);
    if (SUCCEEDED(hres)) {
        IPersistFile* ppf;

	// Set the path to the shortcut target, and add the
	// description.

	psl->lpVtbl->SetPath(psl, lpszTargetPath);
	psl->lpVtbl->SetRelativePath(psl, lpszTargetPath, 0);

	if (lpszDesc)      psl->lpVtbl->SetDescription(psl, lpszDesc);
	if (lpszIconPath)  psl->lpVtbl->SetIconLocation(psl, lpszIconPath,
							iIconIndex);
	if (lpszArguments) psl->lpVtbl->SetArguments(psl, lpszArguments);
	if (lpszCWD)       psl->lpVtbl->SetWorkingDirectory(psl, lpszCWD);

	// Query IShellLink for the IPersistFile interface for saving the
	// shortcut in persistent storage.

        hres = psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, (void**) &ppf);

        if (SUCCEEDED(hres)) {
            WORD wsz[MAX_PATH];

            // Ensure that the string is ANSI.
            MultiByteToWideChar(CP_ACP, 0, lpszLinkPath, -1, wsz, MAX_PATH);

            // Save the link by calling IPersistFile::Save.
            hres = ppf->lpVtbl->Save(ppf, wsz, TRUE);
            ppf->lpVtbl->Release(ppf);
        }
        psl->lpVtbl->Release(psl);
    }
    return hres;
}

// Format and print a HRESULT error code and error text to the supplied FILE.
//   Terminate with a newline.

void
ferrWin32( FILE *fp, char *msg, HRESULT hres )
{
    LPTSTR lpMsg = NULL;

    fprintf(fp,"%s: %s (error 0x%08lx - ",_prgm,msg,hres);
    if (FormatMessage(
		      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS  |
		      FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		      NULL,
		      (DWORD) hres,
		      0,					// language ID
		      (LPTSTR) & lpMsg,	// must free with LocalFree()
		      0,
		      (va_list*)0)
	&& lpMsg != NULL )
	{
	    fprintf(fp,"\"%s\"",lpMsg);
	    LocalFree( (HLOCAL) lpMsg );
	} else {
	    fprintf(fp,"no text available");
	}
    fprintf(fp,")\n");
}

int
main( int argc, char **argv )
{
    HRESULT hres;

    if (argc < 3) {
	fprintf(stderr,
		"\n"
		"%s version 0.02 - create Microsoft \"Shortcuts\"\n"
		"\n"
		"usage: %s targetpath linkpath workdir arguments\n"
		"\n"
		"  targetpath : relative path of existing object to which link points\n"
		"  linkpath   : path at which to create link (MUST END IN \".lnk\"!)\n"
		"  arguments  : command line arguments\n"
		"\n"
		"This program is in the public domain.\n"
		"\n"
		"This program comes with ABSOLUTELY NO WARRANTY, EITHER EXPRESSED OR IMPLIED.\n"
		"USE AT YOUR OWN RISK!\n",
		_prgm,_prgm);
	return 1;
    }

    hres = CoInitialize( NULL );

    if (! SUCCEEDED(hres)) {
	ferrWin32(stderr,"CoInitialize failed",hres);
	return 1;
    }

    hres = CreateShellLink(
			   argv[1], /* Target */
			   argv[2], /* Shortcut path */
			   argv[3], /* Working directory */
			   NULL,
			   NULL,
			   0,
			   argv[4]  /* Arguments */
			   );

    if (! SUCCEEDED(hres)) {
	ferrWin32(stderr,"CreateShellLink failed",hres);
	CoUninitialize();
	return 1;
    }

    CoUninitialize();
    return 0;
}

