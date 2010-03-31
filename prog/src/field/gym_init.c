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

#define INSECT_CHK_MAX  (2)

//--���W���M�~�b�N�f�[�^--
//�X�C�b�`�ɑΉ�����|�[���C���f�b�N�X
static const s8 InsSw[INSECT_SW_MAX][INSECT_CHK_MAX] = {
  {0,-1},     //�X�C�b�`0�F0�ԃ|�[��
  {1,-1},     //�X�C�b�`1�F1�ԃ|�[��
  {2,-1},     //�X�C�b�`2�F2�ԃ|�[��
  {3,4},      //�X�C�b�`3�F3,4�ԃ|�[��
  {5,-1},     //�X�C�b�`4�F5�ԃ|�[��
  {6,-1},     //�X�C�b�`5�F6�ԃ|�[��
  {7,8},      //�X�C�b�`6�F7,8�ԃ|�[��
  {9,-1},     //�X�C�b�`7�F9�ԃ|�[��
};

//�ǂɑΉ�����|�[���C���f�b�N�X
static const u8 InsWall[INSECT_WALL_MAX-1][INSECT_CHK_MAX] = {
  {0,1},     //��0�F0,1�ԃ|�[��
  {2,3},     //��1�F2,3�ԃ|�[��
  {4,5},     //��2�F4,5�ԃ|�[��
  {6,6},      //��3�F6�ԃ|�[��
  {7,7},     //��4�F7�ԃ|�[��
  {8,9},     //��5�F8,9�ԃ|�[��
  //����6�̓|�[���`�F�b�N����
};

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
 * @param     �M�~�b�N�����ォ�ǂ����@TRUE�F������
 * @return    none
 */
//--------------------------------------------------------------
void GYM_INIT_Normal(GAMESYS_WORK *gsys, const BOOL inOpened)
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

  if (inOpened) gmk_sv_work->GmkUnrock = TRUE;
  else gmk_sv_work->GmkUnrock = FALSE;

  //PLAYER_WORK����A����ʒu���W���擾���A�M�~�b�N�ғ��ォ�ǂ����𔻒肷�� @todo
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
 * @param     inCurtain   �J�[�e����
 * @return    none
 */
//--------------------------------------------------------------
void GYM_INIT_Anti(GAMESYS_WORK *gsys, const int inCurtain)
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

  if ( inCurtain & 0x01 ) gmk_sv_work->Door[0] = 1;
  if ( (inCurtain>>1) & 0x01 ) gmk_sv_work->Door[1] = 1;
  if ( (inCurtain>>2) & 0x01 ) gmk_sv_work->Door[2] = 1;
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
 * @param     sw_work �i�[���[�N�|�C���^
 * @return    none
 */
//--------------------------------------------------------------
void GYM_INIT_Insect(GAMESYS_WORK *gsys, u16 **sw_work)
{
  int i,j;
  GYM_INSECT_SV_WORK *gmk_sv_work;
  u16 *ex_wk;
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_INSECT );
    ex_wk = GIMMICKWORK_GetExWork( gmkwork, FLD_GIMMICK_GYM_INSECT );
  }

  //�M�~�b�N���[�N�g�������݂āA���[�N������������
  for (i=0;i<INSECT_SW_MAX;i++)
  {
    if ( ex_wk[i] )
    {
      //�X�C�b�`���������΂ɂ���
      gmk_sv_work->Sw[i] = TRUE;
      *sw_work[i] = 1;
      //�X�C�b�`�ɑΉ�����|�[���̏�Ԃ�ω�������
      for(j=0;j<INSECT_CHK_MAX;j++)
      {
        s8 idx = InsSw[i][j];
        if ( idx != -1) gmk_sv_work->Pl[idx] = TRUE;
      }
    }
  }
  //�ǂ̏�Ԃ��|�[����Ԃ���Z�b�g
  for (i=0;i<INSECT_WALL_MAX-1;i++)
  {
    for (j=0;j<INSECT_CHK_MAX;j++)
    {
      u8 idx = InsWall[i][j];
      if ( !gmk_sv_work->Pl[idx] ) break;
    }
    if (j==INSECT_CHK_MAX) gmk_sv_work->WallSt[i] = WALL_ST_WEAKNESS;
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


