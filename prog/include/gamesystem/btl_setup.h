//=============================================================================================
/**
 * @file  btl_setup.h
 * @brief �|�P����WB �o�g���p�����[�^�ȈՃZ�b�g�A�b�v����
 * @author  taya
 *
 * @date  2009.10.01 �쐬
 */
//=============================================================================================
#pragma once

#include "battle/battle.h"
#include "gamesystem/game_data.h"
#include "savedata/battle_rec.h"

/*
 *  @brief  �퓬�p�����[�^���[�N����
 *  @param  heapID  ���[�N���������m�ۂ���heapID
 *
 *  ��BATTLE_SETUP_PARAM�\���̗̈���A���P�[�g���A���������܂�
 *  ���f�t�H���g�̃p�����[�^�ō\���̂����������܂��B�o�g���^�C�v�ɉ����āA�K�v�ȏ�������ǉ��ōs���Ă�������
 *  ���K�� BATTLE_PARAM_Delete()�ŉ�����Ă�������
 */
extern BATTLE_SETUP_PARAM* BATTLE_PARAM_Create( HEAPID heapID );

/*
 *  @brief  �퓬�p�����[�^���[�N���
 *  @param  bp  BATTLE_PARAM_Create()�Ő������ꂽBATTLE_SETUP_PARAM�\���̌^���[�N�ւ̃|�C���^
 */
extern void BATTLE_PARAM_Delete( BATTLE_SETUP_PARAM* bp );

/**
 *  @brief  �퓬�p�����[�^���[�N�̓�������������
 *  @param  bp  �m�ۍς݂�BATTLE_SETUP_PARAM�\���̌^���[�N�ւ̃|�C���^
 *
 *  ���f�t�H���g�̃p�����[�^�ō\���̂����������܂��B�o�g���^�C�v�ɉ����āA�K�v�ȏ�������ǉ��ōs���Ă�������
 *  ���g���I�������K��BATTLE_PARAM_Release()�֐��ŉ�����������Ă�������
 */
extern void BATTLE_PARAM_Init( BATTLE_SETUP_PARAM* bp );

/*
 *  @brief  �퓬�p�����[�^���[�N�̓����A���P�[�V��������������ƃN���A
 *  @param  bp  �m�ۍς݂�BATTLE_SETUP_PARAM�\���̌^���[�N�ւ̃|�C���^
 */
extern void BATTLE_PARAM_Release( BATTLE_SETUP_PARAM* bp );

/*
 *  @brief  �o�g���p�����@PokeParty�f�[�^�Z�b�g
 *
 *  �n���ꂽ�|�C���^�̓��e���R�s�[���܂�
 */
extern void BATTLE_PARAM_SetPokeParty( BATTLE_SETUP_PARAM* bp, const POKEPARTY* party, BTL_CLIENT_ID client );

/*
 *  @brief  �o�g���p�����@PokeParty�|�C���^�擾
 */
extern POKEPARTY* BATTLE_PARAM_GetPokePartyPointer( BATTLE_SETUP_PARAM* bp, BTL_CLIENT_ID client );

/*
 *  @brief  �o�g���p�����@�o�g���X�e�[�^�X�t���O�Z�b�g
 */
extern void BATTLE_PARAM_SetBtlStatusFlag( BATTLE_SETUP_PARAM* bp, BTL_STATUS_FLAG status_f);

/*
 *  @brief  �o�g���p���� �o�g���X�e�[�^�X�t���O�`�F�b�N
 */
extern BOOL BATTLE_PARAM_CheckBtlStatusFlag( BATTLE_SETUP_PARAM* bp, BTL_STATUS_FLAG status_f );

////////////////////////////////////////////////////////////////////////
//
/*
 *  @brief  �퓬�t�B�[���h�V�`���G�[�V�����f�[�^�f�t�H���g������
 */
extern void BTL_FIELD_SITUATION_Init( BTL_FIELD_SITUATION* sit );


/*
 *  @brief  �Z�b�g�A�b�v�ς݃p�����[�^���o�g���T�u�E�F�C���[�h�p�ɐ؂�ւ�
 */
extern void BTL_SETUP_SetSubwayMode( BATTLE_SETUP_PARAM* dst );


/*
 *  @brief  �Z�b�g�A�b�v�ς݃p�����[�^�ɘ^��o�b�t�@�𐶐�
 */
extern void BTL_SETUP_AllocRecBuffer( BATTLE_SETUP_PARAM* dst, HEAPID heapID );


/*
 *  @brief  �Z�b�g�A�b�v�ς݃p�����[�^��^��Đ����[�h�ɐ؂�ւ�
 */
extern void BTL_SETUP_SetRecordPlayMode( BATTLE_SETUP_PARAM* dst );

////////////////////////////////////////////////////////////////////////
/*
 * BATTLE_SETUP_PARAM���������[�e�B���e�B�֐��Q
 *
 *  BTL_SETUP�n�֐��́A�����Ń������A���P�[�V�������s���܂�
 *  �o�g�����I��������A�K��BATTLE_PARAM_Release()orBATTLE_PARAM_Delete()�֐��ŉ�����s���Ă�������
 */
////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////
//���[�J���o�g���n
//////////////////////////////////////////////
extern void BTL_SETUP_Wild( BATTLE_SETUP_PARAM* bp, GAMEDATA* gdata,
  const POKEPARTY* partyEnemy, const BTL_FIELD_SITUATION* sit, const BtlRule rule, HEAPID heapID );

extern void BTL_SETUP_Single_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID );

extern void BTL_SETUP_Double_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID );

extern void BTL_SETUP_Triple_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID );

extern void BTL_SETUP_Rotation_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID );

extern void BTL_SETUP_Tag_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID tr_id0, TrainerID tr_id1, HEAPID heapID );

extern void BTL_SETUP_AIMulti_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID partner, TrainerID tr_id0, TrainerID tr_id1, HEAPID heapID );

extern void BTL_SETUP_CaptureDemo( BATTLE_SETUP_PARAM* bp, GAMEDATA* gameData,
  const POKEPARTY* partyMine, const POKEPARTY* partyEnemy, const BTL_FIELD_SITUATION* sit, HEAPID heapID );

//////////////////////////////////////////////
//�ʐM�o�g���n
//////////////////////////////////////////////
extern void BTL_SETUP_Single_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID );

extern void BTL_SETUP_Double_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID );

extern void BTL_SETUP_Multi_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, u8 commPos, HEAPID heapID );

extern void BTL_SETUP_Triple_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID );

extern void BTL_SETUP_Rotation_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID );


//////////////////////////////////////////////
// �^��f�[�^����
//////////////////////////////////////////////
extern void BTL_SETUP_InitForRecordPlay( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData, HEAPID heapID );
extern void BTL_SETUP_QuitForRecordPlay( BATTLE_SETUP_PARAM* bsp );

//-----------------------------------------------------------------------------
/*
 * �ȉ��f�o�b�O�p�֗��@�\���������邽�߂̃Z�b�g�A�b�v
 */
//-----------------------------------------------------------------------------
#ifdef PM_DEBUG

extern void BTL_SETUP_SetDebugFlag( BATTLE_SETUP_PARAM* dst, BtlDebugFlag flag );
extern BOOL BTL_SETUP_GetDebugFlag( const BATTLE_SETUP_PARAM* param, BtlDebugFlag flag );


#endif
