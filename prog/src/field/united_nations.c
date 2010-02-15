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
#include "fieldmap/fldmmdl_objcode.h"

#include "system/main.h"   //for HEAPID

#define UNDATA_MAX  (UNITEDNATIONS_PEOPLE_MAX)

#define UN_SETNUM (12)

#define UN_NATURE_MSG_MARGINE (6)   //�e���i���Ƃ�6���b�Z�[�W
#define UN_NATURE_MSG_MARGINE_TOTAL (UN_NATURE_MSG_MARGINE*2)   //�e���i���Ƃ�6���b�Z�[�W������

static void SetData(WIFI_HISTORY *wh, UNSV_WORK* work, const int inCountryCode, const u32 inFloor);
static u32 GetMsg(const int inSex, const u32 inTrID,
                  const u32 inNature, const BOOL inFirst, const BOOL inPlace);
static void SetWords(WORDSET *wordset, GAMEDATA *gdata, const u32 inDataIdx);

#ifdef PM_DEBUG
int DebugUnCountryCode = 0;
#endif

//--------------------------------------------------------------
/**
 * @brief ���A�֘A�f�[�^�̃Z�b�g
 * @param inCountryCode   ���R�[�h
 * @param inFloor         �t���A
 * @retval  none
 */
//--------------------------------------------------------------
void UN_SetData(GAMESYS_WORK *gsys, const int inCountryCode, const u32 inFloor)
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
    SetData(wh, work, code, inFloor);
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
 * @param inFloor           �t���A
 * @retval  none
 */
//--------------------------------------------------------------
static void SetData(WIFI_HISTORY *wh, UNSV_WORK* work, const int inCountryCode, const u32 inFloor)
{
  int i;
  
  work->TargetCountryCode = inCountryCode;
  work->Floor = inFloor;
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
          if (tr_view > UNION_VIEW_INDEX_MAX)
          {
            GF_ASSERT_MSG(0,"tr_view = %d",tr_view);
            if ( MyStatus_GetMySex(my) ) tr_view = GIRL2;
            else tr_view = BOY2;
          }
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
u32 UN_GetRoomObjMsg(WORDSET *wordset, GAMESYS_WORK *gsys, UNSV_WORK* work, const int inObjIdx, const BOOL inFirstMsg)
{
  int undata_idx;
  int sex;
  u32 id;
  int nature;
  BOOL place_flg = FALSE;

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
  //���[�h�Z�b�g�֘A����
  SetWords(wordset, gdata, undata_idx);
  {
    MYSTATUS *my;
    my = WIFIHISTORY_GetUnMyStatus(wh, undata_idx);
    //���ʎ擾
    sex = MyStatus_GetMySex(my);
    //���i�擾
    nature = WIFIHISTORY_GetUnInfo(wh, undata_idx, UN_INFO_NATURE);
    //�g���[�i�[ID�擾
    id = MyStatus_GetID(my);

    //�n�悠�邩�H
    {
      u32 nation;
      u32 place;
      UNSV_WORK *unsv;
      unsv = GAMEDATA_GetUnsvWorkPtr(gdata);
      nation = unsv->TargetCountryCode;
      place = WIFI_COUNTRY_CountryCodeToPlaceIndexMax( nation );
      if(place > 0) place_flg = TRUE;
    }
  }
  //���b�Z�[�W����
  return GetMsg(sex, id, nature, inFirstMsg, place_flg);

}

//--------------------------------------------------------------
/**
 * @brief OBJ�̘b�����b�Z�[�W�̌���
 * @param inSex         ���� 0 or 1
 * @param inTrID        �g���[�i�[ID
 * @param inNature      ���i0�`4
 * @param inFirst       �擾�������̂͏��񃁃b�Z�[�W���H
 * @param inPlace       �n�悠�邩
 * @retval  u32     ���b�Z�[�WID
 */
//--------------------------------------------------------------
static u32 GetMsg(const int inSex, const u32 inTrID,
                  const u32 inNature, const BOOL inFirst, const BOOL inPlace)
{
  u32 base;
  u32 msg;
  //���i�Ń��b�Z�[�W�C���f�b�N�X�̊�_�����߂�

  if (inPlace) base = msg_un_default_man_00;    //�n�悠��
  else base = msg_un_default_man_06;       //�n�斳��
  
  if ( inNature < 5 ) msg = base + (inNature*UN_NATURE_MSG_MARGINE_TOTAL);
  else
  {
    GF_ASSERT_MSG(0,"nature error %d",inNature);
    msg = base;
  }
  
  //���ʂŃI�t�Z�b�g���v�Z
  if (inSex) msg += UN_NATURE_MSG_MARGINE;//����
  //�����b���H
  if ( !inFirst )      //2��ڈȍ~
  {
    msg += 1;
    //�g���[�iID��5�Ŋ������]�����_�ɑ�������
    msg += (inTrID % 5);
  }

  return msg;
}

//--------------------------------------------------------------
/**
 * @brief ���b�Z�[�W�^�O�W�J
 * @param wordset         ���[�h�Z�b�g�|�C���^
 * @param gdata           �Q�[���f�[�^�|�C���^
 * @param inDataIdx       ���A�f�[�^�C���f�b�N�X0�`19
 * @retval  none
 */
//--------------------------------------------------------------
static void SetWords(WORDSET *wordset, GAMEDATA *gdata, const u32 inDataIdx)
{
  HEAPID  heapID;
  u32 monsno;
  WIFI_HISTORY *wh;
  UNSV_WORK *unsv;
  u32 nation;
  MYSTATUS *npc_my;

  {
    SAVE_CONTROL_WORK * sv = GAMEDATA_GetSaveControlWork( gdata );
    wh = SaveData_GetWifiHistory(sv);
    unsv = GAMEDATA_GetUnsvWorkPtr(gdata);
    nation = unsv->TargetCountryCode;
  }

  npc_my = WIFIHISTORY_GetUnMyStatus(wh, inDataIdx);

  heapID = HEAPID_FIELDMAP;

  //0�Ԃɍ����Z�b�g
  WORDSET_RegisterCountryName( wordset, 0, nation );
  //1�Ԃɒn�於�Z�b�g
  {
    //�w�肵�����̒n�搔���擾
    u32 count = WIFI_COUNTRY_CountryCodeToPlaceIndexMax( nation );
    if (count != 0)
    {
      u32 place;
      place = MyStatus_GetMyArea(npc_my);
      WORDSET_RegisterLocalPlaceName( wordset, 1, nation, place );
    }
  }
  //3�ԂɃv���[���[�����Z�b�g
  {
    MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gdata );
    WORDSET_RegisterPlayerName( wordset, 3, mystatus );
  }
  //5�Ԃ�NPC���v���[���[�����������|�P�������Z�b�g
  {
    monsno = WIFIHISTORY_GetUnInfo(wh, inDataIdx, UN_INFO_RECV_POKE);
    WORDSET_RegisterPokeMonsNameNo( wordset, 5, monsno );
  }
  //6�ԂɃv���[���[�̎���Z�b�g
  {
    u8 favorite;
    favorite = WIFIHISTORY_GetMyFavorite( wh );
    WORDSET_RegisterHobbyName( wordset, 6, favorite );
  }
  //7�Ԃ�NPC�g���[�i�[�����Z�b�g
  {
    WORDSET_RegisterPlayerName( wordset, 3, npc_my );
  }
  //8�Ԃ�NPC�����������Z�b�g
  {
    int keta = 3;
    int number;
    number = WIFIHISTORY_GetUnInfo(wh, inDataIdx, UN_INFO_COUNTRY_NUM);
    WORDSET_RegisterNumber( wordset, 8, number, keta, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  }
  //9�Ԃ�NPC���v���[���[�ɂ������|�P�������Z�b�g
  {
    monsno = WIFIHISTORY_GetUnInfo(wh, inDataIdx, UN_INFO_SEND_POKE);
    WORDSET_RegisterPokeMonsNameNo( wordset, 9, monsno );
  }
  //10�Ԃ�NPC�̎���Z�b�g
  {
    u8 favorite;
    favorite = WIFIHISTORY_GetUnInfo(wh, inDataIdx, UN_INFO_FAVORITE);
    WORDSET_RegisterHobbyName( wordset, 10, favorite );
  }
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
