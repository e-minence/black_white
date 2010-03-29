//=============================================================================
/**
 * @file  trainercard_data.c
 * @bfief �g���[�i�[�J�[�h�p�Z�[�u�f�[�^�A�N�Z�X�֘A
 * @author  Nozomu Saito
 *
 *
 */
//=============================================================================
#include <gflib.h>
#include "savedata/save_tbl.h"
#include "savedata/trainercard_data.h"

#define BADGE_MAX (8)
#define SIGHN_W (24)
#define SIGHN_H (8)

#define DEFAULT_BADGE_SCRUCH  (140)
#define TRAINERCARD_BADGE_NUM (  8)


typedef struct TR_CARD_SV_DATA_tag
{
  u8         SignData[SIGHN_W*SIGHN_H*64/8];   ///< �T�C���ʃf�[�^
  u16        personarity;                      ///< ���i�i�g���[�i�[�J�[�h�ŎQ�Ɓj
  u8         trainer_view_change;              ///< �g���[�i�[�J�[�h�Ō����ڂ�ύX����(0:���Ă��Ȃ� 1:�����j
  u8         card_anime;                       ///< �T�C���ʂ��A�j�������Ă���
  SHORT_DATE badgeGetDate[TRAINERCARD_BADGE_NUM]; ///< �o�b�W�擾��
  s64        lastTime;                         ///< �Ō�Ƀg���[�i�[�J�[�h���J�������t�E�����̐��l
  u8         polish[TRAINERCARD_BADGE_NUM];    ///< �o�b�W�̖����p�����[�^
  u32        openflag;                         ///< �e�C�x���g�����Ȃ��Ă��邩�t���O
  u32        dummy[TRAINERCARD_BADGE_NUM];     ///< �����W�������������Ƃ����肻���Ȃ̂ŁB
}TR_CARD_SV_DATA;

//==============================================================================
/**
 * �Z�[�u�f�[�^�T�C�Y�擾
 *
 * @param none
 *
 * @return  int   �T�C�Y
 */
//==============================================================================
int TRCSave_GetSaveDataSize(void)
{
  return sizeof(TR_CARD_SV_DATA);
}

//==============================================================================
/**
 * �Z�[�u�f�[�^������
 *
 * @param outTrCard �g���[�i�[�J�[�h�f�[�^�|�C���^
 *
 * @return  none
 */
//==============================================================================
void TRCSave_InitSaveData(TR_CARD_SV_PTR outTrCard)
{
  GFL_STD_MemFill( outTrCard, 0, sizeof(TR_CARD_SV_DATA) );

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TRCARD)
  SVLD_SetCrc(GMDATA_ID_TRCARD);
#endif //CRC_LOADCHECK
}

//==============================================================================
/**
 * �Z�[�u�f�[�^�擪�|�C���^�擾
 *
 * @param sv    �Z�[�u�|�C���^
 *
 * @return  TR_CARD_SV_PTR    �g���[�i�[�J�[�h�f�[�^�|�C���^
 */
//==============================================================================
TR_CARD_SV_PTR TRCSave_GetSaveDataPtr(SAVE_CONTROL_WORK * sv)
{
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TRCARD)
  SVLD_CheckCrc(GMDATA_ID_TRCARD);
#endif //CRC_LOADCHECK

  return SaveControl_DataPtrGet(sv, GMDATA_ID_TRCARD);
}



//==============================================================================
/**
 * �T�C���f�[�^�擪�|�C���^�擾
 *
 * @param inTrCard    �g���[�i�[�J�[�h�f�[�^�|�C���^
 *
 * @return  u8*       �T�C���f�[�^�|�C���^
 */
//==============================================================================
u8 *TRCSave_GetSignDataPtr(TR_CARD_SV_PTR inTrCard)
{
  return inTrCard->SignData;
}

//==============================================================================
/**
 * @brief   �T�C�����������܂�Ă��邩�m�F����
 *
 * @param   inTrCard    
 *
 * @retval  int   TRUE�Ȃ珑�����܂�Ă���BFALSE�Ȃ�S�ċ�
 */
//==============================================================================
int TRCSave_GetSigned( TR_CARD_SV_PTR inTrCard )
{
  int search=0;
  int i;
  u32 *ptr =  (u32*)inTrCard->SignData; //�T�C���ʃf�[�^

  
  for(i=0;i<SIGHN_W*SIGHN_H*64/(8*4);i++){
    search += ptr[i];
  }

  if(search){
    return TRUE;
  }
  
  return FALSE;
}

//=============================================================================================
/**
 * @brief �g���[�i�[�J�[�h�Ń��j�I�������ڂ�ύX�������H
 *
 * @param   misc    
 *
 * @retval  int   �g���[�i�[�J�[�h�Ń��j�I�������ڂ�ύX�������H�i0:���ĂȂ� 1:�����j
 */
//=============================================================================================
int TRCSave_GetTrainerViewChange( TR_CARD_SV_PTR outTrCard )
{
  return outTrCard->trainer_view_change;
}

//=============================================================================================
/**
 * @brief �g���[�i�[�J�[�h�Ń��j�I�������ڂ�ύX�����t���O�𗧂Ă�
 */
//=============================================================================================
void TRCSave_SetTrainerViewChange( TR_CARD_SV_PTR inTrCard )
{
  inTrCard->trainer_view_change = 1;
}


//=============================================================================================
/**
 * @brief ���i�̎擾
 *
 * @param   misc    
 *
 * @retval  int   ���i�l
 */
//=============================================================================================
int TRCSave_GetPersonarity( TR_CARD_SV_PTR outTrCard )
{
  return outTrCard->personarity;

}

//=============================================================================================
/**
 * @brief �g���[�i�[�J�[�h�ŕύX�������i���i�[
 */
//=============================================================================================
void TRCSave_SetPersonarity( TR_CARD_SV_PTR inTrCard, int personarity )
{
  GF_ASSERT_MSG( (personarity < TRCARD_PERSONARITY_MAX),"���i�̍ő�l�𒴂��Ă���\n");

  inTrCard->personarity = personarity;
}

//=============================================================================================
/**
 * @brief �T�C���A�j����Ԃ��Z�b�g
 *
 * @param   inTrCard    
 * @param   flag      TRUE:�A�j�� FALSE:�A�j�����Ȃ�
 */
//=============================================================================================
void TRCSave_SetSignAnime( TR_CARD_SV_PTR inTrCard, BOOL flag )
{
  inTrCard->card_anime = flag;
}

//=============================================================================================
/**
 * @brief   �T�C���A�j����Ԃ��擾
 *
 * @param   outTrCard   
 *
 * @retval  int   TRUE:�A�j�� FALSE:�A�j�����Ȃ�
 */
//=============================================================================================
int TRCSave_GetSignAnime( TR_CARD_SV_PTR outTrCard )
{
  return outTrCard->card_anime;
}


//=============================================================================================
/**
 * @brief �Ō�ɃJ�[�h���J��������ݒ�
 *
 * @param   inTrCard    
 * @param   DateTime    
 */
//=============================================================================================
void TRCSave_SetLastTime( TR_CARD_SV_PTR inTrCard, s64 DateTime )
{
  inTrCard->lastTime = DateTime;
}

//=============================================================================================
/**
 * @brief �Ō�ɃJ�[�h���J���������擾
 *
 * @param   outTrCard   
 *
 * @retval  s64   
 */
//=============================================================================================
s64 TRCSave_GetLastTime( TR_CARD_SV_PTR outTrCard )
{
  return outTrCard->lastTime;
}

//=============================================================================================
/**
 * @brief �o�b�W�擾�������擾
 *
 * @param   outTrCard   
 * @param   no    
 *
 * @retval  SHORT_DATE    
 */
//=============================================================================================
SHORT_DATE TRCSave_GetBadgeDate( TR_CARD_SV_PTR outTrCard, int no )
{
  GF_ASSERT(no<TRAINERCARD_BADGE_NUM);

  return outTrCard->badgeGetDate[no];
}

//=============================================================================================
/**
 * @brief �o�b�W�擾������ݒ�
 *
 * @param   inTrCard    
 * @param   no    
 * @param   date    
 */
//=============================================================================================
void TRCSave_SetBadgeDate( TR_CARD_SV_PTR inTrCard, int no, int year, int month, int day )
{
  GF_ASSERT(no<TRAINERCARD_BADGE_NUM);

  inTrCard->badgeGetDate[no].year = year;
  inTrCard->badgeGetDate[no].year = month;
  inTrCard->badgeGetDate[no].year = day;
}


//=============================================================================================
/**
 * @brief �o�b�W�̖�������擾
 *
 * @param   outTrCard   
 * @param   no    
 *
 * @retval  SHORT_DATE    
 */
//=============================================================================================
int TRCSave_GetBadgePolish( TR_CARD_SV_PTR outTrCard, int no )
{
  GF_ASSERT(no<TRAINERCARD_BADGE_NUM);

  return outTrCard->polish[no];
}

//=============================================================================================
/**
 * @brief �o�b�W�̖������ݒ�
 *
 * @param   inTrCard    
 * @param   no    
 * @param   polish    
 */
//=============================================================================================
void TRCSave_SetBadgePolish( TR_CARD_SV_PTR inTrCard, int no, int polish )
{
  inTrCard->polish[no] = polish;
}

//=============================================================================================
/**
 * @brief �e��C�x���g���I���ς��擾
 *
 * @param   outTrCard   
 * @param   event   
 *
 * @retval  BOOL    
 */
//=============================================================================================
BOOL TRCSave_GetEventOpenFlag( TR_CARD_SV_PTR outTrCard, int event )
{
  return 1 & (outTrCard->openflag>>event);
}

//=============================================================================================
/**
 * @brief �e��C�x���g�̏I��������ݒ�
 *
 * @param   outTrCard   
 * @param   event   
 */
//=============================================================================================
void TRCSave_SetEventOpenFlag( TR_CARD_SV_PTR inTrCard, int event )
{
  inTrCard->openflag |= (1<<event);
}
