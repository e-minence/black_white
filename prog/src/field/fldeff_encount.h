//======================================================================
/**
 * @file	fldeff_encount.h
 * @brief	�t�B�[���h �G�t�F�N�g�G���J�E���g�p�G�t�F�N�g�Q
 * @author  Miyuki Iwasawa	
 * @date	09.11.18
 */
//======================================================================
#pragma once
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

//======================================================================
//  define
//======================================================================
typedef enum{
 EFFENC_TYPE_SGRASS,  ///<�Z����
 EFFENC_TYPE_LGRASS,  ///<������
 EFFENC_TYPE_CAVE,    ///<���A
 EFFENC_TYPE_WATER,   ///<�W��
 EFFENC_TYPE_SEA,     ///<�C
 EFFENC_TYPE_BRIDGE,  ///<��
 EFFENC_TYPE_MAX,
}EFFENC_TYPE_ID;

//======================================================================
//  struct
//======================================================================

//======================================================================
//  extern
//======================================================================
extern void * FLDEFF_ENCOUNT_SGrassInit( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void * FLDEFF_ENCOUNT_LGrassInit( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void * FLDEFF_ENCOUNT_CaveInit( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void * FLDEFF_ENCOUNT_WaterInit( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void * FLDEFF_ENCOUNT_SeaInit( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void * FLDEFF_ENCOUNT_BridgeInit( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void FLDEFF_ENCOUNT_Delete( FLDEFF_CTRL *fectrl, void *work );

extern FLDEFF_TASK* FLDEFF_ENCOUNT_SetEffect( FIELD_ENCOUNT* fld_enc, FLDEFF_CTRL *fectrl, u16 gx, u16 gz, fx32 height, EFFENC_TYPE_ID type );
extern void FLDEFF_ENCOUNT_AnmPauseSet( FLDEFF_TASK* task, BOOL pause_f );

