//======================================================================
/**
 * @file  gym_init.c
 * @brief  �W��������
 * @author  Saito
 */
//======================================================================

#include "gym_init.h"
#include "savedata/gimmickwork.h"
#include "field_gimmick_def.h"
#include "gym_elec_sv.h"
#include "gym_normal_sv.h"
#include "gym_anti_sv.h"
#include "gym_insect_sv.h"
#include "gym_dragon_sv.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "../../../resource/fldmapdata/gimmick/gym_normal/gym_normal_exit_def.h"

//--------------------------------------------------------------
/**
 * �d�C�W��������
 * @param   gsys            �Q�[���V�X�e���|�C���^
 * @param   inEvtFlgBtl1    �C�x���g�t���O�󋵁@�g���[�i�[��1���I�������H�@1�ŏI��
 * @param   inEvtFlgBtl2    �C�x���g�t���O�󋵁@�g���[�i�[��2���I�������H�@1�ŏI��
 *
 * @return  none
 */
//--------------------------------------------------------------
void GYM_INIT_Elec(GAMESYS_WORK *gsys, const u16 inEvtFlgBtl1, const u16 inEvtFlgBtl2)
{
  GYM_ELEC_SV_WORK *gmk_sv_work;
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ELEC );
  }

  //�g���[�i�[��i���󋵂��Z�b�g
  if (inEvtFlgBtl1){
    gmk_sv_work->EvtFlg[0] = 1;
  }
  if (inEvtFlgBtl2){
    gmk_sv_work->EvtFlg[1] = 1;
  }
}

//--------------------------------------------------------------
/**
 * �m�[�}���W��������
 * @param	    gsys    �Q�[���V�X�e���|�C���^
 * @return    none
 */
//--------------------------------------------------------------
void GYM_INIT_Normal(GAMESYS_WORK *gsys)
{
  u8 i;
  GYM_NORMAL_SV_WORK *gmk_sv_work;
  PLAYER_WORK *player_work;

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_NORM );
    player_work = GAMEDATA_GetMyPlayerWork(gamedata);
  }

  //PLAYER_WORK����A����ʒu���W���擾���A�M�~�b�N�ғ��ォ�ǂ����𔻒肷��
  {
    int gx,gz;
    gx = player_work->position.x / FIELD_CONST_GRID_FX32_SIZE;
    gz = player_work->position.z / FIELD_CONST_GRID_FX32_SIZE;

    OS_Printf("NormalGymInit:: pos gx gz  = %d,%d\n",gx,gz);
  
    if ((gx == HIDEN_EXIT_GX)&&(gz == HIDEN_EXIT_GZ)){
      gmk_sv_work->GmkUnrock = TRUE;
    }else{
      gmk_sv_work->GmkUnrock = FALSE;
    }
  }

  for(i=0;i<NRM_WALL_NUM_MAX;i++){
    if (gmk_sv_work->GmkUnrock){
      gmk_sv_work->Wall[i] = 1;
    }else{
      gmk_sv_work->Wall[i] = 0;
    }
  }
}

//--------------------------------------------------------------
/**
 * �A���`�W��������
 * @param	    gsys    �Q�[���V�X�e���|�C���^
 * @return    none
 */
//--------------------------------------------------------------
void GYM_INIT_Anti(GAMESYS_WORK *gsys)
{
  u8 i;
  GYM_ANTI_SV_WORK *gmk_sv_work;
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ANTI );
  }

  for (i=0;i<ANTI_DOOR_NUM_MAX;i++){
    gmk_sv_work->Door[i] = 0;
  }
  gmk_sv_work->PushSwIdx = ANTI_SW_NUM_MAX;
}

//--------------------------------------------------------------
/**
 * ��s�W��������
 * @param	    gsys    �Q�[���V�X�e���|�C���^
 * @return    none
 */
//--------------------------------------------------------------
void GYM_INIT_Fly(GAMESYS_WORK *gsys)
{
  ;
}

//--------------------------------------------------------------
/**
 * ���W��������
 * @param	    gsys    �Q�[���V�X�e���|�C���^
 * @return    none
 */
//--------------------------------------------------------------
void GYM_INIT_Insect(GAMESYS_WORK *gsys)
{
  GYM_INSECT_SV_WORK *gmk_sv_work;
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_INSECT );
  }

  //�C���f�b�N�X6�Ԃ̕ǂ͎n�߂���󂹂�
  gmk_sv_work->WallSt[6] = WALL_ST_WEAKNESS;

}

//--------------------------------------------------------------
/**
 * �h���S���W��������
 * @param	    gsys    �Q�[���V�X�e���|�C���^
 * @return    none
 */
//--------------------------------------------------------------
void GYM_INIT_Dragon(GAMESYS_WORK *gsys)
{
  DRAGON_WORK *wk;
  GYM_DRAGON_SV_WORK *gmk_sv_work;
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
  }

  wk = &gmk_sv_work->DraWk[0];
  wk->HeadDir = HEAD_DIR_RIGHT;
  wk->ArmDir[0] = ARM_DIR_UP;   //��
  wk->ArmDir[1] = ARM_DIR_DOWN;   //�E
  wk = &gmk_sv_work->DraWk[1];
  wk->HeadDir = HEAD_DIR_DOWN;
  wk->ArmDir[0] = ARM_DIR_DOWN;   //��
  wk->ArmDir[1] = ARM_DIR_DOWN;   //�E
  wk = &gmk_sv_work->DraWk[2];
  wk->HeadDir = HEAD_DIR_LEFT;
  wk->ArmDir[0] = ARM_DIR_DOWN;     //��
  wk->ArmDir[1] = ARM_DIR_UP;     //�E
}


