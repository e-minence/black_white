//======================================================================
/**
 * @file	ctvt_beacon.c
 * @brief	通信トランシーバー：ビーコン
 * @author	ariizumi
 * @data	09/12/22
 *
 * モジュール名：CTVT_BCON
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "net/ctvt_beacon.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

const BOOL CTVT_BCON_CheckCallSelf( CTVT_COMM_BEACON *beacon , u8* selfMacAdr )
{
  u8 i,j;
  for( i=0;i<3;i++ )
  {
    BOOL isSame = TRUE;
    for( j=0;j<6;j++ )
    {
      if( beacon->callTarget[i][j] != selfMacAdr[j] )
      {
        isSame = FALSE;
        break;
      }
    }
    if( isSame == TRUE )
    {
      return TRUE;
    }
  }
  return FALSE;
}
