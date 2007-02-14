
//============================================================================================
/**
 * @file	game.c
 * @brief	DS�Ń��b�V�[�̂��܂��@�Q�[���v���O����
 * @author	sogabe
 * @date	2007.02.08
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "ui.h"

#include "main.h"
#include "yt_common.h"
#include "player.h"
#include "fallchr.h"

#include "sample_graphic/yossyegg.naix"

#define __GAME_H_GLOBAL__
#include "game.h"

static	void	YT_MainGameAct(GAME_PARAM *gp);
static	void	YT_ClactResourceLoad( YT_CLACT_RES *clact_res, u32 heapID );
static	int		YT_ReadyCheck(GAME_PARAM *gp,YT_PLAYER_STATUS *ps);
static	void	YT_ReadyAct(GAME_PARAM *gp,int player_no);
static	BOOL	YT_FallCheck(GAME_PARAM *gp,YT_PLAYER_STATUS *ps);

//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[��������
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
void	YT_InitGame(GAME_PARAM *gp)
{
	// �Z���A�N�^�[���j�b�g�쐬
	gp->clact->p_unit = GFL_CLACT_UnitCreate( YT_CLACT_MAX, gp->heapID );

	//�G���A�}�l�[�W��������
	gp->clact_area=GFL_AREAMAN_Create(YT_CLACT_MAX,gp->heapID);
	
	//BG�V�X�e��������
	GFL_BG_sysInit(gp->heapID);

	//VRAM�ݒ�
	{
		GFL_BG_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
			GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
			GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
			GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
			GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
			GX_VRAM_TEX_0_B,				// �e�N�X�`���C���[�W�X���b�g
			GX_VRAM_TEXPLTT_0_F				// �e�N�X�`���p���b�g�X���b�g
		};
		GFL_DISP_SetBank( &vramSetTable );

		//VRAM�N���A
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}

	// BG SYSTEM
	{
		GFL_BG_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_InitBG( &BGsys_data );
	}

	//���C����ʃt���[���ݒ�
	{
		GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME2_M
			{
				0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME3_M
			{
				0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME2_S
			{
				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME3_S
			{
				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
		};
		GFL_BG_BGControlSet(GFL_BG_FRAME2_M, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
		GFL_BG_ScrClear(GFL_BG_FRAME2_M );
		GFL_BG_BGControlSet(GFL_BG_FRAME3_M, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
		GFL_BG_ScrClear(GFL_BG_FRAME3_M );
		GFL_BG_BGControlSet(GFL_BG_FRAME2_S, &TextBgCntDat[2], GFL_BG_MODE_TEXT );
		GFL_BG_ScrClear(GFL_BG_FRAME2_S );
		GFL_BG_BGControlSet(GFL_BG_FRAME3_S, &TextBgCntDat[3], GFL_BG_MODE_TEXT );
		GFL_BG_ScrClear(GFL_BG_FRAME3_S );

		GFL_DISP_GX_VisibleControl(GX_PLANEMASK_BG0, VISIBLE_ON );

		// OBJ�}�b�s���O���[�h
		GX_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );
		GXS_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );

		GFL_DISP_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		GFL_DISP_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	}

	//��ʐ���
	GFL_ARC_UtilBgCharSet(0,NARC_yossyegg_game_bg_NCGR,GFL_BG_FRAME2_M,0,0,0,gp->heapID);
	GFL_ARC_UtilScrnSet(0,NARC_yossyegg_game_bg_NSCR,GFL_BG_FRAME2_M,0,0,0,gp->heapID);
	GFL_ARC_UtilBgCharSet(0,NARC_yossyegg_YT_BG03_NCGR,GFL_BG_FRAME3_M,0,0,0,gp->heapID);
	GFL_ARC_UtilScrnSet(0,NARC_yossyegg_YT_BG03_NSCR,GFL_BG_FRAME3_M,0,0,0,gp->heapID);
	GFL_ARC_UtilPalSet(0,NARC_yossyegg_YT_BG03_NCLR,PALTYPE_MAIN_BG,0,0x100,gp->heapID);

	GFL_DISP_DispOn();
	GFL_DISP_DispSelect( GFL_DISP_3D_TO_SUB );

	GFL_FADE_MasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN|GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,16,0,2);

	//�Z���A�N�^�[���\�[�X�ǂݍ���
	YT_ClactResourceLoad(&gp->clact->res, gp->heapID);

	//�t���O�֘A����
	{
		int	x,y;

		for(x=0;x<YT_LINE_MAX;x++){
			gp->ps[0].falltbl[x]=x;
			gp->ps[1].falltbl[x]=x;
			gp->ps[0].stoptbl[x]=x;
			gp->ps[1].stoptbl[x]=x;
		}
		for(y=0;y<YT_HEIGHT_MAX;y++){
			for(x=0;x<YT_LINE_MAX;x++){
				gp->ps[0].ready[x][y]=NULL;
				gp->ps[1].ready[x][y]=NULL;
				gp->ps[0].fall[x][y]=NULL;
				gp->ps[1].fall[x][y]=NULL;
				gp->ps[0].stop[x][y]=NULL;
				gp->ps[1].stop[x][y]=NULL;
			}
		}
		gp->default_fall_wait=YT_DEFAULT_FALL_WAIT;
	}

	//����������
	{
		GFL_STD_MTRandInit(0);
	}

	//�v���[���[������
	YT_InitPlayer(gp,0,0);

	YT_JobNoSet(gp,YT_MainGameNo);

}

//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[�����C������
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
void	YT_MainGame(GAME_PARAM *gp)
{
	//�Q�[���V�[�P���X����
	YT_MainGameAct(gp);

	GFL_TCB_SysMain(gp->tcbsys);

	// �Z���A�N�^�[���j�b�g�`�揈��
	GFL_CLACT_UnitDraw( gp->clact->p_unit );

	// �Z���A�N�^�[�V�X�e�����C������
	// �S���j�b�g�`�悪�������Ă���s���K�v������܂��B
	GFL_CLACT_SysMain();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[���V�[�P���X����
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
enum{
	SEQ_GAME_START_WAIT=0,
	SEQ_GAME_READY_CHECK,
	SEQ_GAME_FALL_CHECK,
};

enum{
	YT_READY_WAIT=0,
	YT_READY_MAKE,
	YT_READY_ALREADY,
};

static	void	YT_MainGameAct(GAME_PARAM *gp)
{
	switch(gp->seq_no){
	case SEQ_GAME_START_WAIT:
		if(GFL_FADE_FadeCheck()==FALSE){
			gp->seq_no++;
		}
		break;
	case SEQ_GAME_READY_CHECK:
		switch(YT_ReadyCheck(gp,&gp->ps[0])){
		case YT_READY_MAKE:
			YT_ReadyAct(gp,0);
		case YT_READY_ALREADY:
			gp->ps[0].fall_wait=YT_FALL_WAIT;
			gp->seq_no++;
			break;
		default:
			break;
		}
		break;
	case SEQ_GAME_FALL_CHECK:
		if(YT_FallCheck(gp,&gp->ps[0])==TRUE){
			if(--gp->ps[0].fall_wait==0){
				gp->seq_no=SEQ_GAME_READY_CHECK;
			}
		}
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ҋ@�L�����𐶐����Ă��悢���`�F�b�N
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 *	@param	ps		�v���C���[�X�e�[�^�X
 */
//-----------------------------------------------------------------------------
static	int	YT_ReadyCheck(GAME_PARAM *gp,YT_PLAYER_STATUS *ps)
{
	int	x,y;

	for(x=0;x<4;x++){
		//���ł�READY��ԂȂ�
		if(ps->ready[x][0]){
			return YT_READY_ALREADY;
		}
	}

	for(x=0;x<4;x++){
		for(y=0;y<8;y++){
			if(ps->fall[x][y]){
				{
					CLSYS_POS	pos;
					GFL_CLACT_WkGetWldPos(gp->clact->clact_work[ps->fall[x][y]->clact_no],&pos);
					if(pos.y<YT_READY_NEXT_Y_POS){
						return YT_READY_WAIT;
					}
				}
				break;
			}
		}
	}
	return YT_READY_MAKE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ҋ@�L������������
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
static	void	YT_ReadyAct(GAME_PARAM *gp,int player_no)
{
	int	i=2;
	int	line,type;

	while(i){
		line=__GFL_STD_MTRand()%4;
		if(gp->ps[player_no].ready[line][0]){
			continue;
		}
		type=__GFL_STD_MTRand()%4;
		gp->ps[player_no].ready[line][0]=YT_InitFallChr(gp,player_no,type,line);
		i--;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ҋ@�L�����𗎉������Ă��悢���`�F�b�N
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 *	@param	ps		�v���C���[�X�e�[�^�X
 */
//-----------------------------------------------------------------------------
static	BOOL	YT_FallCheck(GAME_PARAM *gp,YT_PLAYER_STATUS *ps)
{
	int	x,y;

	for(x=0;x<YT_LINE_MAX;x++){
		for(y=0;y<YT_HEIGHT_MAX;y++){
			if(ps->fall[x][y]){
				return FALSE;
			}
		}
	}
	for(x=0;x<YT_LINE_MAX;x++){
		ps->falltbl[x]=x;
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���\�[�X�ǂݍ���
 *
 *	@param	clact_res	���[�N
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void YT_ClactResourceLoad( YT_CLACT_RES *clact_res, u32 heapID )
{
	BOOL result;
	void* p_buff;
	NNSG2dCharacterData* p_char;
	NNSG2dPaletteData* p_pltt;
	
	// �L�����N�^�f�[�^�ǂݍ��݁��]��
	{
		p_buff = GFL_ARC_DataLoadMalloc( 0,NARC_yossyegg_O_WOODS3_NCGR, heapID );
		result = NNS_G2dGetUnpackedCharacterData( p_buff, &p_char );
		GF_ASSERT( result );
		NNS_G2dInitImageProxy( &clact_res->imageproxy );
		NNS_G2dLoadImage1DMapping( 
				p_char, 
				0, 
				NNS_G2D_VRAM_TYPE_2DMAIN, 
				&clact_res->imageproxy );
		NNS_G2dLoadImage1DMapping( 
				p_char, 
				0, 
				NNS_G2D_VRAM_TYPE_2DSUB, 
				&clact_res->imageproxy );
		GFL_HEAP_FreeMemory( p_buff );
	}

	// �p���b�g�f�[�^�ǂݍ��݁��]��
	{
		p_buff = GFL_ARC_DataLoadMalloc( 0,NARC_yossyegg_OBJ_COL_NCLR, heapID );
		result = NNS_G2dGetUnpackedPaletteData( p_buff, &p_pltt );
		GF_ASSERT( result );
		NNS_G2dInitImagePaletteProxy( &clact_res->plttproxy );
		NNS_G2dLoadPalette( 
				p_pltt, 
				0, 
				NNS_G2D_VRAM_TYPE_2DMAIN, 
				&clact_res->plttproxy );
		NNS_G2dLoadPalette( 
				p_pltt, 
				0, 
				NNS_G2D_VRAM_TYPE_2DSUB, 
				&clact_res->plttproxy );
		GFL_HEAP_FreeMemory( p_buff );
	}

	// �Z���f�[�^�ǂݍ���
	{
		clact_res->p_cellbuff = GFL_ARC_DataLoadMalloc( 0,NARC_yossyegg_fall_obj_NCER, heapID );
		result = NNS_G2dGetUnpackedCellBank( clact_res->p_cellbuff, &clact_res->p_cell );
		GF_ASSERT( result );
	}

	// �Z���A�j���f�[�^�ǂݍ���
	{
		clact_res->p_cellanmbuff = GFL_ARC_DataLoadMalloc( 0,NARC_yossyegg_fall_obj_NANR, heapID );
		result = NNS_G2dGetUnpackedAnimBank( clact_res->p_cellanmbuff, &clact_res->p_cellanm );
		GF_ASSERT( result );
	}
}

