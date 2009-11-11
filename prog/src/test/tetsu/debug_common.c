//============================================================================================
/**
 * @file	debug_common.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "system/gfl_use.h"
#include "system\main.h"
#include "arc_def.h"
#include "message.naix"

#include "font/font.naix"
#include "print/printsys.h"
#include "print/str_tool.h"

#include "test/camera_adjust_view.h"
#include "msg/msg_d_tetsu.h"

#include "debug_common.h"
//============================================================================================
/**
 *
 *
 *
 *
 * @brief	�f�o�b�O���ڑI��
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief		�\���̒�`
 */
//------------------------------------------------------------------
struct _DWS_SYS {
	HEAPID						heapID;
	
	GFL_FONT*					fontHandle;
	GFL_MSGDATA*			msgManager;
	GFL_G3D_CAMERA*		g3Dcamera;
	GFL_G3D_LIGHTSET*	g3Dlightset;	
	GFL_CAMADJUST*		gflCamAdjust;
	fx32							cameraLength;
	u16								cameraAngleV;
	u16								cameraAngleH;
	BOOL							cameraAdjustOn;
	VecFx32*					pCameraTarget;

  // camera_adjust�𓮂������߂ɒǉ����܂����B tomoya takahashi
  fx32              cameraFar;
  u16               cameraFovy;
  u16               padding;
};

//------------------------------------------------------------------
/**
 * @brief		�v���g�^�C�v�錾
 */
//------------------------------------------------------------------
DWS_SYS*	DWS_SYS_Setup(HEAPID heapID);
void			DWS_SYS_Framework(DWS_SYS* dws);
void			DWS_SYS_Delete(DWS_SYS* dws);

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�V�X�e���Z�b�g�A�b�v
 *
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�f�B�X�v���C���f�[�^
 */
//------------------------------------------------------------------
///�u�q�`�l�o���N�ݒ�\����
static const GFL_DISP_VRAM dispVram = {
	GX_VRAM_BG_128_C,								//���C��2D�G���W����BG�Ɋ��蓖�� 
	GX_VRAM_BGEXTPLTT_NONE,					//���C��2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_SUB_BG_32_H,						//�T�u2D�G���W����BG�Ɋ��蓖��
	GX_VRAM_SUB_BGEXTPLTT_NONE,			//�T�u2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_OBJ_64_E,								//���C��2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_OBJEXTPLTT_NONE,				//���C��2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_SUB_OBJ_NONE,						//�T�u2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_SUB_OBJEXTPLTT_NONE,		//�T�u2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_TEX_012_ABD,						//�e�N�X�`���C���[�W�X���b�g�Ɋ��蓖��
	GX_VRAM_TEXPLTT_0_G,						//�e�N�X�`���p���b�g�X���b�g�Ɋ��蓖��
	GX_OBJVRAMMODE_CHAR_1D_128K,		// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_32K,			// �T�uOBJ�}�b�s���O���[�h
};

static const GFL_BG_SYS_HEADER bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
};	

static const GFL_BG_BGCNT_HEADER Textcont = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};

static const GFL_BG_BGCNT_HEADER Statuscont = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x3800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_128x128,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};

//�J���������ݒ�f�[�^
static const VecFx32 cameraTarget	= { 0, 0, 0 };
static const VecFx32 cameraPos	= { 0, (FX32_ONE * 32), (FX32_ONE * 64) };

//���C�g�����ݒ�f�[�^
static const GFL_G3D_LIGHT_DATA light0Tbl[] = {
	{ 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
};
static const GFL_G3D_LIGHTSET_SETUP light0Setup = { light0Tbl, NELEMS(light0Tbl) };

static const GFL_CAMADJUST_SETUP camAdjustData= {
	0,
	GFL_DISPUT_BGID_S0,		// = STATUS_FRAME
	GFL_DISPUT_PALID_15,	// = TEXT_PLTTID
};
#define PITCH_LIMIT (0x200)

static void g3Dsys_setup( void )
{
	// �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
	G3X_SetShading( GX_SHADING_TOON );
	G3X_AntiAlias( FALSE );
	G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
	G3X_AlphaBlend( TRUE );		// �A���t�@�u�����h�@�I��
	G3X_EdgeMarking( FALSE );
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	// �N���A�J���[�̐ݒ�
	G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog

	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);
}

//------------------------------------------------------------------
/**
 * @brief		�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
DWS_SYS* DWS_SYS_Setup(HEAPID heapID)
{
	DWS_SYS* dws = GFL_HEAP_AllocClearMemory(heapID, sizeof(DWS_SYS));
	dws->heapID = heapID;

	// VRAM�S�N���A
  GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
  MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
  (void)GX_DisableBankForLCDC();

  MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
  MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

  MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
  MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette

	//VRAM�ݒ�
	GFL_DISP_SetBank(&dispVram);
	GFL_BG_SetBackGroundColor(GFL_BG_FRAME3_M, 0x0b50);

	//�a�f�V�X�e���N��
	GFL_BG_Init(dws->heapID);
	//�a�f���[�h�ݒ�
	GFL_BG_SetBGMode(&bgsysHeader);
	//�a�f�R���g���[���ݒ�
	GFL_BG_SetBGControl(TEXT_FRAME_M, &Textcont, GFL_BG_MODE_TEXT);
	GFL_BG_SetPriority(TEXT_FRAME_M, 0);
	GFL_BG_SetVisible(TEXT_FRAME_M, VISIBLE_ON);
	GFL_BG_SetBGControl(TEXT_FRAME_S, &Textcont, GFL_BG_MODE_TEXT);
	GFL_BG_SetPriority(TEXT_FRAME_S, 0);
	GFL_BG_SetVisible(TEXT_FRAME_S, VISIBLE_ON);
	GFL_BG_SetBGControl(STATUS_FRAME, &Statuscont, GFL_BG_MODE_TEXT);
	GFL_BG_SetPriority(STATUS_FRAME, 0);
	GFL_BG_SetVisible(STATUS_FRAME, VISIBLE_ON);

	GFL_BG_FillCharacter(TEXT_FRAME_M, 0, 1, 0);	// �擪�ɃN���A�L�����z�u
	GFL_BG_ClearScreen(TEXT_FRAME_M);
	GFL_BG_FillCharacter(TEXT_FRAME_S, 0, 1, 0);	// �擪�ɃN���A�L�����z�u
	GFL_BG_ClearScreen(TEXT_FRAME_S);

	//�R�c�V�X�e���N��
	GFL_G3D_Init(	GFL_G3D_VMANLNK, 
								GFL_G3D_TEX384K,
								GFL_G3D_VMANLNK,
								GFL_G3D_PLT64K,
								0,
								heapID,
								g3Dsys_setup );
	GFL_BG_SetBGControl3D(1);

	//�r�b�g�}�b�v�E�C���h�E�N��
	GFL_BMPWIN_Init(heapID);

	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
	GFL_DISP_SetDispOn();

	//�t�H���g�p�p���b�g�]��
	GFL_ARC_UTIL_TransVramPalette(	ARCID_FONT, 
																	NARC_font_default_nclr,
																	PALTYPE_MAIN_BG,
																	TEXT_PLTTID * PLTT_SIZ,
																	PLTT_SIZ,
																	dws->heapID);
	GFL_ARC_UTIL_TransVramPalette(	ARCID_FONT, 
																	NARC_font_default_nclr,
																	PALTYPE_SUB_BG,
																	TEXT_PLTTID * PLTT_SIZ,
																	PLTT_SIZ,
																	dws->heapID);
	//�t�H���g�n���h���쐬
	dws->fontHandle = GFL_FONT_Create(	ARCID_FONT,
																			NARC_font_large_nftr,
																			GFL_FONT_LOADTYPE_FILE,
																			FALSE,
																			dws->heapID);
	//���b�Z�[�W�}�l�[�W���[�쐬
	dws->msgManager = GFL_MSG_Create
		(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_tetsu_dat, dws->heapID);

	//�J�����쐬
	dws->g3Dcamera = GFL_G3D_CAMERA_CreateDefault(&cameraPos, &cameraTarget, dws->heapID);
	GFL_G3D_CAMERA_Switching(dws->g3Dcamera);
	//���C�g�쐬
	dws->g3Dlightset = GFL_G3D_LIGHT_Create( &light0Setup, dws->heapID );
	GFL_G3D_LIGHT_Switching(dws->g3Dlightset);

	//�J���������c�[���ݒ�
	dws->gflCamAdjust = GFL_CAMADJUST_Create(&camAdjustData, dws->heapID);
	dws->cameraAngleV = 0;
	dws->cameraAngleH = 0;
	dws->cameraLength = 16*FX32_ONE; 
  dws->cameraFovy   = defaultCameraFovy/2 *PERSPWAY_COEFFICIENT;
  dws->cameraFar    = defaultCameraFar;
	GFL_CAMADJUST_SetCameraParam(	dws->gflCamAdjust, 
																&dws->cameraAngleV, 
																&dws->cameraAngleH, 
																&dws->cameraLength, 
																&dws->cameraFovy, 
																&dws->cameraFar); 
	dws->pCameraTarget = NULL;
	dws->cameraAdjustOn = TRUE;

	return dws;
}

//------------------------------------------------------------------
/**
 * @brief		�t���[�����[�N
 */
//------------------------------------------------------------------
void DWS_SYS_Framework(DWS_SYS* dws)
{
	if(dws->cameraAdjustOn == TRUE){ GFL_CAMADJUST_Main(dws->gflCamAdjust); }

	//�ˉe�ݒ�
	{
		GFL_G3D_CAMERA_SetfovySin(dws->g3Dcamera, FX_SinIdx(dws->cameraFovy));
		GFL_G3D_CAMERA_SetfovyCos(dws->g3Dcamera, FX_CosIdx(dws->cameraFovy));
		GFL_G3D_CAMERA_SetFar(dws->g3Dcamera, &dws->cameraFar);
	}
	//�����ƃA���O���ɂ��J�����ʒu�v�Z
	{
		VecFx32 cameraPos, cameraTarget;
		VecFx32 vecCamera;
		fx16 sinYaw = FX_SinIdx(dws->cameraAngleV);
		fx16 cosYaw = FX_CosIdx(dws->cameraAngleV);
		fx16 sinPitch = FX_SinIdx(dws->cameraAngleH);
		fx16 cosPitch = FX_CosIdx(dws->cameraAngleH);
	
		if(cosPitch < 0){ cosPitch = -cosPitch; }	// �����肵�Ȃ��悤�Ƀ}�C�i�X�l�̓v���X�l�ɕ␳
		if(cosPitch < PITCH_LIMIT){ cosPitch = PITCH_LIMIT; }	// 0�l�ߕӂ͕s���\���ɂȂ邽�ߕ␳

		// �J�����̍��W�v�Z
		VEC_Set( &vecCamera, sinYaw * cosPitch, sinPitch * FX16_ONE, cosYaw * cosPitch);
		VEC_Normalize(&vecCamera, &vecCamera);
		if(dws->pCameraTarget != NULL){
			cameraTarget = *dws->pCameraTarget;
		} else {
			VEC_Set(&cameraTarget, 0, 0, 0);
		}
		VEC_MultAdd(dws->cameraLength, &vecCamera, &cameraTarget, &cameraPos);
		GFL_G3D_CAMERA_SetTarget(dws->g3Dcamera, &cameraTarget);
		GFL_G3D_CAMERA_SetPos(dws->g3Dcamera, &cameraPos);
	}
	GFL_G3D_CAMERA_Switching(dws->g3Dcamera);
}

//------------------------------------------------------------------
/**
 * @brief		�j��
 */
//------------------------------------------------------------------
void DWS_SYS_Delete(DWS_SYS* dws)
{
	GFL_CAMADJUST_Delete(dws->gflCamAdjust);

	GFL_MSG_Delete(dws->msgManager);
	GFL_FONT_Delete(dws->fontHandle);

	GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);

	GFL_G3D_Exit();
	GFL_BMPWIN_Exit();
	GFL_BG_FreeBGControl(STATUS_FRAME);
	GFL_BG_FreeBGControl(TEXT_FRAME_S);
	GFL_BG_FreeBGControl(TEXT_FRAME_M);
	GFL_BG_Exit();
}


//------------------------------------------------------------------
/**
 * @brief		�ݒ�擾
 */
//------------------------------------------------------------------
GFL_FONT*					DWS_GetFontHandle(DWS_SYS* dws)								{ return dws->fontHandle; }
GFL_MSGDATA*			DWS_GetMsgManager(DWS_SYS* dws)								{ return dws->msgManager; }
GFL_G3D_CAMERA*		DWS_GetG3Dcamera(DWS_SYS* dws)								{ return dws->g3Dcamera; }
GFL_G3D_LIGHTSET*	DWS_GetG3Dlight(DWS_SYS* dws)									{ return dws->g3Dlightset; }	
GFL_CAMADJUST*		DWS_GetCamAdjust(DWS_SYS* dws)								{ return dws->gflCamAdjust; }	
u16								DWS_GetCamAngleH(DWS_SYS* dws)								{ return dws->cameraAngleH; }	
u16								DWS_GetCamAngleV(DWS_SYS* dws)								{ return dws->cameraAngleV; }	
void							DWS_SetG3DcamTarget(DWS_SYS* dws, VecFx32* t)	{ dws->pCameraTarget = t; }
void							DWS_CamAdjustOn(DWS_SYS* dws)
{ 
	GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
	dws->cameraAdjustOn = TRUE;
}
void							DWS_CamAdjustOff(DWS_SYS* dws)
{
	GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_SUB);
	dws->cameraAdjustOn = FALSE;
}
void	DWS_SetCamStatus(DWS_SYS* dws, u16 angleH, u16 angleV, fx32 length )
{
	dws->cameraAngleH = angleH;
	dws->cameraAngleV = angleV;
	dws->cameraLength = length;
}

//------------------------------------------------------------------
/**
 * @brief		����ʕ`��
 */
//------------------------------------------------------------------
void DWS_DrawLocalOriginPlane(DWS_SYS* dws, fx32 scale, GXRgb color)
{
	VecFx32 pos, up, target;

	G3X_Reset();

	GFL_G3D_CAMERA_GetPos(dws->g3Dcamera, &pos);
	GFL_G3D_CAMERA_GetCamUp(dws->g3Dcamera, &up);
	GFL_G3D_CAMERA_GetTarget(dws->g3Dcamera, &target);
	G3_LookAt(&pos, &up, &target, NULL);

	G3_PushMtx();
	//���s�ړ��p�����[�^�ݒ�
	//G3_Translate(target.x, target.y, target.z);
	G3_Translate(0, 0, 0);

	//�O���[�o���X�P�[���ݒ�
	G3_Scale(scale, scale, scale);

	G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, 0, 0, 0, 0, GX_TEXPLTTCOLOR0_USE, 0);
	
	//�}�e���A���ݒ�
	G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31), GX_RGB(16, 16, 16), TRUE);
	G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE);
	G3_PolygonAttr(	GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 63, 31, 0);
		
	G3_Begin( GX_BEGIN_QUADS );
	
	G3_Color(color);
	
	G3_Vtx(-(FX16_ONE-1), 0, (FX16_ONE-1));
	G3_Vtx(-(FX16_ONE-1), 0, -(FX16_ONE-1));
	G3_Vtx((FX16_ONE-1), 0, -(FX16_ONE-1));
	G3_Vtx((FX16_ONE-1), 0, (FX16_ONE-1));
	
	G3_End();
	G3_PopMtx(1);
}

//------------------------------------------------------------------
/**
 * @brief		�Bump���ʕ`��
 */
//------------------------------------------------------------------
void DWS_DrawLocalOriginBumpPlane(DWS_SYS* dws, fx32 scale, GXRgb color1, GXRgb color2, int div)
{
	VecFx32 pos, up, target;
	fx16		sx;
	u8			dir;
	int			i;

	if( div > 32){ div = 32; }
	sx = (FX16_ONE-1)*2 /div;
	dir = 0;

	G3X_Reset();

	GFL_G3D_CAMERA_GetPos(dws->g3Dcamera, &pos);
	GFL_G3D_CAMERA_GetCamUp(dws->g3Dcamera, &up);
	GFL_G3D_CAMERA_GetTarget(dws->g3Dcamera, &target);
	G3_LookAt(&pos, &up, &target, NULL);

	G3_PushMtx();
	//���s�ړ��p�����[�^�ݒ�
	//G3_Translate(target.x, target.y, target.z);
	G3_Translate(0, 0, 0);

	//�O���[�o���X�P�[���ݒ�
	G3_Scale(scale, scale, scale);

	G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, 0, 0, 0, 0, GX_TEXPLTTCOLOR0_USE, 0);
	
	//�}�e���A���ݒ�
	G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31), GX_RGB(16, 16, 16), TRUE);
	G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE);
	G3_PolygonAttr(	GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 63, 31, 0);
		
	G3_Begin( GX_BEGIN_QUADS );
	
	for( i=0; i<div; i++ ){
		fx16 y1, y2;
		if( dir ){
			y1 = FX16_ONE/32;
			y2 = 0;
			G3_Color(color2);
		} else {
			y1 = 0;
			y2 = FX16_ONE/32;
			G3_Color(color1);
		}
		G3_Vtx(-(FX16_ONE-1) + sx*i, y1, (FX16_ONE-1));
		G3_Vtx(-(FX16_ONE-1) + sx*i, y1, -(FX16_ONE-1));
		G3_Vtx(-(FX16_ONE-1) + sx*(i+1), y2, -(FX16_ONE-1));
		G3_Vtx(-(FX16_ONE-1) + sx*(i+1), y2, (FX16_ONE-1));

		dir = (dir)? 0 : 1;
	}
	
	G3_End();
	G3_PopMtx(1);
}

