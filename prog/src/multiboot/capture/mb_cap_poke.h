//======================================================================
/**
 * @file	  mb_cap_poke.h
 * @brief	  �ߊl�Q�[���E�|�P����
 * @author	ariizumi
 * @data	  09/11/24
 *
 * ���W���[�����FMB_CAP_POKE
 */
//======================================================================
#pragma once

#include "poke_tool/poke_tool.h"
#include "multiboot/mb_data_def.h"
#include "multiboot/mb_local_def.h"

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
typedef struct _MB_CAP_POKE MB_CAP_POKE;

typedef struct
{
  HEAPID      heapId;
  GFL_BBD_SYS *bbdSys;
  ARCHANDLE   *arcHandle;
  ARCHANDLE   *pokeArcHandle;

  DLPLAY_CARD_TYPE cardType;
  
  POKEMON_PASO_PARAM *ppp;
  
}MB_CAP_POKE_INIT_WORK;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
extern MB_CAP_POKE *MB_CAP_POKE_CreateObject( MB_CAP_POKE_INIT_WORK *initWork );
extern void MB_CAP_POKE_DeleteObject( MB_CAP_POKE *mbPoke );

