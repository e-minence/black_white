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
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_DrawNon_Init( FLDMMDL * fmmdl )
{
	FLDMMDL_SetStatusBitVanish( fmmdl, TRUE );
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_SHADOW_VANISH );
}

//--------------------------------------------------------------
/**
 * �`�斳���@�`��
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_DrawNon_Draw( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * �`�斳���@�폜
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_DrawNon_Delete( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * �`�斳���@�ޔ�
 * @param	fmmdl		FLDMMDL * 
 * @retval	int			TRUE=����������
 */
//--------------------------------------------------------------
static void FldMMdl_DrawNon_Push( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * �`�斳���@���A
 * �ޔ������������ɍĕ`��B
 * @param	fmmdl		FLDMMDL * 
 * @retval	int			TRUE=����������
 */
//--------------------------------------------------------------
static void FldMMdl_DrawNon_Pop( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
///	�`�揈���@�`��Ȃ��@�܂Ƃ�
//--------------------------------------------------------------
const FLDMMDL_DRAW_PROC_LIST DATA_FLDMMDL_DRAWPROCLIST_Non =
{
	FldMMdl_DrawNon_Init,
	FldMMdl_DrawNon_Draw,
	FldMMdl_DrawNon_Delete,
	FldMMdl_DrawNon_Push,
	FldMMdl_DrawNon_Pop,
	NULL,
};

//======================================================================
//	�`�揈���@�r���{�[�h�@���@��p
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���@��p�@������
 * @param	fmmdl	FLDMMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DrawHero_Init( FLDMMDL *fmmdl )
{
	DRAW_BLACT_WORK *work;
	work = FLDMMDL_InitDrawProcWork( fmmdl, sizeof(DRAW_BLACT_WORK) );
	work->set_anm_dir = DIR_NOT;
	work->actID = FLDMMDL_BLACTCONT_AddActor(
		fmmdl, FLDMMDL_GetOBJCode(fmmdl) );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���@��p�@�폜
 * @param	fmmdl	FLDMMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DrawHero_Delete( FLDMMDL *fmmdl )
{
	DRAW_BLACT_WORK *work;
	work = FLDMMDL_GetDrawProcWork( fmmdl );
	FLDMMDL_BLACTCONT_DeleteActor( fmmdl, work->actID );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���@��p�@�`��
 * @param	fmmdl	FLDMMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DrawHero_Draw( FLDMMDL *fmmdl )
{
	VecFx32 pos;
	BOOL chg_dir = FALSE;
	u16 dir,anm_id,status;
	DRAW_BLACT_WORK *work;
	GFL_BBDACT_SYS *actSys;
	
	work = FLDMMDL_GetDrawProcWork( fmmdl );
	actSys = FLDMMDL_BLACTCONT_GetBbdActSys( FLDMMDL_GetBlActCont(fmmdl) );
	
	dir = FLDMMDL_GetDirDisp( fmmdl );
	status = FLDMMDL_GetDrawStatus( fmmdl );
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
	
	FLDMMDL_GetDrawVectorPos( fmmdl, &pos );
	#ifndef FLDMMDL_BLACT_HEAD3_TEST
	pos.y += FX32_ONE * 12;
	#else
	pos.y += FX32_ONE * 7; //3
	#endif
	
	GFL_BBD_SetObjectTrans(
		GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
	{
		BOOL flag = TRUE;
		if( chg_dir == FALSE && FLDMMDL_CheckDrawPause(fmmdl) == TRUE ){
			flag = FALSE;
		}
		GFL_BBDACT_SetAnimeEnable( actSys, work->actID, flag );
	}
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@���@��p�@�擾�B
 * �r���{�[�h�A�N�^�[ID��Ԃ��B
 * @param	fmmdl	FLDMMDL
 * @param	state	���ɖ���
 * @retval	u32	GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static u32 DrawHero_GetBlActID( FLDMMDL *fmmdl, u32 state )
{
	DRAW_BLACT_WORK *work;
	work = FLDMMDL_GetDrawProcWork( fmmdl );
	return( work->actID );
}

//--------------------------------------------------------------
//	�`�揈���@�r���{�[�h�@���@�@�܂Ƃ�
//--------------------------------------------------------------
const FLDMMDL_DRAW_PROC_LIST DATA_FLDMMDL_DRAWPROCLIST_Hero =
{
	DrawHero_Init,
	DrawHero_Draw,
	DrawHero_Delete,
	DrawHero_Delete,	//�{���͑ޔ�
	DrawHero_Init,		//�{���͕��A
	DrawHero_GetBlActID,
};

//======================================================================
//	�`�揈���@�r���{�[�h�@�ėp
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ėp�@������
 * @param	fmmdl	FLDMMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_Init( FLDMMDL *fmmdl )
{
	DRAW_BLACT_WORK *work;
	work = FLDMMDL_InitDrawProcWork( fmmdl, sizeof(DRAW_BLACT_WORK) );
	work->set_anm_dir = DIR_NOT;
	work->actID = FLDMMDL_BLACTCONT_AddActor(
			fmmdl, FLDMMDL_GetOBJCode(fmmdl) );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ėp�@�폜
 * @param	fmmdl	FLDMMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_Delete( FLDMMDL *fmmdl )
{
	DRAW_BLACT_WORK *work;
	work = FLDMMDL_GetDrawProcWork( fmmdl );
	FLDMMDL_BLACTCONT_DeleteActor( fmmdl, work->actID );
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ėp�@�`��
 * @param	fmmdl	FLDMMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DrawBlAct_Draw( FLDMMDL *fmmdl )
{
	VecFx32 pos;
	BOOL chg_dir = FALSE;
	u16 dir,anm_id,status;
	DRAW_BLACT_WORK *work;
	GFL_BBDACT_SYS *actSys;
	
	work = FLDMMDL_GetDrawProcWork( fmmdl );
	actSys = FLDMMDL_BLACTCONT_GetBbdActSys( FLDMMDL_GetBlActCont(fmmdl) );
	
	dir = FLDMMDL_GetDirDisp( fmmdl );
	status = FLDMMDL_GetDrawStatus( fmmdl );
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
	
	FLDMMDL_GetDrawVectorPos( fmmdl, &pos );
	#ifndef FLDMMDL_BLACT_HEAD3_TEST
	pos.y += FX32_ONE * 12;
	#else
	pos.y += FX32_ONE * 7; //3
	#endif
	
	GFL_BBD_SetObjectTrans(
		GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );

	{
		BOOL flag = TRUE;
		if( chg_dir == FALSE && FLDMMDL_CheckDrawPause(fmmdl) == TRUE ){
			flag = FALSE;
		}
		GFL_BBDACT_SetAnimeEnable( actSys, work->actID, flag );
	}
}

//--------------------------------------------------------------
/**
 * �`�揈���@�r���{�[�h�@�ėp�@�擾�B
 * �r���{�[�h�A�N�^�[ID��Ԃ��B
 * @param	fmmdl	FLDMMDL
 * @param	state	���ɖ���
 * @retval	u32	GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static u32 DrawBlAct_GetBlActID( FLDMMDL *fmmdl, u32 state )
{
	DRAW_BLACT_WORK *work;
	work = FLDMMDL_GetDrawProcWork( fmmdl );
	return( work->actID );
}

//--------------------------------------------------------------
//	�`�揈���@�r���{�[�h�@�ėp�@�܂Ƃ�
//--------------------------------------------------------------
const FLDMMDL_DRAW_PROC_LIST DATA_FLDMMDL_DRAWPROCLIST_BlAct =
{
	DrawBlAct_Init,
	DrawBlAct_Draw,
	DrawBlAct_Delete,
	DrawBlAct_Delete,	//�{���͑ޔ�
	DrawBlAct_Init,		//�{���͕��A
	DrawBlAct_GetBlActID,
};
