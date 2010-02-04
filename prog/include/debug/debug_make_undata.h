//======================================================================
/**
 * @file    debug_make_undata.h
 * @brief   �f�o�b�O�p���A�f�[�^�쐬
 * @author  saito
 * @data    10/02/04
 */
//======================================================================
#pragma once

#include <procsys.h>
#include "savedata/wifihistory.h" //for WIFI_HISTORY
//--------------------------------------------------------------
/**
 *
 */
//--------------------------------------------------------------
typedef struct {

  WIFI_HISTORY *wh;

}PROCPARAM_DEBUG_MAKE_UNDATA;

//--------------------------------------------------------------
/**
 *  Proc Data
 */
//--------------------------------------------------------------
extern const GFL_PROC_DATA ProcData_DebugMakeUNData;


