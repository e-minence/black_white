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


#define UNDATA_MAX  (UNITEDNATIONS_PEOPLE_MAX)

static void SetData(WIFI_HISTORY *wh, UNSV_WORK* work, const int inCountryCode);

//--------------------------------------------------------------
/**
 * @brief ���A�֘A�f�[�^�̃Z�b�g
 * @param inCountryCode
 * @retval  none
 */
//--------------------------------------------------------------
void UN_SetData(GAMESYS_WORK *gsys, const int inCountryCode)
{
  GAMEDATA *gdata =  GAMESYSTEM_GetGameData(gsys);
  UNSV_WORK *work = GAMEDATA_GetUnsvWorkPtr(gdata);
  
  //���R�[�h����`�F�b�N
  if (inCountryCode < WIFI_COUNTRY_MAX)
  {
    //�Z�[�u�f�[�^�ɃA�N�Z�X���āA�w�荑�R�[�h�̃f�[�^�����W
    SAVE_CONTROL_WORK * sv = GAMEDATA_GetSaveControlWork( gdata );
    WIFI_HISTORY *wh = SaveData_GetWifiHistory(sv);
    SetData(wh, work, inCountryCode);
  }
  else
  {
    GF_ASSERT_MSG(0,"CountryCode = %d",inCountryCode);
    UNSV_Init(work);
  }
}

//--------------------------------------------------------------
/**
 * @brief ���A�֘A�f�[�^�̃Z�b�g
 *
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
          work->UnObjCode[work->ObjNum] = tr_view;  //�\��OBJ�R�[�h�i�[ @todo �����Ńe�[�u���ϊ� 
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
    else GF_ASSERT_MSG(0,"OBJ_IDX_ERROR %d",inObjIdx);
  }
  else GF_ASSERT_MSG(0,"OBJ_IDX_ERROR %d",inObjIdx);

  return rc;
}

