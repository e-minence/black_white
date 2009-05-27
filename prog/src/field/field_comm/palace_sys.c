//==============================================================================
/**
 * @file  palace_sys.c
 * @brief �t�B�[���h�ʐM�F�p���X
 * @author  matsuda
 * @date  2009.05.01(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "field_comm_main.h"
#include "field_comm_func.h"
#include "field_comm_data.h"
#include "field_comm_sys.h"
#include "field/field_comm/palace_sys.h"

#include "arc_def.h"
#include "palace.naix"
#include "fieldmap/zone_id.h"
#include "system/main.h"
#include "message.naix"
#include "msg/msg_invasion.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�p���X�}�b�v�͈̔�(���̍��W�O�ɏo���ꍇ�A���[�v������K�v������)
enum{
  PALACE_MAP_RANGE_LEFT_X = 512,
  PALACE_MAP_RANGE_RIGHT_X = 1024,

  PALACE_MAP_RANGE_LEN = 512,     ///<�p���X�}�b�v��X��
};

///�p���X�G���A�ԍ������l(�܂��ʐM���ڑ�����Ă��Ȃ��āA���������Ԗڂ�������Ȃ����)
#define PALACE_AREA_NO_NULL     (128)

//==============================================================================
//  �\���̒�`
//==============================================================================
///�f�o�b�O�F�p���X�G���A�ԍ��\���Ǘ��\����
typedef struct{
  GFL_CLWK *clact;
  u32 cgr_id;
  u32 pltt_id;
  u32 cell_id;
  u8 anmseq;
  u8 padding[3];
}PALACE_DEBUG_NUMBER;

///�p���X�V�X�e�����[�N
typedef struct _PALACE_SYS_WORK{
  u8 area;          ///<���@������G���A�ԍ�
  u8 entry_count;   ///<�Q�������v���C���[��(�v���C���[�������Ă�����Ȃ�)
  u8 entry_count_max; ///���܂ł̍ő�Q���l��
  u8 padding;

  GFL_CLUNIT *clunit;
  PALACE_DEBUG_NUMBER number;
}PALACE_SYS_WORK;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void PALACE_SYS_PosUpdate(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply, COMM_FIELD_SYS_PTR comm_field);
static void PALACE_DEBUG_UpdateNumber(PALACE_SYS_PTR palace);


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �p���X�V�X�e�����[�N�m��
 * @param   heap_id		�q�[�vID
 * @retval  PALACE_SYS_PTR		�m�ۂ����p���X���[�N�V�X�e���ւ̃|�C���^
 */
//==================================================================
PALACE_SYS_PTR PALACE_SYS_Alloc(HEAPID heap_id)
{
  PALACE_SYS_PTR palace;
  
  palace = GFL_HEAP_AllocClearMemory(heap_id, sizeof(PALACE_SYS_WORK));
  PALACE_SYS_InitWork(palace);
  return palace;
}

//==================================================================
/**
 * �p���X�V�X�e�����[�N�����ݒ�
 *
 * @param   palace		
 */
//==================================================================
void PALACE_SYS_InitWork(PALACE_SYS_PTR palace)
{
  palace->entry_count = 1;  //����
  palace->entry_count_max = 1;
  palace->area = PALACE_AREA_NO_NULL;
}

//==================================================================
/**
 * �p���X�V�X�e�����[�N���
 *
 * @param   palace		�p���X�V�X�e�����[�N�ւ̃|�C���^
 */
//==================================================================
void PALACE_SYS_Free(PALACE_SYS_PTR palace)
{
  PALACE_DEBUG_DeleteNumberAct(palace);
  
  GFL_HEAP_FreeMemory(palace);
}

//==================================================================
/**
 * �p���X�Ď��X�V����
 *
 * @param   palace		
 */
//==================================================================
void PALACE_SYS_Update(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply, COMM_FIELD_SYS_PTR comm_field)
{
  int net_num;
  int first_create;
  
  if(palace == NULL){
    return;
  }
  
  //�n�߂ĒʐM�m���������̃p���X�̏����ʒu�ݒ�
  //��check�@������Ƃ�������ۂ��B
  //�������͒ʐM�m�����̃R�[���o�b�N���Ńp���X�̃G���A�����ʒu���Z�b�g���Ă�����̂��悢
  first_create = PALACE_DEBUG_CreateNumberAct(palace, GFL_HEAP_LOWID(GFL_HEAPID_APP));
  if(palace->clunit != NULL){
    if(GFL_NET_GetConnectNum() > 1){
      if(palace->area == PALACE_AREA_NO_NULL){
        palace->area = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
        FIELD_COMM_SYS_SetInvalidNetID(comm_field, palace->area);
      }
      GFL_CLACT_WK_SetDrawEnable(palace->number.clact, TRUE);
    }
  }
  
  //���[�U�[�̑����m�F
  net_num = GFL_NET_GetConnectNum();
  if(net_num > palace->entry_count){
    OS_TPrintf("palace:���[�U�[�������� old=%d, new=%d\n", palace->entry_count, net_num);
    palace->entry_count = net_num;
    palace->entry_count_max = net_num;
    //��check�@�e�̏ꍇ�A�����ő�����entry_count����đ��M
    //         ���݂�GFL_NET_GetConnectNum�Őe�q�֌W�������₵�Ă���
    //         entry_count�̈Ӗ����u�Q�������v���C���[��(�v���C���[�������Ă�����Ȃ�)�v�ׁ̈A
    //         �e���Ǘ����A���M����K�v������
      ;
  }
  else if(FIELD_COMM_SYS_GetExitReq(comm_field) == FALSE 
      && net_num < palace->entry_count && palace->entry_count_max > 1 ){
    OS_TPrintf("palace:���[�U�[���������̂ŋ����G���[����\n");
    GFL_NET_FatalErrorFunc(1);
    palace->entry_count_max = net_num;
    palace->entry_count = net_num;
  }
  
  //���@�̈ʒu�m�F���A���[�v����
  PALACE_SYS_PosUpdate(palace, plwork, fldply, comm_field);

  PALACE_DEBUG_UpdateNumber(palace);
}

//--------------------------------------------------------------
/**
 * ���@�̈ʒu�m�F���A���[�v����
 *
 * @param   palace		
 * @param   plwork		
 * @param   fldply		
 */
//--------------------------------------------------------------
static void PALACE_SYS_PosUpdate(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply, COMM_FIELD_SYS_PTR comm_field)
{
  ZONEID zone_id;
  VecFx32 pos, new_pos;
  s32 offs_left, offs_right;
  int new_area, now_area;
  
  zone_id = PLAYERWORK_getZoneID(plwork);
  if(zone_id != ZONE_ID_PALACETEST || FIELD_PLAYER_CheckLiveFldMMdl(fldply) == FALSE){
    return;
  }
  
  FIELD_PLAYER_GetPos( fldply, &pos );
  new_pos = pos;
  now_area = (palace->area == PALACE_AREA_NO_NULL) ? 0 : palace->area;
  new_area = now_area;
  
  offs_left = FX_Whole(pos.x) - PALACE_MAP_RANGE_LEFT_X;
  if(offs_left <= 0){
    new_pos.x = (PALACE_MAP_RANGE_RIGHT_X + offs_left) << FX32_SHIFT;
    new_area--;
  }
  else{
    offs_right = FX_Whole(pos.x) - PALACE_MAP_RANGE_RIGHT_X;
    if(offs_right > 0){
      new_pos.x = (PALACE_MAP_RANGE_LEFT_X + offs_right) << FX32_SHIFT;
      new_area++;
    }
  }

  if(now_area != new_area){
    if(new_area < 0){
      new_area = palace->entry_count - 1;
    }
    else if(new_area >= palace->entry_count){
      new_area = 0;
    }
    OS_TPrintf("palace:���[�v old_x=%d, new_x=%d, old_area=%d, new_area=%d\n", 
      FX_Whole(pos.x), FX_Whole(new_pos.x), now_area, new_area);
  }
  FIELD_PLAYER_SetPos( fldply, &new_pos );
  if(palace->area != PALACE_AREA_NO_NULL){
    palace->area = new_area;
    FIELD_COMM_SYS_SetInvalidNetID(comm_field, palace->area);
  }
}

//==================================================================
/**
 * �p���X�̃G���A�ԍ���ݒ�
 *
 * @param   palace		    �p���X�V�X�e�����[�N�ւ̃|�C���^
 * @param   palace_area   �p���X�̃G���A�ԍ�
 */
//==================================================================
void PALACE_SYS_SetArea(PALACE_SYS_PTR palace, u8 palace_area)
{
  if(palace == NULL){
    return;
  }
  palace->area = palace_area;
}

//==================================================================
/**
 * �p���X�̃G���A�ԍ����擾
 *
 * @param   palace		�p���X�V�X�e�����[�N�ւ̃|�C���^
 *
 * @retval  u8    		�G���A�ԍ�
 */
//==================================================================
u8 PALACE_SYS_GetArea(PALACE_SYS_PTR palace)
{
  if(palace == NULL){
    return 0;
  }
  return palace->area;
}

//==================================================================
/**
 * �F�B�̎��@���W���p���X�̃G���AID�𔻒肵�ďC�����s��
 *
 * @param   palace		      �p���X�V�X�e���ւ̃|�C���^
 * @param   friend_area	  	�F�B�̃p���X�G���A
 * @param   my_plwork		    ������PLAYER_WORK
 * @param   friend_plwork		�F�B��PLAYER_WORK
 */
//==================================================================
void PALACE_SYS_FriendPosConvert(PALACE_SYS_PTR palace, int friend_area, 
  PLAYER_WORK *my_plwork, PLAYER_WORK *friend_plwork)//, FIELD_PLAYER *friend_fldply)
{
  int left_area, right_area;
  VecFx32 pos;
  
  if(palace == NULL || palace->area == friend_area 
      || PLAYERWORK_getZoneID(my_plwork) != PLAYERWORK_getZoneID(friend_plwork)){
    return;
  }
  
  left_area = palace->area - 1;
  right_area = palace->area + 1;
  if(left_area < 0){
    left_area = palace->entry_count - 1;
  }
  if(right_area >= palace->entry_count){
    right_area = 0;
  }
  
//  FIELD_PLAYER_GetPos( friend_fldply, &pos );
  pos = *(PLAYERWORK_getPosition(friend_plwork));
  if(right_area == left_area && friend_area == right_area){
    //��l�̏ꍇ�͉E�����������G���A�ׁ̈A���W�Ŕ���
    if(pos.x > ((PALACE_MAP_RANGE_LEFT_X + PALACE_MAP_RANGE_LEN/2) << FX32_SHIFT)){
      pos.x -= PALACE_MAP_RANGE_LEN << FX32_SHIFT;
    }
    else{
      pos.x += PALACE_MAP_RANGE_LEN << FX32_SHIFT;
    }
  }
  else if(friend_area == right_area){  //�E�ׂ̃G���A�ɂ���ꍇ
    pos.x += PALACE_MAP_RANGE_LEN << FX32_SHIFT;
  }
  else if(friend_area == left_area){ //���ׂ̃G���A�ɂ���ꍇ
    pos.x -= PALACE_MAP_RANGE_LEN << FX32_SHIFT;
  }
  else{ //���ׂ̃G���A�ł͂Ȃ��ꍇ
    pos.x += (PALACE_MAP_RANGE_LEN * 2) << FX32_SHIFT;
  }
//  FIELD_PLAYER_SetPos( friend_fldply, &pos );
  PLAYERWORK_setPosition(friend_plwork, &pos);
}

//--------------------------------------------------------------
/**
 * �f�o�b�O�F�p���X�G���A�ԍ��\���A�N�^�[����
 *
 * @param   palace		
 * @param   heap_id		
 */
//--------------------------------------------------------------
BOOL PALACE_DEBUG_CreateNumberAct(PALACE_SYS_PTR palace, HEAPID heap_id)
{
  PALACE_DEBUG_NUMBER *number = &palace->number;

  if(palace->clunit != NULL){
    return FALSE;
  }

  palace->clunit = GFL_CLACT_UNIT_Create( 1, 10, heap_id );
  
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
      16, 16, 0, 1, 0,
    };
    
    number->clact = GFL_CLACT_WK_Create(
      palace->clunit, number->cgr_id, number->pltt_id, number->cell_id, 
      &clwkdata, CLSYS_DEFREND_MAIN, heap_id);
    GFL_CLACT_WK_SetAutoAnmFlag(number->clact, TRUE); //�I�[�g�A�j��ON
    GFL_CLACT_WK_SetDrawEnable(number->clact, FALSE);
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �f�o�b�O�F�p���X�G���A�ԍ��\���A�N�^�[�폜
 *
 * @param   palace		
 */
//--------------------------------------------------------------
void PALACE_DEBUG_DeleteNumberAct(PALACE_SYS_PTR palace)
{
  PALACE_DEBUG_NUMBER *number = &palace->number;
  
  if(palace->clunit == NULL){
    return;
  }
  
  GFL_CLACT_WK_Remove(number->clact);

  GFL_CLGRP_CGR_Release(number->cgr_id);
  GFL_CLGRP_PLTT_Release(number->pltt_id);
  GFL_CLGRP_CELLANIM_Release(number->cell_id);

  GFL_CLACT_UNIT_Delete(palace->clunit);
  palace->clunit = NULL;
}

//==================================================================
/**
 * �f�o�b�O�F�p���X�G���A�ԍ��\���A�N�^�[�̕\���E��\���ݒ�
 *
 * @param   palace		
 * @param   enable		
 */
//==================================================================
void PALACE_DEBUG_EnableNumberAct(PALACE_SYS_PTR palace, BOOL enable)
{
  if(palace->clunit == NULL){
    return;
  }
  GFL_CLACT_WK_SetDrawEnable(palace->number.clact, enable);
}

//--------------------------------------------------------------
/**
 * �f�o�b�O�F�p���X�G���A�ԍ��A�N�^�[�X�V����
 *
 * @param   palace		
 */
//--------------------------------------------------------------
static void PALACE_DEBUG_UpdateNumber(PALACE_SYS_PTR palace)
{
  PALACE_DEBUG_NUMBER *number = &palace->number;
  
  if(palace->clunit == NULL || palace->area == number->anmseq || palace->area == PALACE_AREA_NO_NULL){
    return;
  }
  
  number->anmseq = palace->area % 4;  //�����A�j����4�p�^�[�������p�ӂ��Ă��Ȃ�
  GFL_CLACT_WK_SetAnmSeq( number->clact, number->anmseq );
}

//==============================================================================
//
//  ���̓���ʒu�܂ŗ��āA�N�Ƃ��ʐM���Ă��Ȃ��Ȃ璍�Ӄ��b�Z�[�W���o���Ď��@��߂�
//
//==============================================================================
typedef struct{
  FIELD_MAIN_WORK *fieldWork;
  FLDMMDL *player_mmdl;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  BOOL left_right;
}DEBUG_PALACE_NGWIN;

static GMEVENT_RESULT DebugPalaceNGWinEvent( GMEVENT *event, int *seq, void *wk );

GMEVENT * EVENT_DebugPalaceNGWin( GAMESYS_WORK *gsys, FIELD_MAIN_WORK *fieldWork, FIELD_PLAYER *fld_player, BOOL left_right )
{
  DEBUG_PALACE_NGWIN *ngwin;
  GMEVENT * event;
  
  event = GMEVENT_Create(gsys, NULL, DebugPalaceNGWinEvent, sizeof(DEBUG_PALACE_NGWIN));
  
  ngwin = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear( ngwin, sizeof(DEBUG_PALACE_NGWIN) );
  
  ngwin->fieldWork = fieldWork;
  ngwin->player_mmdl = FIELD_PLAYER_GetFldMMdl(fld_player);
  ngwin->left_right = left_right;
  
  return event;
}

static GMEVENT_RESULT DebugPalaceNGWinEvent( GMEVENT *event, int *seq, void *wk )
{
  DEBUG_PALACE_NGWIN *ngwin = wk;
  
  switch(*seq){
  case 0:
    {
      FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(ngwin->fieldWork);
      ngwin->msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
      ngwin->msgWin = FLDMSGWIN_AddTalkWin( msgBG, ngwin->msgData );
      FLDMSGWIN_Print( ngwin->msgWin, 0, 0, msg_invasion_test08_01 );
      GXS_SetMasterBrightness(-16);
      (*seq)++;
    }
    break;
  case 1:
    if( FLDMSGWIN_CheckPrintTrans(ngwin->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 2:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
        (*seq)++;
      }
    }
    break;
  case 3:
    FLDMSGWIN_Delete( ngwin->msgWin );
    GFL_MSG_Delete( ngwin->msgData );
    GXS_SetMasterBrightness(0);
    (*seq)++;
    break;
  case 4:
    if(FLDMMDL_CheckPossibleAcmd(ngwin->player_mmdl) == TRUE){
      u16 code = (ngwin->left_right == 0) ? AC_WALK_R_16F : AC_WALK_L_16F;
      FLDMMDL_SetAcmd(ngwin->player_mmdl, code);
      (*seq)++;
    }
    break;
  case 5:
    if(FLDMMDL_CheckEndAcmd(ngwin->player_mmdl) == TRUE){
      FLDMMDL_EndAcmd(ngwin->player_mmdl);
      return( GMEVENT_RES_FINISH );
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}
