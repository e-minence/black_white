/**
 *  @file   effect_encount.h
 *  @brief  �G���J�E���g�G�t�F�N�g
 *  @author Miyuki Iwasawa
 *  @date   09.11.17
 */

#pragma once

#include "gamesystem/gamedata_def.h"
#include "gamesystem/gamesystem.h"
#include "field_encount.h"

////////////////////////////////////////////////////////////////////////////
//
typedef struct _TAG_EFFECT_ENCOUNT EFFECT_ENCOUNT;

#define EFFENC_DEFAULT_INTERVAL (50)  //�G�t�F�N�g���I�f�t�H���g�C���^�[�o��
#define EFFENC_DEFAULT_PROB (20)  //�G�t�F�N�g���I�m���f�t�H���g

/**
 *  @brief  �G���J�E���g�G�t�F�N�g���[�N����
 */
extern EFFECT_ENCOUNT* EFFECT_ENC_CreateWork( HEAPID heapID );

/*
 *  @brief  �G���J�E���g�G�t�F�N�g���[�N�j��
 */
extern void EFFECT_ENC_DeleteWork( EFFECT_ENCOUNT* eff_wk );

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@�V�X�e���I������
 */
extern void EFFECT_ENC_End( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk );

/*
 *  @brief  �G�t�F�N�g�G���J�E���g �V�X�e��������
 */
extern void EFFECT_ENC_Init( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk );



/*
 *  @brief  �G���J�E���g�G�t�F�N�g�N���`�F�b�N
 */
extern void EFFECT_ENC_CheckEffectEncountStart( FIELD_ENCOUNT* enc );

/*
 *  @brief  �G�t�F�N�g�G���J�E���g  �����j���@
 */
extern void EFFECT_ENC_EffectDelete( FIELD_ENCOUNT* enc );

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@OBJ�Ƃ̐ڐG�ɂ��G�t�F�N�g�j���`�F�b�N
 */
extern void EFFECT_ENC_CheckObjHit( FIELD_ENCOUNT* enc );

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@���W�`�F�b�N
 */
extern BOOL EFFECT_ENC_CheckEffectPos( const FIELD_ENCOUNT* enc, MMDL_GRIDPOS* pos );

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@�C�x���g�N���N���`�F�b�N
 */
extern GMEVENT* EFFECT_ENC_CheckEventApproch( FIELD_ENCOUNT* enc );

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@���@�ʒu�Ƃ̋�����Ԃ�
 *
 *  @retval �G�t�F�N�g���Ȃ��Ƃ���FALSE��Ԃ�
 */
extern BOOL EFFECT_ENC_GetDistanceToPlayer( FIELD_ENCOUNT* enc, u16* o_distance );

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@�t�B�[���h�������G�t�F�N�g���A�����L�����Z��
 */
extern void EFFECT_ENC_EffectRecoverCancel( FIELD_ENCOUNT* enc );

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@�A�j���Đ��|�[�Y
 */
extern void EFFECT_ENC_EffectAnmPauseSet( FIELD_ENCOUNT* enc, BOOL pause_f );

/**
 * @brief   �G�t�F�N�g�G���J�E�g�A�C�e���擾�C�x���g�Ŏ�ɓ����ItemNo���擾����
 * @retval itemno
 */
extern u16 EFFECT_ENC_GetEffectEncountItem( FIELD_ENCOUNT* enc );


/////////////////////////////////////////////////////////////////////////////////////
//�f�o�b�O�p���[�`��
#ifdef PM_DEBUG
enum{
  EFFENC_DNI_INTERVAL,
  EFFENC_DNI_PROB,
  EFFENC_DNI_OFSX,
  EFFENC_DNI_OFSZ,
};

extern u32 EFFENC_DEB_NumInputParamGet( GAMESYS_WORK * gsys, GAMEDATA * gamedata, u32 param );
extern void EFFENC_DEB_NumInputParamSet( GAMESYS_WORK * gsys, GAMEDATA * gamedata, u32 param, u32 value );

#endif  //PM_DEBUG





