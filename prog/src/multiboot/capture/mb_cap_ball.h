//======================================================================
/**
 * @file	  mb_cap_obj.c
 * @brief	  ßlQ[EáQ¨
 * @author	ariizumi
 * @data	  09/11/24
 *
 * W[¼FMB_CAP_BALL
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

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct _MB_CAP_BALL MB_CAP_BALL;

typedef struct
{
  BOOL isBonus;
  VecFx32 targetPos;
}MB_CAP_BALL_INIT_WORK;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
extern MB_CAP_BALL *MB_CAP_BALL_CreateObject( MB_CAPTURE_WORK *capWork , MB_CAP_BALL_INIT_WORK *initWork );
extern void MB_CAP_BALL_DeleteObject( MB_CAPTURE_WORK *capWork , MB_CAP_BALL *ballWork );
extern void MB_CAP_BALL_UpdateObject( MB_CAPTURE_WORK *capWork , MB_CAP_BALL *ballWork );

extern const BOOL MB_CAP_BALL_IsFinish( const MB_CAP_BALL *ballWork );

