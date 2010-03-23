//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		palace_map_gimmick.c
 *	@brief  �p���X�}�b�v�@�M�~�b�N
 *	@author	tomoya takahashi
 *	@date		2010.03.20
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "fieldmap.h"
#include "palace_map_gimmick.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"
#include "p_tree.naix"
#include "palace_effect.naix"
#include "field/field_comm/intrude_field.h"
#include "field/field_comm/intrude_work.h"

#include "eventdata_local.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------


// �g���I�u�W�F�N�g�E���j�b�g�ԍ�
#define EXPOBJ_UNIT_IDX (0)
// �M�~�b�N���[�N�̃A�T�C��ID
#define GIMMICK_WORK_ASSIGN_ID (0)


//-------------------------------------
///	�M�~�b�N���[�N
//=====================================
// ���\�[�X�C���f�b�N�X
enum {
  MAP_RES_CUBE,           // 
  MAP_RES_CUBE_ANIME,     // 
  
  MAP_RES_NUM
} ;
static const GFL_G3D_UTIL_RES map_res_tbl[ MAP_RES_NUM ] = 
{
  { ARCID_PALACE_EFFECT, NARC_palace_effect_block_nsbmd, GFL_G3D_UTIL_RESARC },     // 
  { ARCID_PALACE_EFFECT, NARC_palace_effect_block_nsbma, GFL_G3D_UTIL_RESARC },     // 
};

// �A�j���[�V�����C���f�b�N�X
enum{
  MAP_ANM_CUBE_ANIME,

  MAP_ANM_CUBE_NUM,
} ;
static const GFL_G3D_UTIL_ANM map_res_cube_anm_tbl[ MAP_ANM_CUBE_NUM ] = 
{
  { MAP_RES_CUBE_ANIME, 0 },
};

// �I�u�W�F�N�g�C���f�b�N�X
enum {
  MAP_OBJ_CUBE,

  MAP_OBJ_NUM
} ;
static const GFL_G3D_UTIL_OBJ map_obj_table[ MAP_OBJ_NUM ] = 
{
  // ���f�����\�[�XID, 
  // ���f���f�[�^ID(���\�[�X����INDEX), 
  // �e�N�X�`�����\�[�XID,
  // �A�j���e�[�u��, 
  // �A�j�����\�[�X��
  
  //��
  { MAP_OBJ_CUBE, 0, MAP_RES_CUBE, map_res_cube_anm_tbl, MAP_ANM_CUBE_NUM },
};

static const GFL_G3D_UTIL_SETUP map_setup = { map_res_tbl, MAP_RES_NUM, map_obj_table, MAP_OBJ_NUM };





//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�M�~�b�N���[�N
//=====================================
typedef struct {
  BOOL on;

  GFL_G3D_OBJSTATUS* objstatus;
  
} PALACE_MAP_GMK_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static BOOL IsHit( PALACE_MAP_GMK_WORK* wk, const FIELD_PLAYER* player );




//----------------------------------------------------------------------------
/**
 *	@brief  �p���X�@�i����̊X�ł̃M�~�b�N���Z�b�g�A�b�v
 *
 *	@param	fieldWork   �t�B�[���h���[�N
 */
//-----------------------------------------------------------------------------
void PALACE_MAP_GMK_Setup(FIELDMAP_WORK *fieldWork)
{
  PALACE_MAP_GMK_WORK* wk;  //
  HEAPID                heapID = FIELDMAP_GetHeapID( fieldWork );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�ėp���[�N�m��
  wk = GMK_TMP_WK_AllocWork
      (fieldWork, GIMMICK_WORK_ASSIGN_ID, heapID, sizeof(PALACE_MAP_GMK_WORK));
  // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &map_setup, EXPOBJ_UNIT_IDX );

  // �\��OFF
  FLD_EXP_OBJ_SetVanish( exobj_cnt, EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, TRUE );

  // OBJSTATUS�擾
  wk->objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE );

  // �A�j��OFF
  {
    EXP_OBJ_ANM_CNT_PTR anime = FLD_EXP_OBJ_GetAnmCnt( exobj_cnt, 
        EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, MAP_ANM_CUBE_ANIME );
    FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, MAP_ANM_CUBE_ANIME, TRUE );
    FLD_EXP_OBJ_ChgAnmStopFlg( anime, TRUE );
    FLD_EXP_OBJ_ChgAnmLoopFlg( anime, FALSE );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �p���X�@�i����̊X�ł̃M�~�b�N���I��
 *
 *	@param	fieldWork   �t�B�[���h���[�N
 */
//-----------------------------------------------------------------------------
void PALACE_MAP_GMK_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //���j�b�g�j��
  FLD_EXP_OBJ_DelUnit( exobj_cnt, EXPOBJ_UNIT_IDX );  
  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, GIMMICK_WORK_ASSIGN_ID);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �p���X�@�i����̊X�ł̃M�~�b�N����
 *
 *	@param	fieldWork   �t�B�[���h���[�N
 */
//-----------------------------------------------------------------------------
void PALACE_MAP_GMK_Move(FIELDMAP_WORK *fieldWork)
{
  PALACE_MAP_GMK_WORK* wk;  //
  EXP_OBJ_ANM_CNT_PTR anime;
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldWork );
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldWork );
  VecFx32 pos;
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( fieldWork );
  GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
  EVENTDATA_SYSTEM* evsys = GAMEDATA_GetEventData( gdata );

  // ���[�N�擾
  wk = GMK_TMP_WK_GetWork( fieldWork, GIMMICK_WORK_ASSIGN_ID );

  // �A�j���[�V�����R���g���[���[�擾
  anime = FLD_EXP_OBJ_GetAnmCnt( exobj_cnt, 
      EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, MAP_ANM_CUBE_ANIME );
  
  // �\���L���[�uOFF
  if( wk->on ){

    FLD_EXP_OBJ_PlayAnime( exobj_cnt );
    
    if( FLD_EXP_OBJ_ChkAnmEnd(anime) ){
      // OFF
      FLD_EXP_OBJ_SetVanish( exobj_cnt, EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, TRUE );
      wk->on = FALSE;
      //TOMOYA_Printf( "cube off\n" );
    }
  }

  // �ʒu�`�F�b�N
  // ���@���q�b�g��Ԃ̏ꍇ�A
  // �ڂ̑O�Ƀ_�~�[�C�x���g����������A
  // �L���[�u��\������B
  if(  IsHit(wk, player) ){
    // �ʒu�`�F�b�N
    FIELD_PLAYER_GetDirPos( player, FIELD_PLAYER_GetDir(player), &pos );

    if( EVENTDATA_SYS_CheckPosDummyEvent( evsys, &pos ) ){
      wk->on        = TRUE;
      

      // �\��ON
      FLD_EXP_OBJ_SetVanish( exobj_cnt, EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, FALSE );
      FLD_EXP_OBJ_SetObjAnmFrm( exobj_cnt, 
        EXPOBJ_UNIT_IDX, MAP_OBJ_CUBE, MAP_ANM_CUBE_ANIME, 0 );
      FLD_EXP_OBJ_ChgAnmStopFlg( anime, FALSE );

      wk->objstatus->trans = pos;

      //TOMOYA_Printf( "cube on x[%d] y[%d] z[%d]\n", FX_Whole(pos.x), FX_Whole(pos.y), FX_Whole(pos.z) );
    }
  }
}




//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  �ǃq�b�g���̃A�N�V�����R�}���h���`�F�b�N
 *
 *	@param	acmd  �A�N�V�����R�}���h
 *
 *	@retval TRUE    �q�b�g��
 *	@retval FALSE   ���̂����쒆
 */
//-----------------------------------------------------------------------------
static BOOL IsHit( PALACE_MAP_GMK_WORK* wk, const FIELD_PLAYER* player )
{
  int i;
  u32 move_value = FIELD_PLAYER_GetMoveValue( player );
  u32 move_status = FIELD_PLAYER_GetMoveState( player );

  //TOMOYA_Printf( "move_value [%d] move_status [%d]\n", move_value, move_status );
  
  if( (move_status == PLAYER_MOVE_STATE_END) && (move_value == PLAYER_MOVE_VALUE_STOP) ){

    return TRUE;
  }
  return FALSE;
}
