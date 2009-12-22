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

/**
 *  @brief  �G�t�F�N�g�G���J�E���g�@�}�b�v�`�F���W���̏�ԃN���A
 */
extern void EFFECT_ENC_MapChangeUpdate( GAMEDATA* gdata );

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@���W�`�F�b�N
 */
extern BOOL EFFECT_ENC_CheckEffectPos( const FIELD_ENCOUNT* enc, MMDL_GRIDPOS* pos );

/*
 *  @brief  �G�t�F�N�g�G���J�E���g�@�C�x���g�N���N���`�F�b�N
 */
extern GMEVENT* EFFECT_ENC_CheckEventApproch( FIELD_ENCOUNT* enc );

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

