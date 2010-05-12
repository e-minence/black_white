//======================================================================
/**
 * @file  fldmmdl_draw_0.c
 * @brief  �t�B�[���h���샂�f�� �`�揈�����̂O�@�r���{�[�h�֘A
 * @author  kagaya
 * @date  05.07.13
 */
//======================================================================
#include "fldmmdl.h"
#include "fldmmdl_procdraw.h"

#include "fieldmap.h" //FIELDMAP_GetBbdActYOffs
//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
/// �r���{�[�h���ʃI�t�Z�b�g�\�����W
//--------------------------------------------------------------
//#define MMDL_BBD_OFFS_POS_Y (-FX32_ONE*2)  //(FX32_ONE*4)
#define MMDL_BBD_OFFS_POS_Z (FX32_ONE*4)  //(-FX32_ONE*8)

//--------------------------------------------------------------
/// �A������|�P�����\���I�t�Z�b�g
//--------------------------------------------------------------
#define MMDL_POKE_OFS_UPDOWN		(FX32_CONST(1))
#define MMDL_POKE_OFS_RIGHTLEFT	(FX32_CONST(10))
#define MMDL_POKE_OFS_RIGHTLEFT_S	(FX32_CONST(2))

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// COMMAN_ANMCTRL_WORK
//--------------------------------------------------------------
typedef struct
{
  u8 set_anm_dir; //�Z�b�g���Ă������
  u8 set_anm_status; //�Z�b�g�����A�j��
  u8 next_walk_frmidx; //�ړ��n�Ŏ��ɏo���A�j���C���f�b�N�X
  u8 next_set_wait; //���̃X�e�[�^�X���Z�b�g����܂ł̃E�F�C�g
}COMMAN_ANMCTRL_WORK;

//--------------------------------------------------------------
///  DRAW_BLACT_WORK
//--------------------------------------------------------------
typedef struct
{
  u16 actID;
  u8 padding0;
  u8 padding1;
  COMMAN_ANMCTRL_WORK anmcnt;
}DRAW_BLACT_WORK;

//--------------------------------------------------------------
///  DRAW_BLACT_POKE_WORK
//--------------------------------------------------------------
typedef struct
{
  u16 actID;
  u8 set_anm_dir;
  u8 offs_frame;
  u8 dmy;
  fx32 offs_y;
}DRAW_BLACT_POKE_WORK;

//======================================================================
//  proto
//======================================================================
static void comManAnmCtrl_Init( COMMAN_ANMCTRL_WORK *work );
static void comManAnmCtrl_Update( COMMAN_ANMCTRL_WORK *work,
    MMDL *mmdl, GFL_BBDACT_SYS *actSys, u16 actID );

static void blact_SetCommonOffsPos( MMDL *mmdl, VecFx32 *pos );
static u16 blact_GetDrawDir( MMDL *mmdl );
static void blact_UpdatePauseVanish(
    MMDL *mmdl, GFL_BBDACT_SYS *actSys, u16 actID,
    BOOL start_anm, BOOL anm_pause_only );

static void TsurePoke_SetAnmAndOffset( MMDL* mmdl, DRAW_BLACT_POKE_WORK* work, u8 dir );
static void TsurePoke_GetDrawOffsetFromDir( MMDL* mmdl, u8 dir, const OBJCODE_PARAM* obj_prm, VecFx32* outVec);
static BOOL TsurePoke_CheckUpDown( DRAW_BLACT_POKE_WORK* work, u8 dir, const OBJCODE_PARAM* obj_prm );

//======================================================================
//  �`�揈���@�`�斳��
//======================================================================
//--------------------------------------------------------------
/**
 * �`�斳���@������
 * @param  mmdl    MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_DrawNon_Init( MMDL * mmdl )
{
  MMDL_SetStatusBitVanish( mmdl, TRUE );
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_SHADOW_VANISH );
}

//--------------------------------------------------------------
/**
 * �`�斳���@�`��
 * @param  mmdl    MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_DrawNon_Draw( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`�斳���@�폜
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_DrawNon_Delete( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`�斳���@�ޔ�
 * @param  mmdl    MMDL * 
 * @retval  int      TRUE=����������
 */
//--------------------------------------------------------------
static void MMdl_DrawNon_Push( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`�斳���@���A
 * �ޔ������������ɍĕ`��B
 * @param  mmdl    MMDL * 
 * @retval  int      TRUE=����������
 */
//--------------------------------------------------------------
static void MMdl_DrawNon_Pop( MMDL * mmdl )
{
}

//--------------------------------------------------------------
///  �`�揈���@�`��Ȃ��@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_Non =
{
  MMdl_DrawNon_Init,
  MMdl_DrawNon_Draw,
  MMdl_DrawNon_Delete,
  MMdl_DrawNon_Push,
  MMdl_DrawNon_Pop,
  NULL,
};



//======================================================================
//  DRAW_BLACT_WORK ����
//======================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  DRAW_BLACT_WORK �A�j���[�V�����t���[���̎擾
 *
 *	@param	mmdl        ���f�����[�N
 *	@parma  outIdx      �A�j���[�V�����C���f�b�N�X
 *	@parma  outFrmIdx   �A�j���[�V�����t���[���C���f�b�N�X
 *
 */
//-----------------------------------------------------------------------------
void MMDL_DrawBlactWork_GetAnimeFrame(
    MMDL *mmdl, u16* outIdx, u16* outFrmIdx )
{
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );
  
  if( prm->draw_type == MMDL_DRAWTYPE_BLACT ){
    work = MMDL_GetDrawProcWork( mmdl );
    
    if( work->actID != MMDL_BLACTID_NULL ){
      actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
      *outIdx     = GFL_BBDACT_GetAnimeIdx( actSys,work->actID );
      *outFrmIdx  = GFL_BBDACT_GetAnimeFrmIdx( actSys, work->actID );
    }else{
      *outIdx     = 0;
      *outFrmIdx  = 0;
    }
  }else{
    GF_ASSERT( prm->draw_type == MMDL_DRAWTYPE_BLACT );
    *outIdx     = 0;
    *outFrmIdx  = 0;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  DRAW_BLACT_WORK �A�j���[�V�����t���[���̐ݒ�
 *
 *	@param	mmdl      ���f�����[�N
 *	@param  Idx       �A�j���[�V�����C���f�b�N�X
 *	@param	FrmIdx    �t���[���C���f�b�N�X
 */
//-----------------------------------------------------------------------------
extern void MMDL_DrawBlactWork_SetAnimeFrame( MMDL *mmdl, u16 Idx, u16 FrmIdx )
{
  DRAW_BLACT_WORK *work;
  const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );
  GFL_BBDACT_SYS *actSys;

  if(prm->draw_type == MMDL_DRAWTYPE_BLACT){
    work = MMDL_GetDrawProcWork( mmdl );
    if( work->actID != MMDL_BLACTID_NULL ){
      actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
      GFL_BBDACT_SetAnimeIdx( actSys, work->actID, Idx );
      GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, FrmIdx );
    }
  }
}



//======================================================================
//  �`�揈���@�r���{�[�h�@���@��p
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���@��p�@������
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawHero_Init( MMDL *mmdl )
{
  DRAW_BLACT_WORK *work;
  
  work = MMDL_InitDrawProcWork( mmdl, sizeof(DRAW_BLACT_WORK) );
  comManAnmCtrl_Init( &work->anmcnt );
  MMDL_BLACTCONT_AddActor( mmdl, &work->actID );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���@��p�@�폜
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawHero_Delete( MMDL *mmdl )
{
  DRAW_BLACT_WORK *work;
  work = MMDL_GetDrawProcWork( mmdl );
  MMDL_BLACTCONT_DeleteActor( mmdl, work->actID );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���@��p�@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawHero_Draw( MMDL *mmdl )
{
  VecFx32 pos;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  comManAnmCtrl_Update( &work->anmcnt, mmdl, actSys, work->actID );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
      GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���@��p�@�擾�B
 * �r���{�[�h�A�N�^�[ID��Ԃ��B
 * @param  mmdl  MMDL
 * @param  state  ���ɖ���
 * @retval  u32  GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static u32 DrawHero_GetBlActID( MMDL *mmdl, u32 state )
{
  DRAW_BLACT_WORK *work;
  work = MMDL_GetDrawProcWork( mmdl );
  return( work->actID );
}

//--------------------------------------------------------------
//  �`�揈���@�r���{�[�h�@���@�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_Hero =
{
  DrawHero_Init,
  DrawHero_Draw,
  DrawHero_Delete,
  DrawHero_Delete,  //�ޔ�
  DrawHero_Init,    //�{���͕��A
  DrawHero_GetBlActID,
};

//======================================================================
//  �`�揈���@�r���{�[�h�@���]�Ԏ��@��p
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���]�Ԏ��@��p�@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
#if 0
static void DrawCycleHero_Draw( MMDL *mmdl )
{
  VecFx32 pos;
  BOOL chg_dir = FALSE;
  u16 dir,anm_id,status;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  
  dir = blact_GetDrawDir( mmdl );
  status = MMDL_GetDrawStatus( mmdl );
  GF_ASSERT( status < DRAW_STA_HERO_MAX );

#if 0  //����
  if( status == DRAW_STA_WALK_4F ){ //090731 ���x��4F����8F�Ō��������Ɨv�]
    status = DRAW_STA_WALK_8F;
  }
#endif

  anm_id = status * DIR_MAX4;
  anm_id += dir;
  
  if( work->set_anm_dir != dir ){ //�����X�V
    work->set_anm_dir = dir;
    work->set_anm_status = status;
    GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
    chg_dir = TRUE;
  }else if( work->set_anm_status != status ){
    u16 frame = 0;
    
    switch( work->set_anm_status ){
    case DRAW_STA_WALK_32F:
    case DRAW_STA_WALK_16F:
    case DRAW_STA_WALK_8F:
    case DRAW_STA_WALK_4F:
    case DRAW_STA_WALK_2F:
    case DRAW_STA_HERO_DASH_4F:
      if( GFL_BBDACT_GetAnimeFrmIdx(actSys,work->actID) < 2 ){
        frame = 2;
      }
      break;
    }
    
    GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
    GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, frame );
    work->set_anm_status = status;
  }
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
  {
    BOOL flag = TRUE;
    if( chg_dir == FALSE && MMDL_CheckDrawPause(mmdl) == TRUE ){
      flag = FALSE;
    }
    GFL_BBDACT_SetAnimeEnable( actSys, work->actID, flag );
    
    flag = TRUE; 
    
    if( MMDL_CheckStatusBitVanish(mmdl) == TRUE ){
      flag = FALSE;
    }
    
    GFL_BBDACT_SetDrawEnable( actSys, work->actID, flag );
  }
}
#endif

//--------------------------------------------------------------
/// �`�揈���@���]�Ԏ��@�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_CycleHero =
{
  DrawHero_Init,
  DrawHero_Draw,
  DrawHero_Delete,
  DrawHero_Delete,  //�ޔ�
  DrawHero_Init,    //�{���͕��A
  DrawHero_GetBlActID,
};

//======================================================================
//  �`�揈���@�r���{�[�h�@�g��莩�@��p
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�g��莩�@��p�@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawSwimHero_Draw( MMDL *mmdl )
{
  VecFx32 pos;
  BOOL init_flag = FALSE;
  u16 dir,anm_id,status;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  
  dir = blact_GetDrawDir( mmdl );
  status = MMDL_GetDrawStatus( mmdl );
  GF_ASSERT( dir < DIR_MAX4 );
  anm_id = dir;
  
  if( work->anmcnt.set_anm_dir != dir ){ //�����X�V
    work->anmcnt.set_anm_dir = dir;
    work->anmcnt.set_anm_status = status;
    GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
    init_flag = TRUE;
  }else if( work->anmcnt.set_anm_status != status ){ //�X�e�[�^�X�X�V
    u16 frame = 0;
    GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
    GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, frame );
    work->anmcnt.set_anm_status = status;
  }
  
  blact_UpdatePauseVanish( mmdl, actSys, work->actID, init_flag, FALSE );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/// �`�揈���@�g��莩�@�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_SwimHero =
{
  DrawHero_Init,
  DrawSwimHero_Draw,
  DrawHero_Delete,
  DrawHero_Delete,  //�ޔ�
  DrawHero_Init,    //�{���͕��A
  DrawHero_GetBlActID,
};

//======================================================================
//  �`�揈���@�r���{�[�h�@�ėp
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ėp�@������
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_Init( MMDL *mmdl )
{
  DRAW_BLACT_WORK *work;
  
  work = MMDL_InitDrawProcWork( mmdl, sizeof(DRAW_BLACT_WORK) );
  
  comManAnmCtrl_Init( &work->anmcnt );
  MMDL_BLACTCONT_AddActor( mmdl, &work->actID );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ėp�@�폜
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_Delete( MMDL *mmdl )
{
  DRAW_BLACT_WORK *work;
  work = MMDL_GetDrawProcWork( mmdl );
  MMDL_BLACTCONT_DeleteActor( mmdl, work->actID );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ėp�@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_Draw( MMDL *mmdl )
{
  VecFx32 pos;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  comManAnmCtrl_Update( &work->anmcnt, mmdl, actSys, work->actID );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ėp�{��ɃA�j���@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_DrawAlwaysAnime( MMDL *mmdl )
{
  VecFx32 pos;
  BOOL init_flag = FALSE;
  BOOL anm_pause_only = FALSE;
  u16 dir,anm_id,code;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  dir = blact_GetDrawDir( mmdl );
  code = MMDL_GetOBJCode( mmdl );
  
  if( code == FLAG ){
    anm_pause_only = TRUE;
  }
  
  {
    const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );
    
    if( prm->draw_proc_no == MMDL_DRAWPROCNO_BLACTALWAYSANIME_32 ){
      anm_id = DRAW_STA_WALK_32F * DIR_MAX4;
    }else if( prm->draw_proc_no == MMDL_DRAWPROCNO_BLACTALWAYSANIME_16 ){
      anm_id = DRAW_STA_WALK_16F * DIR_MAX4;
    }else{ //4frame
      anm_id = DRAW_STA_WALK_4F * DIR_MAX4;
    }
  }
  
  anm_id += dir;
  
  if( work->anmcnt.set_anm_dir != dir ){ //�����X�V
    work->anmcnt.set_anm_dir = dir;
    GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
    init_flag = TRUE;
  }
  
  blact_UpdatePauseVanish(
      mmdl, actSys, work->actID, init_flag, anm_pause_only );

  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@��{�p�^�[��+���Z��p
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_DrawAct( MMDL *mmdl )
{
  VecFx32 pos;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  
  {
    u16 status = MMDL_GetDrawStatus( mmdl );
    
    if( status >= DRAW_STA_ACT0 ){
      u16 init_flag = FALSE;
      u16 dir = blact_GetDrawDir( mmdl );
      COMMAN_ANMCTRL_WORK *anmcnt = &work->anmcnt;
      
      if( status != anmcnt->set_anm_status ){
        u16 anm_idx = DRAW_STA_DIR4_MAX + (status - DRAW_STA_ACT0);
        
        init_flag = TRUE;
        anmcnt->set_anm_dir = dir;
        anmcnt->set_anm_status = status;
        anmcnt->next_walk_frmidx = 0;
        D_MMDL_DPrintf( "MMDL ���Z�A�j������0 IDX = %d\n", anm_idx );
        GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_idx );
      }
      
      blact_UpdatePauseVanish(
          mmdl, actSys, work->actID, init_flag, FALSE );
    }else{
      comManAnmCtrl_Update( &work->anmcnt, mmdl, actSys, work->actID );
    }
  }
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@��{�p�^�[��+���Z��p�A���Z���̓|�[�Y����
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_DrawActNonePause( MMDL *mmdl )
{
  VecFx32 pos;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  
  {
    u16 status = MMDL_GetDrawStatus( mmdl );
    
    if( status >= DRAW_STA_ACT0 ){
      u16 init_flag = FALSE;
      u16 dir = blact_GetDrawDir( mmdl );
      COMMAN_ANMCTRL_WORK *anmcnt = &work->anmcnt;
      
      if( status != anmcnt->set_anm_status ){
        u16 anm_idx = DRAW_STA_DIR4_MAX + (status - DRAW_STA_ACT0);
        
        anmcnt->set_anm_dir = dir;
        anmcnt->set_anm_status = status;
        anmcnt->next_walk_frmidx = 0;
        D_MMDL_DPrintf( "MMDL ���Z�A�j�����̂P IDX = %d\n", anm_idx );
        GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_idx );
      }
      
      init_flag = TRUE; //���Z���̓|�[�Y����
      blact_UpdatePauseVanish(
          mmdl, actSys, work->actID, init_flag, FALSE );
    }else{
      comManAnmCtrl_Update( &work->anmcnt, mmdl, actSys, work->actID );
    }
  }
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ėp�@�擾�B
 * �r���{�[�h�A�N�^�[ID��Ԃ��B
 * @param  mmdl  MMDL
 * @param  state  ���ɖ���
 * @retval  u32  GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static u32 DrawBlAct_GetBlActID( MMDL *mmdl, u32 state )
{
  DRAW_BLACT_WORK *work;
  work = MMDL_GetDrawProcWork( mmdl );
  return( work->actID );
}

//--------------------------------------------------------------
//  �`�揈���@�r���{�[�h�@�ėp�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_BlAct =
{
  DrawBlAct_Init,
  DrawBlAct_Draw,
  DrawBlAct_Delete,
  DrawBlAct_Delete,  //�{���͑ޔ�
  DrawBlAct_Init,    //�{���͕��A
  DrawBlAct_GetBlActID,
};

//--------------------------------------------------------------
//  �`�揈���@�r���{�[�h�@�ėp+��ɃA�j���@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_BlActAlwaysAnime =
{
  DrawBlAct_Init,
  DrawBlAct_DrawAlwaysAnime,
  DrawBlAct_Delete,
  DrawBlAct_Delete,  //�{���͑ޔ�
  DrawBlAct_Init,    //�{���͕��A
  DrawBlAct_GetBlActID,
};

//--------------------------------------------------------------
//  �`�揈���@�r���{�[�h�@�ėp+���Z�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_BlActAct =
{
  DrawBlAct_Init,
  DrawBlAct_DrawAct,
  DrawBlAct_Delete,
  DrawBlAct_Delete,  //�{���͑ޔ�
  DrawBlAct_Init,    //�{���͕��A
  DrawBlAct_GetBlActID,
};

//--------------------------------------------------------------
//  �`�揈���@�r���{�[�h�@�ėp+���Z�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_BlActActNonePause =
{
  DrawBlAct_Init,
  DrawBlAct_DrawActNonePause,
  DrawBlAct_Delete,
  DrawBlAct_Delete,  //�{���͑ޔ�
  DrawBlAct_Init,    //�{���͕��A
  DrawBlAct_GetBlActID,
};

//======================================================================
//  �`�揈���@�r���{�[�h ���@�A�C�e���Q�b�g
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���@�A�C�e���Q�b�g�@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawItemGetHero_Draw( MMDL *mmdl )
{
  VecFx32 pos;
  u16 status;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  status = MMDL_GetDrawStatus( mmdl );
  
  //�G���[�t�H���[
  if( work->anmcnt.set_anm_status >= DRAW_STA_ITEMGET_MAX ){ 
    work->anmcnt.set_anm_status = DRAW_STA_ITEMGET_STOP;
    GFL_BBDACT_SetAnimeIdx( actSys,
        work->actID, work->anmcnt.set_anm_status );
    GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, 0 );
  }
 
  if( status < DRAW_STA_ITEMGET_MAX ){
    if( work->anmcnt.set_anm_status != status ){
      work->anmcnt.set_anm_status = status;
      GFL_BBDACT_SetAnimeIdx( actSys,
          work->actID, work->anmcnt.set_anm_status );
      GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, 0 );
    }
  }

  blact_UpdatePauseVanish( mmdl, actSys, work->actID, FALSE, FALSE );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/// �`�揈���@���@�A�C�e���Q�b�g�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_ItemGetHero =
{
  DrawHero_Init,
  DrawItemGetHero_Draw,
  DrawHero_Delete,
  DrawHero_Delete,  //�ޔ�
  DrawHero_Init,    //�{���͕��A
  DrawHero_GetBlActID,
};

//======================================================================
//  ���@PC�a��
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���@PC�a���@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawPCAzukeHero_Draw( MMDL *mmdl )
{
  VecFx32 pos;
  u16 status;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  status = MMDL_GetDrawStatus( mmdl );
  
  //�G���[�t�H���[
  if( work->anmcnt.set_anm_status >= DRAW_STA_PCAZUKE_MAX ){
    work->anmcnt.set_anm_status = DRAW_STA_ITEMGET_STOP;
    GFL_BBDACT_SetAnimeIdx( actSys,
        work->actID, work->anmcnt.set_anm_status );
    GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, 0 );
  }
 
  if( status < DRAW_STA_PCAZUKE_MAX ){
    if( work->anmcnt.set_anm_status != status ){
      work->anmcnt.set_anm_status = status;
      GFL_BBDACT_SetAnimeIdx( actSys,
          work->actID, work->anmcnt.set_anm_status );
      GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, 0 );
    }
  }
  
  blact_UpdatePauseVanish( mmdl, actSys, work->actID, FALSE, FALSE );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/// �`�揈���@���@�A�C�e���Q�b�g�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_PCAzukeHero =
{
  DrawHero_Init,
  DrawPCAzukeHero_Draw,
  DrawHero_Delete,
  DrawHero_Delete,  //�ޔ�
  DrawHero_Init,    //�{���͕��A
  DrawHero_GetBlActID,
};

//======================================================================
//  �ނ莩�@
//======================================================================
//--------------------------------------------------------------
/**
 * �ނ�A�j���@�\���ʂɃI�t�Z�b�g�Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void drawFishingHero_SetOffset(
    MMDL *mmdl, u16 anm_idx, u16 frm_idx, VecFx32 *offs )
{
  u16 code,cell;
  MMDLSYS *mmdlsys;
  const MMDL_BBDACT_ANMTBL *anmTbl;
  const GFL_BBDACT_ANM *anm;
  
  offs->x = 0;
  offs->y = 0;
  offs->z = 0;
#if 0
  code = MMDL_GetOBJCode( mmdl );
  mmdlsys = MMDL_GetMMdlSys( mmdl );
  anmTbl = MMDL_BLACTCONT_GetObjAnimeTable( mmdlsys, code );
  
  GF_ASSERT( anm_idx < anmTbl->anm_max );
  
  anm = anmTbl->pAnmTbl[anm_idx];
  cell = anm[frm_idx].anmData.celIdx;
  
  if( cell == 6 || cell == 7 ){
    offs->z = NUM_FX32( 8 );
  }else if( cell == 10 || cell == 11 ){
    offs->x = NUM_FX32( -5 );

    if( anm[frm_idx].anmData.flipS ){
      offs->x = -offs->x;
    }
  }
#endif
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ނ莩�@�@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawFishingHero_Draw( MMDL *mmdl )
{
  u16 dir,status,anm_id;
  VecFx32 pos;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;

  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  
  dir = blact_GetDrawDir( mmdl );
  status = MMDL_GetDrawStatus( mmdl );
  anm_id = (status * DIR_MAX4) + dir;
  
  if( dir != work->anmcnt.set_anm_dir ||
    work->anmcnt.set_anm_status != status ){
    work->anmcnt.set_anm_dir = dir;
    work->anmcnt.set_anm_status = status;
    GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
    GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, 0 );
  }
  
  blact_UpdatePauseVanish( mmdl, actSys, work->actID, FALSE, FALSE );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );

  {
    VecFx32 offs;
    drawFishingHero_SetOffset( mmdl,
        GFL_BBDACT_GetAnimeIdx(actSys,work->actID),
        GFL_BBDACT_GetAnimeFrmIdx(actSys,work->actID), &offs );
    pos.x += offs.x;
    pos.y += offs.y;
    pos.z += offs.z;
  }
  
  { //64x64�␳
    pos.y += NUM_FX32( -12 );
    pos.z += NUM_FX32( 8 );
  }

  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/// �`�揈���@�ނ莩�@�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_FishingHero =
{
  DrawHero_Init,
  DrawFishingHero_Draw,
  DrawHero_Delete,
  DrawHero_Delete,  //�ޔ�
  DrawHero_Init,    //�{���͕��A
  DrawHero_GetBlActID,
};

//======================================================================
//  ��ꎩ�@
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@��ꎩ�@�@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawYureHero_Draw( MMDL *mmdl )
{
  u16 dir;
  VecFx32 pos;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  dir = blact_GetDrawDir( mmdl );
  
  if( work->anmcnt.set_anm_dir != dir ){
    u16 anm_id = dir;
    GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
    GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, 0 );
    work->anmcnt.set_anm_dir = dir;
  }
  
  blact_UpdatePauseVanish( mmdl, actSys, work->actID, FALSE, FALSE );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/**
 * ��ꎩ�@�@�A�j���t���[���Z�b�g
 * @param
 * @retval
 */
//--------------------------------------------------------------
BOOL MMDL_DrawYureHero_SetAnimeFrame( MMDL *mmdl, u32 frame )
{
  u16 idx = 0;
  DRAW_BLACT_WORK *work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return( FALSE );
  }
  
  if( frame < 24 ){
    if( frame < 6 ){
      idx = 0;
    }else if( frame < 12 ){
      idx = 1;
    }else if( frame < 18 ){
      idx = 2;
    }else{
      idx = 3;
    }
  }else if( frame < 40 ){
    if( frame < 28 ){
      idx = 4;
    }else if( frame < 32 ){
      idx = 5;
    }else if( frame < 36 ){
      idx = 6;
    }else{
      idx = 7;
    }
  }else if( frame < 48 ){
    if( frame < 42 ){
      idx = 8;
    }else if( frame < 44 ){
      idx = 9;
    }else if( frame < 46 ){
      idx = 10;
    }else{
      idx = 11;
    }
  }else{
    idx = 12;
  }
  
  {
    u16 now;
    GFL_BBDACT_SYS *actSys;

    actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
    now = GFL_BBDACT_GetAnimeFrmIdx( actSys, work->actID );

    if( now != idx ){
      GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, 0 );
    }
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/// �`�揈���@��ꎩ�@�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_YureHero =
{
  DrawHero_Init,
  DrawYureHero_Draw,
  DrawHero_Delete,
  DrawHero_Delete,  //�ޔ�
  DrawHero_Init,    //�{���͕��A
  DrawHero_GetBlActID,
};

//======================================================================
//  �r���{�[�h�@�P�p�^�[���A�j�����[�v
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�P�p�^�[���A�j���@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_DrawOnePatternLoop( MMDL *mmdl )
{
  VecFx32 pos;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  
  blact_UpdatePauseVanish( mmdl, actSys, work->actID, FALSE, FALSE );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/// �`�揈�� �r���{�[�h�@�P�p�^�[���A�j���@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_BlActOnePatternLoop =
{
  DrawBlAct_Init,
  DrawBlAct_DrawOnePatternLoop,
  DrawBlAct_Delete,
  DrawBlAct_Delete,  //�{���͑ޔ�
  DrawBlAct_Init,    //�{���͕��A
  DrawBlAct_GetBlActID,
};

//======================================================================
//  �r���{�[�h�@�P�p�^�[���A�j��
//======================================================================
#if 0
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�P�p�^�[���A�j���@�`��@���[�v�Ȃ�
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_DrawOnePattern( MMDL *mmdl )
{
  VecFx32 pos;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  
#if 1  
  {
    u16 comm;

    if( GFL_BBDACT_GetAnimeLastCommand(actSys,work->actID,&comm) == TRUE ){
      int flag = TRUE;
      
      if( comm == GFL_BBDACT_ANMCOM_END ){
        GFL_BBDACT_SetAnimeEnable( actSys, work->actID, FALSE );
      }

      if( MMDL_CheckStatusBitVanish(mmdl) == TRUE ){
        flag = FALSE;
      }

      GFL_BBDACT_SetDrawEnable( actSys, work->actID, flag );
    }else{
      blact_UpdatePauseVanish( mmdl, actSys, work->actID, FALSE );
    }
  }
#endif

  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/// �`�揈�� �r���{�[�h�@�P�p�^�[���A�j���@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_BlActOnePattern =
{
  DrawBlAct_Init,
  DrawBlAct_DrawOnePattern,
  DrawBlAct_Delete,
  DrawBlAct_Delete,  //�{���͑ޔ�
  DrawBlAct_Init,    //�{���͕��A
  DrawBlAct_GetBlActID,
};
#endif

//======================================================================
//  �r���{�[�h�@�A������|�P�����A�j��
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�A������|�P�����@������
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawTsurePoke_Init( MMDL *mmdl )
{
  DRAW_BLACT_POKE_WORK *work;
  
  work = MMDL_InitDrawProcWork( mmdl, sizeof(DRAW_BLACT_WORK) );
  work->set_anm_dir = DIR_NOT;
  MMDL_BLACTCONT_AddActor( mmdl, &work->actID );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�A������|�P�����@�폜
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawTsurePoke_Delete( MMDL *mmdl )
{
  DRAW_BLACT_POKE_WORK *work;
  work = MMDL_GetDrawProcWork( mmdl );
  MMDL_BLACTCONT_DeleteActor( mmdl, work->actID );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�A������|�P�����@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawTsurePoke_Draw( MMDL *mmdl )
{
  DRAW_BLACT_POKE_WORK *work;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID != MMDL_BLACTID_NULL ){
    VecFx32 pos;
    u16 dir,init_flag;
    GFL_BBDACT_SYS *actSys;
  
    init_flag = FALSE;

    actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
    dir = MMDL_GetDirDisp( mmdl );
  
    if( dir != work->set_anm_dir ){
      init_flag = TRUE;
      work->set_anm_dir = dir;
      work->offs_frame = 0;
      work->offs_y = 0;
      GFL_BBDACT_SetAnimeIdx(
          actSys, work->actID, work->set_anm_dir );
      { //�`��I�t�Z�b�g���N���A
        VecFx32 offs = {0,0,0};
        MMDL_GetVectorDrawOffsetPos( mmdl, &offs );
      }
    }
    
    TsurePoke_SetAnmAndOffset( mmdl, work, dir );
    
    MMDL_GetDrawVectorPos( mmdl, &pos );
    blact_SetCommonOffsPos( mmdl, &pos );

    GFL_BBD_SetObjectTrans(
      GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
    
    blact_UpdatePauseVanish( mmdl, actSys, work->actID, init_flag, FALSE );
  }
}

static void TsurePoke_SetAnmAndOffset( MMDL* mmdl, DRAW_BLACT_POKE_WORK* work, u8 dir )
{
  VecFx32 vec;
  BOOL pause_f, anmcmd_f;
  const OBJCODE_PARAM* obj_prm;
  
  obj_prm = MMDL_GetOBJCodeParam( mmdl );
  
  VEC_Set(&vec,0,0,0);

  pause_f = MMDL_CheckDrawPause( mmdl );
  anmcmd_f = MMDL_CheckMoveBitAcmd( mmdl );
  
  /*
   *  ����ŕK�v������anmcmd_f�ɂ�镪��
   *  �A�j���R�}���h���s��(�W�����v���Ȃ�)�͏㉺�A�j�����~�߂鏈�����K�v������
   *  WB�ł͘A�����OBJ�ɑ΂��āA�A�j���R�}���h�𔭍s���邱�Ƃ��Ȃ��̂ŁA
   *  ������t�b�N���Ă���
   */

#if 0 //wb_none
  if( pause_f || anmcmd_f ){
#else
  if( pause_f ){
#endif
    //Y�I�t�Z�b�g�݈̂����p��
    MMDL_GetVectorDrawOffsetPos( mmdl, &vec );
    vec.x = vec.z = 0;
  }

  //��������`��I�t�Z�b�g���Z�b�g
  TsurePoke_GetDrawOffsetFromDir( mmdl, dir, obj_prm, &vec );

#if 0 //wb_none
  if( anmcmd_f ){
    work->offs_frame++; 
  }else if( !pause_f ){
    work->offs_frame++; 
    if( TsurePoke_CheckUpDown( work, dir, obj_prm )){
      vec.y -= FX32_CONST(2);
    }
  }
#else
 if( !pause_f ){
    work->offs_frame++; 
    if( TsurePoke_CheckUpDown( work, dir, obj_prm )){
      vec.y -= FX32_CONST(2);
    }
  }
#endif
  MMDL_SetVectorDrawOffsetPos( mmdl, &vec );
}

static void TsurePoke_GetDrawOffsetFromDir( MMDL* mmdl, u8 dir, const OBJCODE_PARAM* obj_prm, VecFx32* outVec)
{
  //��������`��I�t�Z�b�g������
	if ( obj_prm->mdl_size==MMDL_BLACT_MDLSIZE_64x64 ){
		switch(dir){
		case DIR_UP:
			outVec->z += MMDL_POKE_OFS_UPDOWN;
			break;
		case DIR_DOWN:
			outVec->z -= MMDL_POKE_OFS_UPDOWN;
			break;
    }
	}else{
#if 0
		switch(dir){
		case DIR_LEFT:
			outVec->x += MMDL_POKE_OFS_RIGHTLEFT_S;
			break;
		case DIR_RIGHT:
			outVec->x -= MMDL_POKE_OFS_RIGHTLEFT_S;
			break;
		}
#endif
	}
}

static BOOL TsurePoke_CheckUpDown( DRAW_BLACT_POKE_WORK* work, u8 dir, const OBJCODE_PARAM* obj_prm )
{
  int frame;

  frame = work->offs_frame % 20;
  work->offs_frame = frame;
  
  if( obj_prm->draw_proc_no == MMDL_DRAWPROCNO_TPOKE_FLY){
    //�X�s�[�h����
    if( dir == DIR_DOWN){
      if( (frame < 5) || ( 15 <= frame )){
        return TRUE;
      }
    }else if(frame < 10){
      return TRUE;
    }
  }else{
    if ( ((5<=frame)&&(frame<10)) || (15<=frame) ){
			return TRUE;
		}
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�A������|�P����
 * �r���{�[�h�A�N�^�[ID��Ԃ��B
 * @param  mmdl  MMDL
 * @param  state  ���ɖ���
 * @retval  u32  GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static u32 DrawTsurePoke_GetBlActID( MMDL *mmdl, u32 state )
{
  DRAW_BLACT_POKE_WORK *work;
  work = MMDL_GetDrawProcWork( mmdl );
  return( work->actID );
}

//--------------------------------------------------------------
// �`�揈���@�r���{�[�h�@�A������|�P�����@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_TsurePoke =
{
  DrawTsurePoke_Init,
  DrawTsurePoke_Draw,
  DrawTsurePoke_Delete,
  DrawTsurePoke_Delete,  //�ޔ�
  DrawTsurePoke_Init,    //�{���͕��A
  DrawTsurePoke_GetBlActID,
};

//======================================================================
//  �r���{�[�h�@�|�P���� �V���E��
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�|�P�����@�V���E���@������
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlActShinMu_Init( MMDL *mmdl )
{
  DRAW_BLACT_WORK *work;
  
  work = MMDL_InitDrawProcWork( mmdl, sizeof(DRAW_BLACT_WORK) );
  comManAnmCtrl_Init( &work->anmcnt );
  MMDL_BLACTCONT_AddActor( mmdl, &work->actID );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�|�P���� �V���E���@�폜
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlActShinMu_Delete( MMDL *mmdl )
{
  DRAW_BLACT_WORK *work;
  work = MMDL_GetDrawProcWork( mmdl );
  MMDL_BLACTCONT_DeleteActor( mmdl, work->actID );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�|�P���� �V���E�� �`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlActShinMu_Draw( MMDL *mmdl )
{
  u16 status,code;
  VecFx32 pos;
  u16 init_flag = FALSE;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );

  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  
  code = MMDL_GetOBJCode( mmdl );
  status = MMDL_GetDrawStatus( mmdl );
  
  if( code == SHIN_A || code == MU_A || code == POKE10 ){
    u16 anm_idx = DRAW_STA_SHINMU_A_ANMNO_STOP;
    
    if( status != work->anmcnt.set_anm_status ){
      switch( status ){
      case DRAW_STA_SHINMU_A_GUTARI:
        anm_idx = DRAW_STA_SHINMU_A_ANMNO_GUTARI;
        break;
      case DRAW_STA_SHINMU_A_FLY_UP:
        anm_idx = DRAW_STA_SHINMU_A_ANMNO_FLY_UP;
        break;
      case DRAW_STA_SHINMU_A_FLY:
        anm_idx = DRAW_STA_SHINMU_A_ANMNO_FLY;
        break;
      }
      
      work->anmcnt.set_anm_status = status;
      GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_idx );
      init_flag = TRUE;
    }
  }else if( code == SHIN_B || code == MU_B ){ //B
    u16 dir = blact_GetDrawDir( mmdl );
    u16 anm_idx = DRAW_STA_SHINMU_B_ANMNO_STOP_U;
    
    if( status < DRAW_STA_MAX ){ //��{�`
      if( dir != work->anmcnt.set_anm_dir ||
          status != work->anmcnt.set_anm_status ){
        if( dir == DIR_UP ){
          anm_idx = DRAW_STA_SHINMU_B_ANMNO_STOP_U;
        }else if( dir == DIR_DOWN ){
          anm_idx = DRAW_STA_SHINMU_B_ANMNO_STOP_D;
        }
        
        work->anmcnt.set_anm_dir = dir;
        work->anmcnt.set_anm_status = status;
        GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_idx );
        init_flag = TRUE;
      }
    }else{
      if( status != work->anmcnt.set_anm_status ){
        switch( status ){
        case DRAW_STA_SHINMU_B_HOERU:
          anm_idx = DRAW_STA_SHINMU_B_ANMNO_HOERU;
          break;
        case DRAW_STA_SHINMU_B_TURN:
          anm_idx = DRAW_STA_SHINMU_B_ANMNO_TURN;
          break;
        }
        
        work->anmcnt.set_anm_status = status;
        GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_idx );
        init_flag = TRUE;
      }
    }
  }else{ //C
    if( status != work->anmcnt.set_anm_status ){
      u16 anm_idx = DRAW_STA_SHINMU_C_ANMNO_FLY;
      
      work->anmcnt.set_anm_status = status;
      GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_idx );
      init_flag = TRUE;
    }
  }
  
  blact_UpdatePauseVanish( //�V�����͓���|�[�Y�𖳎�����
      mmdl, actSys, work->actID, init_flag, TRUE );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
  GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�|�P���� �V���E���@�擾�B
 * �r���{�[�h�A�N�^�[ID��Ԃ��B
 * @param  mmdl  MMDL
 * @param  state  ���ɖ���
 * @retval  u32  GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static u32 DrawBlActShinMu_GetBlActID( MMDL *mmdl, u32 state )
{
  DRAW_BLACT_WORK *work;
  work = MMDL_GetDrawProcWork( mmdl );
  return( work->actID );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�|�P�����@�V���E����p�@�A�j���t���[��No�擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
void MMDL_ShinMuA_GetAnimeFrame( MMDL *mmdl, u16 *outIdx, u16 *outFrmIdx )
{
  u16 code = MMDL_GetOBJCode( mmdl );
  
  if( code == SHIN_A || code == MU_A ){
    GFL_BBDACT_SYS *actSys =
      MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
    DRAW_BLACT_WORK *work = MMDL_GetDrawProcWork( mmdl );
    
    *outIdx = GFL_BBDACT_GetAnimeIdx( actSys, work->actID );
    *outFrmIdx = GFL_BBDACT_GetAnimeFrmIdx( actSys, work->actID );
  }else{
    GF_ASSERT( 0 );
    *outIdx = 0;
    *outFrmIdx = 0;
  }
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�|�P�����@�V���E����p�@�A�j���t���[��No�擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
void MMDL_ShinMuC_GetAnimeFrame( MMDL *mmdl, u16 *outIdx, u16 *outFrmIdx )
{
  u16 code = MMDL_GetOBJCode( mmdl );
  
  if( code == SHIN_C || code == MU_C ){
    GFL_BBDACT_SYS *actSys =
      MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
    DRAW_BLACT_WORK *work = MMDL_GetDrawProcWork( mmdl );
    
    *outIdx = GFL_BBDACT_GetAnimeIdx( actSys, work->actID );
    *outFrmIdx = GFL_BBDACT_GetAnimeFrmIdx( actSys, work->actID );
  }else{
    GF_ASSERT( 0 );
    *outIdx = 0;
    *outFrmIdx = 0;
  }
}

//--------------------------------------------------------------
//  �`�揈���@�r���{�[�h�@�|�P���� �V���E���@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_BlActShinMu =
{
  DrawBlActShinMu_Init,
  DrawBlActShinMu_Draw,
  DrawBlActShinMu_Delete,
  DrawBlActShinMu_Delete,  //�{���͑ޔ�
  DrawBlActShinMu_Init,    //�{���͕��A
  DrawBlActShinMu_GetBlActID,
};

//======================================================================
//  �r���{�[�h�@�����̑�
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�����̑��@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlActSpider_Draw( MMDL *mmdl )
{
  u16 status;
  VecFx32 pos;
  u16 init_flag = FALSE;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );

  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  
  status = MMDL_GetDrawStatus( mmdl );
  
  if( status != work->anmcnt.set_anm_status ){
    if( status < DRAW_STA_SPIDER_MAX ){
      u16 anm_idx = status;
      work->anmcnt.set_anm_status = status;
      GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_idx );
      
      init_flag = TRUE;
    }
  }
  
  blact_UpdatePauseVanish( mmdl, actSys, work->actID, init_flag, FALSE );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
//  �`�揈���@�r���{�[�h�@�����̑��@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_BlActSpider =
{
  DrawBlAct_Init,
  DrawBlActSpider_Draw,
  DrawBlAct_Delete,
  DrawBlAct_Delete,  //�{���͑ޔ�
  DrawBlAct_Init,    //�{���͕��A
  DrawBlAct_GetBlActID,
};

//======================================================================
//  �r���{�[�h�@�|�P���� �����f�B�A
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�|�P���� �����f�B�A�@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlActMelodyer_Draw( MMDL *mmdl )
{
  u16 status;
  VecFx32 pos;
  u16 init_flag = FALSE;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );

  if( work->actID == MMDL_BLACTID_NULL ){ //���o�^
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  
  status = MMDL_GetDrawStatus( mmdl );
  
  if( status != work->anmcnt.set_anm_status ){
    u16 anm_idx;
    
    switch( status ){
    case DRAW_STA_MELODYER_SPIN:
      anm_idx = DRAW_STA_MELODYER_ANMNO_SPIN;
      break;
    case DRAW_STA_MELODYER_SPIN_POSE:
      anm_idx = DRAW_STA_MELODYER_ANMNO_SPIN_POSE;
      break;
    case DRAW_STA_MELODYER_SHAKE:
      anm_idx = DRAW_STA_MELODYER_ANMNO_SHAKE;
      break;
    default:
      anm_idx = DRAW_STA_MELODYER_ANMNO_STOP;
    }
    
    work->anmcnt.set_anm_status = status;
    GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_idx );
    init_flag = TRUE;
  }
  
  blact_UpdatePauseVanish( mmdl, actSys, work->actID, init_flag, FALSE );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( mmdl, &pos );
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�|�P���� �����f�B�A �A�j���I���`�F�b�N
 * @param  mmdl  MMDL
 * @retval  BOOL TRUE=�I��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckDrawMelodyerAnime( MMDL *mmdl )
{
  if( MMDL_GetOBJCode(mmdl) != POKE12 ){
    GF_ASSERT( 0 );
    return( TRUE );
  }else{
    u16 actID = MMDL_CallDrawGetProc( mmdl, 0 );
    
    if( actID != MMDL_BLACTID_NULL ){
      u16 comm;
      GFL_BBDACT_SYS *actSys;
      
      actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );

      if( GFL_BBDACT_GetAnimeLastCommand(actSys,actID,&comm) == TRUE ){
        return( TRUE );
      }
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�|�P���� �����f�B�A �A�j���t���[��������
 * @param  mmdl  MMDL
 * @retval  BOOL TRUE=�I��
 */
//--------------------------------------------------------------
void MMDL_SetDrawMelodyerStartAnimeFrame( MMDL *mmdl )
{
  if( MMDL_GetOBJCode(mmdl) == POKE12 ){
    u16 actID = MMDL_CallDrawGetProc( mmdl, 0 );
    
    if( actID != MMDL_BLACTID_NULL ){
      GFL_BBDACT_SYS *actSys;
      actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
      GFL_BBDACT_SetAnimeFrmIdx( actSys, actID, 0 );
    }
  }
}

//--------------------------------------------------------------
/// �`�揈���@�r���{�[�h�@�|�P���� �����f�B�A�@�`��
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_BlActMelodyer =
{
  DrawBlAct_Init,
  DrawBlActMelodyer_Draw,
  DrawBlAct_Delete,
  DrawBlAct_Delete,  //�{���͑ޔ�
  DrawBlAct_Init,    //�{���͕��A
  DrawBlAct_GetBlActID,
};

//======================================================================
//  �l���n���ʁ@�r���{�[�h�A�N�^�[�A�j������
//======================================================================
//--------------------------------------------------------------
/**
 * �l���n���ʁ@�r���{�[�h�A�N�^�[�A�j�������@������
 * @param work COMMAN_ANMCTRL_WORK*
 * @retval nothing
 */
//--------------------------------------------------------------
static void comManAnmCtrl_Init( COMMAN_ANMCTRL_WORK *work )
{
  MI_CpuClear8( work, sizeof(COMMAN_ANMCTRL_WORK) );
  work->set_anm_dir = DIR_MAX;
}

//--------------------------------------------------------------
/**
 * �l���n���ʁ@�r���{�[�h�A�N�^�[�A�j�������@�X�V
 * @param work COMMAN_ANMCTRL_WORK*
 * @param mmdl MMDL*
 * @param actSys GFL_BBDACT_SYS
 * @param actID BBDACT ID
 * @retval nothing
 * @note �ړ��n�A�j��Index����4�ŁA
 * Index0,1��2,3�����ꂼ��ʌ̃A�j����O��Ƃ��Ă���B
 * ����ȊO�͑Ή����Ă��Ȃ��B
 */
//--------------------------------------------------------------
static void comManAnmCtrl_Update( COMMAN_ANMCTRL_WORK *work,
    MMDL *mmdl, GFL_BBDACT_SYS *actSys, u16 actID )
{
  u16 init_flag = FALSE;
  u16 dir = blact_GetDrawDir( mmdl );
  u16 status = MMDL_GetDrawStatus( mmdl );
  u16 anm_idx = (status * DIR_MAX4) + dir;
  
  if( work->next_set_wait ) //�Z�b�g�҂����
  {
    if( status == work->set_anm_status ) //�Z�b�g�҂��ŃX�e�[�^�X���ꉻ
    {
      work->next_set_wait = 0; //�Z�b�g�҂�����
    }
  }
  
  if( dir != work->set_anm_dir ) //�����X�V�͖������ōX�V
  {
    init_flag = TRUE;
    work->set_anm_dir = dir;
    work->set_anm_status = status;
    work->next_walk_frmidx = 0;
    GFL_BBDACT_SetAnimeIdx( actSys, actID, anm_idx );
  }
  else if( status != work->set_anm_status ) //�X�e�[�^�X�X�V
  {
    init_flag = TRUE;
     
    if( status == DRAW_STA_STOP ) //�Z�b�g��~�^�C�v
    {
      if( work->set_anm_status == DRAW_STA_STOP ) //��~�A�j����
      {
        GFL_BBDACT_SetAnimeIdx( actSys, actID, anm_idx );
        work->set_anm_status = status;
        work->next_set_wait = 0;
      }
      else if( work->next_set_wait == 0 ) //�ړ��n 1frame��ɃZ�b�g
      {
        work->next_set_wait++;
        init_flag = FALSE;
      }
      else //�ړ��n 1frame�҂���
      {
        u8 tbl[4] = {0,2,2,0}; //����ړ��n�Z�b�g���̃A�j��Index
        
        work->next_walk_frmidx = GFL_BBDACT_GetAnimeFrmIdx( actSys, actID );
/*        
        #ifdef DEBUG_ONLY_FOR_kagaya
        if( MMDL_GetOBJID(mmdl) == MMDL_ID_PLAYER ){
          OS_Printf( "���@�A�j���؂�ւ��@�ړ��n->��~ ����܂ł�IDX=%d\n",
              work->next_walk_frmidx ); 
        }       
        #endif
*/        
        if( work->next_walk_frmidx >= 4 ) //index over
        {
          work->next_walk_frmidx = 0;
        }
        
        work->next_walk_frmidx = tbl[work->next_walk_frmidx];
        
        GFL_BBDACT_SetAnimeIdx( actSys, actID, anm_idx );
        work->set_anm_status = status;
        work->next_set_wait = 0;
      }
    }
    else //�Z�b�g�����^�C�v
    {
      if( work->set_anm_status != DRAW_STA_STOP ) //�ړ��n�A�j�����s���Ă���
      { //���̃A�j����\��
        work->next_walk_frmidx = GFL_BBDACT_GetAnimeFrmIdx( actSys, actID );
        
        if( work->next_walk_frmidx >= 2 )
        {
          work->next_walk_frmidx = 0;
        }
        else
        {
          work->next_walk_frmidx = 2;
        }
      }
      
      GF_ASSERT( work->next_walk_frmidx < 4 );
      GFL_BBDACT_SetAnimeIdx( actSys, actID, anm_idx );
      GFL_BBDACT_SetAnimeFrmIdx( actSys, actID, work->next_walk_frmidx );
      
      work->set_anm_status = status;
      work->next_set_wait = 0;
    }
  }

#if 0
  //������������A�A�j�����ɉh�����܂łɃA�j����~�o�O�����󂠂�B
  //��~���͖�����蔽�f�����鎖�ŋً}�Ώ�
  //
  //100331 ��������
  //���샂�f���X�V�ƃr���{�[�h�X�V����v���Ȃ��ꍇ������ׂ������B
  if( work->set_anm_status == DRAW_STA_STOP ){
    init_flag = TRUE;
  }
#endif
  
  blact_UpdatePauseVanish( mmdl, actSys, actID, init_flag, FALSE );
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�hOBJ ���ʃI�t�Z�b�g���W�Z�b�g 32x32
 * @param pos VecFx32
 * @retval nothing
 */
//--------------------------------------------------------------
static void blact_SetCommonOffsPos( MMDL * mmdl, VecFx32 *pos )
{
  MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  FIELDMAP_WORK * fieldmap = MMDLSYS_GetFieldMapWork( mmdlsys );
  fx32 y_offs = FIELDMAP_GetBbdActYOffs( fieldmap );
  pos->y += y_offs;
  //pos->y += MMDL_BBD_OFFS_POS_Y;

#ifdef MMDL_BBD_DRAW_OFFS_Z_USE
  pos->z += MMDL_BBD_OFFS_POS_Z;
#endif
}

//--------------------------------------------------------------
/**
 * �J�������E�̐U��p���l�����ĕ����擾
 * @param mmdl MMDL
 * @retval u16 ����
 */
//--------------------------------------------------------------
static u16 blact_GetDrawDir( MMDL *mmdl )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  u16 angle = MMDLSYS_GetTargetCameraAngleYaw( mmdlsys );
  u16 dir = MMDL_GetDirDisp( mmdl );
  
  if( !MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) )
  {
    dir = MMDL_TOOL_GetAngleYawToDirFour( dir, angle );
  }
  else
  {
    // ���[���}�b�v�ł́A�i�s�������֌W���Ă��邽�߁B
    dir = MMDL_RAIL_GetAngleYawToDirFour( mmdl, angle );
  }
  
  return( dir );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[���ʁ@�A�j���ꎞ��~�A��\���X�V
 * @param mmdl MMDL*
 * @param actSys GFL_BBDACT_SYS
 * @param actID BBDACT ID
 * @param start_anm TRUE=����A�j���ł���
 * @param anm_pause_only TRUE=����|�[�Y�𖳎�����(�A�j���|�[�Y�̂ݔ���
 * @retval nothing
 */
//--------------------------------------------------------------
static void blact_UpdatePauseVanish(
    MMDL *mmdl, GFL_BBDACT_SYS *actSys, u16 actID,
    BOOL start_anm, BOOL anm_pause_only )
{
  BOOL flag = TRUE; //�A�j���t���O
  BOOL update = MMDL_BLACTCONT_CheckUpdateBBD( mmdl );
  
  if( start_anm == FALSE )    //����A�j���ł͂Ȃ�
  {
    if( anm_pause_only == TRUE )  //�A�j���|�[�Y�̂ݔ��肷��
    {
      if( update == TRUE )    //�A�N�^�[�X�V�ς݂�
      {                       //�A�j���|�[�Y�w�肪����Ȃ�
	      if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_PAUSE_ANM) )
        {
          flag = FALSE;       //�A�j����~
        }
      }
    }
    else                      //����|�[�Y����������
    {
      if( update == TRUE )    //�A�N�^�[�X�V�ς݂�
      {                       //�`��|�[�Y�w�肪����Ȃ��
        if( MMDL_CheckDrawPause(mmdl) == TRUE )
        {
          flag = FALSE;           //�A�j����~
        }
      }
    }
  }
  
  GFL_BBDACT_SetAnimeEnable( actSys, actID, flag ); //�A�j���L���t���O
  
  flag = TRUE; //�\���t���O

  if( MMDL_CheckStatusBitVanish(mmdl) == TRUE ){
    flag = FALSE; //��\��
  }
  
  GFL_BBDACT_SetDrawEnable( actSys, actID, flag ); //�\���L���t���O
}
