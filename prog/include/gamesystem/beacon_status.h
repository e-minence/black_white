/*
 *  @file   beacon_status.c
 *  @brief  �Q�[�����ێ�����r�[�R�����O�Ǘ�
 *  @author Miyuki Iwasawa
 *  @date   10.01.21
 */
#pragma once

#include "gamesystem/game_beacon.h"

typedef struct _TAG_BEACON_STATUS BEACON_STATUS;

//--------------------------------------------------------------
/**
 * @brief �r�[�R���X�e�[�^�X���[�N�擾
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @param   BEACON_STATUS*
 *
 * ���̂�src/gamesystem/game_data.c�ɂ��邪�A���B����
 * �l�����ăA�N�Z�X�֐��Q�������w�b�_�ɔz�u���Ă���
 */
//--------------------------------------------------------------
extern BEACON_STATUS* GAMEDATA_GetBeaconStatus( GAMEDATA * gamedata );

/*
 *  @brief  �r�[�R���X�e�[�^�X���[�N����
 */
extern BEACON_STATUS* BEACON_STATUS_Create( HEAPID heapID, HEAPID tmpHeapID );

/*
 *  @brief  �r�[�R���X�e�[�^�X���[�N�j��
 */
extern void BEACON_STATUS_Delete( BEACON_STATUS* wk );

/*
 *  @brief  InfoTbl���擾
 */
extern GAMEBEACON_INFO_TBL* BEACON_STATUS_GetInfoTbl( BEACON_STATUS* wk );

/*
 *  @brief  �r���[���X�g�̃g�b�v�I�t�Z�b�g���擾
 */
extern u8  BEACON_STATUS_GetViewTopOffset( BEACON_STATUS* wk );

/*
 *  @brief  �r���[���X�g�̃g�b�v�I�t�Z�b�g���Z�b�g
 */
extern void BEACON_STATUS_SetViewTopOffset( BEACON_STATUS* wk, u8 ofs );

/*
 *  @brief  �t���[���[�h�o�b�t�@�̃A�h���X���擾
 */
extern STRBUF* BEACON_STATUS_GetFreeWordBuffer( BEACON_STATUS* wk );

/*
 *  @brief  �t���[���[�h�o�b�t�@�̓��̓t���O�A�h���X���擾
 */
extern u16* BEACON_STATUS_GetFreeWordInputResultPointer( BEACON_STATUS* wk );


#ifdef PM_DEBUG

//------------------------------------------------------------
/*
 *  @brief  �f�o�b�O�p�X�^�b�N�`�F�b�N�X���[�t���O�Z�b�g
 */
//------------------------------------------------------------
extern void DEBUG_BEACON_STATUS_SetStackCheckThrowFlag( BEACON_STATUS* wk, BOOL flag );

//------------------------------------------------------------
/*
 *  @brief  �f�o�b�O�p�X�^�b�N�`�F�b�N�X���[�t���O�Q�b�g
 */
//------------------------------------------------------------
extern BOOL DEBUG_BEACON_STATUS_GetStackCheckThrowFlag( BEACON_STATUS* wk );

#endif  //PM_DEBUG
