//============================================================================================
/**
 * @file  trcard_card.h
 * @bfief �g���[�i�[�J�[�h�A�v���ďo�p���[�N
 * @author  Akito Mori / Nozomu Saito (WB�ڐA�͗L�򂭂�)
 * @date  10.03.09
 */
//============================================================================================
#ifndef _TRAINER_CARD_H_
#define _TRAINER_CARD_H_

#include "gamesystem/game_data.h"
#include "buflen.h"
#include "savedata/playtime.h"
#include "system/pms_data.h"

#define SIGN_SIZE_X (24)
#define SIGN_SIZE_Y (8)

#define COUNT_LV0 (100)
#define COUNT_LV1 (140)
#define COUNT_LV2 (170)
#define COUNT_LV3 (190)
#define COUNT_LV4 (200)

#define UNION_TR_NONE (0xff)

#define TRAINERCARD_NEXTPROC_RETURN   ( 0 ) // �t�B�[���h���j���[�ɖ߂�
#define TRAINERCARD_NEXTPROC_EXIT     ( 1 ) // �t�B�[���h�ɒ��ږ߂�

typedef enum{
  TR_CARD_RANK_NORMAL = 0,
  TR_CARD_RANK_BRO,
  TR_CARD_RANK_KAP,
  TR_CARD_RANK_SIL,
  TR_CARD_RANK_GOL,
  TR_CARD_RANK_BLACK,
}TR_CARD_RANK;

typedef struct TR_CARD_DATA_tag
{
  u8 Version;       //�o�[�W����
  u8 CountryCode;   //���R�[�h
  u8 GymReaderMask; //�W�����[�_�[�W�l��}�X�N�t���O
  u8 CardRank;      //�J�[�h�����N
  
  u8 TimeUpdate:1;  //���ԍX�V�t���O
  u8 TrSex:1;       //����
  u8 PokeBookFlg:1; //�}�ӏ����t���O
  u8 MySignValid:1; //�T�C���f�[�^�L��/�����t���O
  u8 SignAnimeOn:1; // �T�C���f�[�^�̓A�j�����Ă��邩�H�i0:���ĂȂ��@1:���Ă�j
  u8 SignDisenable:1; // �T�C���������Ȃ�����H�i0:�T�C���͏����� 1:������������ �j
  u8 EditPossible:1;  // �ҏW�\���H�iTRUE:OK�@FALSE:NG)
  u8 OtherTrCard:1;   // ���l�̃J�[�h�����Ă���iTRUE:���l�̃J�[�h FALSE�F�����̃J�[�h)
  u8 Padding:1;
  u8 UnionTrNo;     //���j�I���g���[�i�[�i���o�[�i0�`15�j�w�薳���̂Ƃ���UNION_TR_NONE(0xff)

  u16 BadgeFlag;    //�o�b�W����t���O(16bit)
  
  STRCODE TrainerName[PERSON_NAME_SIZE+EOM_SIZE]; //�g���[�i�[��
  const PLAYTIME *PlayTime; //�v���C���ԍ\���́i�ʐM���͎��ԍX�V���s���Ȃ��̂�NULL���Z�b�g����j
  
  u32 Money;        //�����Â���
  u32 PokeBook;     //�}�Ӄ����X�^�[��
  u16 TrainerID;    //�g���[�i�[ID
  
  u16 PlayTime_h;   //�v���C���ԁi���j
  u16 ClearTime_h;  //�N���A���ԁi���j
  
  u8 PlayTime_m;    //�v���C���ԁi���j
  u8 Start_y;       //�J�n�N
  u8 Start_m;       //�J�n��
  u8 Start_d;       //�J�n��
  
  u8 Clear_y;       //�N���A�N
  u8 Clear_m;       //�N���A��
  u8 Clear_d;       //�N���A��
  u8 ClearTime_m;   //�N���A���ԁi���j
  
  u16 CommNum;        //�ʐM��
  u16 CommBattleNum;  // 
  u16 CommBattleWin;  //�ΐ폟����
  u16 CommBattleLose; //�ΐ핉����
  u32 Personality;    //���i
  u32 CommTrade;      //������
  
  u32 EncountNum;        // �쐶�G���J�E���g��
  u32 TrainerEncountNum; // �g���[�i�[���������
  u32 SuretigaiNum;      // ����Ⴂ�ʐM��������
  u16 FeelingCheckNum;   // �t�B�[�����O�`�F�b�N��������
  u16 MonolithLevel;     // ���m���X���x��
  u16 MusicalNum;        // �~���[�W�J����������
  u16 PokeshifterHigh;   // �|�P�V�t�^�[�̃n�C�X�R�A
  u16 TrialHouseHigh;    // �g���C�A�E�n�E�X�̍ō����_
  u16 TrialHouseRank;    // �g���C�A���n�E�X�̃����N

  u32 CgearGetFlag:1;    // C�M�A�������Ă��邩
  u32 PaleceFlag:1;      // �p���X�ɍs�����������邩
  u32 MusicalFlag:1;     // �~���[�W�J���������������邩
  u32 PokeshifterFlag:1; // �|�P�V�t�^�[�������������邩
  u32 TrianHouseFlag:1;  // �g���C�A���n�E�X�ɎQ�������������邩
  u32 Padding2:27;      


  PMS_DATA Pms;       //�g���[�i�[�J�[�h���ȈՉ�b
  
  u8  SignRawData[SIGN_SIZE_X*SIGN_SIZE_Y*8]; //�T�C���f�[�^
  u16 _xor;         // ���ؗp
  u16 reached;      // �����t���O(�ʐM���Ɏg�p�j
}TR_CARD_DATA;

typedef struct TRCARD_CALL_PARAM_tag{
  TR_CARD_DATA  *TrCardData;    ///< �g���[�i�[�J�[�h�f�[�^�������Ă���ꍇ�̃|�C���^(�ʐM���Ȃǁj
  GAMEDATA      *gameData;      ///< �Q�[���f�[�^�A�N�Z�X�p�|�C���^
  BOOL          edit_possible;  ///< �ҏW�\���H�iTRUE:�\�EFALSE�F�s�\�j
  int           value;          ///< ���^�[���l
  int           next_proc;      ///< �g���[�i�[�J�[�h�I�����̖߂�l
}TRCARD_CALL_PARAM;

FS_EXTERN_OVERLAY(trainercard);
#define TRCARD_OVERLAY_ID (FS_OVERLAY_ID(trainercard))

//�f�[�^�̎擾
void TRAINERCARD_GetSelfData( 
      TR_CARD_DATA *cardData , GAMEDATA *gameData , const BOOL isSendData, BOOL edit, HEAPID heapId );


//�ʐM�p�Ǝ����p��CallParam�쐬
extern TRCARD_CALL_PARAM* TRAINERCASR_CreateCallParam_SelfData( GAMEDATA *gameData , HEAPID heapId, BOOL edit );
extern TRCARD_CALL_PARAM* TRAINERCASR_CreateCallParam_CommData( GAMEDATA *gameData , void* pCardData , HEAPID heapId );

extern const GFL_PROC_DATA TrCardSysProcData;
//���������Ⴄ�ʐM�pProc�f�[�^
extern const GFL_PROC_DATA TrCardSysCommProcData;

#endif //_TRAINER_CARD_H_
