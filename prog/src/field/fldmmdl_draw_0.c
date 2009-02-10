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
	s8 anm_walk_stop_frame;
}DRAW_BLACT_WORK;

//======================================================================
//	proto
//======================================================================

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
#ifdef HEAD3_TEST
	int tbl[] = {
		0,4,4,4,
		4,4,4,4,
		4,4,4,4,
	};
#else
	int tbl[] = {
		 0, 4, 8,12,
		16,20,24,28,
		32,36,40,44
	};
#endif
	VecFx32 pos;
	u16 dir,anm_id,status;
	DRAW_BLACT_WORK *work;
	GFL_BBDACT_SYS *actSys;
	
	work = FLDMMDL_GetDrawProcWork( fmmdl );
	actSys = FLDMMDL_BLACTCONT_GetBbdActSys( FLDMMDL_GetBlActCont(fmmdl) );
	
	dir = FLDMMDL_GetDirDisp( fmmdl );
	status = FLDMMDL_GetDrawStatus( fmmdl );
	anm_id = tbl[status];
	anm_id += dir;
	
	if( work->set_anm_dir != dir ){ //�����X�V
		work->set_anm_dir = dir;
		work->set_anm_status = status;
		GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
	}else if( work->set_anm_status != status ){
		if( work->set_anm_status == DRAW_STA_DASH_4F ){ //�X�e�[�^�X�X�V
			work->anm_walk_stop_frame++;
		}else{
			work->anm_walk_stop_frame = 2;
		}
		
		if( work->anm_walk_stop_frame > 0 ){
			u16 frame = 0;
			work->anm_walk_stop_frame = 0;
			
			switch( work->set_anm_status ){
			case DRAW_STA_WALK_32F:
			case DRAW_STA_WALK_16F:
			case DRAW_STA_WALK_8F:
			case DRAW_STA_WALK_4F:
			case DRAW_STA_WALK_2F:
			case DRAW_STA_DASH_4F:
				if( GFL_BBDACT_GetAnimeFrmIdx(actSys,work->actID) <= 2 ){
					frame = 2;
				}
				break;
			}
			
			GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
			GFL_BBDACT_SetAnimeFrmIdx( actSys, work->actID, frame );
			
			work->set_anm_status = status;
		}
	}
	
	FLDMMDL_GetDrawVectorPos( fmmdl, &pos );
	#ifndef HEAD3_TEST
	pos.y += FX32_ONE * 12;
	#else
	pos.y += FX32_ONE * 7; //3
	#endif
	
	GFL_BBD_SetObjectTrans(
		GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );
	
	if( FLDMMDL_CheckDrawPause(fmmdl) == TRUE ){
		GFL_BBDACT_SetAnimeEnable( actSys, work->actID, TRUE );
	}else{
		GFL_BBDACT_SetAnimeEnable( actSys, work->actID, FALSE );
	}
}

//--------------------------------------------------------------
//	�`�揈���@�r���{�[�h�@���@�@�܂Ƃ�
//--------------------------------------------------------------
const FLDMMDL_DRAW_PROC_LIST DATA_FLDMMDL_DRAWPROCLIST_Hero =
{
	DrawHero_Init,
	DrawHero_Draw,
	DrawHero_Delete,
	FLDMMDL_DrawPushProcDummy,
	FLDMMDL_DrawPopProcDummy,
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
	int tbl[] = {0,4,8,12,16,20};
	VecFx32 pos;
	u16 dir,anm_id,status;
	DRAW_BLACT_WORK *work;
	GFL_BBDACT_SYS *actSys;
	
	work = FLDMMDL_GetDrawProcWork( fmmdl );
	actSys = FLDMMDL_BLACTCONT_GetBbdActSys( FLDMMDL_GetBlActCont(fmmdl) );
	
	dir = FLDMMDL_GetDirDisp( fmmdl );
	status = FLDMMDL_GetDrawStatus( fmmdl );
	anm_id = tbl[status];
	anm_id += dir;
	
	if( work->set_anm_dir != dir ){ //�����X�V
		work->set_anm_dir = dir;
		work->set_anm_status = status;
		GFL_BBDACT_SetAnimeIdx( actSys, work->actID, anm_id );
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
	#ifndef HEAD3_TEST
	pos.y += FX32_ONE * 12;
	#else
	pos.y += FX32_ONE * 7; //3
	#endif
	
	GFL_BBD_SetObjectTrans(
		GFL_BBDACT_GetBBDSystem(actSys), work->actID, &pos );

	if( FLDMMDL_CheckDrawPause(fmmdl) == TRUE ){
		GFL_BBDACT_SetAnimeEnable( actSys, work->actID, TRUE );
	}else{
		GFL_BBDACT_SetAnimeEnable( actSys, work->actID, FALSE );
	}
}

//--------------------------------------------------------------
//	�`�揈���@�r���{�[�h�@�ėp�@�܂Ƃ�
//--------------------------------------------------------------
const FLDMMDL_DRAW_PROC_LIST DATA_FLDMMDL_DRAWPROCLIST_BlAct =
{
	DrawBlAct_Init,
	DrawBlAct_Draw,
	DrawBlAct_Delete,
	FLDMMDL_DrawPushProcDummy,
	FLDMMDL_DrawPopProcDummy,
};
