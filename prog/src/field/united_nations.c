//======================================================================
/*
 * @file	united_nations.c
 * @brief	���A�֘A
 * @author saito
 */
//======================================================================
#include "united_nations.h"
#include "gamesystem/gamesystem.h"

#include "savedata/wifihistory.h"
#include "savedata/wifihistory_local.h"
#include "savedata/un_savedata_local.h"
#include "savedata/un_savedata.h"

#include "united_nations.h"
#include "net_app/union/union_beacon_tool.h"

#include "msg/script/msg_c11r0201.h"


#define UNDATA_MAX  (UNITEDNATIONS_PEOPLE_MAX)

static void SetData(WIFI_HISTORY *wh, UNSV_WORK* work, const int inCountryCode);
static u32 GetMsg(const int inSex, const u32 inTrID, const int inNature, const BOOL inFirst);

#ifdef PM_DEBUG
int DebugUnCountryCode = 0;
#endif

//--------------------------------------------------------------
/**
 * @brief ���A�֘A�f�[�^�̃Z�b�g
 * @param inCountryCode
 * @retval  none
 */
//--------------------------------------------------------------
void UN_SetData(GAMESYS_WORK *gsys, const int inCountryCode)
{
  int code;
  GAMEDATA *gdata =  GAMESYSTEM_GetGameData(gsys);
  UNSV_WORK *work = GAMEDATA_GetUnsvWorkPtr(gdata);
  code = inCountryCode;
#ifdef PM_DEBUG
  if (DebugUnCountryCode != 0) code = DebugUnCountryCode;
#endif //PM_DEBUG
  
  //���R�[�h����`�F�b�N
  if (code < WIFI_COUNTRY_MAX)
  {
    //�Z�[�u�f�[�^�ɃA�N�Z�X���āA�w�荑�R�[�h�̃f�[�^�����W
    SAVE_CONTROL_WORK * sv = GAMEDATA_GetSaveControlWork( gdata );
    WIFI_HISTORY *wh = SaveData_GetWifiHistory(sv);
    SetData(wh, work, code);
  }
  else
  {
    GF_ASSERT_MSG(0,"CountryCode = %d",code);
    UNSV_Init(work);
  }
}

//--------------------------------------------------------------
/**
 * @brief ���A�֘A�f�[�^�̃Z�b�g
 *
 * @param * wh              WIFI�������[�N�|�C���^
 * @param * work            ���A���[�N�|�C���^
 * @param inCountryCode     ���R�[�h
 * @retval  none
 */
//--------------------------------------------------------------
static void SetData(WIFI_HISTORY *wh, UNSV_WORK* work, const int inCountryCode)
{
  int i;
  
  work->TargetCountryCode = inCountryCode;
  work->ObjNum = 0;
  for (i=0;i<UNDATA_MAX;i++)
  {
    //�擾
    u8 valid;
    valid = WIFIHISTORY_GetUnInfo(wh, i, UN_INFO_VALID);
    if ( valid )
    {
      MYSTATUS *my;
      my = WIFIHISTORY_GetUnMyStatus(wh, i);
      if (my != NULL)
      {
        int country_code;
        country_code = MyStatus_GetMyNation(my);
        //���R�[�h��r
        if (country_code == inCountryCode)    //���R�[�h��v�H
        {
          u8 tr_view;
          tr_view = MyStatus_GetTrainerView( my );
          work->UnIdx[work->ObjNum] = i;            //�C���f�b�N�X�i�[
          work->UnObjCode[work->ObjNum] = UnionView_GetObjCode(tr_view);  //�\��OBJ�R�[�h�i�[
          work->ObjNum++;                           //�\��OBJ�����Z
          if (work->ObjNum >= UN_ROOM_OBJ_MAX) break;
        }
      }
      else GF_ASSERT(0);
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief �w�肵���C���f�b�N�X�̍��A�֘A�f�[�^�C���f�b�N�X���擾����
 * @param * work            ���A���[�N�|�C���^
 * @param inOnjIdx ������OBJ�̃C���f�b�N�X0�`4
 * @retval  u32     ���A�Z�[�u�f�[�^�̑��l�f�[�^�C���f�b�N�X  ���s�����ꍇ��UN_IDX_OVER
 */
//--------------------------------------------------------------
u8 UN_GetUnIdx(UNSV_WORK* work, const u32 inObjIdx)
{
  u8 rc = UN_IDX_OVER;
  if (inObjIdx < UN_ROOM_OBJ_MAX)
  {
    if ( inObjIdx < work->ObjNum ) rc = work->UnIdx[inObjIdx];
    else GF_ASSERT_MSG(0,"OBJ_IDX_ERROR obj_idx=%d max=%d",inObjIdx, work->ObjNum);
  }
  else GF_ASSERT_MSG(0,"OBJ_IDX_ERROR %d",inObjIdx);

  return rc;
}

//--------------------------------------------------------------
/**
 * @brief OBJ�̘b�����b�Z�[�W�̌���
 * @param gsys          �Q�[���V�X�e���|�C���^
 * @param work            ���A���[�N�|�C���^
 * @param   inObjIdx        OBJ�C���f�b�N�X0�`4
 * @param  inFirstMsg       �擾�������̂͏��񃁃b�Z�[�W���H
 * @retval  u32     ���b�Z�[�WID
 */
//--------------------------------------------------------------
u32 UN_GetRoomObjMsg(GAMESYS_WORK *gsys, UNSV_WORK* work, const int inObjIdx, const BOOL inFirstMsg)
{
  int undata_idx;
  int sex;
  u32 id;
  int nature;

  //�Z�[�u�f�[�^�ɃA�N�Z�X
  GAMEDATA *gdata =  GAMESYSTEM_GetGameData(gsys);
  SAVE_CONTROL_WORK * sv = GAMEDATA_GetSaveControlWork( gdata );
  WIFI_HISTORY *wh = SaveData_GetWifiHistory(sv);

  if ( inObjIdx >= 5 )
  {
    GF_ASSERT(0);
    //�K���ȃ��b�Z�[�WID��Ԃ��Ă���
    return msg_un_default_man_00;
  }

  undata_idx = work->UnIdx[inObjIdx];
  {
    MYSTATUS *my;
    my = WIFIHISTORY_GetUnMyStatus(wh, undata_idx);
    //���ʎ擾
    sex = MyStatus_GetMySex(my);
    //���i�擾
    nature = WIFIHISTORY_GetUnInfo(wh, undata_idx, UN_INFO_NATURE);
    //�g���[�i�[ID�擾
    id = MyStatus_GetID(my);
  }
  //���b�Z�[�W����
  return GetMsg(sex, id, nature, inFirstMsg);

}

//--------------------------------------------------------------
/**
 * @brief OBJ�̘b�����b�Z�[�W�̌���
 * @param inSex         ���� 0 or 1
 * @param inTrID        �g���[�i�[ID
 * @param inNature      ���i0�`4
 * @param inFirst       �擾�������̂͏��񃁃b�Z�[�W���H
 * @retval  u32     ���b�Z�[�WID
 */
//--------------------------------------------------------------
static u32 GetMsg(const int inSex, const u32 inTrID, const int inNature, const BOOL inFirst)
{
  u32 base;
  u32 msg;
  //���i�Ń��b�Z�[�W�C���f�b�N�X�̊�_�����߂�
  switch(inNature){
  case 0:
    base = msg_un_default_man_00;
    break;
  case 1:
    base = msg_un_leader_man_00;
    break;
  case 2:
    base = msg_un_wierd_man_00;
    break;
  case 3:
    base = msg_un_researcher_man_00;
    break;
  case 4:
    base = msg_un_soft_man_00;
    break;
  default:
    GF_ASSERT_MSG(0,"nature error %d",inNature);
    base = msg_un_default_man_00;
  }
  msg = base;
  //���ʂŃI�t�Z�b�g���v�Z
  if (inSex) msg = base + 6;//����
  //�����b���H
  if ( !inFirst )      //2��ڈȍ~
  {
    msg += 1;
    //�g���[�iID��5�Ŋ������]�����_�ɑ�������
    msg += (inTrID % 5);
  }

  return msg;
}
#ifdef PM_DEBUG
u32 DebugGetUnCouontry(GAMESYS_WORK * gsys, GAMEDATA * gamedata, u32 param)
{
  return DebugUnCountryCode;
}

void DebugSetUnCouontry(GAMESYS_WORK * gsys, GAMEDATA * gamedata, u32 param, u32 value)
{
  DebugUnCountryCode = value;
}
#endif
