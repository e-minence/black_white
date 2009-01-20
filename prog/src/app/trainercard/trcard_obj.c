#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "arc_def.h"

#include "trcard_obj.h"
#include "trainer_case.naix"

#define MAIN_LCD	( GFL_BG_MAIN_DISP )	// 0
#define SUB_LCD		( GFL_BG_SUB_DISP )		// 1

#define TR_SUBSURFACE_Y	(32)

//** CharManager PlttManager�p **//
#define TR_CARD_CHAR_CONT_NUM				(2)
#define TR_CARD_CHAR_VRAMTRANS_MAIN_SIZE	(0)//(2048)
#define TR_CARD_CHAR_VRAMTRANS_SUB_SIZE		(0)
#define TR_CARD_PLTT_CONT_NUM				(2)
#define TR_CARD_PALETTE_NUM					(16)	//�p���b�g9�{
#define TR_CARD_BADGE_PALETTE_NUM			(4)	//�p���b�g4�{

#define BTN_EFF_POS_X	(12*8)
#define BTN_EFF_POS_Y	(17*8)


typedef struct BADGE_POS_tag{
	int x;
	int y;
}BADGE_POS;

//���\�[�X�}�l�[�W���[�o�^���e�[�u��
static const u8 ResEntryNumTbl[RESOURCE_NUM] = {
	1,			//�L�������\�[�X
	2,			//�p���b�g���\�[�X
	1,			//�Z�����\�[�X
	1,			//�Z���A�j�����\�[�X
};

#define BADGE_LX0	(8*12)
#define BADGE_LX1	(8*18)
#define BADGE_LX2	(8*24)
#define BADGE_LX3	(8*30)

#define BADGE_LY0	(8*6)
#define BADGE_LY1	(8*11)
#define BADGE_LY2	(8*17)
#define BADGE_LY3	(8*22)

#define BADGE_NC_OFS	(8*5)

static const BADGE_POS BadgePos[] = {
	{BADGE_LX0,BADGE_LY0},
	{BADGE_LX1,BADGE_LY0},
	{BADGE_LX2,BADGE_LY0},
	{BADGE_LX3,BADGE_LY0},
	{BADGE_LX0,BADGE_LY1},
	{BADGE_LX1,BADGE_LY1},
	{BADGE_LX2,BADGE_LY1},
	{BADGE_LX3,BADGE_LY1},

	{BADGE_LX0,BADGE_LY2},
	{BADGE_LX1,BADGE_LY2},
	{BADGE_LX2,BADGE_LY2},
	{BADGE_LX3,BADGE_LY2},
	{BADGE_LX0,BADGE_LY3},
	{BADGE_LX1,BADGE_LY3},
	{BADGE_LX2,BADGE_LY3},
	{BADGE_LX3,BADGE_LY3},
};

static const BADGE_POS StarPos[] = {
	{8*3,8*5},
	{8*10,8*5},
	{8*17,8*5},
	{8*24,8*5},
	{8*3,8*12},
	{8*10,8*12},
	{8*17,8*12},
	{8*24,8*12},
};

static void InitCharPlttManager(void);

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[������
 *
 * @param	wk	�g���[�i�[�J�[�hOBJ���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void InitTRCardCellActor( TR_CARD_OBJ_WORK *wk )
{
	int i;
//	initVramTransferManagerHeap( 32, HEAPID_TR_CARD );

	//���C���̃L�������E��128kbyte���[�h�ɕύX
	GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_128K);	
	GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);	

	InitCharPlttManager();
	// OAM�}�l�[�W���[�̏�����
	NNS_G2dInitOamManagerModule();

	GFL_CLACT_Init( GFL_CLSYSINIT_DEF_DIVSCREEN , wk->heapId );
	// �Z���A�N�^�[������
	wk->cellUnit = GFL_CLACT_UNIT_Create( TR_CARD_ACT_MAX, wk->heapId );

	//�T�u�T�[�t�F�[�X�̋�����ݒ�
//	CLACT_U_SetSubSurfaceMatrix(&wk->RendData,0,SUB_SURFACE_Y+FX32_CONST(TR_SUBSURFACE_Y));
	
	//���\�[�X�}�l�[�W���[������
//	for(i=0;i<RESOURCE_NUM;i++){		//���\�[�X�}�l�[�W���[�쐬
//		wk->ResMan[MAIN_LCD][i] = CLACT_U_ResManagerInit(ResEntryNumTbl[i], i, HEAPID_TR_CARD);
//		wk->ResMan[SUB_LCD][i] = CLACT_U_ResManagerInit(ResEntryNumTbl[i], i, HEAPID_TR_CARD);
//	}

	for(i = 0;i < 2;i++){
		int vram_type[2] = {
			NNS_G2D_VRAM_TYPE_2DMAIN,NNS_G2D_VRAM_TYPE_2DSUB,
		};
		u8 res_name[2][4] = {
		 {	NARC_trainer_case_card_badge_NCGR, 
			NARC_trainer_case_card_badge_NCLR, 
			NARC_trainer_case_card_badge_NCER, 
			NARC_trainer_case_card_badge_NANR },
		 {	NARC_trainer_case_card_button_NCGR, 
			NARC_trainer_case_card_button_NCLR, 
			NARC_trainer_case_card_button_NCER, 
			NARC_trainer_case_card_button_NANR },
		};
		//chara�ǂݍ���
		GFL_ARC_UTIL_TransVramCharacterMakeProxy( ARCID_TRAINERCARD , res_name[i][1] 
											, FALSE , MAP_TYPE_1D , 0 , vram_type[i] , 
											, wk->heapId , &charaData[i] );
//		wk->ResObjTbl[MAIN_LCD+i][CLACT_U_CHAR_RES] = GFL_ARC_UTIL_LoadOBJCharacter(
//											wk->ResMan[MAIN_LCD+i][CLACT_U_CHAR_RES],
//											ARCID_TRAINERCARD, res_name[i][CLACT_U_CHAR_RES],
//											FALSE, i, vram_type[i], HEAPID_TR_CARD);

		//pal�ǂݍ���
		GFL_ARC_UTIL_TransVramPaletteMakeProxy( ARCID_TRAINERCARD , res_name[i][0]
											, vram_type[i] , 
//		wk->ResObjTbl[MAIN_LCD+i][CLACT_U_PLTT_RES] = CLACT_U_ResManagerResAddArcPltt(
//											wk->ResMan[MAIN_LCD+i][CLACT_U_PLTT_RES],
//											ARCID_TRAINERCARD, res_name[i][CLACT_U_PLTT_RES],
//											FALSE, i, vram_type[i], 
//											TR_CARD_PALETTE_NUM, HEAPID_TR_CARD);

		//cell�ǂݍ���
		wk->ResObjTbl[MAIN_LCD+i][2] = GFL_ARC_UTIL_LoadCellBank( ARCID_TRAINERCARD
											, res_name[i][2] , FALSE , &wk->cellData[i] , wk->heapId );
//		wk->ResObjTbl[MAIN_LCD+i][CLACT_U_CELL_RES] = CLACT_U_ResManagerResAddArcKindCell(
//											wk->ResMan[MAIN_LCD+i][CLACT_U_CELL_RES],
//											ARCID_TRAINERCARD, res_name[i][CLACT_U_CELL_RES],
//											FALSE, i, CLACT_U_CELL_RES, HEAPID_TR_CARD);

		//�����֐���anim�ǂݍ���
		wk->ResObjTbl[MAIN_LCD+i][3] = GFL_ARC_UTIL_LoadAnimeBank( ARCID_TRAINERCARD ,
											, res_name[i][3] , FALSE , &wk->anmData[i] , wk->heapId );
//		wk->ResObjTbl[MAIN_LCD+i][CLACT_U_CELLANM_RES] = CLACT_U_ResManagerResAddArcKindCell(
//											wk->ResMan[MAIN_LCD+i][CLACT_U_CELLANM_RES],
//											ARCID_TRAINERCARD, res_name[i][CLACT_U_CELLANM_RES],
//											FALSE, i, CLACT_U_CELLANM_RES, HEAPID_TR_CARD);

		// ���\�[�X�}�l�[�W���[����]��
		//�ǂݍ��݂Ɠ����ɓ]���ς�
		// Chara�]��
//		CLACT_U_CharManagerSet( wk->ResObjTbl[MAIN_LCD+i][CLACT_U_CHAR_RES] );

		// �p���b�g�]��
//		CLACT_U_PlttManagerSet( wk->ResObjTbl[MAIN_LCD+i][CLACT_U_PLTT_RES] );
	}

	{
		u8 i;
		ARCHANDLE* pal_handle;

		const int pal_arc_idx[TR_CARD_BADGE_ACT_MAX] = {
			NARC_trainer_case_card_badge2_0_NCLR,
			NARC_trainer_case_card_badge2_1_NCLR,
			NARC_trainer_case_card_badge2_2_NCLR,
			NARC_trainer_case_card_badge2_3_NCLR,
			NARC_trainer_case_card_badge2_4_NCLR,
			NARC_trainer_case_card_badge2_5_NCLR,
			NARC_trainer_case_card_badge2_6_NCLR,
			NARC_trainer_case_card_badge2_7_NCLR
		};
		//�p���b�g�A�[�J�C�u�n���h���I�[�v��
		pal_handle  = ArchiveDataHandleOpen( ARCID_TRAINERCARD, HEAPID_TR_CARD);
		for (i=0;i<TR_CARD_BADGE_ACT_MAX;i++){
			wk->PalDataBuf[i] = ArchiveDataLoadAllocByHandle( pal_handle, pal_arc_idx[i], HEAPID_TR_CARD );
			if( wk->PalDataBuf[i] != NULL )
			{
				if( NNS_G2dGetUnpackedPaletteData( wk->PalDataBuf[i], &wk->PalData[i] ) == FALSE ){
					sys_FreeMemoryEz( wk->PalData[i] );
					GF_ASSERT(0);
				}
			}else{
				GF_ASSERT(0);
			}
		}
		//�n���h���N���[�Y
		ArchiveDataHandleClose( pal_handle );
	}
	
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// MAIN DISP OBJ ON
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// SUB DISP OBJ ON
}

#if 0
#pragma mark [>end edit

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�N�^�[���Z�b�g
 *
 * @param	wk				�g���[�i�[�J�[�hOBJ���[�N
 * @param	inBadgeDisp		�o�b�W�\���t���O���X�g
 * @param	isClear			�a�����肵�Ă��邩�H
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void SetTrCardActor( TR_CARD_OBJ_WORK *wk, const u8 *inBadgeDisp ,const BOOL isClear)
{
	int i;
	CLACT_HEADER			cl_act_header;
	
	// �Z���A�N�^�[�w�b�_�쐬	
	CLACT_U_MakeHeader(	&cl_act_header,
						MAIN_LCD, MAIN_LCD, MAIN_LCD, MAIN_LCD,
						CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
						0, 1,
						wk->ResMan[MAIN_LCD][CLACT_U_CHAR_RES],
						wk->ResMan[MAIN_LCD][CLACT_U_PLTT_RES],
						wk->ResMan[MAIN_LCD][CLACT_U_CELL_RES],
						wk->ResMan[MAIN_LCD][CLACT_U_CELLANM_RES],
						NULL,NULL);

	{
		//�o�^���i�[
		u8 ofs;
		CLACT_ADD add;
		u16	badge_ofs = 0;

		add.ClActSet	= wk->ClactSet;
		add.ClActHeader	= &cl_act_header;

		add.mat.x		= 0;//FX32_CONST(32) ;
		add.mat.y		= 0;//FX32_CONST(96) ;		//��ʂ͏㉺�A�����Ă���
		add.mat.z		= 0;
		add.sca.x		= FX32_ONE;
		add.sca.y		= FX32_ONE;
		add.sca.z		= FX32_ONE;
		add.rot			= 0;
		add.pri			= 2;
		add.DrawArea	= NNS_G2D_VRAM_TYPE_2DMAIN;
		add.heap		= HEAPID_TR_CARD;

		//�Z���A�N�^�[�\���J�n
		if(!isClear){
			badge_ofs = BADGE_NC_OFS; 
		}

		// �����(���C�����)
		//�o�b�W
		for(i=0;i<TR_CARD_BADGE_ACT_MAX;i++){
			add.mat.x = FX32_ONE * BadgePos[i].x;
			add.mat.y = FX32_ONE * (BadgePos[i].y+badge_ofs);
			wk->ClActWork[i] = CLACT_Add(&add);
			CLACT_SetAnmFlag(wk->ClActWork[i],TRUE);
			CLACT_AnmChg( wk->ClActWork[i], i );
			if (!inBadgeDisp[i]){
				CLACT_SetDrawFlag(wk->ClActWork[i], 0);	//��\��
			}
		}
	}	
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʃZ���A�N�^�[���Z�b�g
 *
 * @param	wk				�g���[�i�[�J�[�hOBJ���[�N
 * @param	inBadgeDisp		�o�b�W�\���t���O���X�g
 * @param	isClear			�a�����肵�Ă��邩�H
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void SetTrCardActorSub( TR_CARD_OBJ_WORK *wk)
{
	int i;
	CLACT_HEADER			cl_act_header;
	static u8 ActPos[][2] = {
	 {192,21*8},{0,21*8},
	};
	static u8 AnmNo[] = {
		ANMS_BACK_OFF,ANMS_EXP,ANMS_EFF	
	};
	// �Z���A�N�^�[�w�b�_�쐬	
	CLACT_U_MakeHeader(	&cl_act_header,
						SUB_LCD, SUB_LCD, SUB_LCD, SUB_LCD,
						CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
						0, 1,
						wk->ResMan[SUB_LCD][CLACT_U_CHAR_RES],
						wk->ResMan[SUB_LCD][CLACT_U_PLTT_RES],
						wk->ResMan[SUB_LCD][CLACT_U_CELL_RES],
						wk->ResMan[SUB_LCD][CLACT_U_CELLANM_RES],
						NULL,NULL);

	{
		//�o�^���i�[
//		u8 ofs;
		CLACT_ADD add;
//		u16	badge_ofs = 0;

		add.ClActSet	= wk->ClactSet;
		add.ClActHeader	= &cl_act_header;

		add.mat.x		= 0;//FX32_CONST(32) ;
		add.mat.y		= 0;//FX32_CONST(96) ;		//��ʂ͏㉺�A�����Ă���
		add.mat.z		= 0;
		add.sca.x		= FX32_ONE;
		add.sca.y		= FX32_ONE;
		add.sca.z		= FX32_ONE;
		add.rot			= 0;
		add.pri			= 0;
		add.DrawArea	= NNS_G2D_VRAM_TYPE_2DSUB;
		add.heap		= HEAPID_TR_CARD;

		//�Z���A�N�^�[�\���J�n
		// �����(�T�u���)
		for(i=0;i<TR_CARD_SUB_ACT_MAX;i++){
			add.mat.x = FX32_ONE * ActPos[i][0];
			add.mat.y = FX32_ONE * ActPos[i][1] + (FX32_CONST(TR_SUBSURFACE_Y)+SUB_SURFACE_Y);
			wk->ClActWorkS[i] = CLACT_Add(&add);
			CLACT_SetAnmFlag(wk->ClActWorkS[i],TRUE);
			CLACT_AnmChg( wk->ClActWorkS[i], AnmNo[i] );
			CLACT_DrawPriorityChg( wk->ClActWorkS[i],TR_CARD_SUB_ACT_MAX-i);
			CLACT_SetDrawFlag(wk->ClActWorkS[i], FALSE);	//��\��
		}
	}	
}

//--------------------------------------------------------------------------------------------
/**
 * 2D�Z���I�u�W�F�N�g���
 *
 * @param	wk	�I�u�W�F�N�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void RereaseCellObject(TR_CARD_OBJ_WORK *wk)
{
	u8 i;
	//�p���b�g���
	for(i=0;i<TR_CARD_BADGE_ACT_MAX;i++){
		sys_FreeMemoryEz(wk->PalDataBuf[i]);
	}

	// �Z���A�N�^�[���\�[�X���

	// �L�����]���}�l�[�W���[�j��
	CLACT_U_CharManagerDelete(wk->ResObjTbl[MAIN_LCD][CLACT_U_CHAR_RES]);
	CLACT_U_CharManagerDelete(wk->ResObjTbl[SUB_LCD][CLACT_U_CHAR_RES]);

	// �p���b�g�]���}�l�[�W���[�j��
	CLACT_U_PlttManagerDelete(wk->ResObjTbl[MAIN_LCD][CLACT_U_PLTT_RES]);
	CLACT_U_PlttManagerDelete(wk->ResObjTbl[SUB_LCD][CLACT_U_PLTT_RES]);
		
	// �L�����E�p���b�g�E�Z���E�Z���A�j���̃��\�[�X�}�l�[�W���[�j��
	for(i=0;i<RESOURCE_NUM;i++){
		CLACT_U_ResManagerDelete(wk->ResMan[MAIN_LCD][i]);
		CLACT_U_ResManagerDelete(wk->ResMan[SUB_LCD][i]);
	}
	//�T�u�T�[�t�F�[�X�̋�����ݒ�
	CLACT_U_SetSubSurfaceMatrix(&wk->RendData,0,SUB_SURFACE_Y);

	// �Z���A�N�^�[�Z�b�g�j��
	CLACT_DestSet(wk->ClactSet);

	//OAM�����_���[�j��
	REND_OAM_Delete();

	DeleteCharManager();
	DeletePlttManager();
	
	GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);	
}

//--------------------------------------------------------------------------------------------
/**
 * �o�b�W�p���b�g�ύX
 *
 * @param	wk			OBJ���[�N
 * @param	inBadgeNo	�o�b�W�i���o�[
 * @param	inOalNo		�p���b�g�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void SetBadgePalette( TR_CARD_OBJ_WORK *wk, const u8 inBadgeNo, const u8 inPalNo )
{
	NNSG2dImagePaletteProxy *proxy;
	u32 palette_vram;
	u32 pal_ofs;
	u8 * adr;
	
	GF_ASSERT(inBadgeNo<TR_CARD_BADGE_ACT_MAX);
	GF_ASSERT(inPalNo<=3);
	
///	CLACT_PaletteNoChg( wk->ClActWork[inBadgeNo], inPalNo );

	proxy = CLACT_PaletteProxyGet( wk->ClActWork[inBadgeNo] );
	palette_vram = NNS_G2dGetImagePaletteLocation(proxy, NNS_G2D_VRAM_TYPE_2DMAIN);
	palette_vram += inBadgeNo*(16*2);
	adr = wk->PalData[inBadgeNo]->pRawData;
	DC_FlushRange( &adr[inPalNo*16*2], 16*2 );
	GX_LoadOBJPltt( &adr[inPalNo*16*2], palette_vram, 16*2 );	//32byte 1Palette
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʃ{�^���A�N�^�[�@�\����ԕύX
 *
 * @param	wk			OBJ���[�N
 * @param	inBadgeNo	�o�b�W�i���o�[
 * @param	inOalNo		�p���b�g�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void SetSActDrawSt( TR_CARD_OBJ_WORK *wk, u8 act_id, u8 anm_pat ,u8 draw_f)
{
//	CLACT_SetAnmFlag(wk->ClActWorkS[act_id],TRUE);
	CLACT_AnmChg( wk->ClActWorkS[act_id],anm_pat);
	CLACT_AnmReStart(wk->ClActWorkS[act_id]);
	CLACT_SetDrawFlag(wk->ClActWorkS[act_id],draw_f);	
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʃ{�^���G�t�F�N�g�A�N�^�[�@�\����ԕύX
 *
 * @param	wk			OBJ���[�N
 * @param	inBadgeNo	�o�b�W�i���o�[
 * @param	inOalNo		�p���b�g�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void SetEffActDrawSt( TR_CARD_OBJ_WORK *wk, u8 pat ,u8 draw_f)
{
	static const u8 pos_x[2] = {28*8,7*8};
	if(pat == 2){
		CATS_ObjectPosSet(wk->ClActWorkS[ACTS_BTN_EFF],sys.tp_x,sys.tp_y+TR_SUBSURFACE_Y);
	}else{
		CATS_ObjectPosSet(wk->ClActWorkS[ACTS_BTN_EFF],pos_x[pat],22*8+TR_SUBSURFACE_Y);
	}
	CLACT_AnmReStart(wk->ClActWorkS[ACTS_BTN_EFF]);
	CLACT_SetDrawFlag(wk->ClActWorkS[ACTS_BTN_EFF],draw_f);	
}

//--------------------------------------------------------------------------------------------
/**
 * �L�����N�^�}�l�[�W���[
 *�p���b�g�}�l�[�W���[�̏�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitCharPlttManager(void)
{
	// �L�����N�^�}�l�[�W���[������
	{
		CHAR_MANAGER_MAKE cm = {
			TR_CARD_CHAR_CONT_NUM,
			TR_CARD_CHAR_VRAMTRANS_MAIN_SIZE,
			TR_CARD_CHAR_VRAMTRANS_SUB_SIZE,
			HEAPID_TR_CARD
		};
		InitCharManager(&cm);
	}
	// �p���b�g�}�l�[�W���[������
	InitPlttManager(TR_CARD_PLTT_CONT_NUM, HEAPID_TR_CARD);

	// �ǂݍ��݊J�n�ʒu��������
	CharLoadStartAll();
	PlttLoadStartAll();

	REND_OAM_UtilOamRamClear_Main(HEAPID_TR_CARD);
	REND_OAM_UtilOamRamClear_Sub(HEAPID_TR_CARD);

//	DellVramTransferManager();
}


#pragma mark [>start edit
#endif


