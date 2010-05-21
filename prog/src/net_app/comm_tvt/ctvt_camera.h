//======================================================================
/**
 * @file	ctvt_camera.h
 * @brief	�ʐMTVT�V�X�e���F�J����+���ʊǗ�
 * @author	ariizumi
 * @data	09/12/16
 *
 * ���W���[�����FCTVT_CAMERA
 */

//======================================================================
#pragma once

#include "comm_tvt_local_def.h"
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
extern CTVT_CAMERA_WORK* CTVT_CAMERA_Init( COMM_TVT_WORK *work , const HEAPID heapId );
extern void CTVT_CAMERA_Term( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork );
extern void CTVT_CAMERA_Main( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork );
extern void CTVT_CAMERA_VBlank( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork );
extern void CTVT_CAMERA_StopCapture( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork );
extern const BOOL CTVT_CAMERA_IsStopCapture( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork );

extern void CTVT_CAMERA_SetRefreshFlg( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork , const u8 idx );
extern void CTVT_CAMERA_ResetRefreshFlg( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork , const u8 idx );
extern void CTVT_CAMERA_SetWaitAllRefreshFlg( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork , const BOOL isClear );
extern const BOOL CTVT_CAMERA_GetWaitAllRefreshFlg( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork );

extern const u16 CTVT_CAMERA_GetPhotoSizeX( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork );
extern const u16 CTVT_CAMERA_GetPhotoSizeY( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork );
extern void* CTVT_CAMERA_GetSelfBuffer( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork );
extern void* CTVT_CAMERA_GetBuffer( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork , const u8 idx );
extern const u32 CTVT_CAMERA_GetBufferSize( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork );

extern void CTVT_CAMERA_SetNewMember( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork , const u8 idx );
extern const BOOL CTVT_CAMERA_IsUpdateCameraAnime( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork , const u8 idx );
extern void CTVT_CAMERA_SetUpdateCameraAnime( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork , const u8 idx , const u8 flg );
extern void CTVT_CAMERA_SetCameraEffect( COMM_TVT_WORK *work , CTVT_CAMERA_WORK *camWork , const u8 effect );
