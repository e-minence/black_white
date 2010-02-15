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
///1vs1�̎��̗����ʒu���W(�O���b�h���W)
static const POINT_S16 ColosseumStandingPos_1vs1[] = {
  {3, 5},   //��
  {9, 5},  //�E
};

///�}���`�̎��̗����ʒu���W(�O���b�h���W)
static const POINT_S16 ColosseumStandingPos_multi[] = {
  {3, 4},     //����
  {9, 4},     //�E��
  {3, 6},     //����
  {9, 6},     //�E��
};

//--------------------------------------------------------------
//  �o�����W
//--------------------------------------------------------------
///1vs1�̎��̗����ʒu���W
static const POINT_S16 ColosseumWayOutPos_1vs1[] = {
  {72, 168},
  {88, 168},
  {104, 168},
  {120, 168},
  {136, 168},
};

///�}���`�̎��̗����ʒu���W
static const POINT_S16 ColosseumWayOutPos_multi[] = {
  {72, 168},
  {88, 168},
  {104, 168},
  {120, 168},
  {136, 168},
};


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _BattleDemoParent_SetTrainerData(COLOSSEUM_SYSTEM_PTR clsys, COMM_BTL_DEMO_TRAINER_DATA *demo_tr, NetID net_id, HEAPID heap_id);



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
BOOL ColosseumTool_CheckStandingPosition(FIELDMAP_WORK *fieldWork, int entry_num, int *stand_pos)
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
  int i;
  s16 grid_x, grid_z;
  MMDL *player_mmdl;
  
  *stand_pos = 0;
  
  player_mmdl = FIELD_PLAYER_GetMMdl( player );
  grid_x = MMDL_GetGridPosX(player_mmdl);
  grid_z = MMDL_GetGridPosZ(player_mmdl);

  if(entry_num == 2){
    for(i = 0; i < NELEMS(ColosseumStandingPos_1vs1); i++){
      if(grid_x == ColosseumStandingPos_1vs1[i].x && grid_z == ColosseumStandingPos_1vs1[i].z){
        *stand_pos = i;
        return TRUE;
      }
    }
  }
  else{
    for(i = 0; i < NELEMS(ColosseumStandingPos_multi); i++){
      if(grid_x == ColosseumStandingPos_multi[i].x && grid_z == ColosseumStandingPos_multi[i].z){
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
BOOL ColosseumTool_CheckWayOut(FIELDMAP_WORK *fieldWork)
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
        u16 obj_code;
        if(MyStatus_GetMySex(&clsys->basic_status[net_id].myst) == PM_MALE){
          obj_code = HERO;
        }
        else{
          obj_code = HEROINE;
        }
        CommPlayer_Add(clsys->cps, net_id, obj_code, &pospack);
      }
      else{
        CommPlayer_SetParam(clsys->cps, net_id, &pospack);
      }
    }
  }
}

//==================================================================
/**
 * �o�g���J�n�E�I���f���pParentWork�쐬
 *
 * @param   clsys		
 * @param   cbdp		
 * @param   heap_id		
 */
//==================================================================
void ColosseumTool_SetupBattleDemoParent(COLOSSEUM_SYSTEM_PTR clsys, COMM_BTL_DEMO_PARAM *cbdp, HEAPID heap_id)
{
  int net_id, member_max, set_id, stand_pos;
  
  GFL_STD_MemClear(cbdp, sizeof(COMM_BTL_DEMO_PARAM));
  
  cbdp->type = COMM_BTL_DEMO_TYPE_MULTI_START;
  member_max = 0;
  for(net_id = 0; net_id < COLOSSEUM_MEMBER_MAX; net_id++){
    member_max++;
    if(clsys->recvbuf.pokeparty_occ[net_id] == FALSE){
      cbdp->type = COMM_BTL_DEMO_TYPE_NORMAL_START;
      break;
    }
  }
  
  for(net_id = 0; net_id < member_max; net_id++){
    stand_pos = clsys->recvbuf.stand_position[net_id];
    if((stand_pos & 1) == 0){  //����
      set_id = COMM_BTL_DEMO_TRDATA_A;
    }
    else{ //�E��
      if(cbdp->type == COMM_BTL_DEMO_TYPE_NORMAL_START){
        set_id = COMM_BTL_DEMO_TRDATA_B;
      }
      else{
        set_id = COMM_BTL_DEMO_TRDATA_C;
      }
    }
    _BattleDemoParent_SetTrainerData(
      clsys, &cbdp->trainer_data[set_id + (stand_pos >> 1)], net_id, heap_id);
  }
}

//==================================================================
/**
 * �o�g���J�n�E�I���f���pParentWork�������
 *
 * @param   cbdp		
 */
//==================================================================
void ColosseumTool_DeleteBattleDemoParent(COMM_BTL_DEMO_PARAM *cbdp)
{
  GFL_STD_MemClear(cbdp, sizeof(COMM_BTL_DEMO_PARAM));
}

//--------------------------------------------------------------
/**
 * �o�g���J�n�E�I���f���Ŏg�p����g���[�i�[�f�[�^���Z�b�g
 *
 * @param   clsys		
 * @param   demo_tr		�g���[�i�[�f�[�^�����
 * @param   net_id		�Ώۂ�NetID
 * @param   heap_id		
 */
//--------------------------------------------------------------
static void _BattleDemoParent_SetTrainerData(COLOSSEUM_SYSTEM_PTR clsys, COMM_BTL_DEMO_TRAINER_DATA *demo_tr, NetID net_id, HEAPID heap_id)
{
  demo_tr->party = clsys->recvbuf.pokeparty[net_id];
  demo_tr->mystatus = &clsys->basic_status[net_id].myst;
  demo_tr->server_version = clsys->basic_status[net_id].battle_server_version;
}


#if 0 //��check
void ColosseumTool_ActorSetup(COLOSSEUM_SYSTEM_PTR clsys)
{
  clsys->clunit = GFL_CLACT_UNIT_Create( 6, 10, HEAPID_WORLD);
}

void ColosseumTool_ActorExit(COLOSSEUM_SYSTEM_PTR clsys)
{
  if(clsys->clunit != NULL){
    GFL_CLACT_UNIT_Delete(clsys->clunit);
    clsys->clunit = NULL;
  }
}

void ColosseumTool_PokeIconAdd(COLOSSEUM_SYSTEM_PTR clsys, POKEPARTY *party)
{
  GF_ASSERT(clsys->clunit != NULL);
  
  //���\�[�X�o�^
  for(i = 0; i < TEMOTI_POKEMAX; i++){
    POKEMON_PARAM *pp = PokeParty_GetMemberPointer(party, i);
    BOOL backup_fast;
    u32 data_idx;
    
    backup_fast = PP_FastModeOn( pp );
    
    monsno = PP_Get( pp, ID_PARA_monsno, NULL);
    monsno = PP_Get( pp, ID_PARA_form_no, NULL);
    monsno = PP_Get( pp, ID_PARA_monsno_egg, NULL);
    data_idx = POKEICON_GetCgxArcIndexByMonsNumber( u32 mons, u32 form_no, BOOL egg );
    
    PP_FastModeOff( pp, backup_fast );
    
  {//���\�[�X�o�^
    ARCHANDLE *handle;

    handle = GFL_ARC_OpenDataHandle(ARCID_PALACE, heap_id);
    
  	number->cgr_id = GFL_CLGRP_CGR_Register( 
  	  handle, NARC_palace_debug_areano_NCGR, FALSE, CLSYS_DRAW_MAIN, heap_id );
  	number->pltt_id = GFL_CLGRP_PLTT_RegisterEx( 
  	  handle, NARC_palace_debug_areano_NCLR, CLSYS_DRAW_MAIN, 15*32, 0, 1, heap_id );
  	number->cell_id = GFL_CLGRP_CELLANIM_Register( 
  	  handle, NARC_palace_debug_areano_NCER, NARC_palace_debug_areano_NANR, heap_id );
  	
  	GFL_ARC_CloseDataHandle(handle);
  }
  
  {//�A�N�^�[����
    static const GFL_CLWK_DATA clwkdata = {
      16, 40, 0, 1, 0,
    };
    
    number->clact = GFL_CLACT_WK_Create(
      palace->clunit, number->cgr_id, number->pltt_id, number->cell_id, 
      &clwkdata, CLSYS_DEFREND_MAIN, heap_id);
    GFL_CLACT_WK_SetAutoAnmFlag(number->clact, TRUE); //�I�[�g�A�j��ON
    GFL_CLACT_WK_SetDrawEnable(number->clact, FALSE);
  }
  
}
#endif
