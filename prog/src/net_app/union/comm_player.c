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
  VecFx32 pos;
  BOOL vanish;
  u16 dir;
  u8 occ;         ///<���̍\���̑S�̂̃f�[�^�L���E����(TRUE:�L��)
  u8 padding;
}COMM_PLAYER;

///�������g�̍��W�f�[�^�Ȃ�
typedef struct{
  VecFx32 pos;
  u16 dir;
  u8 padding[2];
}MINE_PLAYER;

///�ʐM�v���C���[����V�X�e�����[�N
typedef struct _COMM_PLAYER_SYS{
  FIELD_MAIN_WORK *fieldWork;
  FIELD_COMM_ACTOR_CTRL *act_ctrl;
  COMM_PLAYER act[COMM_PLAYER_MAX];
  MINE_PLAYER mine;     ///<�������g�̍��W�f�[�^�Ȃ�
  u8 max;
  u8 padding[3];
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
COMM_PLAYER_SYS_PTR CommPlayer_Init(int max, FIELD_MAIN_WORK *fieldWork, HEAPID heap_id)
{
  COMM_PLAYER_SYS_PTR cps;
  MMDLSYS *fldMdlSys = FIELDMAP_GetMMdlSys(fieldWork);

  OS_TPrintf("�ʐM�v���C���[����V�X�e���̐���\n");
  
  cps = GFL_HEAP_AllocClearMemory(heap_id, sizeof(COMM_PLAYER_SYS));
  
  GF_ASSERT(max <= COMM_PLAYER_MAX);
  
  cps->max = max;
  cps->fieldWork = fieldWork;
  cps->act_ctrl = FIELD_COMM_ACTOR_CTRL_Create(max, fldMdlSys, heap_id);
  
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

  for(i = 0; i < COMM_PLAYER_MAX; i++){
    CommPlayer_Del(cps, i);
  }
  FIELD_COMM_ACTOR_CTRL_Delete(cps->act_ctrl);
  
  GFL_HEAP_FreeMemory(cps);
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
void CommPlayer_Add(COMM_PLAYER_SYS_PTR cps, int index, int sex, const COMM_PLAYER_PACKAGE *pack)
{
  int objcode;

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

  objcode = (sex == PM_MALE) ? HERO : HEROINE;
  cps->act[index].pos = pack->pos;
  cps->act[index].dir = pack->dir;
  cps->act[index].vanish = pack->vanish;
  cps->act[index].occ = TRUE;
  
  FIELD_COMM_ACTOR_CTRL_AddActor(cps->act_ctrl, index, objcode, &cps->act[index].dir, 
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
  
  OS_TPrintf("�ʐM�v���C���[Del index=%d\n", index);
  FIELD_COMM_ACTOR_CTRL_DeleteActro(cps->act_ctrl, index);
  cps->act[index].occ = FALSE;
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
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(cps->fieldWork);
  VecFx32 pos;
  u16 dir;
  
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

