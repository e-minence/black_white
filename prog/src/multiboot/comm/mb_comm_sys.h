//======================================================================
/**
 * @file	mb_comm_sys.c
 * @brief	マルチブート 通信システム
 * @author	ariizumi
 * @data	09/11/13
 *
 * モジュール名：MB_COMM
 */
//======================================================================
#pragma once

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
typedef struct _MB_COMM_WORK MB_COMM_WORK;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

extern MB_COMM_WORK* MB_COMM_CreateSystem( const HEAPID heapId );
extern void MB_COMM_DeleteSystem( MB_COMM_WORK* commWork );

