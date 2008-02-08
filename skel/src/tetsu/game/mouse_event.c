//============================================================================================
/**
 * @file	mouse_event.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "ui.h"

#include "setup.h"

#include "mouse_event.h"
//============================================================================================
//
//
//	�}�E�X�C�x���g����
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
#define NO_HIT_ICON (-1)

struct _MOUSE_EVENT_SYS {
	HEAPID			heapID;
	GAME_SYSTEM*	gs;

	u16				g3DutilUnitIdx;
	u16				g3DutilObjIdx;

	u16				mouseActionMode;

	u16				mouseCursorID;
	VecFx32			mouseCursorPos;
	int				eventMode;

	u32				eventFlag;
	BOOL			jumpTrg;
	u32				jumpTrgEnableCount;

	u32				attackFrameCounter;
	BOOL			scrClrFlag;
};

enum {
	EVENT_MODE_NONE = 0,
	EVENT_MODE_MOVE,
	EVENT_MODE_ICON,
	EVENT_MODE_ATTACK,
};

enum {
	MOUSE_ACTION_STOP = 0,
	MOUSE_ACTION_NORMAL,
	MOUSE_ACTION_ATTACK,
};

typedef struct {
	MOUSE_EVENT		me;
	u8				px;
	u8				py;
	u8				sx;
	u8				sy;
	BOOL			moveEnable;
}ICON_AREA;

static const GFL_UI_TP_HITTBL icontp_data[] = {
	{ 16, 176-1, 0, 12-1 },		//��
	{ 16, 176-1, 244, 256-1 },	//�E

	{ 8, 24-1, 24, 40-1 },
	{ 8, 24-1, 40, 56-1 },
	{ 8, 24-1, 56, 72-1 },
	{ 8, 24-1, 72, 88-1 },
	{ 8, 24-1, 88, 104-1 },
	{ 8, 24-1, 104, 120-1 },
	{ 8, 24-1, 120, 136-1 },
	{ 8, 24-1, 136, 152-1 },
	{ 8, 24-1, 152, 168-1 },
	{ 8, 24-1, 168, 184-1 },
	{ 8, 24-1, 184, 200-1 },
	{ 8, 24-1, 200, 216-1 },
	{ 8, 24-1, 216, 232-1 },

	{ 168, 184-1, 224, 240-1 },	//���j���[

	{GFL_UI_TP_HIT_END,0,0,0},			//�I���f�[�^
};

static const ICON_AREA iconarea_data[] = {	// icontp_data�ƑΉ�
	{ MOUSE_EVENT_CAMERAMOVE_L, 0, 2, 2, 20, TRUE },			//��
	{ MOUSE_EVENT_CAMERAMOVE_R, 30, 2, 2, 20, TRUE },			//�E

	{ MOUSE_EVENT_ACTION_1, 3, 1, 2, 2, FALSE },
	{ MOUSE_EVENT_ACTION_2, 5, 1, 2, 2, FALSE },
	{ MOUSE_EVENT_ACTION_3, 7, 1, 2, 2, FALSE },
	{ MOUSE_EVENT_ACTION_4, 9, 1, 2, 2, FALSE },
	{ MOUSE_EVENT_ACTION_5, 11, 1, 2, 2, FALSE },
	{ MOUSE_EVENT_ACTION_6, 13, 1, 2, 2, FALSE },
	{ MOUSE_EVENT_ACTION_7, 15, 1, 2, 2, FALSE },
	{ MOUSE_EVENT_ACTION_8, 17, 1, 2, 2, FALSE },
	{ MOUSE_EVENT_ACTION_9, 19, 1, 2, 2, FALSE },
	{ MOUSE_EVENT_ACTION_10, 21, 1, 2, 2, FALSE },
	{ MOUSE_EVENT_ACTION_11, 23, 1, 2, 2, FALSE },
	{ MOUSE_EVENT_ACTION_12, 25, 1, 2, 2, FALSE },
	{ MOUSE_EVENT_ACTION_13, 27, 1, 2, 2, FALSE },

	{ MOUSE_EVENT_OPEN_MENU, 28, 21, 2, 2, FALSE },			//���j���[
};

static const GFL_UI_TP_HITTBL attacktp_data_trg[] = {
	{ GFL_UI_TP_USE_CIRCLE, 128, 128, 16 },

	{GFL_UI_TP_HIT_END,0,0,0},			//�I���f�[�^
};

static const GFL_UI_TP_HITTBL attacktp_data_direct[] = {
	{ 0, 128-1, 104, 152-1 },			//�O��
	{ 128, 192-1, 104, 152-1 },			//���
	{ 0, 128-1, 0, 128-1 },				//����
	{ 0, 128-1, 128, 256-1 },			//�E��
	{ 128, 192-1, 0, 128-1 },			//����
	{ 128, 192-1, 128, 256-1 },			//�E��

	{GFL_UI_TP_HIT_END,0,0,0},			//�I���f�[�^
};

static const MOUSE_EVENT attack_data[] = {	//attacktp_data_direct�ƑΉ�
	MOUSE_EVENT_ATTACK_1,				//�O��
	MOUSE_EVENT_ATTACK_2,				//���
	MOUSE_EVENT_ATTACK_3,				//����
	MOUSE_EVENT_ATTACK_4,				//�E��
	MOUSE_EVENT_ATTACK_5,				//����
	MOUSE_EVENT_ATTACK_6,				//�E��
};
//------------------------------------------------------------------
/**
 * @brief	�R�c�Z�b�g�A�b�v�f�[�^
 */
//------------------------------------------------------------------
#include "src/sample_graphic/haruka.naix"
static const char g3DarcPath2[] = {"src/sample_graphic/haruka.narc"};

//------------------------------------------------------------------
enum {
	G3DRES_MOUSE_CURSOR = 0,
};

//�R�c�O���t�B�b�N���\�[�X�e�[�u��
static const GFL_G3D_UTIL_RES g3DresTbl[] = {
	{ (u32)g3DarcPath2, NARC_haruka_test_wall_nsbmd, GFL_G3D_UTIL_RESPATH },
};

//------------------------------------------------------------------
enum {
	G3DOBJ_MOUSE_CURSOR = 0,
};

//�R�c�I�u�W�F�N�g��`�e�[�u��
static const GFL_G3D_UTIL_OBJ g3DobjTbl[] = {
	{ G3DRES_MOUSE_CURSOR, 0, G3DRES_MOUSE_CURSOR, NULL, 0 },
};

//------------------------------------------------------------------
//�ݒ�e�[�u���f�[�^
static const GFL_G3D_UTIL_SETUP g3Dsetup = {
	g3DresTbl, NELEMS(g3DresTbl),
	g3DobjTbl, NELEMS(g3DobjTbl),
};

//------------------------------------------------------------------
static const GFL_G3D_SCENEOBJ_DATA mouseCursorData[] = {
	{ 
		G3DOBJ_MOUSE_CURSOR, 0, 0, 24, FALSE, FALSE, 
		{	{ 0, 0, 0 },
			{ FX32_ONE/8 , FX32_ONE/8, FX32_ONE/8 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
};

static void MainMouseEventNormal( MOUSE_EVENT_SYS* mes );
static void MainMouseEventAttack( MOUSE_EVENT_SYS* mes );

static void setMouseEvent( MOUSE_EVENT_SYS* mes, u32 checkEventID );
static void resetMouseEvent( MOUSE_EVENT_SYS* mes, u32 checkEventID );
static void clearMouseEvent( MOUSE_EVENT_SYS* mes );
static void setJumpTrg( MOUSE_EVENT_SYS* mes );
static void resetJumpTrg( MOUSE_EVENT_SYS* mes );
static BOOL checkJumpTrg( MOUSE_EVENT_SYS* mes );

static BOOL GetCursorVec( u32 tpx, u32 tpy, VecFx32* cursorPos );
//------------------------------------------------------------------
/**
 * @brief	�}�E�X�C�x���g����V�X�e���N���ƏI��
 */
//------------------------------------------------------------------
MOUSE_EVENT_SYS* InitMouseEvent( GAME_SYSTEM* gs, HEAPID heapID )
{
	MOUSE_EVENT_SYS* mes = GFL_HEAP_AllocClearMemory( heapID, sizeof(MOUSE_EVENT_SYS) );

	mes->heapID = heapID;
	mes->gs = gs;
	mes->eventMode = EVENT_MODE_NONE;
	GFL_BG_LoadScreenReq( PLAYICON_FRM );

	mes->mouseActionMode = MOUSE_ACTION_STOP;
	mes->scrClrFlag = FALSE;

	resetJumpTrg( mes );
	clearMouseEvent( mes );
	//�R�c�f�[�^�Z�b�g�A�b�v
	{
		GFL_G3D_SCENE* g3Dscene = Get_GS_G3Dscene( mes->gs );
		mes->g3DutilUnitIdx = GFL_G3D_SCENE_AddG3DutilUnit( g3Dscene, &g3Dsetup );
		mes->g3DutilObjIdx = GFL_G3D_SCENE_GetG3DutilUnitObjIdx( g3Dscene, mes->g3DutilUnitIdx );


		mes->mouseCursorID = GFL_G3D_SCENEOBJ_Add( g3Dscene,	
										mouseCursorData, NELEMS(mouseCursorData),
										mes->g3DutilObjIdx );
	}
	return mes;
}

void ExitMouseEvent( MOUSE_EVENT_SYS* mes )
{
	{
		GFL_G3D_SCENE* g3Dscene = Get_GS_G3Dscene( mes->gs );

		GFL_G3D_SCENEOBJ_Remove( g3Dscene, mes->mouseCursorID, NELEMS(mouseCursorData) );
		GFL_G3D_SCENE_DelG3DutilUnit( g3Dscene, mes->g3DutilUnitIdx );
	}

	GFL_HEAP_FreeMemory( mes );
}

//------------------------------------------------------------------
/**
 * @brief	�}�E�X�C�x���g����
 */
//------------------------------------------------------------------
void MainMouseEvent( MOUSE_EVENT_SYS* mes )
{
	switch( mes->mouseActionMode ){
		case MOUSE_ACTION_STOP:
			break;
		case MOUSE_ACTION_NORMAL:
			MainMouseEventNormal( mes );
			break;
		case MOUSE_ACTION_ATTACK:
			MainMouseEventAttack( mes );
			break;
	}
}

//------------------------------------------------------------------
// �ʏ탂�[�h

static void MainMouseEventNormal( MOUSE_EVENT_SYS* mes )
{
	u32					tpx, tpy;
	int					tpContTblPos;
	GFL_G3D_SCENEOBJ*	g3DsceneObj;
	BOOL				cursorDrawSw = FALSE;
	BOOL				scrClearReq = FALSE;

	g3DsceneObj = GFL_G3D_SCENEOBJ_Get( Get_GS_G3Dscene( mes->gs ), mes->mouseCursorID );

	clearMouseEvent( mes );

	if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == FALSE ){
		scrClearReq = TRUE;
		mes->eventMode = EVENT_MODE_NONE;
		GFL_G3D_SCENEOBJ_SetDrawSW( g3DsceneObj, &cursorDrawSw );
		if( checkJumpTrg( mes ) == TRUE ){
			setMouseEvent( mes, MOUSE_EVENT_JUMP );
		}
		resetJumpTrg( mes );
	} else {
		//�U���g���K����
		tpContTblPos = GFL_UI_TP_HitCont( attacktp_data_trg );
		if( tpContTblPos != GFL_UI_TP_HIT_NONE ){
			if( mes->eventMode != EVENT_MODE_MOVE ){
				scrClearReq = TRUE;
				cursorDrawSw = FALSE;
				GFL_G3D_SCENEOBJ_SetDrawSW( g3DsceneObj, &cursorDrawSw );
				resetJumpTrg( mes );	//�W�����v����폜
				clearMouseEvent( mes );	//�ړ�����폜
				mes->eventMode = EVENT_MODE_ATTACK;
				mes->attackFrameCounter = 0;
				mes->mouseActionMode = MOUSE_ACTION_ATTACK;	//�U�����[�h�Ɉڍs
				return;
			}
		}
		//�ړ�����
		if( GetCursorVec( tpx, tpy, &mes->mouseCursorPos ) == TRUE ){
			cursorDrawSw = TRUE;
			GFL_G3D_SCENEOBJ_SetDrawSW( g3DsceneObj, &cursorDrawSw );
			GFL_G3D_SCENEOBJ_SetPos( g3DsceneObj, &mes->mouseCursorPos );
			mes->eventMode = EVENT_MODE_MOVE;
			if( mes->jumpTrg == TRUE ){
				if( mes->jumpTrgEnableCount ){
					mes->jumpTrgEnableCount--;
					setMouseEvent( mes, MOUSE_EVENT_MOVESTART );
				} else {
					setMouseEvent( mes, MOUSE_EVENT_MOVE );
				}
			}else{
				setJumpTrg( mes );
				setMouseEvent( mes, MOUSE_EVENT_MOVESTART );
			}
		}
		//�A�C�R������
		tpContTblPos = GFL_UI_TP_HitCont( icontp_data );
		if( tpContTblPos == GFL_UI_TP_HIT_NONE ){
			scrClearReq = TRUE;
		} else {
			GFL_BG_ChangeScreenPalette( PLAYICON_FRM, 0, 0, 32, 24, PLAYICON_PLTT );
			GFL_BG_ChangeScreenPalette( PLAYICON_FRM,
										iconarea_data[ tpContTblPos ].px,
										iconarea_data[ tpContTblPos ].py,
										iconarea_data[ tpContTblPos ].sx,
										iconarea_data[ tpContTblPos ].sy,
										PLAYICON_PLTT+1 );
			GFL_G3D_SCENEOBJ_SetDrawSW( g3DsceneObj, &cursorDrawSw );
			mes->scrClrFlag = FALSE;
			if( iconarea_data[ tpContTblPos ].moveEnable == FALSE ){
				cursorDrawSw = FALSE;
				GFL_G3D_SCENEOBJ_SetDrawSW( g3DsceneObj, &cursorDrawSw );
				mes->eventMode = EVENT_MODE_ICON;
				resetJumpTrg( mes );	//�W�����v����폜
				clearMouseEvent( mes );	//�ړ�����폜
			}
			setMouseEvent( mes, iconarea_data[ tpContTblPos ].me );
		}
	}
	if( scrClearReq == TRUE ){
		if( mes->scrClrFlag == FALSE ){
			mes->scrClrFlag = TRUE;
			GFL_BG_ChangeScreenPalette( PLAYICON_FRM, 0, 0, 32, 24, PLAYICON_PLTT );
			GFL_BG_LoadScreenReq( PLAYICON_FRM );
		}
	} else {
		GFL_BG_LoadScreenReq( PLAYICON_FRM );
	}
}

//------------------------------------------------------------------
// �U�����[�h
static void MainMouseEventAttack( MOUSE_EVENT_SYS* mes )
{
	u32		tpx, tpy;
	int		tpContTblPos;
	BOOL	tpOnFlag = GFL_UI_TP_GetPointCont( &tpx, &tpy );

	clearMouseEvent( mes );

	if( mes->attackFrameCounter < 4 ){
		//4�̊ԉ������ςȂ����ǂ����m�F
		if( tpOnFlag == FALSE ){
			mes->mouseActionMode = MOUSE_ACTION_NORMAL;	//�ʏ탂�[�h�ɕ��A
		}
		mes->attackFrameCounter++;
		return;
	}
	//4wait������m�F
	tpContTblPos = GFL_UI_TP_HitCont( attacktp_data_direct );
	if( tpContTblPos == GFL_UI_TP_HIT_NONE ){
		tpContTblPos = 0;	//�擾�G���[�̏ꍇ0�ɕ␳
	}
	setMouseEvent( mes, attack_data[ tpContTblPos ] );
	mes->mouseActionMode = MOUSE_ACTION_NORMAL;	//�ʏ탂�[�h�ɕ��A
}


//------------------------------------------------------------------
static void setMouseEvent( MOUSE_EVENT_SYS* mes, u32 checkEventID )
{
	u32 setBit = ( 0x00000001 << checkEventID );

	mes->eventFlag |= setBit;
}

static void resetMouseEvent( MOUSE_EVENT_SYS* mes, u32 checkEventID )
{
	u32 setBit = (( 0x00000001 << checkEventID ) ^0xffffffff );

	mes->eventFlag &= setBit;
}

static void clearMouseEvent( MOUSE_EVENT_SYS* mes )
{
	mes->eventFlag = 0;
}

//------------------------------------------------------------------
static void setJumpTrg( MOUSE_EVENT_SYS* mes )
{
	mes->jumpTrg = TRUE;
	mes->jumpTrgEnableCount = 2;
}

static void resetJumpTrg( MOUSE_EVENT_SYS* mes )
{
	mes->jumpTrg = FALSE;
	mes->jumpTrgEnableCount = 0;
}

static BOOL checkJumpTrg( MOUSE_EVENT_SYS* mes )
{
	if( mes->jumpTrg == TRUE ){
		if( mes->jumpTrgEnableCount ){
			return TRUE;
		}
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�}�E�X�C�x���g�擾�J�n
 */
//------------------------------------------------------------------
void StartMouseEvent( MOUSE_EVENT_SYS* mes )
{
	mes->mouseActionMode = MOUSE_ACTION_NORMAL;
}

//------------------------------------------------------------------
/**
 * @brief	�}�E�X�C�x���g�擾
 */
//------------------------------------------------------------------
BOOL CheckMouseEvent( MOUSE_EVENT_SYS* mes, u32 checkEventID )
{
	u32 chkBit = ( 0x00000001 << checkEventID );

	if( mes->eventFlag & chkBit ){
		return TRUE;
	} else {
		return FALSE;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�}�E�X�ʒu�擾
 */
//------------------------------------------------------------------
void GetMousePos( MOUSE_EVENT_SYS* mes, VecFx32* pos )
{
	*pos = mes->mouseCursorPos;
}

//------------------------------------------------------------------
/**
 * @brief	�}�E�X�J�[�\���n�`�����蔻��
 */
//------------------------------------------------------------------
static BOOL GetCursorVec( u32 tpx, u32 tpy, VecFx32* cursorPos )
{
	VecFx32 posRay, vecRay, posRef, vecN;
	VecFx32 pNear, pFar;
	GFL_G3D_CALC_RESULT result;

	//�^�b�`�p�l�����W�����[���h���W�n�ւ̕ϊ� -1�Ńr���[�|�[�g�O
	if( NNS_G3dScrPosToWorldLine( tpx, tpy, &pNear, &pFar ) == -1 ){
		return FALSE;
	}
	//���C�ʒu�̐ݒ�
	posRay = pNear;
	//���C�i�s�x�N�g���̎Z�o
	VEC_Subtract( &pFar, &pNear, &vecRay );
	VEC_Normalize( &vecRay, &vecRay );	//���K��

	//���_(0,0,0)��ʂ�y=0�����ʂ��b��̒n�`�Ƃ���
	posRef.x = 0;
	posRef.y = 0;
	posRef.z = 0;
	vecN.x = 0;
	vecN.y = FX32_ONE;
	vecN.z = 0;

	result = GFL_G3D_Calc_GetClossPointRayPlane
				( &posRay, &vecRay, &posRef, &vecN, cursorPos, 0 );
	if( result == GFL_G3D_CALC_TRUE ){
		return TRUE;
	}
	return FALSE;
}

