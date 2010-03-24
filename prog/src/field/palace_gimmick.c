//======================================================================
/**
 * @file  palace_gimmick.c
 * @brief  �p���X�M�~�b�N
 * @author  Saito
 */
//======================================================================
#include "fieldmap.h"
#include "palace_gimmick.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"
#include "p_tree.naix"
#include "palace_effect.naix"
#include "field/field_comm/intrude_field.h"
#include "field/field_comm/intrude_work.h"


//==============================================================================
//  �萔��`
//==============================================================================
// �g���I�u�W�F�N�g�E���j�b�g�ԍ�
#define EXPOBJ_UNIT_ELBOARD (1)

#define EXPOBJ_UNIT_IDX (0)
// �M�~�b�N���[�N�̃A�T�C��ID
#define GIMMICK_WORK_ASSIGN_ID (0)


//==========================================================================================
// ���M�~�b�N���[�N
//==========================================================================================
typedef struct
{
  u8 tree_palace_area;          ///<�؂�\�����Ă���p���X�G���A
  u8 barrier_effect;            ///<barrier�G�t�F�N�g
  u8 padding[2];
}PALACE_GMK_WORK;


//==========================================================================================
// ��3D���\�[�X
//==========================================================================================
// ���\�[�X�C���f�b�N�X
typedef enum {
  RES_TREE_ALL,           // �p���X�̖ؑS��
  RES_BARRIER_MDL,        // �o���A���o
  RES_BARRIER_ANM0,       // �o���A���o
  RES_BARRIER_ANM1,       // �o���A���o
  
  RES_NUM
} RES_INDEX;
static const GFL_G3D_UTIL_RES res_table[ RES_NUM ] = 
{
  { ARCID_PALACE_TREE, NARC_p_tree_p_tree_all_nsbmd, GFL_G3D_UTIL_RESARC },     // �p���X�̖ؑS��
  { ARCID_PALACE_EFFECT, NARC_palace_effect_p_barrier_nsbmd, GFL_G3D_UTIL_RESARC },     // �o���A���o
  { ARCID_PALACE_EFFECT, NARC_palace_effect_p_barrier_nsbca, GFL_G3D_UTIL_RESARC },     // �o���A���o
  { ARCID_PALACE_EFFECT, NARC_palace_effect_p_barrier_nsbta, GFL_G3D_UTIL_RESARC },     // �o���A���o
};

// �A�j���[�V�����I�u�W�F
typedef enum{
  ANM_BARRIER_ANM0,   // BARRIER�@00
  ANM_BARRIER_ANM1,   // BARRIER  01
  
  ANM_BARRIER_NUM
} ANM_BARRIER_INDEX;
static const GFL_G3D_UTIL_ANM anm_barrier_table[ ANM_BARRIER_NUM ] = 
{
  { RES_BARRIER_ANM0, 0 }, // BARRIER�@00
  { RES_BARRIER_ANM1, 0 }, // BARRIER�@01
};


// �I�u�W�F�N�g�C���f�b�N�X
typedef enum {
  OBJ_TREE_W_1,
  OBJ_TREE_W_2,
  OBJ_TREE_W_3,
  OBJ_TREE_W_4,
  OBJ_TREE_W_5,
  OBJ_TREE_W_6,
  OBJ_TREE_W_7,
  OBJ_TREE_W_8,
  OBJ_TREE_W_9,
  OBJ_TREE_W_10,

  OBJ_TREE_B_1,
  OBJ_TREE_B_2,
  OBJ_TREE_B_3,
  OBJ_TREE_B_4,
  OBJ_TREE_B_5,
  OBJ_TREE_B_6,
  OBJ_TREE_B_7,
  OBJ_TREE_B_8,
  OBJ_TREE_B_9,
  OBJ_TREE_B_10,


  OBJ_BARRIER,

  OBJ_NUM
} OBJ_INDEX;
static const GFL_G3D_UTIL_OBJ obj_table[ OBJ_NUM ] = 
{
  // ���f�����\�[�XID, 
  // ���f���f�[�^ID(���\�[�X����INDEX), 
  // �e�N�X�`�����\�[�XID,
  // �A�j���e�[�u��, 
  // �A�j�����\�[�X��
  
  //��
  { RES_TREE_ALL, 0, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 1, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 2, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 3, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 4, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 5, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 6, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 7, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 8, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 9, RES_TREE_ALL, NULL, 0 },
  //��
  { RES_TREE_ALL, 0+10, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 1+10, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 2+10, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 3+10, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 4+10, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 5+10, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 6+10, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 7+10, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 8+10, RES_TREE_ALL, NULL, 0 },
  { RES_TREE_ALL, 9+10, RES_TREE_ALL, NULL, 0 },

  // BARRIER
  { RES_BARRIER_MDL, 0, RES_BARRIER_MDL, anm_barrier_table, ANM_BARRIER_NUM },
};

static const GFL_G3D_UTIL_SETUP setup = { res_table, RES_NUM, obj_table, OBJ_NUM };


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void InitWork( PALACE_GMK_WORK* work, FIELDMAP_WORK* fieldWork, GAMEDATA *gdata, INTRUDE_COMM_SYS_PTR intcomm );
static void _AllVanish(FLD_EXP_OBJ_CNT_PTR exobj_cnt);
static int _GetTreeLevel(int wb_level);
static u8 _TreeEnable(GAMEDATA *gamedata, FLD_EXP_OBJ_CNT_PTR exobj_cnt, INTRUDE_COMM_SYS_PTR intcomm);
static INTRUDE_COMM_SYS_PTR _GetIntrudeComm(GAME_COMM_SYS_PTR game_comm);


// barrier���o
static void Barrier_Init( PALACE_GMK_WORK* work, FLD_EXP_OBJ_CNT_PTR exobj_cnt );
static void Barrier_Start( PALACE_GMK_WORK* work, FLD_EXP_OBJ_CNT_PTR exobj_cnt, const VecFx32* cp_pos );
static void Barrier_Main( PALACE_GMK_WORK* work, FLD_EXP_OBJ_CNT_PTR exobj_cnt );


//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void PALACE_GMK_Setup(FIELDMAP_WORK *fieldWork)
{
  PALACE_GMK_WORK* work;  // �M�~�b�N�Ǘ����[�N
  HEAPID                heapID = FIELDMAP_GetHeapID( fieldWork );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GAMESYS_WORK*            gsys = FIELDMAP_GetGameSysWork( fieldWork );
  GAMEDATA*               gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*          gmkwork = GAMEDATA_GetGimmickWork(gdata);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  INTRUDE_COMM_SYS_PTR intcomm = _GetIntrudeComm(game_comm);

  //�ėp���[�N�m��
  work = GMK_TMP_WK_AllocWork
      (fieldWork, GIMMICK_WORK_ASSIGN_ID, heapID, sizeof(PALACE_GMK_WORK));
  // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &setup, EXPOBJ_UNIT_IDX );

  // �M�~�b�N�Ǘ����[�N�������� 
  InitWork( work, fieldWork, gdata, intcomm );

  // �o���A���o������
  Barrier_Init( work, exobj_cnt );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void PALACE_GMK_End(FIELDMAP_WORK *fieldWork)
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldWork );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldWork );

/**
  u32*         gmk_save = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H01 );
  PALACE_GMK_WORK*         work = (PALACE_GMK_WORK*)gmk_save[0]; // gmk_save[0]�̓M�~�b�N�Ǘ����[�N�̃A�h���X
  // �Z�[�u
  SaveGimmick( work, fieldWork );

*/
  //���j�b�g�j��
  FLD_EXP_OBJ_DelUnit( exobj_cnt, EXPOBJ_UNIT_IDX );  
  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, GIMMICK_WORK_ASSIGN_ID);
}


//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void PALACE_GMK_Move(FIELDMAP_WORK *fieldWork)
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldWork );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldWork );
  PALACE_GMK_WORK* work = GMK_TMP_WK_GetWork(fieldWork, GIMMICK_WORK_ASSIGN_ID);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  INTRUDE_COMM_SYS_PTR intcomm = _GetIntrudeComm(game_comm);
  
  if(intcomm != NULL && intcomm->intrude_status_mine.palace_area != work->tree_palace_area){
    _AllVanish(exobj_cnt);
    work->tree_palace_area = _TreeEnable(gdata, exobj_cnt, intcomm);
  }


  // �A�j���[�V�����Ǘ�
  FLD_EXP_OBJ_PlayAnime( exobj_cnt );

  // �o���A�G�t�F�N�g���C��
  if( work->barrier_effect ){

    // �o���A�G�t�F�N�g���C��
    Barrier_Main( work, exobj_cnt );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  barrier�G�t�F�N�g���J�n
 *
 *	@param	fieldWork     ���[�N
 */
//-----------------------------------------------------------------------------
void PALACE_GMK_StartBarrierEffect( FIELDMAP_WORK *fieldWork, const VecFx32* cp_pos )
{
  PALACE_GMK_WORK* work = GMK_TMP_WK_GetWork((FIELDMAP_WORK*)fieldWork, GIMMICK_WORK_ASSIGN_ID);
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�@�J�n
  Barrier_Start( work, exobj_cnt, cp_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  barrier�G�t�F�N�g�@���s�`�F�b�N
 *
 *	@param	fieldWork   FIELDMAP���[�N
 *
 *	@retval TRUE    ���s��
 *	@retval FALSE   �I��
 */
//-----------------------------------------------------------------------------
BOOL PALACE_GMK_IsBarrierEffect( const FIELDMAP_WORK * fieldWork )
{
  const PALACE_GMK_WORK* work = GMK_TMP_WK_GetWork((FIELDMAP_WORK*)fieldWork, GIMMICK_WORK_ASSIGN_ID);
  return work->barrier_effect;
}



//==============================================================================
//  
//==============================================================================
//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�Ǘ����[�N������������
 *
 * @param work     �������Ώۃ��[�N
 * @param fieldWork �ˑ�����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
static void InitWork( PALACE_GMK_WORK* work, FIELDMAP_WORK* fieldWork, GAMEDATA *gdata, INTRUDE_COMM_SYS_PTR intcomm )
{
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GFL_G3D_OBJSTATUS* status;
  int i;
  
  // �I�u�W�F�N�g���쐬
  for(i = 0; i < OBJ_NUM; i++){
    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, i );
    status->trans.x = PALACE_MAP_TREE_X;
    status->trans.y = PALACE_MAP_TREE_Y;
    status->trans.z = PALACE_MAP_TREE_Z;
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(exobj_cnt, EXPOBJ_UNIT_IDX, i, TRUE);
    //��\��
    FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, i, TRUE);
  }

  work->tree_palace_area = _TreeEnable(gdata, exobj_cnt, intcomm);
}

//--------------------------------------------------------------
/**
 * �S�Ă̖؂�\��OFF����
 *
 * @param   exobj_cnt		
 */
//--------------------------------------------------------------
static void _AllVanish(FLD_EXP_OBJ_CNT_PTR exobj_cnt)
{
  int i;

  for(i = 0; i < OBJ_NUM; i++){
    FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, i, TRUE);
  }
}

//--------------------------------------------------------------
/**
 * �������x������؂̃��x�����擾����
 *
 * @param   wb_level		�� or �� �̃��x��
 *
 * @retval  int		�؂̃��x��
 */
//--------------------------------------------------------------
static int _GetTreeLevel(int wb_level)
{
  int tree_level;
  
  tree_level = wb_level % 3;
  if(tree_level > 9){
    tree_level = 9;
  }
  return tree_level;
}

//--------------------------------------------------------------
/**
 * ���݂̃p���X�G���A�̖؂����x���ɍ��킹�����̂ɂ��A�\��ON����
 *
 * @param   exobj_cnt		
 * @param   intcomm		
 *
 * @retval  ���݂̃p���X�G���A
 */
//--------------------------------------------------------------
static u8 _TreeEnable(GAMEDATA *gamedata, FLD_EXP_OBJ_CNT_PTR exobj_cnt, INTRUDE_COMM_SYS_PTR intcomm)
{
  GFL_G3D_OBJSTATUS* status;
  OBJ_INDEX obj_id;
  int pm_version;
  int tree_w_lv, tree_b_lv;
  const OCCUPY_INFO *occupy;
  int palace_area, map_offset;
  
  if(intcomm == NULL){
    palace_area = 0;
    map_offset = 0;
  }
  else{
    palace_area = intcomm->intrude_status_mine.palace_area;
    //�q�@�̏ꍇ�Apalace_area == 0 �����[�A�ł͂Ȃ�������NetID�̃p���X�����[�ׁ̈B
    map_offset = palace_area - GAMEDATA_GetIntrudeMyID(gamedata);
    if(map_offset < 0){
      map_offset = intcomm->member_num + map_offset;
    }
  }
  
  if(intcomm == NULL || palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
    pm_version = PM_VERSION;
    occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
  }
  else{
    pm_version = MyStatus_GetRomCode( Intrude_GetMyStatus(intcomm, palace_area) );
    occupy = GAMEDATA_GetOccupyInfo(gamedata, palace_area);
  }
  tree_w_lv = _GetTreeLevel( OccupyInfo_GetWhiteLevel(occupy) );
  tree_b_lv = _GetTreeLevel( OccupyInfo_GetBlackLevel(occupy) );
  
  //��
  status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TREE_W_1 + tree_w_lv );
  status->trans.x = PALACE_MAP_LEN * map_offset + PALACE_MAP_TREE_X;
  status->trans.y = PALACE_MAP_TREE_Y;
  status->trans.z = PALACE_MAP_TREE_Z;
  //��
  status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TREE_B_1 + tree_b_lv );
  status->trans.x = PALACE_MAP_LEN * map_offset + PALACE_MAP_TREE_X;
  status->trans.y = PALACE_MAP_TREE_Y;
  status->trans.z = PALACE_MAP_TREE_Z;
  //�\��
  FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TREE_W_1 + tree_w_lv, FALSE);
  FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TREE_B_1 + tree_b_lv, FALSE);
  
  return palace_area;
}

//--------------------------------------------------------------
/**
 * intcomm���擾
 *
 * @param   game_comm		
 *
 * @retval  INTRUDE_COMM_SYS_PTR		
 */
//--------------------------------------------------------------
static INTRUDE_COMM_SYS_PTR _GetIntrudeComm(GAME_COMM_SYS_PTR game_comm)
{
  if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_INVASION){
    return GameCommSys_GetAppWork(game_comm);
  }
  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  barrier�G�t�F�N�g�̏�����
 *	
 *	@param	work        ���[�N
 *	@param	exobj_cnt   �RD�I�u�W�F�N�g�Ǘ��V�X�e��
 */
//-----------------------------------------------------------------------------
static void Barrier_Init( PALACE_GMK_WORK* work, FLD_EXP_OBJ_CNT_PTR exobj_cnt )
{
  // �\��OFF
  FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_BARRIER, TRUE);
  
  // �A�j�����f
  {
    int i;
    EXP_OBJ_ANM_CNT_PTR anm_cnt;

    for( i=0; i<ANM_BARRIER_NUM; i++ ){
      FLD_EXP_OBJ_ValidCntAnm(
          exobj_cnt,
          EXPOBJ_UNIT_IDX,
          OBJ_BARRIER,
          i,
          TRUE);

      anm_cnt = FLD_EXP_OBJ_GetAnmCnt(
          exobj_cnt,
          EXPOBJ_UNIT_IDX,
          OBJ_BARRIER,
          i );

      // ���[�v�A�j��OFF
      FLD_EXP_OBJ_ChgAnmLoopFlg( anm_cnt, FALSE );
      FLD_EXP_OBJ_ChgAnmStopFlg( anm_cnt, FALSE );
    }
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  barrier���o�J�n
 *
 *	@param	work    ���[�N
 *	@param  cp_pos  �|�W�V����
 */
//-----------------------------------------------------------------------------
static void Barrier_Start( PALACE_GMK_WORK* work, FLD_EXP_OBJ_CNT_PTR exobj_cnt, const VecFx32* cp_pos )
{
  // �\��ON
  FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_BARRIER, FALSE);
  
  // �A�j���[�V�������Z�b�g
  {
    int i;

    for( i=0; i<ANM_BARRIER_NUM; i++ ){
      
      FLD_EXP_OBJ_SetObjAnmFrm(
          exobj_cnt,
          EXPOBJ_UNIT_IDX,
          OBJ_BARRIER,
          i,
          0);

    }
  }

  // �ʒu��ݒ�
  {
    GFL_G3D_OBJSTATUS* status;

    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_BARRIER );

    status->trans = *cp_pos;
  }


  work->barrier_effect = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  barrier���o���C��
 *
 *	@param	work  ���[�N
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void Barrier_Main( PALACE_GMK_WORK* work, FLD_EXP_OBJ_CNT_PTR exobj_cnt )
{
  
  
  if( work->barrier_effect == FALSE ){
    return;
  }

  
  {
    int i;
    EXP_OBJ_ANM_CNT_PTR anm_cnt; 

    anm_cnt = FLD_EXP_OBJ_GetAnmCnt(
        exobj_cnt,
        EXPOBJ_UNIT_IDX,
        OBJ_BARRIER, 0);


    // �I���`�F�b�N
    if( FLD_EXP_OBJ_ChkAnmEnd(anm_cnt) ){
      // �\��OFF
      FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_BARRIER, TRUE);
      work->barrier_effect = FALSE;
    }
  }
}



