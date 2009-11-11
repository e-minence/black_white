//======================================================================
/**
 *
 * @file  fldmmdl_draw_0.c
 * @brief  �t�B�[���h���샂�f�� �`�揈�����̂O
 * @author  kagaya
 * @date  05.07.13
 *
 */
//======================================================================
#include "fldmmdl.h"
#include "fldmmdl_procdraw.h"

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
/// �r���{�[�h���ʃI�t�Z�b�g�\�����W
//--------------------------------------------------------------
#define MMDL_BBD_OFFS_POS_Y (-FX32_ONE*2)  //(FX32_ONE*4)
#define MMDL_BBD_OFFS_POS_Z (FX32_ONE*4)  //(-FX32_ONE*8)

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
  u8 set_anm_dir;
  u8 set_anm_status;
   
  COMMAN_ANMCTRL_WORK anmcnt_work;
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

static void blact_SetCommonOffsPos( VecFx32 *pos );
static u16 blact_GetDrawDir( MMDL *mmdl );
static void blact_UpdatePauseVanish(
    MMDL *mmdl, GFL_BBDACT_SYS *actSys, u16 actID, BOOL force_anm );

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
  MMDL_OnStatusBit( mmdl, MMDL_STABIT_SHADOW_VANISH );
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
  u16 code;
  DRAW_BLACT_WORK *work;
  
  work = MMDL_InitDrawProcWork( mmdl, sizeof(DRAW_BLACT_WORK) );
  work->set_anm_dir = DIR_NOT;
  
  code = MMDL_GetOBJCode( mmdl );
  comManAnmCtrl_Init( &work->anmcnt_work );
  
  if( MMDL_BLACTCONT_AddActor(mmdl,code,&work->actID) == TRUE ){
    MMDL_CallDrawProc( mmdl );
  }
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
  comManAnmCtrl_Update( &work->anmcnt_work, mmdl, actSys, work->actID );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( &pos );
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
  GF_ASSERT( status < DRAW_STA_MAX_HERO );

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
    case DRAW_STA_DASH_4F:
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
  blact_SetCommonOffsPos( &pos );
  
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
  
  if( work->set_anm_dir != dir ){ //�����X�V
    work->set_anm_dir = dir;
    work->set_anm_status = status;
    GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
    init_flag = TRUE;
  }else if( work->set_anm_status != status ){ //�X�e�[�^�X�X�V
    u16 frame = 0;
    GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
    GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, frame );
    work->set_anm_status = status;
  }
  
  blact_UpdatePauseVanish( mmdl, actSys, work->actID, init_flag );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( &pos );
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
  u16 code;
  DRAW_BLACT_WORK *work;
  
  work = MMDL_InitDrawProcWork( mmdl, sizeof(DRAW_BLACT_WORK) );
  work->set_anm_dir = DIR_NOT;
  
  code = MMDL_GetOBJCode( mmdl );
  comManAnmCtrl_Init( &work->anmcnt_work );

  if( MMDL_BLACTCONT_AddActor(mmdl,code,&work->actID) == TRUE ){
    MMDL_CallDrawProc( mmdl );
  }
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
  
#if 0
  if( MMDL_GetOBJID(mmdl) == 1 && MMDL_GetOBJCode(mmdl) == WOMAN2 ){
    KAGAYA_Printf( "���܂���\n" );
  }
#endif
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  comManAnmCtrl_Update( &work->anmcnt_work, mmdl, actSys, work->actID );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( &pos );
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
  u16 dir,anm_id;
  DRAW_BLACT_WORK *work;
  GFL_BBDACT_SYS *actSys;
  
  work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){
    return;
  }
  
  actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
  dir = blact_GetDrawDir( mmdl );
  
  anm_id = DRAW_STA_WALK_16F * DIR_MAX4;
  anm_id += dir;
  
  if( work->set_anm_dir != dir ){ //�����X�V
    work->set_anm_dir = dir;
    GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
    init_flag = TRUE;
  }
  
  blact_UpdatePauseVanish( mmdl, actSys, work->actID, init_flag );

  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( &pos );
  GFL_BBD_SetObjectTrans(
    GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@PC���o����
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_DrawPCWoman( MMDL *mmdl )
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
    
    if( status == DRAW_STA_PC_BOW ){
      u16 init_flag = FALSE;
      u16 dir = blact_GetDrawDir( mmdl );
      u16 anm_idx = (status * DIR_MAX4);
      COMMAN_ANMCTRL_WORK *anmcnt = &work->anmcnt_work;
      
      if( status != anmcnt->set_anm_status ){
        init_flag = TRUE;
        anmcnt->set_anm_dir = dir;
        anmcnt->set_anm_status = status;
        anmcnt->next_walk_frmidx = 0;
        GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_idx );
      }

      blact_UpdatePauseVanish( mmdl, actSys, work->actID, init_flag );
    }else{
      comManAnmCtrl_Update( &work->anmcnt_work, mmdl, actSys, work->actID );
    }
  }
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( &pos );
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
//  �`�揈���@�r���{�[�h�@PC���o����@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_BlActPCWoman =
{
  DrawBlAct_Init,
  DrawBlAct_DrawPCWoman,
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
  
  if( work->set_anm_status >= DRAW_STA_ITEMGET_MAX ){ //�G���[�t�H���[
    work->set_anm_status = DRAW_STA_ITEMGET_STOP;
    GFL_BBDACT_SetAnimeIdx( actSys, work->actID, work->set_anm_status );
    GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, 0 );
  }
 
  if( status < DRAW_STA_ITEMGET_MAX ){
    if( work->set_anm_status != status ){
      work->set_anm_status = status;
      GFL_BBDACT_SetAnimeIdx( actSys, work->actID, work->set_anm_status );
      GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, 0 );
    }
  }

  blact_UpdatePauseVanish( mmdl, actSys, work->actID, FALSE );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( &pos );
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
  
  if( work->set_anm_status >= DRAW_STA_PCAZUKE_MAX ){ //�G���[�t�H���[
    work->set_anm_status = DRAW_STA_ITEMGET_STOP;
    GFL_BBDACT_SetAnimeIdx( actSys, work->actID, work->set_anm_status );
    GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, 0 );
  }
 
  if( status < DRAW_STA_PCAZUKE_MAX ){
    if( work->set_anm_status != status ){
      work->set_anm_status = status;
      GFL_BBDACT_SetAnimeIdx( actSys, work->actID, work->set_anm_status );
      GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, 0 );
    }
  }
  
  blact_UpdatePauseVanish( mmdl, actSys, work->actID, FALSE );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( &pos );
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
  
  blact_UpdatePauseVanish( mmdl, actSys, work->actID, FALSE );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  blact_SetCommonOffsPos( &pos );
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
  u16 code;
  DRAW_BLACT_POKE_WORK *work;
  
  work = MMDL_InitDrawProcWork( mmdl, sizeof(DRAW_BLACT_WORK) );
  work->set_anm_dir = DIR_NOT;
  
  code = MMDL_GetOBJCode( mmdl );
  
  if( MMDL_BLACTCONT_AddActor(mmdl,code,&work->actID) == TRUE ){
    MMDL_CallDrawProc( mmdl );
  }
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
      GFL_BBDACT_SetAnimeIdx( actSys, work->actID, work->set_anm_dir );
    }
    
    MMDL_GetDrawVectorPos( mmdl, &pos );
    
    blact_SetCommonOffsPos( &pos );
    GFL_BBD_SetObjectTrans(
      GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
    
    blact_UpdatePauseVanish( mmdl, actSys, work->actID, init_flag );
  }
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�A������|�P�����@�c�h��t���@�`��
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DrawTsurePokeFly_Draw( MMDL *mmdl )
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
      GFL_BBDACT_SetAnimeIdx( actSys, work->actID, work->set_anm_dir );
    }
    
    if( MMDL_CheckDrawPause(mmdl) == FALSE ){ //to GS
      work->offs_frame++;
      work->offs_frame %= 20;
      
      if( work->offs_frame >= 15 || //5-9 or 15-19
          (work->offs_frame >= 5 && work->offs_frame < 10) ){
        work->offs_y -= FX32_ONE * 2;
      }else{
        work->offs_y += FX32_ONE * 2;
      }
    }
    
    pos.x = 0;
    pos.y = work->offs_y;
    pos.z = 0;
	  MMDL_SetVectorDrawOffsetPos( mmdl, &pos );
    
    MMDL_GetDrawVectorPos( mmdl, &pos );
    blact_SetCommonOffsPos( &pos );
    GFL_BBD_SetObjectTrans(
      GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
    
    MMDL_GetDrawVectorPos( mmdl, &pos );
    blact_SetCommonOffsPos( &pos );
    blact_UpdatePauseVanish( mmdl, actSys, work->actID, init_flag );
  }
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

const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_TsurePokeFly =
{
  DrawTsurePoke_Init,
  DrawTsurePokeFly_Draw,
  DrawTsurePoke_Delete,
  DrawTsurePoke_Delete,  //�ޔ�
  DrawTsurePoke_Init,    //�{���͕��A
  DrawTsurePoke_GetBlActID,
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
        
        #ifdef DEBUG_ONLY_FOR_kagaya
        if( MMDL_GetOBJID(mmdl) == MMDL_ID_PLAYER ){
          OS_Printf( "���@�A�j���؂�ւ��@�ړ��n->��~ ����܂ł�IDX=%d\n",
              work->next_walk_frmidx ); 
        }       
        #endif
        
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

        #ifdef DEBUG_ONLY_FOR_kagaya
        if( MMDL_GetOBJID(mmdl) == MMDL_ID_PLAYER ){
          OS_Printf( "���@�A�j���؂�ւ��@�ړ��n�p�� ����܂ł�IDX=%d\n",
              work->next_walk_frmidx ); 
        }       
        #endif
        
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
  
  #ifdef DEBUG_ONLY_FOR_kagaya
  {
  }
  #endif
  
  blact_UpdatePauseVanish( mmdl, actSys, actID, init_flag );
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
static void blact_SetCommonOffsPos( VecFx32 *pos )
{
  pos->y += MMDL_BBD_OFFS_POS_Y;
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
  dir = MMDL_TOOL_GetAngleYawToDirFour( dir, angle );
  return( dir );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[���ʁ@�A�j���ꎞ��~�A��\���X�V
 * @param mmdl MMDL*
 * @param actSys GFL_BBDACT_SYS
 * @param actID BBDACT ID
 * @param force_anm TRUE=�A�j�������Z�b�g
 * @retval nothing
 */
//--------------------------------------------------------------
static void blact_UpdatePauseVanish(
    MMDL *mmdl, GFL_BBDACT_SYS *actSys, u16 actID, BOOL force_anm )
{
  BOOL flag = TRUE;
  
  if( force_anm == FALSE && MMDL_CheckDrawPause(mmdl) == TRUE ){
    flag = FALSE;
  }
  
  GFL_BBDACT_SetAnimeEnable( actSys, actID, flag );
  
  flag = TRUE;

  if( MMDL_CheckStatusBitVanish(mmdl) == TRUE ){
    flag = FALSE;
  }
  
  GFL_BBDACT_SetDrawEnable( actSys, actID, flag );
}

