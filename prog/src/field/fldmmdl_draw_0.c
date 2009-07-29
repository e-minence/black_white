//======================================================================
/**
 *
 * @file	fldmmdl_draw_0.c
 * @brief	�t�B�[���h���샂�f�� �`�揈�����̂O
 * @author	kagaya
 * @data	05.07.13
 *
 */
//======================================================================
#include "fldmmdl.h"
#include "fldmmdl_procdraw.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
/// �r���{�[�h���ʃI�t�Z�b�g�\�����W
//--------------------------------------------------------------
#define MMDL_BBD_OFFS_POS_Y (-FX32_ONE*2)  //(FX32_ONE*4)
#define MMDL_BBD_OFFS_POS_Z (FX32_ONE*4)  //(-FX32_ONE*8)

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	DRAW_BLACT_WORK
//--------------------------------------------------------------
typedef struct
{
	GFL_BBDACT_ACTUNIT_ID actID;
	u8 set_anm_dir;
	u8 set_anm_status;
#if 0
  fx32 TestScale;
  fx32 TestScaleAdd;
#endif
}DRAW_BLACT_WORK;

//======================================================================
//	proto
//======================================================================

//======================================================================
//	�`�揈���@�`�斳��
//======================================================================
//--------------------------------------------------------------
/**
 * �`�斳���@������
 * @param	mmdl		MMDL * 
 * @retval	nothing
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
 * @param	mmdl		MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_DrawNon_Draw( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`�斳���@�폜
 * @param	mmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_DrawNon_Delete( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`�斳���@�ޔ�
 * @param	mmdl		MMDL * 
 * @retval	int			TRUE=����������
 */
//--------------------------------------------------------------
static void MMdl_DrawNon_Push( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`�斳���@���A
 * �ޔ������������ɍĕ`��B
 * @param	mmdl		MMDL * 
 * @retval	int			TRUE=����������
 */
//--------------------------------------------------------------
static void MMdl_DrawNon_Pop( MMDL * mmdl )
{
}

//--------------------------------------------------------------
///	�`�揈���@�`��Ȃ��@�܂Ƃ�
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
//	�`�揈���@�r���{�[�h�@���@��p
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���@��p�@������
 * @param	mmdl	MMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DrawHero_Init( MMDL *mmdl )
{
  u16 code;
	DRAW_BLACT_WORK *work;
  
	work = MMDL_InitDrawProcWork( mmdl, sizeof(DRAW_BLACT_WORK) );
	work->set_anm_dir = DIR_NOT;
  
  code = MMDL_GetOBJCode( mmdl );
  
	if( MMDL_BLACTCONT_AddActor(mmdl,code,&work->actID) == TRUE ){
    MMDL_CallDrawProc( mmdl );
  }
#if 0  
  { //test
    work->TestScale = FX16_ONE*8;
    work->TestScaleAdd = 0x0100;
  }
#endif
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���@��p�@�폜
 * @param	mmdl	MMDL
 * @retval	nothing
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
 * @param	mmdl	MMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DrawHero_Draw( MMDL *mmdl )
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
	
	dir = MMDL_GetDirDisp( mmdl );
	status = MMDL_GetDrawStatus( mmdl );
	GF_ASSERT( status < DRAW_STA_MAX_HERO );
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

	pos.y += MMDL_BBD_OFFS_POS_Y;
  pos.z += MMDL_BBD_OFFS_POS_Z;
	
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

#if 0 
  { //ScaleTest
    /*
    void	GFL_BBD_SetObjectSiz( GFL_BBD_SYS* billboardSys, int objIdx, const fx16* sizX, const fx16* sizY );
    */
    GFL_BBD_SYS *bbdSys = GFL_BBDACT_GetBBDSystem( actSys );
    int idx = GFL_BBDACT_GetBBDActIdxResIdx( actSys, work->actID );
    fx16 sx = FX16_ONE*4 - 1;
    fx16 sy = FX16_ONE*4 - 1;
    
    work->TestScale += work->TestScaleAdd;
    
    if( work->TestScale <= FX16_ONE ){
      work->TestScale = FX16_ONE;
      work->TestScaleAdd = -work->TestScaleAdd;
    }else if( work->TestScale >= (FX16_ONE*8-1) ){
      work->TestScale = (FX16_ONE*8-1);
      work->TestScaleAdd = -work->TestScaleAdd;
    }
    
    sx = work->TestScale;
    GFL_BBD_SetObjectSiz( bbdSys, idx, &sx, &sy );
    
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_A ){
      KAGAYA_Printf( "���@���T�C�Y 0x%x\n", work->TestScale );
    }else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_B ){
      VecFx32 scale;
      GFL_BBD_GetScale( bbdSys, &scale );
      KAGAYA_Printf( "�r���{�[�h�X�P�[�� 0x%x,0x%x,0x%x\n",
          scale.x, scale.y, scale.z );
    }
    
    {
      BOOL flip = TRUE;
      GFL_BBD_SetObjectFlipT( bbdSys, idx, &flip );
    }
  }
#endif
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���@��p�@�擾�B
 * �r���{�[�h�A�N�^�[ID��Ԃ��B
 * @param	mmdl	MMDL
 * @param	state	���ɖ���
 * @retval	u32	GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static u32 DrawHero_GetBlActID( MMDL *mmdl, u32 state )
{
	DRAW_BLACT_WORK *work;
	work = MMDL_GetDrawProcWork( mmdl );
	return( work->actID );
}

//--------------------------------------------------------------
//	�`�揈���@�r���{�[�h�@���@�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_Hero =
{
	DrawHero_Init,
	DrawHero_Draw,
	DrawHero_Delete,
	DrawHero_Delete,	//�ޔ�
	DrawHero_Init,    //�{���͕��A
	DrawHero_GetBlActID,
};

//======================================================================
//  �`�揈���@�r���{�[�h�@���]�Ԏ��@��p
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���]�Ԏ��@��p�@�`��
 * @param	mmdl	MMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
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
	
	dir = MMDL_GetDirDisp( mmdl );
	status = MMDL_GetDrawStatus( mmdl );
	GF_ASSERT( status < DRAW_STA_MAX_HERO );
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
  
	pos.y += MMDL_BBD_OFFS_POS_Y;
  pos.z += MMDL_BBD_OFFS_POS_Z;
	
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

//--------------------------------------------------------------
/// �`�揈���@���]�Ԏ��@�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_CycleHero =
{
  DrawHero_Init,
	DrawCycleHero_Draw,
	DrawHero_Delete,
	DrawHero_Delete,	//�ޔ�
	DrawHero_Init,    //�{���͕��A
	DrawHero_GetBlActID,
};

//======================================================================
//  �`�揈���@�r���{�[�h�@�g��莩�@��p
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�g��莩�@��p�@�`��
 * @param	mmdl	MMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DrawSwimHero_Draw( MMDL *mmdl )
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
	
	dir = MMDL_GetDirDisp( mmdl );
	status = MMDL_GetDrawStatus( mmdl );
  GF_ASSERT( dir < DIR_MAX4 );
  anm_id = dir;

#if 0
  {
    u16 idx = GFL_BBDACT_GetAnimeFrmIdx(actSys,work->actID);
    KAGAYA_Printf("�g���A�j�� %d\n", idx );
  }
#endif

	if( work->set_anm_dir != dir ){ //�����X�V
		work->set_anm_dir = dir;
		work->set_anm_status = status;
		GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
		chg_dir = TRUE;
	}else if( work->set_anm_status != status ){
		u16 frame = 0;
		GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
		GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, frame );
		work->set_anm_status = status;
	}
	
	MMDL_GetDrawVectorPos( mmdl, &pos );
  
	pos.y += MMDL_BBD_OFFS_POS_Y;
  pos.z += MMDL_BBD_OFFS_POS_Z;
	
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

//--------------------------------------------------------------
/// �`�揈���@�g��莩�@�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_SwimHero =
{
  DrawHero_Init,
	DrawSwimHero_Draw,
	DrawHero_Delete,
	DrawHero_Delete,	//�ޔ�
	DrawHero_Init,    //�{���͕��A
	DrawHero_GetBlActID,
};

//======================================================================
//	�`�揈���@�r���{�[�h�@�ėp
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ėp�@������
 * @param	mmdl	MMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_Init( MMDL *mmdl )
{
  u16 code;
	DRAW_BLACT_WORK *work;
  
	work = MMDL_InitDrawProcWork( mmdl, sizeof(DRAW_BLACT_WORK) );
	work->set_anm_dir = DIR_NOT;
  
  code = MMDL_GetOBJCode( mmdl );

	if( MMDL_BLACTCONT_AddActor(mmdl,code,&work->actID) == TRUE ){
    MMDL_CallDrawProc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ėp�@�폜
 * @param	mmdl	MMDL
 * @retval	nothing
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
 * @param	mmdl	MMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_Draw( MMDL *mmdl )
{
	VecFx32 pos;
	BOOL chg_dir = FALSE;
	u16 dir,anm_id,status;
	DRAW_BLACT_WORK *work;
	GFL_BBDACT_SYS *actSys;
	
	work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){
    return;
  }
  
	actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
	
	dir = MMDL_GetDirDisp( mmdl );
	status = MMDL_GetDrawStatus( mmdl );
	GF_ASSERT( status < DRAW_STA_MAX );
	anm_id = status * DIR_MAX4;
	anm_id += dir;
	
	if( work->set_anm_dir != dir ){ //�����X�V
		work->set_anm_dir = dir;
		work->set_anm_status = status;
		GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
		chg_dir = TRUE;
	}else if( work->set_anm_status != status ){ //�X�e�[�^�X�X�V
		u16 frame = 0;
		switch( work->set_anm_status ){
		case DRAW_STA_WALK_32F:
		case DRAW_STA_WALK_16F:
		case DRAW_STA_WALK_8F:
		case DRAW_STA_WALK_4F:
		case DRAW_STA_WALK_2F:
			if( GFL_BBDACT_GetAnimeFrmIdx(actSys,work->actID) <= 2 ){
				frame = 2;
			}
			break;
		}
		
		GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
		GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, frame );
	}
	
	MMDL_GetDrawVectorPos( mmdl, &pos );
	
  pos.y += MMDL_BBD_OFFS_POS_Y;
  pos.z += MMDL_BBD_OFFS_POS_Z;
	
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

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ėp�{��ɃA�j���@�`��
 * @param	mmdl	MMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_DrawAlwaysAnime( MMDL *mmdl )
{
	VecFx32 pos;
	BOOL chg_dir = FALSE;
	u16 dir,anm_id;
	DRAW_BLACT_WORK *work;
	GFL_BBDACT_SYS *actSys;
	
	work = MMDL_GetDrawProcWork( mmdl );
  
  if( work->actID == MMDL_BLACTID_NULL ){
    return;
  }
  
	actSys = MMDL_BLACTCONT_GetBbdActSys( MMDL_GetBlActCont(mmdl) );
	dir = MMDL_GetDirDisp( mmdl );
  
	anm_id = DRAW_STA_WALK_16F * DIR_MAX4;
	anm_id += dir;
	
	if( work->set_anm_dir != dir ){ //�����X�V
		work->set_anm_dir = dir;
		GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
		chg_dir = TRUE;
	}
	
	MMDL_GetDrawVectorPos( mmdl, &pos );
  pos.y += MMDL_BBD_OFFS_POS_Y;
  pos.z += MMDL_BBD_OFFS_POS_Z;
	
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

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ėp�@�擾�B
 * �r���{�[�h�A�N�^�[ID��Ԃ��B
 * @param	mmdl	MMDL
 * @param	state	���ɖ���
 * @retval	u32	GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static u32 DrawBlAct_GetBlActID( MMDL *mmdl, u32 state )
{
	DRAW_BLACT_WORK *work;
	work = MMDL_GetDrawProcWork( mmdl );
	return( work->actID );
}

//--------------------------------------------------------------
//	�`�揈���@�r���{�[�h�@�ėp�@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_BlAct =
{
	DrawBlAct_Init,
	DrawBlAct_Draw,
	DrawBlAct_Delete,
	DrawBlAct_Delete,	//�{���͑ޔ�
	DrawBlAct_Init,		//�{���͕��A
	DrawBlAct_GetBlActID,
};

//--------------------------------------------------------------
//	�`�揈���@�r���{�[�h�@�ėp+��ɃA�j���@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_BlActAlwaysAnime =
{
	DrawBlAct_Init,
	DrawBlAct_DrawAlwaysAnime,
	DrawBlAct_Delete,
	DrawBlAct_Delete,	//�{���͑ޔ�
	DrawBlAct_Init,		//�{���͕��A
	DrawBlAct_GetBlActID,
};

