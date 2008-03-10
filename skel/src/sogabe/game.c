
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

#include "calctool.h"

#include "sample_graphic/yossyegg.naix"

#define __GAME_H_GLOBAL__
#include "game.h"

#define	PLAYER1_READY	(0)

//#define	DMA_RASTER		//��`�L����DMA�ɂ�郉�X�^�[�i������HBlank���荞�݂ł̃��X�^�[
#ifndef DMA_RASTER
#include "gfl_use.h"
#endif

static	void	YT_MainGameAct(GAME_PARAM *gp);
static	void	YT_ExitGame(GAME_PARAM *gp);
static	void	YT_ClactResourceLoad( YT_CLACT_RES *clact_res, int flag, u32 heapID );
static	int		YT_ReadyCheck(GAME_PARAM *gp,YT_PLAYER_STATUS *ps);
static	void	YT_ReadyAct(GAME_PARAM *gp,int player_no);
static	BOOL	YT_FallCheck(GAME_PARAM *gp,YT_PLAYER_STATUS *ps);
static	void	YT_CheckFlag(GFL_TCB *tcb,void *work);
static	void	YT_AdjAllVanishCheck(GAME_PARAM *gp,int player_no,int *deka_flag,int *kara_flag);

static	u16	RasterBuffer[256+90];
static	int	raster_count=0;
#ifndef DMA_RASTER
static	void	TCB_RasterHBlank(GFL_TCB *tcb,void *work);
static	void	TCB_RasterVBlank(GFL_TCB *tcb,void *work);
static	int	raster_start=0;
static	GFL_TCB	*raster_h_blank=NULL;
static	GFL_TCB	*raster_v_blank=NULL;
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[��������
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
void	YT_InitGame(GAME_PARAM *gp)
{
    PLAYER_PARAM* pp;

    // �Z���A�N�^�[���j�b�g�쐬
	gp->clact->p_unit = GFL_CLACT_UNIT_Create( YT_CLACT_MAX, gp->heapID );

	//�G���A�}�l�[�W��������
	gp->clact_area=GFL_AREAMAN_Create(YT_CLACT_MAX,gp->heapID);
	
	//BG�V�X�e��������
	GFL_BG_Init(gp->heapID);

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
		GFL_BG_SetBGMode( &BGsys_data );
	}

	//���C����ʃt���[���ݒ�
	{
		GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME1_M
			{
				0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME2_M
			{
				0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
			},
			///<FRAME3_M
			{
				0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x4000, GX_BG_CHARBASE_0x18000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
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
		GFL_BG_SetBGControl(GFL_BG_FRAME1_M, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME1_M);
		GFL_BG_SetBGControl(GFL_BG_FRAME2_M, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME2_M);
		GFL_BG_SetBGControl(GFL_BG_FRAME3_M, &TextBgCntDat[2], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
		GFL_BG_SetBGControl(GFL_BG_FRAME2_S, &TextBgCntDat[3], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME2_S);
		GFL_BG_SetBGControl(GFL_BG_FRAME3_S, &TextBgCntDat[4], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME3_S);

		// OBJ�}�b�s���O���[�h
		GX_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );
		GXS_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );

		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	}

	//��ʐ���
	GFL_ARC_UTIL_TransVramBgCharacter(0,NARC_yossyegg_game_bg_NCGR,GFL_BG_FRAME2_M,0,0,0,gp->heapID);
	GFL_ARC_UTIL_TransVramScreen(0,NARC_yossyegg_game_bg_NSCR,GFL_BG_FRAME2_M,0,0,0,gp->heapID);
	if(gp->raster_flag){
		GFL_ARC_UTIL_TransVramBgCharacter(0,NARC_yossyegg_UMI_NCGR,GFL_BG_FRAME3_M,0,0,0,gp->heapID);
		GFL_ARC_UTIL_TransVramScreen(0,NARC_yossyegg_UMI_NSCR,GFL_BG_FRAME3_M,0,0,0,gp->heapID);
		GFL_ARC_UTIL_TransVramPalette(0,NARC_yossyegg_umi_NCLR,PALTYPE_MAIN_BG,0,0x100,gp->heapID);
	}
	else{
		GFL_ARC_UTIL_TransVramBgCharacter(0,NARC_yossyegg_YT_BG03_NCGR,GFL_BG_FRAME3_M,0,0,0,gp->heapID);
		GFL_ARC_UTIL_TransVramScreen(0,NARC_yossyegg_YT_BG03_NSCR,GFL_BG_FRAME3_M,0,0,0,gp->heapID);
		GFL_ARC_UTIL_TransVramPalette(0,NARC_yossyegg_YT_BG03_NCLR,PALTYPE_MAIN_BG,0,0x100,gp->heapID);
	}

	GFL_DISP_SetDispOn();
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );

	GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN|GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,16,0,2);

	//�Z���A�N�^�[���\�[�X�ǂݍ���
	YT_ClactResourceLoad(&gp->clact->res, gp->raster_flag, gp->heapID);

	//���b�V�[�L�����f�[�^�ǂݍ���
	gp->yossy_bmp=GFL_BMP_LoadCharacter(0,NARC_yossyegg_yossy_birth_NCGR,0,gp->heapID);

	//BMP�֘A������
	GFL_BMPWIN_Init(gp->heapID);
	gp->yossy_bmpwin=GFL_BMPWIN_Create(GFL_BG_FRAME1_M,0,0,32,32,2,GFL_BMP_CHRAREA_GET_B);
	GFL_BMPWIN_MakeScreen(gp->yossy_bmpwin);
	GFL_BG_LoadScreenReq(GFL_BG_FRAME1_M);

	//�t���O�֘A����
	{
		int	x,y;

		GFL_STD_MemClear(&gp->ps[0],sizeof(YT_PLAYER_STATUS));
		GFL_STD_MemClear(&gp->ps[1],sizeof(YT_PLAYER_STATUS));

		for(x=0;x<YT_LINE_MAX;x++){
			gp->ps[0].falltbl[x]=x;
			gp->ps[1].falltbl[x]=x;
			gp->ps[0].stoptbl[x]=x;
			gp->ps[1].stoptbl[x]=x;
		}
		for(y=0;y<YT_HEIGHT_MAX;y++){
			gp->ps[0].rensa[y]=NULL;
			gp->ps[1].rensa[y]=NULL;
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
		RTCDate	date;
		RTCTime	time;
		u32		seed;

		RTC_GetDateTime(&date,&time);

		seed=date.year+date.month*0x100+date.day*0x10000+time.hour*0x100000+(time.minute+time.second)*0x1000000;

		GFL_STD_MtRandInit(seed);
	}

	//�v���[���[������
	gp->seq_no=0;
	gp->game_seq_no[0]=0;
	gp->game_seq_no[1]=0;

    if(gp->pNetParam){  // �ʐM�ɂ��킹�ă}���I�̏��������s���܂�
        int	player_no;

        for(player_no=0;player_no<2;player_no++){
            if(GFL_NET_IsParentMachine() && (player_no == 1)){
                continue;
            }
            else if(!GFL_NET_IsParentMachine() && (player_no == 0)){
                continue;
            }
            pp = YT_InitPlayer(gp,player_no,player_no,TRUE);
            YT_InitPlayerAddTask(gp, pp, player_no);
        }
    }
    else{
        pp = YT_InitPlayer(gp,0,0,FALSE);
        YT_InitPlayerAddTask(gp, pp, 0);
		gp->ps[0].exist_flag=1;
    }

	//�Q�[���t���O�`�F�b�N�^�X�N�Z�b�g
	gp->check_tcb=GFL_TCB_AddTask(gp->tcbsys,YT_CheckFlag,gp,TCB_PRI_PLAYER);

	YT_JobNoSet(gp,YT_MainGameNo);

	GFL_SOUND_LoadHeapState(gp->mus_level[MUS_LEVEL_JINGLE]);
	if(gp->raster_flag){
		GFL_SOUND_LoadGroupData(GROUP_UMI);
		GFL_SOUND_PlayBGM(SEQ_UMI);
	}
	else{
		GFL_SOUND_LoadGroupData(GROUP_MORI);
		GFL_SOUND_PlayBGM(SEQ_MORI);
	}

    GFL_NET_ReloadIcon();

	{
		int	rad,i;

		rad=0;

		for(i=0;i<256+90;i++){
			RasterBuffer[i]=(u16)(Sin360FX(rad*FX32_ONE)/1024);
			rad+=4;
		}
	}

#ifndef DMA_RASTER
	raster_h_blank=GFUser_HIntr_CreateTCB(TCB_RasterHBlank,gp,0);
	raster_v_blank=GFUser_VIntr_CreateTCB(TCB_RasterVBlank,gp,0);
#endif

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

	GFL_TCB_Main(gp->tcbsys);

	// �Z���A�N�^�[���j�b�g�`�揈��
	GFL_CLACT_UNIT_Draw( gp->clact->p_unit );

	// �Z���A�N�^�[�V�X�e�����C������
	// �S���j�b�g�`�悪�������Ă���s���K�v������܂��B
	GFL_CLACT_Main();

#ifdef DMA_RASTER
	GFL_DMA_Start(2,(u32)&RasterBuffer[raster_count],(u32)&reg_G2_BG3VOFS,
				  GFL_DMA_ENABLE,
				  GFL_DMA_INTR_DISABLE,
				  GFL_DMA_MODE_HBLANK,
				  GFL_DMA_SEND_BIT_16,
				  GFL_DMA_CONTINUE_MODE_ON,
				  GFL_DMA_SAR_INC,
				  GFL_DMA_DAR_FIX,
				  1);
	if(++raster_count>=90){
		raster_count=0;
	}
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[���V�[�P���X����
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
static	void	YT_MainGameAct(GAME_PARAM *gp)
{
	int	player_no;

	switch(gp->seq_no){
	case SEQ_GAME_PLAY:
		for(player_no=0;player_no<2;player_no++){
			if(gp->pNetParam){
				if(GFL_NET_IsParentMachine() && (player_no == 1)){
					continue;
				}
				else if(!GFL_NET_IsParentMachine() && (player_no == 0)){
					continue;
				}
			}
			else if(gp->ps[player_no].exist_flag==0){
				continue;
			}
			if(gp->ps[player_no].status.win_lose_flag){
				gp->seq_no=SEQ_GAME_OVER;
				gp->wait_work=120;
			}
			switch(gp->game_seq_no[player_no]){
			case SEQ_GAME_START_WAIT:
				if(GFL_FADE_CheckFade()==FALSE){
					gp->game_seq_no[player_no]++;
				}
				break;
			case SEQ_GAME_READY_CHECK:
				switch(YT_ReadyCheck(gp,&gp->ps[player_no])){
				case YT_READY_MAKE:
					gp->ps[player_no].fall_count++;
					YT_ReadyAct(gp,player_no);
				case YT_READY_ALREADY:
					gp->ps[player_no].fall_wait=YT_FALL_WAIT;
					gp->game_seq_no[player_no]++;
					break;
				default:
					break;
				}
				break;
			case SEQ_GAME_FALL_CHECK:
				if(YT_FallCheck(gp,&gp->ps[player_no])==TRUE){
					if(--gp->ps[player_no].fall_wait==0){
						gp->game_seq_no[player_no]=SEQ_GAME_READY_CHECK;
					}
				}
				break;
			}
		}
		break;
	case SEQ_GAME_OVER:
		if(gp->wait_work){
			gp->wait_work--;
		}
		else{
			if(GFL_UI_KEY_GetTrg()){
				GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN|GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,0,16,2);
				gp->seq_no=SEQ_GAME_TO_TITLE;
			}
		}
		break;
	case SEQ_GAME_TO_TITLE:
		if(GFL_FADE_CheckFade()==FALSE){
			YT_ExitGame(gp);
		}
		break;
	}
}
	
//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[���I������
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
static	void	YT_ExitGame(GAME_PARAM *gp)
{
	//�Q�[���t���O�`�F�b�N�^�X�N�j��
	GFL_TCB_DeleteTask(gp->check_tcb);

	//BMP�֘A�I��
	GFL_BMPWIN_Delete(gp->yossy_bmpwin);
	GFL_BMPWIN_Exit();

	//���b�V�[�L�����f�[�^�j��
	GFL_HEAP_FreeMemory(gp->yossy_bmp);

	//BG�V�X�e���I��
	GFL_BG_Exit();

	//�G���A�}�l�[�W���j��
	GFL_AREAMAN_Delete(gp->clact_area);

    // �Z���A�N�^�[���j�b�g�j��
	GFL_CLACT_UNIT_Delete(gp->clact->p_unit);

#ifdef DMA_RASTER
	GFL_DMA_Stop(2);
#else
	GFL_TCB_DeleteTask(raster_h_blank);
	GFL_TCB_DeleteTask(raster_v_blank);
#endif

	YT_JobNoSet(gp,YT_InitTitleNo);
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
					GFL_CLACTPOS	pos;
					GFL_CLACT_WK_GetWldPos(gp->clact->clact_work[ps->fall[x][y]->clact_no],&pos);
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
	int	deka_flag=0,kara_flag=0;

	YT_AdjAllVanishCheck(gp,player_no,&deka_flag,&kara_flag);

	while(i){
		line=__GFL_STD_MtRand()%4;
		if(gp->ps[player_no].ready[line][0]){
			continue;
		}
		type=__GFL_STD_MtRand()%4;
		//�f�J�L�����́A�m����Ⴍ����
		if((type==YT_CHR_TERESA)&&(deka_flag!=2)){
			if((__GFL_STD_MtRand()%5==0)||(deka_flag==1)){
				type=YT_CHR_DEKATERESA;
			}
		}
		else{
			//�^�}�S�̊k�����m��
			if((__GFL_STD_MtRand()%5==0)&&(kara_flag==0)){
				type=YT_CHR_GREEN_EGG_U+__GFL_STD_MtRand()%4;
			}
		}
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
static void YT_ClactResourceLoad( YT_CLACT_RES *clact_res, int flag, u32 heapID )
{
	BOOL result;
	void* p_buff;
	NNSG2dCharacterData* p_char;
	NNSG2dPaletteData* p_pltt;
	
	// �L�����N�^�f�[�^�ǂݍ��݁��]��
	{
		if(flag){
			p_buff = GFL_ARC_LoadDataAlloc( 0,NARC_yossyegg_O_SEA3_NCGR, heapID );
		}
		else{
			p_buff = GFL_ARC_LoadDataAlloc( 0,NARC_yossyegg_O_WOODS3_NCGR, heapID );
		}
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
		p_buff = GFL_ARC_LoadDataAlloc( 0,NARC_yossyegg_OBJ_COL_NCLR, heapID );
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
		if(flag){
			clact_res->p_cellbuff = GFL_ARC_LoadDataAlloc( 0,NARC_yossyegg_fall_obj_umi_NCER, heapID );
		}
		else{
			clact_res->p_cellbuff = GFL_ARC_LoadDataAlloc( 0,NARC_yossyegg_fall_obj_NCER, heapID );
		}
		result = NNS_G2dGetUnpackedCellBank( clact_res->p_cellbuff, &clact_res->p_cell );
		GF_ASSERT( result );
	}

	// �Z���A�j���f�[�^�ǂݍ���
	{
		if(flag){
			clact_res->p_cellanmbuff = GFL_ARC_LoadDataAlloc( 0,NARC_yossyegg_fall_obj_umi_NANR, heapID );
		}
		else{
			clact_res->p_cellanmbuff = GFL_ARC_LoadDataAlloc( 0,NARC_yossyegg_fall_obj_NANR, heapID );
		}
		result = NNS_G2dGetUnpackedAnimBank( clact_res->p_cellanmbuff, &clact_res->p_cellanm );
		GF_ASSERT( result );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[���t���O�`�F�b�N�^�X�N
 */
//-----------------------------------------------------------------------------
static	void	YT_CheckFlag(GFL_TCB *tcb,void *work)
{
	GAME_PARAM			*gp=(GAME_PARAM *)work;
	YT_PLAYER_STATUS	*ps;
	int					player_no;

	for(player_no=0;player_no<2;player_no++){
		ps=&gp->ps[player_no];
		//��]�����I���`�F�b�N
		if(ps->status.rotate_flag){
			{
				int					i;
				int					left_line_stop;
				int					right_line_stop;
				int					left_line_fall;
				int					right_line_fall;
				FALL_CHR_PARAM		*fcp;

				left_line_stop=ps->stoptbl[ps->status.rotate_flag-1];
				right_line_stop=ps->stoptbl[ps->status.rotate_flag];
				left_line_fall=ps->falltbl[ps->status.rotate_flag-1];
				right_line_fall=ps->falltbl[ps->status.rotate_flag];

				for(i=0;i<YT_HEIGHT_MAX;i++){
					fcp=ps->stop[left_line_stop][i];
					if(fcp){
						if(fcp->rotate_flag){
							break;
						}
					}
					fcp=ps->stop[right_line_stop][i];
					if(fcp){
						if(fcp->rotate_flag){
							break;
						}
					}
					fcp=ps->fall[left_line_fall][i];
					if(fcp){
						if(fcp->rotate_flag){
							break;
						}
					}
					fcp=ps->fall[right_line_fall][i];
					if(fcp){
						if(fcp->rotate_flag){
							break;
						}
					}
				}
				if(i==YT_HEIGHT_MAX){
					ps->status.rotate_flag=0;
				}
			}
		}
		//�Ђ�����Ԃ������I���`�F�b�N
		if(ps->status.overturn_flag){
			{
				int					x,y,line;
				FALL_CHR_PARAM		*fcp;

				for(x=0;x<YT_LINE_MAX;x++){
					line=ps->stoptbl[x];
					if(ps->status.overturn_flag&(1<<x)){
						for(y=0;y<YT_HEIGHT_MAX;y++){
							fcp=ps->stop[line][y];
							if(fcp){
								if(fcp->overturn_flag){
									break;
								}
							}
						}
					}
				}
				if((x==YT_LINE_MAX)&&(y==YT_HEIGHT_MAX)){
					ps->status.overturn_flag=0;
				}
			}
		}
		//�^�}�S�����`�F�b�N
		if((ps->status.egg_make_check_flag)&&(ps->status.overturn_flag==0)){
			YT_EggMakeCheck(ps);
		}
		//�^�}�S�������`�F�b�N
		if((ps->status.egg_make_flag)&&(ps->egg_make_count==0)){
			ps->status.egg_make_flag=0;
		}
		//�A���������`�F�b�N
		if(ps->status.rensa_flag){
			{
				int					y;
				FALL_CHR_PARAM		*fcp;

				for(y=0;y<YT_HEIGHT_MAX;y++){
					fcp=ps->rensa[y];
					if(fcp){
						break;
					}
				}
				if(y==YT_HEIGHT_MAX){
					ps->status.rensa_flag=0;
				}
			}
		}
		//�Q�[���I�[�o�[�`�F�b�N
		if(ps->status.no_active_flag==0){
			{
				int				x;
				FALL_CHR_PARAM	*fcp;

				for(x=0;x<YT_LINE_MAX;x++){
					fcp=ps->stop[x][YT_HEIGHT_MAX-1];
					if(fcp){

                        if(gp->pNetParam){ //�ʐM���Ă�ꍇ�͏��s�𑗂�
                            YT_NET_SendGameResult(player_no,FALSE,gp->pNetParam);
                        }
                        else{
                            ps->status.win_lose_flag|=YT_GAME_LOSE;
                            gp->ps[player_no^1].status.win_lose_flag|=YT_GAME_WIN;
                        }
					}
				}
			}
		}
		//�S�����`�F�b�N
		if((ps->status.no_active_flag==0)&&(ps->fall_count>2)){
			{
				int	x,y;

				for(x=0;x<YT_LINE_MAX;x++){
					for(y=0;y<YT_HEIGHT_MAX;y++){
						if(ps->stop[x][y]){
							break;
						}
					}
					if(y<YT_HEIGHT_MAX){
						break;
					}
				}
				if((x==YT_LINE_MAX)&&(y==YT_HEIGHT_MAX)){
					switch(YT_ReadyCheck(gp,ps)){
					case YT_READY_ALREADY:
						for(x=0;x<YT_LINE_MAX;x++){
							for(y=0;y<YT_HEIGHT_MAX;y++){
								if(ps->fall[x][y]){
									break;
								}
							}
							if(y<YT_HEIGHT_MAX){
								break;
							}
						}
						if((x!=YT_LINE_MAX)||(y!=YT_HEIGHT_MAX)){
							break;
						}
					case YT_READY_WAIT:

                        if(gp->pNetParam){ //�ʐM���Ă�ꍇ�͏��s�𑗂�
                            YT_NET_SendGameResult(player_no,TRUE,gp->pNetParam);
                        }
                        else{
                            ps->status.win_lose_flag|=YT_GAME_WIN;
                            gp->ps[player_no^1].status.win_lose_flag|=YT_GAME_LOSE;
                        }
					default:
						break;
					}
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�S�������₷������`�F�b�N�i��莞�ԉ߂�����A�S�������₷������悤�ɁA�����L�����𒲐��j
 *
 *	@param[in]	gp			�Q�[���p�����[�^�\����
 *	@param[in]	player_no	�`�F�b�N����v���[���[�i���o�[
 *	@param[out]	deka_flag	�f�J�L������I������t���O
 *	@param[out]	kara_flag	�^�}�S�̊k��I������t���O
 */
//-----------------------------------------------------------------------------
static	void	YT_AdjAllVanishCheck(GAME_PARAM *gp,int player_no,int *deka_flag,int *kara_flag)
{
	FALL_CHR_PARAM	*fcp_stop,*fcp_fall;
	int				x,y;
	int				deka_count=0,kara_count=0,stop_count=0;

	if(gp->ps[player_no].fall_count<YT_ADJ_FALL_COUNT){
		return;
	}

	for(x=0;x<YT_LINE_MAX;x++){
		for(y=0;y<YT_HEIGHT_MAX;y++){
			fcp_stop=gp->ps[player_no].stop[x][y];
			fcp_fall=gp->ps[player_no].fall[x][y];
			if(fcp_stop){
				stop_count++;
				switch(fcp_stop->type){
				case YT_CHR_DEKATERESA:
					deka_count++;
					break;
				case YT_CHR_GREEN_EGG_U:
				case YT_CHR_GREEN_EGG_D:
				case YT_CHR_RED_EGG_U:
				case YT_CHR_RED_EGG_D:
					kara_count++;
				default:
					break;
				}
			}
			if(fcp_fall){
				switch(fcp_fall->type){
				case YT_CHR_DEKATERESA:
					deka_count++;
					break;
				case YT_CHR_GREEN_EGG_U:
				case YT_CHR_GREEN_EGG_D:
				case YT_CHR_RED_EGG_U:
				case YT_CHR_RED_EGG_D:
					kara_count++;
				default:
					break;
				}
			}
		}
	}
	if(deka_count==0){
		if(stop_count&1){
			deka_flag[0]=1;
		}
		else{
			deka_flag[0]=2;
		}
	}
	if(kara_count==0){
		kara_flag[0]=1;
	}
}

#ifndef	DMA_RASTER
//----------------------------------------------------------------------------
/**
 *	@brief	���荞�ݏ����ɂ�郉�X�^�[�X�N���[��
 *
 *	@param[in]	gp			�Q�[���p�����[�^�\����
 *	@param[in]	player_no	�`�F�b�N����v���[���[�i���o�[
 *	@param[out]	deka_flag	�f�J�L������I������t���O
 *	@param[out]	kara_flag	�^�}�S�̊k��I������t���O
 */
//-----------------------------------------------------------------------------
static	void	TCB_RasterHBlank(GFL_TCB *tcb,void *work)
{
	GAME_PARAM *gp=(GAME_PARAM *)work;

	if(gp->raster_flag){
		reg_G2_BG3VOFS=RasterBuffer[raster_count];
		raster_count++;
	}
}

static	void	TCB_RasterVBlank(GFL_TCB *tcb,void *work)
{
	GAME_PARAM *gp=(GAME_PARAM *)work;

	if(gp->raster_flag){
		if(++raster_start==90){
			raster_start=0;
		}
		raster_count=raster_start;
	}
}

#endif
