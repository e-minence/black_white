/**
 *  @file   effect_encount.h
 *  @brief  �G���J�E���g�G�t�F�N�g
 *  @author Miyuki Iwasawa
 *  @date   09.11.17
 */

#pragma once

////////////////////////////////////////////////////////////////////////////
//
typedef struct _TAG_EFFECT_ENCOUNT EFFECT_ENCOUNT;

/**
 *  @brief  �G���J�E���g�G�t�F�N�g���[�N����
 */
extern EFFECT_ENCOUNT* EFFECT_ENC_CreateWork( FIELDMAP_WORK* fwork, HEAPID heapID );

/*
 *  @brief  �G���J�E���g�G�t�F�N�g���[�N�j��
 */
extern void EFFECT_ENC_DeleteWork( EFFECT_ENCOUNT* eff_wk );

/*
 *  @brief  �G���J�E���g�G�t�F�N�g�N���`�F�b�N
 */
extern void EFFECT_ENC_CheckEffectEncountStart( FIELD_ENCOUNT* enc );

/*
 *  @brief  �G�t�F�N�g�G���J�E���g  �����j���@
 */
extern void EFFECT_ENC_EffectDelete( FIELD_ENCOUNT* enc );

