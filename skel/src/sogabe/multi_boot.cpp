
//============================================================================================
/**
 * @file	multi_boot.c
 * @brief	DS�Ń��b�V�[�̂��܂��@�}���`�u�[�g�e�@
 * @author	sogabe
 * @date	2007.05.11
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "ui.h"

#include "textprint.h"

#include "main.h"
#include "yt_common.h"
#include "yt_net.h"


#include "sample_graphic/yossyegg.naix"

#define __TITLE_H_GLOBAL__
#include "multi_boot.h"

//------------------------------------------------------------------
/**
 * @brief		���[�N�\����
 */
//------------------------------------------------------------------
typedef struct
{
	GFL_TEXT_PRINTPARAM	*textParam;
	MBUserInfo			myUser;
	u32					cwork[MB_SYSTEM_BUF_SIZE/sizeof(u32)];
	u8					*segbuf;
	int					msg_num;
}MULTI_BOOT_WORK;

//------------------------------------------------------------------
/**
 * @brief		�Q�c�f�[�^�R���g���[��
 */
//------------------------------------------------------------------
static const char font_path[] = {"src/gfl_graphic/gfl_font.dat"};

#define G2D_BACKGROUND_COL	(0x0000)
#define G2D_FONT_COL		(0x7fff)
#define G2D_FONTSELECT_COL	(0x001f)
#define G2D_FONTBG_COL		(0x3fef)

static const GFL_TEXT_PRINTPARAM default_param = 
{ NULL, 0, 0, 1, 1, 1, 0, GFL_TEXT_WRITE_16 };

#define TEXT_FRM			(GFL_BG_FRAME2_M)
#define TEXT_FRM_PRI		(0)
#define TEXT_FRM_PAL		(0)

#define	MB_DMA_NO			(2)

static	void	MsgPrint(GAME_PARAM *gp,int num,BOOL flag);
static	void	MultiBootInit(GAME_PARAM *gp);
static	void	MultiBootCallBack(u16 aid, u32 status, void *arg);

//------------------------------------------------------------------
/**
 * @brief	�e�L�X�g�f�[�^
 */
//------------------------------------------------------------------
typedef struct
{
	int			wx;
	int			wy;
	int			length;
	const char	*msg;
}TEXT_DATA;

#define	TEXT_DATA_MENU_Y	(8)
#define	TEXT_DATA_MSG_Y		(92)

static const char	text_data1[] = 
{"���b�V�[�̂��܂��@�}���`�u�[�g�Ă񂻂����[�h"};
#define	TEXT_DATA1_X	(128-(sizeof(text_data1)-1)*2)
#define	TEXT_DATA1_Y	(TEXT_DATA_MENU_Y)
#define	TEXT_DATA1_LEN	(((sizeof(text_data1)-1)/2)*8)

static const char	text_data2[] = 
{"�Ȃɂ��@�{�^���������Ɓ@�Ă񂻂����@���������܂�"};
#define	TEXT_DATA2_X	(128-(sizeof(text_data2)-1)*2)
#define	TEXT_DATA2_Y	(TEXT_DATA_MSG_Y)
#define	TEXT_DATA2_LEN	(((sizeof(text_data2)-1)/2)*8)

static const char	text_data3[] = 
{"��������́@�����Ƃ����@�܂��Ă��܂�"};
#define	TEXT_DATA3_X	(128-(sizeof(text_data3)-1)*2)
#define	TEXT_DATA3_Y	(TEXT_DATA_MSG_Y)
#define	TEXT_DATA3_LEN	(((sizeof(text_data3)-1)/2)*8)

static const char	text_data4[] = 
{"��������@�ւ񂶂��@���܂���"};
#define	TEXT_DATA4_X	(128-(sizeof(text_data4)-1)*2)
#define	TEXT_DATA4_Y	(TEXT_DATA_MSG_Y)
#define	TEXT_DATA4_LEN	(((sizeof(text_data4)-1)/2)*8)

static const char	text_data5[] = 
{"�����ւ́@����������@�������܂�"};
#define	TEXT_DATA5_X	(128-(sizeof(text_data5)-1)*2)
#define	TEXT_DATA5_Y	(TEXT_DATA_MSG_Y)
#define	TEXT_DATA5_LEN	(((sizeof(text_data5)-1)/2)*8)

static const char	text_data6[] = 
{"�����ց@�������񂿂イ"};
#define	TEXT_DATA6_X	(128-(sizeof(text_data6)-1)*2)
#define	TEXT_DATA6_Y	(TEXT_DATA_MSG_Y)
#define	TEXT_DATA6_LEN	(((sizeof(text_data6)-1)/2)*8)

static const char	text_data7[] = 
{"�����ւ́@�������񂩂��傤"};
#define	TEXT_DATA7_X	(128-(sizeof(text_data7)-1)*2)
#define	TEXT_DATA7_Y	(TEXT_DATA_MSG_Y)
#define	TEXT_DATA7_LEN	(((sizeof(text_data7)-1)/2)*8)

static	const	TEXT_DATA	text_data[]={
	{
		TEXT_DATA1_X,
		TEXT_DATA1_Y,
		TEXT_DATA1_LEN,
		text_data1,
	},
	{
		TEXT_DATA2_X,
		TEXT_DATA2_Y,
		TEXT_DATA2_LEN,
		text_data2,
	},
	{
		TEXT_DATA3_X,
		TEXT_DATA3_Y,
		TEXT_DATA3_LEN,
		text_data3,
	},
	{
		TEXT_DATA4_X,
		TEXT_DATA4_Y,
		TEXT_DATA4_LEN,
		text_data4,
	},
	{
		TEXT_DATA5_X,
		TEXT_DATA5_Y,
		TEXT_DATA5_LEN,
		text_data5,
	},
	{
		TEXT_DATA6_X,
		TEXT_DATA6_Y,
		TEXT_DATA6_LEN,
		text_data6,
	},
	{
		TEXT_DATA7_X,
		TEXT_DATA7_Y,
		TEXT_DATA7_LEN,
		text_data7,
	},
};

enum{
	TEXT_DATA1=0,
	TEXT_DATA2,
	TEXT_DATA3,
	TEXT_DATA4,
	TEXT_DATA5,
	TEXT_DATA6,
	TEXT_DATA7,
};

static	int	change_msg_num=0;

//----------------------------------------------------------------------------
/**
 *	@brief	�^�C�g��������
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
void	YT_InitMultiBoot(GAME_PARAM *gp)
{
	//�W���u���[�N���m��
	gp->job_work=GFL_HEAP_AllocClearMemory(gp->heapID,sizeof(MULTI_BOOT_WORK));

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
		GFL_BG_SetBGControl(GFL_BG_FRAME2_M, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME2_M );
		GFL_BG_SetBGControl(GFL_BG_FRAME3_M, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME3_M );
		GFL_BG_SetBGControl(GFL_BG_FRAME2_S, &TextBgCntDat[2], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME2_S );
		GFL_BG_SetBGControl(GFL_BG_FRAME3_S, &TextBgCntDat[3], GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(GFL_BG_FRAME3_S );

		// OBJ�}�b�s���O���[�h
		GX_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );
		GXS_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );

		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	}

	//��ʐ���
	GFL_ARC_UTIL_TransVramBgCharacter(0,NARC_yossyegg_VSSEL_B2_NCGR,GFL_BG_FRAME3_M,0,0,0,gp->heapID);
	GFL_ARC_UTIL_TransVramScreen(0,NARC_yossyegg_VSSEL_B2_NSCR,GFL_BG_FRAME3_M,0,0,0,gp->heapID);
	GFL_ARC_UTIL_TransVramPalette(0,NARC_yossyegg_VSSEL_BG_NCLR,PALTYPE_MAIN_BG,0,0x100,gp->heapID);

	GFL_DISP_SetDispOn();
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );

	//BMP�֘A������
	GFL_BMPWIN_Init(gp->heapID);
	gp->yossy_bmpwin=GFL_BMPWIN_Create(GFL_BG_FRAME2_M,0,0,32,32,TEXT_FRM_PAL,GFL_BMP_CHRAREA_GET_B);
	GFL_BMPWIN_MakeScreen(gp->yossy_bmpwin);
	GFL_BG_LoadScreenReq(GFL_BG_FRAME2_M);

	//�t�H���g�ǂݍ���
	GFL_TEXT_CreateSystem( font_path );
	//�p���b�g�쐬���]��
	{
		u16* plt = (u16 *)GFL_HEAP_AllocClearMemoryLo( gp->heapID, 16*2 );
		plt[0] = G2D_BACKGROUND_COL;
		plt[1] = G2D_FONT_COL;
		plt[2] = G2D_FONTBG_COL;
		GFL_BG_LoadPalette( TEXT_FRM, plt, 16*2, 0 );
		plt[1] = G2D_FONTSELECT_COL;
		GFL_BG_LoadPalette( TEXT_FRM, plt, 16*2, 16*2 );

		GFL_HEAP_FreeMemory( plt );
	}
	//�����\���p�����[�^���[�N�쐬
	{
		MULTI_BOOT_WORK	*mbw=(MULTI_BOOT_WORK *)gp->job_work;

		mbw->textParam=(GFL_TEXT_PRINTPARAM *)GFL_HEAP_AllocClearMemoryLo(gp->heapID,sizeof(GFL_TEXT_PRINTPARAM));
		*mbw->textParam=default_param;
		mbw->textParam->bmp=GFL_BMPWIN_GetBmp(gp->yossy_bmpwin);
	}

	GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN|GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB,16,0,2);

	MsgPrint(gp,TEXT_DATA1,TRUE);

	YT_JobNoSet(gp,YT_MainMultiBootNo);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^�C�g�����C������
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------

enum{
	SEQ_MB_ICHNEUMON_TRANS=0,
	SEQ_MB_ICHNEUMON_TRANS_WAIT,
	SEQ_MB_KEY_WAIT,
	SEQ_MB_CHILD_RES_WAIT,
	SEQ_MB_END_INIT,
	SEQ_MB_END,
};

void	YT_MainMultiBoot(GAME_PARAM *gp)
{
	MULTI_BOOT_WORK	*mbw=(MULTI_BOOT_WORK *)gp->job_work;

	gp->wait_work++;
	switch(gp->seq_no){
	case SEQ_MB_ICHNEUMON_TRANS:
//		GFL_NET_InitIchneumon();
		gp->seq_no++;
		break;
	case SEQ_MB_ICHNEUMON_TRANS_WAIT:
		if((GFL_NET_IsInitIchneumon()==TRUE)&&(GFL_FADE_CheckFade()==FALSE)){
			gp->seq_no++;
		}
		break;
	case SEQ_MB_KEY_WAIT:
		if(GFL_UI_KEY_GetTrg()){
			MultiBootInit(gp);
			MsgPrint(gp,TEXT_DATA2,FALSE);
			gp->seq_no=SEQ_MB_CHILD_RES_WAIT;
			mbw->msg_num=change_msg_num=TEXT_DATA3;
		}
		else{
			MsgPrint(gp,TEXT_DATA2,((gp->wait_work%50)<30));
		}
		break;
	case SEQ_MB_CHILD_RES_WAIT:
		if(change_msg_num!=mbw->msg_num){
			MsgPrint(gp,mbw->msg_num,FALSE);
			mbw->msg_num=change_msg_num;
		}
		MsgPrint(gp,mbw->msg_num,((gp->wait_work%50)<30));
		if(mbw->msg_num==TEXT_DATA7){
			MsgPrint(gp,mbw->msg_num,TRUE);
			gp->wait_work=0;
			gp->seq_no=SEQ_MB_END_INIT;
		}
		break;
	case SEQ_MB_END_INIT:
		if(gp->wait_work>40){
			GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN|GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB,0,16,2);
			gp->seq_no=SEQ_MB_END;
		}
		break;
	case SEQ_MB_END:
		if(GFL_FADE_CheckFade()==FALSE){
			GFL_NET_ExitIchneumon();
			MB_End();
			GFL_BG_Exit();
			YT_JobNoSet(gp,YT_InitTitleNo);
		}
		break;
	default:
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\������
 *	
 *	@param	gp		�Q�[���p�����[�^�|�C���^
 *	@param	num		�\�����郁�b�Z�[�W�f�[�^�i���o�[
 */
//-----------------------------------------------------------------------------
static	void	MsgPrint(GAME_PARAM *gp,int num,BOOL flag)
{
	MULTI_BOOT_WORK	*mbw=(MULTI_BOOT_WORK *)gp->job_work;

	if(flag==TRUE){
		GFL_BMP_Fill(GFL_BMPWIN_GetBmp(gp->yossy_bmpwin),text_data[num].wx-1,text_data[num].wy-1,text_data[num].length+2,10,2);
		mbw->textParam->writex=text_data[num].wx+1;
		mbw->textParam->writey=text_data[num].wy+1;
		mbw->textParam->colorF=3;
		GFL_TEXT_PrintSjisCode(text_data[num].msg,mbw->textParam);
		mbw->textParam->writex=text_data[num].wx;
		mbw->textParam->writey=text_data[num].wy;
		mbw->textParam->colorF=1;
		GFL_TEXT_PrintSjisCode(text_data[num].msg,mbw->textParam);
	}
	else{
		GFL_BMP_Fill(GFL_BMPWIN_GetBmp(gp->yossy_bmpwin),text_data[num].wx-1,text_data[num].wy-1,text_data[num].length+2,10,0);
	}
	GFL_BMPWIN_TransVramCharacter(gp->yossy_bmpwin);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�u�[�g�����f�[�^��`
 */
//-----------------------------------------------------------------------------
static	const	MBGameRegistry	mbGameList={
	"src/sample_multi_boot_rom/yossy_egg.srl",
	(u16 *)L"YossyEgg",
	(u16 *)L"PuzzleGame",
	"src/sample_multi_boot_rom/icon.char",
	"src/sample_multi_boot_rom/icon.plt",
	0x12123434,
	8,
};

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�u�[�g����������
 */
//-----------------------------------------------------------------------------
static	void	MultiBootInit(GAME_PARAM *gp)
{
	FSFile			file[1];
	MULTI_BOOT_WORK	*mbw=(MULTI_BOOT_WORK *)gp->job_work;
	OSOwnerInfo		MyInfo;
	u32				length;
	int ret;

	OS_GetOwnerInfo(&MyInfo);

	mbw->myUser.favoriteColor=MyInfo.favoriteColor;
	mbw->myUser.nameLength=MyInfo.nickNameLength;
	MI_CpuCopy8((char *)MyInfo.nickName,(char *)mbw->myUser.name,(u32)(MyInfo.nickNameLength*2));
	mbw->myUser.playerNo=0;

	(void)MB_Init(mbw->cwork,&mbw->myUser,0x003FFF20,MB_TGID_AUTO,MB_DMA_NO);
	(void)MB_SetParentCommParam(256,15);
	MB_CommSetParentStateCallback(MultiBootCallBack);
	(void)MB_StartParent(13);
	
	FS_InitFile(file);
	if(!FS_OpenFile(file,mbGameList.romFilePathp)){
		OS_TPrintf("file cannot open\n");
	}
	else{
		length=MB_GetSegmentLength(file);
		if(length==0){
			OS_TPrintf("length=0\n");
		}
		else{
			mbw->segbuf=(u8 *)GFL_HEAP_AllocMemory(gp->heapID,length);
			if(mbw->segbuf==NULL){
				OS_TPrintf("Failed AllocMemory\n");
			}
			else{
				if(!MB_ReadSegment(file,mbw->segbuf,length)){
					OS_TPrintf("Failed Extract Segment\n");
				}
				else{
					if(!MB_RegisterFile(&mbGameList,mbw->segbuf)){
						OS_TPrintf("Failed Regster File\n");
					}
				}
			}
		}
	}
	if(FS_IsFile(file)){
		FS_CloseFile(file);
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}���`�u�[�g�R�[���o�b�N����
 */
//-----------------------------------------------------------------------------
static void MultiBootCallBack(u16 aid, u32 status, void *arg)
{
	OS_TPrintf("CallBack: aid->%d status->%d\n",aid,status);
    switch (status)
    {
    case MB_COMM_PSTATE_INIT_COMPLETE:
        /* ���������� ���� */
        break;

    case MB_COMM_PSTATE_CONNECTED:
        /* �q�@����ڑ��������u�Ԃ̒ʒm */
        break;

    case MB_COMM_PSTATE_DISCONNECTED:
        /* �q�@���ڑ���؂������̒ʒm */
        break;

    case MB_COMM_PSTATE_KICKED:
        /* �q�@���L�b�N�������̒ʒm */
        break;

    case MB_COMM_PSTATE_REQ_ACCEPTED:
        /* �q�@����̃_�E�����[�h�v�����󂯂����̒ʒm */
		change_msg_num=TEXT_DATA5;
        break;

    case MB_COMM_PSTATE_SEND_PROCEED:
        /* �q�@�փo�C�i�����M���J�n���鎞�̒ʒm */
        break;

    case MB_COMM_PSTATE_SEND_COMPLETE:
        /* �q�@�ւ̃o�C�i�����M���I���������̒ʒm */
		/* �����u�[�g�v�� */
		(void)MB_CommBootRequest(aid);
		change_msg_num=TEXT_DATA7;
        break;

    case MB_COMM_PSTATE_BOOT_REQUEST:
        /* �q�@�ւ̃u�[�g�v�����M���̒ʒm */
        break;

    case MB_COMM_PSTATE_BOOT_STARTABLE:
        /* ��A�̃_�E�����[�h�������I���A�}���`�u�[�g�Ή��A�v�����N���ɂȂ������̒ʒm */
        break;

    case MB_COMM_PSTATE_REQUESTED:
        /* �q�@����G���g���[���N�G�X�g�������u�Ԃ̒ʒm */
		/* �����G���g���[���� */
		(void)MB_CommResponseRequest(aid, MB_COMM_RESPONSE_REQUEST_ACCEPT);
		change_msg_num=TEXT_DATA4;
        break;

    case MB_COMM_PSTATE_MEMBER_FULL:
        /* ����ɒB�����Q�[���Ɏq�@���ڑ����Ă������̒ʒm */
        break;

    case MB_COMM_PSTATE_END:
        /* �e�@���I���������̒ʒm */
//        prog_state = STATE_MENU;
        break;

    case MB_COMM_PSTATE_WAIT_TO_SEND:
        /* �q�@�ւ̑��M�ҋ@��Ԃ��n�܂������̒ʒm */
		/* �������M�J�n */
		(void)MB_CommStartSending(aid);
		change_msg_num=TEXT_DATA6;
        break;
    }
}
