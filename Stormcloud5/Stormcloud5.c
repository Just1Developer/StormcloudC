#include <stdio.h>
#include <string.h>

#include "h/memory.h"
#include "h/MovePreGen.h"

#ifdef _WIN32
// for error undefined reference to WinMain
#define NOMINMAX
#include <windows.h>
#endif

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

#include "h/Defaults.h"
//#include "h/search.h"
int main(int argc, char* argv[])
{
    string FEN;
    // Process argument pairs
    for(int i = 2; i < argc; i += 2)
    {
        int arglen;
        for (arglen = 0; argv[i][arglen] != '\0'; ++arglen) ;
        char unit = argv[i][arglen-1];
        switch(FirstArgCode(argv[i-1]))
        {
            case 0: continue;
            case 1:
                // Fen Argument
                break;
            case 2:
                switch(unit)
                {
                    case 's':   // seconds
                        break;
                    case 'c':   // milliseconds, dont ask why c
                        break;
                    case 'm':   // minutes
                        break;
                    default:
                        continue;
                }
                // Time Argument
                break;
            case 3:
                // Depth Argument
                break;
            default: continue;
        }
    }

    allocateMagicTables();
    // Pre-Generate moves:
    PreGenerateAllPossibleMoves();

    initDefaults();

    // todo: play chess

    freeMagicTables();
    getchar();
}
