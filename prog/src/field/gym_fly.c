//======================================================================
/**
 * @file  gym_fly.c
 * @brief  ��s�W��
 * @author  Saito
 */
//======================================================================
#include "fieldmap.h"
#include "gym_fly_sv.h"
#include "gym_fly.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

//#include "arc/fieldmap/gym_fly.naix"

#include "system/main.h"    //for HEAPID_FIELDMAP
#include "script.h"     //for SCRIPT_CallScript
//#include "../../../resource/fldmapdata/script/c04gym0101_def.h"  //for SCRID_�`



#define GYM_FLY_UNIT_IDX (0)
#define GYM_FLY_TMP_ASSIGN_ID  (1)

#define FRAME_POS_SIZE  (4*3*360)   //���W�w�x�y�e4�o�C�g��360�t���[��
#define HEADER_SIZE  (8)   //�t���[���T�C�Y4�o�C�g+�i�[���3�o�C�g�̃w�b�_�[��񂪂��邽�߃A���C�����g����������8�o�C�g�m�ۂ���

//�W���������̈ꎞ���[�N
typedef struct GYM_FLY_TMP_tag
{
  u8 ShotIdx;
  u8 EvtDir;
  u8 dummy[2];

  u8 FramePosDat[FRAME_POS_SIZE+HEADER_SIZE];

}GYM_FLY_TMP;

static GMEVENT_RESULT ShotEvt( GMEVENT* event, int* seq, void* work );

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_FLY_Setup(FIELDMAP_WORK *fieldWork)
{
  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldWork, GYM_FLY_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(GYM_FLY_TMP));
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_FLY_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, GYM_FLY_TMP_ASSIGN_ID);
  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, GYM_FLY_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_FLY_Move(FIELDMAP_WORK *fieldWork)
{
  int i;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );

}

//--------------------------------------------------------------
/**
 * �ł��o���C�x���g�N��
 * @param	
 * @return
 */
//--------------------------------------------------------------
GMEVENT *GYM_FLY_Shot(GAMESYS_WORK *gsys, const u8 inShotIdx)
{
  GMEVENT * event;
  GYM_FLY_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_FLY_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_FLY_TMP_ASSIGN_ID);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );

  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_FLY );

  //�C�x���g�R�[��
  event = GMEVENT_Create( gsys, NULL, ShotEvt, 0 );
    
  //�ł��o���C���f�b�N�X���Z�b�g
  tmp->ShotIdx = inShotIdx;

}

//--------------------------------------------------------------
/**
 * �X�C�b�`�C�x���g
 * @param	
 * @return
 */
//--------------------------------------------------------------
static GMEVENT_RESULT ShotEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_FLY_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_FLY_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_FLY_TMP_ASSIGN_ID);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_FLY );
  }

  switch(*seq){
  case 0:
    //�ړ��f�[�^���[�h
    ;
    //���@�̌����ɂ�蕪��
    //��C�h�A�I�[�v���t���[���܂ōĐ��J�n
    ;
    (*seq)++;
    break;
  case 1:
    //�I�[�v���t���[�����B�Ď�
    if (1){
      //���@�A��C�ɓ���
      (*seq)++;
    }
    break;
  case 2:
    //���@�A�j���I���҂�
    if(1){
      //��C�A�j���ۊJ�n
      (*seq)++;
    }
    break;
  case 3:
    //���˃t���[�����B�`�F�b�N
    if (1){
      //�t���[���ǂݎ��J�n
      ;
      (*seq)++;
    }
    break;
  case 4:
    //��C�A�j���I���`�F�b�N���t���[���ǂݎ��I���`�F�b�N
    if (1){
      //�t���[���ǂݎ��I��
      ;
      //��l���ŏI�ʒu�Z�b�g
      ;
      (*seq)++;
    }
    break;
  case 5:
    //�t���[���ǂݎ����
    ;
    return GMEVENT_RES_FINISH;
  }

  //�t���[���ǂݎ�菈������
  if (0){      //�ǂݎ��t���O�������Ă���΁A���s
    ;
  }

  return GMEVENT_RES_CONTINUE;
}


