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


//============================================================================================
//
//  �z�u�o�^�֘A
//
//============================================================================================
//------------------------------------------------------------------
//  �z�u���f��ID��o�^�ςݔz�u���f���̃C���f�b�N�X�ɕϊ�����
//------------------------------------------------------------------
extern u8 FIELD_BMODEL_MAN_GetEntryIndex(const FIELD_BMODEL_MAN* man, BMODEL_ID id);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern BOOL FIELD_BMODEL_MAN_GetSubModel(const FIELD_BMODEL_MAN * man, 
    u16 bm_id, VecFx32 * ofs, u32 * entryNo);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern GFL_G3D_MAP_GLOBALOBJ * FIELD_BMODEL_MAN_GetGlobalObjects(FIELD_BMODEL_MAN * man);

//============================================================================================
//
//  �A�j������֘A
//
//============================================================================================
typedef enum {
  BMANM_REQ_START = 0,
  BMANM_REQ_STOP,
  BMANM_REQ_END,
}BMANM_REQUEST;
//------------------------------------------------------------------
//------------------------------------------------------------------
extern BOOL FIELD_BMODEL_MAN_IsDoor(const FIELD_BMODEL_MAN * man, const GFL_G3D_MAP_GLOBALOBJ_ST * status);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void * FIELD_BMODEL_MAN_GetObjHandle(FIELD_BMODEL_MAN * man, const GFL_G3D_MAP_GLOBALOBJ_ST * status);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_BMODEL_MAN_SetAnime(FIELD_BMODEL_MAN * man, void * handle, u32 idx, BMANM_REQUEST req);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern BOOL FIELD_BMODEL_MAN_GetAnimeStatus(FIELD_BMODEL_MAN *man, void * handle, u32 idx);


//============================================================================================
//    �d���f���֘A
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �d���f���w��̂��߂̒�`
 */
//------------------------------------------------------------------
typedef enum{ 
  FIELD_BMODEL_ELBOARD_ID1 = 0,
  FIELD_BMODEL_ELBOARD_ID2,

  FIELD_BMODEL_ELBOARD_ID_MAX,
}FIELD_BMODEL_ELBOARD_ID;

//------------------------------------------------------------------
/**
 * @brief �d���f���p������o�^����
 */
//------------------------------------------------------------------
extern void FIELD_BMODEL_MAN_EntryELString(const FIELD_BMODEL_MAN* man,
    FIELD_BMODEL_ELBOARD_ID id,
    const STRBUF* str);

extern void FIELD_BMODEL_MAN_EntryELStringID(const FIELD_BMODEL_MAN * man,
    FIELD_BMODEL_ELBOARD_ID id,
    ARCID msg_arc_id, u16 str_id);


//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _G3DMAPOBJST G3DMAPOBJST;
typedef struct _FIELD_BMODEL FIELD_BMODEL;
//------------------------------------------------------------------
//------------------------------------------------------------------
extern FIELD_BMODEL * FIELD_BMODEL_Create(FIELD_BMODEL_MAN * man, const G3DMAPOBJST * obj);
extern void FIELD_BMODEL_Delete(FIELD_BMODEL * bmodel);
extern void FIELD_BMODEL_Draw( const FIELD_BMODEL * bmodel );
extern void * FIELD_BMODEL_GetObjHandle( FIELD_BMODEL * bmodel );
extern void FIELD_BMODEL_SetAnime(FIELD_BMODEL * bmodel, u32 idx, BMANM_REQUEST req);
extern BOOL FIELD_BMODEL_GetAnimeStatus(FIELD_BMODEL * bmodel, u32 idx);

extern void FIELD_BMODEL_MAN_EntryBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel);
extern void FIELD_BMODEL_MAN_releaseBuildModel(FIELD_BMODEL_MAN * man, FIELD_BMODEL * bmodel);

extern void FIELD_BMODEL_MAN_ResistAllMapObjects
(FIELD_BMODEL_MAN * man, GFL_G3D_MAP * g3Dmap, const PositionSt* objStatus, u32 objCount);

extern void FIELD_BMODEL_MAN_ReleaseAllMapObjects
(FIELD_BMODEL_MAN * man, GFL_G3D_MAP * g3Dmap);

extern void FIELD_BMODEL_MAN_ResistMapObject
(FIELD_BMODEL_MAN * man, GFL_G3D_MAP * g3Dmap, const PositionSt* objStatus, u32 objCount);

//------------------------------------------------------------------
//------------------------------------------------------------------

extern G3DMAPOBJST ** FIELD_BMODEL_MAN_CreateObjStatusList
( FIELD_BMODEL_MAN* man, const FLDHIT_RECT * rect, u32 * num );

extern BOOL FIELD_BMODEL_MAN_G3DMAPOBJSTisDoor
(const FIELD_BMODEL_MAN * man, const G3DMAPOBJST * obj);

extern void G3DMAPOBJST_changeViewFlag(G3DMAPOBJST * obj, BOOL flag);

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//  �����̃w�b�_�Ő錾���Ă��邪���Ԃ�field_g3d_mapper.c�ɂ���
//------------------------------------------------------------------
extern FIELD_BMODEL_MAN * FLDMAPPER_GetBuildModelManager( FLDMAPPER* g3Dmapper);

