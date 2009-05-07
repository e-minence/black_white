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
  u8 padding[2];
  
  GFL_CLUNIT *clunit;
  PALACE_DEBUG_NUMBER number;
}PALACE_SYS_WORK;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void PALACE_SYS_PosUpdate(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply);
static void PALACE_DEBUG_CreateNumberAct(PALACE_SYS_PTR palace, HEAPID heap_id);
static void PALACE_DEBUG_DeleteNumberAct(PALACE_SYS_PTR palace);
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
  palace->entry_count = 1;  //����
  palace->area = PALACE_AREA_NO_NULL;
  
  palace->clunit = GFL_CLACT_UNIT_Create( 1, 10, heap_id );
  PALACE_DEBUG_CreateNumberAct(palace, heap_id);
  
  return palace;
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
  GFL_CLACT_UNIT_Delete(palace->clunit);
  
  GFL_HEAP_FreeMemory(palace);
}

//==================================================================
/**
 * �p���X�Ď��X�V����
 *
 * @param   palace		
 */
//==================================================================
void PALACE_SYS_Update(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply)
{
  int net_num;

  if(palace == NULL){
    return;
  }
  
  //�n�߂ĒʐM�m���������̃p���X�̏����ʒu�ݒ�
  //��check�@������Ƃ�������ۂ��B
  //�������͒ʐM�m�����̃R�[���o�b�N���Ńp���X�̃G���A�����ʒu���Z�b�g���Ă�����̂��悢
  if(palace->area == PALACE_AREA_NO_NULL && GFL_NET_GetConnectNum() > 1){
    palace->area = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
    GFL_CLACT_WK_SetDrawEnable(palace->number.clact, TRUE);
  }
  
  //���[�U�[�̑����m�F
  net_num = GFL_NET_GetConnectNum();
  if(net_num > palace->entry_count){
    OS_TPrintf("palace:���[�U�[�������� old=%d, new=%d\n", palace->entry_count, net_num);
    palace->entry_count = net_num;
    //��check�@�e�̏ꍇ�A�����ő�����entry_count����đ��M
    //         ���݂�GFL_NET_GetConnectNum�Őe�q�֌W�������₵�Ă���
    //         entry_count�̈Ӗ����u�Q�������v���C���[��(�v���C���[�������Ă�����Ȃ�)�v�ׁ̈A
    //         �e���Ǘ����A���M����K�v������
      ;
  }
  
  //���@�̈ʒu�m�F���A���[�v����
  PALACE_SYS_PosUpdate(palace, plwork, fldply);

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
static void PALACE_SYS_PosUpdate(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply)
{
  ZONEID zone_id;
  VecFx32 pos, new_pos;
  s32 offs_left, offs_right;
  int new_area, now_area;
  
  zone_id = PLAYERWORK_getZoneID(plwork);
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
  if(friend_area == right_area){  //�E�ׂ̃G���A�ɂ���ꍇ
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
static void PALACE_DEBUG_CreateNumberAct(PALACE_SYS_PTR palace, HEAPID heap_id)
{
  PALACE_DEBUG_NUMBER *number = &palace->number;
  
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
}

//--------------------------------------------------------------
/**
 * �f�o�b�O�F�p���X�G���A�ԍ��\���A�N�^�[�폜
 *
 * @param   palace		
 */
//--------------------------------------------------------------
static void PALACE_DEBUG_DeleteNumberAct(PALACE_SYS_PTR palace)
{
  PALACE_DEBUG_NUMBER *number = &palace->number;

  GFL_CLACT_WK_Remove(number->clact);

  GFL_CLGRP_CGR_Release(number->cgr_id);
  GFL_CLGRP_PLTT_Release(number->pltt_id);
  GFL_CLGRP_CELLANIM_Release(number->cell_id);
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
  
  if(palace->area == number->anmseq || palace->area == PALACE_AREA_NO_NULL){
    return;
  }
  
  number->anmseq = palace->area % 4;  //�����A�j����4�p�^�[�������p�ӂ��Ă��Ȃ�
  GFL_CLACT_WK_SetAnmSeq( number->clact, number->anmseq );
}

