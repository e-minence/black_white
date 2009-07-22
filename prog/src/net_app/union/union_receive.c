//==============================================================================
/**
 * @file    ���j�I���F��M�f�[�^����
 * @brief   �ȒP�Ȑ���������
 * @author  matsuda
 * @date    2009.07.07(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "union_types.h"
#include "savedata/mystatus.h"
#include "net_app/union/union_main.h"
#include "union_local.h"
#include "union_receive.h"
#include "union_chara.h"
#include "union_oneself.h"


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void UnionReceive_BeaconCheck(UNION_SYSTEM_PTR unisys, UNION_BEACON_PC *bpc);


//==================================================================
/**
 * ��M�r�[�R���f�[�^����
 *
 * @param   unisys		
 */
//==================================================================
void UnionReceive_BeaconInterpret(UNION_SYSTEM_PTR unisys)
{
  int i;
  UNION_BEACON_PC *bpc;
  BOOL req_ret;
  
  bpc = unisys->receive_beacon;
  for(i = 0; i < UNION_RECEIVE_BEACON_MAX; i++){
    if(bpc->beacon.data_valid == UNION_BEACON_VALID){
      if(bpc->update_flag == UNION_BEACON_RECEIVE_NEW){
        //�V�K�L�����o�^
//        UNION_CHAR_AddOBJ(unisys, unisys->uniparent->game_data, bpc->beacon.trainer_view, i);
//        req_ret = UNION_CHAR_EventReq(bpc, BPC_EVENT_STATUS_ENTER);
//        GF_ASSERT_MSG(req_ret == TRUE, "add union req_faile!");
      }
      //�r�[�R���̓��e�`�F�b�N
      if(bpc->update_flag != UNION_BEACON_RECEIVE_NULL){
        UnionReceive_BeaconCheck(unisys, bpc);
        bpc->update_flag = UNION_BEACON_RECEIVE_NULL;
      }
    }
    bpc++;
  }
}

//--------------------------------------------------------------
/**
 * ��M�����r�[�R���̒���
 *
 * @param   unisys		
 * @param   bpc		
 */
//--------------------------------------------------------------
static void UnionReceive_BeaconCheck(UNION_SYSTEM_PTR unisys, UNION_BEACON_PC *bpc)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  
  if(situ->union_status == UNION_STATUS_NORMAL){
    //�ڑ��v���`�F�b�N
    if(bpc->beacon.union_status == UNION_STATUS_CONNECT_REQ)
    {
      u8 my_mac[6];
      OS_GetMacAddress(my_mac);
      if(GFL_STD_MemComp(bpc->beacon.connect_mac_address, my_mac, 6) == 0)
      {
        OS_TPrintf("�ڑ��v���FMac��v\n");
        UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_ANSWER_PC, bpc);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_CONNECT_ANSWER);
      }
      else
      {
      //  OS_TPrintf("�ڑ��v���FMac�s��v my = %d %d %d %d %d %d, req = %d %d %d %d %d %d\n", my_mac[0], my_mac[1], my_mac[2], my_mac[3], my_mac[4], my_mac[5], bpc->beacon.connect_mac_address[0], bpc->beacon.connect_mac_address[1], bpc->beacon.connect_mac_address[2], bpc->beacon.connect_mac_address[3], bpc->beacon.connect_mac_address[4], bpc->beacon.connect_mac_address[5]);
      }
    }
    else
    {
    //  OS_TPrintf("�r�[�R���`�F�b�N�Funion_status = %d\n", bpc->beacon.union_status);
    }
  }
}
