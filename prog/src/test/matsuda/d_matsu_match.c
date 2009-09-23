//==============================================================================
/**
 * @file	d_matsu_match.c
 * @brief	�ԊO���}�b�`���O
 * @author	matsuda
 * @date	2008.11.12(��)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include "system\main.h"
#include <backup_system.h>
#include "savedata/contest_savedata.h"
#include "savedata/save_tbl.h"
#include "savedata/save_control.h"

#include "net\network_define.h"

#include "print\printsys.h"
#include "print\gf_font.h"
#include "arc_def.h"
#include "message.naix"
#include "test_graphic\d_taya.naix"
#include "msg\msg_d_matsu.h"
#include "test/performance.h"
#include "font/font.naix"
#include "pokeicon/pokeicon.h"
#include "system\gfl_use.h"
#include "gamesystem/msgspeed.h"


//==============================================================================
//	�萔��`
//==============================================================================
///�G���g���[�ő�l��
#define ENTRY_MAX		(4)
///�q�̍ő吔
#define CHILD_MAX		(ENTRY_MAX - 1)

///�ԊO�����M�R�}���h
enum{
	//�q���瑗�M
	CMD_CHILD_PROFILE = 0,		///<�q�̃v���t�B�[��
	
	//�e���瑗�M
	CMD_PARENT_PROFILE = 2,		///<�e�̃v���t�B�[��
	CMD_PARENT_SUCCESS = 4,		///<�S�Ă̎�M����
};

enum{
	MY_CHILD,		///<�q�ł���
	MY_PARENT,		///<�e�ł���
};

///�|�P�����A�C�R���̃p���b�g�W�J�ʒu
#define D_MATSU_ICON_PALNO		(0)
///�A�N�^�[�ő吔
#define D_MATSU_ACT_MAX			(64)

//==============================================================================
//	�\���̒�`
//==============================================================================
typedef struct{
	u8 initialize;
	u8 connect;
	int connect_wait;
	u8 seq;
	u8 success;			///<TRUE:����M�������Đؒf����
}DM_IRC_WORK;

///�ԊO���G���g���[���ɑ��M����f�[�^
typedef struct{
	u16 name[11];
	u16 name_len;
	
	u8 parent_MacAddress[6];
	u8 oya_child;
	u8 padding;
}IRC_MATCH_ENTRY_PARAM;


typedef struct{
	GFL_BMPWIN		*win;
	GFL_BMP_DATA	*bmp;
	PRINT_UTIL		printUtil[1];
	BOOL			message_req;		///<TRUE:���b�Z�[�W���N�G�X�g��������
}DM_MSG_DRAW_WIN;

typedef struct {

	u16		seq;
	HEAPID	heapID;
	int debug_mode;
	int timer;
	
	//�Z�[�u�֘A
	GFL_SAVEDATA *sv_normal;	///<�m�[�}���Z�[�u�f�[�^�ւ̃|�C���^
	
	//�ʐM
	BOOL connect_ok;
	int send_key;
	
	u8 huge_data[0x1000];
	u8 receive_huge_data[2][0x1000];
	volatile u16 cs;
	
	DM_IRC_WORK irc;
	u8 oya;				///<1:�e�@0:�q
	IRC_MATCH_ENTRY_PARAM my_profile;
	IRC_MATCH_ENTRY_PARAM child_profile[CHILD_MAX];
	IRC_MATCH_ENTRY_PARAM parent_profile;
	u8 entry_num;		///<�G���g���[�ς݂̐�
	BOOL connect_max;	///<TRUE:�ʐM�ő�l���ɒB����
	s32 msg_pos_x[ENTRY_MAX];
	s32 msg_pos_y[ENTRY_MAX];
	
	GFL_FONT		*fontHandle;
	PRINT_QUE		*printQue;
	PRINT_STREAM	*printStream;
	PRINT_STREAM	*ps_entry[ENTRY_MAX];
	GFL_MSGDATA		*mm;
	STRBUF			*strbuf;
	STRBUF			*strbuf_entry[ENTRY_MAX];
	GFL_TCBLSYS		*tcbl;
	DM_MSG_DRAW_WIN drawwin[ENTRY_MAX + 1];	//+1 = ���C�����b�Z�[�W�E�B���h�E
	
	//�A�N�^�[
	GFL_CLUNIT *clunit;
	GFL_CLWK	*clwk[ENTRY_MAX];
	NNSG2dImagePaletteProxy	icon_pal_proxy;
	NNSG2dCellDataBank	*icon_cell_data;
	NNSG2dAnimBankData	*icon_anm_data;
	void	*icon_cell_res;
	void	*icon_anm_res;
	GFL_TCB *vintr_tcb;
	u32 icon_cgr_id;
	u32 icon_pltt_id;
	u32 icon_cell_id;
}D_MATSU_WORK;


//==============================================================================
//	���[�J���ϐ�
//==============================================================================
D_MATSU_WORK * glb_d_matsu = NULL;


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static BOOL DebugMatsuda_IrcMatch(D_MATSU_WORK *wk);
static BOOL DebugMatsuda_Wireless(D_MATSU_WORK *wk);
static void DM_IRC_ReceiveCallback(u8 *data, u8 size, u8 command, u8 value);
static void Local_EntryAdd(D_MATSU_WORK *wk, IRC_MATCH_ENTRY_PARAM *profile);
static void Local_ParentProfileSet(D_MATSU_WORK *wk, IRC_MATCH_ENTRY_PARAM *profile);
static void Local_MessagePut(D_MATSU_WORK *wk, int win_index, STRBUF *strbuf, int x, int y);

static void* _netBeaconGetFunc(void* pWork);
static int _netBeaconGetSizeFunc(void* pWork);
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo);
static void _initCallBack(void* pWork);
static void _connectCallBack(void* pWork);
static void _endCallBack(void* pWork);
static void _RecvMoveData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _RecvHugeData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static u8 * _RecvHugeBuffer(int netID, void* pWork, int size);
static void _RecvKeyData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _RecvMyProfile(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void Local_PokeIconCommonDataSet(D_MATSU_WORK *wk);
static void Local_PokeIconCommonDataFree(D_MATSU_WORK *wk);
static void Local_PokeIconAdd(D_MATSU_WORK *wk, int monsno, int net_id);
static void VintrTCB_VblankFunc(GFL_TCB *tcb, void *work);


//==============================================================================
//	�f�[�^
//==============================================================================
///�|�P�����A�C�R����CLWK�����f�[�^
static const GFL_CLWK_DATA PokeIconClwkData = {
	0, 0, 					//���W(XY)
	POKEICON_ANM_HPMAX,		//�A�j���V�[�P���X
	10,						//�\�t�g�v���C�I���e�B
	0,						//BG�v���C�I���e�B
};



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   proc		
 * @param   seq		
 * @param   pwk		
 * @param   mywk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT DebugMatsudaMainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_0_F,			// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_0123_E,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
	};

	D_MATSU_WORK* wk;
	
	DEBUG_PerformanceSetActive(FALSE);
	
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MATSUDA_DEBUG, 0x70000 );
	wk = GFL_PROC_AllocWork( proc, sizeof(D_MATSU_WORK), HEAPID_MATSUDA_DEBUG );
	glb_d_matsu = wk;
	MI_CpuClear8(wk, sizeof(D_MATSU_WORK));
	wk->heapID = HEAPID_MATSUDA_DEBUG;
	{
		int i;
		volatile u16 cs = 0;
		u8 maca[6];
		OS_GetMacAddress(maca);
		for(i = 0; i < 6; i++){
			OS_TPrintf("mac addrs = %x %x %x %x %x %x\n", maca[0], maca[1], maca[2], maca[3], maca[4], maca[5]);
		}
		for(i = 0; i < 0x1000; i++){
			if(i < 0x5000){
				wk->huge_data[i] = maca[1];
			}
			else{
				wk->huge_data[i] = maca[2];
			}
			cs += wk->huge_data[i];
			if(i % 0x100 == 0 || i == 0x1000-1 || i == 0x1000-2){
				OS_TPrintf("cs = %x, huge %x %x\n", cs, i, wk->huge_data[i]);
			}
		}
		wk->cs = cs;
		OS_TPrintf("����f�[�^�`�F�b�N�T�� = 0x%x\n", wk->cs);
	}
	
//	GFL_ARC_Init( arcFiles, NELEMS(arcFiles) );	gfl_use.c��1�񂾂��������ɕύX

	GFL_DISP_SetBank( &vramBank );

	// �e����ʃ��W�X�^������
	G2_BlendNone();

	// BGsystem������
	GFL_BG_Init( wk->heapID );
	GFL_BMPWIN_Init( wk->heapID );
	GFL_FONTSYS_Init();

	//�a�f���[�h�ݒ�
	{
		static const GFL_BG_SYS_HEADER sysHeader = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysHeader );
	}

	// �ʃt���[���ݒ�
	{
		static const GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x5800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};

		GFL_BG_SetBGControl( GFL_BG_FRAME0_M,   &bgcntText,   GFL_BG_MODE_TEXT );
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S,   &bgcntText,   GFL_BG_MODE_TEXT );

		GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_ON );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME3_M,   VISIBLE_OFF );

//		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0x22, wk->heapID );
		GFL_BG_FillCharacter( GFL_BG_FRAME0_M, 0xff, 1, 0 );

//		void GFL_BG_FillScreen( u8 frmnum, u16 dat, u8 px, u8 py, u8 sx, u8 sy, u8 mode )
		GFL_BG_FillScreen( GFL_BG_FRAME0_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

		GFL_BG_FillCharacter( GFL_BG_FRAME0_S, 0x00, 1, 0 );
		GFL_BG_FillScreen( GFL_BG_FRAME0_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
	}

	// �㉺��ʐݒ�
	GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );

	GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

	{//���b�Z�[�W�`��ׂ̈̏���
		int i;

		wk->drawwin[ENTRY_MAX].win = GFL_BMPWIN_Create( GFL_BG_FRAME0_M, 0, 0, 32, 10, 0, GFL_BMP_CHRAREA_GET_F );
		wk->drawwin[ENTRY_MAX].bmp = GFL_BMPWIN_GetBmp(wk->drawwin[ENTRY_MAX].win);
		GFL_BMP_Clear( wk->drawwin[ENTRY_MAX].bmp, 0xff );
		GFL_BMPWIN_MakeScreen( wk->drawwin[ENTRY_MAX].win );
		GFL_BMPWIN_TransVramCharacter( wk->drawwin[ENTRY_MAX].win );
		PRINT_UTIL_Setup( wk->drawwin[ENTRY_MAX].printUtil, wk->drawwin[ENTRY_MAX].win );
		for(i = 0; i < ENTRY_MAX; i++){
			wk->drawwin[i].win = GFL_BMPWIN_Create( GFL_BG_FRAME0_M, 0, 12+i*2, 32, 2, 0, GFL_BMP_CHRAREA_GET_F);
			wk->drawwin[i].bmp = GFL_BMPWIN_GetBmp(wk->drawwin[i].win);
			GFL_BMP_Clear( wk->drawwin[i].bmp, 0xff );
			GFL_BMPWIN_MakeScreen( wk->drawwin[i].win );
			GFL_BMPWIN_TransVramCharacter( wk->drawwin[i].win );
			PRINT_UTIL_Setup( wk->drawwin[i].printUtil, wk->drawwin[i].win );
		}

		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

		wk->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_nftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

//		PRINTSYS_Init( wk->heapID );
		wk->printQue = PRINTSYS_QUE_Create( wk->heapID );

		wk->mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_matsu_dat, wk->heapID );
		wk->strbuf = GFL_STR_CreateBuffer( 1024, wk->heapID );
		for(i = 0; i < ENTRY_MAX; i++){
			wk->strbuf_entry[i] = GFL_STR_CreateBuffer( 64, wk->heapID );
		}

		wk->tcbl = GFL_TCBL_Init( wk->heapID, wk->heapID, 4, 32 );

		GFL_MSGSYS_SetLangID( 1 );	//JPN_KANJI
	}

	//�t�H���g�p���b�g�]��
	GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
		0, 0x20, HEAPID_MATSUDA_DEBUG);

	//�A�N�^�[�ݒ�
	{
		GFL_CLSYS_INIT clsys_init = GFL_CLSYSINIT_DEF_DIVSCREEN;
		
		clsys_init.oamst_main = GFL_CLSYS_OAMMAN_INTERVAL;	//�ʐM�A�C�R���̕�
		clsys_init.oamnum_main = 128-GFL_CLSYS_OAMMAN_INTERVAL;
		clsys_init.tr_cell = D_MATSU_ACT_MAX;
		GFL_CLACT_SYS_Create(&clsys_init, &vramBank, HEAPID_MATSUDA_DEBUG);
		
		wk->clunit = GFL_CLACT_UNIT_Create(D_MATSU_ACT_MAX, 0, HEAPID_MATSUDA_DEBUG);
		GFL_CLACT_UNIT_SetDefaultRend(wk->clunit);

		//�|�P�����A�C�R�����ʃf�[�^�o�^
		Local_PokeIconCommonDataSet(wk);
		
		//OBJ�\��ON
		GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
	}

	wk->vintr_tcb = GFUser_VIntr_CreateTCB(VintrTCB_VblankFunc, wk, 5);

	return GFL_PROC_RES_FINISH;
}
//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugMatsudaMainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	D_MATSU_WORK* wk = mywk;
	BOOL ret = 0;
	int i;
	BOOL que_ret;
	
	GFL_TCBL_Main( wk->tcbl );
	que_ret = PRINTSYS_QUE_Main( wk->printQue );
	for(i = 0; i < ENTRY_MAX+1; i++){
		if( PRINT_UTIL_Trans( wk->drawwin[i].printUtil, wk->printQue ) == FALSE){
			//return GFL_PROC_RES_CONTINUE;
		}
		else{
			if(que_ret == TRUE && wk->drawwin[i].message_req == TRUE){
	//			GFL_BMP_Clear( wk->bmp, 0xff );
				GFL_BMPWIN_TransVramCharacter( wk->drawwin[i].win );
				wk->drawwin[i].message_req = FALSE;
			}
		}
	}
	
	switch(wk->debug_mode){
	case 0:		//�ԊO���}�b�`���O
		ret = DebugMatsuda_IrcMatch(wk);
		if(ret == TRUE){
			wk->seq = 0;
			wk->debug_mode = 1;
		}
		break;
	case 1:		//���C�����X�ʐM
		ret = DebugMatsuda_Wireless(wk);
		break;
	default:
		ret = TRUE;
	}
	
	if(ret == TRUE){
//		return GFL_PROC_RES_FINISH;
	}

	GFL_CLACT_SYS_Main();
	return GFL_PROC_RES_CONTINUE;
}
//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugMatsudaMainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	D_MATSU_WORK* wk = mywk;

	GFL_TCB_DeleteTask(wk->vintr_tcb);
	
	Local_PokeIconCommonDataFree(wk);

	GFL_CLACT_UNIT_Delete(wk->clunit);
	GFL_CLACT_SYS_Delete();

	GFL_FONT_Delete(wk->fontHandle);
	GFL_TCBL_Exit(wk->tcbl);
	
	GFL_BG_Exit();
	GFL_BMPWIN_Exit();

	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_MATSUDA_DEBUG);
	
	return GFL_PROC_RES_FINISH;
}

static void VintrTCB_VblankFunc(GFL_TCB *tcb, void *work)
{
	GFL_CLACT_SYS_VBlankFunc();
}


//==============================================================================
//	
//==============================================================================
static const NetRecvFuncTable _CommPacketTbl[] = {
//    {_RecvMoveData,        GFL_NET_COMMAND_SIZE( 0 ), NULL},    ///NET_CMD_MOVE
//    {_RecvHugeData,        GFL_NET_COMMAND_SIZE( 0 ), NULL},    ///NET_CMD_HUGE
//    {_RecvKeyData,         GFL_NET_COMMAND_SIZE( 4 ), NULL},    ///NET_CMD_KEY
    {_RecvMoveData,         NULL},    ///NET_CMD_MOVE
    {_RecvHugeData,         _RecvHugeBuffer},    ///NET_CMD_HUGE
    {_RecvKeyData,          NULL},    ///NET_CMD_KEY
    {_RecvMyProfile,     NULL},    ///NET_CMD_MY_PROFILE
};

enum{
	NET_CMD_MOVE = GFL_NET_CMD_DEBUG_MATSUDA,
	NET_CMD_HUGE,
	NET_CMD_KEY,
	NET_CMD_MY_PROFILE,
};

#define _MAXNUM   (4)         // �ő�ڑ��l��
#define _MAXSIZE  (32)        // �ő呗�M�o�C�g��
#define _BCON_GET_NUM (16)    // �ő�r�[�R�����W��


static const GFLNetInitializeStruct aGFLNetInit = {
    _CommPacketTbl,  // ��M�֐��e�[�u��
    NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
    NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
    NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    _netBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    _netBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    _netBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
    FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    _endCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g	
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
    0x532,//0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_IRC,   //��check�@�ԊO���ʐM�p��create�����HEAPID
    GFL_WICON_POSX, GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    TRUE,     // CRC�v�Z
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_WIRELESS,//GFL_NET_TYPE_IRC,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_DEBUG_MATSUDA_SERVICEID,  //GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
    0,//MP�e�ő�T�C�Y 512�܂�
    0,//dummy
};

//--------------------------------------------------------------
/**
 * @brief   �ԊO���ʐM���C��
 *
 * @param   wk		
 * @param   irc		
 *
 * @retval  TRUE:�q�̃G���g���[�������� or �e�̃v���t�B�[�����󂯎����
 */
//--------------------------------------------------------------
static BOOL DM_IrcMain(D_MATSU_WORK *wk, DM_IRC_WORK *irc)
{
	BOOL ret = 0;
	
	if(irc->initialize == TRUE){
		IRC_Move();
	}
	
	if(irc->connect == TRUE){
		if(IRC_IsConnect() == FALSE){
			irc->connect = FALSE;
			OS_TPrintf("�ؒf���ꂽ\n");
			irc->seq = 0;
		}
	}
	else{
		if(IRC_IsConnect() == TRUE){
			irc->connect = TRUE;
			irc->connect_wait = 0;
			OS_TPrintf("�ڑ�����\n");
		}
		else{
			if(wk->oya == MY_CHILD){
				irc->connect_wait++;
				if(irc->connect_wait > 60){
					irc->connect_wait = 0;
					irc->seq = 0;
					OS_TPrintf("�Ȃ��Ȃ��ڑ����Ȃ��̂ōēx���������Đڑ��ɍs��\n");
				}
			}
		}
	}
	
	if(irc->success == TRUE && irc->connect == FALSE){
		irc->success = FALSE;
		return TRUE;
	}
	
	switch( irc->seq ){
	case 0:
		OS_TPrintf("IRC������\n");
		GFL_STD_MemClear(&wk->irc, sizeof(DM_IRC_WORK));
		IRC_Init();
		IRC_SetRecvCallback(DM_IRC_ReceiveCallback);
		if(wk->oya == MY_CHILD){	//�q���ʐM���n�߂�B�e�͏�Ɏ�M��
			IRC_Connect();
		}
		irc->initialize = TRUE;
		irc->seq++;
		break;
	case 1:
		if(irc->connect == TRUE){
			//if(wk->oya == MY_PARENT){	//��M���ł���e����ʐM�J�n
			if(IRC_IsSender() == FALSE){	//��M���͒ʏ�A�e
				IRC_Send((u8*)&wk->my_profile, sizeof(IRC_MATCH_ENTRY_PARAM), CMD_PARENT_PROFILE, 0);
			}
			irc->seq++;
		}
		break;
	case 2:
		if(irc->success == TRUE && irc->connect == FALSE){
			irc->seq = 0;
		}
		break;
	}
	
	return ret;
}


static void _endCallBackSeq(void* vwk)
{
    D_MATSU_WORK *wk = vwk;
    wk->seq++;
}

//--------------------------------------------------------------
/**
 * @brief   �ԊO���ʐM�e�X�g
 *
 * @param   wk		
 *
 * @retval  TRUE:�I���B�@FALSE:�����p����
 */
//--------------------------------------------------------------
static BOOL DebugMatsuda_IrcMatch(D_MATSU_WORK *wk)
{
	BOOL ret, irc_ret = 0;
	int msg_id;
	
	GF_ASSERT(wk);

    OS_TPrintf("-- %d --\n",wk->seq);
	switch(wk->seq){
    case 0:
		{
			GFLNetInitializeStruct net_ini_data;
			
			net_ini_data = aGFLNetInit;
            net_ini_data.bNetType = GFL_NET_TYPE_IRC_WIRELESS;
			GFL_NET_Init(&net_ini_data, _initCallBack, wk);	//�ʐM������
		}
		wk->seq++;
        break;
	case 1:
		{
			OSOwnerInfo info;
			int i;
			
			OS_GetOwnerInfo( &info );
			for(i = 0; i < 11; i++){
				wk->my_profile.name[i] = info.nickName[i];
			}
			wk->my_profile.name_len = info.nickNameLength;
			OS_TPrintf("���O�̒���=%d\n", wk->my_profile.name_len);
			OS_GetMacAddress(wk->my_profile.parent_MacAddress);
			OS_TPrintf("MAC: %d, %d, %d, %d, %d, %d\n", wk->my_profile.parent_MacAddress[0], wk->my_profile.parent_MacAddress[1], wk->my_profile.parent_MacAddress[2], wk->my_profile.parent_MacAddress[3], wk->my_profile.parent_MacAddress[4], wk->my_profile.parent_MacAddress[5]);
		}

		OS_TPrintf("�e��A�{�^���A�@�q��X�{�^��\n");
		GFL_MSG_GetString( wk->mm, DM_MSG_MATCH001, wk->strbuf );
		wk->printStream = PRINTSYS_PrintStream( wk->drawwin[ENTRY_MAX].win, 0, 0,
						wk->strbuf, wk->fontHandle, MSGSPEED_GetWait(), wk->tcbl, 0, wk->heapID, 0xff );

		wk->seq++;
		break;
	case 2:
		if( PRINTSYS_PrintStreamGetState(wk->printStream) == PRINTSTREAM_STATE_DONE ){
			PRINTSYS_PrintStreamDelete( wk->printStream );
			wk->seq++;
		}
		break;

	case 3:
		switch(GFL_UI_KEY_GetTrg()){
		case PAD_BUTTON_A:
			wk->oya = MY_PARENT;
			wk->my_profile.oya_child = MY_PARENT;
			msg_id = DM_MSG_MATCH002;
			GFL_MSG_GetString( wk->mm, msg_id, wk->strbuf );
			Local_MessagePut(wk, ENTRY_MAX, wk->strbuf, 0, 0);
			wk->seq++;
			break;
		case PAD_BUTTON_X:
			wk->oya = MY_CHILD;
			wk->my_profile.oya_child = MY_CHILD;
			msg_id = DM_MSG_MATCH003;
			GFL_MSG_GetString( wk->mm, msg_id, wk->strbuf );
			Local_MessagePut(wk, ENTRY_MAX, wk->strbuf, 0, 0);
			wk->seq++;
			break;
		default:
			break;
		}
		break;
	case 4:
		if(wk->oya == MY_PARENT && ((GFL_UI_KEY_GetTrg() & PAD_BUTTON_START) || wk->connect_max == TRUE)){
			IRC_Shutdown();
			wk->seq++;
			break;
		}
		
		if((wk->oya == MY_CHILD && wk->irc.success == FALSE) 
				|| (wk->oya == MY_PARENT && wk->connect_max == FALSE)){
			irc_ret = DM_IrcMain(wk, &wk->irc);
            OS_TPrintf("DM_IrcMain%d \n",irc_ret);
			if(irc_ret == TRUE && wk->entry_num >= CHILD_MAX){
				wk->connect_max = TRUE;
			}
		}
		
		if(irc_ret == TRUE){
			if(wk->oya == MY_CHILD){
				OS_TPrintf("�q�F���C�����X�ʐM�J�n\n");
				GFL_STR_SetStringCodeOrderLength(wk->strbuf, wk->parent_profile.name, wk->parent_profile.name_len+1);
				Local_MessagePut(wk, 0, wk->strbuf, 0, 0);
                GFL_NET_Exit(_endCallBackSeq);	//�ʐM�I��
				wk->seq++;
			}
			else{
				OS_TPrintf("�G���g���[��������\n");
				GFL_STR_SetStringCodeOrderLength(wk->strbuf, wk->child_profile[wk->entry_num - 1].name, wk->child_profile[wk->entry_num - 1].name_len+1);
				Local_MessagePut(wk, wk->entry_num, wk->strbuf, 0, 0);
			}
			break;
		}
		break;
      case 5:
        //_endCallBackSeq�҂�
        break;
	default:
		return TRUE;	//���C�����X�ʐM������
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   ���C�����X�ʐM�F�e��MAC�A�h���X���w�肵�Čq����
 *
 * @param   wk		
 *
 * @retval  TRUE:�I���B�@FALSE:�����p����
 */
//--------------------------------------------------------------
static BOOL DebugMatsuda_Wireless(D_MATSU_WORK *wk)
{
	BOOL ret;
	
	GF_ASSERT(wk);

	switch( wk->seq ){
	case 0:
		if(wk->oya == MY_PARENT){
			GFL_MSG_GetString( wk->mm, DM_MSG_MATCH006, wk->strbuf );
			Local_MessagePut(wk, ENTRY_MAX, wk->strbuf, 0, 0);
		}
		else{
			GFL_MSG_GetString( wk->mm, DM_MSG_MATCH004, wk->strbuf );
			Local_MessagePut(wk, ENTRY_MAX, wk->strbuf, 0, 0);
		}
		
		{
			GFLNetInitializeStruct net_ini_data;
			
			net_ini_data = aGFLNetInit;
            
			GFL_NET_Init(&net_ini_data, _initCallBack, wk);	//�ʐM������
		}
		wk->seq++;
		break;
	case 1:
		if(GFL_NET_IsInit() == TRUE){	//�������I���҂�
			wk->seq++;
		}
		break;
	case 2:
		if(wk->oya == MY_PARENT){
			GFL_NET_InitServer();
			OS_TPrintf("�e�@�ɂȂ��ă��C�����X�J�n\n");
		}
		else{
			GFL_NET_InitClientAndConnectToParent(wk->parent_profile.parent_MacAddress);
			OS_TPrintf("�q�@�ɂȂ��ă��C�����X�J�n\n");
		}
		wk->seq++;
		break;
	case 3:	//���C�����X�ڑ��҂�
		//��check�@����ڑ����������f�����i���Ȃ��̂ŁA�b��I�Ƀ^�C�}�[�Ōq���������ɂ���
		wk->timer++;
		if(wk->timer > 30){
			wk->timer = 0;
			//�l�S�V�G�[�V�����J�n
			OS_TPrintf("�l�S�V�G�[�V�������M\n");
			if(wk->oya == MY_CHILD){
				if(GFL_NET_HANDLE_RequestNegotiation() == TRUE){
					wk->seq++;
				}
			}
			else{
				wk->seq++;	//�e�@�͍Ō�Ƀl�S�V�G�[�V�������M������
			}
		}
		break;
	case 4:	//�l�S�V�G�[�V���������҂�
		if(wk->oya == MY_PARENT){
			if(GFL_NET_HANDLE_GetNumNegotiation() >= wk->entry_num){
				wk->timer++;
				if(wk->timer > 30){
					wk->timer = 0;
					if(GFL_NET_HANDLE_RequestNegotiation() == TRUE){
						OS_TPrintf("�q�̃l�S�V�G�[�V����������\n");
					 //   wk->connect_ok = TRUE;
					    wk->seq++;
					}
				}
			}
		}
		else{
			wk->seq++;
		}
		break;

	case 5:
		if(wk->oya == MY_PARENT){
			if(GFL_NET_HANDLE_GetNumNegotiation() >= wk->entry_num + 1){
				OS_TPrintf("�e�̃l�S�V�G�[�V����������\n");
				wk->connect_ok = TRUE;
				wk->seq++;
			}
		}
		else{
			wk->seq++;
		}
		break;
	case 6:		//�^�C�~���O�R�}���h���s
		if(wk->oya == MY_PARENT){
			wk->timer++;
			if(wk->timer < 30){
				break;
			}
			wk->timer = 0;
		}
		GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,15);
		wk->seq++;
		break;
	case 7:
		if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),15) == TRUE){
			OS_TPrintf("�^�C�~���O��萬��\n");
			OS_TPrintf("�ڑ��l�� = %d\n", GFL_NET_GetConnectNum());
			GFL_MSG_GetString( wk->mm, DM_MSG_MATCH008, wk->strbuf );
			Local_MessagePut(wk, ENTRY_MAX, wk->strbuf, 0, 0);
			wk->seq++;
		}
		break;
	case 8:
		//netID���ɔz��ɖ��ߍ��ވׁA�ēx�������g�̃v���t�B�[����S���ɑ��M
		if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_MY_PROFILE, sizeof(IRC_MATCH_ENTRY_PARAM), &wk->my_profile) == TRUE){
			OS_TPrintf("�����̃v���t�B�[����S�Ẵv���C���[�ɑ��M����\n");
			OS_TPrintf("MAC: %d, %d, %d, %d, %d, %d\n", wk->my_profile.parent_MacAddress[0], wk->my_profile.parent_MacAddress[1], wk->my_profile.parent_MacAddress[2], wk->my_profile.parent_MacAddress[3], wk->my_profile.parent_MacAddress[4], wk->my_profile.parent_MacAddress[5]);
			wk->seq++;
		}
		else{
			OS_TPrintf("�����v���t�B�[�����M���s\n");
		}
		break;
	case 9:	//�v���t�B�[���𑗐M���������̂ōēx�������
		GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,16);
		wk->seq++;
		break;
	case 10:
		if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),16) == TRUE){
			OS_TPrintf("�^�C�~���O��萬�� 2���\n");
			wk->seq++;
		}
		break;
	
	case 11:	//�L�[�𑗐M������
		ret = -1;
		wk->send_key = GFL_UI_KEY_GetRepeat();
		if(wk->send_key & PAD_KEY_UP){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_KEY, 4, &wk->send_key);
		}
		if(wk->send_key & PAD_KEY_DOWN){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_KEY, 4, &wk->send_key);
		}
		if(wk->send_key & PAD_KEY_LEFT){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_KEY, 4, &wk->send_key);
		}
		if(wk->send_key & PAD_KEY_RIGHT){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_KEY, 4, &wk->send_key);
		}
		
		if(wk->send_key & PAD_BUTTON_B){
			ret = GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), NET_CMD_HUGE, 0x1000, &wk->huge_data);
		}
		
		if(ret == FALSE){
			OS_TPrintf("���M���s\n");
		}
		
		if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT){
//			wk->seq++;
		}
		break;
	case 12:	//�ʐM�I��
		if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_CMD_EXIT_REQ, 0, NULL)){
			//GFL_NET_Exit(_endCallBack);	//�ʐM�I��
			wk->seq++;
		}
		break;
	case 13:	//�ʐM�I���҂�
		if(wk->connect_ok == FALSE){
			wk->seq++;
		}
		break;
	default:
		return TRUE;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   IRC��M���ɌĂ΂��R�[���o�b�N
 *
 * @param   data		
 * @param   size		
 * @param   command		
 * @param   value		
 */
//--------------------------------------------------------------
static void DM_IRC_ReceiveCallback(u8 *data, u8 size, u8 command, u8 value)
{
	D_MATSU_WORK *wk = glb_d_matsu;
	int send_id;
	IRC_MATCH_ENTRY_PARAM *recv = (IRC_MATCH_ENTRY_PARAM *)data;
	
	//�ԊO����p�̃V�X�e���R�}���h����
	switch(command){
	case 0xf4:		//�ԊO�����C�u�����̐ؒf�R�}���h
		OS_TPrintf("IRC�ؒf�R�}���h����M\n");
		return;
	}
	
	//�q���瑗�M���Ă���f�[�^
	switch(command){
	case CMD_CHILD_PROFILE:		//�q�̃v���t�B�[����M
		if(wk->my_profile.oya_child == MY_CHILD){
			OS_TPrintf("������������q�ł���\n");
			IRC_Shutdown();
			return;
		}
		Local_EntryAdd(wk, recv);
		IRC_Send(NULL, 0, CMD_PARENT_SUCCESS, 0);
		wk->irc.success = TRUE;
		break;
	}
	
	//�e���瑗�M���Ă���f�[�^
	switch(command){
	case CMD_PARENT_PROFILE:	//�e�̃v���t�B�[����M
		Local_ParentProfileSet(wk, recv);
		IRC_Send((u8*)&wk->my_profile, sizeof(IRC_MATCH_ENTRY_PARAM), CMD_CHILD_PROFILE, 0);
		break;
	case CMD_PARENT_SUCCESS:		//�e����󂯎�����ƕԎ�����M
		IRC_Shutdown();
		wk->irc.success = TRUE;
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief   �q�̃v���t�B�[����o�^
 *
 * @param   wk		
 * @param   profile		
 */
//--------------------------------------------------------------
static void Local_EntryAdd(D_MATSU_WORK *wk, IRC_MATCH_ENTRY_PARAM *profile)
{
	GF_ASSERT(wk->entry_num < CHILD_MAX);
	GFL_STD_MemCopy(profile, &wk->child_profile[wk->entry_num], sizeof(IRC_MATCH_ENTRY_PARAM));
	wk->entry_num++;
	OS_TPrintf("�q���G���g���[:%d�l��\n", wk->entry_num);
}

//--------------------------------------------------------------
/**
 * @brief   �e�̃v���t�B�[����o�^
 *
 * @param   wk		
 * @param   profile		
 */
//--------------------------------------------------------------
static void Local_ParentProfileSet(D_MATSU_WORK *wk, IRC_MATCH_ENTRY_PARAM *profile)
{
	GFL_STD_MemCopy(profile, &wk->parent_profile, sizeof(IRC_MATCH_ENTRY_PARAM));
	OS_TPrintf("�e�̃v���t�B�[���o�^\n");
}

//--------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W��\������
 *
 * @param   wk			
 * @param   strbuf		�\�����郁�b�Z�[�W�f�[�^
 * @param   x			X���W
 * @param   y			Y���W
 */
//--------------------------------------------------------------
static void Local_MessagePut(D_MATSU_WORK *wk, int win_index, STRBUF *strbuf, int x, int y)
{
	GFL_BMP_Clear( wk->drawwin[win_index].bmp, 0xff );
	PRINTSYS_PrintQue(wk->printQue, wk->drawwin[win_index].bmp, x, y, strbuf, wk->fontHandle);
	wk->drawwin[win_index].message_req = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R���̋��ʃf�[�^�o�^
 *
 * @param   wk		
 */
//--------------------------------------------------------------
static void Local_PokeIconCommonDataSet(D_MATSU_WORK *wk)
{
	ARCHANDLE *hdl;

	hdl = GFL_ARC_OpenDataHandle(ARCID_POKEICON, HEAPID_MATSUDA_DEBUG);
	
	wk->icon_pltt_id = GFL_CLGRP_PLTT_RegisterEx(
		hdl, POKEICON_GetPalArcIndex(), CLSYS_DRAW_MAIN, 
		D_MATSU_ICON_PALNO * 0x20, 0, 1, HEAPID_MATSUDA_DEBUG);
	wk->icon_cell_id = GFL_CLGRP_CELLANIM_Register(hdl, 
		POKEICON_GetCellArcIndex(), POKEICON_GetAnmArcIndex(), HEAPID_MATSUDA_DEBUG);

	GFL_ARC_CloseDataHandle(hdl);
}

//--------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R���̋��ʃf�[�^�j��
 *
 * @param   wk		
 */
//--------------------------------------------------------------
static void Local_PokeIconCommonDataFree(D_MATSU_WORK *wk)
{
	int i;
	
	for(i = 0; i < ENTRY_MAX; i++){
		if(wk->clwk[i] != NULL){
			GFL_CLACT_WK_Remove(wk->clwk[i]);
		}
	}
	GFL_HEAP_FreeMemory(wk->icon_cell_res);
	GFL_HEAP_FreeMemory(wk->icon_anm_res);
}

//--------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R���o�^
 *
 * @param   wk		
 * @param   monsno		�|�P�����ԍ�
 * @param   net_id		�l�b�gID
 */
//--------------------------------------------------------------
static void Local_PokeIconAdd(D_MATSU_WORK *wk, int monsno, int net_id)
{
	GFL_CLWK_DATA clwk_data;
//	GFL_CLWK_RES clwk_res;
	NNSG2dImageProxy imgProxy;
	u32 icon_index;
	u32 vram_offset = 0;	//byte�P��
	ARCHANDLE *hdl;

	GF_ASSERT(wk->clwk[net_id] == NULL);

	hdl = GFL_ARC_OpenDataHandle(ARCID_POKEICON, HEAPID_MATSUDA_DEBUG);

	vram_offset = POKEICON_SIZE_CGX * net_id;
	
	OS_TPrintf("monsno = %d\n", monsno);
	
	//�L�����N�^�ݒ�
	icon_index = POKEICON_GetCgxArcIndexByMonsNumber(monsno, 0, FALSE);
	wk->icon_cgr_id = GFL_CLGRP_CGR_Register(
		hdl, icon_index, FALSE, CLSYS_DRAW_MAIN, HEAPID_MATSUDA_DEBUG);

	//�A�N�^�[�o�^
	clwk_data = PokeIconClwkData;
	clwk_data.pos_x = 120;
	clwk_data.pos_y = 24 + net_id*32;
	wk->clwk[net_id] = GFL_CLACT_WK_Create(wk->clunit, wk->icon_cgr_id, wk->icon_pltt_id, 
		wk->icon_cell_id, &clwk_data, CLWK_SETSF_NONE, HEAPID_MATSUDA_DEBUG);
	
	//�A�j���I�[�g�ݒ�
	GFL_CLACT_WK_SetAutoAnmSpeed(wk->clwk[net_id], FX32_ONE);
	GFL_CLACT_WK_SetAutoAnmFlag(wk->clwk[net_id], TRUE);
	
	//�p���b�gNo�ݒ�
	GFL_CLACT_WK_SetPlttOffs(wk->clwk[net_id], POKEICON_GetPalNum(monsno, 0, FALSE), CLWK_PLTTOFFS_MODE_OAM_COLOR);

	GFL_ARC_CloseDataHandle(hdl);
}


//==============================================================================
//	
//==============================================================================
typedef struct{
    int gameNo;   ///< �Q�[�����
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { WB_NET_DEBUG_MATSUDA_SERVICEID };

//--------------------------------------------------------------
/**
 * @brief   �r�[�R���f�[�^�擾�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------

static void* _netBeaconGetFunc(void* pWork)
{
	return &_testBeacon;
}

///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int _netBeaconGetSizeFunc(void* pWork)
{
	return sizeof(_testBeacon);
}

///< �r�[�R���f�[�^�擾�֐�
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
    if(myNo != beaconNo){
        return FALSE;
    }
    return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �����������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
static void _initCallBack(void* pWork)
{
	OS_TPrintf("�����������R�[���o�b�N���Ăяo���ꂽ\n");
	return;
}

//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �ڑ������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
static void _connectCallBack(void* pWork)
{
	D_MATSU_WORK *wk = pWork;
	
    OS_TPrintf("�l�S�V�G�[�V��������\n");
    wk->connect_ok = TRUE;
}

static void _endCallBack(void* pWork)
{
	D_MATSU_WORK *wk = pWork;

    NET_PRINT("endCallBack�I��\n");
    wk->connect_ok = FALSE;
}


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �ړ��R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------

static void _RecvMoveData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
}

//--------------------------------------------------------------
/**
 * @brief   ����f�[�^�R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------

static void _RecvHugeData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	D_MATSU_WORK *wk = pWork;
	int i;
	const u8 *data = pData;
	u16 cs = 0;
	
	OS_TPrintf("����f�[�^��M netid = %d, size = 0x%x\n", netID, size);
	for(i = 0; i < size; i++){
		cs += data[i];
		//OS_TPrintf("%d, ", data[i]);
	//	if(i % 32 == 0){
	//		OS_TPrintf("\n");
	//	}
	}
	OS_TPrintf("����f�[�^��M�`�F�b�N�T��=0x%x\n", cs);
}

static u8 * _RecvHugeBuffer(int netID, void* pWork, int size)
{
	D_MATSU_WORK *wk = pWork;
	return wk->receive_huge_data[netID - 1];
}


//--------------------------------------------------------------
/**
 * @brief   �L�[���R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _RecvKeyData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	D_MATSU_WORK *wk = pWork;
	int key_data;
	int dx = 0, dy = 0;
	
	if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	
	key_data = *((int *)pData);
	OS_TPrintf("�f�[�^��M�FnetID=%d, size=%d, data=%d\n", netID, size, key_data);
	if(key_data & PAD_KEY_UP){
		OS_TPrintf("��M�F��\n");
		dy -= 4;
	}
	if(key_data & PAD_KEY_DOWN){
		OS_TPrintf("��M�F��\n");
		dy += 4;
	}
	if(key_data & PAD_KEY_LEFT){
		OS_TPrintf("��M�F��\n");
		dx -= 4;
	}
	if(key_data & PAD_KEY_RIGHT){
		OS_TPrintf("��M�F�E\n");
		dx += 4;
	}

#if 0	//���O�ł͂Ȃ��|�P�����A�C�R���𓮂����悤�ɕύX
	wk->msg_pos_x[netID] += dx;
	wk->msg_pos_y[netID] += dy;
	if(netID == 0){
		if(wk->oya == MY_PARENT){
			GFL_STR_SetStringCodeOrderLength(wk->strbuf_entry[netID], wk->my_profile.name, wk->my_profile.name_len+1);
		}
		else{
			GFL_STR_SetStringCodeOrderLength(wk->strbuf_entry[netID], wk->parent_profile.name, wk->parent_profile.name_len+1);
		}
	}
	else{
		GFL_STR_SetStringCodeOrderLength(wk->strbuf_entry[netID], wk->child_profile[netID-1].name, wk->child_profile[netID-1].name_len+1);
	}
	Local_MessagePut(wk, netID, 
		wk->strbuf_entry[netID], wk->msg_pos_x[netID], wk->msg_pos_y[netID])
#else
	if(wk->clwk[netID] != NULL){
		GFL_CLACTPOS pos;
		GFL_CLACT_WK_GetPos(wk->clwk[netID], &pos, CLWK_SETSF_NONE);
		pos.x += dx;
		pos.y += dy;
		GFL_CLACT_WK_SetPos(wk->clwk[netID], &pos, CLWK_SETSF_NONE);
	}
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �L�[���R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _RecvMyProfile(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	D_MATSU_WORK *wk = pWork;
	const IRC_MATCH_ENTRY_PARAM *recv = pData;
	int i;
	
	OS_TPrintf("RecvChildProfile, netID=%d, size=%d\n", netID, size);
	
	if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	
	if(netID == 0){
		GFL_STD_MemCopy(recv, &wk->parent_profile, size);
	}
	else{
		GFL_STD_MemCopy(recv, &wk->child_profile[netID - 1], size);
	}
	GFL_STR_SetStringCodeOrderLength(
		wk->strbuf_entry[netID], recv->name, recv->name_len+1);
	Local_MessagePut(wk, netID, wk->strbuf_entry[netID], 0, 0);

	Local_PokeIconAdd(wk, recv->parent_MacAddress[5], netID);
}




//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA DebugMatsudaIrcMatchProcData = {
	DebugMatsudaMainProcInit,
	DebugMatsudaMainProcMain,
	DebugMatsudaMainProcEnd,
};


