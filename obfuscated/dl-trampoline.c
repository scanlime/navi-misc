/*
 * Proof-of-concept for invoking the dynamic linker without
 * glibc or libdl's help. Recent versions of glibc actually
 * check the stack to ensure _dl_open is being called
 * from inside either libc or libdl: we have to find a return
 * instruction inside glibc to hijack to trick _dl_open.
 *
 * This will start up a python interpreter, assuming the
 * path to your libpython below is correct.
 *
 * --Micah Dowty <micah@navi.cx>
 */

void *dl;
int (*pymain)(int, char**);
char *libname = "/usr/lib/libpython2.4.so";
char *libmain = "Py_Main";
unsigned char *trampoline;

void dlopen()
{
  asm("pushl   $done \n\t"             // Eventual return address

      "pushl   $0 \n\t"                // LM_ID_BASE == 0
      "movl    trampoline, %ecx \n\t"  // caller
      "movl    $1, %edx \n\t"          // RTLD_NOW == 1
      "movl    libname, %eax \n\t"     // library filename

      "pushl   trampoline \n\t"        // Intermediate return address inside glibc
      "jmp     _dl_open \n"

      "done: \n\t"                     // Save the result
      "movl    %eax, dl");
}

void dlsym()
{
  asm("movl    trampoline, %ecx \n\t"  // caller
      "movl    libmain, %edx \n\t"     // symbol name
      "movl    dl, %eax \n\t"          // linker handle
      "call    _dl_sym \n\t"
      "movl    %eax, pymain");
}

// Starting point for the trampoline search
extern void *__malloc_hook;

int main(int argc, char **argv)
{
  /* Find a 'ret' instruction or something like it in libc to jump back through */
  trampoline = __malloc_hook;
  while (*trampoline != 0xC3) {
    trampoline++;
  }
  printf("Found trampoline at 0x%08x\n", trampoline);

  ((void(*)())trampoline)();
  printf("Tested trampoline\n");

  dlopen();
  printf("Back from dlopen! dl=0x%08x\n", dl);

  dlsym();
  printf("Back from dlsym!\n");

  return pymain(argc, argv);
}
