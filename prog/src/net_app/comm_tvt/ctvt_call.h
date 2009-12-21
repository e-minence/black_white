//======================================================================
/**
 * @file	ctvt_call.c
 * @brief	�ʐMTVT�V�X�e���F�Ăяo���������
 * @author	ariizumi
 * @data	09/12/21
 *
 * ���W���[�����FCTVT_CALL
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
extern CTVT_CALL_WORK* CTVT_CALL_InitSystem( COMM_TVT_WORK *work , const HEAPID heapId );
extern void CTVT_CALL_TermSystem( COMM_TVT_WORK *work , CTVT_CALL_WORK *commWork );
extern const COMM_TVT_MODE CTVT_CALL_Main( COMM_TVT_WORK *work , CTVT_CALL_WORK *commWork );
extern void CTVT_CALL_InitMode( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork );
extern void CTVT_CALL_TermMode( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork );

//--------------------------------------------------------------
//	
//--------------------------------------------------------------