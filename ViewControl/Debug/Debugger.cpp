
#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Debugger.h"

#pragma warning (disable : 4996)

//#define USE_INTERNAL_DEBUGGER

void AntlersDebugger::logMessage(const char* message, ...) 
{ 
#ifdef USE_INTERNAL_DEBUGGER
    static bool firstMsg = true;
    FILE *file;	
    const char *fileName = "log.txt";

    if(firstMsg) {
        file = fopen(fileName,"w");
        if (!file) return;
        firstMsg = false;
    }
    else {
        file = fopen(fileName,"a");
        if (!file) return;
    }

    char text[1024]; 
    va_list ap;		
    va_start(ap, message);         
    vsprintf(text, message, ap);   
    va_end(ap); 

    fputs(text,file);
    fputs("\n",file);
    fclose(file);
#endif
}
