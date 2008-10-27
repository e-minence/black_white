//============================================================================================
/**
 * @file	field_main.c
 * @brief	
 * @author	
 * @date	
 *
 * ���t�@�C�� tetsu/sample/samplemain.c
 */
//============================================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "net/network_define.h"
#include "textprint.h"
#include "arc_def.h"

#include "field_g3d_mapper.h"
#include "field_net.h"

#include "field_main.h"
#include "field_common.h"
#include "field_actor.h"
#include "field_player.h"
#include "field_camera.h"

#include "field_debug.h"

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	���C������
 *
 *
 *
 *
 *
 */
//============================================================================================

static FIELD_SETUP*	SetupGameSystem( HEAPID heapID );
static void				RemoveGameSystem( FIELD_SETUP* gs );
static void				MainGameSystem( FIELD_SETUP* gs );

static void _sendGamePlay( VecFx32* pVec  );



typedef enum {
	GAMEMODE_NORMAL = 0,
	GAMEMODE_FINISH,
}GAMEMODE;

//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
struct _FIELD_MAIN_WORK
{
    VecFx32         recvWork;
	HEAPID			heapID;
	GAMEMODE		gamemode;
	int				seq;
	int				timer;
	
	const DEPEND_FUNCTIONS * ftbl;
	FIELD_SETUP*	gs;
	FIELD_CAMERA*	camera_control;
//	CURSOR_CONT*	cursorFriend;#endif
	PC_ACTCONT*		pcActCont;
//	PC_ACTCONT*		friendActCont;
	FLD_ACTCONT*	fldActCont;
	int				mapNum;
	
	int				key_cont;
	
	int d_menu_flag;
	DEBUG_FLDMENU *d_menu;

	void *pGridCont;
};

//------------------------------------------------------------------
//------------------------------------------------------------------
struct _DEPEND_FUNCTIONS{
	void (*create_func)(FIELD_MAIN_WORK*, VecFx32*, u16);
	void (*main_func)(FIELD_MAIN_WORK*, VecFx32*);
	void (*delete_func)(FIELD_MAIN_WORK*);
};

//------------------------------------------------------------------
/**
 * @brief	���[�J���錾
 */
//------------------------------------------------------------------
static BOOL GameEndCheck( int cont );

FIELD_MAIN_WORK* fieldWork;

static BOOL DebugMenuProc( FIELD_MAIN_WORK *fldWork );

//------------------------------------------------------------------
/**
 * @brief	���[�N�̊m�ۂƔj��
 */
//------------------------------------------------------------------
void	FieldBoot( HEAPID heapID )
{
	fieldWork = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_MAIN_WORK) );
	fieldWork->heapID = heapID;
	fieldWork->gamemode = GAMEMODE_NORMAL;

//	GFL_UI_TP_Init( fieldWork->heapID );
}

void	FieldEnd( void )
{
//	GFL_UI_TP_Exit();

	GFL_HEAP_FreeMemory( fieldWork );
}

//------------------------------------------------------------------
/**
 * @brief	���C��
 */
//------------------------------------------------------------------
BOOL	FieldMain( void )
{
	BOOL return_flag = FALSE,bSkip = FALSE;
	int i;

	fieldWork->timer++;
//	GFL_UI_TP_Main();

	switch( fieldWork->seq ){

	case 0:
		//��{�V�X�e���Z�b�g�A�b�v
		fieldWork->gs = SetupGameSystem( fieldWork->heapID );
		fieldWork->mapNum = 0;
		fieldWork->seq++;
        break;

	case 1:
		fieldWork->ftbl = resistMapTbl[fieldWork->mapNum].dep_funcs;
		{
            //�Z�b�g�A�b�v
            ResistDataFieldG3Dmapper( GetFieldG3Dmapper(fieldWork->gs), 
                                &resistMapTbl[fieldWork->mapNum].mapperData );

			//�o�^�e�[�u�����ƂɌʂ̏������������Ăяo��
			{
				VecFx32 pos;
				u16		dir;

				pos = resistMapTbl[fieldWork->mapNum].startPos;
				dir = 0;
				fieldWork->ftbl->create_func( fieldWork, &pos, dir );
			}
			
			{	//�f�o�b�O���j���[
				fieldWork->d_menu = FldDebugMenu_Init(
						fieldWork, fieldWork->mapNum, fieldWork->heapID );
			}

            fieldWork->seq++;
        }
		break;

	case 2:
		if( GameEndCheck( GFL_UI_KEY_GetCont() ) == TRUE ){
			fieldWork->gamemode = GAMEMODE_FINISH;
			fieldWork->seq = 3;
			break;
		}
		if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_START ){
			fieldWork->mapNum++;
			if( fieldWork->mapNum >= (resistMapTblCount) ){
				fieldWork->mapNum = 0;
			}
			fieldWork->seq = 3;
			break;
		}
#if 0
		if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_SELECT ){
			fieldWork->mapNum--;
			if( fieldWork->mapNum < 0 ){
				fieldWork->mapNum = (resistMapTblCount)-1;
			}
			fieldWork->seq = 3;
			break;
		}
#endif
		
		if( !DebugMenuProc( fieldWork ) ) {
		
			VecFx32 pos;
			fieldWork->key_cont = GFL_UI_KEY_GetCont();
			//�o�^�e�[�u�����ƂɌʂ̃��C���������Ăяo��
			fieldWork->ftbl->main_func( fieldWork, &pos );
			//Map�V�X�e���Ɉʒu��n���Ă���B���ꂪ�Ȃ��ƃ}�b�v�ړ����Ȃ��̂Œ���
			SetPosFieldG3Dmapper( GetFieldG3Dmapper(fieldWork->gs), &pos );
		}

		MainGameSystem( fieldWork->gs );
		break;

	case 3:
		//�o�^�e�[�u�����ƂɌʂ̏I���������Ăяo��
		fieldWork->ftbl->delete_func(fieldWork);

        ReleaseDataFieldG3Dmapper( GetFieldG3Dmapper(fieldWork->gs) );

		if (fieldWork->gamemode != GAMEMODE_FINISH) {
			fieldWork->seq = 1;
		} else {
			fieldWork->seq = 4;
		}
		
		FldDebugMenu_Delete( fieldWork->d_menu );
		break;

	case 4:
		RemoveGameSystem( fieldWork->gs );
		return_flag = TRUE;
		break;
	}
	return return_flag;
}

//------------------------------------------------------------------
/**
 * @brief	�I���`�F�b�N
 */
//------------------------------------------------------------------
static BOOL GameEndCheck( int cont )
{
	int resetCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START;

	if( (cont & resetCont ) == resetCont ){
		return TRUE;
	} else {
		return FALSE;
	}
}





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�Z�b�g�A�b�v
 *
 *
 *
 *
 *
 */
//============================================================================================
//#define BACKGROUND_COL	(GX_RGB(23,29,31))		//�w�i�F
//#define FOG_COL			(GX_RGB(31,31,31))		//�t�H�O�F
#define BACKGROUND_COL	(GX_RGB(30,31,31))		//�w�i�F
#define FOG_COL			(GX_RGB(31,31,31))		//�t�H�O�F
#define DTCM_SIZE		(0x1000)				//DTCM�G���A�̃T�C�Y

//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
struct _FIELD_SETUP {
	GFL_G3D_UTIL*			g3Dutil;		//g3Dutil Lib �n���h��
	u16						g3DutilUnitIdx;	//g3Dutil Unit�C���f�b�N�X
	GFL_G3D_SCENE*			g3Dscene;		//g3Dscene Lib �n���h��
	GFL_G3D_CAMERA*			g3Dcamera;		//g3Dcamera Lib �n���h��
	GFL_G3D_LIGHTSET*		g3Dlightset;	//g3Dlight Lib �n���h��
	GFL_TCB*				g3dVintr;		//3D�pvIntrTask�n���h��
	GFL_BBDACT_SYS*			bbdActSys;		//�r���{�[�h�A�N�g�V�X�e���ݒ�n���h��

	FLD_G3D_MAPPER*				g3Dmapper;

	HEAPID					heapID;
};

//------------------------------------------------------------------
/**
 * @brief	�f�B�X�v���C���f�[�^
 */
//------------------------------------------------------------------
///�u�q�`�l�o���N�ݒ�\����
static const GFL_BG_DISPVRAM dispVram = {
	GX_VRAM_BG_128_D,				//���C��2D�G���W����BG�Ɋ��蓖�� 
	GX_VRAM_BGEXTPLTT_NONE,			//���C��2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_SUB_BG_32_H,			//�T�u2D�G���W����BG�Ɋ��蓖��
	GX_VRAM_SUB_BGEXTPLTT_NONE,		//�T�u2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_OBJ_64_E,				//���C��2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_OBJEXTPLTT_NONE,		//���C��2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_SUB_OBJ_16_I,			//�T�u2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	//�T�u2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_TEX_012_ABC,			//�e�N�X�`���C���[�W�X���b�g�Ɋ��蓖��
	GX_VRAM_TEXPLTT_0_G,			//�e�N�X�`���p���b�g�X���b�g�Ɋ��蓖��
};

//------------------------------------------------------------------
/**
 * @brief	�A�[�J�C�u�e�[�u��
 */
//------------------------------------------------------------------
#include "test_graphic/fld_act.naix"

#if 0
static	const	char	*GraphicFileTable[]={
	"test_graphic/test3d.narc",
	"test_graphic/fld_act.narc",
	"test_graphic/fld_map.narc",
	"test_graphic/sample_map.narc",
};
#endif

//static const char font_path[] = {"gfl_font.dat"};

//------------------------------------------------------------------
/**
 * @brief	�R�c�O���t�B�b�N���f�[�^
 */
//------------------------------------------------------------------
//�J���������ݒ�f�[�^
static const VecFx32 cameraTarget	= { 0, 0, 0 };
static const VecFx32 cameraPos	= { 0, (FX32_ONE * 64), (FX32_ONE * 128) };

//���C�g�����ݒ�f�[�^
static const GFL_G3D_LIGHT_DATA light0Tbl[] = {
	{ 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
};
static const GFL_G3D_LIGHTSET_SETUP light0Setup = { light0Tbl, NELEMS(light0Tbl) };
//------------------------------------------------------------------
/**
 * @brief	�R�c���\�[�X�f�[�^
 */
//------------------------------------------------------------------
#define G3D_FRM_PRI			(1)			//�R�c�ʂ̕`��v���C�I���e�B�[
#define G3D_UTIL_RESCOUNT	(512)		//g3Dutil�Ŏg�p���郊�\�[�X�̍ő�ݒ�\��
#define G3D_UTIL_OBJCOUNT	(128)		//g3Dutil�Ŏg�p����I�u�W�F�N�g�̍ő�ݒ�\��
#define G3D_SCENE_OBJCOUNT	(256)		//g3Dscene�Ŏg�p����sceneObj�̍ő�ݒ�\��
#define G3D_OBJWORK_SZ		(64)		//g3Dscene�Ŏg�p����sceneObj�̃��[�N�T�C�Y
#define G3D_ACC_COUNT		(32)		//g3Dscene�Ŏg�p����sceneObjAccesary�̍ő�ݒ�\��
#define G3D_BBDACT_RESMAX	(64)		//billboardAct�Ŏg�p���郊�\�[�X�̍ő�ݒ�\��
#define G3D_BBDACT_ACTMAX	(256)		//billboardAct�Ŏg�p����I�u�W�F�N�g�̍ő�ݒ�\��

static const GXRgb edgeColorTable[8] = {
	GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),
	GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),
};
//------------------------------------------------------------------
/**
 * @brief	���[�J���錾
 */
//------------------------------------------------------------------
//�a�f�ݒ�֐�
static void	bg_init( FIELD_SETUP* gs );
static void	bg_exit( FIELD_SETUP* gs );
//�R�c�֐�
static void g3d_load( FIELD_SETUP* gs );
static void g3d_control( FIELD_SETUP* gs );
static void g3d_draw( FIELD_SETUP* gs );
static void	g3d_unload( FIELD_SETUP* gs );
static void	g3d_vblank( GFL_TCB* tcb, void* work );

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v�֐�
 */
//------------------------------------------------------------------
static FIELD_SETUP*	SetupGameSystem( HEAPID heapID )
{
	FIELD_SETUP*	gs = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_SETUP) );
	gs->heapID = heapID;

	//����������
	GFL_STD_MtRandInit(0);

	//ARC�V�X�e��������
//	GFL_ARC_Init( &GraphicFileTable[0], NELEMS(GraphicFileTable) );		gfl_use.c��1�񂾂��������ɕύX

	//VRAM�N���A
	GFL_DISP_ClearVRAM( GX_VRAM_D );
	//VRAM�ݒ�
	GFL_DISP_SetBank( &dispVram );

	//BG������
	bg_init( gs );

	//�R�c�f�[�^�̃��[�h
	g3d_load( gs );
	gs->g3dVintr = GFUser_VIntr_CreateTCB( g3d_vblank, (void*)gs, 0 );

	return gs;
}

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v�֐�
 */
//------------------------------------------------------------------
static void	RemoveGameSystem( FIELD_SETUP* gs )
{
	GFL_TCB_DeleteTask( gs->g3dVintr );
	g3d_unload( gs );	//�R�c�f�[�^�j��

	bg_exit( gs );
//	GFL_ARC_Exit();

	GFL_HEAP_FreeMemory( gs );
}

//------------------------------------------------------------------
/**
 * @brief	�V�X�e�����C���֐�
 */
//------------------------------------------------------------------
static void	MainGameSystem( FIELD_SETUP* gs )
{
	g3d_control( gs );
	g3d_draw( gs );
}

//------------------------------------------------------------------
/**
 * @brief		�a�f�ݒ聕�f�[�^�]��
 */
//------------------------------------------------------------------
static void G3DsysSetup( void );
static const GFL_BG_SYS_HEADER bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
};	

static void	bg_init( FIELD_SETUP* gs )
{
	//�t�H���g�ǂݍ���
//	GFL_TEXT_CreateSystem( font_path );
//	GFL_TEXT_CreateSystem( NULL );	//�V�X�e���t�H���g�g�p

	//�a�f�V�X�e���N��
	GFL_BG_Init( gs->heapID );

	//�w�i�F�p���b�g�쐬���]��
	{
		u16* plt = GFL_HEAP_AllocClearMemoryLo( gs->heapID, 16*2 );
		plt[0] = BACKGROUND_COL;
		GFL_BG_LoadPalette( GFL_BG_FRAME0_M, plt, 16*2, 0 );	//���C����ʂ̔w�i�F�]��
		GFL_BG_LoadPalette( GFL_BG_FRAME0_S, plt, 16*2, 0 );	//�T�u��ʂ̔w�i�F�]��
		GFL_HEAP_FreeMemory( plt );
	}
	//�a�f���[�h�ݒ�
	GFL_BG_SetBGMode( &bgsysHeader );

	//�a�f�R���g���[���ݒ�
	G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 16, 8 );

	//�R�c�V�X�e���N��
	GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX384K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K,
						DTCM_SIZE, gs->heapID, G3DsysSetup );
	GFL_BG_SetBGControl3D( G3D_FRM_PRI );

	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );
	GFL_DISP_SetDispOn();
}

static void	bg_exit( FIELD_SETUP* gs )
{
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );

	GFL_G3D_Exit();
	GFL_BG_Exit();
//	GFL_TEXT_DeleteSystem();
}

// �R�c�Z�b�g�A�b�v�R�[���o�b�N
static void G3DsysSetup( void )
{
	// �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
	G3X_SetShading( GX_SHADING_TOON );
	G3X_AntiAlias( TRUE );
	G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
	G3X_AlphaBlend( TRUE );
#if 1
	// �t�H�O�Z�b�g�A�b�v
    {
        u32     fog_table[8];
        int     i;

        //G3X_SetFog(TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x1000, 0x01000 );
        G3X_SetFog(TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0400, 0x00e00 );

        G3X_SetFogColor(FOG_COL, 0);

        for ( i=0; i<8; i++ ){
            fog_table[i] = (u32)(((i*16)<<0) | ((i*16+4)<<8) | ((i*16+8)<<16) | ((i*16+12)<<24));
        }
        G3X_SetFogTable(&fog_table[0]);
	}

#else
	G3X_SetFog( FALSE, 0, 0, 0 );
#endif
	// �N���A�J���[�̐ݒ�
	G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog

	G3X_SetEdgeColorTable( edgeColorTable ); 
	//G3X_EdgeMarking( TRUE );
	G3X_EdgeMarking( FALSE );

	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);
	GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_W );
}


//------------------------------------------------------------------
/**
 * @brief		�R�c�f�[�^���[�h
 */
static void	g3d_trans_BBD( GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz );
//------------------------------------------------------------------
//�쐬
static void g3d_load( FIELD_SETUP* gs )
{
	//�z�u���ݒ�

	//g3Dutil���g�p���z��Ǘ�������
	gs->g3Dutil = GFL_G3D_UTIL_Create( G3D_UTIL_RESCOUNT, G3D_UTIL_OBJCOUNT, gs->heapID );
	//g3Dscene���g�p���Ǘ�������
	gs->g3Dscene = GFL_G3D_SCENE_Create( gs->g3Dutil, 
						G3D_SCENE_OBJCOUNT, G3D_OBJWORK_SZ, G3D_ACC_COUNT, TRUE, gs->heapID );

	gs->g3Dmapper = CreateFieldG3Dmapper( gs->heapID );
	gs->bbdActSys = GFL_BBDACT_CreateSys
					( G3D_BBDACT_RESMAX, G3D_BBDACT_ACTMAX, g3d_trans_BBD, gs->heapID );

	//�J�����쐬
	gs->g3Dcamera = GFL_G3D_CAMERA_CreateDefault( &cameraPos, &cameraTarget, gs->heapID );
	{
		//fx32 far = 1500 << FX32_SHIFT;
		//fx32 far = 4096 << FX32_SHIFT;
		fx32 far = 1024 << FX32_SHIFT;

		GFL_G3D_CAMERA_SetFar( gs->g3Dcamera, &far );
	}
	//���C�g�쐬
	gs->g3Dlightset = GFL_G3D_LIGHT_Create( &light0Setup, gs->heapID );

	//�J�������C�g0���f
	GFL_G3D_CAMERA_Switching( gs->g3Dcamera );
	GFL_G3D_LIGHT_Switching( gs->g3Dlightset );
}
	
//����
static void g3d_control( FIELD_SETUP* gs )
{
	GFL_G3D_SCENE_Main( gs->g3Dscene ); 
	MainFieldG3Dmapper( gs->g3Dmapper );
	GFL_BBDACT_Main( gs->bbdActSys );
}

//�`��
static void g3d_draw( FIELD_SETUP* gs )
{
	GFL_G3D_CAMERA_Switching( gs->g3Dcamera );
	GFL_G3D_LIGHT_Switching( gs->g3Dlightset );
	DrawFieldG3Dmapper( gs->g3Dmapper, gs->g3Dcamera );
	GFL_BBDACT_Draw( gs->bbdActSys, gs->g3Dcamera, gs->g3Dlightset );
	GFL_G3D_SCENE_Draw( gs->g3Dscene );  
}

//�j��
static void g3d_unload( FIELD_SETUP* gs )
{
	GFL_G3D_LIGHT_Delete( gs->g3Dlightset );
	GFL_G3D_CAMERA_Delete( gs->g3Dcamera );

	GFL_BBDACT_DeleteSys( gs->bbdActSys );
	DeleteFieldG3Dmapper( gs->g3Dmapper );

	GFL_G3D_SCENE_Delete( gs->g3Dscene );  
	GFL_G3D_UTIL_Delete( gs->g3Dutil );
}
	
static void	g3d_vblank( GFL_TCB* tcb, void* work )
{
	FIELD_SETUP* gs =  (FIELD_SETUP*)work;
}

//BBD�pVRAM�]���֐�
static void	g3d_trans_BBD( GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz )
{
	NNS_GFD_DST_TYPE transType;

	if( type == GFL_BBDACT_TRANSTYPE_DATA ){
		transType = NNS_GFD_DST_3D_TEX_VRAM;
	} else {
		transType = NNS_GFD_DST_3D_TEX_PLTT;
	}
	NNS_GfdRegisterNewVramTransferTask( transType, dst, (void*)src, siz );
}

//------------------------------------------------------------------
/**
 * @brief	�V�X�e���擾
 */
//------------------------------------------------------------------
GFL_G3D_CAMERA* GetG3Dcamera( FIELD_SETUP* gs )
{
	return gs->g3Dcamera;
}

FLD_G3D_MAPPER* GetFieldG3Dmapper( FIELD_SETUP* gs )
{
	return gs->g3Dmapper;
}

GFL_BBDACT_SYS* GetBbdActSys( FIELD_SETUP* gs )
{
	return gs->bbdActSys;
}
	


//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�R���g���[��
 *
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�ړ������̒n�`�ɉ������x�N�g���擾
 */
//------------------------------------------------------------------
#define WALK_LIMIT_HEIGHT ( 16 * FX32_ONE )

static void GetGroundMoveVec
		( const VecFx16* vecN, const VecFx32* pos, const VecFx32* vecMove, VecFx32* result )
{
	VecFx32	vecN32, posNext;
	fx32	by, valD;

	VEC_Add( pos, vecMove, &posNext );

	VEC_Set( &vecN32, vecN->x, vecN->y, vecN->z );
	valD = -( FX_Mul(vecN32.x,pos->x) + FX_Mul(vecN32.y,pos->y) + FX_Mul(vecN32.z,pos->z) ); 
	by = -( FX_Mul( vecN32.x, posNext.x ) + FX_Mul( vecN32.z, posNext.z ) + valD );
	posNext.y = FX_Div( by, vecN32.y );

	VEC_Subtract( &posNext, pos, result );
	VEC_Normalize( result, result );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL CalcSetGroundMove( const FLD_G3D_MAPPER* g3Dmapper, FLD_G3D_MAPPER_INFODATA* gridInfoData, 
								VecFx32* pos, VecFx32* vecMove, fx32 speed )
{
	FLD_G3D_MAPPER_GRIDINFO gridInfo;
	VecFx32	posNext, vecGround;
	fx32	height = 0;
	BOOL	initSw = FALSE;

	//VEC_Normalize( &vecMove, &vecMove );
	//
	if( (gridInfoData->vecN.x|gridInfoData->vecN.y|gridInfoData->vecN.z) == 0 ){
		//vecN = {0,0,0}�̏ꍇ�͏������
		VecFx16	vecNinit = { 0, FX16_ONE, 0 };

		GetGroundMoveVec( &vecNinit, pos, vecMove, &vecGround );
		initSw = TRUE;
	} else {
		GetGroundMoveVec( &gridInfoData->vecN, pos, vecMove, &vecGround );
	}
	VEC_MultAdd( speed, &vecGround, pos, &posNext );
	if( posNext.y < 0 ){
		posNext.y = 0;	//�x�[�X���C��
	}
	if( CheckFieldG3DmapperOutRange( g3Dmapper, &posNext ) == TRUE ){
		OS_Printf("�}�b�v�͈͊O�ňړ��s��\n");
		return FALSE;
	}

	//�v���[���[�p����B���̈ʒu���S�ɍ����f�[�^�����݂��邽�߁A���ׂĎ擾���Đݒ�
	if( GetFieldG3DmapperGridInfo( g3Dmapper, &posNext, &gridInfo ) == FALSE ){
		return FALSE;
	}

	if( gridInfo.count ){
		int		i = 0, p;
		fx32	h_tmp, diff1, diff2;

		height = gridInfo.gridData[0].height;
		p = 0;
		i++;
		for( ; i<gridInfo.count; i++ ){
			h_tmp = gridInfo.gridData[i].height;

			diff1 = height - pos->y;
			diff2 = h_tmp - pos->y;

			if( FX_Mul( diff2, diff2 ) < FX_Mul( diff1, diff1 ) ){
				height = h_tmp;
				p = i;
			}
		}
#if 0
		if( initSw == FALSE ){
			//�ړ������e�X�g
			if(FX_Mul((height-pos->y),(height-pos->y))
					>=FX_Mul(WALK_LIMIT_HEIGHT,WALK_LIMIT_HEIGHT)){
				return FALSE;
			}
		} 
#endif
		*gridInfoData = gridInfo.gridData[p];	//�O���b�h�f�[�^�X�V
		VEC_Set( pos, posNext.x, gridInfoData->height, posNext.z );		//�ʒu���X�V
	}
	return TRUE;
}
	


//============================================================================================
//============================================================================================
#include "field_sub_normal.c"
#include "field_sub_nogrid.c"
#include "field_sub_grid.c"

const DEPEND_FUNCTIONS FieldBaseFunctions = {
	NormalCreate,
	NormalMain,
	NormalDelete,
};

const DEPEND_FUNCTIONS FieldGridFunctions = {
	GridMoveCreate,
	GridMoveMain,
	GridMoveDelete,
};

const DEPEND_FUNCTIONS FieldNoGridFunctions = {
	NoGridCreate,
	NoGridMain,
	NoGridDelete,
};

//======================================================================
//	debug
//======================================================================
//--------------------------------------------------------------
///	�f�o�b�O���j���[����
//--------------------------------------------------------------
static BOOL DebugMenuProc( FIELD_MAIN_WORK *fldWork )
{
	if( fldWork->d_menu_flag == FALSE ){
		if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_SELECT ){
			FldDebugMenu_Create( fldWork->d_menu );
			fldWork->d_menu_flag = TRUE;
		}
	}else{	//�N����
		if( FldDebugMenu_Main(fldWork->d_menu) == TRUE ){
			fldWork->d_menu_flag = FALSE;
		}
	}
	return fldWork->d_menu_flag;
}
