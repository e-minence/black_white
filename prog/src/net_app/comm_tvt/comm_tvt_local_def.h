//======================================================================
/**
 * @file	comm_tvt_local_def.h
 * @brief	�ʐMTVT��`
 * @author	ariizumi
 * @data	09/12/16
 *
 * ���W���[�����FCTVT_
 */
//======================================================================
#pragma once

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define CTVT_FRAME_MAIN_DRAW    (GFL_BG_FRAME2_M)
#define CTVT_FRAME_MAIN_CAMERA  (GFL_BG_FRAME3_M)

#define CTVT_FRAME_SUB_BG  (GFL_BG_FRAME3_M)

//�X�N���[���̃o�b�t�@�T�C�Y
#define CTVT_BUFFER_SCR_SIZE (256*192*2)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct _COMM_TVT_WORK COMM_TVT_WORK;
typedef struct _CTVT_CAMERA_WORK CTVT_CAMERA_WORK;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//UTIL
extern const BOOL COMM_TVT_IsTwlMode( void );

