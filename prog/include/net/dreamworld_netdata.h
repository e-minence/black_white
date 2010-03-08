
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
  DREAM_WORLD_SERVER_ILLEGAL_TOKEN,   // �F�؃g�[�N���ɖ�肪����
  DREAM_WORLD_SERVER_ERROR_ETC,       // ���̑��T�[�o���G���[�̏ꍇ ����ȏ�̒l�����Ă����l�ɃG���[�ɂ���
  DREAM_WORLD_SERVER_ERROR_MAX
} DREAM_WORLD_SERVER_ERROR_TYPE;   /// �T�[�o����Ԃ��Ă���T�[�o�̏��

typedef enum{
  DREAM_WORLD_SERVER_PLAY_EMPTY, //�|�P������a���Ă��Ȃ�
  DREAM_WORLD_SERVER_PLAY_NONE,        // �܂��V��ł��܂���
  DREAM_WORLD_SERVER_PLAY_ACTIVE,     //�V��ł���Œ��ł�
  DREAM_WORLD_SERVER_PLAY_END,	         // �h���[�����[���h�ŗV�т܂���
  DREAM_WORLD_SERVER_PLAY_MAX
} DREAM_WORLD_SERVER_PLAY_TYPE;   /// �T�[�o����Ԃ��Ă���V�тɊւ��Ă̏��



/// ��Ԋm�F�\����
typedef struct {
  u16 PlayStatus; // DREAM_WORLD_SERVER_PLAY_TYPE
  ///<  ���������LV1���J���ׂ̈̎���
  u16 findPokemon;         ///< �ł������|�P�����ԍ�
  u16 findPokemonTecnique; ///< �ł������|�P�����Ɋo��������Z
  u8 findPokemonForm;      ///< �ł������|�P�����̃t�H�����ԍ�
  u8 findPokemonSex;       ///< �ł������|�P��������    0=MALE 1=FEMALE 2=NEUTER 3=RANDOM
  u16 itemID[DREAM_WORLD_DATA_MAX_ITEMBOX];  ///< �����A�����A�C�e��
  u8 itemNum[DREAM_WORLD_DATA_MAX_ITEMBOX];  ///< �����A�����A�C�e���̌�
}DREAM_WORLD_SERVER_STATUS_DATA;



/// �A�J�E���g�쐬�\����
struct DREAM_WORLD_SERVER_CREATE_DATA{
  u8 GameSyncID[12];   /// GameSyncID
};



/// �_�E�����[�h�f�[�^�\����
typedef struct {
  u32 uploadCount;     ///< �A�b�v���[�h��
  u16 sleepPokemonLv;   ///< �Q�Ă����|�P�����̃��x���A�b�v�l
  u16 findPokemon;         ///< �ł������|�P�����ԍ�
  u16 findPokemonTecnique; ///< �ł������|�P�����Ɋo��������Z
  u8 findPokemonForm;      ///< �ł������|�P�����̃t�H�����ԍ�
  u8 findPokemonSex;       ///< �ł������|�P��������    0=MALE 1=FEMALE 2=NEUTER 3=RANDOM
  u8 sleepPokemonState;  ///< �Q�Ă���|�P�����̏��
  u8 musicalNo;      ///< �~���[�W�J������ web�őI�������ԍ�  �����ꍇ 0xff
  u8 cgearNo;        ///< CGEAR�摜   web�őI�������ԍ�  �����ꍇ 0xff
  u8 zukanNo;        ///< �}�Ӊ摜    web�őI�������ԍ�  �����ꍇ 0xff
  u8 signin;        ///<	��D���N���u�œo�^���I��������ǂ���  �o�^�ς݂Ȃ� 1
  u8 bGet;          ///<	���̃f�[�^�����łɎ󂯎�������ǂ��� 
  u16 itemID[DREAM_WORLD_DATA_MAX_ITEMBOX];  ///< �����A�����A�C�e��
  u8 itemNum[DREAM_WORLD_DATA_MAX_ITEMBOX];  ///< �����A�����A�C�e���̌�
  DREAM_WORLD_FURNITUREDATA Furniture[DREAM_WORLD_DATA_MAX_FURNITURE];  ///<	26*5=130  �z�M�Ƌ���
  u16 CRC;  /// ���̃f�[�^��CRC-16/CCITT
} DREAM_WORLD_SERVER_DOWNLOAD_DATA;


/// �_�E�����[�h������PDW�T�[�o�ɒm�点��\����
typedef struct {
  u8 bGet;          ///<	���̃f�[�^�����łɎ󂯎�������ǂ���
  u8 dummy;
} DREAM_WORLD_SERVER_DOWNLOAD_FINISH_DATA;



/// �|�P�������X�g�f�[�^�\����
typedef struct {
  u8 pokemonList[DREAM_WORLD_SERVER_POKMEONLIST_MAX/8];  ///< �Q�����Ă悢�|�P����BITLIST
} DREAM_WORLD_SERVER_POKEMONLIST_DATA;



typedef enum{
  DREAM_WORLD_MATCHUP_NONE,     // �o�^���Ă��Ȃ����
  DREAM_WORLD_MATCHUP_SIGNUP,	  // web�o�^�����BDS->SAKE�̐ڑ��͂܂�
  DREAM_WORLD_MATCHUP_ENTRY,	  // �Q����
  DREAM_WORLD_MATCHUP_END,	  // ���I��
  DREAM_WORLD_MATCHUP_RETIRE,		 // �r���ŉ���
} DREAM_WORLD_MATCHUP_TYPE;   /// ���E�ΐ�̐i�s��

//���L�\���̂ɏ������܂��A�������݃t���O
typedef enum
{ 
  DREAM_WORLD_ENTRYFLAG_GPF_WRITE,
  DREAM_WORLD_ENTRYFLAG_DS_WRITE,
} DREAM_WORLD_ENTRYFLAG;

/// ���E�ΐ�m�F�p�f�[�^�\����
typedef struct {
  u32 WifiMatchUpID;   ///< ���E�ΐ�̊J��ID
  u8 GPFEntryFlg;      ///< GPF���珑�����񂾂�DREAM_WORLD_ENTRYFLAG_GPF_WRITE DS���珑�����񂾂�DREAM_WORLD_ENTRYFLAG_DS_WRITE
  u8 WifiMatchUpState; ///< ���E�ΐ�̐i�s��
  u8 signin;        ///<	��D���N���u�œo�^���I��������ǂ���  �o�^�ς݂Ȃ� 1
  u8 dummy;
} DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA;


/// ���E�ΐ폑�����݃f�[�^�\����
typedef struct {
  u8 GPFEntryFlg;      ///< GPF���珑�����񂾂�DREAM_WORLD_ENTRYFLAG_GPF_WRITE DS���珑�����񂾂�DREAM_WORLD_ENTRYFLAG_DS_WRITE
  u8 WifiMatchUpState; ///< ���E�ΐ�̐i�s��
} DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA;


