//======================================================================
/**
 * @file    debug_makepoke.h
 * @brief   �f�o�b�O�p�|�P�����f�[�^�����c�[��
 * @author  taya
 * @data    09/05/29
 */
//======================================================================
#pragma once

#include <procsys.h>
#include "poke_tool\poke_tool.h"
#include "savedata\mystatus.h"

//--------------------------------------------------------------
/**
 *
 */
//--------------------------------------------------------------
typedef struct {

  POKEMON_PARAM*    dst;
  const MYSTATUS*   oyaStatus;
  u32               defaultMonsNo;

}PROCPARAM_DEBUG_MAKEPOKE;

//--------------------------------------------------------------
/**
 *  Proc Data
 */
//--------------------------------------------------------------
extern const GFL_PROC_DATA ProcData_DebugMakePoke;


