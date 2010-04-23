//==============================================================================
/**
 * @file    comm_player.c
 * @brief   �ʐM�v���C���[����
 * @author  matsuda
 * @date    2009.07.17(��)
 */
//==============================================================================
#include <gflib.h>
#include "field/field_comm_actor.h"
#include "net_app/union/comm_player.h"
#include "field/fieldmap.h"
#include "field/fldeff_gyoe.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�ʐM�v���C���[���[�N�ő�Ǘ���
#define COMM_PLAYER_MAX     (4)   //�R���V�A�����ő�4�l�ׁ̈Bnet_id���������ł��Ή������邽�ߎ������������Ă͂����Ȃ�

//==============================================================================
//  �\���̒�`
//==============================================================================
///�ʐM�v���C���[���[�N
typedef struct{
  //�t�B�[���h�A�N�^�[�����ɎQ�Ƃ��ꑱ���郏�[�N�@�����ׁ̈A�r�b�g�t�B�[���h�֎~
  VecFx32 pos;
  BOOL vanish;
  u16 dir;
  u16 obj_code;
  
  //comm_player.c�ł̂ݎg�p�������
  u8 occ:1;         ///<���̍\���̑S�̂̃f�[�^�L���E����(TRUE:�L��)
  u8 push_flag:1;
  u8 :6;
  u8 padding[3];
  
  FLDEFF_TASK *fldeff_gyoe_task;  ///<�u�I�v�G�t�F�N�g����^�X�N
}COMM_PLAYER;

///�������g�̍��W�f�[�^�Ȃ�
typedef struct{
  VecFx32 pos;
  u16 dir;
  u8 padding[2];
}MINE_PLAYER;

///�ʐM�v���C���[����V�X�e�����[�N
typedef struct _COMM_PLAYER_SYS{
  GAMESYS_WORK *gsys;
  FIELD_COMM_ACTOR_CTRL *act_ctrl;
  COMM_PLAYER act[COMM_PLAYER_MAX];
  MINE_PLAYER mine;     ///<�������g�̍��W�f�[�^�Ȃ�
  HEAPID heap_id;
  u8 max;
  u8 update_stop;       ///<TRUE:���W�X�V���s��Ȃ�
  u8 padding[2];
}COMM_PLAYER_SYS;


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �ʐM�v���C���[����V�X�e��������
 *
 * @param   max		
 * @param   fmmdlsys		
 * @param   heap_id		
 *
 * @retval  COMM_PLAYER_SYS_PTR		
 */
//==================================================================
COMM_PLAYER_SYS_PTR CommPlayer_Init(int max, GAMESYS_WORK *gsys, HEAPID heap_id)
{
  COMM_PLAYER_SYS_PTR cps;

  OS_TPrintf("�ʐM�v���C���[����V�X�e���̐���\n");
  
  cps = GFL_HEAP_AllocClearMemory(heap_id, sizeof(COMM_PLAYER_SYS));
  
  GF_ASSERT(max <= COMM_PLAYER_MAX);
  
  cps->max = max;
  cps->gsys = gsys;
  cps->heap_id = heap_id;

  if(GAMESYSTEM_CheckFieldMapWork(gsys) == TRUE){
    //�t�B�[���h�����݂��Ȃ��ꍇ�Aact_ctrl�͂����ł͍���Ȃ���
    //CommPlayer_Pop��game_comm�̃t�B�[���h�쐬���̃R�[���o�b�N�ŌĂ�ł�����
    //�t�B�[���h�쐬�Ɠ����ɉ��߂�act_ctrl�������̂Ŗ��Ȃ�
    FIELDMAP_WORK *fieldWork;
    MMDLSYS *fldMdlSys;

    fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
    if(FIELDMAP_IsReady(fieldWork) == TRUE){
      fldMdlSys = FIELDMAP_GetMMdlSys(fieldWork);
      cps->act_ctrl = FIELD_COMM_ACTOR_CTRL_Create(max, fldMdlSys, heap_id);
    }
  }
  
  return cps;
}

//==================================================================
/**
 * �ʐM�v���C���[����V�X�e���j��
 *
 * @param   cps		
 */
//==================================================================
void CommPlayer_Exit(COMM_PLAYER_SYS_PTR cps)
{
  int i;
  
  OS_TPrintf("�ʐM�v���C���[����V�X�e���̔j��\n");

  if(cps->act_ctrl != NULL){  //NULL�̏ꍇ��PUSH��Ԃׁ̈A���ɍ폜����Ă���
    for(i = 0; i < COMM_PLAYER_MAX; i++){
      CommPlayer_Del(cps, i);
    }
    FIELD_COMM_ACTOR_CTRL_Delete(cps->act_ctrl);
  }
  
  GFL_HEAP_FreeMemory(cps);
}

//==================================================================
/**
 * �ʐM�v���C���[����V�X�e���X�V����
 *
 * @param   cps		
 */
//==================================================================
void CommPlayer_Update(COMM_PLAYER_SYS_PTR cps)
{
  int index;
  
  //�t�B�[���h�G�t�F�N�g�����s���Ă�����̂�����΁A�I����Ԃ��Ď����A���[�N�̏����������s��
  if(GAMESYSTEM_CheckFieldMapWork(cps->gsys) == TRUE){
    for(index = 0; index < COMM_PLAYER_MAX; index++){
      if(cps->act[index].occ == TRUE && cps->act[index].fldeff_gyoe_task != NULL){
        if(FLDEFF_GYOE_CheckEnd(cps->act[index].fldeff_gyoe_task) == TRUE){
          cps->act[index].fldeff_gyoe_task = NULL;
        }
      }
    }
  }
}

//==================================================================
/**
 * �ʐM�v���C���[�A�N�^�[�o�^
 *
 * @param   cps		
 * @param   index		�Ǘ�Index(��{�I��net_id��n���΂悢�B
 *                  �������Q���ɑΉ����Ă���̂�max��2�ł�0,3�Ƃ������w�肪�\)
 * @param   sex		  ����
 * @param   pack    ���W�f�[�^�p�b�P�[�W
 */
//==================================================================
void CommPlayer_Add(COMM_PLAYER_SYS_PTR cps, int index, u16 obj_code, const COMM_PLAYER_PACKAGE *pack)
{
  OS_TPrintf("�ʐM�v���C���[Add index=%d\n", index);

#ifdef PM_DEBUG
  {//�ő�l���𒴂��Ēǉ����悤�Ƃ��Ă��Ȃ����`�F�b�N
    int i, count;
    count = 0;
    for(i = 0; i < COMM_PLAYER_MAX; i++){
      if(cps->act[i].occ == TRUE){
        count++;
      }
    }
    if(count > cps->max){
      GF_ASSERT(0);   //�ő�l���𒴂��Ēǉ����悤�Ƃ���
      return;
    }
  }
#endif

  GF_ASSERT(cps->act[index].occ == FALSE);
  if(cps->act[index].occ == TRUE){
    return; //�ꉞ
  }

  cps->act[index].pos = pack->pos;
  cps->act[index].dir = pack->dir;
  cps->act[index].vanish = pack->vanish;
  cps->act[index].obj_code = obj_code;
  cps->act[index].push_flag = FALSE;
  cps->act[index].occ = TRUE;
  cps->act[index].fldeff_gyoe_task = NULL;
  
  FIELD_COMM_ACTOR_CTRL_AddActor(cps->act_ctrl, index, obj_code, &cps->act[index].dir, 
    &cps->act[index].pos, &cps->act[index].vanish);
}

//==================================================================
/**
 * �ʐM�v���C���[�A�N�^�[�폜
 *
 * @param   cps		
 * @param   index		�Ǘ�Index
 */
//==================================================================
void CommPlayer_Del(COMM_PLAYER_SYS_PTR cps, int index)
{
  if(cps->act[index].occ == FALSE){
    return;
  }
  
  if(cps->act_ctrl == NULL){
    GF_ASSERT(0);
    return;
  }
  
  OS_TPrintf("�ʐM�v���C���[Del index=%d\n", index);
  cps->act[index].fldeff_gyoe_task = NULL;
  FIELD_COMM_ACTOR_CTRL_DeleteActro(cps->act_ctrl, index);
  cps->act[index].occ = FALSE;
}

//==================================================================
/**
 * �ʐM�v���C���[Push����
 *
 * @param   cps		
 */
//==================================================================
void CommPlayer_Push(COMM_PLAYER_SYS_PTR cps)
{
  int i;
  
  GF_ASSERT(GAMESYSTEM_CheckFieldMapWork(cps->gsys) == TRUE);
  
  if(cps->act_ctrl == NULL){
    return;
  }
  
  for(i = 0; i < COMM_PLAYER_MAX; i++){
    if(cps->act[i].occ == TRUE){
      CommPlayer_Del(cps, i);
      cps->act[i].push_flag = TRUE;
      OS_TPrintf("CommPlayer Push! %d\n", i);
    }
  }
  FIELD_COMM_ACTOR_CTRL_Delete(cps->act_ctrl);
  cps->act_ctrl = NULL;
}

//==================================================================
/**
 * �ʐM�v���C���[Pop����
 *
 * @param   cps		
 */
//==================================================================
void CommPlayer_Pop(COMM_PLAYER_SYS_PTR cps)
{
  int i;
  COMM_PLAYER_PACKAGE pack;
  FIELDMAP_WORK *fieldWork;
  MMDLSYS *fldMdlSys;

  GF_ASSERT(GAMESYSTEM_CheckFieldMapWork(cps->gsys) == TRUE);
  GF_ASSERT(cps->act_ctrl == NULL);

  fieldWork = GAMESYSTEM_GetFieldMapWork(cps->gsys);
  fldMdlSys = FIELDMAP_GetMMdlSys(fieldWork);
  cps->act_ctrl = FIELD_COMM_ACTOR_CTRL_Create(cps->max, fldMdlSys, cps->heap_id);
  
  GFL_STD_MemClear(&pack, sizeof(COMM_PLAYER_PACKAGE));
  for(i = 0; i < COMM_PLAYER_MAX; i++){
    if(cps->act[i].push_flag == TRUE){
      pack.pos = cps->act[i].pos;
      pack.dir = cps->act[i].dir;
      pack.vanish = cps->act[i].vanish;
      CommPlayer_Add(cps, i, cps->act[i].obj_code, &pack);
      OS_TPrintf("CommPlayer Pop! %d\n", i);
    }
  }
}

//==================================================================
/**
 * ���W�X�V�̃|�[�Y�t���O���Z�b�g����
 *
 * @param   cps		
 * @param   stop_flag		TRUE:���W�X�V���s��Ȃ�
 */
//==================================================================
void CommPlayer_FlagSet_UpdateStop(COMM_PLAYER_SYS_PTR cps, BOOL stop_flag)
{
  cps->update_stop = stop_flag;
}

//==================================================================
/**
 * �ʐM�v���C���[���݃`�F�b�N
 *
 * @param   cps		
 * @param   index		�Ǘ�Index
 *
 * @retval  BOOL		TRUE:���݂��Ă���B�@FALSE:�񑶍�
 */
//==================================================================
BOOL CommPlayer_CheckOcc(COMM_PLAYER_SYS_PTR cps, int index)
{
  GF_ASSERT(cps != NULL && index < COMM_PLAYER_MAX);
  return cps->act[index].occ;
}

//==================================================================
/**
 * �ʐM�v���C���[��OBJ�R�[�h���擾
 *
 * @param   cps		
 * @param   index		
 *
 * @retval  u16		OBJ�R�[�h
 */
//==================================================================
u16 CommPlayer_GetObjCode(COMM_PLAYER_SYS_PTR cps, int index)
{
  GF_ASSERT(cps != NULL && index < COMM_PLAYER_MAX && cps->act[index].occ == TRUE);
  return cps->act[index].obj_code;
}

//==================================================================
/**
 * �ʐM�v���C���[�A�N�^�[���W���Z�b�g����
 *
 * @param   cps		  
 * @param   index		�Ǘ�Index
 * @param   pack		���W�p�b�P�[�W
 */
//==================================================================
void CommPlayer_SetParam(COMM_PLAYER_SYS_PTR cps, int index, const COMM_PLAYER_PACKAGE *pack)
{
  GF_ASSERT(cps->act[index].occ == TRUE);

  cps->act[index].pos = pack->pos;
  cps->act[index].dir = pack->dir;
  cps->act[index].vanish = pack->vanish;
}

//==================================================================
/**
 * �������W�X�V����
 *
 * @param   cps		  
 * @param   pack		�������W�f�[�^�����(�ʐM�ō��W������肷��ꍇ�A���̃f�[�^�𑗂��Ă�������)
 *
 * @retval  BOOL		TRUE:�X�V����B�@FALSE:�ȑO�ƒl���ς���Ă��Ȃ�
 *
 * pack�ɂ͍��W�̍X�V�̗L���Ɋւ�炸��ɍ��̒l���������܂�
 * ����̃f�[�^���M�Ƃ������A�f�[�^�X�V�Ɍ��炸���肽���ꍇ�ł������Ƃ����l������܂�
 */
//==================================================================
BOOL CommPlayer_Mine_DataUpdate(COMM_PLAYER_SYS_PTR cps, COMM_PLAYER_PACKAGE *pack)
{
  MINE_PLAYER *mine = &cps->mine;
  FIELDMAP_WORK *fieldWork;
  FIELD_PLAYER * player;
  VecFx32 pos;
  u16 dir;
  
  if(cps->update_stop == TRUE || GAMESYSTEM_CheckFieldMapWork(cps->gsys) == FALSE){
    return FALSE;
  }

  fieldWork = GAMESYSTEM_GetFieldMapWork(cps->gsys);
  player = FIELDMAP_GetFieldPlayer(fieldWork);

  FIELD_PLAYER_GetPos(player, &pos);
  dir = FIELD_PLAYER_GetDir(player);
  
  pack->dir = dir;
  pack->pos = pos;
  pack->vanish = FALSE;

  if(dir != mine->dir || GFL_STD_MemComp(&pos, &mine->pos, sizeof(VecFx32)) != 0){
    mine->dir = dir;
    mine->pos = pos;
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �w����W(�O���b�h)�ʒu�ɂ���ʐM�A�N�^�[��Index��Ԃ�
 *
 * @param   cps		      
 * @param   gx		      ���ׂ�O���b�h���WX
 * @param   gz		      ���ׂ�O���b�h���WZ
 * @param   out_index		Index�i�[��
 *
 * @retval  BOOL		TRUE:�ʐM�A�N�^�[���w����W�ɂ���
 */
//==================================================================
BOOL CommPlayer_SearchGridPos(COMM_PLAYER_SYS_PTR cps, s16 gx, s16 gz, u32 *out_index)
{
  GF_ASSERT(cps->act_ctrl != NULL);
  return FIELD_COMM_ACTOR_CTRL_SearchGridPos(cps->act_ctrl, gx, gz, out_index);
}

//==================================================================
/**
 * �u�I�v�G�t�F�N�g�^�X�N���Z�b�g
 *
 * @param   cps		
 * @param   index		�Ǘ�Index
 *
 * @retval  BOOL		TRUE:�Z�b�g�����B�@FALSE:�Z�b�g���s(���ɋN�����Ȃǂ��܂�)
 */
//==================================================================
BOOL CommPlayer_SetGyoeTask(COMM_PLAYER_SYS_PTR cps, int index)
{
  COMM_PLAYER *act = &cps->act[index];
  MMDL *mmdl;
  FIELDMAP_WORK *fieldWork;
  FLDEFF_CTRL *fectrl;
  
  if(cps->act_ctrl == NULL || act->occ == FALSE || act->fldeff_gyoe_task != NULL){
    return FALSE;
  }
  
  fieldWork = GAMESYSTEM_GetFieldMapWork(cps->gsys);
  fectrl = FIELDMAP_GetFldEffCtrl(fieldWork);
  mmdl = FIELD_COMM_ACTOR_CTRL_GetMMdl(cps->act_ctrl, index);
  
  act->fldeff_gyoe_task = FLDEFF_GYOE_SetMMdlNonDepend(fectrl, mmdl, FLDEFF_GYOETYPE_GYOE, FALSE);
  return TRUE;
}

//==================================================================
/**
 * �Ώۃv���C���[��MMDL�|�C���^���擾
 *
 * @param   cps		
 * @param   index		�Ǘ�Index
 *
 * @retval  MMDL *		
 */
//==================================================================
MMDL * CommPlayer_GetMmdl(COMM_PLAYER_SYS_PTR cps, int index)
{
  GF_ASSERT(cps->act_ctrl != NULL);
  GF_ASSERT(cps->act[index].occ == TRUE);
  return FIELD_COMM_ACTOR_CTRL_GetMMdl(cps->act_ctrl, index);
}
