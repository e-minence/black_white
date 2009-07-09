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


//==============================================================================
//  �^��`
//==============================================================================
///�r�[�R��PC�̓���֐��^
typedef BOOL (*BPC_FUNC)(UNION_BEACON_PC *bpc, MMDL *mmdl, u8 *seq);

///�r�[�R��PC�̓���e�[�u���^
typedef struct{
  BPC_FUNC func_init;
  BPC_FUNC func_update;
  BPC_FUNC func_exit;
  u8 event_pri;///<�C�x���g�D�揇��(���l���Ⴂ�C�x���g�ł͑傫���C�x���g����؂�ւ��鎖�͏o���Ȃ�)
  u8 life_dec;  ///<TRUE:���������炵�Ă��悢���
  u8 padding[2];
}BEACON_PC_FUNC;


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
static BOOL BeaconPCFunc(UNION_BEACON_PC *bpc, MMDL *mmdl);


//==============================================================================
//  �f�[�^
//==============================================================================
///�L�����N�^�̔z�u���W
static const struct{
  u16 x;    //�O���b�h���W�FX
  u16 z;    //�O���b�h���W�FZ
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
static const BEACON_PC_FUNC BeaconPcFuncTbl[] = {
  {//BPC_EVENT_STATUS_NULL
    NULL,
    NULL,
    NULL,
    0,
    TRUE,
  },
  {//BPC_EVENT_STATUS_ENTER
    NULL,
    NULL,
    NULL,
    0xff,
    FALSE,
  },
  {//BPC_EVENT_STATUS_LEAVE
    NULL,
    NULL,
    NULL,
    0xff,
    FALSE,
  },
};
SDK_COMPILER_ASSERT(NELEMS(BeaconPcFuncTbl) == BPC_EVENT_STATUS_MAX);



//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * ���j�I���L�����N�^�[�F�o�^
 *
 * @param   unisys		      
 * @param   gdata		        �Q�[���f�[�^�ւ̃|�C���^
 * @param   trainer_view		������
 * @param   sex		          ����
 * @param   char_index		  �L�����N�^�[Index
 *
 * @retval  MMDL *		      
 */
//==================================================================
MMDL * UNION_CHAR_AddOBJ(UNION_SYSTEM_PTR unisys, GAMEDATA *gdata, u8 trainer_view, u16 char_index)
{
  MMDL *addmdl;
  MMDLSYS *mdlsys;
  MMDL_HEADER head;
  
  mdlsys = GAMEDATA_GetMMdlSys( gdata );
  
  head = UnionChar_MMdlHeader;
  head.id = char_index;
  head.obj_code = UnionView_GetObjCode(trainer_view);
  head.gx = UnionCharPosTbl[char_index].x;
  head.gz = UnionCharPosTbl[char_index].z;
  
  OS_TPrintf("obj_code = %d, id = %d\n", head.obj_code, char_index);
  addmdl = MMDLSYS_AddMMdl(mdlsys, &head, ZONE_ID_UNION);
  return addmdl;
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
  MMDL *mmdl;
  MMDLSYS *mdlsys;
  
  mdlsys = GAMEDATA_GetMMdlSys( gdata );

  for(i = 0; i < UNION_RECEIVE_BEACON_MAX; i++){
    bpc = &unisys->receive_beacon[i];
    if(bpc->beacon.data_valid == UNION_BEACON_VALID){
      mmdl = MMDLSYS_SearchOBJID( mdlsys, i );
      if(mmdl == NULL){ //���肦�Ȃ��͂������ǈꉞ�B�����ꍇ�͍폜�����ɂ���
        GFL_STD_MemClear(bpc, sizeof(UNION_BEACON_PC));
        GF_ASSERT_MSG(0, "mmdl == NULL! %d", i);
        continue;
      }
      
      //�����`�F�b�N
      if(BeaconPcFuncTbl[bpc->event_status].life_dec == TRUE){
        if(bpc->life <= 0){
          UNION_CHAR_EventReq(bpc, BPC_EVENT_STATUS_LEAVE);
        }
        else{
          bpc->life--;
          OS_TPrintf("life = %d\n", bpc->life);
        }
      }
      
      //������s
      ret = BeaconPCFunc(bpc, mmdl);
      if(ret == FALSE){
        MMDL_Delete(mmdl);
        GFL_STD_MemClear(bpc, sizeof(UNION_BEACON_PC));
      }
    }
  }
}

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
BOOL UNION_CHAR_EventReq(UNION_BEACON_PC *bpc, int event_status)
{
  //���Ǝ��̃��N�G�X�g�A�����̃v���C�I���e�B���������ꍇ�̓��N�G�X�g��t
  if(BeaconPcFuncTbl[bpc->event_status].event_pri < BeaconPcFuncTbl[event_status].event_pri
      && BeaconPcFuncTbl[bpc->next_event_status].event_pri < BeaconPcFuncTbl[event_status].event_pri){
    bpc->next_event_status = event_status;
    if(bpc->func_proc != BPC_SUBPROC_EXIT){
      bpc->func_proc = BPC_SUBPROC_EXIT;
      bpc->func_seq = 0;
    }
    return TRUE;
  }
  return FALSE;
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
static BOOL BeaconPCFunc(UNION_BEACON_PC *bpc, MMDL *mmdl)
{
  BOOL next_seq = TRUE;
  BPC_FUNC func;
  
  switch(bpc->func_proc){
  case BPC_SUBPROC_INIT:
    func = BeaconPcFuncTbl[bpc->event_status].func_init;
    break;
  case BPC_SUBPROC_UPDATE:
    func = BeaconPcFuncTbl[bpc->event_status].func_update;
    break;
  case BPC_SUBPROC_EXIT:
    func = BeaconPcFuncTbl[bpc->event_status].func_exit;
    break;
  }

  if(func != NULL){
    next_seq = func(bpc, mmdl, &bpc->func_seq);
  }
  
  if(next_seq == TRUE){
    bpc->func_seq = 0;
    bpc->func_proc++;
    if(bpc->func_proc > BPC_SUBPROC_EXIT){
      if(bpc->event_status == BPC_EVENT_STATUS_LEAVE){
        return FALSE; //�ޏo����
      }
      bpc->func_proc = 0;
      bpc->event_status = bpc->next_event_status;
      bpc->next_event_status = BPC_EVENT_STATUS_NULL;
    }
  }
  
  return TRUE;
}
