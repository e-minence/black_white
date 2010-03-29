//=============================================================================
/**
 * @file  trainerbadge_data.h
 * @bfief �g���[�i�[�J�[�h�p�Z�[�u�f�[�^�A�N�Z�X�֘A
 * @author  Akito Mori
 *
 *
 */
//=============================================================================
#ifndef __TRAINERCARD_DATA_H__
#define __TRAINERCARD_DATA_H__

// �g���[�i�[�J�[�h�Őݒ�ł���u���i�v�̍ő�l
#define TRCARD_PERSONARITY_MAX    ( 25 )

#define TRCARD_FINISH_PALACE      (  0 )
#define TRCARD_FINISH_POKESHIFTER (  1 )
#define TRCARD_FINISH_TRIALHOUSE  (  2 )
#define TRCARD_FINISH_DENDOU      (  3 )

#include "savedata/save_control.h"
#include "system/pms_data.h"

// �T�C�Y��RTCDate��菬�������t�\����
typedef struct{
  u8 year;
  u8 month;
  u8 day;
  u8 etc;
} SHORT_DATE;


typedef struct TR_CARD_SV_DATA_tag * TR_CARD_SV_PTR;

extern int TRCSave_GetSaveDataSize(void);
extern void TRCSave_InitSaveData(TR_CARD_SV_PTR outTrCard);

// �g���[�i�J�[�h�f�[�^�擾
extern TR_CARD_SV_PTR TRCSave_GetSaveDataPtr(SAVE_CONTROL_WORK * sv);

// �T�C���f�[�^����
extern u8 *TRCSave_GetSignDataPtr(TR_CARD_SV_PTR inTrCard);
extern int TRCSave_GetSigned( TR_CARD_SV_PTR inTrCard );
// ���j�I�������ڂ�ύX���Ă��邩�H
extern void TRCSave_SetTrainerViewChange( TR_CARD_SV_PTR inTrCard );
extern  int TRCSave_GetTrainerViewChange( TR_CARD_SV_PTR outTrCard );
// ���i�i�g���[�i�[�J�[�h�ŕ\�������j
extern void TRCSave_SetPersonarity( TR_CARD_SV_PTR inTrCard, int personarity );
extern  int TRCSave_GetPersonarity( TR_CARD_SV_PTR outTrCard );
// �T�C���A�j��
extern void TRCSave_SetSignAnime( TR_CARD_SV_PTR inTrCard, int flag );
extern  int TRCSave_GetSignAnime( TR_CARD_SV_PTR outTrCard );
// �Ō�ɃJ�[�h���J������
extern void TRCSave_SetLastTime( TR_CARD_SV_PTR inTrCard, s64 DateTime );
extern s64  TRCSave_GetLastTime( TR_CARD_SV_PTR outTrCard );
// �o�b�W�擾����
extern void TRCSave_SetBadgeDate( TR_CARD_SV_PTR inTrCard, int no, int year, int month, int day );
extern SHORT_DATE TRCSave_GetBadgeDate( TR_CARD_SV_PTR outTrCard, int no );
// �o�b�W�̖����
extern int  TRCSave_GetBadgePolish( TR_CARD_SV_PTR outTrCard, int no );
extern void TRCSave_SetBadgePolish( TR_CARD_SV_PTR inTrCard, int no, int polish );
// �e��C�x���g�̏I�����
extern void TRCSave_SetEventOpenFlag( TR_CARD_SV_PTR inTrCard, int event );
extern BOOL TRCSave_GetEventOpenFlag( TR_CARD_SV_PTR outTrCard, int event );


#endif //__TRAINERCARD_DATA_H__

