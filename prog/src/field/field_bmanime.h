//============================================================================================
/**
 * @file  field_bmanime.h
 * @brief �t�B�[���h�z�u���f���A�j������
 * @date  2009.05.11
 * @author  tamada GAME FREAK inc.
 */
//============================================================================================

#pragma once

//============================================================================================
//============================================================================================

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _FIELD_BMANIME_DATA FIELD_BMANIME_DATA;

//------------------------------------------------------------------
/**
 * @brief �z�u���f���A�j���̓K�p�^�C�v�w��
 */
//------------------------------------------------------------------
typedef enum {  
  BMANIME_TYPE_NONE = 0,    ///<�A�j���Ȃ�
  BMANIME_TYPE_ETERNAL,     ///<��ɓK�p
  BMANIME_TYPE_EVENT,       ///<�C�x���g�œK�p

  BMANIME_TYPE_MAX
}BMANIME_TYPE;

//------------------------------------------------------------------
/**
 * @brief �z�u���f���A�j���̓���^�C�v�w��
 */
//------------------------------------------------------------------
typedef enum {  
  BMANIME_PROG_TYPE_NONE = 0,   ///<���ɓ���Ȃ�
  BMANIME_PROG_TYPE_ELBOARD1,   ///<�d���f�����̂P
  BMANIME_PROG_TYPE_ELBOARD2,   ///<�d���f�����̂Q

  BMANIME_PROG_TYPE_MAX,
}BMANIME_PROG_TYPE;


//------------------------------------------------------------------
//�A�j���f�[�^�̎擾����
//------------------------------------------------------------------
extern const FIELD_BMANIME_DATA * FIELD_BMODEL_MAN_GetAnimeData(FIELD_BMODEL_MAN * man, u16 bm_id);

extern ARCID FIELD_BMODEL_MAN_GetAnimeArcID(const FIELD_BMODEL_MAN * man);

extern const u16 * FIELD_BMANIME_DATA_getAnimeFileID(const FIELD_BMANIME_DATA * data);
extern u32 FIELD_BMANIME_DATA_getAnimeCount(const FIELD_BMANIME_DATA * data);

extern BMANIME_TYPE FIELD_BMANIME_DATA_getAnimeType(const FIELD_BMANIME_DATA * data);

extern BMANIME_PROG_TYPE FIELD_BMANIME_DATA_getProgType(const FIELD_BMANIME_DATA * data);

//------------------------------------------------------------------
//  �e�N�X�`�����̓o�^����
//------------------------------------------------------------------
extern void FIELD_BMANIME_DATA_entryTexData(FIELD_BMODEL_MAN* man, const FIELD_BMANIME_DATA * data,
    const GFL_G3D_RES * g3DresTex);

