#ifndef	_TRAINER_CARD_H_
#define _TRAINER_CARD_H_

#include "buflen.h"
#include "savedata/playtime.h"

#define SIGN_SIZE_X	(24)
#define SIGN_SIZE_Y	(8)

#define COUNT_LV0	(100)
#define COUNT_LV1	(140)
#define COUNT_LV2	(170)
#define COUNT_LV3	(190)
#define COUNT_LV4	(200)

#define UNION_TR_NONE	(0xff)

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
	u8 Version;			//�o�[�W����
	u8 CountryCode;		//���R�[�h
	u8 GymReaderMask;	//�W�����[�_�[�W�l��}�X�N�t���O
	u8 CardRank;		//�J�[�h�����N
	
	u8 TimeUpdate:1;	//���ԍX�V�t���O
	u8 TrSex:1;			//����
	u8 PokeBookFlg:1;	//�}�ӏ����t���O
	u8 MySignValid:1;	//�T�C���f�[�^�L��/�����t���O
	u8 Padding:4;
	u8 UnionTrNo;		//���j�I���g���[�i�[�i���o�[�i0�`15�j�w�薳���̂Ƃ���UNION_TR_NONE(0xff)

	u16	BadgeFlag;		//�o�b�W����t���O(16bit)
	
	STRCODE TrainerName[PERSON_NAME_SIZE+EOM_SIZE];	//�g���[�i�[��
	const PLAYTIME *PlayTime;	//�v���C���ԍ\���́i�ʐM���͎��ԍX�V���s���Ȃ��̂�NULL���Z�b�g����j
	
	u32 Money;			//�����Â���
	u32 PokeBook;		//�}�Ӄ����X�^�[��
	u32 Score;			//�X�R�A
	u16 TrainerID;		//�g���[�i�[ID
	
	u16 PlayTime_h;		//�v���C���ԁi���j
	u16 ClearTime_h;	//�N���A���ԁi���j
	
	u8 PlayTime_m;		//�v���C���ԁi���j
	u8 Start_y;			//�J�n�N
	u8 Start_m;			//�J�n��
	u8 Start_d;			//�J�n��
	
	u8 Clear_y;			//�N���A�N
	u8 Clear_m;			//�N���A��
	u8 Clear_d;			//�N���A��
	u8 ClearTime_m;		//�N���A���ԁi���j
	
	u32 CommNum;		//�ʐM��
	u32 CommBattleWin;	//�ΐ폟����
	u32 CommBattleLose;	//�ΐ핉����
	u32 CommTrade;		//������

	u8	SignRawData[SIGN_SIZE_X*SIGN_SIZE_Y*8];	//�T�C���f�[�^
	u16	_xor;			// ���ؗp
	u16 reached;		// �����t���O(�ʐM���Ɏg�p�j
}TR_CARD_DATA;

typedef struct TRCARD_CALL_PARAM_tag{
	TR_CARD_DATA		*TrCardData;
	int					value;		///<���^�[���l
}TRCARD_CALL_PARAM;

FS_EXTERN_OVERLAY(trainercard);
#define TRCARD_OVERLAY_ID (FS_OVERLAY_ID(trainercard))

//�f�[�^�̎擾
extern void TRAINERCARD_GetSelfData( TR_CARD_DATA *cardData , const BOOL isSendData );


//�ʐM�p�Ǝ����p��Proc�Ăяo���B
extern void TRAINERCASR_CallProcSelfData( void );
//�ʐM�p��TR_CARD_DATA�̃|�C���^��n��
extern void TRAINERCASR_CallProcCommData( void* pCardData );

extern const GFL_PROC_DATA TrCardSysProcData;
//���������Ⴄ�ʐM�pProc�f�[�^
extern const GFL_PROC_DATA TrCardSysCommProcData;

#endif //_TRAINER_CARD_H_
