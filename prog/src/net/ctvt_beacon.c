//======================================================================
/**
 * @file	ctvt_beacon.c
 * @brief	�ʐM�g�����V�[�o�[�F�r�[�R��
 * @author	ariizumi
 * @data	09/12/22
 *
 * ���W���[�����FCTVT_BCON
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "net/ctvt_beacon.h"

#include "print/str_tool.h"
#include "ctvt_beacon_local.h"

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

const BOOL CTVT_BCON_CheckCallSelf( CTVT_COMM_BEACON *beacon , WIFI_LIST *wifiList , u8* selfMacAdr )
{
  u8 i,j;
  if( wifiList == NULL )
  {
    return FALSE;
  }
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
      //Wifi���X�g���疼�O�E���ʁEID�̃`�F�b�N
      return  WifiList_CheckFriendMystatus( wifiList , &beacon->myStatus, NULL );
    }
  }
  return FALSE;
}

//�r�[�R������f�[�^�擾�֐�
const STRCODE* CTVT_BCON_GetName( CTVT_COMM_BEACON *beacon )
{
  return MyStatus_GetMyName( &beacon->myStatus );
}
const u16 CTVT_BCON_GetIDLow( CTVT_COMM_BEACON *beacon )
{
  return MyStatus_GetID_Low( &beacon->myStatus );
}

