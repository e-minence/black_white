//============================================================================================
/**
 * @file	samplemain.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "include/system/gfl_use.h"
#include "include/net/network_define.h"
#include "textprint.h"

void	SampleBoot( HEAPID heapID );
void	SampleEnd( void );
BOOL	SampleMain( void );

#include "g3d_mapper.h"
#include "sample_net.h"
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
typedef struct _SAMPLE_SETUP SAMPLE_SETUP;

static SAMPLE_SETUP*	SetupGameSystem( HEAPID heapID );
static void				RemoveGameSystem( SAMPLE_SETUP* gs );
static void				MainGameSystem( SAMPLE_SETUP* gs );
static G3D_MAPPER*		GetG3Dmapper( SAMPLE_SETUP* gs );

//�A�[�J�C�u�h�m�c�d�w
enum {
	ARCID_TEST3D = 0,
	ARCID_FLDACT,
	ARCID_FLDMAP,
	ARCID_SAMPLEMAP,
};

typedef struct _CURSOR_CONT	CURSOR_CONT;
static CURSOR_CONT*		CreateCursor( SAMPLE_SETUP*	gs, HEAPID heapID );
static void				DeleteCursor( CURSOR_CONT* cursor );
static void				MainCursor( CURSOR_CONT* cursor);
static void             FriendCursor( CURSOR_CONT* cursor );
static void				SetCursorTrans( CURSOR_CONT* cursor, const VecFx32* trans );
static void				GetCursorTrans( CURSOR_CONT* cursor, VecFx32* trans );

typedef struct _FLD_ACTSYS		FLD_ACTSYS;
static FLD_ACTSYS*		CreateFieldActSys( SAMPLE_SETUP* gs, HEAPID heapID );
static void				DeleteFieldActSys( FLD_ACTSYS* fldActSys );
static void				MainFieldActSys( FLD_ACTSYS* fldActSys );

static void _sendGamePlay( VecFx32* pVec  );

#include "samplemain.h"

//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
typedef struct {
    VecFx32         recvWork;
	HEAPID			heapID;
	int				seq;
	int				timer;

	SAMPLE_SETUP*	gs;
	CURSOR_CONT*	cursor;
	CURSOR_CONT*	cursorFriend;
	FLD_ACTSYS*		fldActSys;
	int				mapNum;

}SAMPLE_WORK;

//------------------------------------------------------------------
/**
 * @brief	���[�J���錾
 */
//------------------------------------------------------------------
static BOOL GameEndCheck( int cont );

SAMPLE_WORK* sampleWork;

//------------------------------------------------------------------
/**
 * @brief	���[�N�̊m�ۂƔj��
 */
//------------------------------------------------------------------
void	SampleBoot( HEAPID heapID )
{
	sampleWork = GFL_HEAP_AllocClearMemory( heapID, sizeof(SAMPLE_WORK) );
	sampleWork->heapID = heapID;

	GFL_UI_TP_Init( sampleWork->heapID );
}

void	SampleEnd( void )
{
	GFL_UI_TP_Exit();

	GFL_HEAP_FreeMemory( sampleWork );
}

//------------------------------------------------------------------
/**
 * @brief	���C��
 */
static void Debug_Regenerate( void );
//------------------------------------------------------------------
BOOL	SampleMain( void )
{
	BOOL return_flag = FALSE,bSkip = FALSE;
	int i;

	sampleWork->timer++;
	GFL_UI_TP_Main();

	switch( sampleWork->seq ){

	case 0:
		//��{�V�X�e���Z�b�g�A�b�v
		sampleWork->gs = SetupGameSystem( sampleWork->heapID );
		sampleWork->mapNum = 0;
		sampleWork->seq++;
#ifdef NET_WORK_ON
        InitSampleGameNet();
#endif
        break;

	case 1:
#ifdef NET_WORK_ON
        bSkip = ConnectSampleGameNet();  // �ʐM����
        if( !bSkip && GFL_UI_KEY_GetTrg() ){  // �L�[�������ꂽ��ʐM��҂����ɊJ�n
            ExitSampleGameNet();
            bSkip = TRUE;
        }
#else
        bSkip = TRUE;
#endif
		if( bSkip ){
            //�Z�b�g�A�b�v
#ifdef NET_WORK_ON
            sampleWork->cursorFriend = CreateCursor( sampleWork->gs, sampleWork->heapID );
#endif
            sampleWork->cursor = CreateCursor( sampleWork->gs, sampleWork->heapID );
            sampleWork->fldActSys = CreateFieldActSys( sampleWork->gs, sampleWork->heapID );

            ResistData3Dmapper( GetG3Dmapper(sampleWork->gs), 
                                &resistMapTbl[sampleWork->mapNum].mapperData );
            {
                G3D_MAPPEROBJ_RESIST resistData;
                
                resistData.arcID = ARCID_SAMPLEMAP;
                resistData.data = resistObjTbl; 
                resistData.count = NELEMS(resistObjTbl); 
                ResistObjRes3Dmapper( GetG3Dmapper(sampleWork->gs), &resistData );
            }
#ifdef NET_WORK_ON
            SetCursorTrans( sampleWork->cursorFriend, &resistMapTbl[sampleWork->mapNum].startPos );
            GFL_NET_ReloadIcon();
#endif
            SetCursorTrans( sampleWork->cursor, &resistMapTbl[sampleWork->mapNum].startPos );
            sampleWork->seq++;
        }
		break;

	case 2:
		if( GameEndCheck( GFL_UI_KEY_GetCont() ) == TRUE ){
			sampleWork->seq = 4;
			break;
		}
		if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_START ){
			sampleWork->mapNum++;
			if( sampleWork->mapNum >= NELEMS(resistMapTbl) ){
				sampleWork->mapNum = 0;
			}
			sampleWork->seq = 3;
			break;
		}
		if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_SELECT ){
			sampleWork->mapNum--;
			if( sampleWork->mapNum < 0 ){
				sampleWork->mapNum = NELEMS(resistMapTbl)-1;
			}
			sampleWork->seq = 3;
			break;
		}
		{
			VecFx32 pos;

			MainCursor( sampleWork->cursor );
#ifdef NET_WORK_ON
			FriendCursor( sampleWork->cursorFriend );
#endif
			GetCursorTrans( sampleWork->cursor, &pos );
			SetPos3Dmapper( GetG3Dmapper(sampleWork->gs), &pos );
			MainFieldActSys( sampleWork->fldActSys );
		}
		MainGameSystem( sampleWork->gs );
		break;

	case 3:
		ReleaseObjRes3Dmapper( GetG3Dmapper(sampleWork->gs) );
		DeleteFieldActSys( sampleWork->fldActSys );
		DeleteCursor( sampleWork->cursor );
#ifdef NET_WORK_ON
		DeleteCursor( sampleWork->cursorFriend );
#endif
        sampleWork->seq = 1;
		break;

	case 4:
		ReleaseObjRes3Dmapper( GetG3Dmapper(sampleWork->gs) );
		DeleteFieldActSys( sampleWork->fldActSys );
		DeleteCursor( sampleWork->cursor );
		RemoveGameSystem( sampleWork->gs );
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
struct _SAMPLE_SETUP {
	GFL_G3D_UTIL*			g3Dutil;		//g3Dutil Lib �n���h��
	u16						g3DutilUnitIdx;	//g3Dutil Unit�C���f�b�N�X
	GFL_G3D_SCENE*			g3Dscene;		//g3Dscene Lib �n���h��
	GFL_G3D_CAMERA*			g3Dcamera;		//g3Dcamera Lib �n���h��
	GFL_G3D_LIGHTSET*		g3Dlightset;	//g3Dlight Lib �n���h��
	GFL_TCB*				g3dVintr;		//3D�pvIntrTask�n���h��
	GFL_BBDACT_SYS*			bbdActSys;		//�r���{�[�h�A�N�g�V�X�e���ݒ�n���h��

	G3D_MAPPER*				g3Dmapper;

	HEAPID					heapID;
};

//------------------------------------------------------------------
/**
 * @brief	�f�B�X�v���C���f�[�^
 */
//------------------------------------------------------------------
///�u�q�`�l�o���N�ݒ�\����
static const GFL_BG_DISPVRAM dispVram = {
	GX_VRAM_BG_16_F,				//���C��2D�G���W����BG�Ɋ��蓖�� 
	GX_VRAM_BGEXTPLTT_NONE,			//���C��2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_SUB_BG_128_C,			//�T�u2D�G���W����BG�Ɋ��蓖��
	GX_VRAM_SUB_BGEXTPLTT_NONE,		//�T�u2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_OBJ_64_E,				//���C��2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_OBJEXTPLTT_NONE,		//���C��2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_SUB_OBJ_16_I,			//�T�u2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	//�T�u2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_TEX_01_AB,				//�e�N�X�`���C���[�W�X���b�g�Ɋ��蓖��
	GX_VRAM_TEXPLTT_0_G,			//�e�N�X�`���p���b�g�X���b�g�Ɋ��蓖��
};

//------------------------------------------------------------------
/**
 * @brief	�A�[�J�C�u�e�[�u��
 */
//------------------------------------------------------------------
static	const	char	*GraphicFileTable[]={
	"test_graphic/test3d.narc",
	"test_graphic/fld_act.narc",
	"test_graphic/fld_map.narc",
	"test_graphic/sample_map.narc",
};

static const char font_path[] = {"gfl_font.dat"};
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
static void	bg_init( SAMPLE_SETUP* gs );
static void	bg_exit( SAMPLE_SETUP* gs );
//�R�c�֐�
static void g3d_load( SAMPLE_SETUP* gs );
static void g3d_control( SAMPLE_SETUP* gs );
static void g3d_draw( SAMPLE_SETUP* gs );
static void	g3d_unload( SAMPLE_SETUP* gs );
static void	g3d_vblank( GFL_TCB* tcb, void* work );

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v�֐�
 */
//------------------------------------------------------------------
static SAMPLE_SETUP*	SetupGameSystem( HEAPID heapID )
{
	SAMPLE_SETUP*	gs = GFL_HEAP_AllocClearMemory( heapID, sizeof(SAMPLE_SETUP) );
	gs->heapID = heapID;

	//����������
	GFL_STD_MtRandInit(0);

	//ARC�V�X�e��������
	GFL_ARC_Init( &GraphicFileTable[0], NELEMS(GraphicFileTable) );

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
static void	RemoveGameSystem( SAMPLE_SETUP* gs )
{
	GFL_TCB_DeleteTask( gs->g3dVintr );
	g3d_unload( gs );	//�R�c�f�[�^�j��

	bg_exit( gs );
	GFL_ARC_Exit();

	GFL_HEAP_FreeMemory( gs );
}

//------------------------------------------------------------------
/**
 * @brief	�V�X�e�����C���֐�
 */
//------------------------------------------------------------------
static void	MainGameSystem( SAMPLE_SETUP* gs )
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

static void	bg_init( SAMPLE_SETUP* gs )
{
	//�t�H���g�ǂݍ���
	GFL_TEXT_CreateSystem( font_path );

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
	GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K,
						DTCM_SIZE, gs->heapID, G3DsysSetup );
	GFL_BG_SetBGControl3D( G3D_FRM_PRI );

	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );
	GFL_DISP_SetDispOn();
}

static void	bg_exit( SAMPLE_SETUP* gs )
{
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );

	GFL_G3D_Exit();
	GFL_BG_Exit();
	GFL_TEXT_DeleteSystem();
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
static void g3d_load( SAMPLE_SETUP* gs )
{
	//�z�u���ݒ�

	//g3Dutil���g�p���z��Ǘ�������
	gs->g3Dutil = GFL_G3D_UTIL_Create( G3D_UTIL_RESCOUNT, G3D_UTIL_OBJCOUNT, gs->heapID );
	//g3Dscene���g�p���Ǘ�������
	gs->g3Dscene = GFL_G3D_SCENE_Create( gs->g3Dutil, 
						G3D_SCENE_OBJCOUNT, G3D_OBJWORK_SZ, G3D_ACC_COUNT, TRUE, gs->heapID );

	gs->g3Dmapper = Create3Dmapper( gs->heapID );
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
static void g3d_control( SAMPLE_SETUP* gs )
{
	GFL_G3D_SCENE_Main( gs->g3Dscene ); 
	Main3Dmapper( gs->g3Dmapper );
	GFL_BBDACT_Main( gs->bbdActSys );
}

//�`��
static void g3d_draw( SAMPLE_SETUP* gs )
{
	GFL_G3D_CAMERA_Switching( gs->g3Dcamera );
	GFL_G3D_LIGHT_Switching( gs->g3Dlightset );
	Draw3Dmapper( gs->g3Dmapper, gs->g3Dcamera );
	GFL_BBDACT_Draw( gs->bbdActSys, gs->g3Dcamera, gs->g3Dlightset );
	GFL_G3D_SCENE_Draw( gs->g3Dscene );  
}

//�j��
static void g3d_unload( SAMPLE_SETUP* gs )
{
	GFL_G3D_LIGHT_Delete( gs->g3Dlightset );
	GFL_G3D_CAMERA_Delete( gs->g3Dcamera );

	GFL_BBDACT_DeleteSys( gs->bbdActSys );
	Delete3Dmapper( gs->g3Dmapper );

	GFL_G3D_SCENE_Delete( gs->g3Dscene );  
	GFL_G3D_UTIL_Delete( gs->g3Dutil );
}
	
static void	g3d_vblank( GFL_TCB* tcb, void* work )
{
	SAMPLE_SETUP* gs =  (SAMPLE_SETUP*)work;
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
static GFL_G3D_CAMERA* GetG3Dcamera( SAMPLE_SETUP* gs )
{
	return gs->g3Dcamera;
}

static G3D_MAPPER* GetG3Dmapper( SAMPLE_SETUP* gs )
{
	return gs->g3Dmapper;
}

static GFL_BBDACT_SYS* GetBbdActSys( SAMPLE_SETUP* gs )
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
//============================================================================================
/**
 * @brief	�J�[�\��
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�^�錾
 */
//------------------------------------------------------------------
struct _CURSOR_CONT {
	HEAPID				heapID;
	SAMPLE_SETUP*		gs;
	u16					unitIdx;
	u16					resIdx;
	u16					objIdx;

	u16					cursorIdx;
	VecFx32				trans;
	fx32				cameraHeight;
	u16					cameraLength;
	u16					direction;
};

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
#include "graphic_data/test_graphic/test3d.naix"

enum {
	G3DRES_CURSOR = 0,
};
//�R�c�O���t�B�b�N���\�[�X�e�[�u��
static const GFL_G3D_UTIL_RES cursor_g3Dutil_resTbl[] = {
	{ ARCID_TEST3D, NARC_haruka_test_wall_nsbmd, GFL_G3D_UTIL_RESARC },
};

enum {
	G3DOBJ_CURSOR = 0,
};
//�R�c�I�u�W�F�N�g��`�e�[�u��
static const GFL_G3D_UTIL_OBJ cursor_g3Dutil_objTbl[] = {
	{ G3DRES_CURSOR, 0, G3DRES_CURSOR, NULL, 0 },
};

//---------------------
//g3Dscene �����ݒ�e�[�u���f�[�^
static const GFL_G3D_UTIL_SETUP cursor_g3Dsetup = {
	cursor_g3Dutil_resTbl, NELEMS(cursor_g3Dutil_resTbl),
	cursor_g3Dutil_objTbl, NELEMS(cursor_g3Dutil_objTbl),
};

static const GFL_G3D_SCENEOBJ_DATA cursorObject[] = {
	{	G3DOBJ_CURSOR, 0, 1, 16, TRUE, TRUE,
		{	{ 0, 0, 0 },
			//{ FX32_ONE/8, FX32_ONE/8, FX32_ONE/8 },
			{ FX32_ONE/64, FX32_ONE/64, FX32_ONE/64 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
};

#define	CAMERA_LENGTH	(16)
//------------------------------------------------------------------
/**
 * @brief	������
 */
//------------------------------------------------------------------
static CURSOR_CONT*	CreateCursor( SAMPLE_SETUP*	gs, HEAPID heapID )
{
	CURSOR_CONT* cursor = GFL_HEAP_AllocClearMemory( heapID, sizeof(CURSOR_CONT) );

	cursor->heapID = heapID;
	cursor->gs = gs;

	VEC_Set( &cursor->trans, 0, 0, 0 );
	cursor->cameraHeight = 0;
	cursor->cameraLength = CAMERA_LENGTH;
	cursor->direction = 0;

	//�R�c�f�[�^�Z�b�g�A�b�v
	cursor->unitIdx = GFL_G3D_SCENE_AddG3DutilUnit( cursor->gs->g3Dscene, &cursor_g3Dsetup );
	cursor->resIdx = GFL_G3D_SCENE_GetG3DutilUnitResIdx( cursor->gs->g3Dscene, cursor->unitIdx );
	cursor->objIdx = GFL_G3D_SCENE_GetG3DutilUnitObjIdx( cursor->gs->g3Dscene, cursor->unitIdx );
	cursor->cursorIdx = GFL_G3D_SCENEOBJ_Add(	cursor->gs->g3Dscene, 
												cursorObject, NELEMS(cursorObject),
												cursor->objIdx );
	
	GFL_G3D_SCENEOBJ_SetPos(	GFL_G3D_SCENEOBJ_Get(cursor->gs->g3Dscene, cursor->cursorIdx),
								&cursor->trans );
	return cursor;
}

//------------------------------------------------------------------
/**
 * @brief	�I��
 */
//------------------------------------------------------------------
static void	DeleteCursor( CURSOR_CONT* cursor )
{
	GFL_G3D_SCENEOBJ_Remove( cursor->gs->g3Dscene, cursor->cursorIdx, NELEMS(cursorObject) ); 
	GFL_G3D_SCENE_DelG3DutilUnit( cursor->gs->g3Dscene, cursor->unitIdx );

	GFL_HEAP_FreeMemory( cursor );
}

//------------------------------------------------------------------
/**
 * @brief	���C��
 */
//------------------------------------------------------------------
#define MV_SPEED		(2)
#define RT_SPEED		(FX32_ONE/8)
#define	CAMERA_TARGET_HEIGHT	(4)//(8)
static void	MainCursor( CURSOR_CONT* cursor )
{
	VecFx32	pos, target;
	VecFx32	vecMove = { 0, 0, 0 };
	VecFx32	vecUD = { 0, 0, 0 };
	int		key;
	BOOL	mvFlag = FALSE;

	key = GFL_UI_KEY_GetCont();

	if( key & PAD_BUTTON_B ){
		if( cursor->cameraLength > 8 ){
			cursor->cameraLength -= 8;
		}
		//vecMove.y = -MV_SPEED * FX32_ONE;
	}
	if( key & PAD_BUTTON_A ){
		if( cursor->cameraLength < 4096 ){
			cursor->cameraLength += 8;
		}
		//vecMove.y = MV_SPEED * FX32_ONE;
	}
	if( key & PAD_BUTTON_Y ){
		cursor->cameraHeight -= MV_SPEED * FX32_ONE;
	}
	if( key & PAD_BUTTON_X ){
		cursor->cameraHeight += MV_SPEED * FX32_ONE;
	}
	if( key & PAD_KEY_UP ){
		mvFlag = TRUE;
		vecMove.x = MV_SPEED * FX_SinIdx( (u16)(cursor->direction + 0x8000) );
		vecMove.z = MV_SPEED * FX_CosIdx( (u16)(cursor->direction + 0x8000) );
	}
	if( key & PAD_KEY_DOWN ){
		mvFlag = TRUE;
		vecMove.x = MV_SPEED * FX_SinIdx( (u16)(cursor->direction + 0x0000) );
		vecMove.z = MV_SPEED * FX_CosIdx( (u16)(cursor->direction + 0x0000) );
	}
	if( key & PAD_KEY_LEFT ){
		mvFlag = TRUE;
		vecMove.x = MV_SPEED * FX_SinIdx( (u16)(cursor->direction + 0xc000) );
		vecMove.z = MV_SPEED * FX_CosIdx( (u16)(cursor->direction + 0xc000) );
	}
	if( key & PAD_KEY_RIGHT ){
		mvFlag = TRUE;
		vecMove.x = MV_SPEED * FX_SinIdx( (u16)(cursor->direction + 0x4000) );
		vecMove.z = MV_SPEED * FX_CosIdx( (u16)(cursor->direction + 0x4000) );
	}
	//vecMove.x /= 2;
	//vecMove.z /= 2;
    
	if( key & PAD_BUTTON_R ){
		cursor->direction -= RT_SPEED;
	}
	if( key & PAD_BUTTON_L ){
		cursor->direction += RT_SPEED;
	}
	{
		VecFx32	moveData;

		//Get3DmapperGroundMoveVec
		//			( GetG3Dmapper( cursor->gs ), &cursor->trans, &vecMove, &moveData );
		//VEC_Add( &cursor->trans, &moveData, &cursor->trans );
		VEC_Add( &cursor->trans, &vecMove, &cursor->trans );
		//OS_Printf("vecMove.y  = %x, ", moveData.y);
		//Get3DmapperHeight_fromROM( GetG3Dmapper( cursor->gs ), &cursor->trans, &cursor->trans.y );
		Get3DmapperHeight( GetG3Dmapper( cursor->gs ), &cursor->trans, &cursor->trans.y );
		//OS_Printf("hit blockIdx = %x\n", debugData);
		Reload3DmapperGridInfo( GetG3Dmapper( cursor->gs ), &cursor->trans );
	}
	cursor->cameraHeight += vecMove.y;

#ifdef NET_WORK_ON
    _sendGamePlay( &cursor->trans  );  // �����̈ʒu�𑊎�ɑ��M
#endif
    
	VEC_Set(&target,cursor->trans.x,cursor->trans.y+CAMERA_TARGET_HEIGHT*FX32_ONE,cursor->trans.z);
	pos.x = cursor->trans.x + cursor->cameraLength * FX_SinIdx(cursor->direction);
	pos.y = cursor->trans.y + cursor->cameraHeight;
	pos.z = cursor->trans.z + cursor->cameraLength * FX_CosIdx(cursor->direction);

	GFL_G3D_SCENEOBJ_SetPos(	GFL_G3D_SCENEOBJ_Get(cursor->gs->g3Dscene, cursor->cursorIdx), 
								&cursor->trans );
	GFL_G3D_CAMERA_SetTarget( cursor->gs->g3Dcamera, &target );
	GFL_G3D_CAMERA_SetPos( cursor->gs->g3Dcamera, &pos );

}



static void	FriendCursor( CURSOR_CONT* cursor )
{
    
    GFL_STD_MemCopy((const void*)&sampleWork->recvWork ,&cursor->trans, sizeof(VecFx32));
	GFL_G3D_SCENEOBJ_SetPos(	GFL_G3D_SCENEOBJ_Get(cursor->gs->g3Dscene, cursor->cursorIdx), 
								&cursor->trans );
}

//------------------------------------------------------------------
/**
 * @brief	
 */
//------------------------------------------------------------------
static void	SetCursorTrans( CURSOR_CONT* cursor, const VecFx32* trans )
{
	cursor->trans = *trans;
}
static void	GetCursorTrans( CURSOR_CONT* cursor, VecFx32* trans )
{
	*trans = cursor->trans;
}

//============================================================================================
/**
 * @brief	�t�B�[���h�A�N�^�[
 */
//============================================================================================
//------------------------------------------------------------------
/**
 *
 * @brief	�^�錾
 *
 */
//------------------------------------------------------------------
#define FLD_BBDACT_ACTMAX	(253)
#define WORK_SIZ			(8)

typedef struct {
	FLD_ACTSYS*	fldActSys;
	u16			work[WORK_SIZ];
}FLD_ACT_WORK;

struct _FLD_ACTSYS {
	HEAPID					heapID;
	SAMPLE_SETUP*			gs;
	u16						cameraRotate;
	GFL_BBDACT_RESUNIT_ID	bbdActResUnitID;
	u16						bbdActResCount;
	GFL_BBDACT_ACTUNIT_ID	bbdActActUnitID;
	u16						bbdActActCount;
	FLD_ACT_WORK			actWork[FLD_BBDACT_ACTMAX];
};

#include "graphic_data/test_graphic/fld_act.naix"

static void	initActWork( FLD_ACTSYS* fldActSys, FLD_ACT_WORK* actWork );
static void	calcCameraRotate( FLD_ACTSYS* fldActSys );
//------------------------------------------------------------------
/**
 *
 * @brief	�Z�b�g�A�b�v�f�[�^
 *
 */
//------------------------------------------------------------------
static const GFL_BBDACT_RESDATA testResTable[] = {
	{ ARCID_FLDACT, NARC_fld_act_hero_nsbtx, 
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDACT, NARC_fld_act_achamo_nsbtx, 
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDACT, NARC_fld_act_artist_nsbtx, 
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDACT, NARC_fld_act_badman_nsbtx, 
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDACT, NARC_fld_act_beachgirl_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDACT, NARC_fld_act_idol_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDACT, NARC_fld_act_lady_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDACT, NARC_fld_act_oldman1_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDACT, NARC_fld_act_policeman_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDACT, NARC_fld_act_rivel_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDACT, NARC_fld_act_waiter_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
};

 

static const GFL_BBDACT_ANM walkLAnm[] = {
	{ 2, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 1, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 2, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 3, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM walkRAnm[] = {
	{ 2, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 1, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 2, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 3, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM walkUAnm[] = {
	{ 0, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 7, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 0, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 8, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM walkDAnm[] = {
	{ 9, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 10, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 9, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 11, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM testAnm1[] = {
	{ 0, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 1, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 2, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 3, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 4, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 5, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 6, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 7, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 8, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 9, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 10, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 11, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM testAnm2[] = {
	{ 1, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 2, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 16 },
	{ 3, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 24 },
	{ 4, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 32 },
	{ 5, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 48 },
	{ 6, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 7, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 8, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 9, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 10, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ GFL_BBDACT_ANMCOM_LOOP, 0, 0, GFL_BBDACT_ANMLOOPPARAM( 4, 5 ) },
	{ 11, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 64 },
	{ GFL_BBDACT_ANMCOM_CHG, 0, 0, 0 },
};
static const GFL_BBDACT_ANM testAnm3[] = {
	{ 0, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 16, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 32, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 64, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 1 },
};

enum {
	ACTWALK_UP = 0,
	ACTWALK_DOWN,
	ACTWALK_LEFT,
	ACTWALK_RIGHT,
};

static const GFL_BBDACT_ANM* testAnmTable[] = { walkUAnm, walkDAnm, walkLAnm, walkRAnm };

static void testFunc( GFL_BBDACT_SYS* bbdActSys, int actIdx, void* work )
{
	GFL_BBD_SYS*	bbdSys = GFL_BBDACT_GetBBDSystem( bbdActSys );
	FLD_ACTSYS*		fldActSys = (FLD_ACTSYS*)work;
	FLD_ACT_WORK*	actWork = &fldActSys->actWork[actIdx];

	if( actWork->work[0] == 0 ){
		u32	timer = GFUser_GetPublicRand(8);

		actWork->work[0] = (timer+2) * 30;
		actWork->work[1] = GFUser_GetPublicRand( 0x10000 );
	} else {
		actWork->work[0]--;
	}
	{
		VecFx32 nowTrans, nextTrans, vecMove, vecGround, rotVec;
		u16	 theta = actWork->work[1];
		fx32 speed = FX32_ONE;

		{
			u16	 dir = actWork->work[1] - actWork->fldActSys->cameraRotate;
			if( (dir > 0x2000)&&(dir < 0x6000)){
				//OS_Printf("�E�@rotate = %x\n", dir );
				GFL_BBDACT_SetAnimeIdxContinue( bbdActSys, actIdx, ACTWALK_RIGHT );
			} else if( (dir >= 0x6000)&&(dir <= 0xa000)){
				//OS_Printf("��@rotate = %x\n", dir );
				GFL_BBDACT_SetAnimeIdxContinue( bbdActSys, actIdx, ACTWALK_UP );
			} else if( (dir > 0xa000)&&(dir < 0xe000)){
				//OS_Printf("���@rotate = %x\n", dir );
				GFL_BBDACT_SetAnimeIdxContinue( bbdActSys, actIdx, ACTWALK_LEFT );
			} else {
				//OS_Printf("���@rotate = %x\n", dir );
				GFL_BBDACT_SetAnimeIdxContinue( bbdActSys, actIdx, ACTWALK_DOWN );
			}
		}
		GFL_BBD_GetObjectTrans( bbdSys, actIdx, &nowTrans );

		vecMove.x = FX_SinIdx( theta );
		vecMove.y = 0;
		vecMove.z = FX_CosIdx( theta );
		{
			VecFx32 mv;

			Get3DmapperGroundMoveVec
				( GetG3Dmapper( fldActSys->gs ), &nowTrans, &vecMove, &mv );
			VEC_Add( &nowTrans, &mv, &nextTrans );
		}
		//�͈͊O�R���g���[��
		if( Check3DmapperOutRange( GetG3Dmapper( fldActSys->gs ), &nextTrans ) == FALSE ){
			Get3DmapperHeight( GetG3Dmapper( fldActSys->gs ), &nextTrans, &nextTrans.y );
			nextTrans.y += FX32_ONE*7;	//�␳
			GFL_BBD_SetObjectTrans( bbdSys, actIdx, &nextTrans );
		} else {
			actWork->work[0] = 0;
		}
	}
}

#define TEST_NPC_SETNUM	(250)
static void testSetUp( FLD_ACTSYS* fldActSys )
{
	GFL_BBDACT_SYS* bbdActSys = GetBbdActSys( fldActSys->gs );	
	GFL_BBDACT_ACTDATA* actData;
	GFL_BBDACT_ACTUNIT_ID actUnitID;
	int		i, objIdx;
	VecFx32	trans;
	u8		alpha;
	BOOL	drawEnable;
	u16		setActNum;

	//���\�[�X�Z�b�g�A�b�v
	fldActSys->bbdActResCount = NELEMS(testResTable);
	fldActSys->bbdActResUnitID = GFL_BBDACT_AddResourceUnit(	bbdActSys, 
																testResTable, 
																fldActSys->bbdActResCount );
	fldActSys->bbdActActCount = FLD_BBDACT_ACTMAX;

	//�m�o�b�A�N�^�[�Z�b�g�A�b�v
	{
		u16	setActNum = TEST_NPC_SETNUM;
		GFL_BBDACT_ACTDATA* actData = GFL_HEAP_AllocClearMemory( fldActSys->heapID,
													setActNum*sizeof(GFL_BBDACT_ACTDATA) );
		for( i=0; i<setActNum; i++ ){
			actData[i].resID = GFUser_GetPublicRand( 10 )+1;
			actData[i].sizX = FX16_ONE*8-1;
			actData[i].sizY = FX16_ONE*8-1;
	
			actData[i].trans.x = (GFUser_GetPublicRand(32*4) ) * MAP_WIDTH/32;
			actData[i].trans.y = 0;
			actData[i].trans.z = (GFUser_GetPublicRand(32*4) ) * MAP_WIDTH/32;
			
			actData[i].alpha = 31;
			actData[i].drawEnable = TRUE;
			actData[i].lightMask = GFL_BBD_LIGHTMASK_01;
			actData[i].func = testFunc;
			actData[i].work = fldActSys;
		}
		fldActSys->bbdActActUnitID = GFL_BBDACT_AddAct(	bbdActSys, fldActSys->bbdActResUnitID,
														actData, setActNum );
		for( i=0; i<setActNum; i++ ){
			GFL_BBDACT_SetAnimeTable( bbdActSys, fldActSys->bbdActActUnitID+i, 
										testAnmTable, NELEMS(testAnmTable) );
			GFL_BBDACT_SetAnimeIdxOn( bbdActSys, fldActSys->bbdActActUnitID+i, 0 );
		}
		GFL_HEAP_FreeMemory( actData );
	}
}

static void testRelease( FLD_ACTSYS* fldActSys )
{
	GFL_BBDACT_SYS* bbdActSys = GetBbdActSys( fldActSys->gs );	
	u16	setActNum = FLD_BBDACT_ACTMAX;

	GFL_BBDACT_RemoveAct( bbdActSys, fldActSys->bbdActActUnitID, TEST_NPC_SETNUM );
		
	GFL_BBDACT_RemoveResourceUnit(	bbdActSys, 
									fldActSys->bbdActResUnitID, fldActSys->bbdActResCount );
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�A�N�g�V�X�e���쐬
 */
//------------------------------------------------------------------
static FLD_ACTSYS*	CreateFieldActSys( SAMPLE_SETUP* gs, HEAPID heapID )
{
	FLD_ACTSYS* fldActSys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLD_ACTSYS) );
	int	i;

	fldActSys->heapID = heapID;
	fldActSys->gs = gs;

	for( i=0; i<FLD_BBDACT_ACTMAX; i++ ){ initActWork( fldActSys, &fldActSys->actWork[i] ); }

	testSetUp( fldActSys );	//�e�X�g

	return fldActSys;
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�A�N�g�V�X�e���j��
 */
//------------------------------------------------------------------
static void	DeleteFieldActSys( FLD_ACTSYS* fldActSys )
{
	testRelease( fldActSys );	//�e�X�g
	GFL_HEAP_FreeMemory( fldActSys ); 
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�A�N�g����֐�
 */
//------------------------------------------------------------------
static void	MainFieldActSys( FLD_ACTSYS* fldActSys )
{
	//�J������]�Z�o(�r���{�[�h���̂��̂ɂ͊֌W�Ȃ��B�A�j�������̕ύX������̂ɎQ��)
	VecFx32 vec, camPos, target;
	GFL_G3D_CAMERA* g3Dcamera = GetG3Dcamera( fldActSys->gs );

	GFL_G3D_CAMERA_GetPos( g3Dcamera, &camPos );
	GFL_G3D_CAMERA_GetTarget( g3Dcamera, &target );

	VEC_Subtract( &target, &camPos, &vec );
	
	fldActSys->cameraRotate = FX_Atan2Idx( -vec.z, vec.x ) - 0x4000;
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�A�N�g���[�N������
 */
//------------------------------------------------------------------
static void	initActWork( FLD_ACTSYS* fldActSys, FLD_ACT_WORK* actWork )
{
	int i;

	actWork->fldActSys = fldActSys;

	for( i=0; i<WORK_SIZ; i++ ){
		actWork->work[i] = 0;
	}
}


//============================================================================================
//
//
//	�ʐM�֐�
//
//
//============================================================================================
#ifdef NET_WORK_ON
//------------------------------------------------------------------
/**
 * @brief	��M���[�N������
 */
//------------------------------------------------------------------
static void _initRecvBuffer( void )
{
}

//------------------------------------------------------------------
/**
 * @brief	��M�֐�
 */
//------------------------------------------------------------------
// ���[�J���ʐM�R�}���h�̒�`
enum _gameCommand_e {
	_GAME_COM_PLAY = GFL_NET_CMD_COMMAND_MAX,
};

//------------------------------------------------------------------
// _GAME_COM_PLAY�@�Q�[����񑗎�M

static void _sendGamePlay( VecFx32* pVec  )
{
    SendSampleGameNet( _GAME_COM_PLAY, pVec );
}

static void _recvGamePlay
	(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    SAMPLE_WORK* psw = sampleWork;

    if(GetSampleNetID() != netID){
        GFL_STD_MemCopy(pData, &psw->recvWork, sizeof(VecFx32));
    }
}

//------------------------------------------------------------------
// ���[�J���ʐM�e�[�u��
const NetRecvFuncTable NetSamplePacketTbl[] = {
    { _recvGamePlay, GFL_NET_COMMAND_SIZE(sizeof(VecFx32)), NULL },
};

#endif
