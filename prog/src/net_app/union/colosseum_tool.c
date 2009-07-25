//==============================================================================
/**
 * @file    colosseum_tool.c
 * @brief   �R���V�A���c�[���ށF�I�[�o�[���C�ɔz�u
 * @author  matsuda
 * @date    2009.07.18(�y)
 */
//==============================================================================
#include <gflib.h>
#include "net_app/union/colosseum.h"
#include "net_app/union/colosseum_types.h"
#include "field\fieldmap_proc.h"
#include "app\trainer_card.h"
#include "colosseum_types.h"
#include "system/main.h"
#include "comm_player.h"
#include "field\field_player.h"
#include "field\fieldmap.h"
#include "colosseum_comm_command.h"
#include "colosseum_tool.h"
#include "fieldmap/zone_id.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///s16�̃|�C���g�^
typedef struct{
  s16 x;
  s16 z;
}POINT_S16;

//==============================================================================
//  �f�[�^
//==============================================================================
//--------------------------------------------------------------
//  �����ʒu���W
//--------------------------------------------------------------
///1vs1�̎��̗����ʒu���W
static const POINT_S16 ColosseumStandingPos_1vs1[] = {
  {72, 88},   //��
  {136, 88},  //�E
};

///�}���`�̎��̗����ʒu���W
static const POINT_S16 ColosseumStandingPos_multi[] = {
  {72, 72},       //����
  {136, 72},      //�E��
  {72, 104},      //����
  {136, 104},     //�E��
};

//--------------------------------------------------------------
//  �o�����W
//--------------------------------------------------------------
///1vs1�̎��̗����ʒu���W
static const POINT_S16 ColosseumWayOutPos_1vs1[] = {
  {88, 168},
  {104, 168},
  {120, 168},
};

///�}���`�̎��̗����ʒu���W
static const POINT_S16 ColosseumWayOutPos_multi[] = {
  {88, 168},
  {104, 168},
  {120, 168},
};



//==================================================================
/**
 * �S�����̊�{��񂪎󂯎��Ă��邩���ׂ�
 *
 * @param   clsys		
 *
 * @retval  BOOL		TRUE:�S������M���Ă���
 */
//==================================================================
BOOL ColosseumTool_AllReceiveCheck_BasicStatus(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i, count;
  
  count = 0;
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(clsys->basic_status[i].occ == TRUE){
      count++;
    }
  }
  if(count >= GFL_NET_GetConnectNum()){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �S�����̃g���[�i�[�J�[�h���󂯎��Ă��邩���ׂ�
 *
 * @param   clsys		
 *
 * @retval  BOOL		TRUE:�S������M���Ă���
 */
//==================================================================
BOOL ColosseumTool_AllReceiveCheck_TrainerCard(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i, count;
  
  count = 0;
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(clsys->recvbuf.tr_card_occ[i] == TRUE){
      count++;
    }
  }
  if(count >= GFL_NET_GetConnectNum()){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �S������POKEPARTY���󂯎��Ă��邩���ׂ�
 *
 * @param   clsys		
 *
 * @retval  BOOL		TRUE:�S������M���Ă���
 */
//==================================================================
BOOL ColosseumTool_AllReceiveCheck_Pokeparty(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i, count;
  
  count = 0;
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(clsys->recvbuf.pokeparty_occ[i] == TRUE){
      count++;
    }
  }
  if(count >= GFL_NET_GetConnectNum()){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * ��M�o�b�t�@�̃N���A�FPOKEPARTY
 *
 * @param   clsys		
 * @param   except_for_mine   TRUE:�����̃f�[�^�̓N���A���Ȃ�(��M�t���O�͗��Ƃ�)
 */
//==================================================================
void ColosseumTool_Clear_ReceivePokeParty(COLOSSEUM_SYSTEM_PTR clsys, BOOL except_for_mine)
{
  int i, my_net_id;
  
  my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(except_for_mine == FALSE || my_net_id != i){
      PokeParty_InitWork(clsys->recvbuf.pokeparty[i]);
    }
    clsys->recvbuf.pokeparty_occ[i] = FALSE;
  }
}

//==================================================================
/**
 * �S�����̗����ʒu���󂯎��Ă��邩���ׂ�
 *
 * @param   clsys		
 *
 * @retval  BOOL		TRUE:OK
 */
//==================================================================
BOOL ColosseumTool_ReceiveCheck_StandingPos(COLOSSEUM_SYSTEM_PTR clsys)
{
  if(clsys->recvbuf.stand_position_occ == TRUE){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * ��M�o�b�t�@�̃N���A�F�S�����̗����ʒu��M
 *
 * @param   clsys		
 */
//==================================================================
void ColosseumTool_Clear_ReceiveStandingPos(COLOSSEUM_SYSTEM_PTR clsys)
{
  clsys->recvbuf.stand_position_occ = FALSE;
}

//==================================================================
/**
 * �����ʒu���W�ɂ��邩�`�F�b�N
 *
 * @param   fieldWork		
 * @param   entry_num		�Q���l��
 * @param   stand_pos		���ʑ����(�����ʒu�ɂ���ꍇ�A���Ԗڂ̗����ʒu�����Ԃ�)
 *
 * @retval  TRUE:�����ʒu�ɂ���B�@FALSE:�����ʒu�ɂ��Ȃ�
 */
//==================================================================
BOOL ColosseumTool_CheckStandingPosition(FIELD_MAIN_WORK *fieldWork, int entry_num, int *stand_pos)
{
  VecFx32 pos;
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
  int i;
  
  *stand_pos = 0;
  
  FIELD_PLAYER_GetPos(player, &pos);
  pos.x >>= FX32_SHIFT;
  pos.z >>= FX32_SHIFT;
  if(entry_num == 2){
    for(i = 0; i < NELEMS(ColosseumStandingPos_1vs1); i++){
      if(pos.x == ColosseumStandingPos_1vs1[i].x && pos.z == ColosseumStandingPos_1vs1[i].z){
        *stand_pos = i;
        return TRUE;
      }
    }
  }
  else{
    for(i = 0; i < NELEMS(ColosseumStandingPos_multi); i++){
      if(pos.x == ColosseumStandingPos_multi[i].x && pos.z == ColosseumStandingPos_multi[i].z){
        *stand_pos = i;
        return TRUE;
      }
    }
  }
  
  return FALSE;
}

//==================================================================
/**
 * �S�����́u�ޏo���܂��v���󂯎��Ă��邩���ׂ�
 *
 * @param   clsys		
 *
 * @retval  BOOL		TRUE:�S������M���Ă���
 */
//==================================================================
BOOL ColosseumTool_AllReceiveCheck_Leave(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i, count;
  
  count = 0;
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(clsys->recvbuf.leave[i] == TRUE){
      count++;
    }
  }
  if(count >= GFL_NET_GetConnectNum()){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �N������u�ޏo���܂��v�t���O�����Ă��Ȃ������ׂ�
 *
 * @param   clsys		
 *
 * @retval  BOOL		TRUE:�t���O�����Ă���
 */
//==================================================================
BOOL ColosseumTool_ReceiveCheck_Leave(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i;
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(clsys->recvbuf.leave[i] == TRUE){
      return TRUE;
    }
  }
  return FALSE;
}

//==================================================================
/**
 * �R���V�A���̏o�����W�ɂ��邩�`�F�b�N
 *
 * @param   fieldWork		
 *
 * @retval  BOOL		TRUE:�o�����W�ɂ���
 */
//==================================================================
BOOL ColosseumTool_CheckWayOut(FIELD_MAIN_WORK *fieldWork)
{
  VecFx32 pos;
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
  const POINT_S16 *postbl;
  int i, tbl_max;
  
  FIELD_PLAYER_GetPos(player, &pos);
  pos.x >>= FX32_SHIFT;
  pos.z >>= FX32_SHIFT;

  if(FIELDMAP_GetZoneID(fieldWork) == ZONE_ID_CLOSSEUM){
    postbl = ColosseumWayOutPos_1vs1;
    tbl_max = NELEMS(ColosseumWayOutPos_1vs1);
  }
  else{
    postbl = ColosseumWayOutPos_multi;
    tbl_max = NELEMS(ColosseumWayOutPos_multi);
  }
  
  for(i = 0; i < tbl_max; i++){
    if(pos.x == postbl[i].x && pos.z == postbl[i].z){
      return TRUE;
    }
  }
  return FALSE;
}

//==================================================================
/**
 * �ʐM�v���C���[�̍��W�X�V
 *
 * @param   clsys		    
 */
//==================================================================
void ColosseumTool_CommPlayerUpdate(COLOSSEUM_SYSTEM_PTR clsys)
{
  int net_id, my_net_id;
  COMM_PLAYER_PACKAGE pospack;
  
  my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  for(net_id = 0; net_id < COLOSSEUM_MEMBER_MAX; net_id++){
    if(my_net_id == net_id){
      continue;
    }
    if(Colosseum_GetCommPlayerPos(clsys, net_id, &pospack) == TRUE){
      if(CommPlayer_CheckOcc(clsys->cps, net_id) == FALSE){
        CommPlayer_Add(clsys->cps, net_id, clsys->basic_status[net_id].sex, &pospack);
      }
      else{
        CommPlayer_SetParam(clsys->cps, net_id, &pospack);
      }
    }
  }
}

