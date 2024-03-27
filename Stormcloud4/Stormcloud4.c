#include <stdio.h>
#include <string.h>

#include "headers/types.h"
#include "headers/memory.h"
#include "headers/MovePreGen.h"

// for error undefined reference to WinMain
#define NOMINMAX
#include <windows.h>


/** GPT4 Generated
#include <psapi.h>
// Link against Psapi.lib
#pragma comment(lib, "psapi.lib")
void printMemoryUsage() {
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;

    // Get the handle to the current process.
    hProcess = GetCurrentProcess();

    // Retrieve the memory usage information.
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        // Print the memory usage information.
        printf("PageFaultCount: %u\n", pmc.PageFaultCount);
        printf("PeakWorkingSetSize: %u\n", pmc.PeakWorkingSetSize);
        printf("WorkingSetSize: %u\n", pmc.WorkingSetSize);
        printf("QuotaPeakPagedPoolUsage: %u\n", pmc.QuotaPeakPagedPoolUsage);
        printf("QuotaPagedPoolUsage: %u\n", pmc.QuotaPagedPoolUsage);
        printf("QuotaPeakNonPagedPoolUsage: %u\n", pmc.QuotaPeakNonPagedPoolUsage);
        printf("QuotaNonPagedPoolUsage: %u\n", pmc.QuotaNonPagedPoolUsage);
        printf("PagefileUsage: %u\n", pmc.PagefileUsage);
        printf("PeakPagefileUsage: %u\n", pmc.PeakPagefileUsage);
    }

    // Close the handle.
    CloseHandle(hProcess);
}
*/

int FirstArgCode(string arg)
{
    // VALID ARGUMENTS:
    // 1: -fen
    // 2: -time
    // 3: -depth

    if (strcmp(arg, "-fen") == 0) {
        return 1;
    }
    else if (strcmp(arg, "-time") == 0) {
        return 2;
    }
    else if (strcmp(arg, "-depth") == 0) {
        return 3;
    }
    return 0;
}

#include "headers/Defaults.h"
#include "headers/search.h"
// Why is there no change in memory before/after allocate calls?
int main(int argc, char* argv[])
{
    string FEN;
    // Process argument pairs
    for(int i = 2; i < argc; i += 2)
    {
        int arglen;
        for (arglen = 0; argv[i][arglen] != '\0'; ++arglen) ;
        switch(FirstArgCode(argv[i-1]))
        {
            case 0: continue;
            case 1:
                // Fen Argument
                break;
            case 2:
                char unit = argv[i][arglen-1];
                switch(unit)
                {
                    case 's':   // seconds
                        break;
                    case 'c':   // milliseconds, dont ask why c
                        break;
                    case 'm':   // minutes
                        break;
                }
                // Time Argument
                break;
            case 3:
                // Depth Argument
                break;
            default: continue;
        }
    }

    printf("Hello World!\n");
    allocateMagicTables();
    printf("Allocated magic tables, rooks: %d\n", sizeof(RookMoves));
    // Pre-Generate moves:
    PreGenerateAllPossibleMoves();

    initDefaults();
    BeginIterativeDeepening(MyPositionsStart, OpponentPositionsStart, 2, 20, true);

    freeMagicTables();
    printf("Freed all memory.\n");
    getchar();
}

// Funy:
// printf("%d", *((int*)NULL));

/**
 * 
 * Controlled memory leak:
 * 
void ControlledMemoryLeak()
{
    printf("Begin of test. We will now produce a memory leak.\n");
    int amount = 10;
    Bitboard** pointers = malloc(sizeof(Bitboard*) * amount);

    for(int i = 0; i < amount; i++)
    {
        pointers[i] = memoryleak(200000000);
    }

    printf("End of test. We will now free the memory.\n");
    getchar();
    
    for(int i = 0; i < amount; i++)
    {
        free(pointers[i]);
    }
    free(pointers);
}
Bitboard* memoryleak(int callsize)
{
    static int size = sizeof (Bitboard);
    Bitboard* memory = malloc(callsize * size);
    if(memory == NULL) return memory;
    for(int i = 0; i < callsize; i++)
    {
        // Write to memory
        memory[i] = 0xFFFF2222FFFF2222;
    }
    // Do not free memory. Hence the memory leak
    return memory;  // Keep track for freeing later
}
 * 
*/