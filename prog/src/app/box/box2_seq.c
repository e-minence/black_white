//============================================================================================
/**
 * @file		box2_seq.c
 * @brief		�{�b�N�X��� ���C���V�[�P���X����
 * @author	Hiroyuki Nakamura
 * @date		09.10.05
 *
 *	���W���[�����FBOX2SEQ
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/main.h"
#include "system/wipe.h"
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"
#include "item/item.h"
#include "poke_tool/monsno_def.h"

#include "msg/msg_boxmenu.h"

#include "box2_main.h"
#include "box2_bmp.h"
#include "box2_obj.h"
#include "box2_seq.h"
#include "box2_ui.h"
#include "box2_bgwfrm.h"
#include "box2_snd_def.h"


//============================================================================================
//	�萔��`
//============================================================================================

// �T�u�v���Z�X�^�C�v
enum {
	SUB_PROC_TYPE_POKESTATUS = 0,		// �X�e�[�^�X���
	SUB_PROC_TYPE_BAG,							// �o�b�O���
	SUB_PROC_TYPE_NAMEIN,						// ��������
};

// �T�u�v���Z�X�Ăяo�����[�h
enum {
	SUB_PROC_MODE_MENU_BOX = 0,		// �{�b�N�X�̃|�P�����̃��j���[����
	SUB_PROC_MODE_MENU_PARTY,			// �莝���̃|�P�����̃��j���[����

	SUB_PROC_MODE_TB_BOX,					// �{�b�N�X�̃|�P�����̃^�b�`�o�[����
	SUB_PROC_MODE_TB_BOX_GET,			// �{�b�N�X�̃|�P�����̃^�b�`�o�[����i�͂�ł��鎞�j

	SUB_PROC_MODE_TB_PARTY,				// �莝���̃|�P�����̃^�b�`�o�[����
	SUB_PROC_MODE_TB_PARTY_GET,		// �莝���̃|�P�����̃^�b�`�o�[����i�͂�ł��鎞�j
};

// �T�u�v���Z�X�ݒ�֐�
typedef struct {
	pBOX2_FUNC	call;		// �Ăяo��
	pBOX2_FUNC	exit;		// �I��
	u32	rcvSeq;					// ���A�V�[�P���X
}SUB_PROC_DATA;

// �͂��E����������
typedef struct {
	pBOX2_FUNC	yes;
	pBOX2_FUNC	no;
}YESNO_DATA;

// �͂��E������ID
enum {
	YESNO_ID_ITEMGET = 0,			// ����a����
	YESNO_ID_POKEFREE,				// ������
	YESNO_ID_ITEM_RET_BAG,		// �A�C�e�����܂�
	YESNO_ID_BOX_END,					// �{�b�N�X�I���i�{�^���j
	YESNO_ID_BOX_END_CANCEL,	// �{�b�N�X�I���i�L�����Z���j
	YESNO_ID_SLEEP,						// �Q������
};

// �g���C�A�C�R���X�N���[�����[�N
typedef struct {
	s16	cnt;
	s16	mv;
}SEQWK_TRAYICON_SCROLL;

//�u�|�P������a����v�z�u�V�[�P���X
enum {
	SEQ_PTOUT_PUT_POKEMOVE = 0,		// �A�C�R���ړ��ݒ�
	SEQ_PTOUT_PUT_PTFRM_RET,			// �莝���t���[���C���i�L�����Z�����j
	SEQ_PTOUT_PUT_ICON_RET,				// �A�C�R���z�u�i�L�����Z�����j

	SEQ_PTOUT_PUT_PTFRM_IN,				// �莝���t���[���C��
	SEQ_PTOUT_PUT_ICON_PUT,				// �A�C�R���z�u
	SEQ_PTOUT_PUT_DATA_CHG,				// �f�[�^����ւ�

	SEQ_PTOUT_PUT_CANCEL_END,			// �I���i�L�����Z�����j
	SEQ_PTOUT_PUT_END,						// �I��
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static int MainSeq_Init( BOX2_SYS_WORK * syswk );
static int MainSeq_Release( BOX2_SYS_WORK * syswk );
static int MainSeq_Wipe( BOX2_SYS_WORK * syswk );
static int MainSeq_PaletteFade( BOX2_SYS_WORK * syswk );
static int MainSeq_Wait( BOX2_SYS_WORK * syswk );
static int MainSeq_VFunc( BOX2_SYS_WORK * syswk );
static int MainSeq_TrgWait( BOX2_SYS_WORK * syswk );
static int MainSeq_YesNo( BOX2_SYS_WORK * syswk );
static int MainSeq_ButtonAnm( BOX2_SYS_WORK * syswk );
static int MainSeq_SubProcCall( BOX2_SYS_WORK * syswk );
static int MainSeq_SubProcMain( BOX2_SYS_WORK * syswk );
static int MainSeq_Start( BOX2_SYS_WORK * syswk );
static int MainSeq_CursorMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeMenuRcv( BOX2_SYS_WORK * syswk );
static int MainSeq_TrayScrollTouch( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxEndDirect( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeTrayPokeChgEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeMenuClose( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeGetInit( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeGetMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeGetExit( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeGetDataChange( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeGetDataChangeErr( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeGetPartyChange( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyInit( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyMenuClose( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyPokeChgEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyPokeGetInit( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyPokeGetMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyPokeGetExit( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyPokeGetBoxListChange( BOX2_SYS_WORK * syswk );
static int MainSeq_BattleBoxMain( BOX2_SYS_WORK * syswk );
static int MainSeq_BattleBoxTrayPokeChgEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_BattleBoxMenuClose( BOX2_SYS_WORK * syswk );
static int MainSeq_BattleBoxPartyInit( BOX2_SYS_WORK * syswk );
static int MainSeq_BattleBoxPartyMain( BOX2_SYS_WORK * syswk );
static int MainSeq_BattleBoxPartyEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_BattleBoxPartyMenuClose( BOX2_SYS_WORK * syswk );
static int MainSeq_BattleBoxPartyPokeChgEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_BattleBoxRock( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInMain( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInMenuClose( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInSelect( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInAction( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInPokeChgEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutMain( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutMenuClose( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutErrRcvMain( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutPutInit( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutPutMain( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutPutEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutErrRcvPut( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutPokeChgEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemMenuClose( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemBagInAnm( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemPokeSetAnm( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemPokeSetRet( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeIconMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemGetInit( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemGetMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemGetEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemBoxPartyIconMovePokeAdd( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemBoxPartyIconMoveEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemBoxPartyChgError( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemKeyGetChangeCkack( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemPartyMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemPartyMenuClose( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemPartyIconMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemPartyIconMovePokeAdd( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemPartyIconMoveChange( BOX2_SYS_WORK * syswk );
static int MainSeq_SleepMain( BOX2_SYS_WORK * syswk );
static int MainSeq_SleepPokeSelect( BOX2_SYS_WORK * syswk );
static int MainSeq_SleepMenuSet( BOX2_SYS_WORK * syswk );
static int MainSeq_SleepMenuClose( BOX2_SYS_WORK * syswk );
static int MainSeq_StatusCall( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemMenuCheck( BOX2_SYS_WORK * syswk );
static int MainSeq_StatusRcv( BOX2_SYS_WORK * syswk );
static int MainSeq_StrInRcv( BOX2_SYS_WORK * syswk );
static int MainSeq_BagRcv( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemSetMsg( BOX2_SYS_WORK * syswk );
static int MainSeq_MarkingInit( BOX2_SYS_WORK * syswk );
static int MainSeq_MarkingMain( BOX2_SYS_WORK * syswk );
static int MainSeq_MarkingEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_MarkingCurMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_PokeFreeInit( BOX2_SYS_WORK * syswk );
static int MainSeq_PokeFreeAnm( BOX2_SYS_WORK * syswk );
static int MainSeq_PokeFreeEnter( BOX2_SYS_WORK * syswk );
static int MainSeq_PokeFreeError( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaInit( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaMain( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaExit( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaJumpInit( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaJumpMain( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaJumpExit( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaJumpRet( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaWallPaperInit( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaWallPaperMain( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaWallPaperChange( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaNameInCall( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxEndInit( BOX2_SYS_WORK * syswk );

static int ChangeSequence( BOX2_SYS_WORK * syswk, int next );
static int FadeInSet( BOX2_SYS_WORK * syswk, int next );
static int FadeOutSet( BOX2_SYS_WORK * syswk, int next );
static int WaitSet( BOX2_SYS_WORK * syswk, int next, int wait );
static int VFuncSet( BOX2_SYS_WORK * syswk, void * func, int next );
static int VFuncReqSet( BOX2_SYS_WORK * syswk, int next );
static int SetCursorMoveRet( BOX2_SYS_WORK * syswk, int next );

static int YesNoSet( BOX2_SYS_WORK * syswk, u32 id );
static int BoxArg_ItemGetYes( BOX2_SYS_WORK * syswk );
static int BoxArg_PokeFreeYes( BOX2_SYS_WORK * syswk );
static int BoxArg_PokeFreeNo( BOX2_SYS_WORK * syswk );
static int BoxItemArg_RetBagYes( BOX2_SYS_WORK * syswk );
static int BoxArg_BoxEndYes( BOX2_SYS_WORK * syswk );
static int BoxArg_BoxEndNo( BOX2_SYS_WORK * syswk );
static int BoxArg_SleepYes( BOX2_SYS_WORK * syswk );
static int BoxArg_SleepNo( BOX2_SYS_WORK * syswk );
static int YesNo_MenuRcv( BOX2_SYS_WORK * syswk );

static int BgButtonAnmSet( BOX2_SYS_WORK * syswk, u32 wfrmID, int next );
static int MarkingButtonAnmSet( BOX2_SYS_WORK * syswk, u32 no, int next );
static int ObjButtonAnmSet( BOX2_SYS_WORK * syswk, u32 objID, u32 anm, int next );

static int SubProcSet( BOX2_SYS_WORK * syswk, u8 type );
static int RcvBag_ModeSeiri( BOX2_SYS_WORK * syswk );
static int RcvBag_ModeItem( BOX2_SYS_WORK * syswk );
static int RcvBag_ModeBattleBox( BOX2_SYS_WORK * syswk );
static int RcvStatus_ModeTureteiku( BOX2_SYS_WORK * syswk );
static int RcvStatus_ModeAzukeru( BOX2_SYS_WORK * syswk );
static int RcvStatus_ModeSeiri( BOX2_SYS_WORK * syswk );
static int RcvStatus_ModeBattleBox( BOX2_SYS_WORK * syswk );

static int TrayLeftScrollSet( BOX2_SYS_WORK * syswk, BOOL btnFlg, int next );
static int TrayRightScrollSet( BOX2_SYS_WORK * syswk, BOOL btnFlg, int next );
static int TrayPokeGet( BOX2_SYS_WORK * syswk, u32 pos );

static int GetBattleBoxTrayPoke( BOX2_SYS_WORK * syswk, u32 pos );
static int TrayPokeGetPartyIn( BOX2_SYS_WORK * syswk, u32 pos );
static int TrayPokeGetKey( BOX2_SYS_WORK * syswk, u32 pos, int ret );
static int PartyPokeGet( BOX2_SYS_WORK * syswk, u32 pos );
static int GetBattleBoxPartyPoke( BOX2_SYS_WORK * syswk, u32 pos );
static int PartyPokeGetDrop( BOX2_SYS_WORK * syswk, u32 pos );
static int PartyPokeGetKey( BOX2_SYS_WORK * syswk, u32 pos, int ret );
static int BoxPartyPokeGet( BOX2_SYS_WORK * syswk, u32 pos, int next );
static int PartyPokeMoveGetKey( BOX2_SYS_WORK * syswk, u32 pos );
static int PokeMoveGetKey( BOX2_SYS_WORK * syswk, u32 pos );
static int PokeMovePutKey( BOX2_SYS_WORK * syswk, u32 pos, int next );
static void PokeMoveWorkAlloc( BOX2_SYS_WORK * syswk );
static void PokeMoveWorkFree( BOX2_SYS_WORK * syswk );

static int TrayItemGet( BOX2_SYS_WORK * syswk, u32 pos );
static int TrayItemGetKey( BOX2_SYS_WORK * syswk, u32 pos );
static int BoxPartyItemGet( BOX2_SYS_WORK * syswk, u32 pos );
static int BoxPartyItemGetKey( BOX2_SYS_WORK * syswk, u32 pos );
static int BoxPartyItemPutKeyCancel( BOX2_SYS_WORK * syswk );
static int BoxPartyItemPutKey( BOX2_SYS_WORK * syswk, u32 pos );
static int PartyItemGet( BOX2_SYS_WORK * syswk, u32 pos );
static int PartyItemGetKey( BOX2_SYS_WORK * syswk, u32 pos );
static void ItemMoveWorkAlloc( BOX2_SYS_WORK * syswk );
static void ItemMoveWorkFree( BOX2_SYS_WORK * syswk );

static void PartyFrmSet( BOX2_SYS_WORK * syswk );
static void PartyFrmSetRight( BOX2_SYS_WORK * syswk );
static void PartyFrmSet_PartyOut( BOX2_SYS_WORK * syswk );
static int BoxMoveTrayScrollSet( BOX2_SYS_WORK * syswk, u32 mode );
static void CreateTrayIconScrollWork( BOX2_SYS_WORK * syswk );
static int BoxMoveFrmInSet( BOX2_SYS_WORK * syswk, int next );
static int BoxMoveFrmOutSet( BOX2_SYS_WORK * syswk, int next );
static int PokeMenuOutSet( BOX2_SYS_WORK * syswk, int next );

static int PartyOutCheck( BOX2_SYS_WORK * syswk );
static void MarkingSwitch( BOX2_SYS_WORK * syswk, u32 pos );
static int SetWallPaperChange( BOX2_SYS_WORK * syswk, u32 pos );
static void BoxEndPassiveSet( BOX2_SYS_WORK * syswk );

static int ChgSeqSelectButton( BOX2_SYS_WORK * syswk, int next );
static int ChgSeqSelectTouch( BOX2_SYS_WORK * syswk, int next );


//============================================================================================
//	�O���[�o���ϐ�
//============================================================================================

// ���C���V�[�P���X�e�[�u��
static const pBOX2_FUNC MainSeq[] = {
	MainSeq_Init,
	MainSeq_Release,
	MainSeq_Wipe,
	MainSeq_PaletteFade,
	MainSeq_Wait,
	MainSeq_VFunc,

	MainSeq_TrgWait,

	MainSeq_YesNo,
	MainSeq_ButtonAnm,

	MainSeq_SubProcCall,
	MainSeq_SubProcMain,

	MainSeq_Start,
	MainSeq_CursorMoveRet,
	MainSeq_ArrangePokeMenuRcv,
	MainSeq_TrayScrollTouch,
	MainSeq_BoxEndDirect,

	// �{�b�N�X����
	MainSeq_ArrangeMain,
	MainSeq_ArrangeTrayPokeChgEnd,
	MainSeq_ArrangeMenuClose,

	MainSeq_ArrangePokeGetInit,
	MainSeq_ArrangePokeGetMain,
	MainSeq_ArrangePokeGetExit,
	MainSeq_ArrangePokeGetDataChange,
	MainSeq_ArrangePokeGetDataChangeErr,
	MainSeq_ArrangePokeGetPartyChange,

	MainSeq_ArrangePartyInit,
	MainSeq_ArrangePartyMain,
	MainSeq_ArrangePartyEnd,
	MainSeq_ArrangePartyMenuClose,
	MainSeq_ArrangePartyPokeChgEnd,

	MainSeq_ArrangePartyPokeGetInit,
	MainSeq_ArrangePartyPokeGetMain,
	MainSeq_ArrangePartyPokeGetExit,
	MainSeq_ArrangePartyPokeGetBoxListChange,

	// �o�g���{�b�N�X
	MainSeq_BattleBoxMain,
	MainSeq_BattleBoxTrayPokeChgEnd,
	MainSeq_BattleBoxMenuClose,

	MainSeq_BattleBoxPartyInit,
	MainSeq_BattleBoxPartyMain,
	MainSeq_BattleBoxPartyEnd,
	MainSeq_BattleBoxPartyMenuClose,
	MainSeq_BattleBoxPartyPokeChgEnd,

	MainSeq_BattleBoxRock,

	// ��Ă���
	MainSeq_PartyInMain,
	MainSeq_PartyInMenuClose,
	MainSeq_PartyInSelect,
	MainSeq_PartyInAction,
	MainSeq_PartyInPokeChgEnd,

	// ��������
	MainSeq_PartyOutMain,
	MainSeq_PartyOutMenuClose,
	MainSeq_PartyOutErrRcvMain,

	MainSeq_PartyOutPutInit,
	MainSeq_PartyOutPutMain,
	MainSeq_PartyOutPutEnd,
	MainSeq_PartyOutErrRcvPut,
	MainSeq_PartyOutPokeChgEnd,

	// ����������
	MainSeq_ItemMain,
	MainSeq_ItemMenuClose,
	MainSeq_ItemBagInAnm,
	MainSeq_ItemPokeSetAnm,
	MainSeq_ItemPokeSetRet,
	MainSeq_ItemArrangeIconMoveRet,
	MainSeq_ItemGetInit,
	MainSeq_ItemGetMain,
	MainSeq_ItemGetEnd,
	MainSeq_ItemBoxPartyIconMovePokeAdd,
	MainSeq_ItemBoxPartyIconMoveEnd,
	MainSeq_ItemBoxPartyChgError,
	MainSeq_ItemKeyGetChangeCkack,
	MainSeq_ItemPartyMain,
	MainSeq_ItemPartyMenuClose,
	MainSeq_ItemPartyIconMoveRet,
	MainSeq_ItemPartyIconMovePokeAdd,
	MainSeq_ItemPartyIconMoveChange,

	// �Q������
	MainSeq_SleepMain,
	MainSeq_SleepPokeSelect,
	MainSeq_SleepMenuSet,
	MainSeq_SleepMenuClose,

	// �T�u�v���Z�X
	MainSeq_StatusCall,
	MainSeq_ItemMenuCheck,

	MainSeq_StatusRcv,
	MainSeq_StrInRcv,
	MainSeq_BagRcv,
	MainSeq_ItemSetMsg,

	// �}�[�L���O
	MainSeq_MarkingInit,
	MainSeq_MarkingMain,
	MainSeq_MarkingEnd,
	MainSeq_MarkingCurMoveRet,
	// ������
	MainSeq_PokeFreeInit,
	MainSeq_PokeFreeAnm,
	MainSeq_PokeFreeEnter,
	MainSeq_PokeFreeError,

	// �{�b�N�X�e�[�}�ύX
	MainSeq_BoxThemaInit,
	MainSeq_BoxThemaMain,
	MainSeq_BoxThemaExit,
	MainSeq_BoxThemaJumpInit,
	MainSeq_BoxThemaJumpMain,
	MainSeq_BoxThemaJumpExit,
	MainSeq_BoxThemaJumpRet,
	MainSeq_BoxThemaWallPaperInit,
	MainSeq_BoxThemaWallPaperMain,
	MainSeq_BoxThemaWallPaperChange,
	MainSeq_BoxThemaNameInCall,

	// �I������
	MainSeq_BoxEndInit,
};

// �T�u�v���Z�X�ݒ�
static const SUB_PROC_DATA SubProcFunc[] =
{
	// �X�e�[�^�X���
	{ BOX2MAIN_PokeStatusCall, BOX2MAIN_PokeStatusExit, BOX2SEQ_MAINSEQ_STATUS_RCV },
	// �o�b�O���
	{ BOX2MAIN_BagCall, BOX2MAIN_BagExit, BOX2SEQ_MAINSEQ_BAG_RCV },
	// ��������
	{ BOX2MAIN_NameInCall, BOX2MAIN_NameInExit, BOX2SEQ_MAINSEQ_STRIN_RCV },
};

// �͂��E�������f�[�^�e�[�u��
static const YESNO_DATA YesNoFunc[] =
{
	// �{�b�N�X�����E�����a����H
	{ BoxArg_ItemGetYes, YesNo_MenuRcv },
	// �{�b�N�X�����E�|�P�����𓦂����H
	{ BoxArg_PokeFreeYes, BoxArg_PokeFreeNo },
	// ������E�A�C�e�����܂��H
	{ BoxItemArg_RetBagYes, YesNo_MenuRcv },
	// �{�b�N�X�I������H�i�{�^���j
	{ BoxArg_BoxEndYes, BoxArg_BoxEndNo },
	// �{�b�N�X�I������H�i�L�����Z���j
	{ BoxArg_BoxEndNo, BoxArg_BoxEndYes },
	// �Q������H
	{ BoxArg_SleepYes, BoxArg_SleepNo },
};

// ���j���[������e�[�u���F�{�b�N�X�������C��
static const BOX2BMP_BUTTON_LIST PokeMenuStrTbl[] =
{
	{ mes_boxbutton_02_08, BOX2BMP_BUTTON_TYPE_WHITE },		// ����
	{ mes_boxbutton_02_01, BOX2BMP_BUTTON_TYPE_WHITE },		// �悤�����݂�
	{ mes_boxbutton_02_02, BOX2BMP_BUTTON_TYPE_WHITE },		// ��������
	{ mes_boxbutton_02_03, BOX2BMP_BUTTON_TYPE_WHITE },		// �}�[�L���O
	{ mes_boxbutton_02_04, BOX2BMP_BUTTON_TYPE_WHITE },		// �ɂ���
	{ mes_boxbutton_02_07, BOX2BMP_BUTTON_TYPE_CANCEL }		// ��߂�
};

// ���j���[������e�[�u���F��Ă���
static const BOX2BMP_BUTTON_LIST PartyInMenuStrTbl[] =
{
	{ mes_boxbutton_02_05, BOX2BMP_BUTTON_TYPE_WHITE },		// ��Ă���
	{ mes_boxbutton_02_01, BOX2BMP_BUTTON_TYPE_WHITE },		// �悤�����݂�
	{ mes_boxbutton_02_03, BOX2BMP_BUTTON_TYPE_WHITE },		// �}�[�L���O
	{ mes_boxbutton_02_04, BOX2BMP_BUTTON_TYPE_WHITE },		// �ɂ���
	{ mes_boxbutton_02_07, BOX2BMP_BUTTON_TYPE_CANCEL }		// ��߂�
};

// ���j���[������e�[�u���F��������
static const BOX2BMP_BUTTON_LIST PartyOutMenuStrTbl[] =
{
	{ mes_boxbutton_02_06, BOX2BMP_BUTTON_TYPE_WHITE },		// ��������
	{ mes_boxbutton_02_01, BOX2BMP_BUTTON_TYPE_WHITE },		// �悤�����݂�
	{ mes_boxbutton_02_03, BOX2BMP_BUTTON_TYPE_WHITE },		// �}�[�L���O
	{ mes_boxbutton_02_04, BOX2BMP_BUTTON_TYPE_WHITE },		// �ɂ���
	{ mes_boxbutton_02_07, BOX2BMP_BUTTON_TYPE_CANCEL }		// ��߂�
};

// ���j���[�����e�[�u���F�{�b�N�X�e�[�}�ύX
static const BOX2BMP_BUTTON_LIST ThemaMenuStrTbl[] =
{
	{ mes_boxmenu_03_01, BOX2BMP_BUTTON_TYPE_WHITE },			// �W�����v����
	{ mes_boxmenu_03_02, BOX2BMP_BUTTON_TYPE_WHITE },			// ���ׂ���
	{ mes_boxmenu_03_03, BOX2BMP_BUTTON_TYPE_WHITE },			// �Ȃ܂�
	{ mes_boxmenu_03_04, BOX2BMP_BUTTON_TYPE_CANCEL },		// ��߂�
};

// ���j���[�����e�[�u���F�ǎ����j���[
static const BOX2BMP_BUTTON_LIST WallPaperMenuStrTbl[] =
{
	{ mes_boxmenu_04_01, BOX2BMP_BUTTON_TYPE_WHITE },			// �ӂ������P
	{ mes_boxmenu_04_02, BOX2BMP_BUTTON_TYPE_WHITE },			// �ӂ������Q
	{ mes_boxmenu_04_03, BOX2BMP_BUTTON_TYPE_WHITE },			// �ӂ������R
	{ mes_boxmenu_04_04, BOX2BMP_BUTTON_TYPE_WHITE },			// �G�g�Z�g��
	{ mes_boxmenu_04_32, BOX2BMP_BUTTON_TYPE_CANCEL },		// ���ǂ�
};

// ���j���[�����e�[�u���F�ǎ����j���[�i�ǉ��ǎ�����j
static const BOX2BMP_BUTTON_LIST WallPaperMenuSpecialStrTbl[] =
{
	{ mes_boxmenu_04_23, BOX2BMP_BUTTON_TYPE_WHITE },			// �X�y�V����
	{ mes_boxmenu_04_01, BOX2BMP_BUTTON_TYPE_WHITE },			// �ӂ������P
	{ mes_boxmenu_04_02, BOX2BMP_BUTTON_TYPE_WHITE },			// �ӂ������Q
	{ mes_boxmenu_04_03, BOX2BMP_BUTTON_TYPE_WHITE },			// �ӂ������R
	{ mes_boxmenu_04_04, BOX2BMP_BUTTON_TYPE_WHITE },			// �G�g�Z�g��
	{ mes_boxmenu_04_32, BOX2BMP_BUTTON_TYPE_CANCEL },		// ���ǂ�
};

// ���j���[�����e�[�u���F�ǎ����j���[�i�ӂ������P�j
static const BOX2BMP_BUTTON_LIST WPLabel1MenuStrTbl[] =
{
	{ mes_boxmenu_04_07, BOX2BMP_BUTTON_TYPE_WHITE },			// ����
	{ mes_boxmenu_04_08, BOX2BMP_BUTTON_TYPE_WHITE },			// �V�e�B
	{ mes_boxmenu_04_09, BOX2BMP_BUTTON_TYPE_WHITE },			// ���΂�
	{ mes_boxmenu_04_10, BOX2BMP_BUTTON_TYPE_WHITE },			// �T�o���i
	{ mes_boxmenu_04_32, BOX2BMP_BUTTON_TYPE_CANCEL },		// ���ǂ�
};

// ���j���[�����e�[�u���F�ǎ����j���[�i�ӂ������Q�j
static const BOX2BMP_BUTTON_LIST WPLabel2MenuStrTbl[] =
{
	{ mes_boxmenu_04_11, BOX2BMP_BUTTON_TYPE_WHITE },			// ������
	{ mes_boxmenu_04_12, BOX2BMP_BUTTON_TYPE_WHITE },			// ������
	{ mes_boxmenu_04_13, BOX2BMP_BUTTON_TYPE_WHITE },			// �䂫���
	{ mes_boxmenu_04_14, BOX2BMP_BUTTON_TYPE_WHITE },			// �ǂ�����
	{ mes_boxmenu_04_32, BOX2BMP_BUTTON_TYPE_CANCEL },		// ���ǂ�
};

// ���j���[�����e�[�u���F�ǎ����j���[�i�ӂ������R�j
static const BOX2BMP_BUTTON_LIST WPLabel3MenuStrTbl[] =
{
	{ mes_boxmenu_04_15, BOX2BMP_BUTTON_TYPE_WHITE },			// ���݂�
	{ mes_boxmenu_04_16, BOX2BMP_BUTTON_TYPE_WHITE },			// �����Ă�
	{ mes_boxmenu_04_17, BOX2BMP_BUTTON_TYPE_WHITE },			// ����
	{ mes_boxmenu_04_18, BOX2BMP_BUTTON_TYPE_WHITE },			// ����
	{ mes_boxmenu_04_32, BOX2BMP_BUTTON_TYPE_CANCEL },		// ���ǂ�
};

// ���j���[�����e�[�u���F�ǎ����j���[�i�G�g�Z�g���j
static const BOX2BMP_BUTTON_LIST WPLabel4MenuStrTbl[] =
{
	{ mes_boxmenu_04_19, BOX2BMP_BUTTON_TYPE_WHITE },			// �|�P�Z��
	{ mes_boxmenu_04_20, BOX2BMP_BUTTON_TYPE_WHITE },			// ���񂼂�
	{ mes_boxmenu_04_21, BOX2BMP_BUTTON_TYPE_WHITE },			// �`�F�b�N
	{ mes_boxmenu_04_22, BOX2BMP_BUTTON_TYPE_WHITE },			// �V���v��
	{ mes_boxmenu_04_32, BOX2BMP_BUTTON_TYPE_CANCEL },		// ���ǂ�
};

// ���j���[�����e�[�u���F�ǎ����j���[�i�X�y�V�����P�̂݁j
static const BOX2BMP_BUTTON_LIST WPLabelS01MenuStrTbl[] =
{
	{ mes_boxmenu_04_24, BOX2BMP_BUTTON_TYPE_WHITE },			// �X�y�V�����P
	{ mes_boxmenu_04_25, BOX2BMP_BUTTON_TYPE_WHITE },			// �X�y�V�����Q
	{ mes_boxmenu_04_26, BOX2BMP_BUTTON_TYPE_WHITE },			// �X�y�V�����R
	{ mes_boxmenu_04_27, BOX2BMP_BUTTON_TYPE_WHITE },			// �X�y�V�����S
	{ mes_boxmenu_04_32, BOX2BMP_BUTTON_TYPE_CANCEL },		// ���ǂ�
};

// ���j���[�����e�[�u���F�ǎ����j���[�i�X�y�V�����Q����j
static const BOX2BMP_BUTTON_LIST WPLabelS11MenuStrTbl[] =
{
	{ mes_boxmenu_04_24, BOX2BMP_BUTTON_TYPE_WHITE },			// �X�y�V�����P
	{ mes_boxmenu_04_25, BOX2BMP_BUTTON_TYPE_WHITE },			// �X�y�V�����Q
	{ mes_boxmenu_04_26, BOX2BMP_BUTTON_TYPE_WHITE },			// �X�y�V�����R
	{ mes_boxmenu_04_27, BOX2BMP_BUTTON_TYPE_WHITE },			// �X�y�V�����S
	{ mes_boxmenu_04_34, BOX2BMP_BUTTON_TYPE_WHITE },			// ����
	{ mes_boxmenu_04_32, BOX2BMP_BUTTON_TYPE_CANCEL },		// ���ǂ�
};

// ���j���[�����e�[�u���F�ǎ����j���[�i�X�y�V�����Q�j
static const BOX2BMP_BUTTON_LIST WPLabelS02MenuStrTbl[] =
{
	{ mes_boxmenu_04_28, BOX2BMP_BUTTON_TYPE_WHITE },			// �X�y�V�����T
	{ mes_boxmenu_04_29, BOX2BMP_BUTTON_TYPE_WHITE },			// �X�y�V�����U
	{ mes_boxmenu_04_30, BOX2BMP_BUTTON_TYPE_WHITE },			// �X�y�V�����V
	{ mes_boxmenu_04_31, BOX2BMP_BUTTON_TYPE_WHITE },			// �X�y�V�����W
	{ mes_boxmenu_04_33, BOX2BMP_BUTTON_TYPE_WHITE },			// ����
	{ mes_boxmenu_04_32, BOX2BMP_BUTTON_TYPE_CANCEL },		// ���ǂ�
};

// ���j���[������e�[�u���F�o�g���{�b�N�X
static const BOX2BMP_BUTTON_LIST BattleBoxMenuStrTbl[] =
{
	{ mes_boxbutton_02_09, BOX2BMP_BUTTON_TYPE_WHITE },		// ���ǂ�����
	{ mes_boxbutton_02_01, BOX2BMP_BUTTON_TYPE_WHITE },		// �悤�����݂�
	{ mes_boxbutton_02_02, BOX2BMP_BUTTON_TYPE_WHITE },		// ��������
	{ mes_boxbutton_02_07, BOX2BMP_BUTTON_TYPE_CANCEL }		// ��߂�
};

// ���j���[������e�[�u���F�����������E����������
static const BOX2BMP_BUTTON_LIST ItemMenuStrTbl[] =
{
	{ mes_boxbutton_08_03, BOX2BMP_BUTTON_TYPE_WHITE },		// ���ǂ�����
	{ mes_boxbutton_08_01, BOX2BMP_BUTTON_TYPE_WHITE },		// �o�b�O��
	{ mes_boxbutton_02_07, BOX2BMP_BUTTON_TYPE_CANCEL }		// ��߂�
};

// ���j���[������e�[�u���F�����������E�������Ȃ�
static const BOX2BMP_BUTTON_LIST ItemNoneMenuStrTbl[] =
{
	{ mes_boxbutton_08_02, BOX2BMP_BUTTON_TYPE_WHITE },		// ��������
	{ mes_boxbutton_02_07, BOX2BMP_BUTTON_TYPE_CANCEL }		// ��߂�
};

// ���j���[������e�[�u���F�{�b�N�X�������C��
static const BOX2BMP_BUTTON_LIST SleepMenuStrTbl[] =
{
	{ mes_boxbutton_02_10, BOX2BMP_BUTTON_TYPE_WHITE },		// �˂�����
	{ mes_boxbutton_02_07, BOX2BMP_BUTTON_TYPE_CANCEL }		// ��߂�
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		seq			���C���V�[�P���X
 *
 * @retval	"TRUE = ������"
 * @retval	"FALSE = �I��"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2SEQ_Main( BOX2_SYS_WORK * syswk, int * seq )
{
	*seq = MainSeq[*seq]( syswk );

	if( *seq == BOX2SEQ_MAINSEQ_END ){
		return FALSE;
	}

	if( syswk->app != NULL ){
		BOX2BMP_PrintUtilTrans( syswk->app );
		BOX2OBJ_AnmMain( syswk->app );
	}

	return TRUE;
}


//============================================================================================
//============================================================================================
//	���C���V�[�P���X
//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�b�N�X��ʏ�����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_INIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Init( BOX2_SYS_WORK * syswk )
{
	FS_EXTERN_OVERLAY(ui_common);

	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

	// ���荞�ݒ�~
	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );
	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �T�u��ʂ����C����
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );

	// �A�v���q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BOX_APP, 0x80000 );
	syswk->app = GFL_HEAP_AllocClearMemory( HEAPID_BOX_APP, sizeof(BOX2_APP_WORK) );

	syswk->app->pokeicon_ah = GFL_ARC_OpenDataHandle( ARCID_POKEICON, HEAPID_BOX_APP );

	GFL_UI_KEY_GetRepeatSpeed( &syswk->app->key_repeat_speed, &syswk->app->key_repeat_wait );
	GFL_UI_KEY_SetRepeatSpeed( 6, 6 );

	BOX2MAIN_VramBankSet();
	BOX2MAIN_BgInit( syswk );
	BOX2MAIN_PaletteFadeInit( syswk );

	BOX2MAIN_BgGraphicLoad( syswk );

	BOX2MAIN_MsgInit( syswk );

	BOX2MAIN_LoadLocalNoList( syswk );

	BOX2BMP_Init( syswk );

	BOX2OBJ_Init( syswk );
	BOX2OBJ_TrayPokeIconChange( syswk );
	BOX2OBJ_PartyPokeIconChange( syswk );

	BOX2MAIN_WallPaperSet(
		syswk, BOX2MAIN_GetWallPaperNumber( syswk, syswk->tray ), BOX2MAIN_TRAY_SCROLL_NONE );

	BOX2BGWFRM_Init( syswk );

	BOX2BMP_TitlePut( syswk );
	BOX2BMP_DefStrPut( syswk );

	BOX2OBJ_TrayIconCgxMakeAll( syswk );

	BOX2UI_CursorMoveInit( syswk );

	BOX2MAIN_YesNoWinInit( syswk );

	BOX2MAIN_SetBlendAlpha( TRUE );

	GFL_NET_ReloadIconTopOrBottom( TRUE, HEAPID_BOX_APP );

	BOX2MAIN_InitVBlank( syswk );

	return syswk->next_seq;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�b�N�X��ʉ������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_RELEASE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Release( BOX2_SYS_WORK * syswk )
{
	FS_EXTERN_OVERLAY(ui_common);

	BOX2MAIN_ExitVBlank( syswk );

	BOX2MAIN_YesNoWinExit( syswk );

	BOX2UI_CursorMoveExit( syswk );

	BOX2BGWFRM_Exit( syswk->app );

	BOX2OBJ_Exit( syswk->app );

	BOX2BMP_Exit( syswk );

	BOX2MAIN_FreeLocalNoList( syswk );

	BOX2MAIN_MsgExit( syswk );

	BOX2MAIN_PaletteFadeExit( syswk );
	BOX2MAIN_BgExit( syswk );

	GFL_UI_KEY_SetRepeatSpeed( syswk->app->key_repeat_speed, syswk->app->key_repeat_wait );

	GFL_ARC_CloseDataHandle( syswk->app->pokeicon_ah );

	GFL_HEAP_FreeMemory( syswk->app );
	GFL_HEAP_DeleteHeap( HEAPID_BOX_APP );
	syswk->app = NULL;

	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );

	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

	return syswk->next_seq;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���C�v�i�t�F�[�h�j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_WIPE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Wipe( BOX2_SYS_WORK * syswk )
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		return syswk->app->wipe_seq;
	}
	return BOX2SEQ_MAINSEQ_WIPE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�p���b�g�t�F�[�h
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PALETTE_FADE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PaletteFade( BOX2_SYS_WORK * syswk )
{
	if( PaletteFadeCheck(syswk->app->pfd) == 0 ){
		return syswk->next_seq;
	}
	return BOX2SEQ_MAINSEQ_PALETTE_FADE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�E�F�C�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_WAIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Wait( BOX2_SYS_WORK * syswk )
{
	if( syswk->app->wait == 0 ){
		return syswk->next_seq;
	}else{
		syswk->app->wait--;
	}
	return BOX2SEQ_MAINSEQ_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�FVBlank����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_VFUNC
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_VFunc( BOX2_SYS_WORK * syswk )
{
	if( syswk->app->vfunk.func != NULL ){
		pBOX2_FUNC func = syswk->app->vfunk.func;
		if( func( syswk ) == 0 ){
			syswk->app->vfunk.func = NULL;
			return syswk->app->vnext_seq;
		}
	}
	return BOX2SEQ_MAINSEQ_VFUNC;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�L�[�E�^�b�`�҂�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_TRGWAIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_TrgWait( BOX2_SYS_WORK * syswk )
{
	if( GFL_UI_TP_GetTrg() == TRUE ){
		PMSND_PlaySE( SE_BOX2_DECIDE );
		GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
		return syswk->next_seq;
	}

	if( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_CANCEL | PAD_BUTTON_DECIDE ) ){
		PMSND_PlaySE( SE_BOX2_DECIDE );
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return syswk->next_seq;
	}

	return BOX2SEQ_MAINSEQ_TRGWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�͂��E������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_YESNO
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_YesNo( BOX2_SYS_WORK * syswk )
{
	APP_TASKMENU_UpdateMenu( syswk->app->ynWork );
	if( APP_TASKMENU_IsFinish( syswk->app->ynWork ) == TRUE ){
		u8	pos = APP_TASKMENU_GetCursorPos( syswk->app->ynWork );
		APP_TASKMENU_CloseMenu( syswk->app->ynWork );
		GFL_UI_KEY_SetRepeatSpeed( 6, 6 );
		if( pos == 0 ){
			return YesNoFunc[syswk->app->ynID].yes( syswk );
		}else{
			return YesNoFunc[syswk->app->ynID].no( syswk );
		}
	}
	return BOX2SEQ_MAINSEQ_YESNO;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�^���A�j��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BUTTON_ANM
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ButtonAnm( BOX2_SYS_WORK * syswk )
{
	if( BOX2MAIN_ButtonAnmMain( syswk ) == FALSE ){
		return syswk->next_seq;
	}
	return BOX2SEQ_MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�T�u�v���Z�X�Ăяo��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_SUB_PROCCALL
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_SubProcCall( BOX2_SYS_WORK * syswk )
{
	SubProcFunc[syswk->subProcType].call( syswk );
	return BOX2SEQ_MAINSEQ_SUB_PROCMAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�T�u�v���Z�X�I���҂�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_SUB_PROCMAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_SubProcMain( BOX2_SYS_WORK * syswk )
{
	if( syswk->procStatus != GFL_PROC_MAIN_VALID ){
		SubProcFunc[syswk->subProcType].exit( syswk );
		syswk->next_seq = SubProcFunc[syswk->subProcType].rcvSeq;
		return BOX2SEQ_MAINSEQ_INIT;
	}
	return BOX2SEQ_MAINSEQ_SUB_PROCMAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�J�n����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_START
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Start( BOX2_SYS_WORK * syswk )
{
	int	seq;

	PMSND_PlaySE( SE_BOX2_LOG_IN );

	switch( syswk->dat->callMode ){
	case BOX_MODE_AZUKERU:		// ��������
		PartyFrmSet_PartyOut( syswk );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		BOX2MAIN_PokeInfoPut( syswk, BOX2OBJ_POKEICON_TRAY_MAX );
		seq = BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
		break;

	case BOX_MODE_TURETEIKU:	// ��Ă���
		BOX2MAIN_PokeInfoPut( syswk, 0 );
		seq = BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
		break;

	case BOX_MODE_SEIRI:		// �{�b�N�X������
		BOX2MAIN_PokeInfoPut( syswk, 0 );
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		seq = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		break;

	case BOX_MODE_ITEM:			// �A�C�e������
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
		BOX2MAIN_PokeInfoPut( syswk, 0 );
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		seq = BOX2SEQ_MAINSEQ_ITEM_MAIN;
		break;

	case BOX_MODE_BATTLE:		// �o�g���{�b�N�X
		if( PokeParty_GetPokeCount(syswk->dat->pokeparty) == 0 ){
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
			BOX2MAIN_PokeInfoPut( syswk, 0 );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BATTLEBOX_MAIN, 0 );
			seq = BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;
		}else{
			PartyFrmSet_PartyOut( syswk );
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
			BOX2BGWFRM_BoxListButtonOn( syswk->app );
			BOX2MAIN_PokeInfoPut( syswk, BOX2OBJ_POKEICON_TRAY_MAX );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BATTLEBOX_PARTY, 0 );
			seq = BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN;
		}
		break;

	case BOX_MODE_SLEEP:		// �˂�����
		BOX2MAIN_PokeInfoPut( syswk, 0 );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );
		seq = BOX2SEQ_MAINSEQ_SLEEP_MAIN;
		break;
	}

	return FadeInSet( syswk, seq );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�J�[�\���ړ���̏���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_CURSORMOVE_RET
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_CursorMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
	if( syswk->app->msg_put == 1 ){
		if( syswk->dat->callMode == BOX_MODE_ITEM ){
			if( syswk->app->get_item == ITEM_DUMMY_DATA ){
				BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_SET, BOX2BMPWIN_ID_MSG1 );
			}else{
				BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_INIT, BOX2BMPWIN_ID_MSG1 );
			}
		}else{
			BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
		}
		syswk->app->msg_put = 0;
	}
	return syswk->next_seq;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�|�P�������j���[�\����Ԃ֕��A
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeMenuRcv( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		switch( syswk->dat->callMode ){
		case BOX_MODE_TURETEIKU:
			BOX2BMP_MenuStrPrint( syswk, PartyInMenuStrTbl, 5 );
			break;
		case BOX_MODE_AZUKERU:
			BOX2BMP_MenuStrPrint( syswk, PartyOutMenuStrTbl, 5 );
			break;
		case BOX_MODE_SEIRI:
			BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 6 );
			break;
		case BOX_MODE_ITEM:
			BOX2SEQ_ItemModeMenuSet( syswk, syswk->app->get_item );
			break;
		case BOX_MODE_BATTLE:
			BOX2BMP_MenuStrPrint( syswk, BattleBoxMenuStrTbl, 4 );
			break;
		case BOX_MODE_SLEEP:
			BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG4 );
			BOX2BMP_MenuStrPrint( syswk, SleepMenuStrTbl, 2 );
			break;
		default:
			break;
		}
		if( syswk->app->poke_free_err == 1 ){
			syswk->app->poke_free_err = 0;
			if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
				BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );
			}else{
				BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
				BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
			}
		}
		BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV );

	case 1:
		syswk->app->sub_seq = 0;
		BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
		BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );
		switch( syswk->dat->callMode ){
		case BOX_MODE_TURETEIKU:
			BOX2BMP_PokeSelectMsgPut(
				syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
			return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;

		case BOX_MODE_AZUKERU:
			BOX2BMP_PokeSelectMsgPut(
				syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
			return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;

		case BOX_MODE_SEIRI:
			BOX2BMP_PokeSelectMsgPut(
				syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
			if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
				return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
			}else{
				return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
			}
			break;

		case BOX_MODE_ITEM:
			if( syswk->app->get_item == ITEM_DUMMY_DATA ){
				BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_SET, BOX2BMPWIN_ID_MSG1 );
			}else{
				BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_INIT, BOX2BMPWIN_ID_MSG1 );
			}
			if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
				return BOX2SEQ_MAINSEQ_ITEM_MAIN;
			}else{
				return BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN;
			}
			break;

		case BOX_MODE_BATTLE:
			BOX2BMP_PokeSelectMsgPut(
				syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
			if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
				return BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;
			}else{
				return BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN;
			}
			break;

		case BOX_MODE_SLEEP:
			BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
			return BOX2SEQ_MAINSEQ_SLEEP_MAIN;

		default:
			break;
		}
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�^�b�`�ł̃g���C�A�C�R���X�N���[��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_TRAYSCROLL_TOUCH
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_TrayScrollTouch( BOX2_SYS_WORK * syswk )
{
	SEQWK_TRAYICON_SCROLL * seqwk;
	u32	x, y;
	u32	oy;

	if( BOX2UI_HitCheckTrayScroll( &x, &y ) == FALSE ){
		GFL_HEAP_FreeMemory( syswk->app->seqwk );
		return syswk->next_seq;
	}

	seqwk = syswk->app->seqwk;

	oy = syswk->app->tpy;

	syswk->app->tpy = y;

	if( GFL_STD_Abs(oy-y) >= 3 ){
		seqwk->cnt = GFL_STD_Abs( oy - y ) / 8;
		if( y < oy ){
			seqwk->mv = 1;
			BOX2MAIN_InitTrayIconScroll( syswk, 1 );
			PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
			if( syswk->next_seq == BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN ){
				return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollUp, BOX2SEQ_MAINSEQ_TRAYSCROLL_TOUCH );
			}else{
				return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollUpJump, BOX2SEQ_MAINSEQ_TRAYSCROLL_TOUCH );
			}
		}
		if( y > oy ){
			seqwk->mv = -1;
			BOX2MAIN_InitTrayIconScroll( syswk, -1 );
			PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
			if( syswk->next_seq == BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN ){
				return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollDown, BOX2SEQ_MAINSEQ_TRAYSCROLL_TOUCH );
			}else{
				return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollDownJump, BOX2SEQ_MAINSEQ_TRAYSCROLL_TOUCH );
			}
		}
	}

	if( seqwk->cnt != 0 ){
		seqwk->cnt--;
		if( seqwk->mv == 1 ){
			BOX2MAIN_InitTrayIconScroll( syswk, 1 );
			PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
			if( syswk->next_seq == BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN ){
				return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollUp, BOX2SEQ_MAINSEQ_TRAYSCROLL_TOUCH );
			}else{
				return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollUpJump, BOX2SEQ_MAINSEQ_TRAYSCROLL_TOUCH );
			}
		}
		if( seqwk->mv == -1 ){
			BOX2MAIN_InitTrayIconScroll( syswk, -1 );
			PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
			if( syswk->next_seq == BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN ){
				return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollDown, BOX2SEQ_MAINSEQ_TRAYSCROLL_TOUCH );
			}else{
				return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollDownJump, BOX2SEQ_MAINSEQ_TRAYSCROLL_TOUCH );
			}
		}
	}

	return BOX2SEQ_MAINSEQ_TRAYSCROLL_TOUCH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�I�������i���b�Z�[�W�Ȃ��j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BOXEND_DIRECT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxEndDirect( BOX2_SYS_WORK * syswk )
{
	return BoxArg_BoxEndYes( syswk );
}


//============================================================================================
//	�{�b�N�X����
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�b�N�X�������C��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	// �Z���N�g�{�^��
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		syswk->get_start_mode = 0;
		return ChgSeqSelectButton( syswk, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_INIT );
	}

	ret = BOX2UI_HitCheckTrgTrayPoke();
	if( ret != GFL_UI_TP_HIT_NONE ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
				BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 6 );
			}
			BOX2MAIN_PokeInfoPut( syswk, ret );
			return TrayPokeGet( syswk, ret );
		}else{
			CURSORMOVE_PosSet( syswk->app->cmwk, ret );
			BOX2UI_PutHandCursor( syswk, ret );
			BOX2MAIN_PokeInfoOff( syswk );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
				u8	pos = syswk->get_pos;
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				BOX2OBJ_PokeCursorVanish( syswk, FALSE );
				BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
				BOX2MAIN_ResetTouchBar( syswk );
				BOX2BGWFRM_TemochiButtonOn( syswk->app );
				return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_ARRANGE_MAIN );
			}else{
				return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
			}
		}
	}

	ret = BOX2UI_BoxArrangeMain( syswk );

	switch( ret ){
	case BOX2UI_ARRANGE_MAIN_NAME:		// 30: �{�b�N�X��
		PMSND_PlaySE( SE_BOX2_DECIDE );
		BOX2MAIN_PokeInfoOff( syswk );
		return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_BOXTHEMA_INIT );

	case BOX2UI_ARRANGE_MAIN_LEFT:		// 31: �g���C�؂�ւ����E��
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MAIN_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_ARRANGE_MAIN_NAME );
		return TrayLeftScrollSet( syswk, TRUE, BOX2SEQ_MAINSEQ_ARRANGE_MAIN );

	case BOX2UI_ARRANGE_MAIN_RIGHT:	// 32: �g���C�؂�ւ����E�E
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MAIN_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_ARRANGE_MAIN_NAME );
		return TrayRightScrollSet( syswk, TRUE, BOX2SEQ_MAINSEQ_ARRANGE_MAIN );

	case BOX2UI_ARRANGE_MAIN_PARTY:	// 33: �莝���|�P����
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_POKE_BTN, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_INIT) );

	case BOX2UI_ARRANGE_MAIN_RETURN1:	// 34: �߂�P
		PMSND_PlaySE( SE_BOX2_CLOSE );
		syswk->dat->retMode = BOX_END_MODE_C_GEAR;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_END, 8, BOX2SEQ_MAINSEQ_BOXEND_DIRECT );

	case BOX2UI_ARRANGE_MAIN_RETURN2:	// 35: �߂�Q
		PMSND_PlaySE( SE_BOX2_CANCEL );
		syswk->dat->retMode = BOX_END_MODE_MENU;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case BOX2UI_ARRANGE_MAIN_GET:			// 36: ����
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->get_start_mode = 0;
		syswk->mv_cnv_mode = 0;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU1, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_INIT) );

	case BOX2UI_ARRANGE_MAIN_STATUS:	// 37: �悤�����݂�
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->subProcMode = SUB_PROC_MODE_MENU_BOX;
		syswk->get_tray    = syswk->tray;
		syswk->cur_rcv_pos = BOX2UI_ARRANGE_MAIN_STATUS;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU2, BOX2SEQ_MAINSEQ_STATUS_CALL );

	case BOX2UI_ARRANGE_MAIN_ITEM:		// 38: ��������
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->subProcMode = SUB_PROC_MODE_MENU_BOX;
		syswk->cur_rcv_pos = BOX2UI_ARRANGE_MAIN_ITEM;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU3, BOX2SEQ_MAINSEQ_ITEM_MENU_CHECK );

	case BOX2UI_ARRANGE_MAIN_MARKING:	// 39: �}�[�L���O
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_MARKING_INIT) );

	case BOX2UI_ARRANGE_MAIN_FREE:		// 40: �ɂ���
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_POKEFREE_INIT) );

	case BOX2UI_ARRANGE_MAIN_CLOSE:		// 41: �Ƃ���
		PMSND_PlaySE( SE_BOX2_CANCEL );
		{
			u8	pos = syswk->get_pos;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_ARRANGE_MENU_CLOSE );

	case BOX2UI_ARRANGE_MAIN_CONV:			// 42: �֗����[�h
		syswk->get_start_mode = 0;
		return ChgSeqSelectTouch( syswk, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_INIT );

	case CURSORMOVE_NO_MOVE_LEFT:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_ARRANGE_MAIN_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			BOX2MAIN_PokeSelectOff( syswk );
			return TrayLeftScrollSet( syswk, TRUE, BOX2SEQ_MAINSEQ_ARRANGE_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_RIGHT:	// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_ARRANGE_MAIN_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			BOX2MAIN_PokeSelectOff( syswk );
			return TrayRightScrollSet( syswk, TRUE, BOX2SEQ_MAINSEQ_ARRANGE_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_UP:			// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_DOWN:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NONE:						// ����Ȃ�
		break;

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_ARRANGE_MAIN_NAME ){
				BOX2MAIN_PokeInfoPut( syswk, pos );
			}else{
				if( pos != BOX2UI_ARRANGE_MAIN_GET &&
					pos != BOX2UI_ARRANGE_MAIN_STATUS &&
					pos != BOX2UI_ARRANGE_MAIN_ITEM &&
					pos != BOX2UI_ARRANGE_MAIN_MARKING &&
					pos != BOX2UI_ARRANGE_MAIN_FREE &&
					pos != BOX2UI_ARRANGE_MAIN_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ARRANGE_MAIN) );

	case CURSORMOVE_CANCEL:				// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		// ���j���[�\����
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
			u8	pos = syswk->get_pos;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_ARRANGE_MENU_CLOSE );
		}
		syswk->dat->retMode = BOX_END_MODE_MENU;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;

	default:
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_DECIDE );
			BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 6 );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_ARRANGE_MAIN_GET, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MAIN_GET );
			return TrayPokeGetKey( syswk, ret, BOX2SEQ_MAINSEQ_ARRANGE_MAIN );
		}
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�g���C�̃|�P�����ړ���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_TRAYPOKE_CHG_END
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeTrayPokeChgEnd( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_PokeDataMove( syswk );
	PokeMoveWorkFree( syswk );
	CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MAIN_GET );
	BOX2UI_PutHandCursor( syswk, BOX2UI_ARRANGE_MAIN_GET );
	BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );
	BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
	return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�{�b�N�X�����v���j���[�����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_MENU_CLOSE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeMenuClose( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	BOX2BGWFRM_TemochiButtonOn( syswk->app );
	return VFuncSet( syswk, BOX2MAIN_VFuncMenuMove, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ARRANGE_MAIN) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u���ށv��������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_POKEGET_INIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeGetInit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		syswk->box_mv_pos = syswk->tray;	// ���ݎQ�Ƃ��Ă���{�b�N�X
		// �Z���N�g�{�^��
		if( syswk->mv_cnv_mode == 1 ){
			syswk->get_tray = BOX2MAIN_GETPOS_NONE;
			syswk->get_pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
				syswk->get_pos = 0;
				BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
			}
		}else{
			syswk->get_tray = syswk->tray;	// �擾�����{�b�N�X
			BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
			BOX2MAIN_ResetTouchBar( syswk );
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
			BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
		}
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_INIT );

	case 1:
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		syswk->app->sub_seq++;
		return BoxMoveFrmInSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_INIT );

	case 2:
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->get_pos );
		BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_ON );
		syswk->app->old_cur_pos = syswk->get_pos;
		syswk->app->sub_seq = 0;
		// �Z���N�g�{�^��
		if( syswk->mv_cnv_mode == 1 ){
			if( BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, syswk->tray, ID_PARA_poke_exist, NULL ) == 0 ){
				BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_PASSIVE );
			}
			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_BLUE, TRUE );
			return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN;
		}else if( syswk->mv_cnv_mode == 2 ){
			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_BLUE, TRUE );
		}else{
			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );
		}
		return PokeMoveGetKey( syswk, syswk->get_pos );
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_INIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u���ށv���C������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeGetMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u32	x, y;

	if( syswk->mv_cnv_end == 1 ){
		syswk->mv_cnv_mode = 0;
		syswk->mv_cnv_end = 0;
		return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_EXIT );
	}

	// �Z���N�g�{�^��
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		if( syswk->mv_cnv_mode != 0 && syswk->poke_get_key == 0 ){
			BOX2MAIN_ChgCursorButton( syswk );
			syswk->mv_cnv_mode = 0;
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_EXIT );
		}
	}

	// �|�P�����h���b�O�`�F�b�N
	if( syswk->poke_get_key == 0 ){
		ret = BOX2UI_HitCheckTrgTrayPoke();
		if( ret != GFL_UI_TP_HIT_NONE ){
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
			BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
			if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
				PMSND_PlaySE( SE_BOX2_POKE_CATCH );
				GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
				BOX2MAIN_PokeInfoPut( syswk, ret );
				CURSORMOVE_PosSet( syswk->app->cmwk, ret );
				return BoxPartyPokeGet( syswk, ret, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN );
			}else{
				CURSORMOVE_PosSet( syswk->app->cmwk, ret );
				BOX2UI_SetTouchBarIconPokeGet( syswk, ret );
				syswk->app->old_cur_pos = ret;
				BOX2UI_PutHandCursor( syswk, ret );
				BOX2MAIN_PokeInfoOff( syswk );
			}
			return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN;
		}
	}

	// �^�b�`�g���C�X�N���[���`�F�b�N
	if( BOX2UI_HitCheckTrayScrollStart( &x, &y ) == TRUE ){
		syswk->app->tpy = y;
		syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN;
		CreateTrayIconScrollWork( syswk );
		return BOX2SEQ_MAINSEQ_TRAYSCROLL_TOUCH;
	}

	// �L�[�E�^�b�`�`�F�b�N
	ret = BOX2UI_ArrangePokeGetMain( syswk );

	switch( ret ){
	case BOX2UI_ARRANGE_PGT_NAME:		// 30: �{�b�N�X��
		BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
		// ���擾
		if( syswk->poke_get_key == 0 ){
			PMSND_PlaySE( SE_BOX2_DECIDE );
			BOX2MAIN_PokeInfoOff( syswk );
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_BOXTHEMA_INIT );
		}
		break;

	case BOX2UI_ARRANGE_PGT_LEFT:		// 31: �{�b�N�X�؂�ւ����i���j
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_PGT_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_ARRANGE_PGT_NAME );
		BOX2UI_SetTouchBarIconPokeGet( syswk, BOX2UI_ARRANGE_PGT_NAME );
		syswk->app->old_cur_pos = BOX2UI_ARRANGE_PGT_NAME;
		// ���擾
		if( syswk->poke_get_key == 0 ){
			BOX2MAIN_PokeSelectOff( syswk );
		// �擾��
		}else{
			BOX2OBJ_MovePokeIconHand( syswk );
		}
		return TrayLeftScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN );

	case BOX2UI_ARRANGE_PGT_RIGHT:	// 32: �{�b�N�X�؂�ւ����i�E�j
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_PGT_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_ARRANGE_PGT_NAME );
		BOX2UI_SetTouchBarIconPokeGet( syswk, BOX2UI_ARRANGE_PGT_NAME );
		syswk->app->old_cur_pos = BOX2UI_ARRANGE_PGT_NAME;
		// ���擾
		if( syswk->poke_get_key == 0 ){
			BOX2MAIN_PokeSelectOff( syswk );
		// �擾��
		}else{
			BOX2OBJ_MovePokeIconHand( syswk );
		}
		return TrayRightScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN );

	case BOX2UI_ARRANGE_PGT_PARTY:	// 39: �莝���|�P����
		PMSND_PlaySE( SE_BOX2_DECIDE );
		if( syswk->poke_get_key == 0 ){
			BOX2MAIN_PokeInfoOff( syswk );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_POKE_BTN, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_PARTY_CHANGE) );

	case BOX2UI_ARRANGE_PGT_STATUS:	// 40: �X�e�[�^�X
		if( syswk->poke_get_key == 0 ){
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
				break;
			}else{
				if( BOX2MAIN_PokeParaGet( syswk, pos, syswk->tray, ID_PARA_poke_exist, NULL ) == 0 ){
					break;
				}
			}
			syswk->get_tray = syswk->tray;
			syswk->get_pos  = pos;
			syswk->subProcMode = SUB_PROC_MODE_TB_BOX;
		}else{
			syswk->subProcMode = SUB_PROC_MODE_TB_BOX_GET;
			syswk->tb_status_pos = CURSORMOVE_PosGet( syswk->app->cmwk );
		}
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_STATUS, 1, BOX2SEQ_MAINSEQ_STATUS_CALL );

	case BOX2UI_ARRANGE_PGT_RET:		// 41: �߂�
		if( syswk->poke_get_key == 0 ){
			// �{�b�N�X�������C����
			PMSND_PlaySE( SE_BOX2_CANCEL );
			syswk->mv_cnv_mode = 0;
			return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_EXIT) );
		}
		break;

	case BOX2UI_ARRANGE_PGT_CONV:		// 42: �֗����[�h
		if( syswk->mv_cnv_mode != 0 && syswk->poke_get_key == 0 ){
			syswk->mv_cnv_mode = 0;
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_EXIT );
		}
		break;

	case CURSORMOVE_CANCEL:					// �L�����Z��
		if( syswk->poke_get_key == 1 ){
			// �擾���Ȃ̂Ŏ��s
			return PokeMovePutKey( syswk, BOX2MAIN_GETPOS_NONE, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN );
		}else{
			// �{�b�N�X�������C����
			PMSND_PlaySE( SE_BOX2_CANCEL );
			syswk->mv_cnv_mode = 0;
			return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_EXIT) );
		}
		break;

	case CURSORMOVE_NO_MOVE_UP:			// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_ARRANGE_PGT_TRAY2 ){
			PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
			BOX2MAIN_InitTrayIconScroll( syswk, -1 );
			return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollDown, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_DOWN:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_ARRANGE_PGT_TRAY5 ){
			PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
			BOX2MAIN_InitTrayIconScroll( syswk, 1 );
			return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollUp, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_LEFT:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_ARRANGE_PGT_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			return TrayLeftScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_RIGHT:	// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_ARRANGE_PGT_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			return TrayRightScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN );
		}
		break;

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( syswk->poke_get_key == 0 ){
				if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
					BOX2MAIN_PokeInfoPut( syswk, pos );
				}else{
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
			if( pos >= BOX2UI_ARRANGE_PGT_TRAY2 && pos <= BOX2UI_ARRANGE_PGT_TRAY5 ){
				BOX2OBJ_ChangeTrayName( syswk, pos-BOX2UI_ARRANGE_PGT_TRAY2, TRUE );
			}else{
				BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
			}
		}
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN) );

	case CURSORMOVE_CURSOR_ON:			// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;

	case CURSORMOVE_NONE:						// ����Ȃ�
		break;

	case BOX2UI_ARRANGE_PGT_TRAY1:	// 33: �g���C�A�C�R��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		BOX2MAIN_InitTrayIconScroll( syswk, -1 );
		return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollDown, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN );

	case BOX2UI_ARRANGE_PGT_TRAY6:	// 38: �g���C�A�C�R��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		BOX2MAIN_InitTrayIconScroll( syswk, 1 );
		return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollUp, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN );

	case BOX2UI_ARRANGE_PGT_TRAY2:	// 34: �g���C�A�C�R��
	case BOX2UI_ARRANGE_PGT_TRAY3:	// 35: �g���C�A�C�R��
	case BOX2UI_ARRANGE_PGT_TRAY4:	// 36: �g���C�A�C�R��
	case BOX2UI_ARRANGE_PGT_TRAY5:	// 37: �g���C�A�C�R��
		BOX2OBJ_ChangeTrayName( syswk, ret-BOX2UI_ARRANGE_PGT_TRAY2, TRUE );
		// �擾��
		if( syswk->poke_get_key == 1 ){
			return PokeMovePutKey( syswk, BOX2OBJ_POKEICON_PUT_MAX+ret-BOX2UI_ARRANGE_PGT_TRAY2, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN );
		// ���擾
		}else{
			PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
			BOX2MAIN_PokeInfoOff( syswk );
			syswk->app->chg_tray_pos = syswk->box_mv_pos + ret - BOX2UI_ARRANGE_PGT_TRAY2;
			if( syswk->app->chg_tray_pos >= syswk->trayMax ){
				syswk->app->chg_tray_pos -= syswk->trayMax;
			}
			if( syswk->app->chg_tray_pos != syswk->tray ){
				return BoxMoveTrayScrollSet( syswk, 0 );
			}
		}
		break;

	default:
		BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
		// �擾��
		if( syswk->poke_get_key == 1 ){
			return PokeMovePutKey( syswk, ret, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN );
		// ���擾
		}else{
			if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
				syswk->get_tray = syswk->tray;	// �擾�����{�b�N�X
				syswk->get_pos  = ret;
				BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
				return PokeMoveGetKey( syswk, syswk->get_pos );
			}
		}
		break;
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u���ށv�I������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_POKEGET_EXIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeGetExit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );
		if( CURSORMOVE_PosGet(syswk->app->cmwk) >= BOX2UI_ARRANGE_PGT_NAME ){
			BOX2MAIN_PokeInfoOff( syswk );
		}
		syswk->app->sub_seq++;
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		return BoxMoveFrmOutSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_EXIT );

	case 1:
		if( syswk->get_start_mode == 1 ){
			BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
			BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
			syswk->app->sub_seq++;
			return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_EXIT );
		}else{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos >= BOX2UI_ARRANGE_PGT_NAME ){
				pos = 0;
			}
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MAIN, pos );
			BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );
			BOX2MAIN_PokeInfoPut( syswk, pos );
			syswk->app->sub_seq = 0;
			return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		}
		break;

	case 2:
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
		syswk->app->sub_seq = 0;
		return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_EXIT;
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_EXIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�莝���E�g���C����ւ���i�L�[����j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_POKEGET_DATA_CHANGE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeGetDataChange( BOX2_SYS_WORK * syswk )
{
	u8	pos;
	u8	tb_status;

	BOX2MAIN_PokeDataMove( syswk );

	syswk->poke_get_key = 0;
	syswk->get_tray = BOX2MAIN_GETPOS_NONE;

	if( syswk->app->mv_err_code == BOX2MAIN_ERR_CODE_NONE ){
		pos = CURSORMOVE_PosGet( syswk->app->cmwk );
	}else{
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
		BOX2UI_PutHandCursor( syswk, syswk->get_pos );
		BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
		pos = syswk->get_pos;
	}
	syswk->app->old_cur_pos = pos;

	PokeMoveWorkFree( syswk );

	// �g���C�ړ��\����
	if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == TRUE ){
		// �莝�����[�h�ɖ߂�ꍇ
		if( syswk->next_seq == BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN ){
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, pos );
			BOX2MAIN_PokeInfoPut( syswk, pos );
			syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN;

			if( BOX2MAIN_PokeParaGet(syswk,pos,syswk->tray,ID_PARA_poke_exist,NULL) != 0 ){
				tb_status = BOX2OBJ_TB_ICON_ON;
			}else{
				tb_status = BOX2OBJ_TB_ICON_PASSIVE;
			}
		}else{
			if( pos < BOX2OBJ_POKEICON_TRAY_MAX &&
					BOX2MAIN_PokeParaGet(syswk,pos,syswk->tray,ID_PARA_poke_exist,NULL) != 0 ){
				BOX2MAIN_PokeInfoPut( syswk, pos );
				tb_status = BOX2OBJ_TB_ICON_ON;
			}else{
				BOX2MAIN_PokeInfoOff( syswk );
				tb_status = BOX2OBJ_TB_ICON_PASSIVE;
			}
		}
	// �莝���\����
	}else if( BOX2BGWFRM_CheckPartyPokeFrameRight(syswk->app->wfrm) == TRUE ){
		// �g���C���[�h�ɖ߂�ꍇ
		if( syswk->next_seq == BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN ){
			pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos > BOX2UI_ARRANGE_PGT_RIGHT ){
				pos = syswk->get_pos;
			}
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY_MOVE, pos );
			BOX2MAIN_PokeInfoPut( syswk, pos );
			syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN;

			if( BOX2MAIN_PokeParaGet(syswk,pos,syswk->tray,ID_PARA_poke_exist,NULL) != 0 ){
				tb_status = BOX2OBJ_TB_ICON_ON;
			}else{
				tb_status = BOX2OBJ_TB_ICON_PASSIVE;
			}
		}else{
			if( pos < BOX2OBJ_POKEICON_PUT_MAX &&
					BOX2MAIN_PokeParaGet(syswk,pos,syswk->tray,ID_PARA_poke_exist,NULL) != 0 ){
				BOX2MAIN_PokeInfoPut( syswk, pos );
				tb_status = BOX2OBJ_TB_ICON_ON;
			}else{
				BOX2MAIN_PokeInfoOff( syswk );
				tb_status = BOX2OBJ_TB_ICON_PASSIVE;
			}
		}
	// ���̑�
	}else{
		if( pos < BOX2OBJ_POKEICON_PUT_MAX &&
				BOX2MAIN_PokeParaGet(syswk,pos,syswk->tray,ID_PARA_poke_exist,NULL) != 0 ){
			BOX2MAIN_PokeInfoPut( syswk, pos );
			tb_status = BOX2OBJ_TB_ICON_ON;
		}else{
			BOX2MAIN_PokeInfoOff( syswk );
			tb_status = BOX2OBJ_TB_ICON_PASSIVE;
		}
	}

	// �ʏ펞�͑��I������
	if( syswk->mv_cnv_mode == 0 ){
		syswk->mv_cnv_end = 1;
		tb_status = syswk->tb_status_btn;
	}

	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, tb_status );

	switch( syswk->app->mv_err_code ){
	case BOX2MAIN_ERR_CODE_MAIL:			// ���[���������Ă���
		PMSND_PlaySE( SE_BOX2_WARNING );
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BMP_PutPokeMoveErrMsg( syswk, BOX2MAIN_ERR_CODE_MAIL, BOX2BMPWIN_ID_MSG1 );
		return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_DATA_CHANGE_ERR;

	case BOX2MAIN_ERR_CODE_BATTLE:		// �킦��|�P���������Ȃ��Ȃ�
		PMSND_PlaySE( SE_BOX2_WARNING );
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BMP_PutPokeMoveErrMsg( syswk, BOX2MAIN_ERR_CODE_BATTLE, BOX2BMPWIN_ID_MSG1 );
		return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_DATA_CHANGE_ERR;

	case BOX2MAIN_ERR_CODE_EGG:				// �^�}�S�͂���ׂȂ��i�o�g���{�b�N�X��p�j
		PMSND_PlaySE( SE_BOX2_WARNING );
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BMP_PutPokeMoveErrMsg( syswk, BOX2MAIN_ERR_CODE_EGG, BOX2BMPWIN_ID_MSG1 );
		return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_DATA_CHANGE_ERR;

	case BOX2MAIN_ERR_CODE_ROCK:			// ���b�N����Ă���i�o�g���{�b�N�X��p�j
		PMSND_PlaySE( SE_BOX2_WARNING );
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BMP_BattleBoxRockMsgPut( syswk, 0 );
		return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_DATA_CHANGE_ERR;
	}

	if( syswk->dat->callMode == BOX_MODE_SEIRI ){
		if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == TRUE ){
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
		}else{
			BOX2BGWFRM_BoxListButtonOn( syswk->app );
		}
	}

	return syswk->next_seq;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�|�P�����z�u���̃G���[����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_POKEGET_DATA_CHANGE_ERR
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeGetDataChangeErr( BOX2_SYS_WORK * syswk )
{
	if( MainSeq_TrgWait( syswk ) == BOX2SEQ_MAINSEQ_TRGWAIT ){
		return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_DATA_CHANGE_ERR;
	}

	if( CURSORMOVE_CursorOnOffGet( syswk->app->cmwk ) == TRUE ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );
	}

	// ���b�N����Ă���i�o�g���{�b�N�X��p�j
	if( syswk->app->mv_err_code == BOX2MAIN_ERR_CODE_ROCK ){
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG4 );
	}else{
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	}
	BOX2MAIN_ResetTouchBar( syswk );

	if( syswk->dat->callMode == BOX_MODE_SEIRI ){
		if( BOX2BGWFRM_CheckBoxMoveFrm( syswk->app->wfrm ) == TRUE ){
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
		}else{
			BOX2BGWFRM_BoxListButtonOn( syswk->app );
		}
	}

	syswk->app->mv_err_code = BOX2MAIN_ERR_CODE_NONE;

	return syswk->next_seq;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�g���C�́u���ށv����莝���́u���ށv�ւ��肩��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_POKEGET_PARTY_CHANGE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeGetPartyChange( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		syswk->app->sub_seq++;
		return BoxMoveFrmOutSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_PARTY_CHANGE );

	case 1:
		BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
		BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
		if( syswk->poke_get_key == 1 ){
			BOX2OBJ_PartyPokeIconFrmInSet( syswk, 1 );
		}else{
			BOX2OBJ_PartyPokeIconFrmInSet( syswk, 0 );
		}
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_PARTY_CHANGE );

	case 2:
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY_MOVE, BOX2UI_ARRANGE_PTGT_PARTY_POKE );
		BOX2UI_SetTouchBarIconPokeGet( syswk, BOX2UI_ARRANGE_PTGT_PARTY_POKE );
		if( syswk->poke_get_key == 0 ){
			BOX2MAIN_PokeInfoPut( syswk, BOX2UI_ARRANGE_PTGT_PARTY_POKE );
		}
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
		syswk->app->old_cur_pos = BOX2UI_ARRANGE_PTGT_PARTY_POKE;
		syswk->app->sub_seq = 0;
		return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN;
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_PARTY_CHANGE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�莝���I����������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_PARTY_INIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyInit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2MAIN_ResetTouchBar( syswk );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );

		BOX2BGWFRM_PartyPokeFrameInitPutLeft( syswk->app->wfrm );
		BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
		BOX2OBJ_PartyPokeIconFrmInSet( syswk, 0 );
		if( syswk->dat->callMode == BOX_MODE_ITEM ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
		}
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_INIT );

	case 1:
		syswk->app->sub_seq = 0;
		BOX2MAIN_PokeInfoPut( syswk, BOX2OBJ_POKEICON_TRAY_MAX );
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
		if( syswk->dat->callMode == BOX_MODE_ITEM ){
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_PARTY, BOX2UI_ITEM_PARTY_POKE1 );
			return BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN;
		}
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_POKE1 );
		return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_INIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�莝���I�����C��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	// �Z���N�g�{�^��
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		syswk->get_start_mode = 1;
		return ChgSeqSelectButton( syswk, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_INIT );
	}

	// �^�b�`�擾�`�F�b�N
	ret = BOX2UI_HitCheckTrgPartyPokeLeft();
	if( ret != GFL_UI_TP_HIT_NONE ){
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
				BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 6 );
			}
			BOX2MAIN_PokeInfoPut( syswk, ret );
			return PartyPokeGet( syswk, ret );
		}else{
			CURSORMOVE_PosSet( syswk->app->cmwk, ret - BOX2OBJ_POKEICON_TRAY_MAX );
			BOX2UI_PutHandCursor( syswk, ret - BOX2OBJ_POKEICON_TRAY_MAX );
			BOX2MAIN_PokeInfoOff( syswk );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				BOX2OBJ_PokeCursorVanish( syswk, FALSE );
				BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
				BOX2MAIN_ResetTouchBar( syswk );
				BOX2BGWFRM_BoxListButtonOn( syswk->app );
				return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN );
			}else{
				return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
			}
		}
	}

	// �L�[�E�^�b�`�`�F�b�N
	ret = BOX2UI_BoxArrangePartyMoveMain( syswk );

	switch( ret ){
	case BOX2UI_ARRANGE_PARTY_BOXLIST:	// 06:�u�{�b�N�X���X�g�v
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_POKE_BTN, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_END) );

	case BOX2UI_ARRANGE_PARTY_RETURN1:	// 07: �߂�P�i�{�b�N�X��ʏI���j
		PMSND_PlaySE( SE_BOX2_CLOSE );
		syswk->dat->retMode = BOX_END_MODE_C_GEAR;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_END, 8, BOX2SEQ_MAINSEQ_BOXEND_DIRECT );

	case BOX2UI_ARRANGE_PARTY_RETURN2:	// 08: �߂�Q
		PMSND_PlaySE( SE_BOX2_CANCEL );
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_END) );

	case BOX2UI_ARRANGE_PARTY_GET:				// 09: ����
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->get_start_mode = 1;
		syswk->mv_cnv_mode = 0;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU1, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_INIT) );

	case BOX2UI_ARRANGE_PARTY_STATUS:			// 10: �悤�����݂�
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->subProcMode = SUB_PROC_MODE_MENU_PARTY;
		syswk->get_tray    = syswk->tray;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU2, BOX2SEQ_MAINSEQ_STATUS_CALL );

	case BOX2UI_ARRANGE_PARTY_ITEM:				// 11: ��������
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->subProcMode = SUB_PROC_MODE_MENU_PARTY;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU3, BOX2SEQ_MAINSEQ_ITEM_MENU_CHECK );

	case BOX2UI_ARRANGE_PARTY_MARKING:		// 12: �}�[�L���O
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_MARKING_INIT) );

	case BOX2UI_ARRANGE_PARTY_FREE:				// 13: �ɂ���
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_POKEFREE_INIT) );

	case BOX2UI_ARRANGE_PARTY_CLOSE:			// 14: ��߂�
		PMSND_PlaySE( SE_BOX2_CANCEL );
		{
			u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MENU_CLOSE );

	case BOX2UI_ARRANGE_PARTY_CONV:				// 15: �֗����[�h
		syswk->get_start_mode = 1;
		return ChgSeqSelectTouch( syswk, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_INIT );

	case CURSORMOVE_NO_MOVE_UP:			// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_DOWN:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_LEFT:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_RIGHT:	// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NONE:						// ����Ȃ�
		break;

	case CURSORMOVE_CURSOR_ON:			// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_ARRANGE_PARTY_BOXLIST ){
				BOX2MAIN_PokeInfoPut( syswk, pos+BOX2OBJ_POKEICON_TRAY_MAX );
			}else if( pos == BOX2UI_ARRANGE_PARTY_BOXLIST ){
				BOX2MAIN_PokeInfoOff( syswk );
			}
		}
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN) );

	case CURSORMOVE_CANCEL:							// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
			u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MENU_CLOSE );
		}
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_END) );

	default:
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_DECIDE );
			BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 6 );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_ARRANGE_PARTY_GET, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_PARTY_GET );
			return PartyPokeGetKey( syswk, ret, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN );
		}
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�莝���I���I������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_PARTY_END
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyEnd( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2MAIN_ResetTouchBar( syswk );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_END );

	case 1:
		syswk->app->sub_seq = 0;
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		if( syswk->dat->callMode == BOX_MODE_ITEM ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_MAIN, BOX2UI_ITEM_MAIN_PARTY );
			return BOX2SEQ_MAINSEQ_ITEM_MAIN;
		}
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MAIN, BOX2UI_ARRANGE_MAIN_PARTY );
		return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_END;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���j���[�����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_PARTY_MENU_CLOSE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyMenuClose( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	BOX2BGWFRM_BoxListButtonOn( syswk->app );
	return VFuncSet( syswk, BOX2MAIN_VFuncMenuMove, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�莝���|�P��������ւ���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_PARTYPOKE_CHG_END
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyPokeChgEnd( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_POKEMOVE_WORK * work;
	u32	pos;

	work = syswk->app->vfunk.work;
	pos  = work->get_pos;

	PMSND_PlaySE( SE_BOX2_POKE_PUT );
	BOX2MAIN_PokeDataMove( syswk );
	PokeMoveWorkFree( syswk );
	CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_PARTY_GET );
	BOX2UI_PutHandCursor( syswk, BOX2UI_ARRANGE_PARTY_GET );
	BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );
	BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�莝���u���ށv��������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_INIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyPokeGetInit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		// �Z���N�g�{�^��
		if( syswk->mv_cnv_mode == 1 ){
			syswk->get_tray = BOX2MAIN_GETPOS_NONE;	// �擾�����{�b�N�X
			syswk->get_pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == TRUE ){
				if( syswk->get_pos >= TEMOTI_POKEMAX ){
					syswk->get_pos = BOX2OBJ_POKEICON_TRAY_MAX;
				}else{
					syswk->get_pos += BOX2OBJ_POKEICON_TRAY_MAX;
				}
			}else{
				if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
					syswk->get_pos = 0;
				}
			}
			BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
		}else{
			if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == TRUE ){
				syswk->get_tray = BOX2MAIN_GETPOS_NONE;	// �擾�����{�b�N�X
			}else{
				syswk->get_tray = syswk->tray;					// �擾�����{�b�N�X
			}
			BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
			BOX2MAIN_ResetTouchBar( syswk );
			BOX2BGWFRM_BoxListButtonOn( syswk->app );
			BOX2OBJ_PokeCursorVanish( syswk, FALSE );
		}
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_INIT );

	case 1:
		// �o�g���{�b�N�X�����b�N����Ă���Ƃ�
		if( syswk->dat->callMode == BOX_MODE_BATTLE && syswk->dat->bbRockFlg == TRUE ){
			PMSND_PlaySE( SE_BOX2_WARNING );
			BOX2BMP_BattleBoxRockMsgPut( syswk, 0 );
			syswk->mv_cnv_mode  = 0;
			syswk->app->sub_seq = 0;
			syswk->next_seq     = BOX2SEQ_MAINSEQ_BATTLEBOX_ROCK;
			return BOX2SEQ_MAINSEQ_TRGWAIT;
		}

		syswk->app->sub_seq++;
		if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == TRUE ){
			BOX2BGWFRM_PartyPokeFrameRightMoveSet( syswk->app->wfrm );
		}else{
			BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
			BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
			BOX2OBJ_PartyPokeIconFrmInSet( syswk, 0 );
		}
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_INIT );

	case 2:
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_ON );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY_MOVE, syswk->get_pos );
		BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
		syswk->app->old_cur_pos = syswk->get_pos;
		syswk->app->sub_seq = 0;
		// �Z���N�g�{�^��
		if( syswk->mv_cnv_mode == 1 ){
			if( BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, syswk->tray, ID_PARA_poke_exist, NULL ) == 0 ){
				BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_PASSIVE );
			}
			if( syswk->dat->callMode == BOX_MODE_BATTLE ){
				BOX2BGWFRM_TemochiButtonOff( syswk->app );
				BOX2BGWFRM_BoxListButtonOff( syswk->app );
				CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_BOXLIST );
			}
			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_BLUE, TRUE );
			return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN;
		}else if( syswk->mv_cnv_mode == 2 ){
			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_BLUE, TRUE );
		}else{
			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );
		}
		BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
		return PartyPokeMoveGetKey( syswk, syswk->get_pos );
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_INIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�莝���u���ށv���C������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyPokeGetMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	if( syswk->mv_cnv_end == 1 ){
		syswk->mv_cnv_mode = 0;
		syswk->mv_cnv_end = 0;
		return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_EXIT );
	}

	// �Z���N�g�{�^��
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		if( syswk->mv_cnv_mode != 0 && syswk->poke_get_key == 0 ){
			BOX2MAIN_ChgCursorButton( syswk );
			syswk->mv_cnv_mode = 0;
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_EXIT );
		}
	}

	// �|�P�����h���b�O�`�F�b�N
	if( syswk->poke_get_key == 0 ){
		// �g���C�̃|�P����
		ret = BOX2UI_HitCheckTrgTrayPoke();
		if( ret != GFL_UI_TP_HIT_NONE ){
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
			if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
				PMSND_PlaySE( SE_BOX2_POKE_CATCH );
				GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
				BOX2MAIN_PokeInfoPut( syswk, ret );
				return BoxPartyPokeGet( syswk, ret, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN );
			}else{
				CURSORMOVE_PosSet( syswk->app->cmwk, ret );
				BOX2UI_SetTouchBarIconPokeGet( syswk, ret );
				syswk->app->old_cur_pos = ret;
				BOX2UI_PutHandCursor( syswk, ret );
				BOX2MAIN_PokeInfoOff( syswk );
			}
			return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN;
		}
		// �莝���̃|�P����
		ret = BOX2UI_HitCheckTrgPartyPokeRight();
		if( ret != GFL_UI_TP_HIT_NONE ){
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
			ret += BOX2OBJ_POKEICON_TRAY_MAX;
			if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
				PMSND_PlaySE( SE_BOX2_POKE_CATCH );
				GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
				BOX2MAIN_PokeInfoPut( syswk, ret );
				return BoxPartyPokeGet( syswk, ret, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN );
			}else{
				CURSORMOVE_PosSet( syswk->app->cmwk, ret );
				BOX2UI_SetTouchBarIconPokeGet( syswk, ret );
				syswk->app->old_cur_pos = ret;
				BOX2UI_PutHandCursor( syswk, ret );
				BOX2MAIN_PokeInfoOff( syswk );
			}
			return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN;
		}
	}

	// �L�[�E�^�b�`�`�F�b�N
	ret = BOX2UI_ArrangePartyPokeGetMain( syswk );

	switch( ret ){
	case BOX2UI_ARRANGE_PTGT_NAME:		// 36: �{�b�N�X��
		// ���擾
		if( syswk->poke_get_key == 0 ){
			PMSND_PlaySE( SE_BOX2_DECIDE );
			BOX2MAIN_PokeInfoOff( syswk );
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_BOXTHEMA_INIT );
		}
		break;

	case BOX2UI_ARRANGE_PTGT_LEFT:		// 37: �{�b�N�X�؂�ւ����i���j
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_ARRANGE_PTGT_NAME );
		BOX2UI_SetTouchBarIconPokeGet( syswk, BOX2UI_ARRANGE_PTGT_NAME );
		syswk->app->old_cur_pos = BOX2UI_ARRANGE_PTGT_NAME;
		// ���擾
		if( syswk->poke_get_key == 0 ){
			BOX2MAIN_PokeSelectOff( syswk );
		// �擾��
		}else{
			BOX2OBJ_MovePokeIconHand( syswk );
		}
		return TrayLeftScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN );

	case BOX2UI_ARRANGE_PTGT_RIGHT:		// 38: �{�b�N�X�؂�ւ����i�E�j
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_ARRANGE_PTGT_NAME );
		BOX2UI_SetTouchBarIconPokeGet( syswk, BOX2UI_ARRANGE_PTGT_NAME );
		syswk->app->old_cur_pos = BOX2UI_ARRANGE_PTGT_NAME;
		// ���擾
		if( syswk->poke_get_key == 0 ){
			BOX2MAIN_PokeSelectOff( syswk );
		// �擾��
		}else{
			BOX2OBJ_MovePokeIconHand( syswk );
		}
		return TrayRightScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN );

	case BOX2UI_ARRANGE_PTGT_BOXLIST:	// 39: �{�b�N�X���X�g
		PMSND_PlaySE( SE_BOX2_DECIDE );
		if( syswk->poke_get_key == 0 ){
			BOX2MAIN_PokeInfoOff( syswk );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_BOXLIST_BTN, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_BOXLIST_CHANGE) );

	case BOX2UI_ARRANGE_PTGT_STATUS:		// 40: �X�e�[�^�X
		if( syswk->poke_get_key == 0 ){
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos >= BOX2OBJ_POKEICON_PUT_MAX ){
				break;
			}else{
				if( BOX2MAIN_PokeParaGet( syswk, pos, syswk->tray, ID_PARA_poke_exist, NULL ) == 0 ){
					break;
				}
			}
			syswk->get_tray = syswk->tray;
			syswk->get_pos  = pos;
			syswk->subProcMode = SUB_PROC_MODE_TB_PARTY;
		}else{
			syswk->subProcMode = SUB_PROC_MODE_TB_PARTY_GET;
			syswk->tb_status_pos = CURSORMOVE_PosGet( syswk->app->cmwk );
		}
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_STATUS, 1, BOX2SEQ_MAINSEQ_STATUS_CALL );

	case BOX2UI_ARRANGE_PTGT_RET:		// 41: �߂�
		if( syswk->poke_get_key == 0 ){
			// �{�b�N�X�������C����
			PMSND_PlaySE( SE_BOX2_CANCEL );
			syswk->mv_cnv_mode = 0;
			return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_EXIT) );
		}
		break;

	case BOX2UI_ARRANGE_PTGT_CONV:	// 42: �֗����[�h
		if( syswk->mv_cnv_mode != 0 && syswk->poke_get_key == 0 ){
			syswk->mv_cnv_mode = 0;
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_EXIT );
		}
		break;

	case CURSORMOVE_CANCEL:					// �L�����Z��
		if( syswk->poke_get_key == 1 ){
			// �擾���Ȃ̂Ŏ��s
			return PokeMovePutKey( syswk, BOX2MAIN_GETPOS_NONE, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN );
		}else{
			// �{�b�N�X�������C����
			PMSND_PlaySE( SE_BOX2_CANCEL );
			syswk->mv_cnv_mode = 0;
			return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_EXIT) );
		}
		break;

	case CURSORMOVE_NO_MOVE_LEFT:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_ARRANGE_PTGT_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			return TrayLeftScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_RIGHT:	// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_ARRANGE_PTGT_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			return TrayRightScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_UP:			// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_DOWN:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NONE:						// ����Ȃ�
		break;

	case CURSORMOVE_CURSOR_ON:			// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		if( syswk->poke_get_key == 0 ){
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2OBJ_POKEICON_PUT_MAX ){
				BOX2MAIN_PokeInfoPut( syswk, pos );
			}else{
				BOX2MAIN_PokeInfoOff( syswk );
			}
		}
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN) );

	default:
		// �擾��
		if( syswk->poke_get_key == 1 ){
			return PokeMovePutKey( syswk, ret, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN );
		// ���擾
		}else{
			if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
				syswk->get_tray = syswk->tray;	// �擾�����{�b�N�X
				syswk->get_pos  = ret;
				BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
				return PartyPokeMoveGetKey( syswk, syswk->get_pos );
			}
		}
		break;
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�莝���u���ށv�I������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_EXIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyPokeGetExit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );
		if( syswk->get_start_mode == 0 ){
			BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
			syswk->app->sub_seq = 2;
		}else{
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
			BOX2BGWFRM_PartyPokeFrameLeftMoveSet( syswk->app->wfrm );
			syswk->app->sub_seq = 1;
		}
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_EXIT );

	case 1:
		syswk->app->sub_seq = 0;
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2OBJ_POKEICON_TRAY_MAX || pos >= BOX2OBJ_POKEICON_PUT_MAX ){
				pos = 0;
			}else{
				pos -= BOX2OBJ_POKEICON_TRAY_MAX;
			}
			BOX2MAIN_PokeInfoPut( syswk, pos+BOX2OBJ_POKEICON_TRAY_MAX );
			BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );
			if( syswk->dat->callMode == BOX_MODE_SEIRI ){
				BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, pos );
				return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
			}else{
				BOX2BGWFRM_BoxListButtonOn( syswk->app );
				BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BATTLEBOX_PARTY, pos );
				return BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN;
			}
		}

	case 2:
		syswk->app->sub_seq = 0;
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
				pos = 0;
			}
			BOX2MAIN_PokeInfoPut( syswk, pos );
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
			BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );
			if( syswk->dat->callMode == BOX_MODE_BATTLE ){
				BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BATTLEBOX_MAIN, pos );
				return BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;
			}else{
				BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MAIN, pos );
				return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
			}
		}
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_EXIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�莝���́u���ށv����g���C�́u���ށv�ւ��肩��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_BOXLIST_CHANGE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyPokeGetBoxListChange( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_BOXLIST_CHANGE );

	case 1:
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		syswk->box_mv_pos = syswk->tray;	// ���ݎQ�Ƃ��Ă���{�b�N�X
		syswk->app->sub_seq++;
		return BoxMoveFrmInSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_BOXLIST_CHANGE );

	case 2:
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, BOX2UI_ARRANGE_PGT_PARTY );
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		syswk->app->old_cur_pos = BOX2UI_ARRANGE_PGT_PARTY;
		syswk->app->sub_seq = 0;
		return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN;
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_BOXLIST_CHANGE;
}


//============================================================================================
//	�o�g���{�b�N�X
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�o�g���{�b�N�X���C��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BATTLEBOX_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BattleBoxMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	// �Z���N�g�{�^��
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		syswk->get_start_mode = 0;
		return ChgSeqSelectButton( syswk, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_INIT );
	}

	ret = BOX2UI_HitCheckTrgTrayPoke();
	if( ret != GFL_UI_TP_HIT_NONE ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
				BOX2BMP_MenuStrPrint( syswk, BattleBoxMenuStrTbl, 4 );
			}
			BOX2MAIN_PokeInfoPut( syswk, ret );
			syswk->get_start_mode = 0;
			return GetBattleBoxTrayPoke( syswk, ret );
		}else{
			CURSORMOVE_PosSet( syswk->app->cmwk, ret );
			BOX2UI_PutHandCursor( syswk, ret );
			BOX2MAIN_PokeInfoOff( syswk );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
				u8	pos = syswk->get_pos;
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				BOX2OBJ_PokeCursorVanish( syswk, FALSE );
				BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
				BOX2MAIN_ResetTouchBar( syswk );
				BOX2BGWFRM_TemochiButtonOn( syswk->app );
				return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN );
			}else{
				return BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;
			}
		}
	}

	ret = BOX2UI_BattleBoxMain( syswk );

	switch( ret ){
	case BOX2UI_BATTLEBOX_MAIN_NAME:		// 30: �{�b�N�X��
		PMSND_PlaySE( SE_BOX2_DECIDE );
		BOX2MAIN_PokeInfoOff( syswk );
		return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_BOXTHEMA_INIT );

	case BOX2UI_BATTLEBOX_MAIN_LEFT:		// 31: �g���C�؂�ւ����E��
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_BATTLEBOX_MAIN_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_BATTLEBOX_MAIN_NAME );
		return TrayLeftScrollSet( syswk, TRUE, BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN );

	case BOX2UI_BATTLEBOX_MAIN_RIGHT:		// 32: �g���C�؂�ւ����E�E
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_BATTLEBOX_MAIN_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_BATTLEBOX_MAIN_NAME );
		return TrayRightScrollSet( syswk, TRUE, BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN );

	case BOX2UI_BATTLEBOX_MAIN_PARTY:		// 33: �o�g���{�b�N�X
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_POKE_BTN, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_INIT) );

	case BOX2UI_BATTLEBOX_MAIN_RETURN1:	// 34: �߂�P
		PMSND_PlaySE( SE_BOX2_CLOSE );
		syswk->dat->retMode = BOX_END_MODE_C_GEAR;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_END, 8, BOX2SEQ_MAINSEQ_BOXEND_DIRECT );

	case BOX2UI_BATTLEBOX_MAIN_RETURN2:	// 35: �߂�Q
		PMSND_PlaySE( SE_BOX2_CANCEL );
		syswk->dat->retMode = BOX_END_MODE_MENU;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case BOX2UI_BATTLEBOX_MAIN_GET:			// 36: ���ǂ�����
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->get_start_mode = 0;
		syswk->mv_cnv_mode = 0;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU3, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_INIT) );

	case BOX2UI_BATTLEBOX_MAIN_STATUS:	// 37: �悤�����݂�
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->subProcMode = SUB_PROC_MODE_MENU_BOX;
		syswk->get_tray    = syswk->tray;
		syswk->cur_rcv_pos = BOX2UI_BATTLEBOX_MAIN_STATUS;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, BOX2SEQ_MAINSEQ_STATUS_CALL );

	case BOX2UI_BATTLEBOX_MAIN_ITEM:		// 38: ��������
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->subProcMode = SUB_PROC_MODE_MENU_BOX;
		syswk->cur_rcv_pos = BOX2UI_BATTLEBOX_MAIN_ITEM;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_ITEM_MENU_CHECK );

	case BOX2UI_BATTLEBOX_MAIN_CLOSE:		// 39: ��߂�
		PMSND_PlaySE( SE_BOX2_CANCEL );
		{
			u8	pos = syswk->get_pos;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_BATTLEBOX_MENU_CLOSE );

	case BOX2UI_BATTLEBOX_MAIN_CONV:		// 40: �֗����[�h
		syswk->get_start_mode = 0;
		return ChgSeqSelectTouch( syswk, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_INIT );

	case CURSORMOVE_NO_MOVE_LEFT:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_BATTLEBOX_MAIN_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			BOX2MAIN_PokeSelectOff( syswk );
			return TrayLeftScrollSet( syswk, TRUE, BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_RIGHT:	// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_BATTLEBOX_MAIN_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			BOX2MAIN_PokeSelectOff( syswk );
			return TrayRightScrollSet( syswk, TRUE, BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_UP:			// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_DOWN:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NONE:						// ����Ȃ�
		break;

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_ARRANGE_MAIN_NAME ){
				BOX2MAIN_PokeInfoPut( syswk, pos );
			}else{
				if( pos != BOX2UI_BATTLEBOX_MAIN_GET &&
						pos != BOX2UI_BATTLEBOX_MAIN_STATUS &&
						pos != BOX2UI_BATTLEBOX_MAIN_ITEM &&
						pos != BOX2UI_BATTLEBOX_MAIN_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN) );

	case CURSORMOVE_CANCEL:				// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		// ���j���[�\����
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
			u8	pos = syswk->get_pos;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_BATTLEBOX_MENU_CLOSE );
		}
		syswk->dat->retMode = BOX_END_MODE_MENU;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;

	default:
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_DECIDE );
			BOX2BMP_MenuStrPrint( syswk, BattleBoxMenuStrTbl, 4 );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_BATTLEBOX_MAIN_GET, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_BATTLEBOX_MAIN_GET );
			return TrayPokeGetKey( syswk, ret, BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN );
		}
	}

	return BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�g���C�̃|�P�����ړ���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BATTLEBOX_TRAYPOKE_CHG_END
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BattleBoxTrayPokeChgEnd( BOX2_SYS_WORK * syswk )
{
	PMSND_PlaySE( SE_BOX2_POKE_PUT );
	BOX2MAIN_PokeDataMove( syswk );
	PokeMoveWorkFree( syswk );
	CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_BATTLEBOX_MAIN_GET );
	BOX2UI_PutHandCursor( syswk, BOX2UI_BATTLEBOX_MAIN_GET );
	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );
	BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
	return BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�o�g���{�b�N�X�v���j���[�����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BATTLEBOX_MENU_CLOSE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BattleBoxMenuClose( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	BOX2BGWFRM_TemochiButtonOn( syswk->app );
	return VFuncSet( syswk, BOX2MAIN_VFuncMenuMove, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�o�g���{�b�N�X�v�莝���I����������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_INIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BattleBoxPartyInit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2MAIN_ResetTouchBar( syswk );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );

		BOX2BGWFRM_PartyPokeFrameInitPutLeft( syswk->app->wfrm );
		BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
		BOX2OBJ_PartyPokeIconFrmInSet( syswk, 0 );
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_INIT );

	case 1:
		syswk->app->sub_seq = 0;
		BOX2MAIN_PokeInfoPut( syswk, BOX2OBJ_POKEICON_TRAY_MAX );
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BATTLEBOX_PARTY, BOX2UI_BATTLEBOX_PARTY_POKE1 );
		return BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN;
	}

	return BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_INIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�o�g���{�b�N�X�v�莝���I�����C������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BattleBoxPartyMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	// �Z���N�g�{�^��
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		syswk->get_start_mode = 1;
		return ChgSeqSelectButton( syswk, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_INIT );
	}

	// �^�b�`�擾�`�F�b�N
	ret = BOX2UI_HitCheckTrgPartyPokeLeft();
	if( ret != GFL_UI_TP_HIT_NONE ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
				BOX2BMP_MenuStrPrint( syswk, BattleBoxMenuStrTbl, 4 );
			}
			BOX2MAIN_PokeInfoPut( syswk, ret );
			return GetBattleBoxPartyPoke( syswk, ret );
		}else{
			CURSORMOVE_PosSet( syswk->app->cmwk, ret - BOX2OBJ_POKEICON_TRAY_MAX );
			BOX2UI_PutHandCursor( syswk, ret - BOX2OBJ_POKEICON_TRAY_MAX );
			BOX2MAIN_PokeInfoOff( syswk );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				BOX2OBJ_PokeCursorVanish( syswk, FALSE );
				BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
				BOX2MAIN_ResetTouchBar( syswk );
				BOX2BGWFRM_BoxListButtonOn( syswk->app );
				return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN );
			}else{
				return BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN;
			}
		}
	}

	// �L�[�E�^�b�`�`�F�b�N
	ret = BOX2UI_BattleBoxPartyMain( syswk );

	switch( ret ){
	case BOX2UI_BATTLEBOX_PARTY_BOXLIST:		// 06:�u�{�b�N�X���X�g�v
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_POKE_BTN, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_END) );

	case BOX2UI_BATTLEBOX_PARTY_RETURN1:		// 07: �߂�P
		PMSND_PlaySE( SE_BOX2_CLOSE );
		syswk->dat->retMode = BOX_END_MODE_C_GEAR;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_END, 8, BOX2SEQ_MAINSEQ_BOXEND_DIRECT );

	case BOX2UI_BATTLEBOX_PARTY_RETURN2:		// 08: �߂�Q
		PMSND_PlaySE( SE_BOX2_CANCEL );
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_END) );

	case BOX2UI_BATTLEBOX_PARTY_GET:				// 09: ���ǂ�����
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->get_start_mode = 1;
		syswk->mv_cnv_mode = 0;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU3, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_INIT) );

	case BOX2UI_BATTLEBOX_PARTY_STATUS:			// 10: �悤�����݂�
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->subProcMode = SUB_PROC_MODE_MENU_PARTY;
		syswk->get_tray    = syswk->tray;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, BOX2SEQ_MAINSEQ_STATUS_CALL );

	case BOX2UI_BATTLEBOX_PARTY_ITEM:				// 11: ��������
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->subProcMode = SUB_PROC_MODE_MENU_PARTY;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_ITEM_MENU_CHECK );

	case BOX2UI_BATTLEBOX_PARTY_CLOSE:			// 12: ��߂�
		PMSND_PlaySE( SE_BOX2_CANCEL );
		{
			u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MENU_CLOSE );

	case BOX2UI_BATTLEBOX_PARTY_CONV:				// 13: �֗����[�h
		syswk->get_start_mode = 1;
		return ChgSeqSelectTouch( syswk, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_INIT );

	case CURSORMOVE_CANCEL:							// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		// ���j���[�\����
		if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
			u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MENU_CLOSE );
		}
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_END) );

	case CURSORMOVE_NO_MOVE_UP:			// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_DOWN:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_LEFT:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_RIGHT:	// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NONE:						// ����Ȃ�
		break;

	case CURSORMOVE_CURSOR_ON:			// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_BATTLEBOX_PARTY_BOXLIST ){
				BOX2MAIN_PokeInfoPut( syswk, pos+BOX2OBJ_POKEICON_TRAY_MAX );
			}else if( pos == BOX2UI_BATTLEBOX_PARTY_BOXLIST ){
				BOX2MAIN_PokeInfoOff( syswk );
			}
		}
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN) );

	default:
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_DECIDE );
			BOX2BMP_MenuStrPrint( syswk, BattleBoxMenuStrTbl, 4 );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_BATTLEBOX_PARTY_GET, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_BATTLEBOX_PARTY_GET );
			return PartyPokeGetKey( syswk, ret, BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN );
		}
	}

	return BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�o�g���{�b�N�X�v�莝���I���I������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_END
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BattleBoxPartyEnd( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2MAIN_ResetTouchBar( syswk );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_END );

	case 1:
		syswk->app->sub_seq = 0;
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BATTLEBOX_MAIN, BOX2UI_BATTLEBOX_MAIN_PARTY );
		return BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;
	}

	return BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_END;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�o�g���{�b�N�X�v�莝�����j���[�����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MENU_CLOSE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BattleBoxPartyMenuClose( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	BOX2BGWFRM_BoxListButtonOn( syswk->app );
	return VFuncSet( syswk, BOX2MAIN_VFuncMenuMove, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�莝���|�P��������ւ���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BATTLEBOX_PARTYPOKE_CHG_END
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BattleBoxPartyPokeChgEnd( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_POKEMOVE_WORK * work;
	u32	pos;

	work = syswk->app->vfunk.work;
	pos  = work->get_pos;

	BOX2MAIN_PokeDataMove( syswk );
	PokeMoveWorkFree( syswk );
	CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_BATTLEBOX_PARTY_GET );
	BOX2UI_PutHandCursor( syswk, BOX2UI_BATTLEBOX_PARTY_GET );
	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );
	BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
	return BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�o�g���{�b�N�X�����b�N����Ă���Ƃ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BATTLEBOX_ROCK
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BattleBoxRock( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG4 );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2MAIN_ResetTouchBar( syswk );
	if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	}

	if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == TRUE ){
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos-BOX2OBJ_POKEICON_TRAY_MAX );
		BOX2UI_PutHandCursor( syswk, syswk->get_pos-BOX2OBJ_POKEICON_TRAY_MAX );
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
		return BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN;
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
	BOX2UI_PutHandCursor( syswk, syswk->get_pos );
	BOX2BGWFRM_TemochiButtonOn( syswk->app );
	return BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;
}


//============================================================================================
//	�|�P��������Ă���
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�|�P��������Ă����v���C��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PARTYIN_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	ret = BOX2UI_HitCheckTrgTrayPoke();
	if( ret != GFL_UI_TP_HIT_NONE ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
				BOX2BMP_MenuStrPrint( syswk, PartyInMenuStrTbl, 5 );
			}
			BOX2MAIN_PokeInfoPut( syswk, ret );
			return TrayPokeGetPartyIn( syswk, ret );
		}else{
			CURSORMOVE_PosSet( syswk->app->cmwk, ret );
			BOX2UI_PutHandCursor( syswk, ret );
			BOX2MAIN_PokeInfoOff( syswk );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				BOX2OBJ_PokeCursorVanish( syswk, FALSE );
				BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
				BOX2MAIN_ResetTouchBar( syswk );
				return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_PARTYIN_MAIN );
			}else{
				return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
			}
		}
	}

	ret = BOX2UI_PartyInMain( syswk );

	switch( ret ){
	case BOX2UI_PTIN_MAIN_NAME:				// 30: �{�b�N�X��
		PMSND_PlaySE( SE_BOX2_DECIDE );
		BOX2MAIN_PokeInfoOff( syswk );
		return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_BOXTHEMA_INIT );

	case BOX2UI_PTIN_MAIN_LEFT:				// 31: �g���C�؂�ւ����E��
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_PTIN_MAIN_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_PTIN_MAIN_NAME );
		return TrayLeftScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_PARTYIN_MAIN );

	case BOX2UI_PTIN_MAIN_RIGHT:			// 32: �g���C�؂�ւ����E�E
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_PTIN_MAIN_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_PTIN_MAIN_NAME );
		return TrayRightScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_PARTYIN_MAIN );

	case BOX2UI_PTIN_MAIN_RETURN1:		// 33: �߂�P
		PMSND_PlaySE( SE_BOX2_CLOSE );
		syswk->dat->retMode = BOX_END_MODE_C_GEAR;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_END, 8, BOX2SEQ_MAINSEQ_BOXEND_DIRECT );

	case BOX2UI_PTIN_MAIN_RETURN2:		// 34: �߂�Q
		PMSND_PlaySE( SE_BOX2_CANCEL );
		syswk->dat->retMode = BOX_END_MODE_MENU;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case BOX2UI_PTIN_MAIN_TSURETEIKU:	// 35: ��Ă���
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU2, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_PARTYIN_SELECT) );

	case BOX2UI_PTIN_MAIN_STATUS:			// 36: �悤�����݂�
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->get_tray    = syswk->tray;
		syswk->cur_rcv_pos = BOX2UI_PTIN_MAIN_STATUS;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU3, BOX2SEQ_MAINSEQ_STATUS_CALL );

	case BOX2UI_PTIN_MAIN_MARKING:		// 37: �}�[�L���O
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_MARKING_INIT) );

	case BOX2UI_PTIN_MAIN_FREE:				// 38: �ɂ���
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_POKEFREE_INIT) );

	case BOX2UI_PTIN_MAIN_CLOSE:			// 39: �Ƃ���
		PMSND_PlaySE( SE_BOX2_CANCEL );
		{
			u8	pos = syswk->get_pos;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_PARTYIN_MENU_CLOSE );

	case CURSORMOVE_NO_MOVE_LEFT:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_PTIN_MAIN_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			BOX2MAIN_PokeSelectOff( syswk );
			return TrayLeftScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_PARTYIN_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_RIGHT:	// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_PTIN_MAIN_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			BOX2MAIN_PokeSelectOff( syswk );
			return TrayRightScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_PARTYIN_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_UP:			// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_DOWN:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NONE:						// ����Ȃ�
		break;

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_PTIN_MAIN_NAME ){
				BOX2MAIN_PokeInfoPut( syswk, pos );
			}else{
				if( pos != BOX2UI_PTIN_MAIN_TSURETEIKU &&
						pos != BOX2UI_PTIN_MAIN_STATUS &&
						pos != BOX2UI_PTIN_MAIN_MARKING &&
						pos != BOX2UI_PTIN_MAIN_FREE &&
						pos != BOX2UI_PTIN_MAIN_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_PARTYIN_MAIN) );

	case CURSORMOVE_CANCEL:				// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		// ���j���[�\����
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
			u8	pos = syswk->get_pos;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_PARTYIN_MENU_CLOSE );
		}
		syswk->dat->retMode = BOX_END_MODE_MENU;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;

	default:
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_DECIDE );
			BOX2BMP_MenuStrPrint( syswk, PartyInMenuStrTbl, 5 );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_PTIN_MAIN_TSURETEIKU, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_PTIN_MAIN_TSURETEIKU );
			return TrayPokeGetKey( syswk, ret, BOX2SEQ_MAINSEQ_PARTYIN_MAIN );
		}
	}

	return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u��Ă����v���j���[�����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PARTYIN_MENU_CLOSE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInMenuClose( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	return VFuncSet( syswk, BOX2MAIN_VFuncMenuMove, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_PARTYIN_MAIN) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u��Ă����v���j���[
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PARTYIN_SELECT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInSelect( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
		BOX2MAIN_ResetTouchBar( syswk );
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_PARTYIN_SELECT );

	case 1:
		// �莝���ɋ󂫂����邩
		if( PokeParty_GetPokeCount( syswk->dat->pokeparty ) != TEMOTI_POKEMAX ){
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_PARTYIN_ACTION );
		}
		PMSND_PlaySE( SE_BOX2_WARNING );
		BOX2BMP_PokeSelectMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_ERR, BOX2BMPWIN_ID_MSG1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
		return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );
	}

	return BOX2SEQ_MAINSEQ_PARTYIN_SELECT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u��Ă����v����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PARTYIN_ACTION
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInAction( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		BOX2OBJ_PartyPokeIconFrmInSetRight( syswk );
		BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
		BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
		syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_PARTYIN_ACTION );

	case 1:
		syswk->get_tray = syswk->tray;
		BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
		PokeMoveWorkAlloc( syswk );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyInPokeMove, BOX2SEQ_MAINSEQ_PARTYIN_ACTION );

	case 2:
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		BOX2MAIN_PokeDataMove( syswk );
		PokeMoveWorkFree( syswk );
		BOX2MAIN_PokeInfoOff( syswk );
		BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_PARTYIN_ACTION );

	case 3:
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
		BOX2UI_PutHandCursor( syswk, syswk->get_pos );
		BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
		syswk->get_pos = BOX2MAIN_GETPOS_NONE;
		syswk->app->sub_seq = 0;
		return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
	}

	return BOX2SEQ_MAINSEQ_PARTYIN_ACTION;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u��Ă����v�|�P�����z�u��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PARTYIN_POKECHG_END
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInPokeChgEnd( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_POKEMOVE_WORK * work;
	u32	pos;

	work = syswk->app->vfunk.work;
	pos  = work->get_pos;

	BOX2MAIN_PokeDataMove( syswk );
	PokeMoveWorkFree( syswk );

	BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );

	if( syswk->get_pos != BOX2MAIN_GETPOS_NONE ){
		BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
		pos = BOX2UI_PTIN_MAIN_TSURETEIKU;
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, pos );
	BOX2UI_PutHandCursor( syswk, pos );

	return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
}


//============================================================================================
//	�|�P������a����
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�|�P��������������v�莝���I�����C��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PARTYOUT_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	ret = BOX2UI_HitCheckTrgPartyPokeLeft();
	if( ret != GFL_UI_TP_HIT_NONE ){
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
				BOX2BMP_MenuStrPrint( syswk, PartyOutMenuStrTbl, 5 );
			}
			BOX2MAIN_PokeInfoPut( syswk, ret );
			return PartyPokeGetDrop( syswk, ret );
		}else{
			CURSORMOVE_PosSet( syswk->app->cmwk, ret-BOX2OBJ_POKEICON_TRAY_MAX );
			BOX2UI_PutHandCursor( syswk, ret+BOX2OBJ_POKEICON_TRAY_MAX );
			BOX2MAIN_PokeInfoOff( syswk );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				BOX2OBJ_PokeCursorVanish( syswk, FALSE );
				BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
				BOX2MAIN_ResetTouchBar( syswk );
				return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_PARTYOUT_MAIN );
			}else{
				return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
			}
		}
	}

	ret = BOX2UI_PartyOutMain( syswk );

	switch( ret ){
	case BOX2UI_PTOUT_MAIN_RETURN1:		// 06: �߂�P
		PMSND_PlaySE( SE_BOX2_CLOSE );
		syswk->dat->retMode = BOX_END_MODE_C_GEAR;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_END, 8, BOX2SEQ_MAINSEQ_BOXEND_DIRECT );

	case BOX2UI_PTOUT_MAIN_RETURN2:		// 07: �߂�Q
		PMSND_PlaySE( SE_BOX2_CANCEL );
		syswk->dat->retMode = BOX_END_MODE_MENU;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case BOX2UI_PTOUT_MAIN_AZUKERU:		// 08: ��������
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU2, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_PARTYOUT_PUT_INIT) );

	case BOX2UI_PTOUT_MAIN_STATUS:		// 09: �悤�����݂�
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->cur_rcv_pos = BOX2UI_PTOUT_MAIN_STATUS;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU3, BOX2SEQ_MAINSEQ_STATUS_CALL );

	case BOX2UI_PTOUT_MAIN_MARKING:		// 10: �}�[�L���O
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_MARKING_INIT) );

	case BOX2UI_PTOUT_MAIN_FREE:			// 11: �ɂ���
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_POKEFREE_INIT) );

	case BOX2UI_PTOUT_MAIN_CLOSE:			// 12: �Ƃ���
		PMSND_PlaySE( SE_BOX2_CANCEL );
		{
			u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_PARTYOUT_MENU_CLOSE );

	case CURSORMOVE_NO_MOVE_UP:				// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_DOWN:			// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_LEFT:			// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_RIGHT:		// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NONE:							// ����Ȃ�
		break;

	case CURSORMOVE_CURSOR_ON:				// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;

	case CURSORMOVE_CURSOR_MOVE:			// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_PTOUT_MAIN_RETURN1 ){
				BOX2MAIN_PokeInfoPut( syswk, pos+BOX2OBJ_POKEICON_TRAY_MAX );
			}else{
				if( pos != BOX2UI_PTOUT_MAIN_AZUKERU &&
						pos != BOX2UI_PTOUT_MAIN_STATUS &&
						pos != BOX2UI_PTOUT_MAIN_MARKING &&
						pos != BOX2UI_PTOUT_MAIN_FREE &&
						pos != BOX2UI_PTOUT_MAIN_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_PARTYOUT_MAIN) );

	case CURSORMOVE_CANCEL:						// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		// ���j���[�\����
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
			u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_PARTYOUT_MENU_CLOSE );
		}
		syswk->dat->retMode = BOX_END_MODE_MENU;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	default:
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_DECIDE );
			BOX2BMP_MenuStrPrint( syswk, PartyOutMenuStrTbl, 5 );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_PTOUT_MAIN_AZUKERU, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_PTOUT_MAIN_AZUKERU );
			return PartyPokeGetKey( syswk, ret, BOX2SEQ_MAINSEQ_PARTYOUT_MAIN );
		}
	}

	return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u��������v���j���[�����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PARTYOUT_MENU_CLOSE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutMenuClose( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	return VFuncSet( syswk, BOX2MAIN_VFuncMenuMove, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_PARTYOUT_MAIN) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u��������v�G���[���A
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PARTYOUT_ERR_RCV_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutErrRcvMain( BOX2_SYS_WORK * syswk )
{
	CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos-BOX2OBJ_POKEICON_TRAY_MAX );
	BOX2UI_PutHandCursor( syswk, syswk->get_pos-BOX2OBJ_POKEICON_TRAY_MAX );
	BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u��������v�z�u�I����������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PARTYOUT_PUT_INIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutPutInit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
		BOX2MAIN_ResetTouchBar( syswk );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_PARTYOUT_PUT_INIT );

	case 1:
		switch( PartyOutCheck(syswk) ){
		case 0:		// �a������
			syswk->poke_get_key = 1;
			BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
			BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_GET_POS, BOX2OBJ_POKEICON_PRI_CHG_GET );
			BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
			BOX2UI_PutHandCursor( syswk, syswk->get_pos-BOX2OBJ_POKEICON_TRAY_MAX );
			syswk->app->sub_seq++;
			return VFuncSet( syswk, BOX2MAIN_VFuncPokeMoveGetKey, BOX2SEQ_MAINSEQ_PARTYOUT_PUT_INIT );
		case 1:		// �킦��|�P���������Ȃ��Ȃ�
			PMSND_PlaySE( SE_BOX2_WARNING );
			BOX2OBJ_PokeCursorVanish( syswk, FALSE );
			BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_ONE, BOX2BMPWIN_ID_MSG1 );
			syswk->next_seq = BOX2SEQ_MAINSEQ_PARTYOUT_ERR_RCV_MAIN;
			syswk->app->sub_seq = 0;
			return BOX2SEQ_MAINSEQ_TRGWAIT;
		case 2:		// ���[���������Ă���
			PMSND_PlaySE( SE_BOX2_WARNING );
			BOX2OBJ_PokeCursorVanish( syswk, FALSE );
			BOX2BMP_PokeSelectMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_MAIL, BOX2BMPWIN_ID_MSG1 );
			syswk->next_seq = BOX2SEQ_MAINSEQ_PARTYOUT_ERR_RCV_MAIN;
			syswk->app->sub_seq = 0;
			return BOX2SEQ_MAINSEQ_TRGWAIT;
		}
		break;

	case 2:
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_PARTYOUT_PUT_INIT );

	case 3:
		BOX2UI_PartyOutBoxSelCursorMoveSet( syswk, syswk->get_pos-BOX2OBJ_POKEICON_TRAY_MAX );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncCursorMove, BOX2SEQ_MAINSEQ_PARTYOUT_PUT_INIT );

	case 4:
		GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2BMP_PokeSelectMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_INIT, BOX2BMPWIN_ID_MSG3 );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_PTOUT_BOXSEL, 0 );
		syswk->app->sub_seq = 0;
		return BOX2SEQ_MAINSEQ_PARTYOUT_PUT_MAIN;
	}

	return BOX2SEQ_MAINSEQ_PARTYOUT_PUT_INIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u��������v�z�u�I�����C��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PARTYOUT_PUT_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutPutMain( BOX2_SYS_WORK * syswk )
{
	u32	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	if( ret == CURSORMOVE_NONE ){
		if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_L ){
			BOX2MAIN_ChgCursorButton( syswk );
			ret = BOX2UI_PTOUT_BOXSEL_LEFT;
		}else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_R ){
			BOX2MAIN_ChgCursorButton( syswk );
			ret = BOX2UI_PTOUT_BOXSEL_RIGHT;
		}
	}

	switch( ret ){
	case BOX2UI_PTOUT_BOXSEL_NAME:		// 00: �g���C��
	case BOX2UI_PTOUT_BOXSEL_TRAY:		// 01: �g���C
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_PTOUT_BOXSEL_NAME );
		if( BOXDAT_GetEmptySpaceTray( syswk->dat->sv_box, syswk->tray ) == 0 ){
			PMSND_PlaySE( SE_BOX2_WARNING );
			BOX2BMP_PokeSelectMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_BOXMAX, BOX2BMPWIN_ID_MSG1 );
			syswk->next_seq = BOX2SEQ_MAINSEQ_PARTYOUT_ERR_RCV_PUT;
			return BOX2SEQ_MAINSEQ_TRGWAIT;
		}else{
			int	pos, tray;
			tray = syswk->tray;
			pos  = 0;
			BOXDAT_GetEmptyTrayNumberAndPos( syswk->dat->sv_box, &tray, &pos );
			syswk->app->poke_put_key = pos;
			syswk->get_tray = syswk->tray;
			syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_TRAY;
			PMSND_PlaySE( SE_BOX2_DECIDE );
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_PARTYOUT_PUT_END );
		}
		break;

	case BOX2UI_PTOUT_BOXSEL_LEFT:		// 02: �g���C�؂�ւ����E��
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_PTOUT_BOXSEL_NAME );
	case CURSORMOVE_NO_MOVE_LEFT:			// �\���L�[���������ꂽ���A�ړ��Ȃ�
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		return TrayLeftScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_PARTYOUT_PUT_MAIN );

	case BOX2UI_PTOUT_BOXSEL_RIGHT:		// 03: �g���C�؂�ւ����E�E
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_PTOUT_BOXSEL_NAME );
	case CURSORMOVE_NO_MOVE_RIGHT:		// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		return TrayRightScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_PARTYOUT_PUT_MAIN );


	case BOX2UI_PTOUT_BOXSEL_RETURN:	// 04: �߂�
	case CURSORMOVE_CANCEL:						// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		syswk->app->poke_put_key = BOX2MAIN_GETPOS_NONE;
		syswk->get_tray = syswk->tray;
		syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_PARTY;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_PARTYOUT_PUT_END) );

	case CURSORMOVE_CURSOR_ON:				// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;

	case CURSORMOVE_CURSOR_MOVE:			// �ړ�
	case CURSORMOVE_NO_MOVE_UP:				// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_DOWN:			// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NONE:							// ����Ȃ�
		break;
	}

	return BOX2SEQ_MAINSEQ_PARTYOUT_PUT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u��������v�z�u�I������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PARTYOUT_PUT_END
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutPutEnd( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case SEQ_PTOUT_PUT_POKEMOVE:
		// �L�����Z��
		if( syswk->app->poke_put_key == BOX2MAIN_GETPOS_NONE ){
			BOX2UI_PartyOutBoxSelCursorMovePutSet( syswk, syswk->get_pos );
			syswk->app->sub_seq = SEQ_PTOUT_PUT_PTFRM_RET;
		// ���s
		}else{
			BOX2UI_PartyOutBoxSelCursorMovePutSet( syswk, syswk->app->poke_put_key );
			syswk->app->sub_seq = SEQ_PTOUT_PUT_ICON_PUT;
		}
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
		BOX2MAIN_ResetTouchBar( syswk );
		return VFuncSet( syswk, BOX2MAIN_VFuncCursorMove, BOX2SEQ_MAINSEQ_PARTYOUT_PUT_END );

	case SEQ_PTOUT_PUT_PTFRM_RET:
		GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
		syswk->app->sub_seq = SEQ_PTOUT_PUT_ICON_RET;
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_PARTYOUT_PUT_END );

	case SEQ_PTOUT_PUT_ICON_RET:
		PokeMoveWorkAlloc( syswk );
		syswk->app->sub_seq = SEQ_PTOUT_PUT_CANCEL_END;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMovePutKey, BOX2SEQ_MAINSEQ_PARTYOUT_PUT_END );

	case SEQ_PTOUT_PUT_PTFRM_IN:
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
		syswk->app->sub_seq = SEQ_PTOUT_PUT_END;
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_PARTYOUT_PUT_END );

	case SEQ_PTOUT_PUT_ICON_PUT:
		GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
		PokeMoveWorkAlloc( syswk );
		BOX2OBJ_PokeCursorVanish( syswk, FALSE );
		syswk->app->sub_seq = SEQ_PTOUT_PUT_DATA_CHG;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMovePutKey2, BOX2SEQ_MAINSEQ_PARTYOUT_PUT_END );

	case SEQ_PTOUT_PUT_DATA_CHG:
		{
			BOX2MAIN_POKEMOVE_WORK * work;
			work = syswk->app->vfunk.work;
			BOX2MAIN_PokeDataMove( syswk );
			PokeMoveWorkFree( syswk );
		}
		BOX2MAIN_PokeInfoOff( syswk );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );
		syswk->app->sub_seq = SEQ_PTOUT_PUT_PTFRM_IN;
		break;

	case SEQ_PTOUT_PUT_CANCEL_END:
		PokeMoveWorkFree( syswk );
	case SEQ_PTOUT_PUT_END:
		syswk->poke_get_key = 0;
		BOX2OBJ_PokeCursorVanish( syswk, FALSE );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_PTOUT_MAIN, syswk->get_pos-BOX2OBJ_POKEICON_TRAY_MAX );
		BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
		syswk->app->sub_seq = 0;
		return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
	}

	return BOX2SEQ_MAINSEQ_PARTYOUT_PUT_END;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u��������v�z�u�G���[���A
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PARTYOUT_ERR_RCV_PUT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutErrRcvPut( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	BOX2BMP_PokeSelectMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_INIT, BOX2BMPWIN_ID_MSG3 );
	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );
	return BOX2SEQ_MAINSEQ_PARTYOUT_PUT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u��������v�z�u��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_PARTYOUT_POKECHG_END
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutPokeChgEnd( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_POKEMOVE_WORK * work;
	u32	pos;

	work = syswk->app->vfunk.work;
	pos  = work->get_pos;

	BOX2MAIN_PokeDataMove( syswk );
	PokeMoveWorkFree( syswk );

	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );

	if( syswk->get_pos == BOX2MAIN_GETPOS_NONE ){
		CURSORMOVE_PosSet( syswk->app->cmwk, 0 );
		BOX2UI_PutHandCursor( syswk, 0 );
		BOX2MAIN_PokeInfoPut( syswk, BOX2OBJ_POKEICON_TRAY_MAX );
	}else{
		BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_PTOUT_MAIN_AZUKERU );
		BOX2UI_PutHandCursor( syswk, BOX2UI_PTOUT_MAIN_AZUKERU );
	}

	return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
}


//============================================================================================
//	�����
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F��������C��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	// �Z���N�g�{�^��
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		int	next = ChgSeqSelectButton( syswk, BOX2SEQ_MAINSEQ_ITEMGET_INIT );
		if( syswk->mv_cnv_mode == 1 ){
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
				syswk->get_pos = pos;
			}else{
				syswk->get_pos = 0;
			}
		}
		return next;
	}

	ret = BOX2UI_HitCheckTrgTrayPoke();
	if( ret != GFL_UI_TP_HIT_NONE ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			return TrayItemGet( syswk, ret );
		}else{
			BOX2MAIN_PokeInfoOff( syswk );
			if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
				BOX2UI_CursorMoveVFuncWorkSet( syswk->app, ret, CURSORMOVE_PosGet(syswk->app->cmwk) );
				CURSORMOVE_PosSet( syswk->app->cmwk, ret );
				return MainSeq_ItemMenuClose( syswk );
			}else{
				CURSORMOVE_PosSet( syswk->app->cmwk, ret );
				BOX2UI_PutHandCursor( syswk, ret );
				return BOX2SEQ_MAINSEQ_ITEM_MAIN;
			}
		}
	}

	ret = BOX2UI_BoxItemArrangeMain( syswk );

	switch( ret ){
	case BOX2UI_ITEM_MAIN_NAME:		// 30: �{�b�N�X��
		PMSND_PlaySE( SE_BOX2_DECIDE );
		BOX2MAIN_PokeInfoOff( syswk );
		return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_BOXTHEMA_INIT );

	case BOX2UI_ITEM_MAIN_LEFT:		// 31: �g���C�؂�ւ����E��
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_MAIN_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_ITEM_MAIN_NAME );
		return TrayLeftScrollSet( syswk, TRUE, BOX2SEQ_MAINSEQ_ITEM_MAIN );

	case BOX2UI_ITEM_MAIN_RIGHT:		// 32: �g���C�؂�ւ����E�E
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_MAIN_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_ITEM_MAIN_NAME );
		return TrayRightScrollSet( syswk, TRUE, BOX2SEQ_MAINSEQ_ITEM_MAIN );

	case BOX2UI_ITEM_MAIN_PARTY:				// 33: �莝���|�P����
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_POKE_BTN, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_INIT) );

	case BOX2UI_ITEM_MAIN_RETURN1:			// 34: �߂�P
		PMSND_PlaySE( SE_BOX2_CLOSE );
		syswk->dat->retMode = BOX_END_MODE_C_GEAR;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_END, 8, BOX2SEQ_MAINSEQ_BOXEND_DIRECT );

	case BOX2UI_ITEM_MAIN_RETURN2:			// 35: �߂�Q
		PMSND_PlaySE( SE_BOX2_CANCEL );
		syswk->dat->retMode = BOX_END_MODE_MENU;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case BOX2UI_ITEM_MAIN_MENU1:				// 36: ���ǂ�����
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->mv_cnv_mode = 0;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ITEMGET_INIT) );

	case BOX2UI_ITEM_MAIN_MENU2:				// 37: �o�b�O�� or ��������
		PMSND_PlaySE( SE_BOX2_DECIDE );
		if( syswk->app->get_item == ITEM_DUMMY_DATA ){
			syswk->subProcMode = SUB_PROC_MODE_MENU_BOX;
			syswk->cur_rcv_pos = BOX2UI_ITEM_MAIN_MENU2;
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_ITEM_MENU_CHECK );
		}else{
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_ITEM_MENU_CHECK );
		}
		break;

	case BOX2UI_ITEM_MAIN_CLOSE:				// 38: �Ƃ���
		PMSND_PlaySE( SE_BOX2_CANCEL );
		{
			u8	pos = syswk->get_pos;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_ITEM_MENU_CLOSE );

	case BOX2UI_ITEM_MAIN_CONV:					// 39: �֗����[�h
		{
			int	next = ChgSeqSelectTouch( syswk, BOX2SEQ_MAINSEQ_ITEMGET_INIT );
			if( syswk->mv_cnv_mode == 1 ){
				u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
				if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
					syswk->get_pos = pos;
				}else{
					syswk->get_pos = 0;
				}
			}
			return next;
		}
		break;

	case CURSORMOVE_NO_MOVE_UP:			// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_DOWN:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NONE:						// ����Ȃ�
		break;

	case CURSORMOVE_NO_MOVE_LEFT:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_ITEM_MAIN_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			BOX2MAIN_PokeSelectOff( syswk );
			return TrayLeftScrollSet( syswk, TRUE, BOX2SEQ_MAINSEQ_ITEM_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_RIGHT:	// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_ITEM_MAIN_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			BOX2MAIN_PokeSelectOff( syswk );
			return TrayRightScrollSet( syswk, TRUE, BOX2SEQ_MAINSEQ_ITEM_MAIN );
		}
		break;

	case CURSORMOVE_CURSOR_ON:			// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_ARRANGE_MAIN_NAME ){
				BOX2MAIN_PokeInfoPut( syswk, pos );
			}else{
				if( pos != BOX2UI_ITEM_MAIN_MENU1 &&
						pos != BOX2UI_ITEM_MAIN_MENU2 &&
						pos != BOX2UI_ITEM_MAIN_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ITEM_MAIN) );

	case CURSORMOVE_CANCEL:					// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		// ���j���[�\����
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
			u8	pos = syswk->get_pos;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_ITEM_MENU_CLOSE );
		}
		syswk->dat->retMode = BOX_END_MODE_MENU;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	default:
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_DECIDE );
			return TrayItemGetKey( syswk, ret );
		}
		break;
	}

	return BOX2SEQ_MAINSEQ_ITEM_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���j���[�����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_MENU_CLOSE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemMenuClose( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	BOX2BGWFRM_TemochiButtonOn( syswk->app );
	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeMenuClose, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ITEM_MAIN) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�o�b�O�ɖ߂�����̏���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_BAGIN_ANM
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemBagInAnm( BOX2_SYS_WORK * syswk )
{
	if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
		return BOX2SEQ_MAINSEQ_ITEM_BAGIN_ANM;
	}

	BOX2SEQ_ItemModeMenuSet( syswk, ITEM_DUMMY_DATA );

	BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
	BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_RET, BOX2BMPWIN_ID_MSG1 );

	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
	}else{
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
	}
	syswk->app->get_item = ITEM_DUMMY_DATA;

	syswk->next_seq = BOX2SEQ_MAINSEQ_ITEM_POKESET_RET;

	return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�o�b�O���玝��������A�A�C�R�������������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_POKESET_ANM
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemPokeSetAnm( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
	}else{
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
	}
	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeIconClose, BOX2SEQ_MAINSEQ_ITEM_POKESET_RET );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�A�C�R������������A���C�������֕��A
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_POKESET_RET
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemPokeSetRet( BOX2_SYS_WORK * syswk )
{
	int	next;

	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );

	// �J�[�\���ʒu���|�P�����ʒu��
	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
		BOX2UI_PutHandCursor( syswk, syswk->get_pos );
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		next = BOX2SEQ_MAINSEQ_ITEM_MAIN;
	}else{
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX );
		BOX2UI_PutHandCursor( syswk, syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX );
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
		next = BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN;
	}
	// �J�[�\���\��
	BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
	BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );

	return next;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�A�C�e���A�C�R���ړ���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEMARRANGE_ICONMOVE_RET
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeIconMoveRet( BOX2_SYS_WORK * syswk )
{
	int	pos;

	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		pos = BOX2UI_ITEM_MAIN_MENU2;
	}else{
		pos = BOX2UI_ITEM_MAIN_MENU1;
	}
	CURSORMOVE_PosSet( syswk->app->cmwk, pos );
	BOX2UI_PutHandCursor( syswk, pos );
	BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );

	return BOX2SEQ_MAINSEQ_ITEM_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�A�C�e���ړ���������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEMGET_INIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemGetInit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		syswk->app->sub_seq++;
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );

		if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == TRUE ){
			syswk->pokechg_mode = 1;
		}else{
			syswk->pokechg_mode = 0;
		}
		// �Z���N�g�{�^��
		if( syswk->mv_cnv_mode == 1 ){
			syswk->get_tray = BOX2MAIN_GETPOS_NONE;
			syswk->app->get_item = ITEM_DUMMY_DATA;
			BOX2MAIN_ResetTouchBar( syswk );
			BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
		}else{
			if( syswk->mv_cnv_mode == 2 ){
				if( syswk->app->get_item == ITEM_DUMMY_DATA ){
					syswk->get_tray = BOX2MAIN_GETPOS_NONE;
					syswk->mv_cnv_mode = 1;
				}
			}
			BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
			BOX2MAIN_ResetTouchBar( syswk );
			BOX2OBJ_PokeCursorVanish( syswk, FALSE );
			BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
			return VFuncSet( syswk, BOX2MAIN_VFuncItemGetMenuClose, BOX2SEQ_MAINSEQ_ITEMGET_INIT );
		}

	case 1:
		syswk->app->sub_seq++;
		if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == TRUE ){
			BOX2BGWFRM_PartyPokeFrameRightMoveSet( syswk->app->wfrm );
		}else{
			BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
			BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
			BOX2OBJ_PartyPokeIconFrmInSet( syswk, 0 );
		}
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ITEMGET_INIT );

	case 2:
		syswk->app->sub_seq = 0;
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY_MOVE, syswk->get_pos );
		BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
		CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_BOXLIST );
		CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_STATUS );
		if( syswk->mv_cnv_mode == 1 ){
			BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );
			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_BLUE, TRUE );
			return BOX2SEQ_MAINSEQ_ITEMGET_MAIN;
		}else if( syswk->mv_cnv_mode == 2 ){
			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_BLUE, TRUE );
		}else{
			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );
		}
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );
		return BoxPartyItemGetKey( syswk, syswk->get_pos );
	}

	return BOX2SEQ_MAINSEQ_ITEMGET_INIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�A�C�e���ړ����C������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEMGET_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemGetMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	if( syswk->mv_cnv_end == 1 ){
		syswk->mv_cnv_mode = 0;
		syswk->mv_cnv_end = 0;
		return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_ITEMGET_END );
	}

	// �Z���N�g�{�^��
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		if( syswk->mv_cnv_mode != 0 && syswk->poke_get_key == 0 ){
			BOX2MAIN_ChgCursorButton( syswk );
			syswk->mv_cnv_mode = 0;
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_ITEMGET_END );
		}
	}

	if( syswk->poke_get_key == 0 ){
		// �莝��
		ret = BOX2UI_HitCheckTrgPartyPokeRight();
		if( ret != GFL_UI_TP_HIT_NONE ){
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
			ret += BOX2OBJ_POKEICON_TRAY_MAX;
			if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
				BOX2MAIN_PokeInfoPut( syswk, ret );
				BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
				return BoxPartyItemGet( syswk, ret );
			}else{
				BOX2MAIN_PokeInfoOff( syswk );
				CURSORMOVE_PosSet( syswk->app->cmwk, ret );
				return BOX2SEQ_MAINSEQ_ITEMGET_MAIN;
			}
		}
		// �{�b�N�X
		ret = BOX2UI_HitCheckTrgTrayPoke();
		if( ret != GFL_UI_TP_HIT_NONE ){
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
			if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
				BOX2MAIN_PokeInfoPut( syswk, ret );
				BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
				return BoxPartyItemGet( syswk, ret );
			}else{
				BOX2MAIN_PokeInfoOff( syswk );
				CURSORMOVE_PosSet( syswk->app->cmwk, ret );
				return BOX2SEQ_MAINSEQ_ITEMGET_MAIN;
			}
		}
	}

	// �L�[�E�^�b�`�`�F�b�N
	ret = BOX2UI_ArrangePartyPokeGetMain( syswk );

	switch( ret ){
	case BOX2UI_ARRANGE_PTGT_NAME:		// 36: �{�b�N�X��
		// ���擾
		if( syswk->poke_get_key == 0 ){
			PMSND_PlaySE( SE_BOX2_DECIDE );
			BOX2MAIN_PokeInfoOff( syswk );
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_BOXTHEMA_INIT );
		}
		break;

	case BOX2UI_ARRANGE_PTGT_LEFT:		// 37: �{�b�N�X�؂�ւ����i���j
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_ARRANGE_PTGT_NAME );
		// ���擾
		if( syswk->poke_get_key == 0 ){
			BOX2MAIN_PokeSelectOff( syswk );
		// �擾��
		}else{
			BOX2OBJ_MovePokeIconHand( syswk );
		}
		return TrayLeftScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_ITEMGET_MAIN );

	case BOX2UI_ARRANGE_PTGT_RIGHT:		// 38: �{�b�N�X�؂�ւ����i�E�j
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_ARRANGE_PTGT_NAME );
		// ���擾
		if( syswk->poke_get_key == 0 ){
			BOX2MAIN_PokeSelectOff( syswk );
		// �擾��
		}else{
			BOX2OBJ_MovePokeIconHand( syswk );
		}
		return TrayRightScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_ITEMGET_MAIN );

	case BOX2UI_ARRANGE_PTGT_RET:		// 41: �߂�
		if( syswk->poke_get_key == 0 ){
			PMSND_PlaySE( SE_BOX2_CANCEL );
			syswk->mv_cnv_mode = 0;
			return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ITEMGET_END) );
		}
		break;

	case BOX2UI_ARRANGE_PTGT_CONV:	// 42: �֗����[�h
		if( syswk->mv_cnv_mode != 0 && syswk->poke_get_key == 0 ){
			syswk->mv_cnv_mode = 0;
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_ITEMGET_END );
		}
		break;

	case CURSORMOVE_CANCEL:					// �L�����Z��
		if( syswk->poke_get_key == 1 ){
			return BoxPartyItemPutKey( syswk, BOX2MAIN_GETPOS_NONE );
		}else{
			PMSND_PlaySE( SE_BOX2_CANCEL );
			syswk->mv_cnv_mode = 0;
			return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ITEMGET_END) );
		}
		break;

	case CURSORMOVE_NO_MOVE_LEFT:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_ARRANGE_PTGT_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			return TrayLeftScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_ITEMGET_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_RIGHT:	// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_ARRANGE_PTGT_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			return TrayRightScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_ITEMGET_MAIN );
		}
		break;

	case BOX2UI_ARRANGE_PTGT_BOXLIST:	// 39: �{�b�N�X���X�g
	case BOX2UI_ARRANGE_PTGT_STATUS:	// 40: �X�e�[�^�X
	case CURSORMOVE_NO_MOVE_UP:				// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_DOWN:			// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NONE:							// ����Ȃ�
		break;

	case CURSORMOVE_CURSOR_ON:				// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		if( syswk->poke_get_key == 0 ){
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2OBJ_POKEICON_PUT_MAX ){
				BOX2MAIN_PokeInfoPut( syswk, pos );
			}else{
				BOX2MAIN_PokeInfoOff( syswk );
			}
		}
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ITEMGET_MAIN) );

	default:
		// �擾��
		if( syswk->poke_get_key == 1 ){
			return BoxPartyItemPutKey( syswk, ret );
		// ���擾
		}else{
			return BoxPartyItemGetKey( syswk, ret );
		}
		break;
	}

	return BOX2SEQ_MAINSEQ_ITEMGET_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�A�C�e���ړ��I������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEMGET_END
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemGetEnd( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2MAIN_PokeInfoOff( syswk );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		if( syswk->pokechg_mode == 0 ){
			BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
		}else{
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
			BOX2BGWFRM_PartyPokeFrameLeftMoveSet( syswk->app->wfrm );
		}
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ITEMGET_END );

	case 1:
		syswk->app->sub_seq = 0;
		BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
		BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );
		if( syswk->pokechg_mode == 0 ){
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos >= BOX2OBJ_POKEICON_PUT_MAX ){
				pos = 0;
			}else if( pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
				pos = 0;
			}
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_MAIN, pos );
			BOX2MAIN_PokeInfoPut( syswk, pos );
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
			return BOX2SEQ_MAINSEQ_ITEM_MAIN;
		}else{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos >= BOX2OBJ_POKEICON_PUT_MAX ){
				pos = 0;
			}else if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
				pos = 0;
			}else{
				pos -= BOX2OBJ_POKEICON_TRAY_MAX;
			}
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_PARTY, pos );
			BOX2MAIN_PokeInfoPut( syswk, pos+BOX2OBJ_POKEICON_TRAY_MAX );
			BOX2BGWFRM_BoxListButtonOn( syswk->app );
			return BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN;
		}
	}

	return BOX2SEQ_MAINSEQ_ITEMGET_END;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F����������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_BOXPARTY_ICONMOVE_POKEADD
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemBoxPartyIconMovePokeAdd( BOX2_SYS_WORK * syswk )
{
	POKEMON_PASO_PARAM * ppp;
	BOX2MAIN_ITEMMOVE_WORK * mvwk;
	u16	item;

	if( syswk->mv_cnv_mode == 0 ){
		syswk->mv_cnv_end = 1;
	}

	BOX2OBJ_PokeCursorVanish( syswk, FALSE );

	mvwk = syswk->app->vfunk.work;

	// �A�C�R���̈ړ��悪���Ɠ���
	if( mvwk->put_pos == syswk->get_pos &&
			( syswk->get_tray == BOX2MAIN_GETPOS_NONE || syswk->get_tray == syswk->tray ) ){
		u32	set_pos = mvwk->set_pos;
		ItemMoveWorkFree( syswk );
		BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );
		if( ITEM_CheckMail( syswk->app->get_item ) == TRUE && set_pos != syswk->get_pos ){
			PMSND_PlaySE( SE_BOX2_WARNING );
			BOX2BMP_MailMoveErrorPut( syswk, BOX2BMPWIN_ID_MSG1 );
			syswk->next_seq = BOX2SEQ_MAINSEQ_ITEM_BOXPARTY_CHG_ERROR;
			return BOX2SEQ_MAINSEQ_TRGWAIT;
		}else{
			return BOX2SEQ_MAINSEQ_ITEMGET_MAIN;
		}
	}

	if( syswk->get_tray == syswk->tray || syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
		BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, TRUE );
	}
	BOX2OBJ_PokeIconBlendSet( syswk->app, mvwk->put_pos, FALSE );

	// �ړ���ɃA�C�e�����Z�b�g
	item = BOX2MAIN_PokeParaGet( syswk, mvwk->put_pos, syswk->tray, ID_PARA_item, NULL );
	BOX2MAIN_PokeParaPut( syswk, mvwk->put_pos, syswk->tray, ID_PARA_item, syswk->app->get_item );
	ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, mvwk->put_pos );
	if( BOX2MAIN_PokeItemFormChange( syswk, ppp ) == TRUE ){
		BOX2MAIN_FormChangeRenew( syswk, mvwk->put_pos );
		BOX2OBJ_PokeIconChange(
			syswk, syswk->tray, mvwk->put_pos,
			syswk->app->pokeicon_id[mvwk->put_pos] );
	}
	BOX2MAIN_PokeInfoPut( syswk, mvwk->put_pos );
	CURSORMOVE_PosSet( syswk->app->cmwk, mvwk->put_pos );

	// �ړ����ɃA�C�e�����Z�b�g
	syswk->app->get_item = item;
	BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, syswk->get_tray, ID_PARA_item, syswk->app->get_item );
	if( syswk->get_tray == BOX2MAIN_GETPOS_NONE || syswk->get_tray == syswk->tray ){
		ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );
		if( BOX2MAIN_PokeItemFormChange( syswk, ppp ) == TRUE ){
			BOX2MAIN_FormChangeRenew( syswk, syswk->get_pos );
			BOX2OBJ_PokeIconChange(
				syswk, syswk->tray, syswk->get_pos,
				syswk->app->pokeicon_id[syswk->get_pos] );
		}
	}

	// �ړ��悪�A�C�e���������Ă��Ȃ�����
	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		syswk->get_pos = mvwk->put_pos;
		ItemMoveWorkFree( syswk );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );
		return BOX2SEQ_MAINSEQ_ITEMGET_MAIN;
	}

	BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
	BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 1 );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
	BOX2OBJ_ItemIconCursorAdd( syswk->app );
	PMSND_PlaySE( SE_BOX2_POKE_CATCH );
	return VFuncSet( syswk, BOX2MAIN_VFuncItemIconChgTouch, BOX2SEQ_MAINSEQ_ITEM_BOXPARTY_ICONMOVE_END );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�����������I��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_BOXPARTY_ICONMOVE_END
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemBoxPartyIconMoveEnd( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_ITEMMOVE_WORK * mvwk;
	u16	item;
	u16	get, set;

	if( syswk->mv_cnv_mode == 0 ){
		syswk->mv_cnv_end = 1;
	}

	mvwk = syswk->app->vfunk.work;

	BOX2OBJ_PokeIconBlendSet( syswk->app, mvwk->put_pos, FALSE );

	get = syswk->get_pos;
	set = mvwk->set_pos;

	syswk->get_pos = mvwk->put_pos;

	ItemMoveWorkFree( syswk );

	item = syswk->app->get_item;
	syswk->app->get_item = ITEM_DUMMY_DATA;

	if( ITEM_CheckMail( item ) == TRUE && set != get ){
		PMSND_PlaySE( SE_BOX2_WARNING );
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BMP_MailMoveErrorPut( syswk, BOX2BMPWIN_ID_MSG1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_ITEM_BOXPARTY_CHG_ERROR;
		return BOX2SEQ_MAINSEQ_TRGWAIT;
	}

	BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );

	{
		u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
		if( pos < BOX2OBJ_POKEICON_PUT_MAX ){
			BOX2MAIN_PokeInfoPut( syswk, pos );
		}
	}

	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );

	return BOX2SEQ_MAINSEQ_ITEMGET_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F����ւ��G���[���b�Z�[�W�\����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_BOXPARTY_CHG_ERROR
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemBoxPartyChgError( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_SysWinVanishTrans( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	if( CURSORMOVE_CursorOnOffGet( syswk->app->cmwk ) == TRUE ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );
	}
	return BOX2SEQ_MAINSEQ_ITEMGET_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�����������i�L�[����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_KEYGET_CHANGE_CHECK
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemKeyGetChangeCkack( BOX2_SYS_WORK * syswk )
{
	POKEMON_PASO_PARAM * ppp;
	u16	item;

	if( syswk->mv_cnv_mode == 0 ){
		syswk->mv_cnv_end = 1;
	}

	// �����ʒu�Ɠ���
	if( syswk->app->poke_put_key == syswk->app->get_item_init_pos &&
			( syswk->get_tray == BOX2MAIN_GETPOS_NONE || syswk->get_tray == syswk->tray ) ){
		syswk->app->get_item = ITEM_DUMMY_DATA;
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );
		return BOX2SEQ_MAINSEQ_ITEMGET_MAIN;
	}

	// �擾�ʒu���\������Ă���Ƃ�
	if( syswk->get_tray == syswk->tray || syswk->app->get_item_init_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
		BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->app->get_item_init_pos, TRUE );
	}
	BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->app->poke_put_key, FALSE );

	// �����Ă���A�C�e�����擾
	item = BOX2MAIN_PokeParaGet( syswk, syswk->app->poke_put_key, syswk->tray, ID_PARA_item, NULL );
	// �A�C�e������������
	BOX2MAIN_PokeParaPut( syswk, syswk->app->poke_put_key, syswk->tray, ID_PARA_item, syswk->app->get_item );
	ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->app->poke_put_key );
	if( BOX2MAIN_PokeItemFormChange( syswk, ppp ) == TRUE ){
		BOX2MAIN_FormChangeRenew( syswk, syswk->app->poke_put_key );
		BOX2OBJ_PokeIconChange(
			syswk, syswk->tray, syswk->app->poke_put_key,
			syswk->app->pokeicon_id[syswk->app->poke_put_key] );
	}
	BOX2MAIN_PokeInfoPut( syswk, syswk->app->poke_put_key );

	// �擾�����A�C�e�������̃|�P�����Ɏ�������
	syswk->app->get_item = item;
	BOX2MAIN_PokeParaPut( syswk, syswk->app->get_item_init_pos, syswk->get_tray, ID_PARA_item, syswk->app->get_item );
	if( syswk->get_tray == BOX2MAIN_GETPOS_NONE || syswk->get_tray == syswk->tray ){
		ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->app->get_item_init_pos );
		if( BOX2MAIN_PokeItemFormChange( syswk, ppp ) == TRUE ){
			BOX2MAIN_FormChangeRenew( syswk, syswk->app->get_item_init_pos );
			BOX2OBJ_PokeIconChange(
				syswk, syswk->tray, syswk->app->get_item_init_pos,
				syswk->app->pokeicon_id[syswk->app->get_item_init_pos] );
		}
	}

	// �ړ��悪�A�C�e���������Ă��Ȃ�������
	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );
		return BOX2SEQ_MAINSEQ_ITEMGET_MAIN;
	}

	ItemMoveWorkAlloc( syswk );
	{
		BOX2MAIN_ITEMMOVE_WORK * mvwk = syswk->app->vfunk.work;
		mvwk->put_pos = syswk->app->poke_put_key;
	}

	BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
	BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 1 );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
	BOX2OBJ_ItemIconCursorAdd( syswk->app );
	PMSND_PlaySE( SE_BOX2_POKE_CATCH );
	return VFuncSet( syswk, BOX2MAIN_VFuncItemIconChgTouch, BOX2SEQ_MAINSEQ_ITEM_BOXPARTY_ICONMOVE_END );

}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�����������E�莝�����C��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemPartyMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	// �Z���N�g�{�^��
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		int	next = ChgSeqSelectButton( syswk, BOX2SEQ_MAINSEQ_ITEMGET_INIT );
		if( syswk->mv_cnv_mode == 1 ){
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2OBJ_POKEICON_MINE_MAX ){
				syswk->get_pos = pos + BOX2OBJ_POKEICON_TRAY_MAX;
			}else{
				syswk->get_pos = BOX2OBJ_POKEICON_TRAY_MAX;
			}
		}
		return next;
	}

	ret = BOX2UI_HitCheckTrgPartyPokeLeft();
	if( ret != GFL_UI_TP_HIT_NONE ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			return PartyItemGet( syswk, ret );
		}else{
			BOX2MAIN_PokeInfoOff( syswk );
			if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
				u8	pos = ret - BOX2OBJ_POKEICON_TRAY_MAX;
				BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
				CURSORMOVE_PosSet( syswk->app->cmwk, ret );
				return MainSeq_ItemPartyMenuClose( syswk );
			}else{
				CURSORMOVE_PosSet( syswk->app->cmwk, ret-BOX2OBJ_POKEICON_TRAY_MAX );
				BOX2UI_PutHandCursor( syswk, ret-BOX2OBJ_POKEICON_TRAY_MAX );
				return BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN;
			}
		}
	}

	ret = BOX2UI_BoxItemArrangePartyMoveMain( syswk );

	switch( ret ){

	case BOX2UI_ITEM_PARTY_BOXLIST:		// 06: �{�b�N�X���X�g
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_POKE_BTN, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_END) );

	case BOX2UI_ITEM_PARTY_RETURN1:		// 07: �߂�P
		PMSND_PlaySE( SE_BOX2_CLOSE );
		syswk->dat->retMode = BOX_END_MODE_C_GEAR;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_END, 8, BOX2SEQ_MAINSEQ_BOXEND_DIRECT );

	case BOX2UI_ITEM_PARTY_RETURN2:		// 08: �߂�Q
		PMSND_PlaySE( SE_BOX2_CANCEL );
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_END) );

	case BOX2UI_ITEM_PARTY_MENU1:			// 09: ���ǂ�����
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->mv_cnv_mode = 0;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ITEMGET_INIT) );

	case BOX2UI_ITEM_PARTY_MENU2:			// 10: �o�b�O�� or ��������
		PMSND_PlaySE( SE_BOX2_DECIDE );
		if( syswk->app->get_item == ITEM_DUMMY_DATA ){
			syswk->subProcMode = SUB_PROC_MODE_MENU_PARTY;
			syswk->cur_rcv_pos = BOX2UI_ITEM_PARTY_MENU2;
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_ITEM_MENU_CHECK );
		}else{
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_ITEM_MENU_CHECK );
		}
		break;

	case BOX2UI_ITEM_PARTY_CLOSE:			// 11: �Ƃ���
		PMSND_PlaySE( SE_BOX2_CANCEL );
		{
			u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_ITEM_PARTY_MENU_CLOSE );

	case BOX2UI_ITEM_PARTY_CONV:			// 12: �֗����[�h
		{
			int	next = ChgSeqSelectTouch( syswk, BOX2SEQ_MAINSEQ_ITEMGET_INIT );
			if( syswk->mv_cnv_mode == 1 ){
				u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
				if( pos < BOX2OBJ_POKEICON_MINE_MAX ){
					syswk->get_pos = pos + BOX2OBJ_POKEICON_TRAY_MAX;
				}else{
					syswk->get_pos = BOX2OBJ_POKEICON_TRAY_MAX;
				}
			}
			return next;
		}
		break;

	case CURSORMOVE_NO_MOVE_UP:			// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_DOWN:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_LEFT:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_RIGHT:	// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NONE:						// ����Ȃ�
		break;

	case CURSORMOVE_CURSOR_ON:			// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_ARRANGE_PARTY_BOXLIST ){
				BOX2MAIN_PokeInfoPut( syswk, pos+BOX2OBJ_POKEICON_TRAY_MAX );
			}else if( pos == BOX2UI_ARRANGE_PARTY_BOXLIST ){
				BOX2MAIN_PokeInfoOff( syswk );
			}
		}
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN) );

	case CURSORMOVE_CANCEL:					// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		// ���j���[�\����
		if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
			u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_ITEM_PARTY_MENU_CLOSE );
		}
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_PARTY_END) );

	default:
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_DECIDE );
			return PartyItemGetKey( syswk, ret );
		}
	}

	return BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���j���[�����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_PARTY_MENU_CLOSE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemPartyMenuClose( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	BOX2BGWFRM_BoxListButtonOn( syswk->app );
	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeMenuClose, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�莝���|�P�����̃A�C�e���ړ��J�n
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_PARTY_ICONMOVE_RET
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemPartyIconMoveRet( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_ITEMMOVE_WORK * mvwk;

	mvwk = syswk->app->vfunk.work;

	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		ItemMoveWorkFree( syswk );
		BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_SET, BOX2BMPWIN_ID_MSG1 );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_PARTY_MENU2 );
//		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		BOX2UI_PutHandCursor( syswk, BOX2UI_ITEM_PARTY_MENU2 );
		BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
		return BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN;
	}

	if( mvwk->put_pos == syswk->get_pos ){
		u32	set_pos = mvwk->set_pos;
		ItemMoveWorkFree( syswk );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_PARTY_MENU1 );
		BOX2UI_PutHandCursor( syswk, BOX2UI_ITEM_PARTY_MENU1 );
		if( ITEM_CheckMail( syswk->app->get_item ) == TRUE && set_pos != syswk->get_pos ){
			BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );
			BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_MAIL_MOVE, BOX2BMPWIN_ID_MSG1 );
			PMSND_PlaySE( SE_BOX2_WARNING );
			syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );
		}
		BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_INIT, BOX2BMPWIN_ID_MSG1 );
//		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_PARTY_MENU1 );
//		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
		return BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN;
	}

	if( ITEM_CheckMail( syswk->app->get_item ) == TRUE ){
		u32	set_pos = mvwk->set_pos;
		ItemMoveWorkFree( syswk );
		PMSND_PlaySE( SE_BOX2_WARNING );
		BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );
		BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_MAIL_MOVE, BOX2BMPWIN_ID_MSG1 );
		PMSND_PlaySE( SE_BOX2_WARNING );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_PARTY_MENU1 );
		BOX2UI_PutHandCursor( syswk, BOX2UI_ITEM_PARTY_MENU1 );
		BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
		syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
		return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );
	}

	BOX2OBJ_PokeIconBlendSet( syswk->app, mvwk->put_pos, FALSE );
	BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, TRUE );

	// �A�E�g���C������
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	// �A�C�e���A�C�R���k��
	BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );

	return BOX2SEQ_MAINSEQ_ITEM_PARTY_ICONMOVE_POKEADD;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�莝���|�P�����̃A�C�e���ړ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_PARTY_ICONMOVE_POKEADD
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemPartyIconMovePokeAdd( BOX2_SYS_WORK * syswk )
{
	POKEMON_PASO_PARAM * ppp;
	BOX2MAIN_ITEMMOVE_WORK * mvwk;
	u16	item;

	if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
		return BOX2SEQ_MAINSEQ_ITEM_PARTY_ICONMOVE_POKEADD;
	}

	BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );

	mvwk = syswk->app->vfunk.work;

	// �ړ���ɃA�C�e�����Z�b�g
	item = BOX2MAIN_PokeParaGet( syswk, mvwk->put_pos, syswk->tray, ID_PARA_item, NULL );
	BOX2MAIN_PokeParaPut( syswk, mvwk->put_pos, syswk->tray, ID_PARA_item, syswk->app->get_item );
	ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, mvwk->put_pos );
	if( BOX2MAIN_PokeItemFormChange( syswk, ppp ) == TRUE ){
		BOX2MAIN_FormChangeRenew( syswk, mvwk->put_pos );
		BOX2OBJ_PokeIconChange(
			syswk, syswk->tray, mvwk->put_pos,
			syswk->app->pokeicon_id[mvwk->put_pos] );
	}
	BOX2MAIN_PokeInfoPut( syswk, mvwk->put_pos );

	// �ړ����ɃA�C�e�����Z�b�g
	syswk->app->get_item = item;
	BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, syswk->get_tray, ID_PARA_item, syswk->app->get_item );
	if( syswk->get_tray == BOX2MAIN_GETPOS_NONE || syswk->get_tray == syswk->tray ){
		ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );
		if( BOX2MAIN_PokeItemFormChange( syswk, ppp ) == TRUE ){
			BOX2MAIN_FormChangeRenew( syswk, syswk->get_pos );
			BOX2OBJ_PokeIconChange(
				syswk, syswk->tray, syswk->get_pos,
				syswk->app->pokeicon_id[syswk->get_pos] );
		}
	}

	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		syswk->get_pos = mvwk->put_pos;
		ItemMoveWorkFree( syswk );
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX );
		BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		return BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN;
	}

	PMSND_PlaySE( SE_BOX2_POKE_CATCH );

	BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
	BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 1 );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
	BOX2OBJ_ItemIconCursorAdd( syswk->app );

	return VFuncSet( syswk, BOX2MAIN_VFuncItemIconPartyChange, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ITEM_PARTY_ICONMOVE_CHANGE) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�莝���|�P�����̃A�C�e������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_PARTY_ICONMOVE_CHANGE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemPartyIconMoveChange( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		PMSND_PlaySE( SE_BOX2_POKE_PUT );
		{
			BOX2MAIN_ITEMMOVE_WORK * mvwk = syswk->app->vfunk.work;
			BOX2OBJ_PokeIconBlendSet( syswk->app, mvwk->put_pos, FALSE );
		}
		// �A�E�g���C������
		BOX2OBJ_PokeCursorVanish( syswk, FALSE );
		// �A�C�e���A�C�R���k��
		BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX );
		ItemMoveWorkFree( syswk );
		syswk->app->sub_seq++;
		break;

	case 1:
		if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
			break;
		}
		BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
		return BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN;
	}

	return BOX2SEQ_MAINSEQ_ITEM_PARTY_ICONMOVE_CHANGE;
}


//============================================================================================
//	�Q������
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�Q������v���C������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_SLEEP_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_SleepMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	ret = BOX2UI_SleepMain( syswk );

	switch( ret ){
	case BOX2UI_SLEEP_MAIN_NAME:		// 30: �{�b�N�X��
		BOX2MAIN_PokeSelectOff( syswk );
		break;

	case BOX2UI_SLEEP_MAIN_LEFT:		// 31: �g���C�؂�ւ����E��
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_SLEEP_MAIN_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_SLEEP_MAIN_NAME );
		return TrayLeftScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_SLEEP_MAIN );

	case BOX2UI_SLEEP_MAIN_RIGHT:		// 32: �g���C�؂�ւ����E�E
		PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_SLEEP_MAIN_NAME );
		BOX2UI_PutHandCursor( syswk, BOX2UI_SLEEP_MAIN_NAME );
		return TrayRightScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_SLEEP_MAIN );

	case BOX2UI_SLEEP_MAIN_RETURN:	// 33: �߂�
		PMSND_PlaySE( SE_BOX2_CANCEL );
		syswk->dat->retMode = BOX_END_MODE_MENU;
		syswk->dat->retTray = BOX_RET_SEL_NONE;
		syswk->dat->retPoke = BOX_RET_SEL_NONE;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case BOX2UI_SLEEP_MAIN_SET:			// 34: �˂�����
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_SLEEP_MENU_SET) );

	case BOX2UI_SLEEP_MAIN_CLOSE:		// 35: ��߂�
		PMSND_PlaySE( SE_BOX2_CANCEL );
		{
			u8	pos = syswk->get_pos;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_SLEEP_MENU_CLOSE );

	case CURSORMOVE_NO_MOVE_LEFT:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_SLEEP_MAIN_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			BOX2MAIN_PokeSelectOff( syswk );
			return TrayLeftScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_SLEEP_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_RIGHT:	// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_SLEEP_MAIN_NAME ){
			PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );
			BOX2MAIN_PokeSelectOff( syswk );
			return TrayRightScrollSet( syswk, FALSE, BOX2SEQ_MAINSEQ_SLEEP_MAIN );
		}
		break;

	case CURSORMOVE_CURSOR_ON:			// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_SLEEP_MAIN_NAME ){
				BOX2MAIN_PokeInfoPut( syswk, pos );
			}else{
				if( pos != BOX2UI_SLEEP_MAIN_SET &&
						pos != BOX2UI_SLEEP_MAIN_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_SLEEP_MAIN) );

	case CURSORMOVE_CANCEL:					// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		// ���j���[�\����
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
			u8	pos = syswk->get_pos;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, BOX2SEQ_MAINSEQ_SLEEP_MENU_CLOSE );
		}
		syswk->dat->retMode = BOX_END_MODE_MENU;
		syswk->dat->retTray = BOX_RET_SEL_NONE;
		syswk->dat->retPoke = BOX_RET_SEL_NONE;
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case CURSORMOVE_NO_MOVE_UP:			// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NO_MOVE_DOWN:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	case CURSORMOVE_NONE:						// ����Ȃ�
		break;

	default:
		if( BOX2MAIN_PokeParaGet( syswk, ret, syswk->tray, ID_PARA_poke_exist, NULL ) != 0 ){
			PMSND_PlaySE( SE_BOX2_DECIDE );
			syswk->get_pos = ret;
			BOX2BMP_MenuStrPrint( syswk, SleepMenuStrTbl, 2 );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2OBJ_PokeCursorAdd( syswk );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_SLEEP_MAIN_SET );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
				return MainSeq_SleepPokeSelect( syswk );
			}else{
				BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_SLEEP_MAIN_SET, ret );
				BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
				return VFuncSet( syswk, BOX2MAIN_VFuncMenuMove, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_SLEEP_POKE_SELECT) );
			}
		}else{
			PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				BOX2OBJ_PokeCursorVanish( syswk, FALSE );
				BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
				BOX2MAIN_ResetTouchBar( syswk );
				return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_SLEEP_MAIN );
			}
		}
	}

	return BOX2SEQ_MAINSEQ_SLEEP_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�Q������|�P�����I��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_SLEEP_POKE_SELECT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_SleepPokeSelect( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
	return BOX2SEQ_MAINSEQ_SLEEP_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�˂�����v�I����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_SLEEP_MENU_SET
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_SleepMenuSet( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_SLEEP_MENU_SET );

	case 1:
		if( BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, syswk->tray, ID_PARA_tamago_flag, NULL ) != 0 ){
			BOX2BMP_PutPokeMoveErrMsg( syswk, BOX2MAIN_ERR_CODE_EGG, BOX2BMPWIN_ID_MSG1 );
			syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );
		}
		if( BOX2MAIN_CheckSleep( syswk, syswk->get_pos ) == FALSE ){
			BOX2BMP_PutPokeMoveErrMsg( syswk, BOX2MAIN_ERR_CODE_SLEEP, BOX2BMPWIN_ID_MSG4 );
			syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );
		}
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );
		BOX2BMP_SleepSelectMsgPut( syswk );
		syswk->app->sub_seq = 0;
		return YesNoSet( syswk, YESNO_ID_SLEEP );
	}

	return BOX2SEQ_MAINSEQ_SLEEP_MENU_SET;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���j���[�����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_SLEEP_MENU_CLOSE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_SleepMenuClose( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	return VFuncSet( syswk, BOX2MAIN_VFuncMenuMove, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_SLEEP_MAIN) );
}


//============================================================================================
//	�T�u�v���Z�X
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�X�e�[�^�X��ʌĂяo��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_STATUS_CALL
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_StatusCall( BOX2_SYS_WORK * syswk )
{
	return SubProcSet( syswk, SUB_PROC_TYPE_POKESTATUS );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�u�������́v���j���[�`�F�b�N
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEM_MENU_CHECK
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemMenuCheck( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		// �o�g���{�b�N�X�����b�N����Ă���
		if( syswk->dat->callMode == BOX_MODE_BATTLE && syswk->dat->bbRockFlg == TRUE ){
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
			BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
			syswk->app->sub_seq = 3;
			return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_ITEM_MENU_CHECK );
		}

		if( BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, syswk->tray, ID_PARA_tamago_flag, NULL ) == 0 ){
			u32 item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, syswk->tray, ID_PARA_item, NULL );
			// ���������Ă��Ȃ�
			if( item == ITEM_DUMMY_DATA ){
				return SubProcSet( syswk, SUB_PROC_TYPE_BAG );
			}
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, FALSE );
			// ���[���������Ă���
			if( ITEM_CheckMail( item ) == TRUE ){
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				syswk->app->sub_seq = 1;
				return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_ITEM_MENU_CHECK );
			}
			BOX2BGWFRM_PokeMenuOff( syswk->app->wfrm );
			BOX2BMP_ItemGetCheckMsgPut( syswk, item, BOX2BMPWIN_ID_MSG1 );
			if( syswk->dat->callMode == BOX_MODE_ITEM ){
				return YesNoSet( syswk, YESNO_ID_ITEM_RET_BAG );
			}else{
				return YesNoSet( syswk, YESNO_ID_ITEMGET );
			}
		}
		// �^�}�S
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		syswk->app->sub_seq = 2;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_ITEM_MENU_CHECK );

	case 1:		// ���[���������Ă���
		PMSND_PlaySE( SE_BOX2_WARNING );
		BOX2BMP_MailGetErrorPut( syswk, BOX2BMPWIN_ID_MSG1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
		return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );

	case 2:		// �^�}�S
		PMSND_PlaySE( SE_BOX2_WARNING );
		BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_EGG, BOX2BMPWIN_ID_MSG1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
		return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );

	case 3:		// �o�g���{�b�N�X�����b�N����Ă���
		PMSND_PlaySE( SE_BOX2_WARNING );
		BOX2BMP_BattleBoxRockMsgPut( syswk, 1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_BATTLEBOX_ROCK;
		return BOX2SEQ_MAINSEQ_TRGWAIT;
	}

	return BOX2SEQ_MAINSEQ_ITEM_MENU_CHECK;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�X�e�[�^�X��ʂ���̕��A
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_STATUS_RCV
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_StatusRcv( BOX2_SYS_WORK * syswk )
{
	int	next;

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
	BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
	if( syswk->mv_cnv_mode == 0 ){
		BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );
	}else{
		BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_BLUE, TRUE );
	}

	switch( syswk->dat->callMode ){
	case BOX_MODE_AZUKERU:		// ��������
		next = RcvStatus_ModeAzukeru( syswk );
		break;
	case BOX_MODE_TURETEIKU:	// ��Ă���
		next = RcvStatus_ModeTureteiku( syswk );
		break;
	case BOX_MODE_SEIRI:			// ������
		next = RcvStatus_ModeSeiri( syswk );
		break;
	case BOX_MODE_BATTLE:			// �o�g���{�b�N�X
		next = RcvStatus_ModeBattleBox( syswk );
		break;
	}

	return FadeInSet( syswk, next );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�������͉�ʂ���̕��A
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_STRIN_RCV
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_StrInRcv( BOX2_SYS_WORK * syswk )
{
	int	next;

	BOX2MAIN_PokeInfoOff( syswk );

	switch( syswk->dat->callMode ){
	case BOX_MODE_TURETEIKU:	// ��Ă���
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_PTIN_MAIN, BOX2UI_PTIN_MAIN_NAME );
		syswk->app->old_cur_pos = BOX2UI_PTIN_MAIN_NAME;
		next = BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
		break;

	case BOX_MODE_SEIRI:			// ������
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MAIN, BOX2UI_ARRANGE_MAIN_NAME );
		syswk->app->old_cur_pos = BOX2UI_ARRANGE_MAIN_NAME;
		next = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		break;

	case BOX_MODE_ITEM:				// �ǂ���������
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_MAIN, BOX2UI_ITEM_MAIN_NAME );
		syswk->app->old_cur_pos = BOX2UI_ITEM_MAIN_NAME;
		next = BOX2SEQ_MAINSEQ_ITEM_MAIN;
		break;

	case BOX_MODE_BATTLE:			// �o�g���{�b�N�X
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BATTLEBOX_MAIN, BOX2UI_BATTLEBOX_MAIN_NAME );
		syswk->app->old_cur_pos = BOX2UI_BATTLEBOX_MAIN_NAME;
		next = BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;
		break;
	}

	return FadeInSet( syswk, next );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�o�b�O��ʂ���̕��A
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BAG_RCV
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BagRcv( BOX2_SYS_WORK * syswk )
{
	int	next;

	switch( syswk->dat->callMode ){
	case BOX_MODE_SEIRI:			// ������
		next = RcvBag_ModeSeiri( syswk );
		break;
	case BOX_MODE_ITEM:				// �ǂ���������
		next = RcvBag_ModeItem( syswk );
		break;
	case BOX_MODE_BATTLE:			// �o�g���{�b�N�X
		next = RcvBag_ModeBattleBox( syswk );
		break;
	}

	return FadeInSet( syswk, next );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F����������������b�Z�[�W�\��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_ITEMSET_MSG
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemSetMsg( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_ItemSetMsgPut( syswk, syswk->subRet, BOX2BMPWIN_ID_MSG1 );
	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		syswk->next_seq = BOX2SEQ_MAINSEQ_ITEM_POKESET_ANM;
	}else{
		syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
	}
	return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );
}


//============================================================================================
//	�}�[�L���O
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�}�[�L���O������������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_MARKING_INIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MarkingInit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, FALSE );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
		BOX2MAIN_ResetTouchBar( syswk );
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_MARKING_INIT );

	case 1:
		syswk->app->pokeMark = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, syswk->tray, ID_PARA_mark, NULL );
		BOX2MAIN_MainDispMarkingChange( syswk, syswk->app->pokeMark );
		BOX2BGWFRM_MarkingFrameInSet( syswk->app->wfrm );
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncMarkingFrameMove, BOX2SEQ_MAINSEQ_MARKING_INIT );

	case 2:
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_MARKING, 0 );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, TRUE );
		BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );
		BOX2BMP_MarkingMsgPut( syswk, BOX2BMPWIN_ID_MSG1 );
		return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_MARKING_MAIN );
	}

	return BOX2SEQ_MAINSEQ_MARKING_INIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�}�[�L���O�������C��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_MARKING_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MarkingMain( BOX2_SYS_WORK * syswk )
{
	switch( CURSORMOVE_MainCont(syswk->app->cmwk) ){
	case BOX2UI_MARKING_MARK1:		// 00: ��
		PMSND_PlaySE( SE_BOX2_DECIDE );
		MarkingSwitch( syswk, 0 );
		break;
	case BOX2UI_MARKING_MARK2:		// 01: ��
		PMSND_PlaySE( SE_BOX2_DECIDE );
		MarkingSwitch( syswk, 1 );
		break;
	case BOX2UI_MARKING_MARK3:		// 02: ��
		PMSND_PlaySE( SE_BOX2_DECIDE );
		MarkingSwitch( syswk, 2 );
		break;
	case BOX2UI_MARKING_MARK4:		// 03: �n�[�g
		PMSND_PlaySE( SE_BOX2_DECIDE );
		MarkingSwitch( syswk, 3 );
		break;
	case BOX2UI_MARKING_MARK5:		// 04: ��
		PMSND_PlaySE( SE_BOX2_DECIDE );
		MarkingSwitch( syswk, 4 );
		break;
	case BOX2UI_MARKING_MARK6:		// 05: ��
		PMSND_PlaySE( SE_BOX2_DECIDE );
		MarkingSwitch( syswk, 5 );
		break;

	case BOX2UI_MARKING_ENTER:		// 06:�u�����Ă��v
		{
			u8	mark = syswk->app->pokeMark;
			BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, syswk->tray, ID_PARA_mark, mark );
			BOX2MAIN_SubDispMarkingChange( syswk, mark );
		}
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return MarkingButtonAnmSet( syswk, 0, BOX2SEQ_MAINSEQ_MARKING_END );

	case BOX2UI_MARKING_CANCEL:	// 07:�u��߂�v
		PMSND_PlaySE( SE_BOX2_CANCEL );
		return MarkingButtonAnmSet( syswk, 1, BOX2SEQ_MAINSEQ_MARKING_END );

	case CURSORMOVE_CANCEL:			// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		return MarkingButtonAnmSet( syswk, 1, BOX2SEQ_MAINSEQ_MARKING_END );

	case CURSORMOVE_CURSOR_MOVE:	// �ړ�
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_MARKING_CURMOVE_RET );

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		break;
	}

	return BOX2SEQ_MAINSEQ_MARKING_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�}�[�L���O�����I��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_MARKING_END
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MarkingEnd( BOX2_SYS_WORK * syswk )
{
	// �L�[����ݒ�
	switch( syswk->dat->callMode ){
	case BOX_MODE_AZUKERU:
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_PTOUT_MAIN, BOX2UI_PTOUT_MAIN_MARKING );
		break;
	case BOX_MODE_TURETEIKU:
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_PTIN_MAIN, BOX2UI_PTIN_MAIN_MARKING );
		break;
	case BOX_MODE_SEIRI:
	default:
		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MAIN, BOX2UI_ARRANGE_MAIN_MARKING );
		}else{
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_MARKING );
		}
		break;
	}
	BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );

	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );

	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
	}
	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );

	BOX2BGWFRM_MarkingFrameOutSet( syswk->app->wfrm );

	PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );

	return VFuncSet( syswk, BOX2MAIN_VFuncMarkingFrameMove, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�}�[�L���O�����J�[�\���ړ���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_MARKING_CURMOVE_RET
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MarkingCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
	return BOX2SEQ_MAINSEQ_MARKING_MAIN;
}


//============================================================================================
//	������
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�|�P�����𓦂����J�n
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_POKEFREE_INIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PokeFreeInit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, FALSE );
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_POKEFREE_INIT );

	case 1:
		if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
			POKEMON_PARAM * pp;
			u16	item;
			u32	pos;

			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, TRUE );
			BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );

			pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;

			// �莝���P
			if( BOX2MAIN_BattlePokeCheck( syswk, pos ) == FALSE ){
				PMSND_PlaySE( SE_BOX2_WARNING );
				BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_ONE, BOX2BMPWIN_ID_MSG1 );
				syswk->app->poke_free_err = 1;
				syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
				return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );
			}
			// ���[��
			pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, pos );
			item = PP_Get( pp, ID_PARA_item, NULL );
			if( ITEM_CheckMail( item ) == TRUE ){
				PMSND_PlaySE( SE_BOX2_WARNING );
				BOX2BMP_PokeSelectMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_MAIL, BOX2BMPWIN_ID_MSG1 );
				syswk->app->poke_free_err = 1;
				syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
				return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );
			}
		}else{
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
			BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );
		}
		{
			POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );
			// �^�}�S
			if( PPP_Get( ppp, ID_PARA_tamago_flag, NULL ) != 0 ){
				PMSND_PlaySE( SE_BOX2_WARNING );
				BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_EGG, BOX2BMPWIN_ID_MSG1 );
				syswk->app->poke_free_err = 1;
				syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
				return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );
			}
		}
		BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_CHECK, BOX2BMPWIN_ID_MSG1 );
		return ChangeSequence( syswk, YesNoSet(syswk,YESNO_ID_POKEFREE) );
	}

	return BOX2SEQ_MAINSEQ_POKEFREE_INIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�|�P�����𓦂����A�j��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_POKEFREE_ANM
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PokeFreeAnm( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_PokeFreeWazaCheck( syswk );

	if( BOX2OBJ_PokeIconFreeMove( syswk->app->seqwk ) == FALSE ){
		BOX2MAIN_POKEFREE_WORK * wk;
		u8	waza_flg;
		wk = syswk->app->seqwk;
		waza_flg = wk->check_flg;
		if( waza_flg != 0 ){
			// �߂�������
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_POKEFREE_ERROR );
		}else{
			// ����������
			BOX2OBJ_PokeIconFreeEnd( syswk->app->seqwk );
			BOX2MAIN_PokeFreeExit( syswk );
			return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_POKEFREE_ENTER );
		}
	}
	return BOX2SEQ_MAINSEQ_POKEFREE_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�|�P�����𓦂������s����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_POKEFREE_ENTER
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PokeFreeEnter( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_ENTER, BOX2BMPWIN_ID_MSG1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_POKEFREE_ENTER;
		syswk->app->sub_seq++;
		return BOX2SEQ_MAINSEQ_TRGWAIT;

	case 1:
		BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_BY, BOX2BMPWIN_ID_MSG1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_POKEFREE_ENTER;
		syswk->app->sub_seq++;
		return BOX2SEQ_MAINSEQ_TRGWAIT;

	case 2:
		BOX2MAIN_PokeDataClear( syswk, syswk->tray, syswk->get_pos );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
		BOX2MAIN_ResetTouchBar( syswk );
		BOX2MAIN_PokeInfoOff( syswk );

		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			// �g���C�A�C�R���X�V
			BOX2OBJ_TrayIconCgxMake( syswk, syswk->tray );
			BOX2OBJ_TrayIconCgxTrans( syswk, syswk->tray );
			if( syswk->dat->callMode == BOX_MODE_TURETEIKU ){
				syswk->next_seq = BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
			}else{
				syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
				BOX2BGWFRM_TemochiButtonOn( syswk->app );
			}

			CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
			BOX2UI_PutHandCursor( syswk, syswk->get_pos );
			BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );

			BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );

			BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );

			syswk->get_pos = BOX2MAIN_GETPOS_NONE;

			syswk->app->sub_seq = 0;
			return syswk->next_seq;
		}else{
			PokeMoveWorkAlloc( syswk );
			syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_PARTY;
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );
			syswk->app->sub_seq++;
			return VFuncSet( syswk, BOX2MAIN_VFuncPartyPokeFreeSort, BOX2SEQ_MAINSEQ_POKEFREE_ENTER );
		}

	case 3:
		syswk->app->sub_seq = 0;
		PokeMoveWorkFree( syswk );
		BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX );
		BOX2UI_PutHandCursor( syswk, syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX );
		BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );

		syswk->get_pos = BOX2MAIN_GETPOS_NONE;
		if( syswk->dat->callMode == BOX_MODE_AZUKERU ){
			BOX2BMP_PokeSelectMsgPut(
				syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG1 );
			return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
		}
		BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
		return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
	}

	return BOX2SEQ_MAINSEQ_POKEFREE_ENTER;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�|�P�����𓦂������s����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_POKEFREE_ERROR
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PokeFreeError( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		if( BOX2OBJ_PokeIconFreeErrorMove( syswk->app->seqwk ) == FALSE ){
			BOX2OBJ_PokeIconFreeErrorEnd( syswk->app->seqwk );
			BOX2MAIN_PokeFreeExit( syswk );
			BOX2OBJ_PokeCursorAdd( syswk );
			BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_RETURN, BOX2BMPWIN_ID_MSG1 );
			syswk->next_seq = BOX2SEQ_MAINSEQ_POKEFREE_ERROR;
			syswk->app->sub_seq++;
			return BOX2SEQ_MAINSEQ_TRGWAIT;
		}
		break;

	case 1:
		BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_FEAR, BOX2BMPWIN_ID_MSG1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_POKEFREE_ERROR;
		syswk->app->sub_seq++;
		return BOX2SEQ_MAINSEQ_TRGWAIT;

	case 2:
		syswk->app->sub_seq = 0;
		return BoxArg_PokeFreeNo( syswk );
	}

	return BOX2SEQ_MAINSEQ_POKEFREE_ERROR;
}


//============================================================================================
//	�{�b�N�X�e�[�}�ύX
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�b�N�X�e�[�}�ύX��������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BOXTHEMA_INIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaInit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		syswk->app->sub_seq++;
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, FALSE );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		// ���j���[
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
			if( BOX2OBJ_VanishCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
				BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
			}
			BOX2OBJ_PokeCursorVanish( syswk, FALSE );
			BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
			BOX2MAIN_ResetTouchBar( syswk );
			BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
			return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_BOXTHEMA_INIT );
		}
		// �g���C�t���[��
		if( BOX2BGWFRM_CheckBoxMoveFrm(syswk->app->wfrm) == TRUE ){
			BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
			BOX2BGWFRM_TemochiButtonOff( syswk->app );
			return BoxMoveFrmOutSet( syswk, BOX2SEQ_MAINSEQ_BOXTHEMA_INIT );
		}
		// �莝���t���[��
		if( BOX2BGWFRM_CheckPartyPokeFrameRight(syswk->app->wfrm) == TRUE ){
			BOX2BGWFRM_BoxListButtonOff( syswk->app );
			BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
			return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_BOXTHEMA_INIT );
		}
		BOX2BGWFRM_TemochiButtonOff( syswk->app );

	case 1:
		BOX2BMP_MenuStrPrint( syswk, ThemaMenuStrTbl, 4 );
		BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_BOXTHEMA_INIT );

	case 2:
		syswk->app->sub_seq = 0;
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		BOX2BMP_BoxThemaMsgPut( syswk, BOX2BMP_MSGID_THEMA_INIT, BOX2BMPWIN_ID_MSG1 );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BOXTHEMA_CHG, 0 );
		syswk->mv_cnv_mode = 0;
		BOX2OBJ_SetHandCursorAnm( syswk, BOX2OBJ_ANM_HAND_NORMAL );
		return BOX2SEQ_MAINSEQ_BOXTHEMA_MAIN;
	}

	return BOX2SEQ_MAINSEQ_BOXTHEMA_INIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�b�N�X�e�[�}�ύX���C������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BOXTHEMA_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaMain( BOX2_SYS_WORK * syswk )
{
	switch( CURSORMOVE_MainCont(syswk->app->cmwk) ){
	case BOX2UI_BOXTHEMA_MENU1:			// 00: �W�����v����
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU3, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_INIT) );

	case BOX2UI_BOXTHEMA_MENU2:			// 01: ���ׂ���
		PMSND_PlaySE( SE_BOX2_DECIDE );
		syswk->app->wp_menu = 0;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT) );

	case BOX2UI_BOXTHEMA_MENU3:			// 02: �Ȃ܂�
		PMSND_PlaySE( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_BOXTHEMA_NAMEIN_CALL );

	case BOX2UI_BOXTHEMA_MENU4:			// 03: ��߂�
		PMSND_PlaySE( SE_BOX2_CANCEL );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_EXIT) );

	case CURSORMOVE_CANCEL:					// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_EXIT) );

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_MAIN) );
	}

	return BOX2SEQ_MAINSEQ_BOXTHEMA_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�b�N�X�e�[�}�ύX�I������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BOXTHEMA_EXIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaExit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
		BOX2MAIN_ResetTouchBar( syswk );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_BOXTHEMA_EXIT );

	case 1:
		syswk->app->sub_seq = 0;
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );
		if( syswk->dat->callMode == BOX_MODE_SEIRI ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MAIN, BOX2UI_ARRANGE_MAIN_NAME );
			return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		}else if( syswk->dat->callMode == BOX_MODE_TURETEIKU ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_PTIN_MAIN, BOX2UI_PTIN_MAIN_NAME );
			return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
		}else if( syswk->dat->callMode == BOX_MODE_ITEM ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_MAIN, BOX2UI_ITEM_MAIN_NAME );
			return BOX2SEQ_MAINSEQ_ITEM_MAIN;
		}else if( syswk->dat->callMode == BOX_MODE_BATTLE ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BATTLEBOX_MAIN, BOX2UI_BATTLEBOX_MAIN_NAME );
			return BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;
		}
	}

	return BOX2SEQ_MAINSEQ_BOXTHEMA_EXIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�b�N�X�؂�ւ���������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_INIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaJumpInit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		syswk->app->jump_tray = 1;
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
		BOX2MAIN_ResetTouchBar( syswk );
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_INIT );

	case 1:
		syswk->box_mv_pos = syswk->tray;	// ���ݎQ�Ƃ��Ă���{�b�N�X
		syswk->app->sub_seq++;
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		return BoxMoveFrmInSet( syswk, BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_INIT );

	case 2:
		syswk->app->sub_seq = 0;
		BOX2OBJ_ChangeTrayName( syswk, 0, TRUE );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BOXJUMP_MAIN, BOX2UI_BOXJUMP_TRAY2 );
		syswk->app->old_cur_pos = BOX2UI_BOXJUMP_TRAY2;
		BOX2BMP_BoxThemaMsgPut( syswk, BOX2BMP_MSGID_THEMA_JUMP, BOX2BMPWIN_ID_MSG2 );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );
		return BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_MAIN;
	}

	return BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_INIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�b�N�X�؂�ւ����C������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaJumpMain( BOX2_SYS_WORK * syswk )
{
	u32	x, y;
	u32	ret;

	// �^�b�`�g���C�X�N���[���`�F�b�N
	if( BOX2UI_HitCheckTrayScrollStart( &x, &y ) == TRUE ){
		syswk->app->tpy = y;
		syswk->next_seq = BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_MAIN;
		CreateTrayIconScrollWork( syswk );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
		return BOX2SEQ_MAINSEQ_TRAYSCROLL_TOUCH;
	}

	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	switch( ret ){
	case BOX2UI_BOXJUMP_TRAY1:		// 00: �g���C�A�C�R��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		BOX2MAIN_InitTrayIconScroll( syswk, -1 );
		return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollDownJump, BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_MAIN );

	case BOX2UI_BOXJUMP_TRAY6:		// 05: �g���C�A�C�R��
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		BOX2MAIN_InitTrayIconScroll( syswk, 1 );
		return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollUpJump, BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_MAIN );

	case BOX2UI_BOXJUMP_TRAY2:		// 01: �g���C�A�C�R��
	case BOX2UI_BOXJUMP_TRAY3:		// 02: �g���C�A�C�R��
	case BOX2UI_BOXJUMP_TRAY4:		// 03: �g���C�A�C�R��
	case BOX2UI_BOXJUMP_TRAY5:		// 04: �g���C�A�C�R��
		syswk->app->chg_tray_pos = syswk->box_mv_pos + ret - BOX2UI_BOXJUMP_TRAY2;
		if( syswk->app->chg_tray_pos >= syswk->trayMax ){
			syswk->app->chg_tray_pos -= syswk->trayMax;
		}
		if( syswk->app->chg_tray_pos != syswk->tray ){
			BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
			return BoxMoveTrayScrollSet( syswk, 1 );
		}else{
			PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
			BOX2OBJ_ChangeTrayName( syswk, ret-BOX2UI_BOXJUMP_TRAY2, TRUE );
		}
		break;

	case BOX2UI_BOXJUMP_RET:			// 06: �߂�
	case CURSORMOVE_CANCEL:					// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		return ObjButtonAnmSet( syswk, BOX2OBJ_ID_TB_CANCEL, 9, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_EXIT) );

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_MAIN) );

	case CURSORMOVE_NO_MOVE_UP:			// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_BOXJUMP_TRAY2 ){
			PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
			BOX2MAIN_InitTrayIconScroll( syswk, -1 );
			return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollDownJump, BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_MAIN );
		}
		break;

	case CURSORMOVE_NO_MOVE_DOWN:		// �\���L�[���������ꂽ���A�ړ��Ȃ�
		if( CURSORMOVE_PosGet( syswk->app->cmwk ) == BOX2UI_BOXJUMP_TRAY5 ){
			PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
			BOX2MAIN_InitTrayIconScroll( syswk, 1 );
			return VFuncSet( syswk, BOX2MAIN_VFuncTrayIconScrollUpJump, BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_MAIN );
		}
		break;
	}

	return BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�b�N�X�؂�ւ��I������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_EXIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaJumpExit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		syswk->app->jump_tray = 0;
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG2 );
		BOX2MAIN_ResetTouchBar( syswk );
		BOX2OBJ_ChangeTrayName( syswk, 0, FALSE );
		PMSND_PlaySE( SE_BOX2_OPEN_PARTY_TRAY );
		syswk->app->sub_seq++;
		return BoxMoveFrmOutSet( syswk, BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_EXIT );

	case 1:
		syswk->app->sub_seq = 0;
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_ON, BOX2OBJ_TB_ICON_OFF );
		BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );

		if( syswk->dat->callMode == BOX_MODE_SEIRI ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MAIN, BOX2UI_ARRANGE_MAIN_NAME );
			return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		}else if( syswk->dat->callMode == BOX_MODE_TURETEIKU ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_PTIN_MAIN, BOX2UI_PTIN_MAIN_NAME );
			return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
		}else if( syswk->dat->callMode == BOX_MODE_ITEM ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_MAIN, BOX2UI_ITEM_MAIN_NAME );
			return BOX2SEQ_MAINSEQ_ITEM_MAIN;
		}else if( syswk->dat->callMode == BOX_MODE_BATTLE ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BATTLEBOX_MAIN, BOX2UI_BATTLEBOX_MAIN_NAME );
			return BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;
		}
	}

	return BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_EXIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�b�N�X�؂�ւ���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_RET
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaJumpRet( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_ChangeTrayName( syswk, CURSORMOVE_PosGet(syswk->app->cmwk)-BOX2UI_BOXJUMP_TRAY2, TRUE );
	return BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�ǎ��I����������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaWallPaperInit( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
		BOX2MAIN_ResetTouchBar( syswk );
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT );

	case 1:
		if( syswk->app->wp_menu == 0 ){
			if( BOXDAT_GetExWallPaperFlag( syswk->dat->sv_box, BOX_EX_WALLPAPER_SET_FLAG_1 ) == TRUE ){
				BOX2BMP_MenuStrPrint( syswk, WallPaperMenuSpecialStrTbl, 6 );
			}else{
				BOX2BMP_MenuStrPrint( syswk, WallPaperMenuStrTbl, 5 );
			}
		}else if( syswk->app->wp_menu == 1 ){
			BOX2BMP_MenuStrPrint( syswk, WPLabel1MenuStrTbl, 5 );
		}else if( syswk->app->wp_menu == 2 ){
			BOX2BMP_MenuStrPrint( syswk, WPLabel2MenuStrTbl, 5 );
		}else if( syswk->app->wp_menu == 3 ){
			BOX2BMP_MenuStrPrint( syswk, WPLabel3MenuStrTbl, 5 );
		}else if( syswk->app->wp_menu == 4 ){
			BOX2BMP_MenuStrPrint( syswk, WPLabel4MenuStrTbl, 5 );
		}else if( syswk->app->wp_menu == 5 ){
			if( BOXDAT_GetExWallPaperFlag( syswk->dat->sv_box, BOX_EX_WALLPAPER_SET_FLAG_2 ) == TRUE ){
				BOX2BMP_MenuStrPrint( syswk, WPLabelS11MenuStrTbl, 6 );
			}else{
				BOX2BMP_MenuStrPrint( syswk, WPLabelS01MenuStrTbl, 5 );
			}
		}else if( syswk->app->wp_menu == 6 ){
			BOX2BMP_MenuStrPrint( syswk, WPLabelS02MenuStrTbl, 6 );
		}
		BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
		syswk->app->sub_seq++;
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT );

	case 2:
		syswk->app->sub_seq = 0;
		if( syswk->app->wp_menu == 0 ){
			BOX2BMP_BoxThemaMsgPut( syswk, BOX2BMP_MSGID_THEMA_MES, BOX2BMPWIN_ID_MSG1 );
		}else{
			BOX2BMP_BoxThemaMsgPut( syswk, BOX2BMP_MSGID_THEMA_WALL, BOX2BMPWIN_ID_MSG1 );
		}
		{	// �J�[�\���ړ��ݒ�
			BOOL	flg = FALSE;
			// ���j���[�P�𖳌��ɂ���
			if( syswk->app->wp_menu == 0 ){
				if( BOXDAT_GetExWallPaperFlag( syswk->dat->sv_box, BOX_EX_WALLPAPER_SET_FLAG_1 ) == FALSE ){
					flg = TRUE;
				}
			}else if( syswk->app->wp_menu == 5 ){
				if( BOXDAT_GetExWallPaperFlag( syswk->dat->sv_box, BOX_EX_WALLPAPER_SET_FLAG_2 ) == FALSE ){
					flg = TRUE;
				}
			}else if( syswk->app->wp_menu != 6 ){
				flg = TRUE;
			}
			if( flg == TRUE ){
				BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_WALLPAPER_CHG, BOX2UI_WALLPAPER_MENU2 );
				CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_WALLPAPER_MENU1 );
			}else{
				BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_WALLPAPER_CHG, BOX2UI_WALLPAPER_MENU1 );
			}
		}
		return BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_MAIN;
	}

	return BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�ǎ��I�����C��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_MAIN
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaWallPaperMain( BOX2_SYS_WORK * syswk )
{
	switch( CURSORMOVE_MainCont(syswk->app->cmwk) ){
	case BOX2UI_WALLPAPER_MENU1:			// 00: ���j���[�P
		PMSND_PlaySE( SE_BOX2_DECIDE );
		// �g�b�v���j���[
		if( syswk->app->wp_menu == 0 ){
			syswk->app->wp_menu = 5;
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU1, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT) );
		}
		return SetWallPaperChange( syswk, 0 );

	case BOX2UI_WALLPAPER_MENU2:			// 01: ���j���[�Q
		PMSND_PlaySE( SE_BOX2_DECIDE );
		if( syswk->app->wp_menu == 0 ){
			syswk->app->wp_menu = 1;
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU2, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT) );
		}
		return SetWallPaperChange( syswk, 1 );

	case BOX2UI_WALLPAPER_MENU3:			// 02: ���j���[�R
		PMSND_PlaySE( SE_BOX2_DECIDE );
		if( syswk->app->wp_menu == 0 ){
			syswk->app->wp_menu = 2;
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU3, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT) );
		}
		return SetWallPaperChange( syswk, 2 );

	case BOX2UI_WALLPAPER_MENU4:			// 03: ���j���[�S
		PMSND_PlaySE( SE_BOX2_DECIDE );
		if( syswk->app->wp_menu == 0 ){
			syswk->app->wp_menu = 3;
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT) );
		}
		return SetWallPaperChange( syswk, 3 );

	case BOX2UI_WALLPAPER_MENU5:			// 04: ���j���[�T
		PMSND_PlaySE( SE_BOX2_DECIDE );
		// �g�b�v���j���[
		if( syswk->app->wp_menu == 0 ){
			syswk->app->wp_menu = 4;
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT) );
		}
		// �X�y�V�����P
		if( syswk->app->wp_menu == 5 ){
			if( BOXDAT_GetExWallPaperFlag( syswk->dat->sv_box, BOX_EX_WALLPAPER_SET_FLAG_2 ) == TRUE ){
				syswk->app->wp_menu = 6;
				return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT) );
			}
		}
		// �X�y�V�����Q
		if( syswk->app->wp_menu == 6 ){
			syswk->app->wp_menu = 5;
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT) );
		}
		return SetWallPaperChange( syswk, 4 );

	case BOX2UI_WALLPAPER_MENU6:			// 05: ��߂�
		PMSND_PlaySE( SE_BOX2_CANCEL );
		if( syswk->app->wp_menu == 0 ){
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_EXIT) );
		}
		syswk->app->wp_menu = 0;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT) );

	case CURSORMOVE_CANCEL:						// �L�����Z��
		PMSND_PlaySE( SE_BOX2_CANCEL );
		if( syswk->app->wp_menu == 0 ){
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_EXIT) );
		}
		syswk->app->wp_menu = 0;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU6, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_INIT) );

	case CURSORMOVE_CURSOR_MOVE:			// �ړ�
		PMSND_PlaySE( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_MAIN) );
	}

	return BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�ǎ��ύX
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_CHANGE
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaWallPaperChange( BOX2_SYS_WORK * syswk )
{
	switch( syswk->app->sub_seq ){
	case 0:
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
		PaletteWorkSet_VramCopy( syswk->app->pfd, FADE_MAIN_BG, 0, FADE_PAL_ALL_SIZE );
		PaletteFadeReq( syswk->app->pfd, PF_BIT_MAIN_BG, 0xc000, 0, 0, 16, 0x7fff, GFUser_VIntr_GetTCBSYS() );
		syswk->next_seq = BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_CHANGE;
		syswk->app->sub_seq++;
		return BOX2SEQ_MAINSEQ_PALETTE_FADE;

	case 1:
		BOX2MAIN_WallPaperChange( syswk, syswk->app->wallpaper_pos );
		BOXDAT_SetWallPaperNumber( syswk->dat->sv_box, syswk->tray, syswk->app->wallpaper_pos );
		syswk->app->sub_seq++;
		break;

	case 2:
		PaletteFadeReq( syswk->app->pfd, PF_BIT_MAIN_BG, 0xc000, 0, 16, 0, 0x7fff, GFUser_VIntr_GetTCBSYS() );
		syswk->next_seq = BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_CHANGE;
		syswk->app->sub_seq++;
		return BOX2SEQ_MAINSEQ_PALETTE_FADE;

	case 3:
		BOX2OBJ_SetHandCursorOnOff( syswk, TRUE );
		syswk->app->sub_seq = 0;
		return BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_MAIN;
	}

	return BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_CHANGE;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�������͉�ʌĂяo��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BOXTHEMA_NAMEIN_CALL
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaNameInCall( BOX2_SYS_WORK * syswk )
{
	return SubProcSet( syswk, SUB_PROC_TYPE_NAMEIN );
}


//============================================================================================
//	�I������
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�b�N�X�I���u�͂��E�������v�Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	��`�FBOX2SEQ_MAINSEQ_BOXEND_INIT
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxEndInit( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, FALSE );
	BOX2BMP_BoxEndMsgPut( syswk, BOX2BMPWIN_ID_MSG1 );
	BoxEndPassiveSet( syswk );
	return YesNoSet( syswk, YESNO_ID_BOX_END_CANCEL );
}


//============================================================================================
//============================================================================================
//	���̑�
//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�V�[�P���X�؂�ւ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		next		���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int ChangeSequence( BOX2_SYS_WORK * syswk, int next )
{
	syswk->app->sub_seq = 0;
	return next;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�C���ݒ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		next		�t�F�[�h��̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int FadeInSet( BOX2_SYS_WORK * syswk, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BOX_APP );

	syswk->app->wipe_seq = next;

	return BOX2SEQ_MAINSEQ_WIPE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�A�E�g�ݒ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		next		�t�F�[�h��̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int FadeOutSet( BOX2_SYS_WORK * syswk, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BOX_APP );

	syswk->app->wipe_seq = next;

	return BOX2SEQ_MAINSEQ_WIPE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�E�F�C�g�ݒ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		next		�E�F�C�g��̃V�[�P���X
 * @param		wait		�E�F�C�g
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WaitSet( BOX2_SYS_WORK * syswk, int next, int wait )
{
	syswk->next_seq  = next;
	syswk->app->wait = wait;
	return BOX2SEQ_MAINSEQ_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		func		VBLANK�֐�
 * @param		next		VBLANK�֐��I����̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int VFuncSet( BOX2_SYS_WORK * syswk, void * func, int next )
{
	syswk->app->vnext_seq = next;
	BOX2MAIN_VFuncSet( syswk->app, func );
	return BOX2SEQ_MAINSEQ_VFUNC;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ݒ�ς݂�VBLANK�֐��J�n���N�G�X�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		next		VBLANK�֐��I����̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int VFuncReqSet( BOX2_SYS_WORK * syswk, int next )
{
	syswk->app->vnext_seq = next;
	BOX2MAIN_VFuncReqSet( syswk->app );
	return BOX2SEQ_MAINSEQ_VFUNC;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ���̃V�[�P���X��ݒ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		next		�J�[�\���ړ���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int SetCursorMoveRet( BOX2_SYS_WORK * syswk, int next )
{
	syswk->next_seq = next;
	return BOX2SEQ_MAINSEQ_CURSORMOVE_RET;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�͂��E�������Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		id			�͂��E�������h�c
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int YesNoSet( BOX2_SYS_WORK * syswk, u32 id )
{
	syswk->app->ynID = id;
	BOX2MAIN_YesNoWinSet( syswk, 0 );
	GFL_UI_KEY_SetRepeatSpeed( syswk->app->key_repeat_speed, syswk->app->key_repeat_wait );
	return BOX2SEQ_MAINSEQ_YESNO;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�͂��E�����������F�����a���� >> �͂�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxArg_ItemGetYes( BOX2_SYS_WORK * syswk )
{
	u32 item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, syswk->tray, ID_PARA_item, NULL );

	// �A�C�e���`�F�b�N
	if( MYITEM_AddItem( syswk->dat->myitem, item, 1, HEAPID_BOX_APP ) == TRUE ){
		POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );

		BOX2BMP_ItemGetMsgPut( syswk, item, BOX2BMPWIN_ID_MSG1 );
		item = 0;
		BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, syswk->tray, ID_PARA_item, item );
		if( BOX2MAIN_PokeItemFormChange( syswk, ppp ) == TRUE ){
			BOX2MAIN_FormChangeRenew( syswk, syswk->get_pos );
			BOX2OBJ_PokeIconChange(
				syswk, syswk->tray, syswk->get_pos,
				syswk->app->pokeicon_id[syswk->get_pos] );
		}
		BOX2MAIN_PokeInfoRewrite( syswk, syswk->get_pos );
	}else{
		PMSND_PlaySE( SE_BOX2_WARNING );
		BOX2BMP_ItemGetErrorPut( syswk, BOX2BMPWIN_ID_MSG1 );
	}

	if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
	}else{
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
	}

	syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
	return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�͂��E�����������F�|�P�����𓦂��� >> �͂�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxArg_PokeFreeYes( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	BOX2MAIN_PokeFreeCreate( syswk );
	BOX2OBJ_PokeIconFreeStart( syswk->app->seqwk );

	if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
	}else{
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
	}

	return BOX2SEQ_MAINSEQ_POKEFREE_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�͂��E�����������F�|�P�����𓦂��� >> ������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxArg_PokeFreeNo( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_PokeCursorVanish( syswk, TRUE );

	BOX2BMP_SysWinVanishTrans( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );

	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
	}else{
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );
	}

	if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
	}else{
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV) );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�͂��E�����������F�o�b�O�֖߂� >> �͂�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxItemArg_RetBagYes( BOX2_SYS_WORK * syswk )
{
	if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
	}else{
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
	}

	if( MYITEM_AddItem( syswk->dat->myitem, syswk->app->get_item, 1, HEAPID_BOX_APP ) == TRUE ){
		POKEMON_PASO_PARAM * ppp;
		u16	item;

		ppp  = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );
		item = ITEM_DUMMY_DATA;

		BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, syswk->tray, ID_PARA_item, item );

		if( BOX2MAIN_PokeItemFormChange( syswk, ppp ) == TRUE ){
			BOX2MAIN_FormChangeRenew( syswk, syswk->get_pos );
			BOX2OBJ_PokeIconChange( syswk, syswk->tray, syswk->get_pos, syswk->app->pokeicon_id[syswk->get_pos] );
		}
		BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

		BOX2OBJ_PokeCursorVanish( syswk, FALSE );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
		BOX2MAIN_ResetTouchBar( syswk );
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );
		return BOX2SEQ_MAINSEQ_ITEM_BAGIN_ANM;
	}

	PMSND_PlaySE( SE_BOX2_WARNING );
	BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_MAX, BOX2BMPWIN_ID_MSG1 );
	syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
	return ChangeSequence( syswk, BOX2SEQ_MAINSEQ_TRGWAIT );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�͂��E�����������F�{�b�N�X�I�� >> �͂�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxArg_BoxEndYes( BOX2_SYS_WORK * syswk )
{
	PMSND_PlaySE( SE_BOX2_LOG_OFF );
	BOX2MAIN_CheckPartyPerapu( syswk );			// �y���b�v�{�C�X
	syswk->next_seq = BOX2SEQ_MAINSEQ_END;
	return FadeOutSet( syswk, BOX2SEQ_MAINSEQ_RELEASE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�͂��E�����������F�{�b�N�X�I�� >> ������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxArg_BoxEndNo( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );

	if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
	}else{
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	}

	BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );

	switch( syswk->dat->callMode ){
	case BOX_MODE_AZUKERU:		// ��������
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );
		return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;

	case BOX_MODE_TURETEIKU:	// ��Ă���
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
		return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;

	case BOX_MODE_SEIRI:		// ������
		if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == TRUE ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );
			BOX2BGWFRM_BoxListButtonOn( syswk->app );
			return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
		}else{
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
			return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		}
		break;

	case BOX_MODE_ITEM:			// �ǂ���������
		if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == TRUE ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, FALSE );
			BOX2BGWFRM_BoxListButtonOn( syswk->app );
			return BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN;
		}else{
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, FALSE );
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
			return BOX2SEQ_MAINSEQ_ITEM_MAIN;
		}
		break;

	case BOX_MODE_BATTLE:		// �o�g���{�b�N�X
		if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == TRUE ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );
			BOX2BGWFRM_BoxListButtonOn( syswk->app );
			return BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN;
		}else{
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
			return BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;
		}
		break;

	case BOX_MODE_SLEEP:	// �Q������
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
		return BOX2SEQ_MAINSEQ_SLEEP_MAIN;
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�͂��E�����������F�Q������ >> �͂�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxArg_SleepYes( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_CheckPartyPerapu( syswk );			// �y���b�v�{�C�X
	syswk->dat->retTray = syswk->tray;
	syswk->dat->retPoke = syswk->get_pos;
	syswk->next_seq = BOX2SEQ_MAINSEQ_END;
	return FadeOutSet( syswk, BOX2SEQ_MAINSEQ_RELEASE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�͂��E�����������F�Q������ >> ������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxArg_SleepNo( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG4 );
	BOX2MAIN_ResetTouchBar( syswk );
	CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );

	if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
	}else{
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	}

	return BOX2SEQ_MAINSEQ_SLEEP_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�͂��E�����������F���j���[�ɖ߂�i�ėp�j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int YesNo_MenuRcv( BOX2_SYS_WORK * syswk )
{
	if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
	}else{
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );
	}
	return MainSeq_ArrangePokeMenuRcv( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�{�^���A�j���Z�b�g
 *
 * @param		syswk			�{�b�N�X��ʃV�X�e�����[�N
 * @param		wfrmID		�a�f�E�B���h�E�t���[���h�c
 * @param		next			�{�^���A�j����̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BgButtonAnmSet( BOX2_SYS_WORK * syswk, u32 wfrmID, int next )
{
	BOX2MAIN_InitBgButtonAnm( syswk, wfrmID );
	syswk->next_seq = next;
	return BOX2SEQ_MAINSEQ_BUTTON_ANM;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�}�[�L���O�t���[���p�a�f�{�^���A�j���Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		no			�{�^���ԍ�
 * @param		next		�{�^���A�j����̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MarkingButtonAnmSet( BOX2_SYS_WORK * syswk, u32 no, int next )
{
	syswk->app->bawk.btn_mode = BOX2MAIN_BTN_ANM_MODE_BG;
	syswk->app->bawk.btn_id   = BGWINFRM_BGFrameGet( syswk->app->wfrm, BOX2MAIN_WINFRM_MARK );
	syswk->app->bawk.btn_pal1 = BOX2MAIN_BG_PAL_SELWIN + 1;
	syswk->app->bawk.btn_pal2 = BOX2MAIN_BG_PAL_SELWIN;
	syswk->app->bawk.btn_seq  = 0;
	syswk->app->bawk.btn_cnt  = 0;

	if( no == 0 ){
		syswk->app->bawk.btn_py = 14;
	}else{
		syswk->app->bawk.btn_py = 17;
	}
	syswk->app->bawk.btn_px = 21;
	syswk->app->bawk.btn_sx = 11;
	syswk->app->bawk.btn_sy = 3;

	syswk->next_seq = next;
	return BOX2SEQ_MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n�a�i�{�^���A�j���Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		objID		�a�f�E�B���h�E�t���[���h�c
 * @param		anm			�A�j���ԍ�
 * @param		next		�{�^���A�j����̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int ObjButtonAnmSet( BOX2_SYS_WORK * syswk, u32 objID, u32 anm, int next )
{
	u16	sx, sy;
	s8	px, py;

	syswk->app->bawk.btn_mode = BOX2MAIN_BTN_ANM_MODE_OBJ;
	syswk->app->bawk.btn_id   = objID;
	syswk->app->bawk.btn_pal1 = anm;
	syswk->app->bawk.btn_pal2 = 0;
	syswk->app->bawk.btn_seq  = 0;
	syswk->app->bawk.btn_cnt  = 0;

	syswk->app->bawk.btn_px   = 0;
	syswk->app->bawk.btn_py   = 0;
	syswk->app->bawk.btn_sx   = 0;
	syswk->app->bawk.btn_sy   = 0;

	syswk->next_seq = next;
	return BOX2SEQ_MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�T�u�v���Z�X�Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		type		�T�u�v���Z�X�^�C�v
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int SubProcSet( BOX2_SYS_WORK * syswk, u8 type )
{
	// �J�[�\���̏�����Ԃ�ݒ�
	if( CURSORMOVE_CursorOnOffGet( syswk->app->cmwk ) == TRUE ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
	}else{
		GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
	}
	syswk->subProcType = type;
	syswk->next_seq = BOX2SEQ_MAINSEQ_SUB_PROCCALL;
	return FadeOutSet( syswk, BOX2SEQ_MAINSEQ_RELEASE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�o�b�O��ʕ��A�����y�{�b�N�X�����z
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int RcvBag_ModeSeiri( BOX2_SYS_WORK * syswk )
{
	int	next;

	switch( syswk->subProcMode ){
	case SUB_PROC_MODE_MENU_BOX:			// �{�b�N�X�̃|�P�����̃��j���[����
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		next = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		break;

	case SUB_PROC_MODE_MENU_PARTY:		// �莝���̃|�P�����̃��j���[����
		PartyFrmSet( syswk );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_ITEM );
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
		next = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
		break;
	}

	BOX2OBJ_PokeCursorAdd( syswk );

	if( syswk->subRet == 0 ){
		BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
		BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 6 );
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
		BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
		return next;
	}

	if( ITEM_CheckMail( syswk->subRet ) == FALSE ){
		POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );

		BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, syswk->tray, ID_PARA_item, syswk->subRet );
		if( BOX2MAIN_PokeItemFormChange( syswk, ppp ) == TRUE ){
			BOX2MAIN_FormChangeRenew( syswk, syswk->get_pos );
			BOX2OBJ_PokeIconChange(
				syswk, syswk->tray, syswk->get_pos, syswk->app->pokeicon_id[syswk->get_pos] );
		}
		MYITEM_SubItem( syswk->dat->myitem, syswk->subRet, 1, HEAPID_BOX_APP_L );
	}

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

	BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );

	return BOX2SEQ_MAINSEQ_ITEMSET_MSG;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�o�b�O��ʕ��A�����y�����������z
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int RcvBag_ModeItem( BOX2_SYS_WORK * syswk )
{
	int	next;

	switch( syswk->subProcMode ){
	case SUB_PROC_MODE_MENU_BOX:			// �{�b�N�X�̃|�P�����̃��j���[����
		BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_TRAY );
		next = BOX2SEQ_MAINSEQ_ITEM_MAIN;
		break;

	case SUB_PROC_MODE_MENU_PARTY:		// �莝���̃|�P�����̃��j���[����
		PartyFrmSet( syswk );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_PARTY );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_PARTY, BOX2UI_ITEM_PARTY_MENU2 );
		next = BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN;
		break;
	}

	BOX2SEQ_ItemModeMenuSet( syswk, syswk->subRet );

	if( syswk->subRet == 0 ){
		BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
		BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_SET, BOX2BMPWIN_ID_MSG1 );
		// �{�b�N�X�̃|�P�����̃��j���[����
		if( syswk->subProcMode == SUB_PROC_MODE_MENU_BOX ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
		// �莝���̃|�P�����̃��j���[����
		}else{
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
		}
		return next;
	}

	if( ITEM_CheckMail( syswk->subRet ) == FALSE ){
		POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );

		BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, syswk->tray, ID_PARA_item, syswk->subRet );
		if( BOX2MAIN_PokeItemFormChange( syswk, ppp ) == TRUE ){
			BOX2MAIN_FormChangeRenew( syswk, syswk->get_pos );
			BOX2OBJ_PokeIconChange(
				syswk, syswk->tray, syswk->get_pos, syswk->app->pokeicon_id[syswk->get_pos] );
		}
		MYITEM_SubItem( syswk->dat->myitem, syswk->subRet, 1, HEAPID_BOX_APP_L );

		// �A�C�e���A�C�R���Z�b�g
		syswk->app->get_item = syswk->subRet;
		BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
		BOX2OBJ_ItemIconCursorAdd( syswk->app );
		BOX2OBJ_ItemIconCursorOn( syswk->app );
	}

	// �{�b�N�X�̃|�P�����̃��j���[����
	if( syswk->subProcMode == SUB_PROC_MODE_MENU_BOX ){
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
	// �莝���̃|�P�����̃��j���[����
	}else{
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
	}

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

	BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );

	return BOX2SEQ_MAINSEQ_ITEMSET_MSG;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�o�b�O��ʕ��A�����y�o�g���{�b�N�X�z
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int RcvBag_ModeBattleBox( BOX2_SYS_WORK * syswk )
{
	int	next;

	switch( syswk->subProcMode ){
	case SUB_PROC_MODE_MENU_BOX:			// �{�b�N�X�̃|�P�����̃��j���[����
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		next = BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;
		break;

	case SUB_PROC_MODE_MENU_PARTY:		// �莝���̃|�P�����̃��j���[����
		PartyFrmSet( syswk );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BATTLEBOX_PARTY, BOX2UI_BATTLEBOX_PARTY_ITEM );
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
		next = BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN;
		break;
	}

	BOX2OBJ_PokeCursorAdd( syswk );

	if( syswk->subRet == 0 ){
		BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
		BOX2BMP_MenuStrPrint( syswk, BattleBoxMenuStrTbl, 4 );
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
		BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
		return next;
	}

	if( ITEM_CheckMail( syswk->subRet ) == FALSE ){
		POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );

		BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, syswk->tray, ID_PARA_item, syswk->subRet );
		if( BOX2MAIN_PokeItemFormChange( syswk, ppp ) == TRUE ){
			BOX2MAIN_FormChangeRenew( syswk, syswk->get_pos );
			BOX2OBJ_PokeIconChange(
				syswk, syswk->tray, syswk->get_pos, syswk->app->pokeicon_id[syswk->get_pos] );
		}
		MYITEM_SubItem( syswk->dat->myitem, syswk->subRet, 1, HEAPID_BOX_APP_L );
	}

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

	BOX2OBJ_SetHandCursorOnOff( syswk, FALSE );

	return BOX2SEQ_MAINSEQ_ITEMSET_MSG;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�e�[�^�X��ʕ��A�����y�A��čs���z
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int RcvStatus_ModeTureteiku( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_MenuStrPrint( syswk, PartyInMenuStrTbl, 5 );
	BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
	BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
	BOX2OBJ_PokeCursorAdd( syswk );
	return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�e�[�^�X��ʕ��A�����y�a����z
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int RcvStatus_ModeAzukeru( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_MenuStrPrint( syswk, PartyOutMenuStrTbl, 5 );
	BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
	BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
	BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
	PartyFrmSet( syswk );
	BOX2OBJ_PokeCursorAdd( syswk );
	return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�e�[�^�X��ʕ��A�����y�{�b�N�X�����z
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int RcvStatus_ModeSeiri( BOX2_SYS_WORK * syswk )
{
	switch( syswk->subProcMode ){
	case SUB_PROC_MODE_MENU_BOX:			// �{�b�N�X�̃|�P�����̃��j���[����
		BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 6 );
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
		BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
		BOX2OBJ_PokeCursorAdd( syswk );
		return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;

	case SUB_PROC_MODE_MENU_PARTY:		// �莝���̃|�P�����̃��j���[����
		BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 6 );
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
		BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_STATUS );
		PartyFrmSet( syswk );
		BOX2OBJ_PokeCursorAdd( syswk );
		return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;

	case SUB_PROC_MODE_TB_BOX:				// �{�b�N�X�̃|�P�����̃^�b�`�o�[����
		// �莝���擾���[�h����J�n�����Ƃ�
		if( syswk->get_start_mode == 1 ){
			BOX2OBJ_PartyPokeIconFrmInSetRight( syswk );
		}
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->get_pos );
		BOX2MAIN_InitBoxMoveFrameScroll( syswk );
		while( BOX2MAIN_VFuncBoxMoveFrmIn(syswk) ){}
		BOX2OBJ_EndTrayCursorScroll( syswk );
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN;

	case SUB_PROC_MODE_TB_BOX_GET:		// �{�b�N�X�̃|�P�����̃^�b�`�o�[����i�͂�ł��鎞�j
		// �莝���擾���͑��̎莝���A�C�R�����E�t���[���C���O�̈ʒu�Ɉړ�
		if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2OBJ_PartyPokeIconFrmInSetRight( syswk );
			BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
		}else if( syswk->tray == syswk->get_tray ){
			BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
		}else{
			BOX2OBJ_PokeIconChange( syswk, syswk->get_tray, syswk->get_pos, syswk->app->pokeicon_id[BOX2OBJ_POKEICON_GET_POS] );
		}
		BOX2MAIN_HandGetPokeSet( syswk );
		BOX2OBJ_ChgPokeCursorPriority( syswk, BOX2OBJ_POKEICON_GET_POS );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->tb_status_pos );
		BOX2MAIN_InitBoxMoveFrameScroll( syswk );
		while( BOX2MAIN_VFuncBoxMoveFrmIn(syswk) ){}
		BOX2OBJ_EndTrayCursorScroll( syswk );
		BOX2BGWFRM_TemochiButtonOn( syswk->app );
		if( syswk->tb_status_pos >= BOX2UI_ARRANGE_PGT_TRAY2 && syswk->tb_status_pos <= BOX2UI_ARRANGE_PGT_TRAY5 ){
			BOX2OBJ_ChangeTrayName( syswk, syswk->tb_status_pos-BOX2UI_ARRANGE_PGT_TRAY2, TRUE );
		}
		BOX2OBJ_PokeCursorAdd2( syswk, BOX2OBJ_POKEICON_GET_POS );
		return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN;

	case SUB_PROC_MODE_TB_PARTY:			// �莝���̃|�P�����̃^�b�`�o�[����
		PartyFrmSetRight( syswk );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY_MOVE, syswk->get_pos );
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
		return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN;

	case SUB_PROC_MODE_TB_PARTY_GET:	// �莝���̃|�P�����̃^�b�`�o�[����i�͂�ł��鎞�j
		PartyFrmSetRight( syswk );
		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX && syswk->tray != syswk->get_tray ){
			BOX2OBJ_PokeIconChange( syswk, syswk->get_tray, syswk->get_pos, syswk->app->pokeicon_id[BOX2OBJ_POKEICON_GET_POS] );
		}else{
			BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
		}
		BOX2MAIN_HandGetPokeSet( syswk );
		BOX2OBJ_ChgPokeCursorPriority( syswk, BOX2OBJ_POKEICON_GET_POS );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY_MOVE, syswk->tb_status_pos );
		BOX2BGWFRM_BoxListButtonOn( syswk->app );
		BOX2OBJ_PokeCursorAdd2( syswk, BOX2OBJ_POKEICON_GET_POS );
		return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN;
	}

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�e�[�^�X��ʕ��A�����y�o�g���{�b�N�X�z
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int RcvStatus_ModeBattleBox( BOX2_SYS_WORK * syswk )
{
	switch( syswk->subProcMode ){
	case SUB_PROC_MODE_MENU_BOX:			// �{�b�N�X�̃|�P�����̃��j���[����
		BOX2BMP_MenuStrPrint( syswk, BattleBoxMenuStrTbl, 4 );
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
		BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BATTLEBOX_MAIN, BOX2UI_BATTLEBOX_MAIN_STATUS );
		BOX2OBJ_PokeCursorAdd( syswk );
		return BOX2SEQ_MAINSEQ_BATTLEBOX_MAIN;

	case SUB_PROC_MODE_MENU_PARTY:		// �莝���̃|�P�����̃��j���[����
		BOX2BMP_MenuStrPrint( syswk, BattleBoxMenuStrTbl, 4 );
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
		BOX2BMP_PokeSelectMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG1 );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BATTLEBOX_PARTY, BOX2UI_BATTLEBOX_PARTY_STATUS );
		PartyFrmSet( syswk );
		BOX2OBJ_PokeCursorAdd( syswk );
		return BOX2SEQ_MAINSEQ_BATTLEBOX_PARTY_MAIN;

	case SUB_PROC_MODE_TB_PARTY:			// �莝���̃|�P�����̃^�b�`�o�[����
		PartyFrmSetRight( syswk );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY_MOVE, syswk->get_pos );
		CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_BOXLIST );
		return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN;

	case SUB_PROC_MODE_TB_PARTY_GET:	// �莝���̃|�P�����̃^�b�`�o�[����i�͂�ł��鎞�j
		PartyFrmSetRight( syswk );
		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX && syswk->tray != syswk->get_tray ){
			BOX2OBJ_PokeIconChange( syswk, syswk->get_tray, syswk->get_pos, syswk->app->pokeicon_id[BOX2OBJ_POKEICON_GET_POS] );
		}else{
			BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
		}
		BOX2MAIN_HandGetPokeSet( syswk );
		BOX2OBJ_ChgPokeCursorPriority( syswk, BOX2OBJ_POKEICON_GET_POS );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY_MOVE, syswk->tb_status_pos );
		CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_BOXLIST );
		BOX2OBJ_PokeCursorAdd2( syswk, BOX2OBJ_POKEICON_GET_POS );
		return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN;
	}

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C�؂�ւ��E��
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		btnFlg	�{�^���t���O
 * @param		next		�؂�ւ���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayLeftScrollSet( BOX2_SYS_WORK * syswk, BOOL btnFlg, int next )
{
	BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_L );

	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
		BOX2MAIN_ResetTouchBar( syswk );
		if( btnFlg == TRUE ){
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
		}
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncTrayScrollLeft, next );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C�؂�ւ��E�E
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		btnFlg	�{�^���t���O
 * @param		next		�؂�ւ���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayRightScrollSet( BOX2_SYS_WORK * syswk, BOOL btnFlg, int next )
{
	BOX2MAIN_InitTrayScroll( syswk, BOX2MAIN_TRAY_SCROLL_R );

	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
		BOX2MAIN_ResetTouchBar( syswk );
		if( btnFlg == TRUE ){
			BOX2BGWFRM_TemochiButtonOn( syswk->app );
		}
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncTrayScrollRight, next );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C�̃|�P�������擾
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayPokeGet( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_start_mode = 0;
	syswk->mv_cnv_mode = 0;

	syswk->get_pos  = pos;
	syswk->get_tray = syswk->tray;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
	BOX2OBJ_PokeCursorAdd2( syswk, BOX2OBJ_POKEICON_GET_POS );

	PokeMoveWorkAlloc( syswk );

	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	BOX2BGWFRM_TemochiButtonOn( syswk->app );

	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_TRAY;
	syswk->next_seq  = BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN;	// �͂񂾂܂܃{�b�N�X�ړ��Ɉڍs����ꍇ�͂����ɂ���
	return VFuncSet( syswk, BOX2MAIN_VFuncGetPokeMoveBoxParty, BOX2SEQ_MAINSEQ_ARRANGE_TRAYPOKE_CHG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C�̃|�P�������擾�y�o�g���{�b�N�X�z
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int GetBattleBoxTrayPoke( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos  = pos;
	syswk->get_tray = syswk->tray;
	syswk->get_start_mode = 0;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
	BOX2OBJ_PokeCursorAdd2( syswk, BOX2OBJ_POKEICON_GET_POS );

	PokeMoveWorkAlloc( syswk );

	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );

	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_TRAY;
	syswk->next_seq  = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN;	// �͂񂾂܂܃{�b�N�X�ړ��Ɉڍs����ꍇ�͂����ɂ���
	return VFuncSet( syswk, BOX2MAIN_VFuncGetPokeMoveBattleBoxMain, BOX2SEQ_MAINSEQ_BATTLEBOX_TRAYPOKE_CHG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C�̃|�P�������擾�y�A��čs���z
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayPokeGetPartyIn( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;
	syswk->get_tray = syswk->tray;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
	BOX2OBJ_PokeCursorAdd2( syswk, BOX2OBJ_POKEICON_GET_POS );

	BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
	BOX2OBJ_PartyPokeIconFrmInSetRight( syswk );

	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, TRUE );
	BOX2OBJ_PokeIconBlendSet( syswk->app, BOX2OBJ_POKEICON_GET_POS, FALSE );

	PokeMoveWorkAlloc( syswk );
	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );

	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;
	return VFuncSet( syswk, BOX2MAIN_GetTrayPokeMoveDrop, BOX2SEQ_MAINSEQ_PARTYIN_POKECHG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C�̃|�P�������擾�i�L�[����j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 * @param		ret			�擾��̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayPokeGetKey( BOX2_SYS_WORK * syswk, u32 pos, int ret )
{
	syswk->get_pos = pos;
	syswk->app->msg_put = 1;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
	BOX2OBJ_PokeCursorAdd( syswk );

	BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );

	return VFuncSet( syswk, BOX2MAIN_VFuncMenuMove, SetCursorMoveRet(syswk,ret) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�莝���|�P�������擾
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyPokeGet( BOX2_SYS_WORK * syswk, u32 pos )
{
	POKEMON_PARAM * pp;

	syswk->get_start_mode = 1;
	syswk->mv_cnv_mode = 0;

	syswk->get_pos  = pos;
	syswk->get_tray = syswk->tray;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
	BOX2OBJ_PokeCursorAdd2( syswk, BOX2OBJ_POKEICON_GET_POS );

	pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, pos-BOX2OBJ_POKEICON_TRAY_MAX );

	PokeMoveWorkAlloc( syswk );
	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	BOX2BGWFRM_BoxListButtonOn( syswk->app );
	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_PARTY;
	syswk->next_seq  = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN;	// �͂�Ŏ莝���O�ɔz�u����ꍇ�͂����Ɉڍs����
	return VFuncSet( syswk, BOX2MAIN_VFuncGetPokeMoveParty, BOX2SEQ_MAINSEQ_ARRANGE_PARTYPOKE_CHG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�莝���|�P�������擾�y�o�g���{�b�N�X�z
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int GetBattleBoxPartyPoke( BOX2_SYS_WORK * syswk, u32 pos )
{
	POKEMON_PARAM * pp;

	syswk->get_pos  = pos;
	syswk->get_tray = syswk->tray;
	syswk->get_start_mode = 1;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
	BOX2OBJ_PokeCursorAdd2( syswk, BOX2OBJ_POKEICON_GET_POS );

	pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, pos-BOX2OBJ_POKEICON_TRAY_MAX );

	PokeMoveWorkAlloc( syswk );
	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_PARTY;
	syswk->next_seq  = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN;	// �͂�Ŏ莝���O�ɔz�u����ꍇ�͂����Ɉڍs����
	return VFuncSet( syswk, BOX2MAIN_VFuncGetPokeMoveParty, BOX2SEQ_MAINSEQ_BATTLEBOX_PARTYPOKE_CHG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�莝���|�P�������擾�y�a����z
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyPokeGetDrop( BOX2_SYS_WORK * syswk, u32 pos )
{
	POKEMON_PARAM * pp;
	u32	i;

	syswk->get_pos = pos;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
	BOX2OBJ_PokeCursorAdd2( syswk, BOX2OBJ_POKEICON_GET_POS );

	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, TRUE );
	BOX2OBJ_PokeIconBlendSet( syswk->app, BOX2OBJ_POKEICON_GET_POS, FALSE );

	pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, pos - BOX2OBJ_POKEICON_TRAY_MAX );

	PokeMoveWorkAlloc( syswk );
	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );
	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_PARTY;
	return VFuncSet( syswk, BOX2MAIN_GetPartyPokeMoveDrop, BOX2SEQ_MAINSEQ_PARTYOUT_POKECHG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�莝���|�P�������擾�i�L�[����j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 * @param		ret			�擾��̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyPokeGetKey( BOX2_SYS_WORK * syswk, u32 pos, int ret )
{
	syswk->get_pos = pos;
	syswk->app->msg_put = 1;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
	BOX2OBJ_PokeCursorAdd( syswk );

	BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );

	return VFuncSet( syswk, BOX2MAIN_VFuncMenuMove, SetCursorMoveRet(syswk,ret) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C/�莝���|�P�������擾
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 * @param		next		�擾��̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxPartyPokeGet( BOX2_SYS_WORK * syswk, u32 pos, int next )
{
	syswk->get_tray = syswk->tray;	// �擾�����{�b�N�X
	syswk->get_pos  = pos;
	syswk->app->old_cur_pos = pos;

	BOX2OBJ_GetPokeIcon( syswk->app, syswk->get_pos );
	BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_GET_POS, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_PokeCursorAdd2( syswk, BOX2OBJ_POKEICON_GET_POS );

	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_PASSIVE );

	PokeMoveWorkAlloc( syswk );
	syswk->next_seq  = next;
	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;
	return VFuncSet( syswk, BOX2MAIN_VFuncGetPokeMoveBoxParty, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_DATA_CHANGE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�������u���ށv�i�{�b�N�X<->�莝������ւ��p�j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyPokeMoveGetKey( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;
	syswk->poke_get_key = 1;

	BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_GET_POS, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_PokeCursorAdd2( syswk, BOX2OBJ_POKEICON_GET_POS );
	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_ON );

	if( syswk->dat->callMode == BOX_MODE_BATTLE ){
		BOX2BGWFRM_TemochiButtonOff( syswk->app );
		BOX2BGWFRM_BoxListButtonOff( syswk->app );
		CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_PTGT_BOXLIST );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncPokeMoveGetKey, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_POKEGET_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�������u���ށv�i�{�b�N�X<->�g���C�ړ��p�j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PokeMoveGetKey( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;
	syswk->poke_get_key = 1;

	BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_GET_POS, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_PokeCursorAdd2( syswk, BOX2OBJ_POKEICON_GET_POS );
	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_ON );

	return VFuncSet( syswk, BOX2MAIN_VFuncPokeMoveGetKey, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u���ށv�Ŏ擾�����|�P������z�u
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�z�u�ʒu
 * @param		next		�z�u��̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PokeMovePutKey( BOX2_SYS_WORK * syswk, u32 pos, int next )
{
	// �g���C
	if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
	// �莝��
	}else if( pos < BOX2OBJ_POKEICON_PUT_MAX ){
	// �ʃg���C��
	}else if( pos != BOX2MAIN_GETPOS_NONE ){
		u32	tray = syswk->box_mv_pos + pos - BOX2OBJ_POKEICON_PUT_MAX;
		if( tray >= syswk->trayMax ){
			tray -= syswk->trayMax;
		}
		// ���ݎQ�Ƃ��Ă���g���C�Ɠ���
		if( tray == syswk->tray ){
			PMSND_PlaySE( SE_BOX2_WARNING );
			return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN;
		}
		// �I�������g���C�������ς�
		if( BOXDAT_GetEmptySpaceTray( syswk->dat->sv_box, tray ) == 0 ){
			PMSND_PlaySE( SE_BOX2_WARNING );
			return BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN;
		}
	}

	syswk->app->poke_put_key = pos;
	syswk->next_seq = next;
	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;
	PokeMoveWorkAlloc( syswk );
	return VFuncSet( syswk, BOX2MAIN_VFuncPokeMovePutKey, BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_DATA_CHANGE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����A�C�R�����샏�[�N�擾
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeMoveWorkAlloc( BOX2_SYS_WORK * syswk )
{
	syswk->app->vfunk.work = GFL_HEAP_AllocClearMemory( HEAPID_BOX_APP, sizeof(BOX2MAIN_POKEMOVE_WORK) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����A�C�R�����샏�[�N���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeMoveWorkFree( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C�̃A�C�e�����擾�i�^�b�`�j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayItemGet( BOX2_SYS_WORK * syswk, u32 pos )
{
	u16	tmp;

	tmp = syswk->app->get_item;

	syswk->get_pos  = pos;
	syswk->get_tray = syswk->tray;
	syswk->app->msg_put = 1;

	syswk->app->get_item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, syswk->tray, ID_PARA_item, NULL );

	BOX2BGWFRM_PokeMenuOff( syswk->app->wfrm );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );

	if( syswk->app->get_item != ITEM_DUMMY_DATA ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_TRAY );
		BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
		BOX2OBJ_ItemIconCursorAdd( syswk->app );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF );
	}else{
		if( BOX2OBJ_VanishCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
			BOX2OBJ_PokeCursorVanish( syswk, FALSE );
		}
	}

	{
		s16	x, y;

		BOX2OBJ_PokeIconDefaultPosGet( syswk->get_pos, &x, &y, BOX2MAIN_POKEMOVE_MODE_TRAY );
		syswk->app->tpx = x + 8;
		syswk->app->tpy = y + 8;
	}

	ItemMoveWorkAlloc( syswk );

	return VFuncSet( syswk, BOX2MAIN_VFuncItemGetTouch, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ITEMARRANGE_ICONMOVE_RET) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C�̃A�C�e�����擾�i�L�[����j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayItemGetKey( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos  = pos;
	syswk->get_tray = syswk->tray;

	syswk->app->msg_put = 1;

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

	syswk->app->get_item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, syswk->tray, ID_PARA_item, NULL );

	BOX2SEQ_ItemModeMenuSet( syswk, syswk->app->get_item );

	BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );

	if( syswk->app->get_item != ITEM_DUMMY_DATA ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_TRAY );
		BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
		BOX2OBJ_ItemIconCursorAdd( syswk->app );

		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_ITEM_MAIN_MENU1, CURSORMOVE_PosGet(syswk->app->cmwk) );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_MAIN_MENU1 );
	}else{
		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_ITEM_MAIN_MENU2, CURSORMOVE_PosGet(syswk->app->cmwk) );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_MAIN_MENU2 );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeGetKey, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ITEM_MAIN) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C/�莝���A�C�e�����擾
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxPartyItemGet( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;
	if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		syswk->get_tray = syswk->tray;
	}else{
		syswk->get_tray = BOX2MAIN_GETPOS_NONE;
	}

	syswk->app->get_item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, syswk->tray, ID_PARA_item, NULL );

	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
		return BOX2SEQ_MAINSEQ_ITEMGET_MAIN;
	}

	PMSND_PlaySE( SE_BOX2_POKE_CATCH );

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
	BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_ALL );
	BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
	BOX2OBJ_ItemIconCursorAdd( syswk->app );
	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );

	{
		s16	x, y;

		BOX2OBJ_PokeIconDefaultPosGet( syswk->get_pos, &x, &y, BOX2MAIN_POKEMOVE_MODE_ALL );
		syswk->app->tpx = x + 8;
		syswk->app->tpy = y + 8;
	}

	ItemMoveWorkAlloc( syswk );

	return VFuncSet( syswk, BOX2MAIN_VFuncItemMoveTouch, BOX2SEQ_MAINSEQ_ITEM_BOXPARTY_ICONMOVE_POKEADD );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		��J�[�\���Ńg���C/�莝���A�C�e�����擾
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxPartyItemGetKey( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;
	syswk->app->get_item_init_pos = pos;
	if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		syswk->get_tray = syswk->tray;
	}else{
		syswk->get_tray = BOX2MAIN_GETPOS_NONE;
	}

	syswk->app->get_item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, syswk->tray, ID_PARA_item, NULL );

	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		return BOX2SEQ_MAINSEQ_ITEMGET_MAIN;
	}

	syswk->poke_get_key = 1;

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
	BOX2OBJ_ItemIconPokePutHand( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_ALL );
	BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );

	BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF, BOX2OBJ_TB_ICON_OFF );

	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeBoxPartyIconGetKey, BOX2SEQ_MAINSEQ_ITEMGET_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		��J�[�\���ŃA�C�e����z�u�E�L�����Z������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxPartyItemPutKeyCancel( BOX2_SYS_WORK * syswk )
{
	u32	pos = CURSORMOVE_PosGet( syswk->app->cmwk );

	if( pos < BOX2OBJ_POKEICON_PUT_MAX ){
		BOX2MAIN_PokeInfoPut( syswk, pos );
	}else{
		BOX2MAIN_PokeInfoOff( syswk );
	}

	ItemMoveWorkAlloc( syswk );
	return VFuncSet(
			syswk,
			BOX2MAIN_VFuncItemIconPutKeyCancel,
			BOX2SEQ_MAINSEQ_ITEM_BOXPARTY_ICONMOVE_END );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		��J�[�\���ŃA�C�e����z�u
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�z�u�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxPartyItemPutKey( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->app->poke_put_key = pos;
	syswk->poke_get_key = 0;

	{	// �A�C�e���A�C�R���̈ʒu��␳����
		s16	x, y;
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, &x, &y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_ITEMICON, x, y+8, CLSYS_DEFREND_MAIN );
	}

	BOX2OBJ_PokeCursorVanish( syswk, FALSE );

	if( syswk->app->poke_put_key == BOX2MAIN_GETPOS_NONE ){
		return BoxPartyItemPutKeyCancel( syswk );
	}else if( BOX2MAIN_PokeItemMoveCheck( syswk, syswk->get_pos, pos ) == FALSE ){
		return BoxPartyItemPutKeyCancel( syswk );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncItemIconPutKey, BOX2SEQ_MAINSEQ_ITEM_KEYGET_CHANGE_CHECK );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�莝���|�P�����̃A�C�e�����擾�i�^�b�`�j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyItemGet( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos  = pos;
	syswk->get_tray = BOX2MAIN_GETPOS_NONE;

	syswk->app->get_item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, syswk->tray, ID_PARA_item, NULL );

	BOX2BGWFRM_PokeMenuOff( syswk->app->wfrm );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_ResetTouchBar( syswk );

	if( syswk->app->get_item != ITEM_DUMMY_DATA ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_PARTY );
		BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
		BOX2OBJ_ItemIconCursorAdd( syswk->app );
		BOX2OBJ_SetTouchBarButton( syswk, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_PASSIVE, BOX2OBJ_TB_ICON_OFF );
	}else{
		if( BOX2OBJ_VanishCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
			BOX2OBJ_PokeCursorVanish( syswk, FALSE );
		}
	}

	{
		s16	x, y;

		BOX2OBJ_PokeIconDefaultPosGet( syswk->get_pos, &x, &y, BOX2MAIN_POKEMOVE_MODE_PARTY );
		syswk->app->tpx = x + 8;
		syswk->app->tpy = y + 8;
	}

	ItemMoveWorkAlloc( syswk );

	return VFuncSet( syswk, BOX2MAIN_VFuncItemPartyGetTouch, BOX2SEQ_MAINSEQ_ITEM_PARTY_ICONMOVE_RET );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�莝���|�P�����̃A�C�e�����擾�i�L�[����E�����������j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyItemGetKey( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos  = pos;
	syswk->get_tray = syswk->tray;
	syswk->app->msg_put = 1;

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

	syswk->app->get_item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, syswk->tray, ID_PARA_item, NULL );

	BOX2SEQ_ItemModeMenuSet( syswk, syswk->app->get_item );

	BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );

	if( syswk->app->get_item != ITEM_DUMMY_DATA ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_PARTY );
		BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
		BOX2OBJ_ItemIconCursorAdd( syswk->app );
		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_ITEM_PARTY_MENU1, CURSORMOVE_PosGet(syswk->app->cmwk) );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_PARTY_MENU1 );
	}else{
		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_ITEM_PARTY_MENU2, CURSORMOVE_PosGet(syswk->app->cmwk) );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_PARTY_MENU2 );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeGetKey, SetCursorMoveRet(syswk,BOX2SEQ_MAINSEQ_ITEM_PARTY_MAIN) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���A�C�R�����샏�[�N�擾
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemMoveWorkAlloc( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_ITEMMOVE_WORK * work;

	work = GFL_HEAP_AllocMemory( HEAPID_BOX_APP, sizeof(BOX2MAIN_ITEMMOVE_WORK) );

	syswk->app->vfunk.work = work;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���A�C�R�����샏�[�N���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemMoveWorkFree( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�莝���t���[���Z�b�g�i���j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyFrmSet( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PartyPokeFramePut( syswk->app->wfrm );
	BOX2OBJ_PartyPokeIconFrmSet( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�莝���t���[���Z�b�g�i�E�j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyFrmSetRight( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PartyPokeFramePutRight( syswk->app->wfrm );
	BOX2OBJ_PartyPokeIconFrmSetRight( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�莝���t���[���Z�b�g�y�a����z
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyFrmSet_PartyOut( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_PartyPokeIconFrmSet( syswk );
	BOX2BGWFRM_PartyPokeFramePut( syswk->app->wfrm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�b�N�X�؂�ւ��i�g���C�A�C�R���I���j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		mode		�X�N���[�����[�h
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxMoveTrayScrollSet( BOX2_SYS_WORK * syswk, u32 mode )
{
	void * func;
	u32	dir;

	dir = BOX2MAIN_GetTrayScrollDir( syswk, syswk->tray, syswk->app->chg_tray_pos );

	syswk->tray = syswk->app->chg_tray_pos;

	BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
	BOX2MAIN_WallPaperSet( syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), dir );

	if( dir == BOX2MAIN_TRAY_SCROLL_R ){
		func = BOX2MAIN_VFuncTrayScrollRight;
	}else{
		func = BOX2MAIN_VFuncTrayScrollLeft;
	}

	switch( mode ){
	case 0:		//�u���ށv���̃g���C�I��
		syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEGET_MAIN;
		break;

	case 1:		// �{�b�N�X�W�����v
		syswk->next_seq = BOX2SEQ_MAINSEQ_BOXTHEMA_JUMP_RET;
		break;
	}

	PMSND_PlaySE( SE_BOX2_CHANGE_TRAY );

	return VFuncSet( syswk, func, syswk->next_seq );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C�A�C�R���X�N���[�����[�N�쐬
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CreateTrayIconScrollWork( BOX2_SYS_WORK * syswk )
{
	SEQWK_TRAYICON_SCROLL * wk = GFL_HEAP_AllocMemoryLo( HEAPID_BOX_APP, sizeof(SEQWK_TRAYICON_SCROLL) );
	wk->cnt = 0;
	wk->mv  = 0;
	syswk->app->seqwk = wk;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�b�N�X�ړ��t���[���C���Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		next		�t���[���ړ���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxMoveFrmInSet( BOX2_SYS_WORK * syswk, int next )
{
	BOX2MAIN_InitBoxMoveFrameScroll( syswk );
	return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveFrmIn, next );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�b�N�X�ړ��t���[���A�E�g�Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		next		�t���[���ړ���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxMoveFrmOutSet( BOX2_SYS_WORK * syswk, int next )
{
	BOX2BGWFRM_BoxMoveFrmOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveFrmOut, next );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�������j���[�A�E�g�Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		next		�t���[���ړ���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PokeMenuOutSet( BOX2_SYS_WORK * syswk, int next )
{
	syswk->get_pos = BOX2MAIN_GETPOS_NONE;
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, next );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���j���[�Z�b�g�y�����������z
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		item		�A�C�e��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2SEQ_ItemModeMenuSet( BOX2_SYS_WORK * syswk, s16 item )
{
	if( item != ITEM_DUMMY_DATA ){
		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			CURSORMOVE_MoveTableBitOn( syswk->app->cmwk, BOX2UI_ITEM_MAIN_MENU1 );
		}else{
			CURSORMOVE_MoveTableBitOn( syswk->app->cmwk, BOX2UI_ITEM_PARTY_MENU1 );
		}
		BOX2BMP_MenuStrPrint( syswk,ItemMenuStrTbl, 3 );
	}else{
		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ITEM_MAIN_MENU1 );
		}else{
			CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ITEM_PARTY_MENU1 );
		}
		BOX2BMP_MenuStrPrint( syswk,ItemNoneMenuStrTbl, 2 );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�莝���Ɉړ��ł��邩
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"0 = �ړ���"
 * @retval	"1 = �킦��|�P���������Ȃ��Ȃ�"
 * @retval	"2 = ���[���������Ă���"
 */
//--------------------------------------------------------------------------------------------
static int PartyOutCheck( BOX2_SYS_WORK * syswk )
{
	POKEMON_PARAM * pp;
	u32	pos;
	u16	item;

	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;

	// �莝���P
	if( BOX2MAIN_BattlePokeCheck( syswk, pos ) == FALSE ){
		return 1;
	}
	// ���[��
	pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, pos );
	item = PP_Get( pp, ID_PARA_item, NULL );
	if( ITEM_CheckMail( item ) == TRUE ){
		return 2;
	}

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�}�[�L���O�ύX
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�ύX�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MarkingSwitch( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->app->pokeMark ^= (1<<pos);
	BOX2MAIN_MainDispMarkingChange( syswk, syswk->app->pokeMark );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ǎ��ύX
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�ǎ��I���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static int SetWallPaperChange( BOX2_SYS_WORK * syswk, u32 pos )
{
	if( syswk->app->wp_menu <= 4 ||
			( syswk->app->wp_menu == 5 && BOXDAT_GetExWallPaperFlag(syswk->dat->sv_box,BOX_EX_WALLPAPER_SET_FLAG_2) == FALSE ) ){
		syswk->app->wallpaper_pos = ( syswk->app->wp_menu - 1 ) * 4 + pos - 1;
	}else{
		syswk->app->wallpaper_pos = ( syswk->app->wp_menu - 1 ) * 4 + pos;
	}
	return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU1+pos, ChangeSequence(syswk,BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_CHANGE) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�b�N�X�I���m�F���̃p�b�V�u�Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxEndPassiveSet( BOX2_SYS_WORK * syswk )
{
	switch( syswk->dat->callMode ){
	case BOX_MODE_AZUKERU:		// ��������
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, TRUE );
		break;

	case BOX_MODE_TURETEIKU:	// ��Ă���
	case BOX_MODE_ITEM:				// �ǂ���������
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		break;

	case BOX_MODE_SEIRI:			// ������
	case BOX_MODE_BATTLE:			// �o�g���{�b�N�X
	case BOX_MODE_SLEEP:			// �˂�����
		if( BOX2BGWFRM_CheckPartyPokeFrameLeft( syswk->app->wfrm ) == TRUE ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, TRUE );
		}else{
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		}
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���N�g�{�^���ŕ֗����[�h�ֈڍs
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		next		���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int ChgSeqSelectButton( BOX2_SYS_WORK * syswk, int next )
{
	PMSND_PlaySE( SE_BOX2_DECIDE );
	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
		syswk->mv_cnv_mode = 2;
	}else{
		syswk->mv_cnv_mode = 1;
	}
	syswk->mv_cnv_end = 0;
	BOX2MAIN_ChgCursorButton( syswk );
	return ChangeSequence( syswk, next );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�ŕ֗����[�h�ֈڍs
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		next		���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int ChgSeqSelectTouch( BOX2_SYS_WORK * syswk, int next )
{
	PMSND_PlaySE( SE_BOX2_DECIDE );
	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
		syswk->mv_cnv_mode = 2;
	}else{
		syswk->mv_cnv_mode = 1;
	}
	syswk->mv_cnv_end = 0;
	return ChangeSequence( syswk, next );
}
