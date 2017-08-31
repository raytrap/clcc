#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include "compiler.h"

HMODULE nvcompiler = NULL;
tNvCliCompileProgram pNvCliCompileProgram;
tNvCliCompileLogFree pNvCliCompileLogFree;
tNvCliCompiledProgramFree pNvCliCompiledProgramFree;
int loaded = 0;

#ifdef _WIN64
const char *compiler_path = "nvcompiler.dll";
#else
const char *compiler_path = "nvcompiler32.dll";
#endif

int unload_compiler(void)
{
    if (!loaded)
    {
        return 0;
    }

    pNvCliCompileProgram = NULL;
    pNvCliCompileLogFree = NULL;
    pNvCliCompiledProgramFree = NULL;

    FreeLibrary(nvcompiler);
    nvcompiler = NULL;

    loaded = 0;

	return 0;
}

void print_error()
{
    char *message = NULL;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        0,
        (LPSTR)&message,
        0,
        NULL);
    fprintf(stderr, "%s: %s\n", compiler_path, message);
    LocalFree(message);
}

void load_compiler()
{
    if (loaded)
    {
        return;
    }

    nvcompiler = LoadLibrary(compiler_path);
    if (nvcompiler == NULL)
    {
        print_error();
        exit(EXIT_FAILURE);
    }

    pNvCliCompileProgram = (tNvCliCompileProgram) GetProcAddress(nvcompiler, "NvCliCompileProgram");
    if (pNvCliCompileProgram == NULL)
    {
        print_error();
        exit(EXIT_FAILURE);
    }

    pNvCliCompileLogFree = (tNvCliCompileLogFree) GetProcAddress(nvcompiler, "NvCliCompileLogFree");
    if (pNvCliCompileLogFree == NULL)
    {
        print_error();
        exit(EXIT_FAILURE);
    }

    pNvCliCompiledProgramFree = (tNvCliCompiledProgramFree) GetProcAddress(nvcompiler, "NvCliCompiledProgramFree");
    if (pNvCliCompiledProgramFree == NULL)
    {
        print_error();
        exit(EXIT_FAILURE);
    }

    _onexit(unload_compiler);

    loaded = 1;
}