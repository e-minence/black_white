//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_connect.c
 *	@brief		Wi-Fi���r�[�ɐڑ�	�ؒf�����v���b�N
 *	@author		tomoya takahashi
 *	@data		2007.12.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "print\printsys.h"

#include "print/wordset.h"
#include "print\gf_font.h"
#include "arc_def.h"
#include "system/wipe.h"
#include "system/bmp_menu.h"
//#include "system/fontproc.h"
//#include "system/pm_rtc.h"

//#include "communication/comm_state.h"

#include "savedata/config.h"
#include "savedata/record.h"


#include "message.naix"
#include "msg/msg_wifi_lobby.h"
#include "msg/msg_wifi_hiroba.h"
#include "msg/msg_wifi_system.h"
#include "msg/msg_debug_hiroba.h"

//#include  "communication/wm_icon.h"

#include "wflby_connect.h"
#include "wflby_def.h"
#include "wflby_snd.h"

#include "net_app/connect_anm.h"

#include "net/network_define.h"
#include "system/main.h"
#include "font/font.naix"
#include "net_app/net_bugfix.h"
#include "arc_def.h"
#include "test/wflby_debug.h"
#include "savedata/wifilist.h"
#include "gamesystem/msgspeed.h"
#include "net/net_exchange.h"

// �_�~�[�O���t�B�b�N�ł�
#include "wifip2pmatch.naix"
#include "system/gfl_use.h"
#include "system/bmp_winframe.h"

//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
*/
//-----------------------------------------------------------------------------
#ifdef PM_DEBUG
#define WFLBY_CONNECT_DEBUG_START		// �����̎�ނȂǂ�I�����Ă��炤
#endif

#ifdef WFLBY_CONNECT_DEBUG_START
static s32 DEBUG_SEL_ROOM;		// �I�����������Ȃ�Ȃ�Ȃ�
static s32 DEBUG_SEL_SEASON;
static s32 DEBUG_SEL_ITEM;
#endif



//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���C���V�[�P���X
//=====================================
enum {
	WFLBY_CONNECT_SEQ_FADEIN,
	WFLBY_CONNECT_SEQ_FADEWAIT,
	WFLBY_CONNECT_SEQ_LOGIN_CHECK,
	WFLBY_CONNECT_SEQ_LOGIN_CHECK_WAIT,
	
#ifdef WFLBY_CONNECT_DEBUG_START
	WFLBY_CONNECT_SEQ_DEBUG_SEL_ROOM,
	WFLBY_CONNECT_SEQ_DEBUG_SEL_ROOMWAIT,
	WFLBY_CONNECT_SEQ_DEBUG_SEL_SEASON,
	WFLBY_CONNECT_SEQ_DEBUG_SEL_SEASONWAIT,
	WFLBY_CONNECT_SEQ_DEBUG_SEL_ITEM,
	WFLBY_CONNECT_SEQ_DEBUG_SEL_ITEMWAIT,
#endif

	WFLBY_CONNECT_SEQ_LOGIN,
	WFLBY_CONNECT_SEQ_LOGIN_WAIT_DWC,
	WFLBY_CONNECT_SEQ_LOGIN_WAIT,
	WFLBY_CONNECT_SEQ_FADEOUT,
	WFLBY_CONNECT_SEQ_FADEOUTWAIT,
	WFLBY_CONNECT_SEQ_ERRON,
	WFLBY_CONNECT_SEQ_ERR,
	WFLBY_CONNECT_SEQ_RETRY,
	WFLBY_CONNECT_SEQ_RETRYWAIT,
	WFLBY_CONNECT_SEQ_RETRYLOGOUTWAIT,
	WFLBY_CONNECT_SEQ_LOGOUT,
	WFLBY_CONNECT_SEQ_LOGOUTWAIT,
} ;



//-------------------------------------
///	�ގ��V�[�P���X
//=====================================
enum {
	WFLBY_DISCONNECT_SEQ_FADEIN,
	WFLBY_DISCONNECT_SEQ_FADEWAIT,
#if 0	// �^�C���A�E�g���b�Z�[�W�͍L����ŏo�����ƂɂȂ���
	WFLBY_DISCONNECT_SEQ_TIMEOUT_MSG,
	WFLBY_DISCONNECT_SEQ_TIMEOUT_MSG_WAIT,
#endif
	WFLBY_DISCONNECT_SEQ_LOGOUT_MSG,
	WFLBY_DISCONNECT_SEQ_WLDSEND,
	WFLBY_DISCONNECT_SEQ_WLDWAIT,
	WFLBY_DISCONNECT_SEQ_LOGOUT,
	WFLBY_DISCONNECT_SEQ_LOGOUT_WAIT,
	WFLBY_DISCONNECT_SEQ_LOGOUT_END,
	WFLBY_DISCONNECT_SEQ_LOGOUT_END_MSGWAIT,
	WFLBY_DISCONNECT_SEQ_FADEOUT,
	WFLBY_DISCONNECT_SEQ_FADEOUTWAIT,
} ;
#define WFLBY_DISCONNECT_WLDSENDWAIT_TIMEOUT	( 30*30 )	// ���E�f�[�^���M�����`�F�b�N�^�C���A�E�g
#define WFLBY_DISCONNECT_LOGOUT_ENDMSG_WAIT		( 3*30 )	// ���b�Z�[�W�\���Ԋu





//-------------------------------------
///	�\���V�X�e��
//=====================================
// BG
enum{
	// ���C�����
	WFLBY_BGCNT_MAIN_BACK,
	WFLBY_BGCNT_MAIN_WIN,
	
	// �T�u���
	WFLBY_BGCNT_SUB_BACK,

	WFLBY_BGCNT_NUM,	// �a�f�R���g���[���e�[�u����
};
// ���C��PLTT
enum{
	WFLBY_MAIN_PLTT_BACKSTART,
	WFLBY_MAIN_PLTT_BACKEND = 0xb,
	WFLBY_MAIN_PLTT_TALKFONT = WFLBY_MAIN_PLTT_BACKEND,
	WFLBY_MAIN_PLTT_SYSFONT,
	WFLBY_MAIN_PLTT_TALKWIN,
	WFLBY_MAIN_PLTT_SYSWIN,
	WFLBY_MAIN_PLTT_NUM,
};
// �E�B���h�E�O���t�B�b�N
#define WFLBY_TALKWINGRA_CGX		( 1 )
#define WFLBY_SYSWINGRA_CGX			( WFLBY_TALKWINGRA_CGX+TALK_WIN_CGX_SIZ )
#define WFLBY_SYSWINGRA_CGXEND		( WFLBY_SYSWINGRA_CGX+MENU_WIN_CGX_SIZ )



//-------------------------------------
///	�E�B���h�E�V�X�e��
//=====================================
#define WFLBY_WINSYS_STRBUFNUM	( 256 )




//-------------------------------------
///	��b�E�B���h�E
//=====================================
#define WFLBY_TALKWIN_X			( 2 )
#define WFLBY_TALKWIN_Y			( 19 )
#define WFLBY_TALKWIN_SIZX		( 27 )
#define WFLBY_TALKWIN_SIZY		( 4 )
#define WFLBY_TALKWIN_CGX		( WFLBY_SYSWINGRA_CGXEND )
#define WFLBY_TALKWIN_CGXEND	( WFLBY_TALKWIN_CGX+(WFLBY_TALKWIN_SIZX*WFLBY_TALKWIN_SIZY) )


//-------------------------------------
///	�V�X�e���E�C���h�E
//=====================================
#define WFLBY_SYSTEMWIN_X			( 4 )
#define WFLBY_SYSTEMWIN_Y			( 4 )
#define WFLBY_SYSTEMWIN_SIZX		( 23 )
#define WFLBY_SYSTEMWIN_SIZY		( 16 )
#define WFLBY_SYSTEMWIN_CGX		( WFLBY_TALKWIN_CGXEND )
#define WFLBY_SYSTEMWIN_CGXEND	( WFLBY_SYSTEMWIN_CGX+(WFLBY_SYSTEMWIN_SIZX*WFLBY_SYSTEMWIN_SIZY) )

//-------------------------------------
///	YESNO�E�B���h�E
//=====================================
#define WFLBY_YESNOWIN_X			( 25 )
#define WFLBY_YESNOWIN_Y			( 13 )
#define WFLBY_YESNOWIN_SIZX		( 6 )
#define WFLBY_YESNOWIN_SIZY		( 4 )
#define WFLBY_YESNOWIN_CGX		( WFLBY_SYSTEMWIN_CGXEND )
#define WFLBY_YESNOWIN_CGXEND	( WFLBY_YESNOWIN_CGX+(WFLBY_YESNOWIN_SIZX*WFLBY_YESNOWIN_SIZY) )

//-------------------------------------
///	�^�C�g��
//=====================================
#define WFLBY_TITLEWIN_X		( 5 )
#define WFLBY_TITLEWIN_Y		( 1 )
#define WFLBY_TITLEWIN_SIZX		( 22 )
#define WFLBY_TITLEWIN_SIZY		( 2 )
#define WFLBY_TITLEWIN_CGX		( WFLBY_YESNOWIN_CGXEND )
#define WFLBY_TITLEWIN_CGXEND	( WFLBY_TITLEWIN_CGX+(WFLBY_TITLEWIN_SIZX*WFLBY_TITLEWIN_SIZY) )
#define WFLBY_TITLEWIN_COL		( GF_PRINTCOLOR_MAKE( 15, 14, 0 ) )

enum{
	FONTID_SYSTEM,
	FONTID_TALK,
};




//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�E�B���h�E�V�X�e��
//=====================================
typedef struct {
	WORDSET*			p_wordset;	// ���[�h�Z�b�g
	GFL_MSGDATA*	p_msgman;	// ���[�h�Z�b�g
	GFL_BMPWIN*		win;		// ��b�E�B���h�E
	STRBUF*				p_str;		// ������o�b�t�@
	STRBUF*				p_tmp;		// ������o�b�t�@
	GFL_FONT*			font_handle;	// �t�H���g�n���h��
	u32					fontid;		// ���b�Z�[�W�̃t�H���gID	
	void*				p_timewait;	// �^�C���E�G�C�g
	int					msgspeed;
#if WB_FIX
	u32					msgno;
#else
	PRINT_STREAM*		msg_stream;
	GFL_TCBLSYS *tcbl;
	PRINT_QUE *printQue;
	PRINT_UTIL print_util;
#endif

#ifdef WFLBY_CONNECT_DEBUG_START
	GFL_MSGDATA*	p_debug_msgman;	// ���[�h�Z�b�g
#endif
} WFLBY_WINWK;


//-------------------------------------
///	���O���p���[�N
//=====================================
typedef struct {
	SAVE_CONTROL_WORK*			p_save;		// �Z�[�u�f�[�^
	u32					seq;		// �V�[�P���X
	u32					wait;		// �ėp�E�G�C�g
	//GF_BGL_INI*			p_bgl;		// bgl�R���g���[��
	WFLBY_WINWK			title;		// �^�C�g���E�B���h�E
	WFLBY_WINWK			talk;		// ��b�E�B���h�E
	WFLBY_WINWK			talk_system;// ��b�E�B���h�E
	WFLBY_WINWK			system;		// �V�X�e���E�B���h�E
	BMPMENU_WORK*		p_yesno;	// yesno�E�B���h�E
	
	CONNECT_BG_PALANM	cbp;		// Wifi�ڑ�BG�p���b�g�A�j������

	GFL_TCB *vintr_tcb;
	
	GFL_FONT *fontHandle_talk;
	GFL_FONT *fontHandle_system;
	
	GFL_TCBLSYS *tcbl;
	PRINT_QUE *printQue;
	PRINT_UTIL print_util;
	
	WIFI_EXCHANGE_WORK my_exchange_data;    ///<�����̌����f�[�^
} WFLBY_CONNECTWK;


//-----------------------------------------------------------------------------
/**
 *					�f�[�^����
 */
//-----------------------------------------------------------------------------

//-------------------------------------
///	�O���t�B�b�N�֘A
//=====================================
// �o���N�ݒ�
static const GFL_DISP_VRAM sc_WFLBY_BANK = {
	GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
	GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
	GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
	GX_VRAM_SUB_OBJ_NONE,			// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
	GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_NONE,			// �e�N�X�`���p���b�g�X���b�g
	GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
};

// BG�ݒ�
static const GFL_BG_SYS_HEADER sc_BGINIT = {
	GX_DISPMODE_GRAPHICS,
	GX_BGMODE_0,
	GX_BGMODE_0,
	GX_BG0_AS_2D
};

// BG�R���g���[��
static const u32 sc_WFLBY_BGCNT_FRM[ WFLBY_BGCNT_NUM ] = {
	GFL_BG_FRAME0_M,
	GFL_BG_FRAME1_M,
	GFL_BG_FRAME0_S,
};
static const GFL_BG_BGCNT_HEADER sc_WFLBY_BGCNT_DATA[ WFLBY_BGCNT_NUM ] = {
	// ���C�����
	{	// sc_WFLBY_BGCNT_FRM[WFLBY_BGCNT_MAIN_BACK]
		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, 0x8000, GX_BG_EXTPLTT_01,
		1, 0, 0, FALSE
	},
	{	// sc_WFLBY_BGCNT_FRM[WFLBY_BGCNT_MAIN_WIN]
		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x10000, 0x8000, GX_BG_EXTPLTT_01,
		0, 0, 0, FALSE
	},

	// �T�u���
	{	// sc_WFLBY_BGCNT_FRM[WFLBY_BGCNT_SUB_BACK]
		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, 0x8000, GX_BG_EXTPLTT_01,
		0, 0, 0, FALSE
	},
};
static const BMPWIN_YESNO_DAT sc_WFLBY_BMPWIN_DAT_YESNO = {
	GFL_BG_FRAME1_M, WFLBY_YESNOWIN_X, WFLBY_YESNOWIN_Y,
//	WFLBY_YESNOWIN_SIZX, WFLBY_YESNOWIN_SIZY, 
	WFLBY_MAIN_PLTT_SYSFONT, WFLBY_YESNOWIN_CGX,
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_VBlank(GFL_TCB *tcb, void *work);


static void WFLBY_CONNECT_GraphicInit( WFLBY_CONNECTWK* p_wk, u32 heapID );
static void WFLBY_CONNECT_GraphicExit( WFLBY_CONNECTWK* p_wk );
static void WFLBY_CONNECT_GraphicVBlank( WFLBY_CONNECTWK* p_wk );

static void WFLBY_CONNECT_WIN_Init( WFLBY_WINWK* p_wk, GFL_FONT *font_handle, GFL_TCBLSYS *tcbl, PRINT_QUE *printQue, u32 fontid, u32 msgid, u32 x, u32 y, u32 sizx, u32 sizy, u32 cgx, SAVE_CONTROL_WORK* p_save, u32 heapID );
static void WFLBY_CONNECT_WIN_Print( WFLBY_WINWK* p_wk, u32 strid );
static void WFLBY_CONNECT_WIN_Off( WFLBY_WINWK* p_wk );
static void WFLBY_CONNECT_WIN_StartTimeWait( WFLBY_WINWK* p_wk );
static void WFLBY_CONNECT_WIN_EndTimeWait( WFLBY_WINWK* p_wk );
static void WFLBY_CONNECT_WIN_SetErrNumber( WFLBY_WINWK* p_wk, u32 number );
static void WFLBY_CONNECT_WIN_Exit( WFLBY_WINWK* p_wk );
static void WFLBY_CONNECT_WIN_PrintWait( WFLBY_WINWK* p_wk, u32 strid );

static void WFLBY_CONNECT_WIN_PrintTitle( WFLBY_WINWK* p_wk, u32 strid );


#ifdef WFLBY_CONNECT_DEBUG_START
static void WFLBY_CONNECT_WIN_PrintDEBUG( WFLBY_WINWK* p_wk, u32 strid, u32 num );
static void WFLBY_CONNECT_WIN_PrintDEBUG2( WFLBY_WINWK* p_wk, u32 strid, u32 item );
#endif

static DWCUserData* _getMyUserData(void* pWork);
static DWCFriendData* _getFriendData(void* pWork);
static void _deleteFriendList(int deletedIndex,int srcIndex, void* pWork);
static void * _WFLBY_GetExchangeDataPtr(void *pWork);
static int _WFLBY_GetExchangeDataSize(void *pWork);


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
//	
//--------------------------------------------------------------
#define _MAXNUM   (4)         // �ő�ڑ��l��
#define _MAXSIZE  (80)        // �ő呗�M�o�C�g��
#define _BCON_GET_NUM (16)    // �ő�r�[�R�����W��

static const GFLNetInitializeStruct aGFLNetInit = {
    NULL,  // ��M�֐��e�[�u��
    0, // ��M�e�[�u���v�f��
    NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
    NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL,//_WFLBY_GetExchangeDataPtr,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL,//_WFLBY_GetExchangeDataSize,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    NULL,  // �r�[�R���f�[�^�擾�֐�
    NULL,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    NULL,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
    NULL,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    _deleteFriendList,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    _getFriendData,   ///< DWC�`���̗F�B���X�g	
    _getMyUserData,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    GFL_NET_DWCLOBBY_HEAPSIZE,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
    0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_NETWORK,  //IRC�p��create�����HEAPID
    GFL_WICON_POSX,GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    TRUE,     // CRC�v�Z
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_WIFI_LOBBY,  //�ʐM���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_WIFILOBBY,  //GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
};



//----------------------------------------------------------------------------
/**
 *	@brief	���O�C��	�J�n
 *
 *	@param	p_proc		�v���b�N���[�N
 *	@param	p_seq		�V�[�P���X
 *
 *	@retval	GFL_PROC_RES_CONTINUE = 0,		///<����p����
 *	@retval	GFL_PROC_RES_FINISH,			///<����I��
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WFLBY_CONNECT_Init(GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk)
{
	WFLBY_CONNECTWK* p_wk;
	WFLBY_CONNECT_PARAM* p_param;

	// �q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WFLBY_ROOM, 0x20000 );
	
	// ���[�N�쐬
	p_wk = GFL_PROC_AllocWork( p_proc, sizeof(WFLBY_CONNECTWK), HEAPID_WFLBY_ROOM );
	GFL_STD_MemFill( p_wk, 0, sizeof(WFLBY_CONNECTWK) );

	// �p�����[�^�擾
	p_param	= pwk;
	p_wk->p_save		= p_param->p_save;

  // �����̌����f�[�^�쐬
  NET_EXCHANGE_SetParam(p_param->p_save, &p_wk->my_exchange_data);

	//TCBL�쐬
	p_wk->tcbl = GFL_TCBL_Init( HEAPID_WFLBY_ROOM, HEAPID_WFLBY_ROOM, 4, 32);
	//PrintQue�쐬
	p_wk->printQue = PRINTSYS_QUE_Create(HEAPID_WFLBY_ROOM);
	
	// BGM�`�F���W
#if WB_TEMP_FIX
	Snd_DataSetByScene( SND_SCENE_P2P, SEQ_WIFILOBBY, 0 );	//wifi���r�[�Đ�
#endif

	// ���̃v���b�N���Őݒ肷��p�����[�^��������
	p_param->enter			= FALSE;


	// �O���t�B�b�N������
	WFLBY_CONNECT_GraphicInit( p_wk, HEAPID_WFLBY_ROOM );

	// �t�H���g�쐬	��check PL�̂悤�ɉ�b�t�H���g�A�V�X�e���t�H���g�Ƃ��������̂��܂������̂�
	//						�ǂ���������t�H���g��ǂݍ���ł��� 2009.03.10(��) matsuda
	p_wk->fontHandle_talk = GFL_FONT_Create(ARCID_FONT, NARC_font_large_nftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WFLBY_ROOM);
	p_wk->fontHandle_system = GFL_FONT_Create(ARCID_FONT, NARC_font_large_nftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WFLBY_ROOM);

	// �E�B���h�E�V�X�e��������
	WFLBY_CONNECT_WIN_Init( &p_wk->talk, p_wk->fontHandle_talk, 
			p_wk->tcbl, p_wk->printQue,
			FONTID_TALK, NARC_message_wifi_lobby_dat,
			WFLBY_TALKWIN_X, WFLBY_TALKWIN_Y, 
			WFLBY_TALKWIN_SIZX, WFLBY_TALKWIN_SIZY,
			WFLBY_TALKWIN_CGX, p_wk->p_save, HEAPID_WFLBY_ROOM );
	WFLBY_CONNECT_WIN_Init( &p_wk->talk_system, p_wk->fontHandle_talk, 
			p_wk->tcbl, p_wk->printQue,
			FONTID_TALK, NARC_message_wifi_system_dat,
			WFLBY_TALKWIN_X, WFLBY_TALKWIN_Y, 
			WFLBY_TALKWIN_SIZX, WFLBY_TALKWIN_SIZY,
			WFLBY_TALKWIN_CGX, p_wk->p_save, HEAPID_WFLBY_ROOM );
	WFLBY_CONNECT_WIN_Init( &p_wk->system, p_wk->fontHandle_system, 
			p_wk->tcbl, p_wk->printQue,
			FONTID_SYSTEM, NARC_message_wifi_system_dat,
			WFLBY_SYSTEMWIN_X, WFLBY_SYSTEMWIN_Y,
			WFLBY_SYSTEMWIN_SIZX, WFLBY_SYSTEMWIN_SIZY,
			WFLBY_SYSTEMWIN_CGX, p_wk->p_save, HEAPID_WFLBY_ROOM );

	WFLBY_CONNECT_WIN_Init( &p_wk->title, p_wk->fontHandle_talk, 
			p_wk->tcbl, p_wk->printQue,
			FONTID_TALK, NARC_message_wifi_lobby_dat,
			WFLBY_TITLEWIN_X, WFLBY_TITLEWIN_Y, 
			WFLBY_TITLEWIN_SIZX, WFLBY_TITLEWIN_SIZY,
			WFLBY_TITLEWIN_CGX, p_wk->p_save, HEAPID_WFLBY_ROOM );


	//  �^�C�g����������
	WFLBY_CONNECT_WIN_PrintTitle( &p_wk->title, msg_wifilobby_018 );


	// VBlank�֐��ݒ�
	p_wk->vintr_tcb = GFUser_VIntr_CreateTCB(WFLBY_CONNECT_VBlank, p_wk, 200);
	//sys_HBlankIntrStop();	//HBlank���荞�ݒ�~

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���O�C�����C��
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WFLBY_CONNECT_Main(GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk)
{
	WFLBY_CONNECTWK* p_wk;
	WFLBY_CONNECT_PARAM* p_param;
	
	p_wk	= mywk;
	p_param	= pwk;

	PRINTSYS_QUE_Main(p_wk->printQue);
	PRINT_UTIL_Trans(&p_wk->title.print_util, p_wk->printQue);
	PRINT_UTIL_Trans(&p_wk->talk.print_util, p_wk->printQue);
	PRINT_UTIL_Trans(&p_wk->talk_system.print_util, p_wk->printQue);
	PRINT_UTIL_Trans(&p_wk->system.print_util, p_wk->printQue);
	
	switch( *p_seq ){
	// �t�F�[�h�C��
	case WFLBY_CONNECT_SEQ_FADEIN:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
				WIPE_FADE_OUTCOLOR, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WFLBY_ROOM );
		(*p_seq)++;
		break;

	// �t�F�[�h�C���҂�
	case WFLBY_CONNECT_SEQ_FADEWAIT:
		if( WIPE_SYS_EndCheck() ){
			(*p_seq)++;
		}
		break;

	// ���O�C���`�F�b�N
	case WFLBY_CONNECT_SEQ_LOGIN_CHECK:
		if( p_param->check_skip == FALSE ){
			WFLBY_CONNECT_WIN_Print( &p_wk->talk_system, dwc_message_0002 );
			p_wk->p_yesno = BmpMenu_YesNoSelectInit( &sc_WFLBY_BMPWIN_DAT_YESNO, 
					WFLBY_SYSWINGRA_CGX, WFLBY_MAIN_PLTT_SYSWIN, 0, HEAPID_WFLBY_ROOM );
			(*p_seq)++;
		}else{
#ifdef WFLBY_CONNECT_DEBUG_START
			// ���ԒZ�k���[�h�ł���������I�΂Ȃ�
			if( D_Tomoya_WiFiLobby_DebugStart == TRUE ){
				(*p_seq) = WFLBY_CONNECT_SEQ_DEBUG_SEL_ROOM;
			}else{
				(*p_seq) = WFLBY_CONNECT_SEQ_LOGIN;
			}
#else
			(*p_seq) = WFLBY_CONNECT_SEQ_LOGIN;
#endif
		}
		break;

	// ���O�C���`�F�b�N�҂�
	case WFLBY_CONNECT_SEQ_LOGIN_CHECK_WAIT:
		{
			u32 result;
			result = BmpMenu_YesNoSelectMain( p_wk->p_yesno);
			switch( result ){
			// YES
			case 0:	

#ifdef WFLBY_CONNECT_DEBUG_START
				// ���ԒZ�k���[�h�ł���������I�΂Ȃ�
				if( D_Tomoya_WiFiLobby_DebugStart == TRUE ){
					(*p_seq) = WFLBY_CONNECT_SEQ_DEBUG_SEL_ROOM;
				}else{
					(*p_seq) = WFLBY_CONNECT_SEQ_LOGIN;
				}
#else
				(*p_seq) = WFLBY_CONNECT_SEQ_LOGIN;
#endif
				break;

			// NO
			case BMPMENU_CANCEL:
				WFLBY_CONNECT_WIN_Off( &p_wk->talk );
				(*p_seq) = WFLBY_CONNECT_SEQ_FADEOUT;
				break;
			}
		}
		break;

#ifdef WFLBY_CONNECT_DEBUG_START
	case WFLBY_CONNECT_SEQ_DEBUG_SEL_ROOM:

		WFLBY_CONNECT_WIN_PrintDEBUG( &p_wk->talk, MSG_WFLBY_00, DEBUG_SEL_ROOM );
		(*p_seq)++;

	case WFLBY_CONNECT_SEQ_DEBUG_SEL_ROOMWAIT:
		if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){
			DEBUG_SEL_ROOM = (DEBUG_SEL_ROOM + 1) % WFLBY_ROOM_NUM;
			WFLBY_CONNECT_WIN_PrintDEBUG( &p_wk->talk, MSG_WFLBY_00, DEBUG_SEL_ROOM );
		}
		if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){
			
			DEBUG_SEL_ROOM --;
			if( DEBUG_SEL_ROOM < 0 ){
				DEBUG_SEL_ROOM += WFLBY_ROOM_NUM;
			}
			WFLBY_CONNECT_WIN_PrintDEBUG( &p_wk->talk, MSG_WFLBY_00, DEBUG_SEL_ROOM );
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
			(*p_seq)++;
		}
		break;

	case WFLBY_CONNECT_SEQ_DEBUG_SEL_SEASON:
		WFLBY_CONNECT_WIN_PrintDEBUG( &p_wk->talk, MSG_WFLBY_01, DEBUG_SEL_SEASON );
		(*p_seq)++;

	case WFLBY_CONNECT_SEQ_DEBUG_SEL_SEASONWAIT:
		if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){
			DEBUG_SEL_SEASON = (DEBUG_SEL_SEASON + 1) % WFLBY_SEASON_NUM;
			WFLBY_CONNECT_WIN_PrintDEBUG( &p_wk->talk, MSG_WFLBY_01, DEBUG_SEL_SEASON );
		}
		if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){
			
			DEBUG_SEL_SEASON --;
			if( DEBUG_SEL_SEASON < 0 ){
				DEBUG_SEL_SEASON += WFLBY_SEASON_NUM;
			}
			WFLBY_CONNECT_WIN_PrintDEBUG( &p_wk->talk, MSG_WFLBY_01, DEBUG_SEL_SEASON );
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
			(*p_seq)++;
		}
		break;

	case WFLBY_CONNECT_SEQ_DEBUG_SEL_ITEM:
		WFLBY_CONNECT_WIN_PrintDEBUG2( &p_wk->talk, MSG_WFLBY_02, DEBUG_SEL_ITEM );
		(*p_seq)++;

	case WFLBY_CONNECT_SEQ_DEBUG_SEL_ITEMWAIT:
		if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
			DEBUG_SEL_ITEM = (DEBUG_SEL_ITEM + 1) % WFLBY_ITEM_NUM;
			WFLBY_CONNECT_WIN_PrintDEBUG2( &p_wk->talk, MSG_WFLBY_02, DEBUG_SEL_ITEM );
		}
		if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
			
			DEBUG_SEL_ITEM --;
			if( DEBUG_SEL_ITEM < 0 ){
				DEBUG_SEL_ITEM += WFLBY_ITEM_NUM;
			}
			WFLBY_CONNECT_WIN_PrintDEBUG2( &p_wk->talk, MSG_WFLBY_02, DEBUG_SEL_ITEM );
		}
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
			(*p_seq)++;
		}
		break;
#endif

	// ���O�C���J�n
	case WFLBY_CONNECT_SEQ_LOGIN:
		{
			WFLBY_USER_PROFILE* p_profile;
			
			GFL_NET_Init(&aGFLNetInit, NULL, p_wk);
#ifdef WFLBY_CONNECT_DEBUG_START
			if( D_Tomoya_WiFiLobby_DebugStart == TRUE ){
				WFLBY_SYSTEM_DEBUG_SetItem( p_param->p_system, DEBUG_SEL_ITEM );
				p_profile = WFLBY_SYSTEM_GetMyProfileLocal( p_param->p_system );
				GFL_NET_StateWifiLobbyLogin_Debug( p_profile, DEBUG_SEL_SEASON, DEBUG_SEL_ROOM );
			}else{
				p_profile = WFLBY_SYSTEM_GetMyProfileLocal( p_param->p_system );
				GFL_NET_StateWifiLobbyLogin( p_profile );
			}
#else
			p_profile = WFLBY_SYSTEM_GetMyProfileLocal( p_param->p_system );
			GFL_NET_StateWifiLobbyLogin( p_profile );
#endif
		}

		WFLBY_CONNECT_WIN_Print( &p_wk->talk_system, dwc_message_0008 );
		WFLBY_CONNECT_WIN_StartTimeWait( &p_wk->talk_system );
		(*p_seq)++;
		break;

	// DWC ���O�C���҂�
	case WFLBY_CONNECT_SEQ_LOGIN_WAIT_DWC:
		// �G���[����
		if( GFL_NET_SystemIsError() || GFL_NET_SystemIsLobbyError() ){
			OS_TPrintf("�G���[���� aaa %d, %d\n", GFL_NET_SystemIsError(), GFL_NET_SystemIsLobbyError());
			WFLBY_CONNECT_WIN_EndTimeWait( &p_wk->talk_system );
			(*p_seq) = WFLBY_CONNECT_SEQ_ERRON;
		}

		if( GFL_NET_StateWifiLobbyDwcLoginCheck() == TRUE ){	// DWC_LoginAsync�̐ڑ�����
			// WiFi�N���u�ł̏�Ԃ�NONE�ɂ��鏈��
			WFLBY_SYSTEM_WiFiClubBuff_Init( p_param->p_system );
			(*p_seq)++;
		}
		break;

	// ���O�C���҂�
	case WFLBY_CONNECT_SEQ_LOGIN_WAIT:
		
		// �G���[����
		if( GFL_NET_SystemIsError() || GFL_NET_SystemIsLobbyError() ){
			OS_TPrintf("�G���[���� bbb %d, %d\n", GFL_NET_SystemIsError(), GFL_NET_SystemIsLobbyError());
			WFLBY_CONNECT_WIN_EndTimeWait( &p_wk->talk_system );
			(*p_seq) = WFLBY_CONNECT_SEQ_ERRON;
		}

		if( GFL_NET_StateIsWifiLoginState() ){

			WFLBY_CONNECT_WIN_EndTimeWait( &p_wk->talk_system );

			{
				RECORD* p_rec;

				p_rec = SaveData_GetRecord( p_wk->p_save );
				RECORD_Score_Add( p_rec, SCORE_ID_LOBBY_LOGIN );
			}

			// ���O�C���������Ԃ�ۑ�
			{
				p_param->p_wflby_counter->time = GFL_RTC_GetDateTimeBySecond();
			}
			
			p_param->enter = TRUE;	// ���ꊮ��
			(*p_seq)++;
		}
		break;

	// �G���[�\��
	case WFLBY_CONNECT_SEQ_ERRON:
		{
			GFL_NETSTATE_DWCERROR* pErr;
			int msgno,err_no;
			if( GFL_NET_SystemIsError() ){
				pErr = GFL_NET_StateGetWifiError();
				msgno = WFLBY_ERR_GetStrID( pErr->errorCode,  pErr->errorType);
                err_no = pErr->errorCode;
			}else{
				err_no = DWC_LOBBY_GetErr();
				err_no = DWC_LOBBY_GetErrNo( err_no );
				msgno = dwc_lobby_0001;
			}

			// WiFi�N���u�ł̏�Ԃ�NONE�ɂ���o�b�t�@��j��
			WFLBY_SYSTEM_WiFiClubBuff_Exit( p_param->p_system );

			WFLBY_CONNECT_WIN_Off( &p_wk->talk );
			WFLBY_CONNECT_WIN_Off( &p_wk->talk_system );
			WFLBY_CONNECT_WIN_SetErrNumber( &p_wk->system, err_no );
			WFLBY_CONNECT_WIN_Print( &p_wk->system, msgno );
			(*p_seq)++;
		}
		break;

	// �L�[���͑҂�
	case WFLBY_CONNECT_SEQ_ERR:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
			WFLBY_ERR_TYPE err_type;
			int err_no;

			if( GFL_NET_SystemIsError() ){
				GFL_NETSTATE_DWCERROR* pErr = GFL_NET_StateGetWifiError();
				err_type = WFLBY_ERR_GetErrType( pErr->errorCode, pErr->errorType );
				if( err_type == WFLBY_ERR_TYPE_RETRY ){
					// �Đڑ�
					(*p_seq) = WFLBY_CONNECT_SEQ_RETRY;
				}else{
					// ������
					(*p_seq) = WFLBY_CONNECT_SEQ_LOGOUT;
					break;
				}
			}else{

				// �Đڑ�
				(*p_seq) = WFLBY_CONNECT_SEQ_RETRY;
			}
		}
		break;

	// �Đڑ�����
	case WFLBY_CONNECT_SEQ_RETRY:
		WFLBY_CONNECT_WIN_Off( &p_wk->system );
		WFLBY_CONNECT_WIN_Print( &p_wk->talk, msg_wifilobby_052 );

		p_wk->p_yesno = BmpMenu_YesNoSelectInit( &sc_WFLBY_BMPWIN_DAT_YESNO, 
				WFLBY_SYSWINGRA_CGX, WFLBY_MAIN_PLTT_SYSWIN, 0, HEAPID_WFLBY_ROOM );
		(*p_seq)++;
		break;
	
	// �Đڑ���
	case WFLBY_CONNECT_SEQ_RETRYWAIT:
		{
			u32 result;
			result = BmpMenu_YesNoSelectMain( p_wk->p_yesno );
			switch( result ){
			// YES
			case 0:	
				// �������񃍃O�A�E�g
				GFL_NET_StateWifiLobbyLogout();
				(*p_seq) = WFLBY_CONNECT_SEQ_RETRYLOGOUTWAIT;
				break;

			// NO
			case BMPMENU_CANCEL:
				(*p_seq) = WFLBY_CONNECT_SEQ_LOGOUT;
				break;
			}
		}
		break;

	// �Đڑ����̃��O�A�E�g�҂�
	case WFLBY_CONNECT_SEQ_RETRYLOGOUTWAIT:
		if( GFL_NET_IsInit() == FALSE ){
			(*p_seq) = WFLBY_CONNECT_SEQ_LOGIN;
		}
		break;

	// ���O�A�E�g
	case WFLBY_CONNECT_SEQ_LOGOUT:
		WFLBY_CONNECT_WIN_Off( &p_wk->talk );
		WFLBY_CONNECT_WIN_Off( &p_wk->talk_system );
		WFLBY_CONNECT_WIN_Off( &p_wk->system );
		GFL_NET_StateWifiLobbyLogout();
		(*p_seq)++;
		break;
		
	// ���O�A�E�g�܂�
	case WFLBY_CONNECT_SEQ_LOGOUTWAIT:
		if( GFL_NET_IsInit() == FALSE ){
			WFLBY_CONNECT_WIN_Off( &p_wk->talk );
			WFLBY_CONNECT_WIN_Off( &p_wk->talk_system );
			(*p_seq) = WFLBY_CONNECT_SEQ_FADEOUT;
		}
		break;

	// �t�F�[�h�A�E�g
	case WFLBY_CONNECT_SEQ_FADEOUT:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WFLBY_ROOM );
		(*p_seq)++;
		break;

	// �t�F�[�h�A�E�g�҂�
	case WFLBY_CONNECT_SEQ_FADEOUTWAIT:
		if( WIPE_SYS_EndCheck() ){
			return GFL_PROC_RES_FINISH;
		}
		break;

	default:
		GF_ASSERT( 0 );
		break;
	}

	ConnectBGPalAnm_Main(&p_wk->cbp);
	
	return GFL_PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���O�C���@�I��
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WFLBY_CONNECT_Exit(GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk)
{
	WFLBY_CONNECTWK* p_wk;
	
	p_wk	= mywk;

	// ���荞�ݐݒ�
	GFL_TCB_DeleteTask(p_wk->vintr_tcb);
	//sys_HBlankIntrStop();	//HBlank���荞�ݒ�~

	ConnectBGPalAnm_End(&p_wk->cbp);

	//�t�H���g�j��
	GFL_FONT_Delete(p_wk->fontHandle_talk);
	GFL_FONT_Delete(p_wk->fontHandle_system);
	
	//TCBL�j��
	GFL_TCBL_Exit(p_wk->tcbl);
	//PrintQue�j��
	PRINTSYS_QUE_Delete(p_wk->printQue);

	// �E�B���h�E�V�X�e���j��
	WFLBY_CONNECT_WIN_Exit( &p_wk->talk );
	WFLBY_CONNECT_WIN_Exit( &p_wk->talk_system );
	WFLBY_CONNECT_WIN_Exit( &p_wk->system );
	WFLBY_CONNECT_WIN_Exit( &p_wk->title );

	// �O���t�B�b�N�j��
	WFLBY_CONNECT_GraphicExit( p_wk );

	ConnectBGPalAnm_OccSet(&p_wk->cbp, FALSE);

	// ���[�N�j��
	GFL_PROC_FreeWork(p_proc);

	// �q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_WFLBY_ROOM );

	return GFL_PROC_RES_FINISH;
}




//----------------------------------------------------------------------------
/**
 *	@brief	�ؒf����	������
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WFLBY_DISCONNECT_Init(GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk)
{
	WFLBY_CONNECTWK* p_wk;
	WFLBY_DISCONNECT_PARAM* p_param;

	// �q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WFLBY_ROOM, 0x20000 );
	
	// ���[�N�쐬
	p_wk = GFL_PROC_AllocWork( p_proc, sizeof(WFLBY_CONNECTWK), HEAPID_WFLBY_ROOM );
	GFL_STD_MemFill( p_wk, 0, sizeof(WFLBY_CONNECTWK) );

	// �p�����[�^�擾
	p_param	= pwk;
	p_wk->p_save		= p_param->p_save;

	//TCBL�쐬
	p_wk->tcbl = GFL_TCBL_Init( HEAPID_WFLBY_ROOM, HEAPID_WFLBY_ROOM, 4, 32);
	//PrintQue�쐬
	p_wk->printQue = PRINTSYS_QUE_Create(HEAPID_WFLBY_ROOM);

	// �O���t�B�b�N������
	WFLBY_CONNECT_GraphicInit( p_wk, HEAPID_WFLBY_ROOM );

	// �t�H���g�쐬	��check PL�̂悤�ɉ�b�t�H���g�A�V�X�e���t�H���g�Ƃ��������̂��܂������̂�
	//						�ǂ���������t�H���g��ǂݍ���ł��� 2009.03.10(��) matsuda
	p_wk->fontHandle_talk = GFL_FONT_Create(ARCID_FONT, NARC_font_large_nftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WFLBY_ROOM);
	p_wk->fontHandle_system = GFL_FONT_Create(ARCID_FONT, NARC_font_large_nftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WFLBY_ROOM);

	// �E�B���h�E�V�X�e��������
	WFLBY_CONNECT_WIN_Init( &p_wk->talk, p_wk->fontHandle_talk, 
			p_wk->tcbl, p_wk->printQue,
			FONTID_TALK, NARC_message_wifi_system_dat,
			WFLBY_TALKWIN_X, WFLBY_TALKWIN_Y, 
			WFLBY_TALKWIN_SIZX, WFLBY_TALKWIN_SIZY,
			WFLBY_TALKWIN_CGX, p_wk->p_save, HEAPID_WFLBY_ROOM );
	WFLBY_CONNECT_WIN_Init( &p_wk->system, p_wk->fontHandle_system, 
			p_wk->tcbl, p_wk->printQue,
			FONTID_SYSTEM, NARC_message_wifi_system_dat,
			WFLBY_SYSTEMWIN_X, WFLBY_SYSTEMWIN_Y,
			WFLBY_SYSTEMWIN_SIZX, WFLBY_SYSTEMWIN_SIZY,
			WFLBY_SYSTEMWIN_CGX, p_wk->p_save, HEAPID_WFLBY_ROOM );


	WFLBY_CONNECT_WIN_Init( &p_wk->title, p_wk->fontHandle_talk, 
			p_wk->tcbl, p_wk->printQue,
			FONTID_TALK, NARC_message_wifi_lobby_dat,
			WFLBY_TITLEWIN_X, WFLBY_TITLEWIN_Y, 
			WFLBY_TITLEWIN_SIZX, WFLBY_TITLEWIN_SIZY,
			WFLBY_TITLEWIN_CGX, p_wk->p_save, HEAPID_WFLBY_ROOM );


	//  �^�C�g����������
	WFLBY_CONNECT_WIN_PrintTitle( &p_wk->title, msg_wifilobby_018 );

#if PL_G0220_081027_FIX
	// �A�C�R��OAM�\��
#if WB_TEMP_FIX
    WirelessIconEasy();
#endif
#endif

	// VBlank�֐��ݒ�
	p_wk->vintr_tcb = GFUser_VIntr_CreateTCB(WFLBY_CONNECT_VBlank, p_wk, 200);
	//sys_HBlankIntrStop();	//HBlank���荞�ݒ�~

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ؒf����	���C��
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WFLBY_DISCONNECT_Main(GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk)
{
	WFLBY_CONNECTWK* p_wk;
	WFLBY_DISCONNECT_PARAM* p_param;
	
	p_wk	= mywk;
	p_param	= pwk;

	PRINTSYS_QUE_Main(p_wk->printQue);
	PRINT_UTIL_Trans(&p_wk->title.print_util, p_wk->printQue);
	PRINT_UTIL_Trans(&p_wk->talk.print_util, p_wk->printQue);
	PRINT_UTIL_Trans(&p_wk->talk_system.print_util, p_wk->printQue);
	PRINT_UTIL_Trans(&p_wk->system.print_util, p_wk->printQue);

	switch( *p_seq ){
	// �t�F�[�h�C��
	case WFLBY_DISCONNECT_SEQ_FADEIN:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
				WIPE_FADE_OUTCOLOR, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WFLBY_ROOM );
		(*p_seq)++;
		break;

	// �t�F�[�h�C���E�G�C�g
	case WFLBY_DISCONNECT_SEQ_FADEWAIT:
		if( WIPE_SYS_EndCheck() ){
#if 0	// �^�C���A�E�g���b�Z�[�W�͍L����ŏo�����ƂɂȂ���
			if( p_param->timeout == TRUE ){
				(*p_seq) = WFLBY_DISCONNECT_SEQ_TIMEOUT_MSG;
			}else{
			}
#endif
		(*p_seq) = WFLBY_DISCONNECT_SEQ_LOGOUT_MSG;
		}
		break;

	// �ގ����b�Z�[�W�\��
	case WFLBY_DISCONNECT_SEQ_LOGOUT_MSG:
		WFLBY_CONNECT_WIN_Print( &p_wk->talk, dwc_message_0011 );
		// �G���[�`�F�b�N
		if( GFL_NET_SystemIsError() || GFL_NET_SystemIsLobbyError() ){
			// �G���[�Ȃ炷����LOGOUT
			(*p_seq) = WFLBY_DISCONNECT_SEQ_LOGOUT;
			WFLBY_CONNECT_WIN_StartTimeWait( &p_wk->talk );
		}else{
			(*p_seq) = WFLBY_DISCONNECT_SEQ_WLDSEND;
			WFLBY_CONNECT_WIN_StartTimeWait( &p_wk->talk );
		}
		break;

	//  ���E�f�[�^���M
	case WFLBY_DISCONNECT_SEQ_WLDSEND:
		DWC_LOBBY_WLDDATA_Send();

		// �^�C���E�G�C�g�ݒ�
		p_wk->wait = WFLBY_DISCONNECT_WLDSENDWAIT_TIMEOUT;
		(*p_seq) = WFLBY_DISCONNECT_SEQ_WLDWAIT;
		break;
		
	//  ���E�f�[�^���M�����҂�
	case WFLBY_DISCONNECT_SEQ_WLDWAIT:
		if( (p_wk->wait - 1) >= 0 ){
			p_wk->wait --;
		}
		// �f�[�^���u���[�h�L���X�g����邩�^�C���A�E�g�����邩�Ń��O�A�E�g�����ɂ���
		if( DWC_LOBBY_WLDDATA_SendWait() || (p_wk->wait == 0) ){
			(*p_seq) = WFLBY_DISCONNECT_SEQ_LOGOUT;
		}
		break;

	// ���O�A�E�g����
	case WFLBY_DISCONNECT_SEQ_LOGOUT:
		GFL_NET_StateWifiLobbyLogout();
		(*p_seq) = WFLBY_DISCONNECT_SEQ_LOGOUT_WAIT;
		break;

	case WFLBY_DISCONNECT_SEQ_LOGOUT_WAIT:
		if( GFL_NET_IsInit() == FALSE ){

			// �^�C���A�E�g�I��
			WFLBY_CONNECT_WIN_EndTimeWait( &p_wk->talk_system );

			// WiFi�N���u�p�ɑ�����_�~�[����j������
			WFLBY_SYSTEM_WiFiClubBuff_Exit( p_param->p_system );
			
			(*p_seq) = WFLBY_DISCONNECT_SEQ_LOGOUT_END;
		}
		break;

	// ���O�A�E�g�������b�Z�[�W
	case WFLBY_DISCONNECT_SEQ_LOGOUT_END:
		WFLBY_CONNECT_WIN_Print( &p_wk->talk, dwc_message_0012 );

		// �^�C���E�G�C�g�ݒ�
		p_wk->wait = WFLBY_DISCONNECT_LOGOUT_ENDMSG_WAIT;

		(*p_seq) = WFLBY_DISCONNECT_SEQ_LOGOUT_END_MSGWAIT;
		break;

	// ���O�A�E�g�������b�Z�[�W�҂�
	case WFLBY_DISCONNECT_SEQ_LOGOUT_END_MSGWAIT:
		if( (p_wk->wait - 1) >= 0 ){
			p_wk->wait --;
		}
		if( p_wk->wait == 0 ){
			(*p_seq) = WFLBY_DISCONNECT_SEQ_FADEOUT;
		}
		break;

	case WFLBY_DISCONNECT_SEQ_FADEOUT:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WFLBY_ROOM );
		(*p_seq)++;
		break;

	case WFLBY_DISCONNECT_SEQ_FADEOUTWAIT:
		if( WIPE_SYS_EndCheck() ){
			WFLBY_CONNECT_WIN_EndTimeWait( &p_wk->talk );
			return GFL_PROC_RES_FINISH;
		}
		break;

	}

	ConnectBGPalAnm_Main(&p_wk->cbp);
	
	return GFL_PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ؒf����	�j��
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WFLBY_DISCONNECT_Exit(GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk)
{
	WFLBY_CONNECTWK* p_wk;
	
	p_wk	= mywk;

	// ���荞�ݐݒ�
	GFL_TCB_DeleteTask(p_wk->vintr_tcb);
	//sys_HBlankIntrStop();	//HBlank���荞�ݒ�~

	ConnectBGPalAnm_End(&p_wk->cbp);

	//�t�H���g�j��
	GFL_FONT_Delete(p_wk->fontHandle_talk);
	GFL_FONT_Delete(p_wk->fontHandle_system);
	
	//TCBL�j��
	GFL_TCBL_Exit(p_wk->tcbl);
	//PrintQue�j��
	PRINTSYS_QUE_Delete(p_wk->printQue);

	// �E�B���h�E�V�X�e���j��
	WFLBY_CONNECT_WIN_Exit( &p_wk->talk );
	WFLBY_CONNECT_WIN_Exit( &p_wk->system );
	WFLBY_CONNECT_WIN_Exit( &p_wk->title );

	// �O���t�B�b�N�j��
	WFLBY_CONNECT_GraphicExit( p_wk );

	ConnectBGPalAnm_OccSet(&p_wk->cbp, FALSE);

	// ���[�N�j��
	GFL_PROC_FreeWork(p_proc);

	// �q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_WFLBY_ROOM );

	return GFL_PROC_RES_FINISH;
}





//-----------------------------------------------------------------------------
/**
 *				�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	VBlank�֐�
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_VBlank(GFL_TCB *tcb, void *p_work)
{
	WFLBY_CONNECTWK* p_wk = p_work;

	WFLBY_CONNECT_GraphicVBlank( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N������
 *
 *	@param	p_wk		���[�N
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_GraphicInit( WFLBY_CONNECTWK* p_wk, u32 heapID )
{
	// ���������W�X�^OFF
	G2_BlendNone();
	G2S_BlendNone();
	
	// BANK�ݒ�
	GFL_DISP_SetBank( &sc_WFLBY_BANK );
	//VRAM�N���A
	GFL_STD_MemClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
	GFL_STD_MemClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
	GFL_STD_MemClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
	GFL_STD_MemClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);

	// �o�b�N�O���E���h�����ɂ���
	{
		GFL_BG_SetBackGroundColor( sc_WFLBY_BGCNT_FRM[WFLBY_BGCNT_MAIN_BACK], 0 );
	}
	

	// BG�ݒ�
	{
		int i;

		GFL_BG_SetBGMode(&sc_BGINIT);

		GFL_BG_Init( heapID );
		GFL_BMPWIN_Init(heapID);

		for( i=0; i<WFLBY_BGCNT_NUM; i++ ){
			GFL_BG_SetBGControl( 
					sc_WFLBY_BGCNT_FRM[i], &sc_WFLBY_BGCNT_DATA[i],
					GFL_BG_MODE_TEXT );
			GFL_BG_SetVisible(sc_WFLBY_BGCNT_FRM[i], VISIBLE_ON);
			GFL_BG_SetClearCharacter( sc_WFLBY_BGCNT_FRM[i], 32, 0, heapID);
			GFL_BG_ClearScreen( sc_WFLBY_BGCNT_FRM[i] );
			GFL_BG_SetVisible(sc_WFLBY_BGCNT_FRM[i], VISIBLE_ON);
		}
	}
	

	// �E�B���h�E�ݒ�
	{
		CONFIG* p_config;
		u8 winnum;

		p_config = SaveData_GetConfig( p_wk->p_save );
		winnum = CONFIG_GetWindowType( p_config );

		// �����p���b�g
		SystemFontPaletteLoad( PALTYPE_MAIN_BG, WFLBY_MAIN_PLTT_SYSFONT*32, heapID );
		TalkFontPaletteLoad( PALTYPE_MAIN_BG, WFLBY_MAIN_PLTT_TALKFONT*32, heapID );

		// �E�B���h�E�O���t�B�b�N
		BmpWinFrame_GraphicSet(
				sc_WFLBY_BGCNT_FRM[WFLBY_BGCNT_MAIN_WIN], WFLBY_SYSWINGRA_CGX,
				WFLBY_MAIN_PLTT_SYSWIN, 0, heapID );
		TalkWinFrame_GraphicSet(
				sc_WFLBY_BGCNT_FRM[WFLBY_BGCNT_MAIN_WIN], WFLBY_TALKWINGRA_CGX, 
				WFLBY_MAIN_PLTT_TALKWIN, winnum, heapID );
	}


	// �o�b�N�O���t�B�b�N����������
	{
		// �J���[�p���b�g
		GFL_ARC_UTIL_TransVramPalette( ARCID_WIFIP2PMATCH, 
				NARC_wifip2pmatch_conect_NCLR, 
				PALTYPE_MAIN_BG, WFLBY_MAIN_PLTT_BACKSTART, 
				WFLBY_MAIN_PLTT_BACKEND*32, heapID );
		GFL_ARC_UTIL_TransVramPalette( ARCID_WIFIP2PMATCH, 
				NARC_wifip2pmatch_conect_NCLR, 
				PALTYPE_SUB_BG, WFLBY_MAIN_PLTT_BACKSTART, 
				WFLBY_MAIN_PLTT_BACKEND*32, heapID );

		// �L�����N�^
		GFL_ARC_UTIL_TransVramBgCharacter( ARCID_WIFIP2PMATCH, 
				NARC_wifip2pmatch_conect_NCGR, 
				sc_WFLBY_BGCNT_FRM[WFLBY_BGCNT_MAIN_BACK], 0, 0, FALSE, heapID );
		GFL_ARC_UTIL_TransVramBgCharacter( ARCID_WIFIP2PMATCH, 
				NARC_wifip2pmatch_conect_sub_NCGR, 
				sc_WFLBY_BGCNT_FRM[WFLBY_BGCNT_SUB_BACK], 0, 0, FALSE, heapID );

		// �X�N���[��
		GFL_ARC_UTIL_TransVramScreen( ARCID_WIFIP2PMATCH, NARC_wifip2pmatch_conect_01_NSCR,
				sc_WFLBY_BGCNT_FRM[WFLBY_BGCNT_MAIN_BACK], 0, 0, FALSE, heapID );
		GFL_ARC_UTIL_TransVramScreen( ARCID_WIFIP2PMATCH, NARC_wifip2pmatch_conect_sub_NSCR,
				sc_WFLBY_BGCNT_FRM[WFLBY_BGCNT_SUB_BACK], 0, 0, FALSE, heapID );
	}

	// �ʐM�O���t�B�b�NON
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	
	{
		ARCHANDLE* p_handle;
		p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFIP2PMATCH, heapID );

		//Wifi�ڑ�BG�p���b�g�A�j���V�X�e��������
		ConnectBGPalAnm_Init(&p_wk->cbp, p_handle, 
			NARC_wifip2pmatch_conect_anm_NCLR, heapID);

		GFL_ARC_CloseDataHandle( p_handle );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�j��
 *	
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_GraphicExit( WFLBY_CONNECTWK* p_wk )
{
	// BG�̔j��
	{
		int i;

		for( i=0; i<WFLBY_BGCNT_NUM; i++ ){
			GFL_BG_FreeBGControl( sc_WFLBY_BGCNT_FRM[i] );
		}

		// BGL�j��
		GFL_BG_Exit();
		GFL_BMPWIN_Exit();
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N	VBLANK
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_GraphicVBlank( WFLBY_CONNECTWK* p_wk )
{
	// BGLVBLANK
    GFL_BG_VBlankFunc( );
}


//----------------------------------------------------------------------------
/**
 *	@brief	�E�B���h�E�I�u�W�F������
 *	
 *	@param	p_wk		���[�N
 *	@param	fontid		�t�H���gID
 *	@param	msgid		���b�Z�[�WID
 *	@param	x			�����W
 *	@param	y			�����W
 *	@param	sizx		�T�C�Y��
 *	@param	sizy		�T�C�Y��
 *	@param	cgx			�L�����N�^�I�t�Z�b�g
 *	@param	p_save		�Z�[�u�f�[�^
 *	@param	heapID		�q�[�v
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_WIN_Init( WFLBY_WINWK* p_wk, GFL_FONT *font_handle, GFL_TCBLSYS *tcbl, PRINT_QUE *printQue, u32 fontid, u32 msgid, u32 x, u32 y, u32 sizx, u32 sizy, u32 cgx, SAVE_CONTROL_WORK* p_save, u32 heapID )
{
  GF_ASSERT(printQue);
  
	p_wk->p_wordset = WORDSET_Create( heapID );
	p_wk->p_msgman	= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, msgid, heapID );
	p_wk->p_str		= GFL_STR_CreateBuffer( WFLBY_WINSYS_STRBUFNUM, heapID );
	p_wk->p_tmp		= GFL_STR_CreateBuffer( WFLBY_WINSYS_STRBUFNUM, heapID );
	p_wk->font_handle = font_handle;
	p_wk->fontid	= fontid;
	p_wk->msgspeed	= MSGSPEED_GetWait();//CONFIG_GetMsgPrintSpeed( SaveData_GetConfig( p_save ) );
#if WB_FIX
	p_wk->msgno		= 0;
#else
	p_wk->msg_stream = NULL;
	p_wk->tcbl = tcbl;
	p_wk->printQue = printQue;
#endif

#ifdef WFLBY_CONNECT_DEBUG_START
	p_wk->p_debug_msgman	= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_debug_hiroba_dat, heapID );	// ���[�h�Z�b�g
#endif

	p_wk->win = GFL_BMPWIN_Create( 
			sc_WFLBY_BGCNT_FRM[WFLBY_BGCNT_MAIN_WIN], x, y, sizx, sizy, WFLBY_MAIN_PLTT_TALKFONT, GFL_BMP_CHRAREA_GET_F );
	GFL_BMPWIN_MakeScreen(p_wk->win);
	PRINT_UTIL_Setup( &p_wk->print_util, p_wk->win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�E�B���h�E�I�u�W�F	���b�Z�[�W�\��
 *
 *	@param	p_wk		���[�N
 *	@param	strid		�\��STR
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_WIN_Print( WFLBY_WINWK* p_wk, u32 strid )
{
	// ���b�Z�[�W�\�����Ȃ����
#if WB_FIX
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}
#else
	if(p_wk->msg_stream != NULL){
		PRINTSYS_PrintStreamDelete(p_wk->msg_stream);
		p_wk->msg_stream = NULL;
	}
#endif

	// �E�B���h�E�̃N���[��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->win), 15 );

	// �\��
	GFL_MSG_GetString( p_wk->p_msgman, strid, p_wk->p_tmp );
	WORDSET_ExpandStr( p_wk->p_wordset, p_wk->p_str, p_wk->p_tmp );

#if WB_FIX
	GF_STR_PrintSimple(&p_wk->win, FONTID_TALK, p_wk->p_str,
			0,0,MSG_NO_PUT,NULL);
#else
	PRINT_UTIL_Print( &p_wk->print_util, p_wk->printQue, 0, 0, p_wk->p_str, p_wk->font_handle );
#endif

	if( p_wk->fontid == FONTID_SYSTEM ){	
		BmpWinFrame_Write( p_wk->win, WINDOW_TRANS_OFF, 
				WFLBY_SYSWINGRA_CGX, WFLBY_MAIN_PLTT_SYSWIN );
	}else{
		TalkWinFrame_Write( p_wk->win, WINDOW_TRANS_OFF, 
				WFLBY_TALKWINGRA_CGX, WFLBY_MAIN_PLTT_TALKWIN );
	}
	BmpWinFrame_TransScreen( p_wk->win ,WINDOW_TRANS_ON_V);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�E�B���h�E�\��OFF
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_WIN_Off( WFLBY_WINWK* p_wk )
{
	// ���b�Z�[�W�\�����Ȃ����
#if WB_FIX
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}
#else
	if(p_wk->msg_stream != NULL){
		PRINTSYS_PrintStreamDelete(p_wk->msg_stream);
		p_wk->msg_stream = NULL;
	}
#endif

	if( p_wk->fontid == FONTID_SYSTEM ){
		BmpWinFrame_Clear( p_wk->win, WINDOW_TRANS_OFF );
		BmpWinFrame_TransScreen( p_wk->win ,WINDOW_TRANS_ON_V);
	}else{
		if( p_wk->p_timewait ){
			WFLBY_CONNECT_WIN_EndTimeWait( p_wk );
		}

		TalkWinFrame_Clear( p_wk->win, WINDOW_TRANS_OFF );
		BmpWinFrame_TransScreen( p_wk->win ,WINDOW_TRANS_ON_V);
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^�C���E�G�C�g�J�n
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_WIN_StartTimeWait( WFLBY_WINWK* p_wk )
{
	if( p_wk->fontid == FONTID_TALK ){
		GF_ASSERT( p_wk->p_timewait == NULL );
	#if WB_TEMP_FIX
		p_wk->p_timewait = TimeWaitIconAdd( &p_wk->win, WFLBY_TALKWINGRA_CGX );
	#endif
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^�C���E�G�C�g��~
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_WIN_EndTimeWait( WFLBY_WINWK* p_wk )
{
	if( p_wk->fontid == FONTID_TALK ){
	#if WB_TEMP_FIX
		GF_ASSERT( p_wk->p_timewait != NULL );
		TimeWaitIconDel( p_wk->p_timewait );
		p_wk->p_timewait = NULL;
	#endif
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�E�B���h�E�I�u�W�F	OFF
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_WIN_Exit( WFLBY_WINWK* p_wk )
{
	// ���b�Z�[�W�\�����Ȃ����
#if WB_FIX
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}
#else
	if(p_wk->msg_stream != NULL){
		PRINTSYS_PrintStreamDelete(p_wk->msg_stream);
		p_wk->msg_stream = NULL;
	}
#endif

	if( p_wk->p_timewait ){
		WFLBY_CONNECT_WIN_EndTimeWait( p_wk );
	}
	
	GFL_BMPWIN_Delete( p_wk->win );

	GFL_STR_DeleteBuffer( p_wk->p_tmp );
	GFL_STR_DeleteBuffer( p_wk->p_str );
	GFL_MSG_Delete( p_wk->p_msgman );
	WORDSET_Delete( p_wk->p_wordset );

#ifdef WFLBY_CONNECT_DEBUG_START
	GFL_MSG_Delete( p_wk->p_debug_msgman );
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����Z�[�W�`��
 *
 *	@param	p_wk		���[�N
 *	@param	strid		������
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_WIN_PrintWait( WFLBY_WINWK* p_wk, u32 strid )
{
	// ���b�Z�[�W�\�����Ȃ����
#if WB_FIX
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}
#else
	if(p_wk->msg_stream != NULL){
		PRINTSYS_PrintStreamDelete(p_wk->msg_stream);
		p_wk->msg_stream = NULL;
	}
#endif

	// �E�B���h�E�̃N���[��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->win), 15 );

	// �\��
	GFL_MSG_GetString( p_wk->p_msgman, strid, p_wk->p_tmp );
	WORDSET_ExpandStr( p_wk->p_wordset, p_wk->p_str, p_wk->p_tmp );

#if WB_FIX
	p_wk->msgno = GF_STR_PrintSimple(&p_wk->win, FONTID_TALK, p_wk->p_str,
			0,0, p_wk->msgspeed,NULL);
#else
	p_wk->msg_stream = PRINTSYS_PrintStream(p_wk->win, 0, 0, p_wk->p_str, p_wk->font_handle,
		p_wk->msgspeed, p_wk->tcbl, 10, HEAPID_WFLBY_ROOM, 0xff);
#endif

	if( p_wk->fontid == FONTID_SYSTEM ){	
		BmpWinFrame_Write( p_wk->win, WINDOW_TRANS_OFF, 
				WFLBY_SYSWINGRA_CGX, WFLBY_MAIN_PLTT_SYSWIN );
	}else{
		TalkWinFrame_Write( p_wk->win, WINDOW_TRANS_OFF, 
				WFLBY_TALKWINGRA_CGX, WFLBY_MAIN_PLTT_TALKWIN );
	}
	BmpWinFrame_TransScreen( p_wk->win ,WINDOW_TRANS_ON_V);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^�C�g�������`��֐�
 *
 *	@param	p_wk		���[�N	
 *	@param	strid		������ID
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_WIN_PrintTitle( WFLBY_WINWK* p_wk, u32 strid )
{
	// �E�B���h�E�̃N���[��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->win), 0 );

	// �\��
	GFL_MSG_GetString( p_wk->p_msgman, strid, p_wk->p_tmp );
	WORDSET_ExpandStr( p_wk->p_wordset, p_wk->p_str, p_wk->p_tmp );

#if WB_FIX
	GF_STR_PrintColor(&p_wk->win, FONTID_TALK, p_wk->p_str,
			0,0, MSG_ALLPUT, WFLBY_TITLEWIN_COL, NULL);
#else
	PRINT_UTIL_PrintColor( &p_wk->print_util, p_wk->printQue, 0, 0, 
		p_wk->p_str, p_wk->font_handle, WFLBY_TITLEWIN_COL );
#endif
}


#ifdef WFLBY_CONNECT_DEBUG_START
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�N�����̕\��
 *
 *	@param	p_wk		���[�N
 *	@param	strid		������
 *	@param	num			����
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_WIN_PrintDEBUG( WFLBY_WINWK* p_wk, u32 strid, u32 num )
{
	// ���[�h�Z�b�g�ɐ����ݒ�
	WORDSET_RegisterNumber( p_wk->p_wordset, 0, num, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );

	// �E�B���h�E�̃N���[��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->win), 15 );

	// �\��
	GFL_MSG_GetString( p_wk->p_debug_msgman, strid, p_wk->p_tmp );
	WORDSET_ExpandStr( p_wk->p_wordset, p_wk->p_str, p_wk->p_tmp );

#if WB_FIX
	GF_STR_PrintSimple(&p_wk->win, FONTID_TALK, p_wk->p_str,
			0,0,MSG_NO_PUT,NULL);
#else
	PRINT_UTIL_Print( &p_wk->print_util, p_wk->printQue, 0, 0, p_wk->p_str, p_wk->font_handle );
#endif

	if( p_wk->fontid == FONTID_SYSTEM ){	
		BmpWinFrame_Write( p_wk->win, WINDOW_TRANS_OFF, 
				WFLBY_SYSWINGRA_CGX, WFLBY_MAIN_PLTT_SYSWIN );
	}else{
		TalkWinFrame_Write( p_wk->win, WINDOW_TRANS_OFF, 
				WFLBY_TALKWINGRA_CGX, WFLBY_MAIN_PLTT_TALKWIN );
	}
	BmpWinFrame_TransScreen( p_wk->win ,WINDOW_TRANS_ON_V);
}

static void WFLBY_CONNECT_WIN_PrintDEBUG2( WFLBY_WINWK* p_wk, u32 strid, u32 item )
{
	// ���[�h�Z�b�g�ɐ����ݒ�
	WORDSET_RegisterWiFiLobbyItemName( p_wk->p_wordset, 0, item );

	// �E�B���h�E�̃N���[��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->win), 15 );

	// �\��
	GFL_MSG_GetString( p_wk->p_debug_msgman, strid, p_wk->p_tmp );
	WORDSET_ExpandStr( p_wk->p_wordset, p_wk->p_str, p_wk->p_tmp );

#if WB_FIX
	GF_STR_PrintSimple(&p_wk->win, FONTID_TALK, p_wk->p_str,
			0,0,MSG_NO_PUT,NULL);
#else
	PRINT_UTIL_Print( &p_wk->print_util, p_wk->printQue, 0, 0, p_wk->p_str, p_wk->font_handle );
#endif

	if( p_wk->fontid == FONTID_SYSTEM ){	
		BmpWinFrame_Write( p_wk->win, WINDOW_TRANS_OFF, 
				WFLBY_SYSWINGRA_CGX, WFLBY_MAIN_PLTT_SYSWIN );
	}else{
		TalkWinFrame_Write( p_wk->win, WINDOW_TRANS_OFF, 
				WFLBY_TALKWINGRA_CGX, WFLBY_MAIN_PLTT_TALKWIN );
	}
	BmpWinFrame_TransScreen( p_wk->win ,WINDOW_TRANS_ON_V);
}
#endif


//----------------------------------------------------------------------------
/**
 *	@brief	���[�h�Z�b�g�ɃG���[�i���o�[��ݒ肷��
 *
 *	@param	p_wk		���[�N
 *	@param	number		����
 */
//-----------------------------------------------------------------------------
static void WFLBY_CONNECT_WIN_SetErrNumber( WFLBY_WINWK* p_wk, u32 number )
{
    WORDSET_RegisterNumber( p_wk->p_wordset, 0, number,
                           5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
}


//==============================================================================
//	
//==============================================================================
static DWCUserData* _getMyUserData(void* pWork)
{
    WFLBY_CONNECTWK *wk = pWork;
    return WifiList_GetMyUserInfo(SaveData_GetWifiListData(wk->p_save));
}

static DWCFriendData* _getFriendData(void* pWork)
{
    WFLBY_CONNECTWK *wk = pWork;
    return WifiList_GetDwcDataPtr(SaveData_GetWifiListData(wk->p_save),0);
}

static void _deleteFriendList(int deletedIndex,int srcIndex, void* pWork)
{
    WFLBY_CONNECTWK *wk = pWork;

    WifiList_DataMarge(SaveData_GetWifiListData(wk->p_save), deletedIndex, srcIndex);
	//�t�����h���Ɏ��t�����e�B�A�f�[�^���}�[�W���� 2008.05.24(�y) matsuda
//	FrontierRecord_DataMarge(SaveData_GetFrontier(wk->pSaveData), deletedIndex, srcIndex);
}

//--------------------------------------------------------------
/**
 * ���ʌ����f�[�^�ւ̃|�C���^�擾
 * @param   pWork		
 * @retval  void *		
 */
//--------------------------------------------------------------
static void * _WFLBY_GetExchangeDataPtr(void *pWork)
{
  WFLBY_CONNECTWK *wk = pWork;
  return &wk->my_exchange_data;
}

//--------------------------------------------------------------
/**
 * ���ʌ����f�[�^�̃T�C�Y�擾
 * @param   pWork		
 * @retval  int		
 */
//--------------------------------------------------------------
static int _WFLBY_GetExchangeDataSize(void *pWork)
{
  return sizeof(WIFI_EXCHANGE_WORK);
}
