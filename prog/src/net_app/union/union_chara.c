//==============================================================================
/**
 * @file    union_char.c
 * @brief   ���j�I�����[���ł̐l���֘A
 * @author  matsuda
 * @date    2009.07.06(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_comm.h"
#include "union_types.h"
#include "net_app/union/union_main.h"
#include "net_app/union/union_beacon_tool.h"
#include "fieldmap/zone_id.h"
#include "net_app/union/union_chara.h"
#include "union_local.h"
#include "union_msg.h"


//==============================================================================
//  �^��`
//==============================================================================
///�r�[�R��PC�̓���֐��^
typedef BOOL (*UNICHARA_FUNC)(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, u8 *seq);

///�r�[�R��PC�̓���e�[�u���^
typedef struct{
  UNICHARA_FUNC func_init;
  UNICHARA_FUNC func_update;
  UNICHARA_FUNC func_exit;
}UNICHARA_FUNC_DATA;


//==============================================================================
//  �萔��`
//==============================================================================
///�r�[�R��PC����T�u�v���Z�XNo
enum{
  BPC_SUBPROC_INIT,     ///<����������
  BPC_SUBPROC_UPDATE,   ///<�X�V����
  BPC_SUBPROC_EXIT,     ///<�I������
};


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static BOOL UnionCharaFunc(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara);
static BOOL BeaconPC_UpdateLife(UNION_SYSTEM_PTR unisys, UNION_BEACON_PC *bpc);

static BOOL UnicharaSeq_NormalUpdate(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, u8 *seq);


//==============================================================================
//  �f�[�^
//==============================================================================
///�L�����N�^�̔z�u���W
static const struct{
  s16 x;    //�O���b�h���W�FX
  s16 z;    //�O���b�h���W�FZ
}UnionCharPosTbl[] = {
  {10 + 3, 14 - 4},   //index 0
  {10 - 6, 14 - 2},   //index 1
  {10 - 3, 14 - 4},   //index 2
  {10 + 3, 14 - 9},   //index 3
  {10 + 6, 14 - 2},   //index 4
  {10 - 6, 14 - 7},   //index 5
  {10 + 6, 14 - 7},   //index 6
  {10 + 0, 14 - 7},   //index 7
  {10 - 3, 14 - 9},   //index 8
  {10 + 0, 14 - 2},   //index 9
};
SDK_COMPILER_ASSERT(NELEMS(UnionCharPosTbl) == UNION_RECEIVE_BEACON_MAX);

///�����N���̃L�����N�^�̐e����̃I�t�Z�b�g�z�u���W
static const struct{
  s16 x;    //�O���b�h���W�FX
  s16 z;    //�O���b�h���W�FZ
}LinkParentOffsetPosTbl[] = {
  {0, 0},             //�e
  {1, 0},             //�q1
  {-1, 0},            //�q2
  {0, -1},            //�q3
  {0, 1},             //�q4
};


///���j�I���L�����N�^�[��MMDL�w�b�_�[�f�[�^
static const MMDL_HEADER UnionChar_MMdlHeader = 
{
  0,
  0,
  MV_DIR_RND,
  0,	///<�C�x���g�^�C�v
  0,	///<�C�x���g�t���O
  0,	///<�C�x���gID
  DIR_DOWN,	///<�w�����
  0,	///<�w��p�����^ 0
  0,	///<�w��p�����^ 1
  0,	///<�w��p�����^ 2
  0,	///<X�����ړ�����
  0,	///<Z�����ړ�����
  0,	///<�O���b�hX
  0,	///<�O���b�hZ
  0,	///<Y�l fx32�^
};

///�r�[�R��PC�̓���e�[�u��
static const UNICHARA_FUNC_DATA UnicharaFuncTbl[] = {
  {//BPC_EVENT_STATUS_NORMAL
    NULL,
    UnicharaSeq_NormalUpdate,
    NULL,
  },
  {//BPC_EVENT_STATUS_ENTER
    NULL,
    NULL,
    NULL,
  },
  {//BPC_EVENT_STATUS_LEAVE
    NULL,
    NULL,
    NULL,
  },
};
SDK_COMPILER_ASSERT(NELEMS(UnicharaFuncTbl) == BPC_EVENT_STATUS_MAX);



//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * �r�[�R��PC�F�C�x���g���N�G�X�g
 *
 * @param   bpc		          �r�[�R��PC�ւ̃|�C���^
 * @param   event_status		BPC_EVENT_STATUS_???
 *
 * @retval  TRUE:���N�G�X�g�����t�B�@FALSE:���N�G�X�g����
 */
//--------------------------------------------------------------
static BOOL UNION_CHAR_EventReq(UNION_CHARACTER *unichara, int event_status)
{
  unichara->next_event_status = event_status;
  return TRUE;
}

//==================================================================
/**
 * ���j�I���L�����N�^�[�F�o�^
 *
 * @param   unisys		      
 * @param   gdata		        �Q�[���f�[�^�ւ̃|�C���^
 * @param   trainer_view		������
 * @param   sex		          ����
 * @param   chara_index		  �L�����N�^�[Index
 *
 * @retval  MMDL *		      
 */
//==================================================================
static MMDL * UNION_CHAR_AddOBJ(UNION_SYSTEM_PTR unisys, GAMEDATA *gdata, u8 trainer_view, u16 chara_index)
{
  MMDL *addmdl;
  MMDLSYS *mdlsys;
  MMDL_HEADER head;
  u16 oya_chara_index, child_chara_index;
  
  mdlsys = GAMEDATA_GetMMdlSys( gdata );
  oya_chara_index = chara_index >> 8;
  child_chara_index = chara_index & 0x00ff;
  
  head = UnionChar_MMdlHeader;
  head.id = chara_index;
  head.obj_code = UnionView_GetObjCode(trainer_view);
  head.gx = UnionCharPosTbl[oya_chara_index].x + LinkParentOffsetPosTbl[child_chara_index].x;
  head.gz = UnionCharPosTbl[oya_chara_index].z + LinkParentOffsetPosTbl[child_chara_index].z;
  
  OS_TPrintf("obj_code = %d, id = %d\n", head.obj_code, chara_index);
  addmdl = MMDLSYS_AddMMdl(mdlsys, &head, ZONE_ID_UNION);
  return addmdl;
}

//==================================================================
/**
 * ���j�I���L������CharacterIndex���擾����
 *
 * @param   pc		
 * @param   unichara		
 *
 * @retval  u16		
 */
//==================================================================
static u16 UNION_CHARA_GetCharaIndex(UNION_BEACON_PC *pc, UNION_CHARACTER *unichara)
{
  u16 chara_index;
  
  chara_index = (pc->buffer_no << 8) | unichara->child_no;
  return chara_index;
}

//==================================================================
/**
 * CharacterIndex����A�e�Ȃ̂��A�q�Ȃ̂��𔻕ʂ���
 *
 * @param   chara_index		�L�����N�^�C���f�b�N�X
 *
 * @retval  UNION_CHARA_INDEX_PARENT or UNION_CHARA_INDEX_CHILD
 */
//==================================================================
UNION_CHARA_INDEX UNION_CHARA_CheckCharaIndex(u16 chara_index)
{
  u16 oya_chara_index, child_chara_index;
  
  oya_chara_index = chara_index >> 8;
  child_chara_index = chara_index & 0x00ff;
  
  GF_ASSERT(oya_chara_index < UNION_RECEIVE_BEACON_MAX);
  GF_ASSERT(child_chara_index < UNION_CONNECT_PLAYER_NUM);
  
  if(child_chara_index == 0){
    return UNION_CHARA_INDEX_PARENT;
  }
  return UNION_CHARA_INDEX_CHILD;
}

//==================================================================
/**
 * CharacterIndex����A�e�ԍ����擾����
 *
 * @param   chara_index		
 *
 * @retval  u16		
 */
//==================================================================
u16 UNION_CHARA_GetCharaIndex_to_ParentNo(u16 chara_index)
{
  return chara_index >> 8;
}

//==================================================================
/**
 * ���j�I���L�����N�^��MMDL*���擾����
 *
 * @param   unisys		
 * @param   pc		      �ePC��PC�p�����[�^
 * @param   unichara		�Ώۂ̃��j�I���L�����N�^�[�ւ̃|�C���^
 *
 * @retval  MMDL *		
 */
//==================================================================
static MMDL * UNION_CHARA_GetMmdl(UNION_SYSTEM_PTR unisys, UNION_BEACON_PC *pc, UNION_CHARACTER *unichara)
{
  u16 chara_index;
  MMDLSYS *mdlsys;
  MMDL *mmdl;
  
  //�|�C���^������@���Ɖ�ʐ؂�ւ��ŃA�h���X���ς��̂Ŗ���T�[�`�ɂ��Ă���
  chara_index = UNION_CHARA_GetCharaIndex(pc, unichara);
  mdlsys = GAMEDATA_GetMMdlSys( unisys->uniparent->game_data );
  mmdl = MMDLSYS_SearchOBJID(mdlsys, chara_index);
  GF_ASSERT(mmdl != NULL);
  return mmdl;
}

//==================================================================
/**
 * ���j�I���L�����O���[�v�F�L�����N�^�[�o�^
 *
 * @param   unisys		
 * @param   pc		    
 */
//==================================================================
static UNION_CHARACTER * UNION_CHARA_AddChar(UNION_SYSTEM_PTR unisys, UNION_BEACON_PC *pc, UNION_MEMBER *member, int child_no)
{
  int i;
  UNION_CHARACTER *unichara;
  
  unichara = unisys->character;
  for(i = 0; i < UNION_CHARACTER_MAX; i++){
    if(unichara->occ == FALSE){
      unichara->parent_pc = pc;
      unichara->trainer_view = member->trainer_view;
      unichara->sex = member->sex;
      unichara->child_no = child_no;
      unichara->occ = TRUE;
      
      UNION_CHAR_AddOBJ(unisys, unisys->uniparent->game_data, 
        member->trainer_view, UNION_CHARA_GetCharaIndex(pc, unichara));
      
      return unichara;
    }
    unichara++;
  }
  GF_ASSERT(0); //�L�����N�^�o�b�t�@�̋󂫂��Ȃ�
  return NULL;
}

//==================================================================
/**
 * ���j�I���L�����N�^�[���폜����
 *
 * @param   unisys		
 * @param   pc		
 * @param   unichara		
 */
//==================================================================
static void UNION_CHARA_DeleteOBJ(UNION_SYSTEM_PTR unisys, UNION_BEACON_PC *pc, UNION_CHARACTER *unichara)
{
  OS_TPrintf("���j�I���L�����폜 chara_index = %x\n", UNION_CHARA_GetMmdl(unisys, pc, unichara));
  MMDL_Delete(UNION_CHARA_GetMmdl(unisys, pc, unichara));
  GFL_STD_MemClear(unichara, sizeof(UNION_CHARACTER));
}

//==================================================================
/**
 * �V�K�f�[�^������΃L�����N�^�[��Add
 *
 * @param   unisys		
 * @param   pc		
 */
//==================================================================
static void UNION_CHARA_CheckOBJ_Entry(UNION_SYSTEM_PTR unisys, UNION_BEACON_PC *pc)
{
  int i;
  UNION_CHARACTER *unichara;
  UNION_BEACON *beacon;
  
  beacon = &pc->beacon;
  for(i = 0; i < UNION_CONNECT_PLAYER_NUM; i++){
    unichara = pc->chara_group.character[i];
    if(unichara == NULL && beacon->party.member[i].occ == TRUE){
      pc->chara_group.character[i] = UNION_CHARA_AddChar(unisys, pc, &beacon->party.member[i], i);
      UNION_CHAR_EventReq(pc->chara_group.character[i], BPC_EVENT_STATUS_ENTER);
    }
  }
}

//==================================================================
/**
 * ���j�I���L�����N�^�[�F�X�V����
 *
 * @param   unisys		
 */
//==================================================================
void UNION_CHAR_Update(UNION_SYSTEM_PTR unisys, GAMEDATA *gdata)
{
  int i;
  UNION_BEACON_PC *bpc;
  BOOL ret;
  MMDLSYS *mdlsys;
  UNION_CHARACTER *unichara;
  int del_count, child_no;
  
  mdlsys = GAMEDATA_GetMMdlSys( gdata );

  bpc = unisys->receive_beacon;
  for(i = 0; i < UNION_RECEIVE_BEACON_MAX; i++){
    if(bpc->beacon.data_valid == UNION_BEACON_VALID){
      //�����X�V
      if(BeaconPC_UpdateLife(unisys, bpc) == FALSE){
        OS_TPrintf("PC:�������s���� group=%d\n", i);
      }
      
      //�V�K�L�����o�^�`�F�b�N
      UNION_CHARA_CheckOBJ_Entry(unisys, bpc);
      
      del_count = 0;
      for(child_no = 0; child_no < UNION_CONNECT_PLAYER_NUM; child_no++){
        unichara = bpc->chara_group.character[child_no];
        //������s
        if(unichara != NULL){
          ret = UnionCharaFunc(unisys, unichara);
          if(ret == FALSE){
            UNION_CHARA_DeleteOBJ(unisys, bpc, unichara);
            bpc->chara_group.character[child_no] = NULL;
            del_count++;
          }
        }
        else{
          del_count++;
        }
      }
      
      //�S�ẴL���������݂��Ȃ��Ȃ����Ȃ�Delete
      if(del_count >= UNION_CONNECT_PLAYER_NUM){
        GFL_STD_MemClear(bpc, sizeof(UNION_BEACON_PC));
        OS_TPrintf("group %d �̃L�������S�ċ��Ȃ��Ȃ����̂ŏ�����\n", i);
      }
    }
    bpc++;
  }
}

//--------------------------------------------------------------
/**
 * �r�[�R��PC����A�b�v�f�[�g
 *
 * @param   bpc		  �r�[�R��PC�ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:����p���B�@FALSE:����I��(���f�����폜���Ă�������)
 */
//--------------------------------------------------------------
static BOOL UnionCharaFunc(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara)
{
  BOOL next_seq = TRUE;
  UNICHARA_FUNC func;
  
  switch(unichara->func_proc){
  case BPC_SUBPROC_INIT:
    func = UnicharaFuncTbl[unichara->event_status].func_init;
    break;
  case BPC_SUBPROC_UPDATE:
    func = UnicharaFuncTbl[unichara->event_status].func_update;
    break;
  case BPC_SUBPROC_EXIT:
    func = UnicharaFuncTbl[unichara->event_status].func_exit;
    break;
  }

  if(func != NULL){
    next_seq = func(unisys, unichara, &unichara->func_seq);
  }
  
  if(next_seq == TRUE){
    unichara->func_seq = 0;
    unichara->func_proc++;
    if(unichara->func_proc > BPC_SUBPROC_EXIT){
      if(unichara->event_status == BPC_EVENT_STATUS_LEAVE){
        return FALSE; //�ޏo����
      }
      unichara->func_proc = 0;
      unichara->event_status = unichara->next_event_status;
      unichara->next_event_status = BPC_EVENT_STATUS_NORMAL;
    }
  }
  
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �����X�V����
 *
 * @param   unisys		
 * @param   bpc		
 *
 * @retval  BOOL		TRUE:�����͂܂�����B�@FALSE:�������s����
 */
//--------------------------------------------------------------
static BOOL BeaconPC_UpdateLife(UNION_SYSTEM_PTR unisys, UNION_BEACON_PC *bpc)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  
  if(situ->mycomm.calling_pc == bpc || situ->mycomm.answer_pc == bpc || situ->mycomm.connect_pc == bpc){
    if(bpc->life == 0){
      bpc->life = 1;
    }
    return TRUE;  //�b���������A�ڑ�����͎����͌���Ȃ�
  }
  
  if(bpc->life <= 0){
    return FALSE;
  }
  else{
    bpc->life--;
  }
 
  return TRUE;
}

//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �������Ă��Ȃ���ԁF�X�V
 *
 * @param   unisys		
 * @param   bpc		
 * @param   mmdl		
 * @param   seq		
 *
 * @retval  static BOOL		
 */
//==================================================================
static BOOL UnicharaSeq_NormalUpdate(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, u8 *seq)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  UNION_BEACON *beacon;
  MMDL *mmdl;
  
  beacon = &unichara->parent_pc->beacon;
  
  //�����`�F�b�N
  if(unichara->parent_pc->life == 0){
    UNION_CHAR_EventReq(unichara, BPC_EVENT_STATUS_LEAVE);
    return TRUE;
  }
  
  //�����̑��݃`�F�b�N
  if(unichara->parent_pc->beacon.party.member[unichara->child_no].occ == FALSE){
    UNION_CHAR_EventReq(unichara, BPC_EVENT_STATUS_LEAVE);
    return TRUE;
  }

  //�A�s�[���ԍ�����v���Ă���΃W�����v
  if(situ->appeal_no != UNION_APPEAL_NULL 
      && situ->appeal_no == beacon->appeal_no   //�A�s�[���ԍ���v
      && unichara->child_no == 0                //�e����
      && UnionMsg_GetMemberMax(beacon->play_category) > beacon->connect_num){ //�ڑ��l������
    mmdl = UNION_CHARA_GetMmdl(unisys, unichara->parent_pc, unichara);
    if(MMDL_CheckPossibleAcmd(mmdl) == TRUE){
      MMDL_SetAcmd(mmdl, AC_STAY_JUMP_D_8F);
    }
  }
  
  return FALSE;
}

