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

	u16				mouseCursorID;
	VecFx32			mouseCursorPos;
	int				now_icon;

	MOUSE_EVENT		me;
};

typedef struct {
	MOUSE_EVENT		me;
	u8				px;
	u8				py;
	u8				sx;
	u8				sy;
}ICON_AREA;

static const GFL_UI_TP_HITTBL tp_data[] = {
	{ 16, 176-1, 0, 12-1 },		//��
	{ 16, 176-1, 244, 256-1 },	//�E
	{ 0, 12-1, 16, 240-1 },		//��
	{ 180, 192-1, 16, 240-1 },	//��

	{ 160, 176-1, 24, 40-1 },
	{ 160, 176-1, 40, 56-1 },
	{ 160, 176-1, 56, 72-1 },
	{ 160, 176-1, 72, 88-1 },
	{ 160, 176-1, 88, 104-1 },
	{ 160, 176-1, 104, 120-1 },
	{ 160, 176-1, 120, 136-1 },
	{ 160, 176-1, 136, 152-1 },
	{ 160, 176-1, 152, 168-1 },
	{ 160, 176-1, 168, 184-1 },
	{ 160, 176-1, 184, 200-1 },
	{ 160, 176-1, 200, 216-1 },
	{ 160, 176-1, 216, 232-1 },

	{ 16, 32-1, 224, 240-1 },	//���j���[
	{GFL_UI_TP_HIT_END,0,0,0},			//�I���f�[�^
};

static const ICON_AREA area_data[] = {	// tp_data�ƑΉ�
	{ MOUSE_EVENT_CAMERAMOVE_L, 0, 2, 2, 20 },			//��
	{ MOUSE_EVENT_CAMERAMOVE_R, 30, 2, 2, 20 },			//�E
	{ MOUSE_EVENT_CAMERAMOVE_U, 2, 0, 28, 2 },			//��
	{ MOUSE_EVENT_CAMERAMOVE_D, 2, 22, 28, 2 },			//��

	{ MOUSE_EVENT_ACTION_1, 3, 20, 2, 2 },
	{ MOUSE_EVENT_ACTION_2, 5, 20, 2, 2 },
	{ MOUSE_EVENT_ACTION_3, 7, 20, 2, 2 },
	{ MOUSE_EVENT_ACTION_4, 9, 20, 2, 2 },
	{ MOUSE_EVENT_ACTION_5, 11, 20, 2, 2 },
	{ MOUSE_EVENT_ACTION_6, 13, 20, 2, 2 },
	{ MOUSE_EVENT_ACTION_7, 15, 20, 2, 2 },
	{ MOUSE_EVENT_ACTION_8, 17, 20, 2, 2 },
	{ MOUSE_EVENT_ACTION_9, 19, 20, 2, 2 },
	{ MOUSE_EVENT_ACTION_10, 21, 20, 2, 2 },
	{ MOUSE_EVENT_ACTION_11, 23, 20, 2, 2 },
	{ MOUSE_EVENT_ACTION_12, 25, 20, 2, 2 },
	{ MOUSE_EVENT_ACTION_13, 27, 20, 2, 2 },

	{ MOUSE_EVENT_OPEN_MENU, 28, 2, 2, 2 },			//���j���[
	{GFL_UI_TP_HIT_END,0,0,0},			//�I���f�[�^
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
	mes->now_icon = NO_HIT_ICON;
	GFL_BG_LoadScreenReq( PLAYICON_FRM );

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
MOUSE_EVENT MainMouseEvent( MOUSE_EVENT_SYS* mes )
{
	MOUSE_EVENT			me;
	u32					tpx, tpy;
	int					tpContTblPos;
	GFL_G3D_SCENEOBJ*	g3DsceneObj;
	BOOL				cursorDrawSw = FALSE;
	BOOL				scrClearFlag = FALSE;

	g3DsceneObj = GFL_G3D_SCENEOBJ_Get( Get_GS_G3Dscene( mes->gs ), mes->mouseCursorID );

	if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == FALSE ){
		scrClearFlag = TRUE;
		GFL_G3D_SCENEOBJ_SetDrawSW( g3DsceneObj, &cursorDrawSw );
		me = MOUSE_EVENT_NONE;
	} else {
		tpContTblPos = GFL_UI_TP_HitCont( tp_data );

		if( tpContTblPos == GFL_UI_TP_HIT_NONE ){
			scrClearFlag = TRUE;

			if( GetCursorVec( tpx, tpy, &mes->mouseCursorPos ) == TRUE ){
				cursorDrawSw = TRUE;
				GFL_G3D_SCENEOBJ_SetDrawSW( g3DsceneObj, &cursorDrawSw );
				GFL_G3D_SCENEOBJ_SetPos( g3DsceneObj, &mes->mouseCursorPos );
				me = MOUSE_EVENT_MOVE;
			} else {
				GFL_G3D_SCENEOBJ_SetDrawSW( g3DsceneObj, &cursorDrawSw );
				me = MOUSE_EVENT_NONE;
			}
		} else {
			GFL_BG_ChangeScreenPalette( PLAYICON_FRM, 0, 0, 32, 24, PLAYICON_PLTT );
			GFL_BG_ChangeScreenPalette( PLAYICON_FRM,
										area_data[ tpContTblPos ].px,
										area_data[ tpContTblPos ].py,
										area_data[ tpContTblPos ].sx,
										area_data[ tpContTblPos ].sy,
										PLAYICON_PLTT+1 );
			mes->now_icon = tpContTblPos;
			GFL_G3D_SCENEOBJ_SetDrawSW( g3DsceneObj, &cursorDrawSw );
	
			me = area_data[ tpContTblPos ].me;
		}
	}
	mes->me = me;
	if( scrClearFlag == TRUE ){
		if( mes->now_icon != NO_HIT_ICON ){
			mes->now_icon = NO_HIT_ICON;
			GFL_BG_ChangeScreenPalette( PLAYICON_FRM, 0, 0, 32, 24, PLAYICON_PLTT );
			GFL_BG_LoadScreenReq( PLAYICON_FRM );
		}
	} else {
		GFL_BG_LoadScreenReq( PLAYICON_FRM );
	}
	return mes->me;
}

//------------------------------------------------------------------
/**
 * @brief	�}�E�X�C�x���g�擾
 */
//------------------------------------------------------------------
MOUSE_EVENT GetMouseEvent( MOUSE_EVENT_SYS* mes )
{
	return mes->me;
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
	vecN.y = -FX32_ONE;
	vecN.z = 0;

	return GFL_G3D_Calc_GetClossPointRayPlane( &posRay, &vecRay, &posRef, &vecN, cursorPos );
}

