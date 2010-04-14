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
#include "union_tool.h"
#include "net_app/union_eff.h"
#include "field/fieldmap.h"
#include "savedata/etc_save.h"


//==============================================================================
//  �^��`
//==============================================================================
///�r�[�R��PC�̓���֐��^
typedef BOOL (*UNICHARA_FUNC)(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, FIELDMAP_WORK *fieldmap, u8 *seq);

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

///�X�N���v�g��OBJID�Ɣ��Ȃ��悤�Ƀv���O�����ł��炷�I�t�Z�b�g
#define UNION_CHARINDEX_OFFSET    (32)


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static BOOL UnionCharaFunc(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, FIELDMAP_WORK *fieldmap);
static BOOL BeaconPC_UpdateLife(UNION_SYSTEM_PTR unisys, UNION_BEACON_PC *bpc);

static BOOL UnicharaSeq_NormalUpdate(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, FIELDMAP_WORK *fieldmap, u8 *seq);
static BOOL UnicharaSeq_EnterUpdate(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, FIELDMAP_WORK *fieldmap, u8 *seq);
static BOOL UnicharaSeq_LeaveUpdate(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, FIELDMAP_WORK *fieldmap, u8 *seq);
static BOOL UnicharaSeq_TalkingUpdate(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, FIELDMAP_WORK *fieldmap, u8 *seq);


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
    UnicharaSeq_EnterUpdate,
    NULL,
  },
  {//BPC_EVENT_STATUS_LEAVE
    NULL,
    UnicharaSeq_LeaveUpdate,
    NULL,
  },
  {//BPC_EVENT_STATUS_TALKING
    NULL,
    UnicharaSeq_TalkingUpdate,
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
  child_chara_index = (chara_index & 0x00ff) - UNION_CHARINDEX_OFFSET;
  
  head = UnionChar_MMdlHeader;
  head.id = chara_index;
  head.obj_code = UnionView_GetObjCode(trainer_view);
  MMDLHEADER_SetGridPos( &head,
      UnionCharPosTbl[oya_chara_index].x + LinkParentOffsetPosTbl[child_chara_index].x,
      UnionCharPosTbl[oya_chara_index].z + LinkParentOffsetPosTbl[child_chara_index].z,
      0 );
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
  
  chara_index = (pc->buffer_no << 8) | (unichara->child_no + UNION_CHARINDEX_OFFSET);
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
  child_chara_index = (chara_index & 0x00ff) - UNION_CHARINDEX_OFFSET;
  
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
 * CharacterIndex���ʐM�v���C���[��NPC���𔻒肷��
 *
 * @param   chara_index		
 *
 * @retval  BOOL		TRUE:�ʐM�v���C���[�@�@FALSE:NPC
 */
//==================================================================
BOOL UNION_CHARA_CheckCommPlayer(u16 chara_index)
{
  u16 child_chara_index;
  
  child_chara_index = (chara_index & 0x00ff);
  if(child_chara_index < UNION_CHARINDEX_OFFSET){
    return FALSE;
  }
  return TRUE;
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
static UNION_CHARACTER * UNION_CHARA_AddChar(UNION_SYSTEM_PTR unisys, UNION_BEACON_PC *pc, UNION_MEMBER *member, int child_no, BOOL enable, MMDL **dest_mmdl_ptr)
{
  int i;
  UNION_CHARACTER *unichara;
  MMDL *mmdl;
  
  unichara = unisys->character;
  for(i = 0; i < UNION_CHARACTER_MAX; i++){
    if(unichara->occ == FALSE){
      unichara->parent_pc = pc;
      unichara->trainer_view = member->trainer_view;
      unichara->sex = member->sex;
      unichara->child_no = child_no;
      unichara->occ = TRUE;
      
      mmdl = UNION_CHAR_AddOBJ(unisys, unisys->uniparent->game_data, 
        member->trainer_view, UNION_CHARA_GetCharaIndex(pc, unichara));
      if(enable == FALSE){
        MMDL_SetStatusBitVanish( mmdl, TRUE );
      }
      *dest_mmdl_ptr = mmdl;
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
static void UNION_CHARA_CheckOBJ_Entry(UNION_SYSTEM_PTR unisys, ETC_SAVE_WORK *etc_save, WIFI_LIST *wifilist, UNION_EFF_SYSTEM *unieff, UNION_BEACON_PC *pc)
{
  int i;
  UNION_CHARACTER *unichara;
  UNION_BEACON *beacon;
  MMDL *mmdl;
  
  beacon = &pc->beacon;
  for(i = 0; i < UNION_CONNECT_PLAYER_NUM; i++){
    unichara = pc->chara_group.character[i];
    if(unichara == NULL && beacon->party.member[i].occ == TRUE){
      pc->chara_group.character[i] = UNION_CHARA_AddChar(unisys, pc, &beacon->party.member[i], i, FALSE, &mmdl);
      pc->chara_group.character[i]->event_status = BPC_EVENT_STATUS_ENTER;
      UNION_CHAR_EventReq(pc->chara_group.character[i], BPC_EVENT_STATUS_NORMAL);
      if(i == 0){ //��L�����̂ݗF�B�}�[�N�`�F�b�N
        if(UnionTool_CheckDwcFriend(wifilist, beacon) == TRUE){
          UnionEff_App_ReqFriendMark(unieff, mmdl, FRIENDMARK_TYPE_FRIEND);
        }
        else if(EtcSave_CheckAcquaintance(etc_save, beacon->trainer_id) == TRUE){
          UnionEff_App_ReqFriendMark(unieff, mmdl, FRIENDMARK_TYPE_ACQUAINTANCE);
        }
      #ifdef PM_DEBUG   //ibe mana ��p ��check�@��ŏ���
        else if(GFL_UI_KEY_GetCont() & PAD_BUTTON_R){
          UnionEff_App_ReqFriendMark(unieff, mmdl, GFUser_GetPublicRand(2));
        }
      #endif
      }
    }
  }
}

//==================================================================
/**
 * �t�H�[�J�X�Ώۂ̃L�����N�^�[������΃t�H�[�J�X�G�t�F�N�g�����N�G�X�g
 *
 * @param   unisys		
 * @param   pc		
 */
//==================================================================
static BOOL UNION_CHARA_CheckFocusOBJ(UNION_SYSTEM_PTR unisys, UNION_BEACON_PC *pc)
{
  int i;
  UNION_CHARACTER *unichara;
  UNION_BEACON *beacon;
  
  beacon = &pc->beacon;
  for(i = 0; i < UNION_CONNECT_PLAYER_NUM; i++){
    unichara = pc->chara_group.character[i];
    if(unichara == NULL){
      continue;
    }
    
    if(beacon->party.member[i].occ == TRUE
        && GFL_STD_MemComp(beacon->party.member[i].mac_address, 
        unisys->my_situation.focus_mac_address, 6) == 0){
      if(GAMESYSTEM_CheckFieldMapWork(unisys->uniparent->gsys) == TRUE){
        FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(unisys->uniparent->gsys);
        UNION_EFF_SYSTEM *unieff = FIELDMAP_GetUnionEffSystem(fieldWork);
        
        if(unieff != NULL){
          u16 char_index = UNION_CHARA_GetCharaIndex(pc, unichara);
          MMDLSYS *mdlsys = GAMEDATA_GetMMdlSys( unisys->uniparent->game_data );
          MMDL *focus_mmdl = MMDLSYS_SearchOBJID( mdlsys, char_index );
        
          if(focus_mmdl != NULL){
            UnionEff_App_ReqFocus(unieff, focus_mmdl);
          }
        }
      }
      GFL_STD_MemClear(unisys->my_situation.focus_mac_address, 6);
      return TRUE;
    }
  }
  return FALSE;
}

static void _ReqFocusOBJ(UNION_SYSTEM_PTR unisys, u16 char_index)
{
}


//==================================================================
/**
 * ���j�I���L�����N�^�[�F�X�V����
 *
 * @param   unisys		
 */
//==================================================================
void UNION_CHAR_Update(UNION_SYSTEM_PTR unisys, GAMEDATA *gdata, FIELDMAP_WORK *fieldmap)
{
  int i;
  UNION_BEACON_PC *bpc;
  BOOL ret;
  MMDLSYS *mdlsys;
  UNION_CHARACTER *unichara;
  int del_count, child_no;
  const u8 focus_null_mac[6] = {0,0,0,0,0,0};
  BOOL focus_check = FALSE;
  ETC_SAVE_WORK *etc_save = SaveData_GetEtc( GAMEDATA_GetSaveControlWork(gdata) );
  WIFI_LIST* wifilist = GAMEDATA_GetWiFiList(gdata);
  UNION_EFF_SYSTEM *unieff = FIELDMAP_GetUnionEffSystem(fieldmap);
  
  mdlsys = GAMEDATA_GetMMdlSys( gdata );

  if(GFL_STD_MemComp(focus_null_mac, &unisys->my_situation.focus_mac_address, 6) != 0){
    focus_check = TRUE;
  }

  bpc = unisys->receive_beacon;
  for(i = 0; i < UNION_RECEIVE_BEACON_MAX; i++){
    if(bpc->beacon.data_valid == UNION_BEACON_VALID){
      //�����X�V
      if(BeaconPC_UpdateLife(unisys, bpc) == FALSE){
        OS_TPrintf("PC:�������s���� group=%d\n", i);
      }
      
      //�V�K�L�����o�^�`�F�b�N
      UNION_CHARA_CheckOBJ_Entry(unisys, etc_save, wifilist, unieff, bpc);

      //�t�H�[�J�X�L�����`�F�b�N
      if(focus_check == TRUE){
        if(UNION_CHARA_CheckFocusOBJ(unisys, bpc) == TRUE){
          focus_check = FALSE;
        }
      }
      
      del_count = 0;
      for(child_no = 0; child_no < UNION_CONNECT_PLAYER_NUM; child_no++){
        unichara = bpc->chara_group.character[child_no];
        //������s
        if(unichara != NULL){
          ret = UnionCharaFunc(unisys, unichara, fieldmap);
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
  GFL_STD_MemClear(unisys->my_situation.focus_mac_address, 6);
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
static BOOL UnionCharaFunc(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, FIELDMAP_WORK *fieldmap)
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
    next_seq = func(unisys, unichara, fieldmap, &unichara->func_seq);
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
  
  if(situ->mycomm.talk_pc == bpc || situ->mycomm.calling_pc == bpc || situ->mycomm.answer_pc == bpc || situ->mycomm.connect_pc == bpc){
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
static BOOL UnicharaSeq_NormalUpdate(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, FIELDMAP_WORK *fieldmap, u8 *seq)
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
  
  //�b����������Ȃ�΃v���C���[�̕���������
  if(situ->mycomm.talk_obj_id == UNION_CHARA_GetCharaIndex(unichara->parent_pc, unichara)){
    UNION_CHAR_EventReq(unichara, BPC_EVENT_STATUS_TALKING);
    return TRUE;
  }
  else if(situ->appeal_no != UNION_APPEAL_NULL //�A�s�[���ԍ�����v���Ă���΃W�����v
      && (situ->appeal_no == UnionTool_PlayCategory_to_AppealNo(beacon->play_category)
      || situ->appeal_no == beacon->appeal_no)   //�A�s�[���ԍ���v
      && unichara->child_no == 0                //�e����
      && UnionMsg_GetMemberMax(beacon->play_category) > beacon->connect_num){ //�ڑ��l������
    mmdl = UNION_CHARA_GetMmdl(unisys, unichara->parent_pc, unichara);
    if(MMDL_CheckPossibleAcmd(mmdl) == TRUE){
      MMDL_SetAcmd(mmdl, AC_STAY_JUMP_D_8F);
    }
  }
  
  return FALSE;
}

//==================================================================
/**
 * �����F�X�V
 *
 * @param   unisys		
 * @param   bpc		
 * @param   mmdl		
 * @param   seq		
 *
 * @retval  static BOOL		
 */
//==================================================================
static BOOL UnicharaSeq_EnterUpdate(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, FIELDMAP_WORK *fieldmap, u8 *seq)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  UNION_BEACON *beacon;
  MMDL *mmdl;

  mmdl = UNION_CHARA_GetMmdl(unisys, unichara->parent_pc, unichara);
  
  switch(*seq){
  case 0:
    if(MMDL_CheckPossibleAcmd(mmdl) == TRUE){
      MMDL_SetAcmd(mmdl, AC_WARP_DOWN);
      MMDL_SetStatusBitVanish( mmdl, FALSE );
      (*seq)++;
    }
    break;
  case 1:
    if(MMDL_CheckEndAcmd(mmdl) == TRUE){
      MMDL_EndAcmd(mmdl);
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//==================================================================
/**
 * �ގ��F�X�V
 *
 * @param   unisys		
 * @param   bpc		
 * @param   mmdl		
 * @param   seq		
 *
 * @retval  static BOOL		
 */
//==================================================================
static BOOL UnicharaSeq_LeaveUpdate(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, FIELDMAP_WORK *fieldmap, u8 *seq)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  UNION_BEACON *beacon;
  MMDL *mmdl;

  mmdl = UNION_CHARA_GetMmdl(unisys, unichara->parent_pc, unichara);
  
  switch(*seq){
  case 0:
    if(MMDL_CheckPossibleAcmd(mmdl) == TRUE){
      MMDL_SetAcmd(mmdl, AC_WARP_UP);
      (*seq)++;
    }
    break;
  case 1:
    if(MMDL_CheckEndAcmd(mmdl) == TRUE){
      MMDL_EndAcmd(mmdl);
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//==================================================================
/**
 * ���b���F�X�V
 *
 * @param   unisys		
 * @param   bpc		
 * @param   mmdl		
 * @param   seq		
 *
 * @retval  static BOOL		
 */
//==================================================================
static BOOL UnicharaSeq_TalkingUpdate(UNION_SYSTEM_PTR unisys, UNION_CHARACTER *unichara, FIELDMAP_WORK *fieldmap, u8 *seq)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  MMDL *mmdl;

  mmdl = UNION_CHARA_GetMmdl(unisys, unichara->parent_pc, unichara);

  switch(*seq){
  case 0:
    if(MMDL_CheckPossibleAcmd(mmdl) == TRUE)
    {
      FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldmap);
    	u16 dir = MMDL_GetDirDisp( FIELD_PLAYER_GetMMdl( player ) );
    	u16 movecode, anmcode;
    	switch(dir){
      case DIR_UP:
        movecode = MV_DOWN;
        anmcode = AC_DIR_D;
        break;
      case DIR_DOWN:
        movecode = MV_UP;
        anmcode = AC_DIR_U;
        break;
      case DIR_LEFT:
        movecode = MV_RIGHT;
        anmcode = AC_DIR_R;
        break;
      case DIR_RIGHT:
      default:
        movecode = MV_LEFT;
        anmcode = AC_DIR_L;
        break;
      }
      IWASAWA_Printf( "before mv_code = %d\n", MMDL_GetMoveCode( mmdl ));
      //MMDL_ChangeMoveCode( mmdl, movecode );
      MMDL_SetAcmd(mmdl, anmcode);
      (*seq)++;
    }
    break;
  case 1:
    if(MMDL_EndAcmd(mmdl) == TRUE){ //�A�j���R�}���h�I���҂�
      MMDL_OnStatusBit(mmdl, MMDL_STABIT_PAUSE_DIR);
      (*seq)++;
    }
    break;
  case 2:
    if(situ->mycomm.talk_obj_id != UNION_CHARA_GetCharaIndex(unichara->parent_pc, unichara)){
      //MMDL_ChangeMoveCode( mmdl, MV_DIR_RND );
      IWASAWA_Printf( "after mv_code = %d\n", MMDL_GetMoveCode( mmdl ));
      MMDL_OffStatusBit(mmdl, MMDL_STABIT_PAUSE_DIR);
      return TRUE;
    }
    break;
  }
  return FALSE;
}

