//======================================================================
/**
 * @file	  mb_cap_down.h
 * @brief	  �ߊl�Q�[���E�����
 * @author	ariizumi
 * @data	  09/11/26
 *
 * ���W���[�����FMB_CAP_DOWN
 */
//======================================================================
#pragma once
#include "./mb_cap_local_def.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
//�X�e�[�g
typedef enum
{
  MCDS_NONE,        //��������
  MCDS_DRAG,        //��������
  MCDS_SHOT_WAIT,   //���ł�
  MCDS_SUPPLY_BALL, //�{�[����[
}MB_CAP_DOWN_STATE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct _MB_CAP_DOWN MB_CAP_DOWN;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
extern MB_CAP_DOWN* MB_CAP_DOWN_InitSystem( MB_CAPTURE_WORK *work );
extern void MB_CAP_DOWN_DeleteSystem( MB_CAPTURE_WORK *work , MB_CAP_DOWN *downWork );
extern void MB_CAP_DOWN_UpdateSystem( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork );

extern const MB_CAP_DOWN_STATE MB_CAP_DOWN_GetState( const MB_CAP_DOWN *downWork );

extern const fx32 MB_CAP_DOWN_GetPullLen( const MB_CAP_DOWN *downWork );
extern const u16  MB_CAP_DOWN_GetRotAngle( const MB_CAP_DOWN *downWork );
extern const fx32 MB_CAP_DOWN_GetShotLen( const MB_CAP_DOWN *downWork );
extern const u16  MB_CAP_DOWN_GetShotAngle( const MB_CAP_DOWN *downWork );
extern const BOOL MB_CAP_DOWN_GetIsBonusBall( const MB_CAP_DOWN *downWork );

//�{�[���̕�[
extern void MB_CAP_DOWN_ReloadBall( MB_CAP_DOWN *downWork , const BOOL isBonus );

