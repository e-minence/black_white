//=============================================================================
/**
 * @file	worldtrade_local.h
 * @brief	���E������ʃw�b�_�t�@�C��
 *          
 * @author	Akito Mori
 * @date    	2006.04.16
 */
//=============================================================================


#ifndef __WORLDTRADE_LOCAL_H__
#define __WORLDTRADE_LOCAL_H__


///< DP�̂ӂ������(??)
#define GS_DP_GISOU

#include "system/bmp_menulist.h"
#include "system/touch_subwindow.h"
//#include "system/numfont.h"
//#include "system/selbox.h"
//#include "communication/wm_icon.h"
//#include "savedata/zukanwork.h"
#include "savedata/wifilist.h"
#include "savedata/wifihistory.h"
#include "app/p_status.h"
//#include "demo/demo_trade.h"
//#include "battle/shinka.h"
#include "net_app/connect_anm.h"
#include "system/bmp_winframe.h"
#include "worldtrade_input.h"
#include "sound/pm_sndsys.h"
#include "worldtrade_adapter.h"
#include "system/mcss.h"
#include "system/mcss_tool.h"

// Proc_Main�V�[�P���X��`
enum {
	SEQ_INIT_DPW=0,
	SEQ_INIT,
	SEQ_FADEIN,
	SEQ_MAIN,
	SEQ_FADEOUT,
	SEQ_OUT,
	SEQ_EXIT,
};

// �T�u�v���Z�X����ԍ�
enum{
	WORLDTRADE_ENTER=0,		///< �ڑ����
	WORLDTRADE_TITLE,		///< �^�C�g���E���j���[
	WORLDTRADE_MYPOKE,		///< �悤�����݂�
	WORLDTRADE_PARTNER,		///< �����Ẵ|�P�����̂悤�����݂�
	WORLDTRADE_SEARCH,		///< �|�P��������������
	WORLDTRADE_MYBOX,		///< �����̃{�b�N�X
	WORLDTRADE_DEPOSIT,		///< �ق����|�P�����̏���
	WORLDTRADE_UPLOAD,		///< �|�P�����f�[�^���A�b�v���[�h
	WORLDTRADE_STATUS,		///< �|�P�����X�e�[�^�X�Ăяo��
	WORLDTRADE_DEMO,		///< �|�P�����X�e�[�^�X�Ăяo��
};

// �T�u�v���Z�X�ɓn�����[�h�i�����T�u�v���Z�X�ł��g�������Ⴄ�������邽�߁j
enum{
	// worldtrade_title.c
	MODE_OPENING=0,				///< ���߂Ă����Ƃ�
	MODE_CONTINUE,				///< �p�����邽�߂ɂ���
	MODE_NOT_END,				///< ����߂��I�񂾂��A��߂Ȃ�����

	// worldtrade_mypoke.c
	MODE_VIEW,					///< �a�����|�P�����̂悤�����݂�
	MODE_DEPOSIT,				///< ���������|�P�����ƌ�������Ƃ��ɂ݂�

	// worldtrade_box.c
	MODE_DEPOSIT_SELECT,		///< ��������|�P������a����Ƃ�
	MODE_EXCHANGE_SELECT,		///< ���������|�P�����ƌ�������|�P������I��
	
	// worldtrade_upload.c
	MODE_UPLOAD,				///< �|�P������a�����ʂɍs��
	MODE_DOWNLOAD,				///< �|�P�������������
	MODE_EXCHANGE,				///< �|�P��������������
	MODE_DOWNLOAD_EX,
	MODE_SERVER_CHECK,			///< �|�P��������������Ă��邩�m�F���遨���j���[��
	MODE_POKEMON_EVO_SAVE,		///< �����|�P�������i��������̃Z�[�u����

	// worldtrade_search.c
	MODE_NORMAL,				///< ������ʂɕ��ʂɓ���
	MODE_PARTNER_VIEW,			///< ����������ʂɍs��
	MODE_PARTNER_RETURN,		///< ����������ʂ���߂��Ă���

	// worldtrade_partner.c
	MODE_FROM_SEARCH,			///< ������ʂ��炫��
	MODE_FROM_BOX,				///< BOX��ʂ��炫��
	MODE_TO_BOX,				///< BOX��ʂɍs��
	
};

///< CLACT�Œ�`���Ă���Z�����傫�����ăT�u��ʂɉe�����łĂ��܂��̂ŗ����Ă݂�
#define NAMEIN_SUB_ACTOR_DISTANCE 	(512)
//		256*FX32_ONE

///< CellActor�ɏ��������郊�\�[�X�}�l�[�W���̎�ނ̐��i���}���`�Z���E�}���`�Z���A�j���͎g�p���Ȃ��j
enum
{	
	CLACT_U_CHAR_RES,
	CLACT_U_PLTT_RES,
	CLACT_U_CELL_RES,
	CLACT_RESOURCE_NUM,
};
#define NAMEIN_OAM_NUM			( 14 )

#define WORLDTRADE_MESFRAME_PAL	 ( /*10*/14 )	//�����{�b�N�X���X�g�Ƃ��Ԃ邩��ύX
#define WORLDTRADE_MENUFRAME_PAL ( 11 )
#define WORLDTRADE_MESFRAME_CHR	 (  1 )
#define WORLDTRADE_MENUFRAME_CHR ( WORLDTRADE_MESFRAME_CHR + TALK_WIN_CGX_SIZ )
#define WORLDTRADE_TALKFONT_PAL	 ( 13 )
#define WORLDTRADE_SUB_TALKFONT_PAL	( 1 )

///< ��b�E�C���h�E�\���ʒu��`
#define TALK_WIN_X		(  2 )
#define TALK_WIN_Y		( 19 )
#define	TALK_WIN_SX		( 27 )
#define	TALK_WIN_SY		(  4 )


///< �P�s���b�Z�[�W�E�C���h�E�\���ʒu��`
#define LINE_TEXT_X		(  2 )
#define LINE_TEXT_Y		( 21 )
#define	LINE_TEXT_SX	( 27 )
#define	LINE_TEXT_SY	(  2 )


///< ��ʏ㕔�^�C�g���E�C���h�E�\���ʒu��`
#define TITLE_TEXT_X	(  2 )
#define TITLE_TEXT_Y	(  1 )
#define TITLE_TEXT_SX	( 28 )
#define TITLE_TEXT_SY	(  2 )

#define HEAD_LIST_X		( 15 )
#define HEAD_LIST_Y		(  5 )
#define HEAD_LIST_SX	(  4 )
#define HEAD_LIST_SY	( 13 )

#define NAME_LIST_X		( 21 )
#define NAME_LIST_Y		(  5 )
#define NAME_LIST_SX	( 10 )
#define NAME_LIST_SY	( 13 )


// �I�����X�g�̈ʒu�E����`(_deposit.c _search.c)
// ������
#define SELECT_MENU1_X	(  15 )
#define SELECT_MENU1_Y 	(   5 )
#define SELECT_MENU1_SX	(   4 )
#define SELECT_MENU1_SY	(  13 )

// �|�P������
#define SELECT_MENU2_X	(  21 )
#define SELECT_MENU2_Y 	(   5 )
#define SELECT_MENU2_SX	(  10 )
#define SELECT_MENU2_SY	(  13 )

// ����
#define SELECT_MENU3_X	(  21 )
#define SELECT_MENU3_Y 	(  10 )
#define SELECT_MENU3_SX	(  10 )
#define SELECT_MENU3_SY	(   8 )

// �~�������x��
#define SELECT_MENU4_X	(  15 )
#define SELECT_MENU4_Y 	(   5 )
#define SELECT_MENU4_SX	(  16 )
#define SELECT_MENU4_SY	(  13 )

// ��
#define SELECT_MENU5_X	(  2 )
#define SELECT_MENU5_Y 	(   5 )
#define SELECT_MENU5_SX	(  28 )
#define SELECT_MENU5_SY	(  13 )

// �uGTS�v�Ƃ��u�|�P�����������ł��������v�Ƃ�
#define EXPLAIN_WIN_X	(  13 )
#define EXPLAIN_WIN_Y 	(  19 )
#define EXPLAIN_WIN_SX	(  16 )
#define EXPLAIN_WIN_SY	(   4 )

#define GTS_EXPLAIN_OFFSET	   ( 1 )


///< �͂��E�������E�C���h�E��Y���W
#define	WORLDTRADE_YESNO_PY2	( 10 )		// ��b�E�C���h�E���Q�s�̎�
#define	WORLDTRADE_YESNO_PY1	( 12 )		// ��b�E�C���h�E���P�s�̎�

///< �^�C�g��������o�b�t�@��
#define TITLE_MESSAGE_BUF_NUM	( 20 * 2 )

///< ��b�E�C���h�E������o�b�t�@��
#define TALK_MESSAGE_BUF_NUM	( 90*2 )

#define DWC_ERROR_BUF_NUM		(16*8*2)

///< �{�b�N�X�g������
#define BOX_TRAY_NAME_BUF_NUM	( 9*2 )

///< �㉺��ʎw���`
#define RES_NUM	( 3 )
#define MAIN_LCD	( GFL_BG_MAIN_DISP )	// �v�͂O��
#define SUB_LCD		( GFL_BG_SUB_DISP )		// �P�Ȃ�ł����B
#define CHARA_RES	( 2 )

///< �T�u��ʂ̃e�L�X�g�p���b�g�]���ʒu
#define WORLDTRADE_MESFRAME_SUB_PAL	 	(  2 )

///< �T�u��ʂ̕����t�H���g�]���ʒu
#define WORLDTRADE_TALKFONT_SUB_PAL		( 1 )


// BMPWIN�w��
enum{
	BMP_NAME1_S_BG0,
	BMP_NAME2_S_BG0,
	BMP_NAME3_S_BG0,
	BMP_NAME4_S_BG0,
	BMP_NAME5_S_BG0,
	BMP_RECORD_MAX,
};

///< �P��ʂ̃{�b�N�X�̒��̃|�P�����̐�
#define BOX_POKE_NUM		( 30 )

///< �|�P�����A�C�R���̃p���b�g��]������I�t�Z�b�g
#define POKEICON_PAL_OFFSET				(  3 )

///< �I���ł��Ȃ��|�P�����A�C�R���̃p���b�g
#define POKEICON_NOTSELECT_PAL_OFFSET	(  6 )

///< �|�P�����A�C�R�����n�܂�VRAM��ł̃I�t�Z�b�g
#define POKEICON_VRAM_OFFSET	(12 + 16)


///< �T�O�����̃|�P�����̓������̐��i�A�J�T�^�i�n�}�������j
#define HEADWORD_NUM		( 10 )

///< �A�C�e���E���[���A�C�R���̃Z���A�j���ԍ�
#define CELL_ITEMICON_NO	( 40 )
#define CELL_MAILICON_NO	( 41 )
#define CELL_CBALLICON_NO	( 42 )

///< �{�b�N�X���̖��
#define CELL_BOXARROW_NO	( 38 )


///< �|�P��������
#define SEARCH_POKE_MAX		(  7 )

#define SUB_OBJ_NUM			(  8 )

///< 1�b�҂�
#define WAIT_ONE_SECONDE_NUM	( 30 )

///< ���C�v�̃X�s�[�h
#define WORLDTRADE_WIPE_SPPED	( 6 )

///< �T�[�o�[�m�F���ł���悤�ɂȂ�܂ł̕b���i�U�O�b�j
#define SEVER_RETRY_WAIT		( 60*30 )

///< �^�b�`�Ή��I���{�b�N�X�̕\���ݒ�
#define WORLDTRADE_SELBOX_X		( 20  )
#define WORLDTRADE_SELBOX_W		( 5*2 )

///< ��ʂ��㉺����ւ��ۂ̃��C�v�ɂ�����V���N��
#define EXCHANGE_SCREEN_SYNC	( 16 )

///< �p�b�V�u��Ԃ̋P�x
#define PASSIVE_SCREEN_RATE		(  9 )


///< �uDS�̉���ʂ����Ăˁv�A�C�R���̍��W
#define DS_ICON_X	(  55 )
#define DS_ICON_Y	( 176-8 )


///< ���������̃��x���w��̎g�p���郁�b�Z�[�W�e�[�u�����w�肷��
enum{
	LEVEL_PRINT_TBL_DEPOSIT,	///<�a���鎞�̃��x�������w��e�[�u��
	LEVEL_PRINT_TBL_SEARCH,		///<�������鎞�̃��x�������w��e�[�u��
};

///< ���������^�C�v
enum{
	TRADE_TYPE_DEPOSIT,		///<�a���Č�������
	TRADE_TYPE_SEARCH,		///<�������Č�������
};



enum{
	EXPLAIN_GTS=0,
	EXPLAIN_KOUKAN,
	EXPLAIN_YOUSU,
	EXPLAIN_AZUKERU,
	EXPLAIN_SAGASU,
};

//============================================================================================
//	�\���̒�`
//============================================================================================

// �|�P������a����ۂ̏��
typedef struct{

	// �|�P�����̎푰�����i�[����e�[�u���i2�d��ALLOC����j
	STRBUF **pokename;

	// �|�P�����a���E�������p
	u16		headwordPos;		///< �I�𓪃J�[�\���ʒu
	u16		headwordListPos;	///< �I�����X�g�ʒu
	u16		namePos;			///< ���O�I���J�[�\���ʒu
	u16		nameListPos;		///< ���O�I�����X�g�ʒu
	int		sexPos;				///< ���ʃJ�[�\���ʒu
	int		levelPos;			///< ���x���J�[�\���ʒu

	u8		*sinouTable;		///< �V���I�E�}�Ӄf�[�^�i���l�͊J��NO�j
	u16		*nameSortTable;		///< �|�P�������\�[�g�e�[�u���ǂݍ��݃|�C���^
	int		nameSortNum;		///< ������
	int		sex_selection;		///< �I�������|�P�����̐��ʕ��z��


	// �|�P����������ʗp
	int		cursorSide;			///< ���E�ǂ���ɃJ�[�\�������邩
	int		leftCursorPos;		///< ����̃J�[�\���ʒu
	int		rightCursorPos;		///< �E��̃J�[�\���ʒu

}DEPOSIT_WORK;

typedef struct{
	Dpw_Tr_PokemonDataSimple	info[BOX_POKE_NUM];
}BOX_RESEARCH;

// ��x���������|�P�������i���������̂��߂ɁA2��ڂ̏������݈ʒu��ۑ����Ă����\����
typedef struct{
	int boxno;
	int pos;
}EVOLUTION_POKEMON_INFO;

#define NAME_HEAD_MAX	( 10 )	///< [�A�J�T�^�i�n�}������]�̐��͂P�O��

typedef struct{
	u16 head_list,head_pos;

	u16 name_list[NAME_HEAD_MAX];
	u16 name_pos[NAME_HEAD_MAX];
}SELECT_LIST_POS;

typedef struct _WORLDTRADE_WORK{
	// �N�����ɖ���Ă����
	WORLDTRADE_PARAM *param;							///< �Ăяo�����p�����[�^
	WIFI_LIST		*wifilist;							///< wifi�ڑ��f�[�^�E�F�B�f�[�^
	
	
	// ���E�����V�X�e���Ɋւ�����
	int				seq;								///< ���݂̐��E������ʂ̏��
	int				nextseq;							///< ���݂̃V�[�P���X���I�������ۂ̑J�ڐ�

	int				sub_process;						///< ���E�����T�u�v���O��������i���o�[
	int				sub_nextprocess;					///< ���E�����T�uNEXT����i���o�[
	int				sub_returnprocess;					///< next�v���Z�X���Ă�ŏI���������̖߂�v���Z�X
	int				old_sub_process;					///< ��O�̃T�u�v���Z�X��ۑ����Ă���
	int				sub_process_mode;					///< ���̉�ʂɂ������[�h�͉����H
	int				error_mes_no;

	int				subprocess_seq;						///< �T�u�v���O�����V�[�P���XNO
	int				subprocess_nextseq;					///< �T�u�v���O����NEXT�V�[�P���XNO

	u16				OpeningFlag;						///< ���E�����^�C�g�����������H
	u16				DepositFlag;						///< �|�P���������ɃT�[�o�[�ɗa���Ă��邩�H�i1:�a���Ă���j
	u16				ExchangeFlag;						///< �a�����|�P��������������Ă���
	u16				serverWaitTime;						///< �T�[�o�[�ɃA�N�Z�X�ł��Ȃ��悤�ɂ�����^�C�}�[

	int				ConnectErrorNo;						///< DWC�E�܂��̓T�[�o�[����̃G���[
	int				ErrorRet;
	int				ErrorCode;
    int ErrorType;

	void 			*heapPtr;							///< NitroDWC�ɓn���q�[�v���[�N�̉���p�|�C���^
	NNSFndHeapHandle heapHandle;						///< heapPtr��32�o�C�g�A���C�����g�ɍ��킹���|�C���^
	DWCInetControl   stConnCtrl;						///< DWC�ڑ����[�N

	GFL_PROC			*proc;								///< �X�^�[�^�X�Ăяo���p�T�u�v���Z�X�f�[�^
	PSTATUS_DATA	 statusParam;						///< �X�^�[�^�X�Ăяo���p�p�����[�^
	DEMO_TRADE_PARAM tradeDemoParam;					///< �����f���p�����[�^
	SHINKA_WORK		 *shinkaWork;						///< �ʐM�i���f���p���[�N
	int				 subprocflag;						///< �X�e�[�^�X������f�����Ăяo�����߂̃t���O
	u16				listpos;				
	u16				dummy;

	// �e��ʂ��ʂ��Ă��ۑ�����Ă����
	u16				TitleCursorPos;						///< �^�C�g�����j���[�̃J�[�\���ʒu
	u16				PartnerPageInfo;					///< ���E�E�y�[�W�̂ǂ�����݂Ă��邩�H
	u16				BoxTrayNo;							///< ���ԃ{�b�N�X�����Ă��邩�H
	u16				BoxCursorPos;						///< �{�b�N�X��ł̃J�[�\���̈ʒu
	POKEMON_PASO_PARAM	*deposit_ppp;					///< ��U�a����w��ɂȂ����|�P�����̃|�C���^
	int				SearchResult;						///< �����̌��ʕԂ��Ă�����
	int				TouchTrainerPos;					///< �������ʂ̒N���^�b�`������
	MYSTATUS		*partnerStatus;						///< �����f���悤�ɂł���������MYSTATUS;
	EVOLUTION_POKEMON_INFO EvoPokeInfo;


	Dpw_Tr_Data		UploadPokemonData;					///< ���M�f�[�^
	Dpw_Tr_Data		DownloadPokemonData[SEARCH_POKE_MAX];///< �������ʃf�[�^
	Dpw_Tr_Data		ExchangePokemonData;				///< �������ʃf�[�^
	Dpw_Tr_PokemonDataSimple Post;						///< �a����|�P�������
	Dpw_Tr_PokemonSearchData Want;						///< �~�����|�P�������
	Dpw_Tr_PokemonSearchData Search;					///< �����|�P��������
	Dpw_Tr_PokemonSearchData SearchBackup;				///< ���������͂ł��Ȃ��悤�ɂ��邽�߂ɕۑ�
	int				SearchBackup_CountryCode;			///< �����̕ۑ�



	// �`��܂��̃��[�N�i���BMP�p�̕��������j
	WORDSET			*WordSet;								///< ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
	GFL_MSGDATA *MsgManager;							///< ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
	GFL_MSGDATA *MonsNameManager;						///< �|�P���������b�Z�[�W�f�[�^�}�l�[�W���[
	GFL_MSGDATA *LobbyMsgManager;						///< ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
	GFL_MSGDATA *SystemMsgManager;						///< Wifi�V�X�e�����b�Z�[�W�f�[�^
	GFL_MSGDATA *CountryNameManager;					///< �������b�Z�[�W�f�[�^�}�l�[�W���[
	STRBUF			*BoxTrayNameString;						///< �{�b�N�X�g���C��
	STRBUF			*EndString;								///< ������u��߂�v
	STRBUF			*TalkString;							///< ��b���b�Z�[�W�p
	STRBUF			*TitleString;							///< �^�C�g�����b�Z�[�W�p
	STRBUF			*InfoString[10];						///< �^�C�g�����b�Z�[�W�p
	STRBUF			*ErrorString;							///< �G���[���b�Z�[�W�\���p
	int				MsgIndex;								///< �I�����o�p���[�N


	// �`�����̃��[�N�i���OAM)
	GFL_CLUNIT*					clactSet;								///< �Z���A�N�^�[�Z�b�g

	u32 resObjTbl[RES_NUM][CLACT_RESOURCE_NUM]; ///< ���\�[�X�I�u�W�F�e�[�u��


	GFL_CLWK *			CursorActWork;							///< �Z���A�N�^�[���[�N�|�C���^�z��
	GFL_CLWK *			SubCursorActWork;						///< �Z���A�N�^�[���[�N�|�C���^�z��
	GFL_CLWK *			FingerActWork;							///< �Z���A�N�^�[���[�N�|�C���^�z��
	GFL_CLWK *			PokeIconActWork[BOX_POKE_NUM];			///< �|�P�����A�C�R���A�N�^�[
	GFL_CLWK *			ItemIconActWork[BOX_POKE_NUM];			///< �A�C�e���A�C�R���A�N�^�[
	GFL_CLWK *			CBallActWork[6];						///< �J�X�^���{�[���A�C�R���A�N�^�[
	GFL_CLWK *			PokemonActWork;							///< �Z���A�N�^�[���[�N�|�C���^�z��
	GFL_CLWK *			SubActWork[SUB_OBJ_NUM];				///< �T�u��ʗp�n�a�i�|�C���^
	GFL_CLWK *			BoxArrowActWork[2];						///< �{�b�N�X���̉��ɂ�����
	GFL_CLWK *			PartnerCursorActWork;					///< ����ʂŌ��������I�ԃJ�[�\��
	GFL_CLWK *			PromptDsActWork;						///< ����ʂ����Ȃ����A�C�R��
	int						DrawOffset;								///< ��ʑS�̂��Y�������߂̕ϐ�

	// BMPWIN�`�����
	GFL_BMPWIN*			MsgWin;									///< ��b�E�C���h�E
	GFL_BMPWIN*			NumberWin;								///< ��߂�
	GFL_BMPWIN*			TitleWin;								///< �u���R�[�h�R�[�i�[�@�ڂ��イ���イ�I�v�Ȃ�
	GFL_BMPWIN*			SubWin;									///< �u���R�[�h�R�[�i�[�@�ڂ��イ���イ�I�v�Ȃ�
	GFL_BMPWIN*			MenuWin[13];							///< ���j���[�pBMPWIN
	GFL_BMPWIN*			InfoWin[16];							///< �v���`�i��2�ǉ����ċ���ł�2�ǉ�
	GFL_BMPWIN*			TalkWin;								///< ��b�E�C���h�E
	GFL_BMPWIN*			BackWin;								///< �u���ǂ�v
	GFL_BMPWIN*			CountryWin[2];							///< ���E�n��\��
	GFL_BMPWIN*			ExplainWin;								///< �T�u��ʂŌ��݂̏󋵂��������
	WORLDTRADE_INPUT_WORK	*WInputWork;							///< ���O�E���ʁE���x���E���������̓V�X�e�����[�N


	BMP_MENULIST_DATA		*BmpMenuList;
	BMPMENU_WORK			*YesNoMenuWork;
	BMPMENU_WORK 			*BmpMenuWork;
	BMPMENULIST_WORK 			*BmpListWork;
	void*					timeWaitWork;							// ��b�E�C���h�E���A�C�R�����[�N

	int						wait;								///< �ėp�҂����[�N


	// ��ʖ��Ɏg�����Ƃ����郏�[�N
	DEPOSIT_WORK			*dw;								///< �a����p���[�N
	TOUCH_SW_SYS			*tss;								///< �͂��E�������E�C���h�E
	SELBOX_SYS				*SelBoxSys;							///< �^�b�`�L�[�I���{�b�N�X�V�X�e��
	SELBOX_WORK				*SelBoxWork;						///< �^�b�`�L�[�I���{�b�N�X���[�N

	// worldtrade_upload.c�p���[�N
	u16						saveNextSeq1st;						///< �Z�[�u�̑O���I�����ɔ�ԃV�[�P���X
	u16						saveNextSeq2nd;						///< �Z�[�u�̌㔼�I�����ɔ�ԃV�[�P���X

	// worldtrade_sublcd.c�p���[�N
	GFL_TCB*			demotask;							///< ��l���f���p�^�X�N�|�C���^
	u16						demo_end;							///< �f���I���t���O
	u16						SubLcdTouchOK;						///< �l��OBJ���łĂ��Ă���G���悤�ɂȂ�t���O
	void*					FieldObjCharaBuf;					///< �l��OBJ�L�����t�@�C���f�[�^
	NNSG2dCharacterData*	FieldObjCharaData;					///< �l��OBJ�L�����f�[�^�̎��|�C���^				
	void*					FieldObjPalBuf;						///< �l��OBJ�p���b�g�@�C���f�[�^
	NNSG2dPaletteData*		FieldObjPalData;					///< �l��OBJ�p���b�g�t�@�C���f�[�^

	// worldtrade_demo.c�p���[�N
	POKEMON_PARAM *demoPokePara;								///< �����f���p�̃|�P�����|�C���^


	// worldtrade_box.c�p���[�N
	BOX_RESEARCH			*boxWork;							///< �{�b�N�X���f�[�^�̑S�|�P�����̊�{���
	u16						boxPokeNum;							///< �{�b�N�X�ɂ���|�P�����̑���
	u16						boxSearchFlag;						///< �{�b�N�X�̑������擾���邽�߂̃t���O
	u32	sub_out_flg;		// ����ʃt�F�[�h�R���g���[��
	
	void *boxicon;												///< �|�P�����A�C�R�����������Ɏg���L�����|�C���^
	void (*vfunc)(void *);										///< V�u�����N�^�X�N(�|�P�����A�C�R�����������j
	void (*vfunc2)(void *);										///< V�u�����N�^�X�N�iBG�X�N���[���j

	// worldtrade_search.c�Ŏg�p
	s16						SubActY[10][2];						///< �e�l��OBJ�̏���Y�ʒu

	///<worldtrade_deposit.c worldtrade_search.c�Ō��p
	SELECT_LIST_POS			selectListPos;
	
	// �v���`�i����ǉ�
	Dpw_Common_Profile			dc_profile;						// �����̏��o�^�p�\����
	Dpw_Common_ProfileResult	dc_profile_result;				// �����̏��o�^���X�|���X�p�\����

	int							country_code;					///< ���������ɍ��ԍ�
	
	CONNECT_BG_PALANM cbp;		// Wifi�ڑ�BG��ʂ̃p���b�g�A�j������V�X�e��
	
	s16 local_seq;												///< �T�[�o�[�����G���[�p�̃V�[�P���X���[�N
	s16 local_wait;												///< �T�[�o�[�����G���[�p�̃E�F�C�g
	s32 timeout_count;											///< �T�[�o�[�����G���[�p�̌v���p

	GFL_TCB *	vblank_task;								///< VBlank�֐��̓^�X�N�ɂȂ���
	GFL_TCBSYS* tcbsys;										///< PMDS_task���Ȃ��Ȃ����̂Ŏ��O�ŗp��
	void * task_wk;												///< �^�X�N�p���[�N
	void * task_wk_area;									///< �^�X�N���[�N�쐬�p�G���A
	BOOL	is_net_init;										///< NET_Init���������ǂ���
	WT_PRINT	print;
	GFL_G3D_CAMERA    * camera;						///< MCSS�Ń|�P�����\�����邽�߂̐��ˉe�J����
	MCSS_SYS_WORK *mcssSys;								///< MCSS�{��
	MCSS_WORK     *pokeMcss;							///< ��C��

#ifdef PM_DEBUG
	int 					frame;									//
	int						framenum[9][2];							//
#endif
} WORLDTRADE_WORK;


//============================================================================================
// SE�p��`
//============================================================================================
#define WORLDTRADE_MOVE_SE		(SEQ_SE_DP_SELECT)
#define WORLDTRADE_DECIDE_SE	(SEQ_SE_DP_SELECT)
//#define WORLDTRADE_PAGE_SE		(SEQ_SE_DP_SELECT78)
#define WORLDTRADE_PAGE_SE		(SEQ_SE_DP_SELECT)
//TODO 

//============================================================================================
//	�f�o�b�O�p
//============================================================================================
//#define	GTS_FADE_OSP	// �t�F�[�h�ꏊ�\���p��`


//============================================================================================
// extern�錾
//============================================================================================
// worldtrade.c
extern void WorldTrade_MakeCLACT(CLACT_ADD *add, WORLDTRADE_WORK *wk, CLACT_HEADER *header, int param);
extern BMPMENU_WORK *WorldTrade_BmpWinYesNoMake( int y, int yesno_bmp_cgx );
extern TOUCH_SW_SYS *WorldTrade_TouchWinYesNoMake( int y, int yesno_bmp_cgx, int pltt, u8 inPassive );
extern TOUCH_SW_SYS *WorldTrade_TouchWinYesNoMakeEx( int y, int yesno_bmp_cgx, int pltt, int frame, u8 inPassive );

extern u32 WorldTrade_TouchSwMain(WORLDTRADE_WORK *wk);

extern void WorldTrade_SetNextSeq( WORLDTRADE_WORK *wk, int to_seq, int next_seq );
extern void WorldTrade_SetNextProcess( WORLDTRADE_WORK *wk, int next_process );
extern  int WorldTrade_WifiLinkLevel( void );
extern void WorldTrade_BmpWinPrint( GFL_BMPWIN *win, GFL_MSGDATA *msgman, int font, int msgno, u16 dat, WT_PRINT *print );
extern void WorldTrade_SysPrint( GFL_BMPWIN *win, STRBUF *strbuf, int x, int y, int flag, PRINTSYS_LSB color, WT_PRINT *print );
extern void WorldTrade_TalkPrint( GFL_BMPWIN *win, STRBUF *strbuf, int x, int y, int flag, PRINTSYS_LSB color, WT_PRINT *print );
extern void WorldTrade_TouchPrint( GFL_BMPWIN *win, STRBUF *strbuf, int x, int y, int flag, PRINTSYS_LSB color , WT_PRINT *print );

extern void WorldTrade_WifiIconAdd( WORLDTRADE_WORK *wk );
extern void Enter_MessagePrint( WORLDTRADE_WORK *wk, GFL_MSGDATA *msgman, int msgno, int wait, u16 dat );
extern void WorldTrade_SubProcessChange( WORLDTRADE_WORK *wk, int subprccess, int mode );
extern WINTYPE WorldTrade_GetMesWinType( WORLDTRADE_WORK *wk );
extern int WorldTrade_GetTalkSpeed( WORLDTRADE_WORK *wk );
extern void FreeFieldObjData( WORLDTRADE_WORK *wk );
extern void WorldTrade_BoxPokeNumGetStart( WORLDTRADE_WORK *wk );
extern void WorldTrade_SubProcessUpdate( WORLDTRADE_WORK *wk );
extern void WorldTrade_TimeIconAdd( WORLDTRADE_WORK *wk );
extern void WorldTrade_TimeIconDel( WORLDTRADE_WORK *wk );
extern void WorldTrade_CLACT_PosChange( GFL_CLWK * ptr, int x, int y );
extern void WorldTrade_CLACT_PosChangeSub( GFL_CLWK * act, int x, int y );
extern SELBOX_WORK* WorldTrade_SelBoxInit( WORLDTRADE_WORK *wk, u8 frm, int count, int y );
extern void WorldTrade_SelBoxEnd( WORLDTRADE_WORK *wk );

extern void WorldTrade_SetPassive(u8 inTarget);
extern void WorldTrade_SetPassiveMyPoke(u8 inIsMain);
extern void WorldTrade_ClearPassive(void);



// worldtrade_sublcd.c
extern void WorldTrade_HeroDemo( WORLDTRADE_WORK *wk, int sex );
extern int  WorldTrade_SubLcdObjHitCheck( int max );
extern void WorldTrade_SubLcdMatchObjAppear( WORLDTRADE_WORK *wk, int num, int flag );
extern void WorldTrade_SubLcdMatchObjHide( WORLDTRADE_WORK *wk );
extern void WorldTrade_SubLcdActorAdd( WORLDTRADE_WORK *wk, int sex );
extern void WorldTrade_ReturnHeroDemo( WORLDTRADE_WORK *wk, int sex );
extern void WorldTrade_SetPartnerCursorPos( WORLDTRADE_WORK *wk, int index, int offset_y );
extern void WorldTrade_SetPartnerExchangePos( WORLDTRADE_WORK *wk );
extern void WorldTrade_SetPartnerExchangePosIsReturns( WORLDTRADE_WORK *wk );


// worldtrade_enter.c
extern int  WorldTrade_Enter_End( WORLDTRADE_WORK *wk, int seq);
extern int  WorldTrade_Enter_Main(WORLDTRADE_WORK *wk, int seq);
extern int  WorldTrade_Enter_Init(WORLDTRADE_WORK *wk, int seq);

extern void WorldTrade_ExplainPrint( GFL_BMPWIN *win,  GFL_MSGDATA *msgman, int no, WT_PRINT *print );

// worldtrade_title.c
extern int  WorldTrade_Title_End( WORLDTRADE_WORK *wk, int seq);
extern int  WorldTrade_Title_Main(WORLDTRADE_WORK *wk, int seq);
extern int  WorldTrade_Title_Init(WORLDTRADE_WORK *wk, int seq);
extern void WorldTrade_SubLcdBgGraphicSet( WORLDTRADE_WORK *wk );
extern void WorldTrade_SubLcdBgInit( int sub_bg1_y_offset, BOOL sub_bg2_no_clear );
extern void WorldTrade_SubLcdBgExit( void );

extern void WorldTrade_SubLcdWinGraphicSet( WORLDTRADE_WORK *wk );
extern void WorldTrade_SubLcdExpainPut( WORLDTRADE_WORK *wk, int explain );



// worldtrade_mypoke.c
extern int  WorldTrade_MyPoke_Init(WORLDTRADE_WORK *wk, int seq);
extern int  WorldTrade_MyPoke_Main(WORLDTRADE_WORK *wk, int seq);
extern int  WorldTrade_MyPoke_End(WORLDTRADE_WORK *wk, int seq);
extern void WorldTrade_PokeInfoPrint( 	GFL_MSGDATA *MsgManager,
							GFL_MSGDATA *MonsNameManager, 	
							WORDSET *WordSet, 
							GFL_BMPWIN *win[], 	
							POKEMON_PASO_PARAM *ppp,
							Dpw_Tr_PokemonDataSimple *post,
							WT_PRINT *print );
extern void WorldTrade_PokeInfoPrint2( GFL_MSGDATA *MsgManager, GFL_BMPWIN *win[], STRCODE *name, POKEMON_PARAM *pp, GFL_BMPWIN* oya_win[], WT_PRINT *print );
extern void WorldTrade_TransPokeGraphic( POKEMON_PARAM *pp );
//�ȉ��A�|�P�����\����MCSS�ɕύX���邽�߂ɍ쐬nagihashi
extern void WorldTrade_MyPoke_G3D_Init( WORLDTRADE_WORK *wk );
extern void WorldTrade_MyPoke_G3D_Exit( WORLDTRADE_WORK *wk );
extern void WorldTrade_MyPoke_G3D_Draw( WORLDTRADE_WORK *wk );
extern MCSS_WORK * WorldTrade_MyPoke_MCSS_Create( WORLDTRADE_WORK *wk, POKEMON_PASO_PARAM *ppp, const VecFx32 *pos );
extern void WorldTrade_MyPoke_MCSS_Delete( WORLDTRADE_WORK *wk, MCSS_WORK *poke );
//�����܂�

// worldtrade_partner.c
extern int WorldTrade_Partner_Init(WORLDTRADE_WORK *wk, int seq);
extern int WorldTrade_Partner_Main(WORLDTRADE_WORK *wk, int seq);
extern int WorldTrade_Partner_End(WORLDTRADE_WORK *wk, int seq);

// worldtrade_search.c
extern  int WorldTrade_Search_Init(WORLDTRADE_WORK *wk, int seq);
extern  int WorldTrade_Search_Main(WORLDTRADE_WORK *wk, int seq);
extern  int WorldTrade_Search_End(WORLDTRADE_WORK *wk, int seq);

// worldtrade_box.c
extern int WorldTrade_Box_Init(WORLDTRADE_WORK *wk, int seq);
extern int WorldTrade_Box_Main(WORLDTRADE_WORK *wk, int seq);
extern int WorldTrade_Box_End(WORLDTRADE_WORK *wk, int seq);
extern POKEMON_PASO_PARAM *WorldTrade_GetPokePtr( POKEPARTY *party, BOX_DATA *box,  int  tray, int pos );
extern int WorldTrade_GetPPorPPP( int tray );
extern BOOL WorldTrade_PokemonMailCheck( POKEMON_PARAM *pp );

// workdtrade_deposit.c
extern int WorldTrade_Deposit_Init(WORLDTRADE_WORK *wk, int seq);
extern int WorldTrade_Deposit_Main(WORLDTRADE_WORK *wk, int seq);
extern int WorldTrade_Deposit_End(WORLDTRADE_WORK *wk, int seq);
extern void WodrldTrade_PokeWantPrint( GFL_MSGDATA *MsgManager, GFL_MSGDATA *MonsNameManager,
	WORDSET *WordSet, GFL_BMPWIN *win[], int monsno, int sex, int level, WT_PRINT *print );
extern void WodrldTrade_MyPokeWantPrint( GFL_MSGDATA *MsgManager, GFL_MSGDATA *MonsNameManager,
	WORDSET *WordSet, GFL_BMPWIN *win[], int monsno, int sex, int level, WT_PRINT *print );

extern BMPMENULIST_WORK *WorldTrade_WordheadBmpListMake( WORLDTRADE_WORK *wk, BMP_MENULIST_DATA **menulist, 
													GFL_BMPWIN *win, GFL_MSGDATA *MsgManager );
extern BMPMENULIST_WORK *WorldTrade_PokeNameListMake( WORLDTRADE_WORK *wk, BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, GFL_MSGDATA *MsgManager, GFL_MSGDATA *MonsNameManager, DEPOSIT_WORK* dw, ZUKAN_WORK *zukan);
extern BMPMENULIST_WORK *WorldTrade_SexSelectListMake( BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, GFL_MSGDATA *MsgManager  );
extern BMPMENULIST_WORK *WorldTrade_LevelListMake(BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, GFL_MSGDATA *MsgManager, int tbl_select  );
extern const int WorldTrade_WantLevelTable[];
extern const int WorldTrade_SexStringTable[];
extern u16* WorldTrade_ZukanSortDataGet( int heap, int idx, int* p_arry_num );
extern u16* WorldTrade_ZukanSortDataGet2( int heap, int idx, int* p_arry_num );

extern BMPMENULIST_WORK *WorldTrade_CountryListMake(BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, GFL_MSGDATA *CountryMsgManager, GFL_MSGDATA *NormalMsgManager);

extern void WorldTrade_PokeNamePrint( GFL_BMPWIN *win, GFL_MSGDATA *nameman, int monsno, int flag, int y, PRINTSYS_LSB color, WT_PRINT *print );
extern void WorldTrade_SexPrint( GFL_BMPWIN *win, GFL_MSGDATA *msgman, int sex, int flag, int y, int printflag, PRINTSYS_LSB color, WT_PRINT *print );
extern void WorldTrade_WantLevelPrint( GFL_BMPWIN *win, GFL_MSGDATA *msgman, int level, int flag, int y, PRINTSYS_LSB color, int tbl_select, WT_PRINT *print );
extern void WorldTrade_WantLevelPrint_XY(GFL_BMPWIN *win, GFL_MSGDATA *msgman, int level, int flag, int x, int y, PRINTSYS_LSB color, int tbl_select, WT_PRINT *print );
extern void WorldTrade_CountryPrint( GFL_BMPWIN *win, GFL_MSGDATA *nameman, GFL_MSGDATA *msgman, int country_code, int flag, int y, PRINTSYS_LSB color, WT_PRINT *print );
extern void WorldTrade_PostPokemonBaseDataMake( Dpw_Tr_Data *dtd, WORLDTRADE_WORK *wk );
extern int  WorldTrade_SexSelectionCheck( Dpw_Tr_PokemonSearchData *dtp, int sex_selection );
extern int  WorldTrade_LevelTermGet( int min, int max, int tbl_select );
extern void WorldTrade_LevelMinMaxSet( Dpw_Tr_PokemonSearchData *dtps, int index, int tbl_select );
extern void WorldTrade_CountryCodeSet( WORLDTRADE_WORK *wk, int country_code );
extern u8  *WorldTrade_SinouZukanDataGet( int heap  );
extern u32  WorldTrade_BmpMenuList_Main( BMPMENULIST_WORK * lw, u16 *posbackup );
extern void WorldTrade_SelectListPosInit( SELECT_LIST_POS *slp);
extern void WorldTrade_SelectNameListBackup( SELECT_LIST_POS *slp, int head, int list, int pos );
extern int  WorldTrade_LevelListAdd( BMP_MENULIST_DATA **menulist, GFL_MSGDATA *MsgManager, int tbl_select);
extern int WorldTrade_NationSortListNumGet( int start, int *number );
extern int WorldTrade_NationSortListMake( BMP_MENULIST_DATA **menulist, GFL_MSGDATA *CountryNameManager, int start );
extern void WorldTrade_PokeNamePrintNoPut( GFL_BMPWIN *win, GFL_MSGDATA *nameman, int monsno, int y, PRINTSYS_LSB color, WT_PRINT *print );
extern void WorldTrade_SexPrintNoPut( GFL_BMPWIN *win, GFL_MSGDATA *msgman, int sex, int flag, int x, int y, PRINTSYS_LSB color, WT_PRINT *print );

// workdtrade_uploade.c
extern int WorldTrade_Upload_Init(WORLDTRADE_WORK *wk, int seq);
extern int WorldTrade_Upload_Main(WORLDTRADE_WORK *wk, int seq);
extern int WorldTrade_Upload_End(WORLDTRADE_WORK *wk, int seq);

// workdtrade_status.c
extern int WorldTrade_Status_Init(WORLDTRADE_WORK *wk, int seq);
extern int WorldTrade_Status_Main(WORLDTRADE_WORK *wk, int seq);
extern int WorldTrade_Status_End(WORLDTRADE_WORK *wk, int seq);

// workdtrade_demo.c
extern int WorldTrade_Demo_Init(WORLDTRADE_WORK *wk, int seq);
extern int WorldTrade_Demo_Main(WORLDTRADE_WORK *wk, int seq);
extern int WorldTrade_Demo_End(WORLDTRADE_WORK *wk, int seq);


//==============================================================================
//	�O���f�[�^�錾
//==============================================================================
extern const u32 CountryListTblNum;

#endif
