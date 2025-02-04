//======================================================================
/**
 * @file	  mb_cap_obj.c
 * @brief	  ßlQ[EáQ¨
 * @author	ariizumi
 * @data	  09/11/24
 *
 * W[¼FMB_CAP_OBJ
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
typedef enum
{
  MCOT_GRASS,
  MCOT_GRASS_SIDE,
  MCOT_WOOD,
  MCOT_WATER,
  MCOT_STAR,
  
  MCOT_MAX,
}MB_CAP_OBJ_TYPE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct
{
  MB_CAP_OBJ_TYPE type;
  VecFx32     pos;
}MB_CAP_OBJ_INIT_WORK;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
extern MB_CAP_OBJ *MB_CAP_OBJ_CreateObject( MB_CAPTURE_WORK *capWork , MB_CAP_OBJ_INIT_WORK *initWork );
extern void MB_CAP_OBJ_DeleteObject( MB_CAPTURE_WORK *capWork , MB_CAP_OBJ *objWork );
extern void MB_CAP_OBJ_UpdateObject( MB_CAPTURE_WORK *capWork , MB_CAP_OBJ *objWork );
extern void MB_CAP_OBJ_UpdateObject_Star( MB_CAPTURE_WORK *capWork , MB_CAP_OBJ *objWork );
extern void MB_CAP_OBJ_GetHitWork( MB_CAP_OBJ *objWork , MB_CAP_HIT_WORK *hitWork );
extern void MB_CAP_OBJ_StartAnime( MB_CAP_OBJ *objWork );
extern void MB_CAP_OBJ_SetEnable( MB_CAPTURE_WORK *capWork , MB_CAP_OBJ *objWork , const BOOL flg );
extern const BOOL MB_CAP_OBJ_GetEnable( MB_CAPTURE_WORK *capWork , MB_CAP_OBJ *objWork );

