//============================================================================================
/**
 * @file	field_buildmodel.h
 * @brief		�z�u���f���̐���
 * @author	tamada GAMEFREAK inc.
 * @date	2009.04.24
 */
//============================================================================================
#pragma once

#include <gflib.h>

#include "field/areadata.h"
#include "field_g3d_mapper.h"

#include "map/dp3format.h"
#include "field_hit_check.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �z�u���f��ID�̌^��`
 */
//------------------------------------------------------------------
typedef u16 BMODEL_ID;
//------------------------------------------------------------------
//  �z�u���f���}�l�W���[
//------------------------------------------------------------------
typedef struct _FIELD_BMODEL_MAN FIELD_BMODEL_MAN;

//============================================================================================
//
//  ���C�������֐��Q
//
//============================================================================================
//------------------------------------------------------------------
//  ����
//------------------------------------------------------------------
extern FIELD_BMODEL_MAN * FIELD_BMODEL_MAN_Create(HEAPID heapID, FLDMAPPER * fldmapper);

//------------------------------------------------------------------
//  �폜
//------------------------------------------------------------------
extern void FIELD_BMODEL_MAN_Delete(FIELD_BMODEL_MAN * man);

//------------------------------------------------------------------
//  �X�V����
//------------------------------------------------------------------
extern void FIELD_BMODEL_MAN_Main(FIELD_BMODEL_MAN * man);

//------------------------------------------------------------------
//  �`�揈��
//------------------------------------------------------------------
extern void FIELD_BMODEL_MAN_Draw(FIELD_BMODEL_MAN * man);

//------------------------------------------------------------------
//  �}�b�v�f�[�^�̓ǂݍ��ݏ���
//------------------------------------------------------------------
extern void FIELD_BMODEL_MAN_Load(FIELD_BMODEL_MAN * man, u16 zoneid, const AREADATA * areadata);

//------------------------------------------------------------------
/// �q�[�v�w��̎擾
//------------------------------------------------------------------
extern HEAPID FIELD_BMODEL_MAN_GetHeapID(const FIELD_BMODEL_MAN * man);

//============================================================================================
//
//  �z�u�o�^�֘A
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
extern GFL_G3D_MAP_GLOBALOBJ * FIELD_BMODEL_MAN_GetGlobalObjects(FIELD_BMODEL_MAN * man);

extern void FIELD_BMODEL_MAN_ResistAllMapObjects
(FIELD_BMODEL_MAN * man, GFL_G3D_MAP * g3Dmap, const PositionSt* objStatus, u32 objCount);

extern void FIELD_BMODEL_MAN_ReleaseAllMapObjects
(FIELD_BMODEL_MAN * man, GFL_G3D_MAP * g3Dmap);

extern void FIELD_BMODEL_MAN_ResistMapObject
(FIELD_BMODEL_MAN * man, GFL_G3D_MAP * g3Dmap, const PositionSt* objStatus, u32 objCount);

//============================================================================================
//
//  �A�j������֘A
//
//============================================================================================
typedef enum {
  BMANM_REQ_START = 0,
  BMANM_REQ_REVERSE_START,
  BMANM_REQ_STOP,
  BMANM_REQ_END,
}BMANM_REQUEST;


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/// �v���O�����w��ID
//------------------------------------------------------------------
typedef enum {
  BM_PROG_ID_NONE = 0,
  BM_PROG_ID_DOOR_NORMAL,
  BM_PROG_ID_DOOR_AUTO,
  BM_PROG_ID_BADGEGATE,
  BM_PROG_ID_SANDSTREAM,
  BM_PROG_ID_PCELEVATOR,

  BM_PROG_ID_MAX,
}BM_PROG_ID;
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef enum {
  BM_SEARCH_ID_NULL = 0,
  BM_SEARCH_ID_DOOR,
  BM_SEARCH_ID_SANDSTREAM,

  BM_SEARCH_ID_MAX,
}BM_SEARCH_ID;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _G3DMAPOBJST G3DMAPOBJST;
typedef struct _FIELD_BMODEL FIELD_BMODEL;

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_BMODEL_MAN_EntryBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel);
extern void FIELD_BMODEL_MAN_releaseBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern FIELD_BMODEL * FIELD_BMODEL_Create(FIELD_BMODEL_MAN * man, const G3DMAPOBJST * obj);
extern FIELD_BMODEL * FIELD_BMODEL_Create_Direct(
    FIELD_BMODEL_MAN * man, BMODEL_ID bmodel_id, const GFL_G3D_OBJSTATUS* status );
extern void FIELD_BMODEL_Delete(FIELD_BMODEL * bmodel);
extern void FIELD_BMODEL_SetAnime(FIELD_BMODEL * bmodel, u32 idx, BMANM_REQUEST req);
extern BOOL FIELD_BMODEL_GetAnimeStatus( const FIELD_BMODEL * bmodel, u32 idx);
extern BM_PROG_ID FIELD_BMODEL_GetProgID(const FIELD_BMODEL * bmodel);


//------------------------------------------------------------------
//------------------------------------------------------------------

extern G3DMAPOBJST ** FIELD_BMODEL_MAN_CreateObjStatusList
( FIELD_BMODEL_MAN* man, const FLDHIT_RECT * rect, BM_SEARCH_ID search, u32 * num );

extern void G3DMAPOBJST_changeViewFlag(G3DMAPOBJST * obj, BOOL flag);
extern void G3DMAPOBJST_setAnime( FIELD_BMODEL_MAN * man, G3DMAPOBJST * obj, u32 anm_idx, BMANM_REQUEST req );

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//  �����̃w�b�_�Ő錾���Ă��邪���Ԃ�field_g3d_mapper.c�ɂ���
//------------------------------------------------------------------
extern FIELD_BMODEL_MAN * FLDMAPPER_GetBuildModelManager( FLDMAPPER* g3Dmapper);


#ifdef PM_DEBUG

#define BMODEL_DEBUG_RESOURCE_MEMORY_SIZE // ON�Ń������T�C�Y���v�Z

#endif


//============================================================================================
//  DEBUG ���\�[�X�������g�p�ʂ̌���
//============================================================================================
#ifdef BMODEL_DEBUG_RESOURCE_MEMORY_SIZE

extern u32 FIELD_BMODEL_MAN_GetUseResourceMemorySize(void);

#endif

