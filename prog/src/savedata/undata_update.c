//==============================================================================
/**
 * @file    undata_update.c
 * @brief   ���A�f�[�^�����o������
 * @author  saito
 * @date    2010.01.02.03
 */
//==============================================================================

#include "savedata/wifihistory_local.h"
#include "savedata/undata_update.h"

#define SAME_COUNTRY_DATA_MAX  (5)    //�������̃f�[�^��5���܂�

static u32 GetDataNumByCountryCode(const UNITEDNATIONS_SAVE *un_data, const u32 inCountryCode);
static u32 GetDataIdxByCountryCode(const UNITEDNATIONS_SAVE *un_data, const u32 inCountryCode);
static u32 SearchSameData(const UNITEDNATIONS_SAVE *un_data, const MYSTATUS * inMyStatus);
static void  DelData(UNITEDNATIONS_SAVE *un_data, const u32 inDelIdx);
static void  AddData(WIFI_HISTORY * wh, UNITEDNATIONS_SAVE *add_data, const BOOL inTalkFlg);
static void  AddDataCore(UNITEDNATIONS_SAVE *un_data, UNITEDNATIONS_SAVE *add_data, const BOOL inTalkFlg);

static DAT_ADD_ST UpdateData(WIFI_HISTORY * wh, UNITEDNATIONS_SAVE *add_data);

#ifdef PM_DEBUG

//�O���錾
DAT_ADD_ST UNDATAUP_UpdateD(WIFI_HISTORY * wh, UNITEDNATIONS_SAVE *add_data);

//----------------------------------------------------------
/**
 * @brief	�f�[�^�X�V�@�f�o�b�O�p
 * @param	  wh        wifi�����f�[�^�|�C���^
 * @param   add_data  �ǉ�����f�[�^
 *
 * @return	DAT_ADD_ST      �ǉ�����
 */
//----------------------------------------------------------
DAT_ADD_ST UNDATAUP_UpdateD(WIFI_HISTORY * wh, UNITEDNATIONS_SAVE *add_data)
{
  UNITEDNATIONS_SAVE *un_data;
  u32 same_idx;

  {
    //�����̍��R�[�h���擾
    int my_country_code = MyStatus_GetMyNation(&add_data->aMyStatus);
    if ( my_country_code == 0 )
    {
      //�o�^�w�薳���ꍇ�͏������s��Ȃ�
      NOZOMU_Printf("�����̍��R�[�h���ݒ肳��Ă��Ȃ��̂ŁA�������Ȃ�\n");
      return DAT_ADD_ST_FAIL;
    }
  }

  return UpdateData(wh, add_data);
}
#endif

//----------------------------------------------------------
/**
 * @brief	�f�[�^�X�V
 * @param	  wh        wifi�����f�[�^�|�C���^
 * @param   add_data  �ǉ�����f�[�^
 *
 * @return	DAT_ADD_ST      �ǉ�����
 */
//----------------------------------------------------------
DAT_ADD_ST UNDATAUP_Update(WIFI_HISTORY * wh, UNITEDNATIONS_SAVE *add_data)
{
  UNITEDNATIONS_SAVE *un_data;
  u32 same_idx;

  {
    //����̍��R�[�h���擾
    int country_code = MyStatus_GetMyNation(&add_data->aMyStatus);
    if ( country_code == 0 )
    {
      //�o�^�w�薳���ꍇ�͏������s��Ȃ�
      NOZOMU_Printf("���肪���R�[�h���ݒ肳��Ă��Ȃ��̂ŁA�������Ȃ�\n");
      return DAT_ADD_ST_FAIL;
    }
  }

  //�ǉ����悤�Ƃ��Ă���f�[�^�̓��{�o�[�W�����`�F�b�N
#if (PM_LANG == LANG_JAPAN)
  {
    int lang = MyStatus_GetRegionCode(&add_data->aMyStatus);
    // ����̃J�[�g���b�W�����{�Ȃ̂�
    if(lang == LANG_JAPAN){
      int nation = MyStatus_GetMyNation(&add_data->aMyStatus);
      // �Ⴄ���̃R�[�h�����Ă�����o�^���Ȃ�
      if(nation != WIFI_NATION_JAPAN){
        return DAT_ADD_ST_FAIL;
      }
    }
  }
#endif

  return UpdateData(wh, add_data);
}

//----------------------------------------------------------
/**
 * @brief	�f�[�^�X�V
 * @param	  wh        wifi�����f�[�^�|�C���^
 * @param   add_data  �ǉ�����f�[�^
 *
 * @return	DAT_ADD_ST      �ǉ�����
 */
//----------------------------------------------------------
static DAT_ADD_ST UpdateData(WIFI_HISTORY * wh, UNITEDNATIONS_SAVE *add_data)
{
  UNITEDNATIONS_SAVE *un_data;
  u32 same_idx;

  un_data = WIFIHISTORY_GetUNDataPtr(wh);

  //����l������
  same_idx = SearchSameData(un_data, &add_data->aMyStatus);
  if ( same_idx != UNITEDNATIONS_PEOPLE_MAX )
  {       //��������
    BOOL talk;
    //�T���o�����f�[�^�̉�b��Ԃ��`�F�b�N
    if ( un_data[same_idx].bTalk ) talk = TRUE;   //��b���Ă���
    else talk = FALSE;                            //��b���Ă��Ȃ�
    //�폜
    DelData(un_data, same_idx);
    //���X�g�̍Ō���֒ǉ�  �����̎����Ă����f�[�^�ɑ��݂��Ă����̂ŉ�b��Ԃ͈����p��
    AddData(wh, add_data, talk);
    return DAT_ADD_ST_CHG;
  }
  else
  {       //������Ȃ�����
    u32 same_cont_num;
    u32 country_code;
    //�ǉ��f�[�^�̍��R�[�h���擾
    country_code = MyStatus_GetMyNation(&add_data->aMyStatus);
    //�ǉ��f�[�^�Ɠ������R�[�h�����f�[�^�̐����擾
    same_cont_num = GetDataNumByCountryCode(un_data, country_code);
    if (same_cont_num < SAME_COUNTRY_DATA_MAX)
    {     //�萔����
      u32 total_num;
      //�L���f�[�^���擾
      total_num = UNDATAUP_GetDataNum(un_data);
      //�L���f�[�^������UNITEDNATIONS_PEOPLE_MAX���H
      if ( total_num >= UNITEDNATIONS_PEOPLE_MAX )
      {
        //���X�g�̐擪�̃f�[�^(��ԌÂ��f�[�^)���폜
        DelData(un_data, 0);
      }
      //���X�g�̍Ō���֒ǉ��@�����̎����Ă����f�[�^�ɂ͑��݂��Ă��Ȃ������̂ŉ�b�t���O��FALSE�m��
      AddData(wh, add_data, FALSE);
    }
    else //�萔
    {
      u32 del_idx;
      //�������f�[�^�̒��ň�ԌÂ��f�[�^������
      del_idx = GetDataIdxByCountryCode(un_data, country_code);
      DelData(un_data, del_idx);
      //���X�g�̍Ō���֒ǉ�  �����̎����Ă����f�[�^�ɂ͑��݂��Ă��Ȃ������̂ŉ�b�t���O��FALSE�m��
      AddData(wh, add_data, FALSE);
    }
    return DAT_ADD_ST_ADD;
  }
}

//----------------------------------------------------------
/**
 * @brief	�L���f�[�^���̎擾
 * @param	  un_data   ���A�f�[�^�̐擪�|�C���^
 *
 * @return	u32   �L���f�[�^��
 */
//----------------------------------------------------------
u32 UNDATAUP_GetDataNum(const UNITEDNATIONS_SAVE *un_data)
{
  u32 i;
  for (i=0;i<UNITEDNATIONS_PEOPLE_MAX;i++)
  {
    if ( !un_data[i].valid ) break;
  }
  return i;
}

//----------------------------------------------------------
/**
 * @brief	�w�荑�R�[�h�����f�[�^�̐���Ԃ�
 * @param	  un_data   ���A�f�[�^�̐擪�|�C���^
 * @param   inCountryCode   �������R�[�h
 *
 * @return	u32   ����(�ő��SAME_COUNTRY_DATA_MAX)
 */
//----------------------------------------------------------
static u32 GetDataNumByCountryCode(const UNITEDNATIONS_SAVE *un_data, const u32 inCountryCode)
{
  u32 i;
  u32 num;
  num = 0;
  for (i=0;i<UNITEDNATIONS_PEOPLE_MAX;i++)
  {
    u32 code;
    if ( !un_data[i].valid ) break;   //�L���f�[�^�̖����܂œ��B
    //�f�[�^�̍��R�[�h���擾
    code = MyStatus_GetMyNation(&un_data[i].aMyStatus);
    if ( code == inCountryCode ) num++;
  }

  if (num > SAME_COUNTRY_DATA_MAX)
  {
    GF_ASSERT_MSG(0,"num over %d",num);
    num = SAME_COUNTRY_DATA_MAX;
  }
  return num;
}

//----------------------------------------------------------
/**
 * @brief	�w�荑�R�[�h������ԏ��߂Ɍ�������(�ł��Â�)�f�[�^�̃C���f�b�N�X��Ԃ�
 * @param	  un_data   ���A�f�[�^�̐擪�|�C���^
 * @param   inCountryCode   �������R�[�h
 *
 * @return	u32   ��ԏ��߂Ɍ��������C���f�b�N�X�@�������̏ꍇ��UNITEDNATIONS_PEOPLE_MAX
 */
//----------------------------------------------------------
static u32 GetDataIdxByCountryCode(const UNITEDNATIONS_SAVE *un_data, const u32 inCountryCode)
{
  u32 i;
  for (i=0;i<UNITEDNATIONS_PEOPLE_MAX;i++)
  {
    u32 code;
    if ( !un_data[i].valid ) break;   //�L���f�[�^�̖����܂œ��B
    //�f�[�^�̍��R�[�h���擾
    code = MyStatus_GetMyNation(&un_data[i].aMyStatus);
    if ( code == inCountryCode ) return i;    //���v�f�[�^����
  }
  return UNITEDNATIONS_PEOPLE_MAX;
}

//----------------------------------------------------------
/**
 * @brief	����l���̌�������
 * @param	  un_data   ���A�f�[�^�̐擪�|�C���^
 * @param   inMyStatus    �Ώۂ̃}�C�X�e�[�^�X
 *
 * @return	u32   ��ԏ��߂Ɍ��������C���f�b�N�X�@�������̏ꍇ��UNITEDNATIONS_PEOPLE_MAX
 */
//----------------------------------------------------------
static u32 SearchSameData(const UNITEDNATIONS_SAVE *un_data, const MYSTATUS * inMyStatus)
{
  u32 i;
  u32 id;
  u32 country_code;
  id = MyStatus_GetID(inMyStatus);
  country_code = MyStatus_GetMyNation(inMyStatus);

  for (i=0;i<UNITEDNATIONS_PEOPLE_MAX;i++)
  {
    u32 tr_id;
    if ( !un_data[i].valid ) break;   //�L���f�[�^�̖����܂œ��B
    //ID�擾
    tr_id = MyStatus_GetID(&un_data[i].aMyStatus);
    if ( tr_id == id )
    {
      u32 code;
      //���R�[�h���擾
      code = MyStatus_GetMyNation(&un_data[i].aMyStatus);
      if ( code == country_code ) return i;    //���v�f�[�^����
    }
  }
  return UNITEDNATIONS_PEOPLE_MAX;
}

//----------------------------------------------------------
/**
 * @brief	�w��ʒu�̃f�[�^���폜���ăf�[�^���߂�
 * @param	  un_data   ���A�f�[�^�̐擪�|�C���^
 * @param   inDelIdx     �w��ʒu  0�`UNITEDNATIONS_PEOPLE_MAX-1
 *
 * @return	none
 */
//----------------------------------------------------------
static void  DelData(UNITEDNATIONS_SAVE *un_data, const u32 inDelIdx)
{
  u32 i;

  if ( inDelIdx >= UNITEDNATIONS_PEOPLE_MAX )
  {
    GF_ASSERT_MSG(0,"idx %d",inDelIdx);
    return;
  }
  //�L���f�[�^���߂�
  for(i=inDelIdx;i<(UNITEDNATIONS_PEOPLE_MAX-1);i++)
  {
    un_data[i] = un_data[i+1];
    if ( !un_data[i+1].valid ){
      i++;
      break;
    }
  }
  NOZOMU_Printf("idx=%d�ȍ~�͖����f�[�^\n",i);
  //�ȍ~�͖����f�[�^�̂͂��Ȃ̂Ŗ�����Ԃɂ���
  for (;i<UNITEDNATIONS_PEOPLE_MAX;i++)
  {
    un_data[i].valid = 0;
  }
}

//----------------------------------------------------------
/**
 * @brief	�w��ʒu�̃f�[�^���폜���ăf�[�^���߂�
 * @param	  wh          wifi�����f�[�^�|�C���^
 * @param   add_data    �ǉ����鍑�A�f�[�^
 * @param   inTalkFlg   �ǉ�����f�[�^�̐l����b�������i�����������Ă����f�[�^�̍X�V�̉\��������̂Łj
 *
 * @return	none
 */
//----------------------------------------------------------
static void  AddData(WIFI_HISTORY * wh, UNITEDNATIONS_SAVE *add_data, const BOOL inTalkFlg)
{
  UNITEDNATIONS_SAVE *un_data = WIFIHISTORY_GetUNDataPtr(wh);
  AddDataCore(un_data, add_data, inTalkFlg);
  //���r�b�g���Ă�
  {
    BOOL rc;
    u32 code;
    //���R�[�h���擾
    code = MyStatus_GetMyNation(&add_data->aMyStatus);
    rc = WIFIHISTORY_SetCountryBit(wh, code);
    if (rc) WIFIHISTORY_AddMyCountryCount(wh);
  }
}


//----------------------------------------------------------
/**
 * @brief	�w��ʒu�̃f�[�^���폜���ăf�[�^���߂�
 * @param	  un_data     ���A�f�[�^�̐擪�|�C���^
 * @param   add_data    �ǉ����鍑�A�f�[�^
 * @param   inTalkFlg   �ǉ�����f�[�^�̐l����b�������i�����������Ă����f�[�^�̍X�V�̉\��������̂Łj
 *
 * @return	none
 */
//----------------------------------------------------------
static void  AddDataCore(UNITEDNATIONS_SAVE *un_data, UNITEDNATIONS_SAVE *add_data, const BOOL inTalkFlg)
{
  u32 last_idx;

  //�L���f�[�^�������擾���Ė����̃C���f�b�N�X�����肷��
  last_idx = UNDATAUP_GetDataNum(un_data);

  if (last_idx >= UNITEDNATIONS_PEOPLE_MAX)
  {
    GF_ASSERT_MSG(0,"index over %d",last_idx);
    return;   //�ǉ������ɖ߂�
  }

  //�ǉ�����f�[�^���ŁA���������Ȃ���΂Ȃ�Ȃ������o�������ŏ���������
  if (inTalkFlg) add_data->bTalk = 1;     //�b�������H >>  �b����
  else add_data->bTalk = 0;               //�b�������H >>  �܂��b���Ă��Ȃ�
  add_data->valid = 1;                    //�L���f�[�^�H >> �L���f�[�^

  un_data[last_idx] = *add_data;
}
