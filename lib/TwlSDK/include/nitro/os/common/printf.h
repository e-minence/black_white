/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS - include
  File:     printf.h

  Copyright 2003-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_COMMON_PRINTF_H_
#define NITRO_OS_COMMON_PRINTF_H_

#ifdef __MWEKS__
#pragma  warn_padding off
#endif
#include <stdio.h>                     // stdio.h �ł� PADDING �x�����~�߂�
#ifdef __MWEKS__
#pragma  warn_padding reset
#endif
#include <stdarg.h>
#include <nitro/types.h>


#ifdef __cplusplus
extern "C" {
#endif  // ifdef __cplusplus


//
//  Ensata controls
//
#define REG_EMU_CONSOLE_OUT      0x4fff000
#define reg_OS_EMU_CONSOLE_OUT   (*((REGType8v *)REG_EMU_CONSOLE_OUT))


// output window on IS-TWL-DEBUGGER
//
typedef enum
{
	OS_PRINT_OUTPUT_PROC_ARM9     = 0,
	OS_PRINT_OUTPUT_PROC_ARM7     = 1,
	OS_PRINT_OUTPUT_PROC_ARM9ERR  = 2,
	OS_PRINT_OUTPUT_PROC_ARM7ERR  = 3
}
OSPrintOutputProc;

#define OS_PRINT_OUTPUT_DEFAULT_ARM9     0
#define OS_PRINT_OUTPUT_DEFAULT_ARM7     3
#define OS_PRINT_OUTPUT_DEFAULT_ARM9ERR  0
#define OS_PRINT_OUTPUT_DEFAULT_ARM7ERR  3
#define OS_PRINT_OUTPUT_WINDOW_MAX       3

#define OS_PRINT_OUTPUT_NONE             0xff
#define OS_PRINT_OUTPUT_CURRENT          0xfe
#define OS_PRINT_OUTPUT_ERROR            0xfd


#define OS_FPUTSTRING_DUMMY          0
#define OS_FPUTSTRING_ARIS           1
#define OS_FPUTSTRING_ISD            2
#define OS_FPUTSTRING_ISTD           3
typedef void (*OSFPutStringHookType)(int type, int console, const char *str);

//
//  Prototypes
//
#ifndef SDK_FINALROM
extern void (*OS_PutString) (const char *str);
extern void (*OS_FPutString) (int console, const char *str);
#else   // ifndef SDK_FINALROM
#define OS_PutString(x)        ((void)0)
#define OS_FPutString(c, x)    ((void)0)
#endif  // ifndef SDK_FINALROM else

int     OS_SPrintf(char *dst, const char *fmt, ...);
int     OS_VSPrintf(char *dst, const char *fmt, va_list vlist);
int     OS_SNPrintf(char *dst, size_t len, const char *fmt, ...);
int     OS_VSNPrintf(char *dst, size_t len, const char *fmt, va_list vlist);
int     OS_VSNPrintfEx(char *dst, size_t len, const char *fmt, va_list vlist);

#ifndef SDK_FINALROM
void    OS_PutChar(char c);
void    OS_VPrintf(const char *fmt, va_list vlist);
void    OS_TVPrintf(const char *fmt, va_list vlist);
void    OS_TVPrintfEx(const char *fmt, va_list vlist);
void    OS_Printf(const char *fmt, ...);
void    OS_TPrintf(const char *fmt, ...);
void    OS_TPrintfEx(const char *fmt, ...);

void    OS_FPutChar(int console, char c);
void    OS_VFPrintf(int console, const char *fmt, va_list vlist);
void    OS_TVFPrintf(int console, const char *fmt, va_list vlist);
void    OS_TVFPrintfEx(int console, const char *fmt, va_list vlist);
void    OS_FPrintf(int console, const char *fmt, ...);
void    OS_TFPrintf(int console, const char *fmt, ...);
void    OS_TFPrintfEx(int console, const char *fmt, ...);
#ifndef SDK_NO_MESSAGE
void    OSi_FWarning(int console, const char *file, int line, const char *fmt, ...);
void    OSi_TFWarning(int console, const char *file, int line, const char *fmt, ...);
void    OSi_FPanic(int console, const char *file, int line, const char *fmt, ...);
void    OSi_TFPanic(int console, const char *file, int line, const char *fmt, ...);

void    OSi_Warning(const char *file, int line, const char *fmt, ...);
void    OSi_TWarning(const char *file, int line, const char *fmt, ...);
void    OSi_Panic(const char *file, int line, const char *fmt, ...);
void    OSi_TPanic(const char *file, int line, const char *fmt, ...);
#else   // ifndef SDK_NO_MESSAGE
void    OS_Terminate();
#define OSi_FWarning( console, file, line, ... )    ((void)0)
#define OSi_FPanic( console, file, line, ... )      OS_Terminate()
#define OSi_TFWarning( console, file, line, ... )   ((void)0)
#define OSi_TFPanic( console, file, line, ... )     OS_Terminate()

#define OSi_Warning( file, line, ... )              ((void)0)
#define OSi_TWarning( file, line, ... )             ((void)0)
#define OSi_Panic( file, line, ... )                OS_Terminate()
#define OSi_TPanic( file, line, ... )               OS_Terminate()
#endif  // ifndef SDK_NO_MESSAGE else

#define OS_Warning( ... )                           OSi_Warning( __FILE__, __LINE__, __VA_ARGS__ );
#define OS_Panic( ... )                             OSi_Panic( __FILE__, __LINE__, __VA_ARGS__ );
#define OS_TWarning( ... )                          OSi_TWarning( __FILE__, __LINE__, __VA_ARGS__ );
#define OS_TPanic( ... )                            OSi_TPanic( __FILE__, __LINE__, __VA_ARGS__ );

#define OS_FWarning( console, ... )                 OSi_FWarning(  (console), __FILE__, __LINE__, __VA_ARGS__ );
#define OS_FPanic( console, ... )                   OSi_FPanic(    (console), __FILE__, __LINE__, __VA_ARGS__ );
#define OS_TFWarning( console, ... )                OSi_TFWarning( (console), __FILE__, __LINE__, __VA_ARGS__ );
#define OS_TFPanic( console, ... )                  OSi_TFPanic(   (console), __FILE__, __LINE__, __VA_ARGS__ );

#else   // ifndef SDK_FINALROM
//---- invalidate debug functions when FINALROM
#define OS_PutChar( ... )                           ((void)0)
#define OS_VPrintf( fmt, ... )                      ((void)0)
#define OS_TVPrintf( fmt, ... )                     ((void)0)
#define OS_TVPrintfEx( fmt, ... )                   ((void)0)
#define OS_Printf( ... )                            ((void)0)
#define OS_TPrintf( ... )                           ((void)0)
#define OS_TPrintfEx( ... )                         ((void)0)

#define OS_FPutChar( console, ... )                 ((void)0)
#define OS_VFPrintf( console, fmt, ... )            ((void)0)
#define OS_TVFPrintf( console, fmt, ... )           ((void)0)
#define OS_TVFPrintfEx( console, fmt, ... )         ((void)0)
#define OS_FPrintf( console,  ... )                 ((void)0)
#define OS_TFPrintf( console,  ... )                ((void)0)
#define OS_TFPrintfEx( console, ... )               ((void)0)

void    OS_Terminate();
#define OSi_FWarning( console, file, line, ... )    ((void)0)
#define OSi_FPanic( console, file, line, ... )      OS_Terminate()
#define OSi_TFWarning( console, file, line, ... )   ((void)0)
#define OSi_TFPanic( console, file, line, ... )     OS_Terminate()

#define OSi_Warning( file, line, ... )              ((void)0)
#define OSi_Panic( file, line, ... )                OS_Terminate()
#define OSi_TWarning( file, line, ... )             ((void)0)
#define OSi_TPanic( file, line, ... )               OS_Terminate()

#define OS_Warning( ... )                           ((void)0)
#define OS_Panic( ... )                             OS_Terminate()
#define OS_TWarning( ... )                          ((void)0)
#define OS_TPanic( ... )                            OS_Terminate()

#define OS_FWarning( console, ... )                 ((void)0)
#define OS_FPanic( console, ... )                   OS_Terminate()
#define OS_TFWarning( console, ... )                ((void)0)
#define OS_TFPanic( console, ... )                  OS_Terminate()

#endif  // ifndef SDK_FINALROM else

/*---------------------------------------------------------------------------*
  Name:         OS_SetPrintOutput

  Description:  Specify the output window OS_Printf uses.
                This is only for IS-TWL-DEBUGGER.

  Arguments:    proc : processor
                         OS_PRINT_OUTPUT_PROC_ARM9     : ARM9
                         OS_PRINT_OUTPUT_PROC_ARM7     : ARM7
                         OS_PRINT_OUTPUT_PROC_ARM9ERR  : ARM9 for error
                         OS_PRINT_OUTPUT_PROC_ARM7ERR  : ARM7 for error
                num  : output window number (0-3)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_SetPrintOutput( OSPrintOutputProc proc, int num );
#define OS_SetPrintOutput_Arm9( num )     OS_SetPrintOutput( OS_PRINT_OUTPUT_PROC_ARM9, (num) )
#define OS_SetPrintOutput_Arm7( num )     OS_SetPrintOutput( OS_PRINT_OUTPUT_PROC_ARM7, (num) )
#define OS_SetPrintOutput_Arm9Err( num )  OS_SetPrintOutput( OS_PRINT_OUTPUT_PROC_ARM9ERR, (num) )
#define OS_SetPrintOutput_Arm7Err( num )  OS_SetPrintOutput( OS_PRINT_OUTPUT_PROC_ARM7ERR, (num) )

/*---------------------------------------------------------------------------*
  Name:         OS_SetFPutStringHook

  Description:  set hook function in FPutString

  Arguments:    func : hook function

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
void OS_SetFPutStringHook( OSFPutStringHookType func );
#else
#define OS_SetFPutStringHook( func )         ((void)0)
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_GetFPutStringHook

  Description:  get hook function in FPutString

  Arguments:    None.

  Returns:      hook function
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
OSFPutStringHookType OS_GetFPutStringHook(void);
#else
#define OS_GetFPutStringHook()               ((void)0)
#endif

//================================================================================
//  PrintServer
//================================================================================
typedef union
{
    u16     s;
    char    c[2];
}
OSPrintWChar;

#define OS_PRINTSRV_BUFFERSIZE  1024
#define OS_PRINTSRV_WCHARSIZE   (OS_PRINTSRV_BUFFERSIZE/sizeof(OSPrintWChar))

typedef volatile struct
{
    u32     in;
    u32     out;
    OSPrintWChar buffer[OS_PRINTSRV_WCHARSIZE];

}
OSPrintServerBuffer;

#ifdef  SDK_ARM9
#ifndef SDK_FINALROM
void    OS_InitPrintServer(void);
void    OS_PrintServer(void);
#else   // ifndef SDK_FINALROM
#define OS_InitPrintServer()                    ((void)0)
#define OS_PrintServer()                        ((void)0)
#endif  // ifndef SDK_FINALROM else
#endif  // ifdef  SDK_ARM9

//================================================================================
//              Blocking mode
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_SetPrintBlockingMode

  Description:  set blocking mode on debug print

  Arguments:    sw : blocking mode to set.
                     TRUE if want to set block mode, or FALSE if not.

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
void OS_SetPrintBlockingMode( BOOL sw );
#else
#define OS_SetPrintBlockingMode(sw)  ((void)0)
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_GetPrintBlockingMode

  Description:  get blocking mode on debug print

  Arguments:    None.

  Returns:      blocking mode.
                TRUE if blocking is available, FALSE if not.
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
BOOL OS_GetPrintBlockingMode( void );
#else
#define OS_GetPrintBlockingMode() (0)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif  // ifdef __cplusplus

/* NITRO_OS_COMMON_PRINTF_H_ */
#endif
