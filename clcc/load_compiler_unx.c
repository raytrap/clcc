#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include "compiler.h"

void *libnvidiacompiler = NULL;
tNvCliCompileProgram pNvCliCompileProgram;
tNvCliCompileLogFree pNvCliCompileLogFree;
tNvCliCompiledProgramFree pNvCliCompiledProgramFree;
int loaded = 0;
const char *compiler_path = "libnvidia-compiler.so";

void unload_compiler(void)
{
    if (!loaded)
    {
        return;
    }

    pNvCliCompileProgram = NULL;
    pNvCliCompileLogFree = NULL;
    pNvCliCompiledProgramFree = NULL;

    dlclose(libnvidiacompiler);
    libnvidiacompiler = NULL;

    loaded = 0;
}

void load_compiler()
{
    if (loaded)
    {
        return;
    }

    libnvidiacompiler = dlopen(compiler_path, RTLD_GLOBAL | RTLD_LAZY);
    if (libnvidiacompiler == NULL)
    {
        fprintf(stderr, "%s\n", dlerror());
        if (strcmp(compiler_path,"libnvidia-compiler.so")==0) {
            fprintf(stderr, "You may need to create a symlink to /usr/lib/libnvidia-compiler.so.XXX.XX.XX\n");
        }
        else {
            fprintf(stderr, "Unable to load library from (%s)\n", compiler_path);
        }
        exit(EXIT_FAILURE);
    }

    pNvCliCompileProgram = (tNvCliCompileProgram) dlsym(libnvidiacompiler, "NvCliCompileProgram");
    if (pNvCliCompileProgram == NULL)
    {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    pNvCliCompileLogFree = (tNvCliCompileLogFree) dlsym(libnvidiacompiler, "NvCliCompileLogFree");
    if (pNvCliCompileLogFree == NULL)
    {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    pNvCliCompiledProgramFree = (tNvCliCompiledProgramFree) dlsym(libnvidiacompiler, "NvCliCompiledProgramFree");
    if (pNvCliCompiledProgramFree == NULL)
    {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    atexit(unload_compiler);

    loaded = 1;
}