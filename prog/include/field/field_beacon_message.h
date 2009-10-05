//======================================================================
/**
 * @file    field_beacon_message.h
 * @brief   �t�B�[���h�Ńr�[�R���Ń��b�Z�[�W������肷��
 * @author  ariizumi
 * @data    09/10/02
 *
 * ���W���[�����FFIELD_BEACON_MSG
 */
//======================================================================
#pragma once
#include "field/game_beacon_search.h"
#include "savedata/mystatus.h"

//======================================================================
//  define
//======================================================================
#pragma mark [> define

//======================================================================
//  enum
//======================================================================
#pragma mark [> enum


//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct

typedef struct _FIELD_BEACON_MSG_DATA FIELD_BEACON_MSG_DATA;
typedef struct _FIELD_BEACON_MSG_SYS FIELD_BEACON_MSG_SYS;

//======================================================================
//  proto
//======================================================================
#pragma mark [> proto


//--------------------------------------------------------------
//	�f�[�^�̍쐬�E�J���E������(���O�Z�b�g�̂���MYSTATUS��n��
//--------------------------------------------------------------
extern FIELD_BEACON_MSG_DATA* FIELD_BEACON_MSG_CreateData( const HEAPID heapId  , MYSTATUS *myStatus );
extern void FIELD_BEACON_MSG_DeleteData( FIELD_BEACON_MSG_DATA* fbmData );
extern void FIELD_BEACON_MSG_ResetData( FIELD_BEACON_MSG_DATA* fbmData  , MYSTATUS *myStatus );

//--------------------------------------------------------------
//	�r�[�R���`�F�b�N����
//--------------------------------------------------------------
extern FIELD_BEACON_MSG_SYS* FIELD_BEACON_MSG_InitSystem( FIELD_BEACON_MSG_DATA* fbmData , const HEAPID heapId );
extern void FIELD_BEACON_MSG_ExitSystem( FIELD_BEACON_MSG_SYS* fbmSys );
extern void FIELD_BEACON_MSG_SetBeaconMessage( FIELD_BEACON_MSG_SYS *fbmSys , GBS_BEACON *beacon );
extern void FIELD_BEACON_MSG_CheckBeacon( FIELD_BEACON_MSG_SYS *fbmSys , GBS_BEACON *beacon , u8 *macAddress );

//--------------------------------------------------------------
//	���b�Z�[�W�n
//--------------------------------------------------------------
extern void FIELD_BEACON_MESSAGE_SetWord( FIELD_BEACON_MSG_DATA *fbmData , u16 *word );
