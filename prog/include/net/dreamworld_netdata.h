
//============================================================================================
/**
 * @file	  dreamworld_netdata.h
 * @brief	  �|�P�����h���[�����[���h�ɕK�v�ȃf�[�^
 * @author	k.ohno
 * @date	  2009.11.08
 */
//============================================================================================
#pragma once

#include "savedata/dreamworld_data.h"

#define DREAM_WORLD_SERVER_POKMEONLIST_MAX (1024)  //�Q��|�P�����̃��X�g�ő�

typedef enum{
  DREAM_WORLD_SERVER_ERROR_NONE,        // ����ł�
  DREAM_WORLD_SERVER_ERROR_SERVER_FULL,	// �T�[�o�[�̗e�ʃI�[�o�[�ł�
	DREAM_WORLD_SERVER_ALREADY_EXISTS,	  // �T�[�o�[�Ɋ��Ƀf�[�^������܂�
	DREAM_WORLD_SERVER_ILLEGAL_DATA,		// �A�b�v���[�h���ꂽ�f�[�^�����Ă��܂�
	DREAM_WORLD_SERVER_CHEAT_DATA,		  // �A�b�v���[�h���ꂽ�f�[�^���s���ł�
	DREAM_WORLD_SERVER_NO_DATA,		      // �T�[�o�Ƀf�[�^������܂���
  DREAM_WORLD_SERVER_END,             // �T�[�r�X���I�����Ă���
  DREAM_WORLD_SERVER_ETC_ERROR,       // ���̑��T�[�o���G���[�̏ꍇ ����ȏ�̒l�����Ă����l�ɃG���[�ɂ���
} DREAM_WORLD_SERVER_ERROR_TYPE;   /// �T�[�o����Ԃ��Ă��铚��

/// ��Ԋm�F�\����
struct DREAM_WORLD_SERVER_STATUS_DATA{
  DREAM_WORLD_SERVER_ERROR_TYPE type;   /// DREAM_WORLD_SERVER_ERROR_TYPE
};



/// �A�J�E���g�쐬�\����
struct DREAM_WORLD_SERVER_CREATE_DATA{
  u8 GameSyncID[12];   /// GameSyncID
};



/// �_�E�����[�h�f�[�^�\����
struct DREAM_WORLD_SERVER_DOWNLOAD_DATA{
	u32 RomCodeBit;   ///< ROM�R�[�h	4byte			ROM�̃o�[�W����		pm_version.h��BIT�Ǘ� ��v������󂯎��
	u32 PassCode;     ///<  ����C�x���g�p�N���L�[�R�[�h�i�ȒP�ɋN���ł��Ȃ��悤�ɈÍ����j
	u16 TreatType;    ///< 2byte		DREAM_WORLD_RESULT_TYPE	���̌��ʃf�[�^�̃J�e�S��
	DREAM_WORLD_TREAT_DATA TreatData;  ///< 2byte  DREAM_WORLD_TREAT_DATA�̃f�[�^
	DREAM_WORLD_FURNITUREDATA Furniture[DREAM_WORLD_DATA_MAX_FURNITURE];  ///<	26*5=130  �z�M�Ƌ�
	u16 CountryBit;   ///<	���R�[�h	2byte			����o�[�W����		pm_version.h��BIT�Ǘ� ��v������󂯎��
  u8 OnceBit;       ///<  ��񂾂����ǂ��� ����C�x���g��1�񂵂��󂯎��Ȃ��悤�ݒ肷��t���O
  u8 PokemonState;  ///< �Q�Ă���|�P�����̏��
};

/// �|�P�������X�g�f�[�^�\����
struct DREAM_WORLD_SERVER_POKEMONLIST_DATA{
  u8 pokemonList[DREAM_WORLD_SERVER_POKMEONLIST_MAX/8];  ///< �Q�����Ă悢�|�P����BITLIST
};
