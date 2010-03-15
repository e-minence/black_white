//============================================================================
/**
 *
 *	@file		app_keycursor.h
 *	@brief  ���b�Z�[�W�A�L�[�J�[�\���i���b�Z�[�W����A�C�R���j
 *	@author	hosaka genya(kagaya����̃\�[�X���Q�l�ɋ��ʉ�)
 *	@data		2010.01.05
 *
 */
//============================================================================
#pragma once

#include "print/printsys.h"

// �s���S�^
typedef struct _APP_KEYCURSOR_WORK APP_KEYCURSOR_WORK;

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//=============================================================================
/**
 *								EXTERN�錾
 */
//=============================================================================

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� ������
 *  @param n_col      [IN] �����F�w�� 0-15,GF_BMPPRT_NOTNUKI	
 *  @param is_decide_key  [IN] TRUE:����{�^���A�L�����Z���{�^���ŃJ�[�\���������B
 *  @param is_decide_tp   [IN] TRUE:�^�b�`�ŃJ�[�\���������B
 *  @param heapID     [IN] HEAPID �q�[�vID
 *  @retval nothing
 */
//--------------------------------------------------------------
extern APP_KEYCURSOR_WORK* APP_KEYCURSOR_Create( u16 n_col, BOOL is_decide_key, BOOL is_decide_tp, HEAPID heap_id );

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� ������
 *  @param n_col      [IN] �����F�w�� 0-15,GF_BMPPRT_NOTNUKI	
 *  @param is_decide_key  [IN] TRUE:����{�^���A�L�����Z���{�^���ŃJ�[�\���������B
 *  @param is_decide_tp   [IN] TRUE:�^�b�`�ŃJ�[�\���������B
 *  @param heapID     [IN] HEAPID �q�[�vID
 *  @param skip_cursor_character    [IN] �L�����G���A�ւ̃|�C���^(APP_KEYCURSOR_Delete����܂ŌĂяo�����ŕێ����邱��)
 *                                       (app_keycursor.c���ɂ���sc_skip_cursor_character���Q�l�ɗp�ӂ��ĉ�����)
 *  @retval nothing
 */
//--------------------------------------------------------------
extern APP_KEYCURSOR_WORK* APP_KEYCURSOR_CreateEx( u16 n_col, BOOL is_decide_key, BOOL is_decide_tp, HEAPID heap_id,
                                                   u8* skip_cursor_character );

//--------------------------------------------------------------
/**
 *  @brief  �L�[����J�[�\�� �폜
 *  @param  work ���C�����[�N
 *  @retval nothing
 */
//--------------------------------------------------------------
extern void APP_KEYCURSOR_Delete( APP_KEYCURSOR_WORK *work );

//-----------------------------------------------------------------------------
/**
 *	@brief  �L�[�J�[�\�� �又��
 *
 *	@param	work ���C�����[�N
 *	@param	stream �v�����g�X�g���[��
 *	@note  ���b�Z�[�W�\�����ɖ�SYNC�ĂԁBstream��NULL�̎��͌Ă΂Ȃ��悤�ɂ���B
 *
 *	@retval nothing
 */
//-----------------------------------------------------------------------------
extern void APP_KEYCURSOR_Main( APP_KEYCURSOR_WORK* work, PRINT_STREAM* stream, GFL_BMPWIN* msgwin );


//====================================================================================
// �ȉ��AMain�ɓ����Ă���̂ŁA��{�I�ɂ͊O������g�p���Ȃ��֐��B�O�̂��ߌ��J
//====================================================================================


//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �N���A
 * @param work  ���C�����[�N
 * @param bmp   �\����GFL_BMP_DATA
 * @param n_col �����F�w�� 0-15,GF_BMPPRT_NOTNUKI	
 * @retval nothing
 */
//--------------------------------------------------------------
extern void APP_KEYCURSOR_Clear( APP_KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col );

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �\��
 * @param work APP_KEYCURSOR_WORK
 * @param bmp �\����GFL_BMP_DATA
 * @param n_col �����F�w�� 0-15,GF_BMPPRT_NOTNUKI	
 * @retval nothing
 */
//--------------------------------------------------------------
extern void APP_KEYCURSOR_Write( APP_KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col );

