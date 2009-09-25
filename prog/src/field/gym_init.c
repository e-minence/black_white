//======================================================================
/**
 * @file  gym_init.c
 * @bfief  �W��������
 * @author  Saito
 */
//======================================================================

#include "gym_init.h"

#include "savedata/gimmickwork.h"
#include "field_gimmick_def.h"
#include "fieldmap.h"

#include "gym_elec_sv.h"
#include "gym_normal_sv.h"

//--------------------------------------------------------------
/**
 * �d�C�W��������
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_INIT_Elec(FIELDMAP_WORK *fieldWork)
{
  GYM_ELEC_SV_WORK *gmk_sv_work;
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ELEC );
  }
  gmk_sv_work->NowRaleIdx[0] = 0;
  gmk_sv_work->NowRaleIdx[1] = 2;
  gmk_sv_work->NowRaleIdx[2] = 4;
  gmk_sv_work->NowRaleIdx[3] = 6;

  gmk_sv_work->StopPlatformIdx[0] = PLATFORM_NO_STOP;
  gmk_sv_work->StopPlatformIdx[1] = PLATFORM_NO_STOP;
  gmk_sv_work->StopPlatformIdx[2] = PLATFORM_NO_STOP;
  gmk_sv_work->StopPlatformIdx[3] = PLATFORM_NO_STOP;
}

//--------------------------------------------------------------
/**
 * �m�[�}���W��������
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_INIT_Normal(FIELDMAP_WORK *fieldWork, const u8 inRoomNo)
{
  u8 i;
  int gmk_id;
  GYM_NORMAL_SV_WORK *gmk_sv_work;

  if (inRoomNo == 0){
    gmk_id = FLD_GIMMICK_GYM_NORM1;
  }else{
    gmk_id = FLD_GIMMICK_GYM_NORM2;
  }

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, gmk_id );
  }

  //PLAYERWORK����A����ʒu���W���擾���A�M�~�b�N�ғ��ォ�ǂ����𔻒肷��

  if (0){
    gmk_sv_work->GmkUnrock = TRUE;
  }else{
    gmk_sv_work->GmkUnrock = FALSE;
  }

  for(i=0;i<WALL_NUM_MAX;i++){
    gmk_sv_work->Wall[i] = 0;
  }
}

