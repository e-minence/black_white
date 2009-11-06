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
#include "item/item.h"
#include "poke_tool/monsno_def.h"

#include "msg/msg_boxmenu.h"

#include "box2_main.h"
#include "box2_bmp.h"
#include "box2_obj.h"
#include "box2_seq.h"
#include "box2_ui.h"
#include "box2_bgwfrm.h"

/*
#if 0
//��[GS_CONVERT_TAG]

#include "gflib/touchpanel.h"
#include "system/wipe.h"
#include "system/snd_tool.h"
#include "msgdata/msg_boxmenu.h"

#include "box2_main.h"
#include "box2_seq.h"
#include "box2_bmp.h"
#include "box2_obj.h"
#include "box2_tp.h"
#include "box2_key.h"
#include "box2_snd_def.h"





//#define	BTS_5436_TEST
#ifdef	BTS_5436_TEST
static u32 test_5436_flg = 0;
#endif	// BTS_5436_TEST
*/


//============================================================================================
//	�萔��`
//============================================================================================

// �T�u�v���Z�X�^�C�v
enum {
	SUB_PROC_TYPE_POKESTATUS = 0,	// �X�e�[�^�X���
	SUB_PROC_TYPE_BAG,				// �o�b�O���
	SUB_PROC_TYPE_NAMEIN,			// ��������
};

// �T�u�v���Z�X�ݒ�֐�
typedef struct {
	pBOX2_FUNC	call;	// �Ăяo��
	pBOX2_FUNC	exit;	// �I��
	u32	rcvSeq;			// ���A�V�[�P���X
}SUBGFL_PROC_DATA;
//��[GS_CONVERT_TAG]

// �͂��E����������
typedef struct {
	pBOX2_FUNC	yes;
	pBOX2_FUNC	no;
}YESNO_DATA;

// �͂��E������ID
enum {
	YESNO_ID_ITEMGET = 0,		// ����a����
	YESNO_ID_POKEFREE,			// ������
	YESNO_ID_ITEM_RET_BAG,		// �A�C�e�����܂�
	YESNO_ID_BOX_END,			// �{�b�N�X�I���i�{�^���j
	YESNO_ID_BOX_END_CANCEL,	// �{�b�N�X�I���i�L�����Z���j
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

static int MainSeq_ArrangeMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeTrayPokeChgEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeMenuRcv( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeStatusRcv( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeStrInRcv( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBagRcv( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeItemSetMsg( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeMarkingInit( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeMarkingFrmIn( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeMarkingStart( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeMarking( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeMarkingRcv( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeFreeStart( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeFreeAnm( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeFreeEnter1( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeFreeEnter2( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeFreeEnter3( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeFreeError1( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeFreeError2( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePokeFreeError3( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyPokeFreeEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyPokeFrmIn( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyStart( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyPokeFrmOut( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxPartyButtonIn( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyPokeChgEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxPartyChgStart( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxPartyChgMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxPartyPokeChgEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxPartyPokeChgEndKey( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxPartyChgError( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyFrmMoveREnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyFrmMoveLEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxMoveFrmIn( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeExitButtonFrmIn( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxMoveFrmOutSet( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxMoveFrmOut( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxRetButtonIn( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxThemaChgMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxThemaButtonOut( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxThemaTrayIn( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxThemaExitIn( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxThemaMenuIn( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxThemaMsgSet( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxThemaExitOut( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxThemaMenuOut( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxThemaTrayOut( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxThemaButtonIn( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeWallPaperFrmIn( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeWallPaperMenuIn( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeWallPaperChgStart( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeWallPaperChgMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeWallPaperMenuOut( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeWallPaperFrmOut( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeWallPaperChgEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeWallPaperChange( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeWallPaperWhiteOut( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeWallPaperWhiteIn( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeMainCurMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxPartyChgMainCurMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyMainCurMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxArrangePokeMenuRcv( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxArrangePartyMenuRcv( BOX2_SYS_WORK * syswk );

static int MainSeq_PartyInMain( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInMainRcv( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInPartyFrmIn( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInPokeMoveSet( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInPartyFrmOut( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInRetFrmIn( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInPokeChgEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInMainCurMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInMenuCloseEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInPokeMenuRcv( BOX2_SYS_WORK * syswk );

static int MainSeq_PartyOutMain( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutStartCheck( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutStartError( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutStartRetFrmIn( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutMenuFrmIn( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutBoxSelectStart( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutBoxSelectMain( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutBoxMenuFrmOut( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutEndRetFrmOut( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutBoxMoveFrmOut( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutEndRetFrmIn( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutEnterButton( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutCheck( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutEnterPartyFrmRight( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutEnterTrayScroll( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutEnterPokeMove( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutEnterPartyFrmLeft( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutEnterRetIn( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutComp( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutPokeChgEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutMainCurMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutMenuCloseEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutBoxSelCurMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutPokeMenuRcv( BOX2_SYS_WORK * syswk );

static int MainSeq_MarkingCurMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaChgCurMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_WallPaperChgCurMoveRet( BOX2_SYS_WORK * syswk );

static int MainSeq_KeyGetPokeMoveMain( BOX2_SYS_WORK * syswk );
static int MainSeq_KeyGetPokeMoveMainCurMoveRet( BOX2_SYS_WORK * syswk );

static int MainSeq_ItemArrangeMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeMainCurMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeRetBagSet( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeRetBagCheck( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeRetBagEnter( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeRetBagCancel( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeRetBagEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeGetBagCheck( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeItemSetMsg( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeIconMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeIconMovePokeAdd( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeIconMoveChange( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeIconMoveChangeEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeBoxPartyChgMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeBoxPartyChgMainCurMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeBoxPartyIconMoveSubFrmOut( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeBoxPartyIconMovePokeAdd( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeBoxPartyIconMoveEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeBoxPartyChgError( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeKeyGetMoveMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeKeyGetMoveMainCurMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeKeyGetChangeCkack( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangePartyMain( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangePartyMainCurMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangePartyIconMoveRet( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangePartyIconMovePokeAdd( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangePartyIconMovePokeAddEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangePartyIconMoveChange( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangePartyIconMoveChangeEnd( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangePartyIconMoveErr( BOX2_SYS_WORK * syswk );

static int MainSeq_BoxEndInit( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxEndCancelInit( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeMoveButton( BOX2_SYS_WORK * syswk );
static int MainSeq_StatusButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemButton( BOX2_SYS_WORK * syswk );
static int MainSeq_MarkingButton( BOX2_SYS_WORK * syswk );
static int MainSeq_MarkingEndButton( BOX2_SYS_WORK * syswk );
static int MainSeq_FreeButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeCloseButton( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyChgButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangePartyCloseButton( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyEndButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeBoxMoveButton( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxMoveEndButton( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaTrayChgButton( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaWallPaperButton( BOX2_SYS_WORK * syswk );
static int MainSeq_BoxThemaNameButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeWallPaperChgButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeWallPaperChgCancelButton( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInButton( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyInCloseButton( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutButton( BOX2_SYS_WORK * syswk );
static int MainSeq_PartyOutCloseButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangePartyButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeMoveButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeBagButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeCloseButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangeBoxMoveButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemChgButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ItemArrangePartyCloseButton( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeItemSetEggError( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeItemGetMailError( BOX2_SYS_WORK * syswk );
static int MainSeq_ArrangeMoveMenuButton( BOX2_SYS_WORK * syswk );

static int FadeInSet( BOX2_SYS_WORK * syswk, int next );
static int FadeOutSet( BOX2_SYS_WORK * syswk, int next );
static int WaitSet( BOX2_SYS_WORK * syswk, int next, int wait );
static int YesNoSet( BOX2_SYS_WORK * syswk, u32 id );
static int VFuncSet( BOX2_SYS_WORK * syswk, void * func, int next );
static int VFuncReqSet( BOX2_SYS_WORK * syswk, int next );
static int SubProcSet( BOX2_SYS_WORK * syswk, u8 type );

static int TrayLeftScrollSet( BOX2_SYS_WORK * syswk, int next );
static int TrayRightScrollSet( BOX2_SYS_WORK * syswk, int next );
static int TrayPokeGet( BOX2_SYS_WORK * syswk, u32 pos );
static int PartyPokeGet( BOX2_SYS_WORK * syswk, u32 pos );
static void PokeMoveWorkAlloc( BOX2_SYS_WORK * syswk );
static void PokeMoveWorkFree( BOX2_SYS_WORK * syswk );
static int PokeItemCheck( BOX2_SYS_WORK * syswk );
static int MarkingScrollOutSet( BOX2_SYS_WORK * syswk );
static int MarkingStartSet( BOX2_SYS_WORK * syswk );
static int PokeFreeStartSet( BOX2_SYS_WORK * syswk );
static int BoxPartyPokeGet( BOX2_SYS_WORK * syswk, u32 pos );
static int BoxPartyButtonOutSet( BOX2_SYS_WORK * syswk );
static int PartyFrmOutSet( BOX2_SYS_WORK * syswk );
static void PartyFrmSet( BOX2_SYS_WORK * syswk );
static void PartyFrmSetRight( BOX2_SYS_WORK * syswk );
static void PartyFrmSet_PartyOut( BOX2_SYS_WORK * syswk );
static int PartyFrmMoveRight( BOX2_SYS_WORK * syswk );
static int PartyFrmMoveLeft( BOX2_SYS_WORK * syswk );
static int BoxMoveButtonOutSet( BOX2_SYS_WORK * syswk );
static int BoxMoveEndStart( BOX2_SYS_WORK * syswk );
static int BoxMoveMenuFrmIn( BOX2_SYS_WORK * syswk, u32 ret );
static int BoxMoveMenuFrmOut( BOX2_SYS_WORK * syswk, u32 pos );
static int BoxMoveButtonFrmIn( BOX2_SYS_WORK * syswk, u32 ret );
static int BoxMoveButtonFrmOut( BOX2_SYS_WORK * syswk, int next );
static int YStatusButtonFrmOut( BOX2_SYS_WORK * syswk, int next );
static int BoxMoveTrayScrollSet( BOX2_SYS_WORK * syswk, u32 mode );
static void BoxMoveNameChange( BOX2_SYS_WORK * syswk, u32 ret );
static void BoxMoveTrayIconChgOnly( BOX2_SYS_WORK * syswk, s8 mv );
static int BoxMoveFrmInSet( BOX2_SYS_WORK * syswk, int next );
static int BoxMoveFrmOutSet( BOX2_SYS_WORK * syswk, int next );
static int BoxMoveExitButtonInSet( BOX2_SYS_WORK * syswk, int next );
static int BoxMoveExitButtonOutSet( BOX2_SYS_WORK * syswk, int next );
static int PokeMenuOutSet( BOX2_SYS_WORK * syswk, int next );
static int BoxThemaChgStart( BOX2_SYS_WORK * syswk );
static int BoxThemaTrayIconSel( BOX2_SYS_WORK * syswk, u32 ret );
static int BoxThemaTrayIconChg( BOX2_SYS_WORK * syswk, s8 mv );
static void WallPaperPageChange( BOX2_SYS_WORK * syswk, s32 mv );
static void WallPaperPosSet( BOX2_SYS_WORK * syswk, u32 pos );
static int WallPaperChange( BOX2_SYS_WORK * syswk );
static int PartyInPokeCheck( BOX2_SYS_WORK * syswk );
static int PartyOutInit( BOX2_SYS_WORK * syswk );
static int PartyOutEnd( BOX2_SYS_WORK * syswk, BOOL flg );
static int PokeOutTraySelect( BOX2_SYS_WORK * syswk, u32 ret );
static int PokeOutTrayIconChg( BOX2_SYS_WORK * syswk, s8 mv );
static int PartyOutEnter( BOX2_SYS_WORK * syswk );

static int BoxArg_ItemGetYes( BOX2_SYS_WORK * syswk );
static int BoxArg_PokeFreeYes( BOX2_SYS_WORK * syswk );
static int BoxItemArg_RetBagYes( BOX2_SYS_WORK * syswk );
static int BoxArg_BoxEndYes( BOX2_SYS_WORK * syswk );
static int BoxArg_BoxEndNo( BOX2_SYS_WORK * syswk );

static int PartyPokeGetDrop( BOX2_SYS_WORK * syswk, u32 pos );
static int TrayPokeGetDrop( BOX2_SYS_WORK * syswk, u32 pos );

static int PartyPokeGetKey( BOX2_SYS_WORK * syswk, u32 pos );
static int PartyPokeGetKeyArrange( BOX2_SYS_WORK * syswk, u32 pos );

static int TrayPokeGetKey( BOX2_SYS_WORK * syswk, u32 pos );
static int TrayPokeGetKeyArrange( BOX2_SYS_WORK * syswk, u32 pos );

static int PokeMoveGetKey( BOX2_SYS_WORK * syswk, u32 pos );

static int TrayItemGetKey( BOX2_SYS_WORK * syswk, u32 pos );
static int TrayItemGet( BOX2_SYS_WORK * syswk, u32 pos );

static int PartyItemGetKey( BOX2_SYS_WORK * syswk, u32 pos );
static int PartyItemGet( BOX2_SYS_WORK * syswk, u32 pos );

static int ItemArrangeMenuStart( BOX2_SYS_WORK * syswk );

static void ItemMoveWorkAlloc( BOX2_SYS_WORK * syswk );
static void ItemMoveWorkFree( BOX2_SYS_WORK * syswk );

static int BoxPartyItemGet( BOX2_SYS_WORK * syswk, u32 pos );
static int BoxPartyItemGetKey( BOX2_SYS_WORK * syswk, u32 pos );
static int BoxPartyItemPutKey( BOX2_SYS_WORK * syswk, u32 pos );

static int BgButtonAnmSet( BOX2_SYS_WORK * syswk, u32 wfrmID, int next );
static int YStatusButtonAnmSet( BOX2_SYS_WORK * syswk, u32 wfrmID, int next );
static int MarkingButtonAnmSet( BOX2_SYS_WORK * syswk, u32 no, int next );
static int PartyButtonAnmSet( BOX2_SYS_WORK * syswk, u32 no, int next );


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

	MainSeq_ArrangeMain,
	MainSeq_ArrangeTrayPokeChgEnd,
	MainSeq_ArrangePokeMenuRcv,
	MainSeq_ArrangeStatusRcv,
	MainSeq_ArrangeStrInRcv,
	MainSeq_ArrangeBagRcv,
	MainSeq_ArrangeItemSetMsg,
	MainSeq_ArrangeMarkingInit,
	MainSeq_ArrangeMarkingFrmIn,
	MainSeq_ArrangeMarkingStart,
	MainSeq_ArrangeMarking,
	MainSeq_ArrangeMarkingRcv,
	MainSeq_MarkingCurMoveRet,
	MainSeq_ArrangePokeFreeStart,
	MainSeq_ArrangePokeFreeAnm,
	MainSeq_ArrangePokeFreeEnter1,
	MainSeq_ArrangePokeFreeEnter2,
	MainSeq_ArrangePokeFreeEnter3,
	MainSeq_ArrangePokeFreeError1,
	MainSeq_ArrangePokeFreeError2,
	MainSeq_ArrangePokeFreeError3,
	MainSeq_ArrangePartyPokeFreeEnd,
	MainSeq_ArrangePartyPokeFrmIn,
	MainSeq_ArrangePartyStart,
	MainSeq_ArrangePartyMain,
	MainSeq_ArrangePartyPokeFrmOut,
	MainSeq_ArrangeBoxPartyButtonIn,
	MainSeq_ArrangePartyPokeChgEnd,
	MainSeq_ArrangeBoxPartyChgStart,
	MainSeq_ArrangeBoxPartyChgMain,
	MainSeq_ArrangeBoxPartyPokeChgEnd,
	MainSeq_ArrangeBoxPartyPokeChgEndKey,
	MainSeq_ArrangeBoxPartyChgError,
	MainSeq_ArrangePartyFrmMoveREnd,
	MainSeq_ArrangePartyFrmMoveLEnd,
	MainSeq_ArrangeBoxMoveFrmIn,
	MainSeq_ArrangeExitButtonFrmIn,
	MainSeq_ArrangeBoxMoveFrmOutSet,
	MainSeq_ArrangeBoxMoveFrmOut,
	MainSeq_ArrangeBoxRetButtonIn,
	MainSeq_ArrangeBoxThemaButtonOut,
	MainSeq_ArrangeBoxThemaTrayIn,
	MainSeq_ArrangeBoxThemaExitIn,
	MainSeq_ArrangeBoxThemaMenuIn,
	MainSeq_ArrangeBoxThemaMsgSet,
	MainSeq_ArrangeBoxThemaExitOut,
	MainSeq_ArrangeBoxThemaMenuOut,
	MainSeq_ArrangeBoxThemaTrayOut,
	MainSeq_ArrangeBoxThemaButtonIn,
	MainSeq_ArrangeBoxThemaChgMain,
	MainSeq_BoxThemaChgCurMoveRet,
	MainSeq_ArrangeWallPaperFrmIn,
	MainSeq_ArrangeWallPaperMenuIn,
	MainSeq_ArrangeWallPaperChgStart,
	MainSeq_ArrangeWallPaperChgMain,
	MainSeq_ArrangeWallPaperMenuOut,
	MainSeq_ArrangeWallPaperFrmOut,
	MainSeq_ArrangeWallPaperChgEnd,
	MainSeq_ArrangeWallPaperChange,
	MainSeq_ArrangeWallPaperWhiteOut,
	MainSeq_ArrangeWallPaperWhiteIn,
	MainSeq_WallPaperChgCurMoveRet,
	MainSeq_ArrangeMainCurMoveRet,
	MainSeq_ArrangeBoxPartyChgMainCurMoveRet,
	MainSeq_ArrangePartyMainCurMoveRet,
	MainSeq_BoxArrangePokeMenuRcv,
	MainSeq_BoxArrangePartyMenuRcv,
	MainSeq_ArrangeItemSetEggError,
	MainSeq_ArrangeItemGetMailError,

	MainSeq_PartyInMain,
	MainSeq_PartyInMainRcv,
	MainSeq_PartyInPartyFrmIn,
	MainSeq_PartyInPokeMoveSet,
	MainSeq_PartyInPartyFrmOut,
	MainSeq_PartyInRetFrmIn,
	// ��Ă����d�l�ύX�@7/28
	MainSeq_PartyInPokeChgEnd,
	MainSeq_PartyInMainCurMoveRet,
	MainSeq_PartyInMenuCloseEnd,
	MainSeq_PartyInPokeMenuRcv,

	MainSeq_PartyOutMain,
	MainSeq_PartyOutStartCheck,
	MainSeq_PartyOutStartError,
	MainSeq_PartyOutStartRetFrmIn,
	MainSeq_PartyOutMenuFrmIn,
	MainSeq_PartyOutBoxSelectStart,
	MainSeq_PartyOutBoxSelectMain,
	MainSeq_PartyOutBoxMenuFrmOut,
	MainSeq_PartyOutEndRetFrmOut,
	MainSeq_PartyOutBoxMoveFrmOut,
	MainSeq_PartyOutEndRetFrmIn,
	MainSeq_PartyOutEnterButton,
	MainSeq_PartyOutCheck,
	MainSeq_PartyOutEnterPartyFrmRight,
	MainSeq_PartyOutEnterTrayScroll,
	MainSeq_PartyOutEnterPokeMove,
	MainSeq_PartyOutEnterPartyFrmLeft,
	MainSeq_PartyOutEnterRetIn,
	MainSeq_PartyOutComp,
	// ��������d�l�ύX�@7/28
	MainSeq_PartyOutPokeChgEnd,
	MainSeq_PartyOutMainCurMoveRet,
	MainSeq_PartyOutMenuCloseEnd,
	MainSeq_PartyOutBoxSelCurMoveRet,
	MainSeq_PartyOutPokeMenuRcv,

	MainSeq_KeyGetPokeMoveMain,
	MainSeq_KeyGetPokeMoveMainCurMoveRet,

	// �����
	MainSeq_ItemArrangeMain,
	MainSeq_ItemArrangeMainCurMoveRet,
	MainSeq_ItemArrangeRetBagSet,
	MainSeq_ItemArrangeRetBagCheck,
	MainSeq_ItemArrangeRetBagEnter,
	MainSeq_ItemArrangeRetBagCancel,
	MainSeq_ItemArrangeRetBagEnd,
	MainSeq_ItemArrangeGetBagCheck,
	MainSeq_ItemArrangeItemSetMsg,
	MainSeq_ItemArrangeIconMoveRet,
	MainSeq_ItemArrangeIconMovePokeAdd,
	MainSeq_ItemArrangeIconMoveChange,
	MainSeq_ItemArrangeIconMoveChangeEnd,
	MainSeq_ItemArrangeBoxPartyChgMain,
	MainSeq_ItemArrangeBoxPartyChgMainCurMoveRet,
	MainSeq_ItemArrangeBoxPartyIconMoveSubFrmOut,
	MainSeq_ItemArrangeBoxPartyIconMovePokeAdd,
	MainSeq_ItemArrangeBoxPartyIconMoveEnd,
	MainSeq_ItemArrangeBoxPartyChgError,
	MainSeq_ItemArrangeKeyGetMoveMain,
	MainSeq_ItemArrangeKeyGetMoveMainCurMoveRet,
	MainSeq_ItemArrangeKeyGetChangeCkack,
	MainSeq_ItemArrangePartyMain,
	MainSeq_ItemArrangePartyMainCurMoveRet,
	MainSeq_ItemArrangePartyIconMoveRet,
	MainSeq_ItemArrangePartyIconMovePokeAdd,
	MainSeq_ItemArrangePartyIconMovePokeAddEnd,
	MainSeq_ItemArrangePartyIconMoveChange,
	MainSeq_ItemArrangePartyIconMoveChangeEnd,
	MainSeq_ItemArrangePartyIconMoveErr,

	MainSeq_BoxEndInit,
	MainSeq_BoxEndCancelInit,
	MainSeq_ArrangePartyButton,
	MainSeq_ArrangeMoveButton,
	MainSeq_StatusButton,
	MainSeq_ItemButton,
	MainSeq_MarkingButton,
	MainSeq_MarkingEndButton,
	MainSeq_FreeButton,
	MainSeq_ArrangeCloseButton,
	MainSeq_PartyChgButton,
	MainSeq_ArrangePartyCloseButton,
	MainSeq_PartyEndButton,
	MainSeq_ArrangeBoxMoveButton,
	MainSeq_BoxMoveEndButton,
	MainSeq_BoxThemaTrayChgButton,
	MainSeq_BoxThemaWallPaperButton,
	MainSeq_BoxThemaNameButton,
	MainSeq_ArrangeWallPaperChgButton,
	MainSeq_ArrangeWallPaperChgCancelButton,
	MainSeq_PartyInButton,
	MainSeq_PartyInCloseButton,
	MainSeq_PartyOutButton,
	MainSeq_PartyOutCloseButton,
	MainSeq_ItemArrangePartyButton,
	MainSeq_ItemArrangeMoveButton,
	MainSeq_ItemArrangeBagButton,
	MainSeq_ItemArrangeCloseButton,
	MainSeq_ItemArrangeBoxMoveButton,
	MainSeq_ItemChgButton,
	MainSeq_ItemArrangePartyCloseButton,
	MainSeq_ArrangeMoveMenuButton,
};

// �T�u�v���Z�X�ݒ�
static const SUBGFL_PROC_DATA SubProcFunc[] =
//��[GS_CONVERT_TAG]
{
	// �X�e�[�^�X���
	{ BOX2MAIN_PokeStatusCall, BOX2MAIN_PokeStatusExit, BOX2SEQ_MAINSEQ_ARRANGE_STATUS_RCV },
	// �o�b�O���
	{ BOX2MAIN_BagCall, BOX2MAIN_BagExit, BOX2SEQ_MAINSEQ_ARRANGE_BAG_RCV },
	// ��������
	{ BOX2MAIN_NameInCall, BOX2MAIN_NameInExit, BOX2SEQ_MAINSEQ_ARRANGE_STRIN_RCV },
};

// �͂��E�������f�[�^�e�[�u��
static const YESNO_DATA YesNoFunc[] =
{
	// �{�b�N�X�����E�����a����H
	{ BoxArg_ItemGetYes, MainSeq_ArrangePokeMenuRcv },
	// �{�b�N�X�����E�|�P�����𓦂����H
	{ BoxArg_PokeFreeYes, MainSeq_ArrangePokeFreeError3 },
	// ������E�A�C�e�����܂��H
	{ BoxItemArg_RetBagYes, MainSeq_ItemArrangeRetBagCancel },
	// �{�b�N�X�I������H�i�{�^���j
	{ BoxArg_BoxEndYes, BoxArg_BoxEndNo },
	// �{�b�N�X�I������H�i�L�����Z���j
	{ BoxArg_BoxEndNo, BoxArg_BoxEndYes },
};

// ���j���[������e�[�u���F�{�b�N�X�������C��
static const BOX2BMP_BUTTON_LIST PokeMenuStrTbl[] = {
	{ mes_boxbutton_01_02, BOX2BMP_BUTTON_TYPE_RED },		// �|�P�������ǂ�
	{ mes_boxbutton_02_01, BOX2BMP_BUTTON_TYPE_WHITE },		// �悤�����݂�
	{ mes_boxbutton_02_02, BOX2BMP_BUTTON_TYPE_WHITE },		// ��������
	{ mes_boxbutton_02_03, BOX2BMP_BUTTON_TYPE_WHITE },		// �}�[�L���O
	{ mes_boxbutton_02_04, BOX2BMP_BUTTON_TYPE_WHITE }		// �ɂ���
};

// ���j���[������e�[�u���F��Ă���
static const BOX2BMP_BUTTON_LIST PartyInMenuStrTbl[] = {
	{ mes_boxbutton_02_05, BOX2BMP_BUTTON_TYPE_WHITE },		// ��Ă���
	{ mes_boxbutton_02_01, BOX2BMP_BUTTON_TYPE_WHITE },		// �悤�����݂�
	{ mes_boxbutton_02_03, BOX2BMP_BUTTON_TYPE_WHITE },		// �}�[�L���O
	{ mes_boxbutton_02_04, BOX2BMP_BUTTON_TYPE_WHITE }		// �ɂ���
};

// ���j���[������e�[�u���F��������
static const BOX2BMP_BUTTON_LIST PartyOutMenuStrTbl[] = {
	{ mes_boxbutton_02_06, BOX2BMP_BUTTON_TYPE_WHITE },		// ��������
	{ mes_boxbutton_02_01, BOX2BMP_BUTTON_TYPE_WHITE },		// �悤�����݂�
	{ mes_boxbutton_02_03, BOX2BMP_BUTTON_TYPE_WHITE },		// �}�[�L���O
	{ mes_boxbutton_02_04, BOX2BMP_BUTTON_TYPE_WHITE }		// �ɂ���
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
//		BGWINFRM_MoveMain( syswk->app->wfrm );
		BOX2BGWFRM_SubDispWinFrmMove( syswk );
		BOX2BMP_PrintUtilTrans( syswk->app );
		BOX2OBJ_AnmMain( syswk->app );
	}

	return TRUE;
}



//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�b�N�X��ʏ�����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Init( BOX2_SYS_WORK * syswk )
{
	FS_EXTERN_OVERLAY(ui_common);

	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

	// ���荞�ݒ�~
//	sys_VBlankFuncChange( NULL, NULL );
//	sys_HBlankIntrStop();
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

//	syswk->app->ppd_ah      = GFL_ARC_OpenDataHandle( ARCID_PERSONAL, HEAPID_BOX_APP );
	syswk->app->pokeicon_ah = GFL_ARC_OpenDataHandle( ARCID_POKEICON, HEAPID_BOX_APP );

	BOX2MAIN_VramBankSet();
	BOX2MAIN_BgInit( syswk );
	BOX2MAIN_PaletteFadeInit( syswk );

	BOX2MAIN_BgGraphicLoad( syswk );

	BOX2MAIN_MsgInit( syswk );

	BOX2BMP_Init( syswk );

	BOX2OBJ_Init( syswk );
	BOX2OBJ_TrayPokeIconChange( syswk );
	BOX2OBJ_PartyPokeIconChange( syswk );

	BOX2MAIN_WallPaperSet(
		syswk, BOX2MAIN_GetWallPaperNumber( syswk, syswk->tray ), BOX2MAIN_TRAY_SCROLL_NONE );

	BOX2BGWFRM_Init( syswk );

	if( syswk->dat->callMode != BOX_MODE_TURETEIKU && syswk->dat->callMode != BOX_MODE_AZUKERU ){
		BOX2BGWFRM_ButtonPutTemochi( syswk );
		BOX2BGWFRM_ButtonPutIdou( syswk );
	}
	BOX2BGWFRM_ButtonPutModoru( syswk );

	BOX2BMP_TitlePut( syswk );
	BOX2BMP_DefStrPut( syswk );

	BOX2OBJ_TrayIconCgxMakeAll( syswk );

	BOX2UI_CursorMoveInit( syswk );

	BOX2MAIN_YesNoWinInit( syswk );

//	syswk->app->vtask = VIntrTCB_Add( BOX2MAIN_VBlank, syswk, 0 );

	BOX2MAIN_SetBlendAlpha( TRUE );

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
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Release( BOX2_SYS_WORK * syswk )
{
	FS_EXTERN_OVERLAY(ui_common);

	BOX2MAIN_ExitVBlank( syswk );

/*
	BOX2MAIN_KeyTouchStatusSet( syswk );

	TCB_Delete( syswk->app->vtask );



	BOX2MAIN_BgFrameWorkExit( syswk->app );
*/
	BOX2MAIN_YesNoWinExit( syswk );

	BOX2UI_CursorMoveExit( syswk );

	BOX2BGWFRM_Exit( syswk->app );

	BOX2OBJ_Exit( syswk->app );

	BOX2BMP_Exit( syswk );

	BOX2MAIN_MsgExit( syswk );

	BOX2MAIN_PaletteFadeExit( syswk );
	BOX2MAIN_BgExit( syswk );

	GFL_ARC_CloseDataHandle( syswk->app->pokeicon_ah );
//	GFL_ARC_CloseDataHandle( syswk->app->ppd_ah );

	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );

	GFL_HEAP_FreeMemory( syswk->app );
	syswk->app = NULL;

	GFL_HEAP_DeleteHeap( HEAPID_BOX_APP );

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
 * ���C���V�[�P���X�F�p���b�g�t�F�[�h
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
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
 * ���C���V�[�P���X�F�E�F�C�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
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
 * ���C���V�[�P���X�FVBlank�����I���҂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_VFunc( BOX2_SYS_WORK * syswk )
{
	if( syswk->app->vfunk.func != NULL ){
		pBOX2_FUNC func = syswk->app->vfunk.func;
		if( func( syswk ) == 0 ){
			syswk->app->vfunk.func = NULL;
			return MainSeq[syswk->next_seq]( syswk );
		}
	}
/*
	if( syswk->app->vfunk.func == NULL ){
		return MainSeq[syswk->next_seq]( syswk );
	}
*/
	return BOX2SEQ_MAINSEQ_VFUNC;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�L�[�E�^�b�`�҂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_TrgWait( BOX2_SYS_WORK * syswk )
{
	if( GFL_UI_TP_GetTrg() == TRUE ){
//		Snd_SePlay( SE_BOX2_DECIDE );
		return syswk->next_seq;
	}

	if( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_CANCEL | PAD_BUTTON_DECIDE ) ){
//		Snd_SePlay( SE_BOX2_DECIDE );
		return syswk->next_seq;
	}

	return BOX2SEQ_MAINSEQ_TRGWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�͂��E������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_YesNo( BOX2_SYS_WORK * syswk )
{
/*
	switch( TOUCH_SW_Main( syswk->app->tsw ) ){
	case TOUCH_SW_RET_YES:
		TOUCH_SW_Reset( syswk->app->tsw );
		return YesNoFunc[syswk->app->ynID].yes( syswk );

	case TOUCH_SW_RET_NO:
		TOUCH_SW_Reset( syswk->app->tsw );
		return YesNoFunc[syswk->app->ynID].no( syswk );
	}
*/
	APP_TASKMENU_UpdateMenu( syswk->app->ynWork );
	if( APP_TASKMENU_IsFinish( syswk->app->ynWork ) == TRUE ){
		APP_TASKMENU_CloseMenu( syswk->app->ynWork );
		if( APP_TASKMENU_GetCursorPos( syswk->app->ynWork ) == 0 ){
			return YesNoFunc[syswk->app->ynID].yes( syswk );
		}else{
			return YesNoFunc[syswk->app->ynID].no( syswk );
		}
	}
	return BOX2SEQ_MAINSEQ_YESNO;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�^���A�j��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
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
 * ���C���V�[�P���X�F�T�u�v���Z�X�Ăяo��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_SubProcCall( BOX2_SYS_WORK * syswk )
{
	SubProcFunc[syswk->subProcType].call( syswk );
	return BOX2SEQ_MAINSEQ_SUB_PROCMAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�T�u�v���Z�X�I���҂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_SubProcMain( BOX2_SYS_WORK * syswk )
{
/*
	if( ProcMain( syswk->subProcFunc ) == FALSE ){
		return BOX2SEQ_MAINSEQ_SUB_PROCMAIN;
	}

	GFL_PROC_Delete( syswk->subProcFunc );
//��[GS_CONVERT_TAG]
	SubProcFunc[syswk->subProcType].exit( syswk );
	syswk->next_seq = SubProcFunc[syswk->subProcType].rcvSeq;
*/
	SubProcFunc[syswk->subProcType].exit( syswk );
	syswk->next_seq = SubProcFunc[syswk->subProcType].rcvSeq;
	return BOX2SEQ_MAINSEQ_INIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�J�n����
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Start( BOX2_SYS_WORK * syswk )
{
	int	seq;

//	seq = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;

//	Snd_SePlay( SE_BOX2_LOG_IN );

	switch( syswk->dat->callMode ){
	case BOX_MODE_AZUKERU:		// ��������
		PartyFrmSet_PartyOut( syswk );
		BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );

		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );

		BOX2MAIN_PokeInfoPut( syswk, BOX2OBJ_POKEICON_TRAY_MAX );

		seq = BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
		break;

	case BOX_MODE_TURETEIKU:	// ��Ă���
		BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );
		BOX2MAIN_PokeInfoPut( syswk, 0 );
		seq = BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
		break;

	case BOX_MODE_SEIRI:		// �{�b�N�X������
		BOX2MAIN_PokeInfoPut( syswk, 0 );
		seq = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		break;

	case BOX_MODE_ITEM:			// �A�C�e������
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
		BOX2MAIN_PokeInfoPut( syswk, 0 );
		seq = BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN;
		break;
	}

	return FadeInSet( syswk, seq );
}


//============================================================================================
//	�{�b�N�X����
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�������C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	ret = BOX2UI_HitTrayPokeTrg();
	if( ret != GFL_UI_TP_HIT_NONE ){
		OS_Printf( "get_pos = %d\n", ret );
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
				BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 5 );
			}
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
			return TrayPokeGet( syswk, ret );
		}else{
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
				u8	pos = syswk->get_pos;
				BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
				CURSORMOVE_PosSet( syswk->app->cmwk, pos );
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
				BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );
				BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
				BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
				BOX2OBJ_PokeCursorVanish( syswk, FALSE );
				return VFuncSet( syswk, BOX2MAIN_VFuncMenuCloseKeyArrange, BOX2SEQ_MAINSEQ_ARRANGE_MAIN_CURMOVE_RET );
			}else{
				CURSORMOVE_PosSet( syswk->app->cmwk, ret );
				CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
				BOX2MAIN_PokeInfoOff( syswk );
				return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
			}
		}
	}

	ret = BOX2UI_BoxArrangeMain( syswk );

	switch( ret ){
	case BOX2UI_ARRANGE_MAIN_NAME:		// 30: �{�b�N�X��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxThemaChgStart( syswk );

	case BOX2UI_ARRANGE_MAIN_LEFT:		// 31: �g���C�؂�ւ����E��
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MAIN_NAME );
		return TrayLeftScrollSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_MAIN );

	case BOX2UI_ARRANGE_MAIN_RIGHT:	// 32: �g���C�؂�ւ����E�E
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MAIN_NAME );
		return TrayRightScrollSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_MAIN );

	case BOX2UI_ARRANGE_MAIN_PARTY:	// 33: �莝���|�P����
//		Snd_SePlay( SE_BOX2_OPEN_PARTY_TRAY );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_POKE_BTN, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_BUTTON );

	case BOX2UI_ARRANGE_MAIN_CHANGE:	// 34: �|�P�����ړ�
//		Snd_SePlay( SE_BOX2_OPEN_PARTY_TRAY );
		syswk->quick_mode = 0;
		syswk->quick_get  = 0;
		return YStatusButtonAnmSet( syswk, BOX2MAIN_WINFRM_MV_BTN, BOX2SEQ_MAINSEQ_ARRANGE_MOVE_BUTTON );

	case BOX2UI_ARRANGE_MAIN_MOVE:		// 37: �|�P�������ǂ��i���j���[�j
//		Snd_SePlay( SE_BOX2_DECIDE );
		if( GFL_UI_TP_GetTrg() == FALSE ){
//��[GS_CONVERT_TAG]
			syswk->quick_mode = 1;
		}
		syswk->quick_get = syswk->get_pos;
		return YStatusButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU1, BOX2SEQ_MAINSEQ_ARRANGE_MOVE_MENU_BUTTON );

	case BOX2UI_ARRANGE_MAIN_STATUS:	// 38: �悤�����݂�
//		Snd_SePlay( SE_BOX2_DECIDE );
		syswk->cur_rcv_pos = BOX2UI_ARRANGE_MAIN_STATUS;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU2, BOX2SEQ_MAINSEQ_STATUS_BUTTON );

	case BOX2UI_ARRANGE_MAIN_ITEM:		// 39: ��������
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU3, BOX2SEQ_MAINSEQ_ITEM_BUTTON );

	case BOX2UI_ARRANGE_MAIN_MARKING:	// 40: �}�[�L���O
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, BOX2SEQ_MAINSEQ_MARKING_BUTTON );

	case BOX2UI_ARRANGE_MAIN_FREE:		// 41: �ɂ���
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_FREE_BUTTON );

	case BOX2UI_ARRANGE_MAIN_CLOSE:	// 42: �Ƃ���
//		Snd_SePlay( SE_BOX2_CANCEL );
		{
			u8	pos = syswk->get_pos;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_CLOSE_BTN, BOX2SEQ_MAINSEQ_ARRANGE_CLOSE_BUTTON );

	case CURSORMOVE_NONE:				// ����Ȃ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos == BOX2UI_ARRANGE_MAIN_NAME ){
				if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT ){
//					Snd_SePlay( SE_BOX2_CHANGE_TRAY );
					BOX2MAIN_PokeSelectOff( syswk );
					return TrayLeftScrollSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_MAIN );
				}
				if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT ){
//					Snd_SePlay( SE_BOX2_CHANGE_TRAY );
					BOX2MAIN_PokeSelectOff( syswk );
					return TrayRightScrollSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_MAIN );
				}
			}
		}
		break;

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_ARRANGE_MAIN_NAME ){
				BOX2MAIN_PokeInfoPut( syswk, pos );
			}else{
				if( pos != BOX2UI_ARRANGE_MAIN_MOVE &&
					pos != BOX2UI_ARRANGE_MAIN_STATUS &&
					pos != BOX2UI_ARRANGE_MAIN_ITEM &&
					pos != BOX2UI_ARRANGE_MAIN_MARKING &&
					pos != BOX2UI_ARRANGE_MAIN_FREE &&
					pos != BOX2UI_ARRANGE_MAIN_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_MAIN_CURMOVE_RET );

	case BOX2UI_ARRANGE_MAIN_RETURN:	// 35: ���ǂ�
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeInfoOff( syswk );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_RET_BTN, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case CURSORMOVE_CANCEL:				// �L�����Z��
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_RET_BTN, BOX2SEQ_MAINSEQ_BOXEND_CANCEL_INIT );

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_ARRANGE_MAIN_NAME ){
				BOX2MAIN_PokeInfoPut( syswk, pos );
			}else{
				if( pos != BOX2UI_ARRANGE_MAIN_MOVE &&
					pos != BOX2UI_ARRANGE_MAIN_STATUS &&
					pos != BOX2UI_ARRANGE_MAIN_ITEM &&
					pos != BOX2UI_ARRANGE_MAIN_MARKING &&
					pos != BOX2UI_ARRANGE_MAIN_FREE &&
					pos != BOX2UI_ARRANGE_MAIN_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		break;

	default:
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_DECIDE );
			BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 5 );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_ARRANGE_MAIN_MOVE, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MAIN_MOVE );
			return TrayPokeGetKeyArrange( syswk, ret );
		}
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�g���C�̃|�P�����ړ���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeTrayPokeChgEnd( BOX2_SYS_WORK * syswk )
{
//	Snd_SePlay( SE_BOX2_POKE_PUT );
	BOX2MAIN_PokeDataMove( syswk );
	PokeMoveWorkFree( syswk );
	CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MAIN_MOVE );
	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�|�P�������j���[�\����Ԃ֕��A
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeMenuRcv( BOX2_SYS_WORK * syswk )
{
	int	next;

	BOX2BMP_SysWinVanishTrans( syswk->app, BOX2BMPWIN_ID_MSG1 );

	switch( syswk->dat->callMode ){
	case BOX_MODE_TURETEIKU:
		BOX2BMP_MenuStrPrint( syswk, PartyInMenuStrTbl, 4 );
		BOX2BMP_PokePartyInMsgPut(
			syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG3 );
		next = BOX2SEQ_MAINSEQ_PARTYIN_POKE_MENU_RCV;
		break;

	case BOX_MODE_AZUKERU:
		BOX2BMP_MenuStrPrint( syswk, PartyOutMenuStrTbl, 4 );
		BOX2BMP_PokePartyInMsgPut(
			syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG3 );
		next = BOX2SEQ_MAINSEQ_PARTYOUT_POKE_MENU_RCV;
		break;

	case BOX_MODE_SEIRI:
	default:
		BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 5 );
		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			next = BOX2SEQ_MAINSEQ_BOXARRANGE_POKE_MENU_RCV;
		}else{
			next = BOX2SEQ_MAINSEQ_BOXARRANGE_PARTY_MENU_RCV;
		}
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

	return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, next );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�X�e�[�^�X��ʂ���̕��A
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int RcvStatus_ModeTureteiku( BOX2_SYS_WORK * syswk )
{
	int	next;

	if( syswk->y_status_flg == 0 ){
		BOX2BMP_MenuStrPrint( syswk, PartyInMenuStrTbl, 4 );
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );

		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2BMP_PokePartyInMsgPut(
				syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG3 );
			next = BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
		}else{
			PartyFrmSet( syswk );
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_STATUS );
			next = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
		}

		BOX2OBJ_PokeCursorAdd( syswk );
		BOX2BGWFRM_SubDispWazaFrmInPosSet( syswk->app );

	}else{
		PartyFrmSetRight( syswk );
		BOX2BGWFRM_YStatusButtonPut( syswk->app->wfrm );

		BOX2OBJ_BoxMoveObjInit( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2BMP_BoxMoveNameNumWrite( syswk );
		BOX2BGWFRM_BoxMoveFrmInSet( syswk->app->wfrm );
		while( BOX2MAIN_VFuncBoxMoveFrmIn(syswk) ){}

		if( syswk->y_status_hand == 0 ){
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->get_pos );
			next = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
		}else{
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->y_status_pos );
			syswk->app->poke_get_key = 1;
			BOX2MAIN_HandGetPokeSet( syswk );
			BOX2BGWFRM_SubDispWazaFrmInPosSet( syswk->app );
			next = BOX2SEQ_MAINSEQ_KEYGET_POKEMOVE_MAIN;
		}
		syswk->app->old_cur_pos = BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;

		if( syswk->pokechg_mode == 1 ){
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		}

		syswk->y_status_flg  = 0;
		syswk->y_status_pos  = 0;
		syswk->y_status_hand = 0;
	}

	return next;
}
static int RcvStatus_ModeAzukeru( BOX2_SYS_WORK * syswk )
{
	int	next;

	if( syswk->y_status_flg == 0 ){
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		BOX2BMP_MenuStrPrint( syswk, PartyOutMenuStrTbl, 4 );
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );

		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			next = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		}else{
			PartyFrmSet_PartyOut( syswk );
			BOX2BMP_PokePartyInMsgPut(
				syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG3 );
			next = BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
		}

		BOX2OBJ_PokeCursorAdd( syswk );
		BOX2BGWFRM_SubDispWazaFrmInPosSet( syswk->app );

	}else{
		PartyFrmSetRight( syswk );
		BOX2BGWFRM_YStatusButtonPut( syswk->app->wfrm );

		BOX2OBJ_BoxMoveObjInit( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2BMP_BoxMoveNameNumWrite( syswk );
		BOX2BGWFRM_BoxMoveFrmInSet( syswk->app->wfrm );
		while( BOX2MAIN_VFuncBoxMoveFrmIn(syswk) ){}

		if( syswk->y_status_hand == 0 ){
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->get_pos );
			next = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
		}else{
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->y_status_pos );
			syswk->app->poke_get_key = 1;
			BOX2MAIN_HandGetPokeSet( syswk );
			BOX2BGWFRM_SubDispWazaFrmInPosSet( syswk->app );
			next = BOX2SEQ_MAINSEQ_KEYGET_POKEMOVE_MAIN;
		}
		syswk->app->old_cur_pos = BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;

		if( syswk->pokechg_mode == 1 ){
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		}

		syswk->y_status_flg  = 0;
		syswk->y_status_pos  = 0;
		syswk->y_status_hand = 0;
	}

	return next;
}
static int RcvStatus_ModeSeiri( BOX2_SYS_WORK * syswk )
{
	int	next;

	if( syswk->y_status_flg == 0 ){
		if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
			if( syswk->box_mv_flg != 0 ){
				BOX2BGWFRM_BoxMoveFrmPut( syswk );
			}
		}else{
			if( syswk->box_mv_flg != 0 ){
				BOX2BGWFRM_BoxMoveFrmPut( syswk );
			}
		}
		BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 5 );
		BOX2BGWFRM_ArrangeUnderButtonDel( syswk );
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );

		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			next = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		}else{
			PartyFrmSet( syswk );
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_STATUS );
			next = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
		}

		BOX2OBJ_PokeCursorAdd( syswk );
		BOX2BGWFRM_SubDispWazaFrmInPosSet( syswk->app );

	}else{
		PartyFrmSetRight( syswk );
		BOX2BGWFRM_YStatusButtonPut( syswk->app->wfrm );

		BOX2OBJ_BoxMoveObjInit( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2BMP_BoxMoveNameNumWrite( syswk );
		BOX2BGWFRM_BoxMoveFrmInSet( syswk->app->wfrm );
		while( BOX2MAIN_VFuncBoxMoveFrmIn(syswk) ){}

		if( syswk->y_status_hand == 0 ){
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->get_pos );
			next = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
		}else{
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->y_status_pos );
			syswk->app->poke_get_key = 1;
			BOX2MAIN_HandGetPokeSet( syswk );
			BOX2BGWFRM_SubDispWazaFrmInPosSet( syswk->app );
			next = BOX2SEQ_MAINSEQ_KEYGET_POKEMOVE_MAIN;
		}
		syswk->app->old_cur_pos = BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;

		if( syswk->pokechg_mode == 1 ){
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		}

		syswk->y_status_flg  = 0;
		syswk->y_status_pos  = 0;
		syswk->y_status_hand = 0;
	}

	return next;
}
static int RcvStatus_ModeItem( BOX2_SYS_WORK * syswk )
{
	int	next;

	if( syswk->y_status_flg == 0 ){
		if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
			if( syswk->box_mv_flg != 0 ){
				BOX2BGWFRM_BoxMoveFrmPut( syswk );
			}
		}else{
			if( syswk->box_mv_flg != 0 ){
				BOX2BGWFRM_BoxMoveFrmPut( syswk );
			}
		}
		BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 5 );
		BOX2BGWFRM_ArrangeUnderButtonDel( syswk );
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );

		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			next = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		}else{
			PartyFrmSet( syswk );
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_STATUS );
			next = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
		}

		BOX2OBJ_PokeCursorAdd( syswk );

	}else{
		PartyFrmSetRight( syswk );
		BOX2BGWFRM_YStatusButtonPut( syswk->app->wfrm );

		BOX2OBJ_BoxMoveObjInit( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2BMP_BoxMoveNameNumWrite( syswk );
		BOX2BGWFRM_BoxMoveFrmInSet( syswk->app->wfrm );
		while( BOX2MAIN_VFuncBoxMoveFrmIn(syswk) ){}

		if( syswk->y_status_hand == 0 ){
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->get_pos );
		}else{
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->y_status_pos );
			syswk->app->poke_get_key = 1;
			BOX2MAIN_HandGetPokeSet( syswk );
		}
		syswk->app->old_cur_pos = BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;

		if( syswk->dat->callMode == BOX_MODE_ITEM ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
			next = BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
		}

		if( syswk->pokechg_mode == 1 ){
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		}

		syswk->y_status_flg  = 0;
		syswk->y_status_pos  = 0;
		syswk->y_status_hand = 0;
	}

	return next;
}

static int MainSeq_ArrangeStatusRcv( BOX2_SYS_WORK * syswk )
{
/*
	u8	next;

#ifdef	BTS_5436_TEST
	test_5436_flg = 1;
#endif	// BTS_5436_TEST

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

	if( syswk->y_status_flg == 0 ){
		if( syswk->dat->callMode == BOX_MODE_TURETEIKU ){
			BOX2BMP_MenuStrPrint( syswk, PartyInMenuStrTbl, 4 );
		}else if( syswk->dat->callMode == BOX_MODE_AZUKERU ){
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
			BOX2BMP_MenuStrPrint( syswk, PartyOutMenuStrTbl, 4 );
		}else{
			if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
				BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
				if( syswk->box_mv_flg != 0 ){
					BOX2BGWFRM_BoxMoveFrmPut( syswk );
				}
			}else{
				if( syswk->box_mv_flg != 0 ){
					BOX2BGWFRM_BoxMoveFrmPut( syswk );
				}
			}
			BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 5 );
			BOX2BGWFRM_ArrangeUnderButtonDel( syswk );
		}
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );

		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			if( syswk->dat->callMode == BOX_MODE_TURETEIKU ){
				BOX2BMP_PokePartyInMsgPut(
					syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG3 );
				next = BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
			}else{
				next = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
			}
		}else{
			if( syswk->dat->callMode == BOX_MODE_AZUKERU ){
				PartyFrmSet_PartyOut( syswk );
				BOX2BMP_PokePartyInMsgPut(
					syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG3 );
				next = BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
			}else{
				PartyFrmSet( syswk );
				BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
				BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_STATUS );
				next = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
			}
		}

		BOX2OBJ_PokeCursorAdd( syswk );

		if( syswk->dat->callMode != BOX_MODE_ITEM ){
			BOX2BGWFRM_SubDispWazaFrmInPosSet( syswk->app );
		}

	}else{
		PartyFrmSetRight( syswk );
		BOX2BGWFRM_YStatusButtonPut( syswk->app->wfrm );

		BOX2OBJ_BoxMoveObjInit( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2BMP_BoxMoveNameNumWrite( syswk );
		BOX2BGWFRM_BoxMoveFrmInSet( syswk->app->wfrm );
		while( BOX2MAIN_VFuncBoxMoveFrmIn(syswk) ){}

		if( syswk->y_status_hand == 0 ){
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->get_pos );
		}else{
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->y_status_pos );
			syswk->app->poke_get_key = 1;
			BOX2MAIN_HandGetPokeSet( syswk );
		}
		syswk->app->old_cur_pos = BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;

		if( syswk->dat->callMode == BOX_MODE_ITEM ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
			next = BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
		}else{
			if( syswk->y_status_hand == 0 ){
				next = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
			}else{
				next = BOX2SEQ_MAINSEQ_KEYGET_POKEMOVE_MAIN;
			}
		}

		if( syswk->dat->callMode != BOX_MODE_ITEM && syswk->y_status_hand != 0 ){
			BOX2BGWFRM_SubDispWazaFrmInPosSet( syswk->app );
		}

		if( syswk->pokechg_mode == 1 ){
			BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		}

		syswk->y_status_flg  = 0;
		syswk->y_status_pos  = 0;
		syswk->y_status_hand = 0;
	}

	return FadeInSet( syswk, next );
*/
	int	next;

#ifdef	BTS_5436_TEST
	test_5436_flg = 1;
#endif	// BTS_5436_TEST

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

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
	case BOX_MODE_ITEM:				// �ǂ���������
		next = RcvStatus_ModeItem( syswk );
		break;
	}

	return FadeInSet( syswk, next );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�������͉�ʂ���̕��A
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeStrInRcv( BOX2_SYS_WORK * syswk )
{
	BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BOXTHEMA_CHG, BOX2UI_BOXTHEMA_CHG_NANE );
	syswk->app->old_cur_pos = syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;

	BOX2BGWFRM_BoxThemaFrmPut( syswk );
	BOX2BMP_BoxThemaMsgPut( syswk, BOX2BMP_MSGID_THEMA_INIT, BOX2BMPWIN_ID_MSG3 );

	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
	}

	return FadeInSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_CHG_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�o�b�O��ʂ���̕��A
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int RcvBag_ModeAzukeru( BOX2_SYS_WORK * syswk )
{
	int	next;

	BOX2OBJ_PokeCursorAdd( syswk );

	if( syswk->subRet == 0 ){
		BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 5 );
		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			next = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		}else{
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_ITEM );
			next = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
		}
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
	}else{
		BOOL err = FALSE;
		if( syswk->subRet == ITEM_HAKKINDAMA &&
			BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_monsno, NULL ) != MONSNO_GIRATHINA ){
			err = TRUE;
		}else{
			POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );

			BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, ID_PARA_item, syswk->subRet );
			if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
				BOX2OBJ_PokeIconChange(
					syswk, syswk->tray, syswk->get_pos, syswk->app->pokeicon_id[syswk->get_pos] );
			}
			MYITEM_SubItem( syswk->dat->myitem, syswk->subRet, 1, HEAPID_BOX_APP );
		}

		if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_ITEM );
		}
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

		if( err == TRUE ){
			syswk->subRet = 0;	// ���ŎQ�Ƃ��邯�ǁA�ނ���O�̂ق����ǂ�
		}
		next = BOX2SEQ_MAINSEQ_ARRANGE_ITEMSET_MSG;
	}

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

	BOX2BGWFRM_SubDispWazaFrmInPosSet( syswk->app );

	return next;
}

static int RcvBag_ModeTureteiku( BOX2_SYS_WORK * syswk )
{
	int	next;

	BOX2OBJ_PokeCursorAdd( syswk );

	if( syswk->subRet == 0 ){
		BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 5 );
		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			next = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		}else{
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_ITEM );
			next = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
		}
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
	}else{
		BOOL err = FALSE;
		if( syswk->subRet == ITEM_HAKKINDAMA &&
			BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_monsno, NULL ) != MONSNO_GIRATHINA ){
			err = TRUE;
		}else{
			POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );

			BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, ID_PARA_item, syswk->subRet );
			if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
				BOX2OBJ_PokeIconChange(
					syswk, syswk->tray, syswk->get_pos, syswk->app->pokeicon_id[syswk->get_pos] );
			}
			MYITEM_SubItem( syswk->dat->myitem, syswk->subRet, 1, HEAPID_BOX_APP );
		}

		if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_ITEM );
		}
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

		if( err == TRUE ){
			syswk->subRet = 0;	// ���ŎQ�Ƃ��邯�ǁA�ނ���O�̂ق����ǂ�
		}
		next = BOX2SEQ_MAINSEQ_ARRANGE_ITEMSET_MSG;
	}

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

	BOX2BGWFRM_SubDispWazaFrmInPosSet( syswk->app );

	return next;
}

static int RcvBag_ModeSeiri( BOX2_SYS_WORK * syswk )
{
	int	next;

	BOX2OBJ_PokeCursorAdd( syswk );

	BOX2BGWFRM_ArrangeUnderButtonDel( syswk );
	BOX2BGWFRM_RetButtonOutPut( syswk->app->wfrm );

	if( syswk->subRet == 0 ){
		BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 5 );
		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			next = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		}else{
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_ITEM );
			next = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
		}
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
	}else{
		BOOL err = FALSE;
		if( syswk->subRet == ITEM_HAKKINDAMA &&
			BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_monsno, NULL ) != MONSNO_GIRATHINA ){
			err = TRUE;
		}else{
			POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );

			BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, ID_PARA_item, syswk->subRet );
			if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
				BOX2OBJ_PokeIconChange(
					syswk, syswk->tray, syswk->get_pos, syswk->app->pokeicon_id[syswk->get_pos] );
			}
			MYITEM_SubItem( syswk->dat->myitem, syswk->subRet, 1, HEAPID_BOX_APP );
		}

		if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_ITEM );
		}
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

		if( err == TRUE ){
			syswk->subRet = 0;	// ���ŎQ�Ƃ��邯�ǁA�ނ���O�̂ق����ǂ�
		}
		next = BOX2SEQ_MAINSEQ_ARRANGE_ITEMSET_MSG;
	}

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

	BOX2BGWFRM_SubDispWazaFrmInPosSet( syswk->app );

	return next;
}

static int RcvBag_ModeItem( BOX2_SYS_WORK * syswk )
{
	int	next;

	BOX2BGWFRM_ArrangeUnderButtonDel( syswk );
	BOX2BGWFRM_RetButtonOutPut( syswk->app->wfrm );

	if( syswk->subRet == 0 ){
		BOX2BMP_ItemArrangeMenuStrPrint( syswk, 0 );
		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
			next = BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN;
		}else{
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_A_PARTY, BOX2UI_ITEM_A_PARTY_MENU1 );
			next = BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN;
		}
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
	}else{
		BOOL err = FALSE;
		if( syswk->subRet == ITEM_HAKKINDAMA &&
			BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_monsno, NULL ) != MONSNO_GIRATHINA ){
			err = TRUE;
		}else{
			POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );

			BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, ID_PARA_item, syswk->subRet );
			if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
				BOX2OBJ_PokeIconChange(
					syswk, syswk->tray, syswk->get_pos, syswk->app->pokeicon_id[syswk->get_pos] );
			}
			MYITEM_SubItem( syswk->dat->myitem, syswk->subRet, 1, HEAPID_BOX_APP );
		}

		if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2UI_CursorMoveChange(
				syswk, BOX2UI_INIT_ID_ITEM_A_PARTY, syswk->get_pos-BOX2OBJ_POKEICON_TRAY_MAX );
		}else{
			const CURSORMOVE_DATA * pw;
			u8	x, y;
			CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
			pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, syswk->get_pos );
			x = pw->px;
			y = pw->py;
			BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );
		}
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

		if( err == FALSE ){
			syswk->app->get_item = syswk->subRet;
			BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
			if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
				BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
				BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_PARTY );
			}else{
				BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
				BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_TRAY );
			}
			BOX2OBJ_ItemIconCursorAdd( syswk->app );
			BOX2OBJ_ItemIconCursorOn( syswk->app );
		}else{
			syswk->subRet = 0;	// ���ŎQ�Ƃ��邯�ǁA�ނ���O�̂ق����ǂ�
			if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
				BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
			}else{
				BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
			}
		}
		next = BOX2SEQ_MAINSEQ_ITEMARRANGE_ITEMSET_MSG;
	}

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

	if( syswk->subRet != 0 ){
		BOX2BGWFRM_SubDispItemFrmInPosSet( syswk->app->wfrm );
	}

	return next;
}

static int MainSeq_ArrangeBagRcv( BOX2_SYS_WORK * syswk )
{
/*
	int	next;

	if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
		PartyFrmSet( syswk );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
	}
	if( syswk->dat->callMode != BOX_MODE_ITEM ){
		BOX2OBJ_PokeCursorAdd( syswk );
	}

	if( syswk->dat->callMode == BOX_MODE_SEIRI || syswk->dat->callMode == BOX_MODE_ITEM ){
		BOX2BGWFRM_ArrangeUnderButtonDel( syswk );
		BOX2BGWFRM_RetButtonOutPut( syswk->app->wfrm );
	}

	if( syswk->subRet == 0 ){
		if( syswk->dat->callMode == BOX_MODE_ITEM ){
			BOX2BMP_ItemArrangeMenuStrPrint( syswk, 0 );
			if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
				BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
				next = BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN;
			}else{
				BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
				BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_A_PARTY, BOX2UI_ITEM_A_PARTY_MENU1 );
				next = BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN;
			}
		}else{
			BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 5 );
			if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
				next = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
			}else{
				BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_ITEM );
				next = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
			}
		}
		BOX2BGWFRM_PokeMenuOpenPosSet( syswk->app->wfrm );
	}else{
		BOOL err = FALSE;
		if( syswk->subRet == ITEM_HAKKINDAMA &&
			BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_monsno, NULL ) != MONSNO_GIRATHINA ){
			err = TRUE;
		}else{
			POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );

			BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, ID_PARA_item, syswk->subRet );
			if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
				BOX2OBJ_PokeIconChange(
					syswk, syswk->tray, syswk->get_pos, syswk->app->pokeicon_id[syswk->get_pos] );
			}
			MYITEM_SubItem( syswk->dat->myitem, syswk->subRet, 1, HEAPID_BOX_APP );
		}

		if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
			if( syswk->dat->callMode == BOX_MODE_ITEM ){
				BOX2UI_CursorMoveChange(
					syswk, BOX2UI_INIT_ID_ITEM_A_PARTY, syswk->get_pos-BOX2OBJ_POKEICON_TRAY_MAX );
			}else{
				BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_ITEM );
			}
		}else{
			if( syswk->dat->callMode == BOX_MODE_ITEM ){
				const CURSORMOVE_DATA * pw;
				u8	x, y;
				CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
				pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, syswk->get_pos );
				x = pw->px;
				y = pw->py;
				BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );
			}
		}
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

		if( syswk->dat->callMode == BOX_MODE_ITEM ){
			if( err == FALSE ){
				syswk->app->get_item = syswk->subRet;
				BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
				BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
				if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
					BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
					BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_PARTY );
				}else{
					BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
					BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_TRAY );
				}
				BOX2OBJ_ItemIconCursorAdd( syswk->app );
				BOX2OBJ_ItemIconCursorOn( syswk->app );
			}else{
				syswk->subRet = 0;	// ���ŎQ�Ƃ��邯�ǁA�ނ���O�̂ق����ǂ�
				if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
					BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
				}else{
					BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
				}
			}
			next = BOX2SEQ_MAINSEQ_ITEMARRANGE_ITEMSET_MSG;
		}else{
			if( err == TRUE ){
				syswk->subRet = 0;	// ���ŎQ�Ƃ��邯�ǁA�ނ���O�̂ق����ǂ�
			}
			next = BOX2SEQ_MAINSEQ_ARRANGE_ITEMSET_MSG;
		}
	}

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

	if( syswk->dat->callMode != BOX_MODE_ITEM ){
		BOX2BGWFRM_SubDispWazaFrmInPosSet( syswk->app );
	}else{
		if( syswk->subRet != 0 ){
			BOX2BGWFRM_SubDispItemFrmInPosSet( syswk->app->wfrm );
		}
	}

	return FadeInSet( syswk, next );
*/
	int	next;

	if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
		PartyFrmSet( syswk );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
	}

	switch( syswk->dat->callMode ){
	case BOX_MODE_AZUKERU:		// ��������
		next = RcvBag_ModeAzukeru( syswk );
		break;
	case BOX_MODE_TURETEIKU:	// ��Ă���
		next = RcvBag_ModeTureteiku( syswk );
		break;
	case BOX_MODE_SEIRI:			// ������
		next = RcvBag_ModeSeiri( syswk );
		break;
	case BOX_MODE_ITEM:				// �ǂ���������
		next = RcvBag_ModeItem( syswk );
		break;
	}

	return FadeInSet( syswk, next );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F����������������b�Z�[�W�\��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeItemSetMsg( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_ItemSetMsgPut( syswk, syswk->subRet, BOX2BMPWIN_ID_MSG1 );
	syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
	return BOX2SEQ_MAINSEQ_TRGWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�}�[�L���O�t���[��������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 *
 *	���ۂ�MainSeq_ArrangeMarkingFrmIn()�Ɠ���
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeMarkingInit( BOX2_SYS_WORK * syswk )
{
	return MainSeq_ArrangeMarkingFrmIn( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�}�[�L���O�t���[���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeMarkingFrmIn( BOX2_SYS_WORK * syswk )
{
	syswk->app->pokeMark = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_mark, NULL );

	BOX2BGWFRM_MarkingFramePut( syswk );
	BOX2BMP_MarkingButtonPut( syswk->app );
	BOX2BGWFRM_MarkingFrameInSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncMarkingFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_MARKING_START );
}


//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�}�[�L���O�����J�n
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeMarkingStart( BOX2_SYS_WORK * syswk )
{
	BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_MARKING, 0 );

	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, TRUE );
	BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );

	BOX2BMP_MarkingMsgPut( syswk, BOX2BMPWIN_ID_MSG2 );
	return BOX2SEQ_MAINSEQ_ARRANGE_MARKING;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�}�[�L���O�������C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeMarking( BOX2_SYS_WORK * syswk )
{
	switch( CURSORMOVE_MainCont(syswk->app->cmwk) ){
	case BOX2UI_MARKING_MARK1:		// 00: ��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2BGWFRM_MarkingSwitch( syswk, 0 );
		break;
	case BOX2UI_MARKING_MARK2:		// 01: ��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2BGWFRM_MarkingSwitch( syswk, 1 );
		break;
	case BOX2UI_MARKING_MARK3:		// 02: ��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2BGWFRM_MarkingSwitch( syswk, 2 );
		break;
	case BOX2UI_MARKING_MARK4:		// 03: �n�[�g
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2BGWFRM_MarkingSwitch( syswk, 3 );
		break;
	case BOX2UI_MARKING_MARK5:		// 04: ��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2BGWFRM_MarkingSwitch( syswk, 4 );
		break;
	case BOX2UI_MARKING_MARK6:		// 05: ��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2BGWFRM_MarkingSwitch( syswk, 5 );
		break;

	case BOX2UI_MARKING_ENTER:		// 06:�u�����Ă��v
		{
			u8	mark = syswk->app->pokeMark;
			BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, ID_PARA_mark, mark );
			BOX2MAIN_SubDispMarkingChange( syswk, mark );
		}
//		Snd_SePlay( SE_BOX2_DECIDE );
		return MarkingButtonAnmSet( syswk, 0, BOX2SEQ_MAINSEQ_MARKING_END_BUTTON );

	case BOX2UI_MARKING_CANCEL:	// 07:�u��߂�v
	case CURSORMOVE_CANCEL:			// �L�����Z��
//		Snd_SePlay( SE_BOX2_CANCEL );
		return MarkingButtonAnmSet( syswk, 1, BOX2SEQ_MAINSEQ_MARKING_END_BUTTON );

	case CURSORMOVE_CURSOR_MOVE:	// �ړ�
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_MARKING_CURMOVE_RET );

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		break;
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_MARKING;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�}�[�W�L���O���I�����ă��j���[��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeMarkingRcv( BOX2_SYS_WORK * syswk )
{
	return VFuncSet( syswk, BOX2MAIN_VFuncUnderButtonMove, BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�|�P�����𓦂����J�n
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeFreeStart( BOX2_SYS_WORK * syswk )
{
	POKEMON_PASO_PARAM * ppp;

	if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
		POKEMON_PARAM * pp;
		u16	item;
		u32	pos;

		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, TRUE );
		BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );

		pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;

		// �莝���P
		if( BOX2MAIN_BattlePokeCheck( syswk, pos ) == FALSE ){
//			Snd_SePlay( SE_BOX2_WARNING );
			BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_ONE, BOX2BMPWIN_ID_MSG1 );
			syswk->app->poke_free_err = 1;
			syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
			return BOX2SEQ_MAINSEQ_TRGWAIT;
		}
		// ���[��
		pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, pos );
		item = PP_Get( pp, ID_PARA_item, NULL );
		if( ITEM_CheckMail( item ) == TRUE ){
//			Snd_SePlay( SE_BOX2_WARNING );
			BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_MAIL, BOX2BMPWIN_ID_MSG1 );
			syswk->app->poke_free_err = 1;
			syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
			return BOX2SEQ_MAINSEQ_TRGWAIT;
		}
/*	�J�X�^���{�[���͂Ȃ��n�Y
		// �J�v�Z��
		if( PP_Get( pp, ID_PARA_cb_id, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_WARNING );
			BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_CAPSULE, BOX2BMPWIN_ID_MSG1 );
			syswk->app->poke_free_err = 1;
			syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
			return BOX2SEQ_MAINSEQ_TRGWAIT;
		}
*/
	}else{
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );
	}

	ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );

	// �^�}�S
	if( PPP_Get( ppp, ID_PARA_tamago_flag, NULL ) != 0 ){
//		Snd_SePlay( SE_BOX2_WARNING );
		BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_EGG, BOX2BMPWIN_ID_MSG1 );
		syswk->app->poke_free_err = 1;
		syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
		return BOX2SEQ_MAINSEQ_TRGWAIT;
	}

	BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_CHECK, BOX2BMPWIN_ID_MSG1 );
	return YesNoSet( syswk, YESNO_ID_POKEFREE );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�|�P�����𓦂����A�j��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeFreeAnm( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_PokeFreeWazaCheck( syswk );

	if( BOX2OBJ_PokeIconFreeMove( syswk->app->seqwk ) == FALSE ){
		BOX2MAIN_POKEFREE_WORK * wk;
		u8	waza_flg;
		wk = syswk->app->seqwk;
		waza_flg = wk->check_flg;
		if( waza_flg != 0 ){
			// �߂�������
			return BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ERROR1;
		}else{
			// ����������
			BOX2OBJ_PokeIconFreeEnd( syswk->app->seqwk );
			BOX2MAIN_PokeFreeExit( syswk );
			return BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ENTER1;
		}
	}
	return BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�|�P�����𓦂������s�����P
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeFreeEnter1( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_ENTER, BOX2BMPWIN_ID_MSG1 );
	syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ENTER2;
	return BOX2SEQ_MAINSEQ_TRGWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�|�P�����𓦂������s�����Q
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeFreeEnter2( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_BY, BOX2BMPWIN_ID_MSG1 );
	syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ENTER3;
	return BOX2SEQ_MAINSEQ_TRGWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�|�P�����𓦂������s�����R
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeFreeEnter3( BOX2_SYS_WORK * syswk )
{
	void * func;

	BOX2MAIN_PokeDataClear( syswk, syswk->tray, syswk->get_pos );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_PokeInfoOff( syswk );

	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		// �g���C�A�C�R���X�V
		BOX2OBJ_TrayIconCgxMake( syswk, syswk->tray );
		BOX2OBJ_TrayIconCgxTrans( syswk, syswk->tray );
		if( syswk->dat->callMode == BOX_MODE_TURETEIKU ){
			BOX2BMP_PokePartyInMsgPut(
				syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );
			syswk->next_seq = BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
		}else{
			syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
			BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
			BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );
		}
		BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );

		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );

		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );

		syswk->get_pos = BOX2MAIN_GETPOS_NONE;

		func = BOX2MAIN_VFuncPokeFreeMenuMove;

	}else{
		PokeMoveWorkAlloc( syswk );
		syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_PARTY;
		syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_PARTYPOKE_FREE_END;
		BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );
		func = BOX2MAIN_VFuncPartyPokeFreeSort;
	}

	return VFuncSet( syswk, func, syswk->next_seq );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���|�P��������������̏���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyPokeFreeEnd( BOX2_SYS_WORK * syswk )
{
	PokeMoveWorkFree( syswk );
	if( syswk->box_mv_flg != 0 ){
		BOX2BMP_BoxMoveNameNumWrite( syswk );
	}

	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );

	CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX );
	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );

	syswk->get_pos = BOX2MAIN_GETPOS_NONE;

	if( syswk->dat->callMode == BOX_MODE_AZUKERU ){
		BOX2BMP_PokePartyInMsgPut(
			syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );
		return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�|�P�����𓦂������s�����P
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeFreeError1( BOX2_SYS_WORK * syswk )
{
	if( BOX2OBJ_PokeIconFreeErrorMove( syswk->app->seqwk ) == FALSE ){
		BOX2OBJ_PokeIconFreeErrorEnd( syswk->app->seqwk );
		BOX2MAIN_PokeFreeExit( syswk );
		BOX2OBJ_PokeCursorAdd( syswk );
		BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_RETURN, BOX2BMPWIN_ID_MSG1 );
		BOX2BGWFRM_SubDispWazaFrmInSet( syswk->app );
		syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ERROR2;
		return BOX2SEQ_MAINSEQ_TRGWAIT;
	}
	return BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ERROR1;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�|�P�����𓦂������s�����Q
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeFreeError2( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_FEAR, BOX2BMPWIN_ID_MSG1 );
	syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ERROR3;
	return BOX2SEQ_MAINSEQ_TRGWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�|�P�����𓦂������s�����R
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePokeFreeError3( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_PokeCursorVanish( syswk, TRUE );

	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
	}else{
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncPokeFreeMenuMove, BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������������v���C�������֕��A
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutPokeMenuRcv( BOX2_SYS_WORK * syswk )
{
	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���t���[���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyPokeFrmIn( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PartyPokeFrameLoadArrange( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY );
	BOX2BGWFRM_PartyPokeFrameInitPutLeft( syswk->app->wfrm );
	BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
	BOX2BMP_PartyCngButtonFrmPut( syswk->app );
	BOX2BMP_PartyChangeButtonPut( syswk->app );
	BOX2BMP_ButtonPutYameru( syswk );
	BOX2OBJ_PartyPokeIconFrmInSet( syswk );
	return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_START );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�{�b�N�X������v�莝���|�P�����I��������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyStart( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_PokeInfoPut( syswk, BOX2OBJ_POKEICON_TRAY_MAX );

	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_A_PARTY, BOX2UI_ITEM_A_PARTY_POKE1 );
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN;
	}
	BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_POKE1 );
	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
}


//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���I�����C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	ret = BOX2UI_HitCheckTrgPartyPokeLeft();
	if( ret != GFL_UI_TP_HIT_NONE ){
//��[GS_CONVERT_TAG]
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
//��[GS_CONVERT_TAG]
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
				BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 5 );
			}
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
			return PartyPokeGet( syswk, ret );
		}else{
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
				u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
				BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
				CURSORMOVE_PosSet( syswk->app->cmwk, pos );
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
				BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
				BOX2OBJ_PokeCursorVanish( syswk, FALSE );
				return VFuncSet( syswk, BOX2MAIN_VFuncMenuCloseKeyArrange, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN_CURMOVE_RET );
			}else{
				CURSORMOVE_PosSet( syswk->app->cmwk, ret - BOX2OBJ_POKEICON_TRAY_MAX );
				CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
				BOX2MAIN_PokeInfoOff( syswk );
				return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
			}
		}
	}

	ret = BOX2UI_BoxArrangePartyMoveMain( syswk );

	switch( ret ){

	case BOX2UI_ARRANGE_PARTY_CHANGE:	//�u���ꂩ���v
//		Snd_SePlay( SE_BOX2_DECIDE );
		syswk->quick_mode = 0;
		syswk->quick_get  = 0;
		return PartyButtonAnmSet( syswk, 0, BOX2SEQ_MAINSEQ_PARTY_CHG_BUTTON );

	case BOX2UI_ARRANGE_PARTY_MOVE:	//�u�|�P�������ǂ��v�i���j���[�j
//		Snd_SePlay( SE_BOX2_DECIDE );
		if( GFL_UI_TP_GetTrg() == FALSE ){
//��[GS_CONVERT_TAG]
			syswk->quick_mode = 2;
		}
		syswk->quick_get = syswk->get_pos;
		return YStatusButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU1, BOX2SEQ_MAINSEQ_PARTY_CHG_BUTTON );

	case BOX2UI_ARRANGE_PARTY_STATUS:	//�u�悤�����݂�v
//		Snd_SePlay( SE_BOX2_DECIDE );
		syswk->cur_rcv_pos = BOX2UI_ARRANGE_PARTY_STATUS;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU2, BOX2SEQ_MAINSEQ_STATUS_BUTTON );

	case BOX2UI_ARRANGE_PARTY_ITEM:	//�u�������́v
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU3, BOX2SEQ_MAINSEQ_ITEM_BUTTON );

	case BOX2UI_ARRANGE_PARTY_MARKING:	//�u�}�[�L���O�v
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, BOX2SEQ_MAINSEQ_MARKING_BUTTON );

	case BOX2UI_ARRANGE_PARTY_FREE:	//�u�ɂ����v
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_FREE_BUTTON );

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos <= BOX2UI_ARRANGE_PARTY_POKE6 ){
				BOX2MAIN_PokeInfoPut( syswk, pos+BOX2OBJ_POKEICON_TRAY_MAX );
			}else{
				if( pos != BOX2UI_ARRANGE_PARTY_MOVE &&
					pos != BOX2UI_ARRANGE_PARTY_STATUS &&
					pos != BOX2UI_ARRANGE_PARTY_ITEM &&
					pos != BOX2UI_ARRANGE_PARTY_MARKING &&
					pos != BOX2UI_ARRANGE_PARTY_FREE &&
					pos != BOX2UI_ARRANGE_PARTY_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN_CURMOVE_RET );

	case BOX2UI_ARRANGE_PARTY_RETURN:	//�u���ǂ�v
	case CURSORMOVE_CANCEL:				// �L�����Z��
//		Snd_SePlay( SE_BOX2_CLOSE_PARTY_TRAY );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_RET_BTN, BOX2SEQ_MAINSEQ_PARTY_END_BUTTON );

	case BOX2UI_ARRANGE_PARTY_CLOSE:	//�u�Ƃ���v
//		Snd_SePlay( SE_BOX2_CANCEL );
		{
			u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_CLOSE_BTN, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_CLOSE_BUTTON );

	case CURSORMOVE_CURSOR_ON:			// �J�[�\���\��
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos <= BOX2UI_ARRANGE_PARTY_POKE6 ){
				BOX2MAIN_PokeInfoPut( syswk, pos+BOX2OBJ_POKEICON_TRAY_MAX );
			}else{
				if( pos != BOX2UI_ARRANGE_PARTY_MOVE &&
					pos != BOX2UI_ARRANGE_PARTY_STATUS &&
					pos != BOX2UI_ARRANGE_PARTY_ITEM &&
					pos != BOX2UI_ARRANGE_PARTY_MARKING &&
					pos != BOX2UI_ARRANGE_PARTY_FREE &&
					pos != BOX2UI_ARRANGE_PARTY_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		break;

	case CURSORMOVE_NONE:				// ����Ȃ�
		break;

	default:
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_DECIDE );
			BOX2BMP_MenuStrPrint( syswk, PokeMenuStrTbl, 5 );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_ARRANGE_PARTY_MOVE, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_PARTY_MOVE );
			return PartyPokeGetKeyArrange( syswk, ret );
		}
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���t���[���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyPokeFrmOut( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_BUTTON_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X/�莝���{�^���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxPartyButtonIn( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PartyPokeFrameOff( syswk );

	BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );

	BOX2BMP_ButtonPutTemochi( syswk );
	BOX2BMP_ButtonPutIdou( syswk );
	if( syswk->box_mv_flg == 0 ){
		BOX2BMP_ButtonPutModoru( syswk );
	}

	BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );

	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_A_MAIN, BOX2UI_ITEM_A_MAIN_PARTY );
		return VFuncSet( syswk, BOX2MAIN_VFuncUnderButtonMove, BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN );
	}

	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
	BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MAIN, BOX2UI_ARRANGE_MAIN_PARTY );
	return VFuncSet( syswk, BOX2MAIN_VFuncUnderButtonMove, BOX2SEQ_MAINSEQ_ARRANGE_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���|�P��������ւ���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyPokeChgEnd( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_POKEMOVE_WORK * work;
	u32	pos;

	work = syswk->app->vfunk.work;
	pos  = work->get_pos;

	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG4 );

//	Snd_SePlay( SE_BOX2_POKE_PUT );
	BOX2MAIN_PokeDataMove( syswk );
	BOX2MAIN_FormChangeSheimi( syswk, work->get_pos, work->put_pos );
	PokeMoveWorkFree( syswk );

	if( syswk->get_pos == BOX2MAIN_GETPOS_NONE ){
		CURSORMOVE_PosSet( syswk->app->cmwk, pos - BOX2OBJ_POKEICON_TRAY_MAX );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		BOX2MAIN_PokeInfoPut( syswk, pos );
	}else{
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_PARTY_MOVE );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���E�g���C����ւ����C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxPartyChgStart( BOX2_SYS_WORK * syswk )
{
	// �g���C�I������߂�Ƃ��̃f�t�H���g�ʒu�ݒ�
	syswk->app->old_cur_pos = BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;

	if( syswk->quick_mode != 0 ){
		int	ret;
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->quick_get );
		syswk->get_pos = syswk->quick_get;
		ret = PokeMoveGetKey( syswk, syswk->quick_get );
		syswk->quick_mode = 0;
		return ret;
	}else{
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MOVE, syswk->quick_get );
		BOX2MAIN_PokeInfoPut( syswk, syswk->quick_get );
	}

	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
}

// �f�o�b�O�I�[�g�v���C
//#define	BTS_2299_TEST
#ifdef	BTS_2299_TEST
static int test_flg_2299 = 0;
static int test_pos_2299 = 0;
#endif	// BTS_2299_TEST

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�{�b�N�X������v�|�P�����ړ����C������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxPartyChgMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	// �莝��
	ret = BOX2UI_HitCheckTrgPartyPokeRight();
	if( ret != GFL_UI_TP_HIT_NONE ){
//��[GS_CONVERT_TAG]
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
//��[GS_CONVERT_TAG]
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
			return BoxPartyPokeGet( syswk, ret );
		}else{
			BOX2MAIN_PokeInfoOff( syswk );
			CURSORMOVE_PosSet( syswk->app->cmwk, ret );
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
			if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == TRUE ){
				return BoxMoveButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN );
			}else if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
				return YStatusButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN );
			}
			return BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
		}
	}

	// �{�b�N�X
	ret = BOX2UI_HitTrayPokeTrg();
	if( ret != GFL_UI_TP_HIT_NONE ){
//��[GS_CONVERT_TAG]
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
//��[GS_CONVERT_TAG]
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
			return BoxPartyPokeGet( syswk, ret );
		}else{
			BOX2MAIN_PokeInfoOff( syswk );
			CURSORMOVE_PosSet( syswk->app->cmwk, ret );
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
			if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == TRUE ){
				return BoxMoveButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN );
			}else if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
				return YStatusButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN );
			}
			return BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
		}
	}

	// �x�X�e�[�^�X�{�^��
	if( BOX2UI_YStatusButtonCheck( syswk ) == TRUE ){
		ret = CURSORMOVE_PosGet( syswk->app->cmwk );
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_DECIDE );
			syswk->get_pos = ret;
			syswk->y_status_flg = 1;
			return YStatusButtonAnmSet( syswk, BOX2MAIN_WINFRM_Y_ST_BTN, BOX2SEQ_MAINSEQ_STATUS_BUTTON );
		}
		return BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
	}

	ret = BOX2UI_BoxArrangePokeMove( syswk );

#ifdef	BTS_2299_TEST
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
//��[GS_CONVERT_TAG]
		test_flg_2299 = 1;
		test_pos_2299 = 0;
	}
	if( test_flg_2299 == 1 ){
		if( test_pos_2299 != 0 ){
			test_pos_2299 = 0;
		}else{
			test_pos_2299 = 2;
		}
		ret = test_pos_2299;
		CURSORMOVE_PosSet( syswk->app->cmwk, ret );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	}
#endif	// BTS_2299_TEST

	switch( ret ){
	case BOX2UI_ARRANGE_MOVE_TRAY1:	// 37: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveButtonFrmIn( syswk, 0 );

	case BOX2UI_ARRANGE_MOVE_TRAY2:	// 38: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveButtonFrmIn( syswk, 1 );

	case BOX2UI_ARRANGE_MOVE_TRAY3:	// 39: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveButtonFrmIn( syswk, 2 );

	case BOX2UI_ARRANGE_MOVE_TRAY4:	// 40: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveButtonFrmIn( syswk, 3 );

	case BOX2UI_ARRANGE_MOVE_TRAY5:	// 41: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveButtonFrmIn( syswk, 4 );

	case BOX2UI_ARRANGE_MOVE_TRAY6:	// 42: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveButtonFrmIn( syswk, 5 );

	case BOX2UI_ARRANGE_MOVE_LEFT:		// 43: �g���C���i���j
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		BoxMoveTrayIconChgOnly( syswk, -1 );
		CURSORMOVE_PosSet( syswk->app->cmwk, (syswk->box_mv_pos%BOX2OBJ_TRAYICON_MAX)+BOX2UI_ARRANGE_MOVE_TRAY1 );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == TRUE ){
			return BoxMoveButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN );
		}else if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
			return YStatusButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN );
		}
		return BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;

	case BOX2UI_ARRANGE_MOVE_RIGHT:	// 44: �g���C���i�E�j
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		BoxMoveTrayIconChgOnly( syswk, 1 );
		CURSORMOVE_PosSet( syswk->app->cmwk, (syswk->box_mv_pos%BOX2OBJ_TRAYICON_MAX)+BOX2UI_ARRANGE_MOVE_TRAY1 );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == TRUE ){
			return BoxMoveButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN );
		}else if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
			return YStatusButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN );
		}
		return BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;

	case BOX2UI_ARRANGE_MOVE_CHANGE:	// 45: �{�b�N�X�����肩����
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_BOXMV_BTN, BOX2SEQ_MAINSEQ_ARRANGE_BOX_MOVE_BUTTON );

	case CURSORMOVE_CURSOR_MOVE:	// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_ARRANGE_MOVE_TRAY1 ){
				if( BOX2MAIN_PokeInfoPut( syswk, pos ) == TRUE ){
					if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == FALSE ){
						BOX2BMP_ButtonPutYStatus( syswk );
						BOX2BGWFRM_YStatusButtonInSet( syswk->app->wfrm );
					}
				}else{
					if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
						BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
					}
				}
			}else{
				BOX2MAIN_PokeInfoOff( syswk );
				if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
					BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
				}
			}
		}
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		if( syswk->app->vfunk.freq != NULL ){
			return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN_CURMOVE_RET );
		}
		break;

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
	case CURSORMOVE_NONE:			// ����Ȃ�
		break;

	case BOX2UI_ARRANGE_MOVE_RETURN:	// 36: ��߂�
//		Snd_SePlay( SE_BOX2_CLOSE_PARTY_TRAY );
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
		return PartyButtonAnmSet( syswk, 1, BOX2SEQ_MAINSEQ_BOX_MOVE_END_BUTTON );

	case CURSORMOVE_CANCEL:				// �L�����Z��
		if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == FALSE ){
//			Snd_SePlay( SE_BOX2_CLOSE_PARTY_TRAY );
			return PartyButtonAnmSet( syswk, 1, BOX2SEQ_MAINSEQ_BOX_MOVE_END_BUTTON );
		}else{
//			Snd_SePlay( SE_BOX2_CANCEL );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX );
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
			return BoxMoveButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN );
		}
		break;

	default:
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_POKE_CATCH );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			return PokeMoveGetKey( syswk, ret );
		}
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���E�g���C����ւ���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxPartyPokeChgEnd( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_POKEMOVE_WORK * work;
	u32	put_pos;
	u32	set_pos;

	work = syswk->app->vfunk.work;
	put_pos = work->put_pos;
	set_pos = work->set_pos;

//	Snd_SePlay( SE_BOX2_POKE_PUT );
	BOX2MAIN_PokeDataMove( syswk );
	if( ( put_pos & BOX2MAIN_BOXMOVE_FLG ) == 0 ){
		BOX2MAIN_FormChangeSheimi( syswk, work->get_pos, work->put_pos );
	}
	PokeMoveWorkFree( syswk );
	CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );

	if( syswk->box_mv_flg != 0 ){
		BOX2BMP_BoxMoveNameNumWrite( syswk );
	}
	if( BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_poke_exist, NULL ) == 0 ){
		BOX2MAIN_PokeInfoOff( syswk );
	}else{
		if( put_pos != BOX2MAIN_GETPOS_NONE ){
			if( ( put_pos & BOX2MAIN_BOXMOVE_FLG ) != 0 ){
				BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
			}
		}else{
			if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX && set_pos != syswk->get_pos ){
				u32	party_pos;
				POKEMON_PARAM * pp;

				party_pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
				pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, party_pos );

				if( ITEM_CheckMail( PP_Get(pp,ID_PARA_item,NULL) ) == TRUE ){
//					Snd_SePlay( SE_BOX2_WARNING );
					BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_MAIL, BOX2BMPWIN_ID_MSG1 );
					syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_ERROR;
					return BOX2SEQ_MAINSEQ_TRGWAIT;
/*	�J�X�^���{�[���͂Ȃ��n�Y
				}else if( PP_Get( pp, ID_PARA_cb_id, NULL ) != 0 ){
//					Snd_SePlay( SE_BOX2_WARNING );
					BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_CAPSULE, BOX2BMPWIN_ID_MSG1 );
					syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_ERROR;
					return BOX2SEQ_MAINSEQ_TRGWAIT;
*/
				}else if( BOX2MAIN_BattlePokeCheck( syswk, party_pos ) == FALSE ){
//					Snd_SePlay( SE_BOX2_WARNING );
					BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_ONE, BOX2BMPWIN_ID_MSG1 );
					syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_ERROR;
					return BOX2SEQ_MAINSEQ_TRGWAIT;
				}
			}

			if( set_pos != BOX2MAIN_GETPOS_NONE &&
				( set_pos & BOX2MAIN_BOXMOVE_FLG ) != 0 &&
				BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos)+(set_pos^BOX2MAIN_BOXMOVE_FLG) != syswk->tray ){
				if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
					BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_BOXMAX, BOX2BMPWIN_ID_MSG1 );
				}else if( BOX2MAIN_BattlePokeCheck( syswk, syswk->get_pos-BOX2OBJ_POKEICON_TRAY_MAX ) == TRUE ){
					BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_BOXMAX, BOX2BMPWIN_ID_MSG1 );
				}else{
					BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_ONE, BOX2BMPWIN_ID_MSG1 );
				}
//				Snd_SePlay( SE_BOX2_WARNING );
				syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_ERROR;
				return BOX2SEQ_MAINSEQ_TRGWAIT;
			}
		}
	}

	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );

	return BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���E�g���C����ւ���i�L�[����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxPartyPokeChgEndKey( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_POKEMOVE_WORK * work;
	u32	put_pos, get_pos;
	u8	pos;

	work = syswk->app->vfunk.work;
	get_pos = work->get_pos;
	put_pos = work->put_pos;

//	Snd_SePlay( SE_BOX2_POKE_PUT );
	BOX2MAIN_PokeDataMove( syswk );
	if( ( put_pos & BOX2MAIN_BOXMOVE_FLG ) == 0 ){
		BOX2MAIN_FormChangeSheimi( syswk, work->get_pos, work->put_pos );
	}
	PokeMoveWorkFree( syswk );
	if( syswk->box_mv_flg != 0 ){
		BOX2BMP_BoxMoveNameNumWrite( syswk );
	}

	pos = CURSORMOVE_PosGet( syswk->app->cmwk );

	if( syswk->app->cancel_flg == FALSE ){
		if( put_pos == BOX2MAIN_GETPOS_NONE && pos != syswk->get_pos ){
			if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
				u32	party_pos;
				POKEMON_PARAM * pp;

				party_pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
				pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, party_pos );

				if( ITEM_CheckMail( PP_Get(pp,ID_PARA_item,NULL) ) == TRUE ){
//					Snd_SePlay( SE_BOX2_WARNING );
					CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
					CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
					BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
					BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_MAIL, BOX2BMPWIN_ID_MSG1 );
					syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_ERROR;
					return BOX2SEQ_MAINSEQ_TRGWAIT;
/*	�J�X�^���{�[���͂Ȃ��n�Y
				}else if( PP_Get( pp, ID_PARA_cb_id, NULL ) != 0 ){
//					Snd_SePlay( SE_BOX2_WARNING );
					CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
					CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
					BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
					BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_CAPSULE, BOX2BMPWIN_ID_MSG1 );
					syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_ERROR;
					return BOX2SEQ_MAINSEQ_TRGWAIT;
*/
				}else if( BOX2MAIN_BattlePokeCheck( syswk, party_pos ) == FALSE ){
//					Snd_SePlay( SE_BOX2_WARNING );
					CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
					CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
					BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
					BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_ONE, BOX2BMPWIN_ID_MSG1 );
					syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_ERROR;
					return BOX2SEQ_MAINSEQ_TRGWAIT;
				}
			}
			if( pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && pos <= BOX2UI_ARRANGE_MOVE_TRAY6 &&
				BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos)+(pos-BOX2UI_ARRANGE_MOVE_TRAY1) != syswk->tray ){
				if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
					BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_BOXMAX, BOX2BMPWIN_ID_MSG1 );
				}else if( BOX2MAIN_BattlePokeCheck( syswk, syswk->get_pos-BOX2OBJ_POKEICON_TRAY_MAX ) == TRUE ){
					BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_BOXMAX, BOX2BMPWIN_ID_MSG1 );
				}else{
					BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_ONE, BOX2BMPWIN_ID_MSG1 );
				}
//				Snd_SePlay( SE_BOX2_WARNING );
				CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
				CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
				BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
				syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_ERROR;
				return BOX2SEQ_MAINSEQ_TRGWAIT;
			}
			if( syswk->quick_mode == 0 ){
				BOX2MAIN_PokeInfoPut( syswk, CURSORMOVE_PosGet(syswk->app->cmwk) );
			}
		}
	}

	if( syswk->quick_mode != 0 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveEndStart( syswk );
	}

	if( pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F����ւ��G���[���b�Z�[�W�\����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxPartyChgError( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_SysWinVanishTrans( syswk->app, BOX2BMPWIN_ID_MSG1 );

	if( syswk->quick_mode != 0 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveEndStart( syswk );
	}

	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	return BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���t���[���E�ړ���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyFrmMoveREnd( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_PartySelectButtonPut( syswk->app );

	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
	}else{
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
	}

	if( BOX2MAIN_PokeParaGet( syswk, syswk->quick_get, ID_PARA_poke_exist, NULL ) != 0 ){
		BOX2BMP_ButtonPutYStatus( syswk );
		BOX2BGWFRM_YStatusButtonInSet( syswk->app->wfrm );
	}

	syswk->box_mv_flg = 1;		// �{�b�N�X�ړ��t���O
	syswk->pokechg_mode = 1;

	syswk->box_mv_pos = syswk->tray;

	return BoxMoveFrmInSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_START );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���t���[�����ړ���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyFrmMoveLEnd( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_PartyChangeButtonPut( syswk->app );

	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_A_PARTY, BOX2UI_ITEM_A_PARTY_POKE1 );
		BOX2MAIN_PokeInfoPut(
			syswk, CURSORMOVE_PosGet(syswk->app->cmwk)+BOX2OBJ_POKEICON_TRAY_MAX );
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN;
	}

	if( syswk->quick_mode != 0 ){
		BOX2UI_CursorMoveChange(
			syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, syswk->quick_get-BOX2OBJ_POKEICON_TRAY_MAX );
	}else{
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_POKE1 );
	}
	BOX2MAIN_PokeInfoPut(
		syswk, CURSORMOVE_PosGet(syswk->app->cmwk)+BOX2OBJ_POKEICON_TRAY_MAX );
	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�ړ��t���[���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxMoveFrmIn( BOX2_SYS_WORK * syswk )
{
	syswk->box_mv_pos = syswk->tray;

	return BoxMoveFrmInSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_EXIT_BUTTON_FRMIN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F��߂�t���[���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeExitButtonFrmIn( BOX2_SYS_WORK * syswk )
{
/*
���̏������莝���t���[���C���ɕύX
*/
	BOX2BGWFRM_PartyPokeFrameLoadArrange( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY );
	BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
	BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
	BOX2BMP_PartyCngButtonFrmPut( syswk->app );
	BOX2BMP_PartySelectButtonPut( syswk->app );
	BOX2OBJ_PartyPokeIconFrmInSetRight( syswk );

	BOX2BMP_ButtonPutYStatus( syswk );
	if( BOX2MAIN_PokeParaGet( syswk, syswk->quick_get, ID_PARA_poke_exist, NULL ) != 0 ){
		BOX2BGWFRM_YStatusButtonInSet( syswk->app->wfrm );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_START );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�ړ��t���[���A�E�g�ݒ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxMoveFrmOutSet( BOX2_SYS_WORK * syswk )
{
	return BoxMoveFrmOutSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXMOVE_FRMOUT );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�ړ��t���[���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxMoveFrmOut( BOX2_SYS_WORK * syswk )
{
	if( syswk->pokechg_mode == 1 ){
		return PartyFrmMoveLeft( syswk );
	}
	BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_BOX_RET_BUTTON_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X/���ǂ�{�^���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxRetButtonIn( BOX2_SYS_WORK * syswk )
{
	u32	id;
	u32	pos;
	int	next;

	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		id   = BOX2UI_INIT_ID_ITEM_A_MAIN;
		pos  = BOX2UI_ITEM_A_MAIN_CHANGE;
		next = BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN;
	}else{
		id   = BOX2UI_INIT_ID_ARRANGE_MAIN;
		pos  = BOX2UI_ARRANGE_MAIN_CHANGE;
		next = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		if( syswk->quick_mode != 0 ){
			pos = syswk->quick_get;
			BOX2MAIN_PokeInfoPut( syswk, pos );
		}
	}

	BOX2UI_CursorMoveChange( syswk, id, pos );

	BOX2BMP_ButtonPutTemochi( syswk );
	BOX2BMP_ButtonPutIdou( syswk );
	BOX2BMP_ButtonPutModoru( syswk );

	BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );

	return VFuncSet( syswk, BOX2MAIN_VFuncUnderButtonMove, next );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�e�[�}�ύX���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxThemaChgMain( BOX2_SYS_WORK * syswk )
{
	switch( CURSORMOVE_MainCont( syswk->app->cmwk ) ){
	case BOX2UI_BOXTHEMA_CHG_TRAY1:	// 00: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BoxThemaTrayIconSel( syswk, 0 );

	case BOX2UI_BOXTHEMA_CHG_TRAY2:	// 01: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BoxThemaTrayIconSel( syswk, 1 );

	case BOX2UI_BOXTHEMA_CHG_TRAY3:	// 02: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BoxThemaTrayIconSel( syswk, 2 );

	case BOX2UI_BOXTHEMA_CHG_TRAY4:	// 03: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BoxThemaTrayIconSel( syswk, 3 );

	case BOX2UI_BOXTHEMA_CHG_TRAY5:	// 04: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BoxThemaTrayIconSel( syswk, 4 );

	case BOX2UI_BOXTHEMA_CHG_TRAY6:	// 05: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BoxThemaTrayIconSel( syswk, 5 );


	case BOX2UI_BOXTHEMA_CHG_LEFT:		// 06: �g���C���i���j
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		return BoxThemaTrayIconChg( syswk, -1 );

	case BOX2UI_BOXTHEMA_CHG_RIGHT:	// 07: �g���C���i�E�j
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		return BoxThemaTrayIconChg( syswk, 1 );

	case BOX2UI_BOXTHEMA_CHG_MOVE:		// 08: �{�b�N�X�����肩����
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
		if( syswk->tray == syswk->box_mv_pos ){
//			Snd_SePlay( SE_BOX2_DISABLE_BUTTON );
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_BOXMV_MENU, BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_CHG_MAIN );
		}
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_BOXMV_MENU, BOX2SEQ_MAINSEQ_BOXTHEMA_TRAYCHG_BUTTON );

	case BOX2UI_BOXTHEMA_CHG_WP:		// 09: ���ׂ���
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_BUTTON );

	case BOX2UI_BOXTHEMA_CHG_NANE:		// 10: �Ȃ܂�
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_BOXTHEMA_NAME_BUTTON );

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		if( syswk->app->vfunk.freq != NULL ){
//			Snd_SePlay( SE_BOX2_MOVE_CURSOR );
			return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_CHG_CURMOVE_RET );
		}
		break;

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		break;

	case BOX2UI_BOXTHEMA_CHG_CANCEL:	// 11: ��߂�
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	case CURSORMOVE_CANCEL:				// �L�����Z��
//		Snd_SePlay( SE_BOX2_CANCEL );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_RET_BTN, BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_EXIT_OUT );
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�e�[�}�ύX�J�n�Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxThemaButtonOut( BOX2_SYS_WORK * syswk )
{
	if( syswk->dat->callMode == BOX_MODE_TURETEIKU ){
		if( BOX2BGWFRM_CloseButtonPutCheck(syswk->app->wfrm) == FALSE ){
			BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );
		}
	}else{
		BOX2BGWFRM_TemochiButtonOutSet( syswk->app->wfrm );
		BOX2BGWFRM_IdouButtonOutSet( syswk->app->wfrm );
		BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncUnderButtonMove, BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_TRAY_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�ړ��t���[���C���Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxThemaTrayIn( BOX2_SYS_WORK * syswk )
{
	syswk->box_mv_pos = syswk->tray;

	return BoxMoveFrmInSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_EXIT_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u��߂�v�t���[���C���Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxThemaExitIn( BOX2_SYS_WORK * syswk )
{
	return BoxMoveExitButtonInSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_MENU_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�e�[�}�ύX���j���[�C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxThemaMenuIn( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_BoxThemaMenuPrint( syswk );
	BOX2BGWFRM_BoxThemaMenuInSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncBoxThemaMenuMove, BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_MSG_SET );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�e�[�}�ύX���b�Z�[�W�Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxThemaMsgSet( BOX2_SYS_WORK * syswk )
{
	BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_BOXTHEMA_CHG, syswk->cur_rcv_pos );

	if( syswk->cur_rcv_pos >= BOX2UI_BOXTHEMA_CHG_TRAY1 &&
		syswk->cur_rcv_pos <= BOX2UI_BOXTHEMA_CHG_TRAY6 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}

	BOX2BMP_BoxThemaMsgPut( syswk, BOX2BMP_MSGID_THEMA_INIT, BOX2BMPWIN_ID_MSG3 );
	return BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_CHG_MAIN;
}


//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u��߂�v�t���[���A�E�g�Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxThemaExitOut( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG3 );
	return BoxMoveExitButtonOutSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_MENU_OUT );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�e�[�}�ύX���j���[�A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxThemaMenuOut( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_BoxThemaMenuOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncBoxThemaMenuMove, BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_TRAY_OUT );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�ړ��t���[���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxThemaTrayOut( BOX2_SYS_WORK * syswk )
{
	return BoxMoveFrmOutSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_BUTTON_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�e�[�}�ύX�I�����|�P�����I����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxThemaButtonIn( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	BOX2BMP_ButtonPutModoru( syswk );
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );

	if( syswk->dat->callMode == BOX_MODE_TURETEIKU ){
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_PTIN_MAIN, BOX2UI_PTIN_MAIN_NAME );
		return VFuncSet( syswk, BOX2MAIN_VFuncExitButtonMove, BOX2SEQ_MAINSEQ_PARTYIN_MAIN_RCV );
	}

	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ITEM_A_MAIN, BOX2UI_ITEM_A_MAIN_NAME );
	}else{
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MAIN, BOX2UI_ARRANGE_MAIN_NAME );
	}

	BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );

	return VFuncSet( syswk, BOX2MAIN_VFuncUnderButtonMove, BOX2SEQ_MAINSEQ_BOXARRANGE_POKE_MENU_RCV );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�ǎ��t���[���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeWallPaperFrmIn( BOX2_SYS_WORK * syswk )
{
	syswk->app->wallpaper_pos = BOX2MAIN_GetWallPaperNumber( syswk, syswk->box_mv_pos );

	BOX2OBJ_WallPaperChgObjInit( syswk );
	BOX2OBJ_WallPaperCursorSet( syswk );
	BOX2OBJ_WallPaperNameSet( syswk );
	BOX2BMP_WallPaperNameWrite( syswk );

	BOX2BGWFRM_BoxThemaMenuOutSet( syswk->app->wfrm );
	BOX2BGWFRM_WallPaperFrameInSet( syswk->app->wfrm );

	return VFuncSet( syswk, BOX2MAIN_VFuncWallPaperFrmIn, BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_MENU_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�ǎ��ύX���j���[�C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeWallPaperMenuIn( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_WallPaperChgMenuPrint( syswk );
	BOX2BGWFRM_BoxMoveMenuInSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveMenuIn, BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_START );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�ǎ��I���J�n
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeWallPaperChgStart( BOX2_SYS_WORK * syswk )
{
	syswk->app->old_cur_pos = syswk->app->wallpaper_pos % BOX2OBJ_WPICON_MAX;
	BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_WALLPAPER_CHG, syswk->app->old_cur_pos );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );

	BOX2BMP_BoxThemaMsgPut( syswk, BOX2BMP_MSGID_THEMA_WALL, BOX2BMPWIN_ID_MSG3 );
	return BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�ǎ��I�����C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeWallPaperChgMain( BOX2_SYS_WORK * syswk )
{
	switch( CURSORMOVE_MainCont( syswk->app->cmwk ) ){
	case BOX2UI_WALLPAPER_CHG_ICON1:		// 00: �ǎ��A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		WallPaperPosSet( syswk, 0 );
		return BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_MAIN;

	case BOX2UI_WALLPAPER_CHG_ICON2:		// 01: �ǎ��A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		WallPaperPosSet( syswk, 1 );
		return BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_MAIN;

	case BOX2UI_WALLPAPER_CHG_ICON3:		// 02: �ǎ��A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		WallPaperPosSet( syswk, 2 );
		return BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_MAIN;

	case BOX2UI_WALLPAPER_CHG_ICON4:		// 03: �ǎ��A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		WallPaperPosSet( syswk, 3 );
		return BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_MAIN;

	case BOX2UI_WALLPAPER_CHG_LEFT:		// 04: �g���C���i���j
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		WallPaperPageChange( syswk, -1 );
		return BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_MAIN;

	case BOX2UI_WALLPAPER_CHG_RIGHT:		// 05: �g���C���i�E�j
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		WallPaperPageChange( syswk, 1 );
		return BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_MAIN;

	case BOX2UI_WALLPAPER_CHG_ENTER:		// 06: �ύX
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
		return WallPaperChange( syswk );

	case CURSORMOVE_CURSOR_MOVE:			// �ړ�
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		if( syswk->app->vfunk.freq != NULL ){
			return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_WALLPAPER_CURMOVE_RET );
		}
		break;

	case BOX2UI_WALLPAPER_CHG_CANCEL:		// 07: �L�����Z��
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	case CURSORMOVE_CANCEL:
//		Snd_SePlay( SE_BOX2_CANCEL );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_RET_BTN, BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_CANCEL_BUTTON );
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�ǎ����j���[�A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeWallPaperMenuOut( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_BoxMoveMenuOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveMenuOut, BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_FRM_OUT );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�ǎ��t���[���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeWallPaperFrmOut( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_WallPaperFrameOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncWallPaperFrmOut, BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�ǎ��ύX�I��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeWallPaperChgEnd( BOX2_SYS_WORK * syswk )
{
	return BoxMoveFrmInSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_MENU_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�ǎ��ύX
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeWallPaperChange( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_WallPaperChange( syswk, syswk->app->wallpaper_pos );
/*
	// �ǉ��ǎ��̓v���`�i�ȍ~�̔ԍ����g�p���Ă���
	if( syswk->app->wallpaper_pos < BOX_NORMAL_WALLPAPER_MAX ){
		BOXDAT_SetWallPaperNumber( syswk->dat->sv_box, syswk->tray, syswk->app->wallpaper_pos );
	}else{
		BOXDAT_SetWallPaperNumber(
			syswk->dat->sv_box, syswk->tray,
			syswk->app->wallpaper_pos - BOX_NORMAL_WALLPAPER_MAX + BOX_TOTAL_WALLPAPER_MAX_PL );
	}
*/
	BOXDAT_SetWallPaperNumber( syswk->dat->sv_box, syswk->tray, syswk->app->wallpaper_pos );
	BOX2OBJ_WallPaperCursorSet( syswk );
	BOX2OBJ_TrayIconCgxMake( syswk, syswk->tray );

	return BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_WHITE_IN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�ǎ��ύX�z���C�g�A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeWallPaperWhiteOut( BOX2_SYS_WORK * syswk )
{
	PaletteWorkSet_VramCopy( syswk->app->pfd, FADE_MAIN_BG, 0, FADE_PAL_ALL_SIZE );
	PaletteFadeReq( syswk->app->pfd, PF_BIT_MAIN_BG, 0xc000, 0, 0, 16, 0x7fff, GFUser_VIntr_GetTCBSYS() );
	syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHANGE;
	return BOX2SEQ_MAINSEQ_PALETTE_FADE;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�ǎ��ύX�z���C�g�C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeWallPaperWhiteIn( BOX2_SYS_WORK * syswk )
{
	PaletteFadeReq( syswk->app->pfd, PF_BIT_MAIN_BG, 0xc000, 0, 16, 0, 0x7fff, GFUser_VIntr_GetTCBSYS() );
	syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_MAIN;
	return BOX2SEQ_MAINSEQ_PALETTE_FADE;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F���C�������֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxArrangePokeMenuRcv( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );

	// �����
	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN;
	}
	return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���I�����C�������֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxArrangePartyMenuRcv( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );

	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
}


//============================================================================================
//	�|�P��������Ă���
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������Ă����v���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	ret = BOX2UI_HitTrayPokeTrg();
	if( ret != GFL_UI_TP_HIT_NONE ){
//��[GS_CONVERT_TAG]
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
//��[GS_CONVERT_TAG]
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
				BOX2BMP_MenuStrPrint( syswk, PartyInMenuStrTbl, 4 );
			}
//			Snd_SePlay( SE_BOX2_POKE_CATCH );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
			return TrayPokeGetDrop( syswk, ret );
		}else{
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
				u8	pos = syswk->get_pos;
				BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
				CURSORMOVE_PosSet( syswk->app->cmwk, pos );
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
				BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
				BOX2OBJ_PokeCursorVanish( syswk, FALSE );
				return VFuncSet( syswk, BOX2MAIN_VFuncMenuCloseKey, BOX2SEQ_MAINSEQ_PARTYIN_MENU_CLOSE_END );
			}else{
				CURSORMOVE_PosSet( syswk->app->cmwk, ret );
				CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
				BOX2MAIN_PokeInfoOff( syswk );
				return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
			}
		}
	}

	ret = BOX2UI_PartyInMain( syswk );

	switch( ret ){
	case BOX2UI_PTIN_MAIN_TSURETEIKU:	//�u��Ă����v
		if( PokeParty_GetPokeCount( syswk->dat->pokeparty ) != TEMOTI_POKEMAX ){
//			Snd_SePlay( SE_BOX2_DECIDE );
		}else{
//			Snd_SePlay( SE_BOX2_WARNING );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU2, BOX2SEQ_MAINSEQ_PARTYIN_BUTTON );

	case BOX2UI_PTIN_MAIN_STATUS:		//�u�悤�����݂�v
//		Snd_SePlay( SE_BOX2_DECIDE );
		syswk->cur_rcv_pos = BOX2UI_PTIN_MAIN_STATUS;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU3, BOX2SEQ_MAINSEQ_STATUS_BUTTON );

	case BOX2UI_PTIN_MAIN_MARKING:		//�u�}�[�L���O�v
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG3 );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, BOX2SEQ_MAINSEQ_MARKING_BUTTON );

	case BOX2UI_PTIN_MAIN_FREE:		//�u�ɂ����v
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_FREE_BUTTON );

	case BOX2UI_PTIN_MAIN_CLOSE:		//�u�Ƃ���v
		{
			u8	pos = syswk->get_pos;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
//		Snd_SePlay( SE_BOX2_CANCEL );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_CLOSE_BTN, BOX2SEQ_MAINSEQ_PARTYIN_CLOSE_BUTTON );

	case BOX2UI_PTIN_MAIN_NAME:		// �{�b�N�X��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG3 );
		return BoxThemaChgStart( syswk );

	case BOX2UI_PTIN_MAIN_LEFT:		// �g���C�؂�ւ����E��
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_PTIN_MAIN_NAME );
		BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );
		return TrayLeftScrollSet( syswk, BOX2SEQ_MAINSEQ_PARTYIN_MAIN );

	case BOX2UI_PTIN_MAIN_RIGHT:		// �g���C�؂�ւ����E�E
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_PTIN_MAIN_NAME );
		BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );
		return TrayRightScrollSet( syswk, BOX2SEQ_MAINSEQ_PARTYIN_MAIN );

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
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_PARTYIN_MAIN_CURMOVE_RET );

	case BOX2UI_PTIN_MAIN_RETURN:		//�u���ǂ�v
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeInfoOff( syswk );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_RET_BTN, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case CURSORMOVE_CANCEL:				// �L�����Z��
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_RET_BTN, BOX2SEQ_MAINSEQ_BOXEND_CANCEL_INIT );

	case CURSORMOVE_NONE:			// ����Ȃ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos == BOX2UI_PTIN_MAIN_NAME ){
				if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT ){
//��[GS_CONVERT_TAG]
//					Snd_SePlay( SE_BOX2_CHANGE_TRAY );
					BOX2MAIN_PokeSelectOff( syswk );
					return TrayLeftScrollSet( syswk, BOX2SEQ_MAINSEQ_PARTYIN_MAIN );
				}
				if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT ){
//��[GS_CONVERT_TAG]
//					Snd_SePlay( SE_BOX2_CHANGE_TRAY );
					BOX2MAIN_PokeSelectOff( syswk );
					return TrayRightScrollSet( syswk, BOX2SEQ_MAINSEQ_PARTYIN_MAIN );
				}
			}
		}
		break;

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
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
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		break;

	default:
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_DECIDE );
			BOX2BMP_MenuStrPrint( syswk, PartyInMenuStrTbl, 4 );
			BOX2MAIN_PokeInfoPut( syswk, ret );

			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_PTIN_MAIN_TSURETEIKU, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_PTIN_MAIN_TSURETEIKU );
			return TrayPokeGetKey( syswk, ret );
		}
	}

	return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������Ă����v���C���֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInMainRcv( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );
	return MainSeq_PartyInPokeMenuRcv( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������Ă����v�莝���|�P�����t���[���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInPartyFrmIn( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_PartyPokeIconFrmInSetRight( syswk );
	BOX2BGWFRM_PartyPokeFrameInitPutRight( syswk->app->wfrm );
	BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;
	return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_PARTYIN_POKEMOVE_SET );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������Ă����v�|�P�����ړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInPokeMoveSet( BOX2_SYS_WORK * syswk )
{
	PokeMoveWorkAlloc( syswk );
	return VFuncSet( syswk, BOX2MAIN_VFuncPartyInPokeMove, BOX2SEQ_MAINSEQ_PARTYIN_PARTY_FRM_OUT );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������Ă����v�莝���|�P�����t���[���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInPartyFrmOut( BOX2_SYS_WORK * syswk )
{
	const CURSORMOVE_DATA * pw;
	u8	x, y;

//	Snd_SePlay( SE_BOX2_POKE_PUT );

	CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );

	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, syswk->get_pos );
	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );

	syswk->get_pos = BOX2MAIN_GETPOS_NONE;

	BOX2MAIN_PokeDataMove( syswk );
	PokeMoveWorkFree( syswk );

	BOX2MAIN_PokeInfoOff( syswk );

	BOX2BGWFRM_PartyPokeFrameOutSet( syswk->app->wfrm );

	return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_PARTYIN_RET_FRM_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������Ă����v�߂�{�^���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInRetFrmIn( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncExitButtonMove, BOX2SEQ_MAINSEQ_PARTYIN_MAIN_RCV );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F��Ă����|�P�������h���b�v
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInPokeChgEnd( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_POKEMOVE_WORK * work;
	u32	pos;

	work = syswk->app->vfunk.work;
	pos  = work->get_pos;

//	Snd_SePlay( SE_BOX2_POKE_PUT );

	BOX2MAIN_PokeDataMove( syswk );
	PokeMoveWorkFree( syswk );

	BOX2BMP_SysWinVanishEz( syswk->app, BOX2BMPWIN_ID_MSG4 );

	if( syswk->get_pos == BOX2MAIN_GETPOS_NONE ){
		BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );
	}else{
		BOX2BMP_PokePartyInMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG3 );
		pos = BOX2UI_PTIN_MAIN_TSURETEIKU;
	}
	CURSORMOVE_PosSet( syswk->app->cmwk, pos );
	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );

	return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������Ă����v���C���֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInPokeMenuRcv( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );

	return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
}


//============================================================================================
//	�|�P������a����
//============================================================================================

// �f�o�b�O�I�[�g�v���C
//#define	BTS_090509_AUTO_PLAY
#ifdef	BTS_090509_AUTO_PLAY
static u32 test_rect_flg = 0;
static u32 test_rect_pos = 0;
static u32 test_rect_cnt = 0;
#endif

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������������v�莝���I�����C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

#ifdef	BTS_090509_AUTO_PLAY
	if( test_rect_flg != 0 ){
		if( test_rect_cnt != 0 ){
			test_rect_cnt--;
		}else{
			test_rect_cnt = GFUser_GetPublicRand(GFUSER_RAND_PAST_MAX)() & 0x3f;
//��[GS_CONVERT_TAG]
			test_rect_pos++;
			if( test_rect_pos >= 6 ){
				test_rect_pos = 0;
			}
			ret = BOX2OBJ_POKEICON_TRAY_MAX + test_rect_pos;
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
//��[GS_CONVERT_TAG]
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
				BOX2BMP_MenuStrPrint( syswk, PartyOutMenuStrTbl, 4 );
			}
//			Snd_SePlay( SE_BOX2_POKE_CATCH );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
			return PartyPokeGetDrop( syswk, ret );
		}
	}
#endif

	ret = BOX2UI_HitCheckTrgPartyPokeLeft();
	if( ret != GFL_UI_TP_HIT_NONE ){
//��[GS_CONVERT_TAG]
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
//��[GS_CONVERT_TAG]
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
				BOX2BMP_MenuStrPrint( syswk, PartyOutMenuStrTbl, 4 );
			}
//			Snd_SePlay( SE_BOX2_POKE_CATCH );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
#ifdef	BTS_090509_AUTO_PLAY
			OS_Printf( "\n�������������@�f�o�b�O�I�[�g�v���C�J�n ver.090516-�@������������\n" );
			test_rect_flg = 1;
			test_rect_cnt = GFUser_GetPublicRand(GFUSER_RAND_PAST_MAX)() & 0x3f;
//��[GS_CONVERT_TAG]
#endif
			return PartyPokeGetDrop( syswk, ret );
		}else{
			if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
				u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
				BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
				CURSORMOVE_PosSet( syswk->app->cmwk, pos );
				BOX2BMP_ButtonPutModoru( syswk );
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
				BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
				BOX2OBJ_PokeCursorVanish( syswk, FALSE );
				return VFuncSet( syswk, BOX2MAIN_VFuncMenuCloseKey, BOX2SEQ_MAINSEQ_PARTYOUT_MENU_CLOSE_END );
			}else{
				CURSORMOVE_PosSet( syswk->app->cmwk, ret - BOX2OBJ_POKEICON_TRAY_MAX );
				CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
				BOX2MAIN_PokeInfoOff( syswk );
				return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
			}
		}
	}

	ret = BOX2UI_PartyOutMain( syswk );

#ifdef	BTS_5436_TEST
	if( test_5436_flg == 1 ){
		ret = BOX2UI_PTOUT_MAIN_STATUS;
	}
#endif	// BTS_5436_TEST

	switch( ret ){
	case BOX2UI_PTOUT_MAIN_AZUKERU:	//�u��������v
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU2, BOX2SEQ_MAINSEQ_PARTYOUT_BUTTON );

	case BOX2UI_PTOUT_MAIN_STATUS:		//�u�悤�����݂�v
//		Snd_SePlay( SE_BOX2_DECIDE );
		syswk->cur_rcv_pos = BOX2UI_PTOUT_MAIN_STATUS;
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU3, BOX2SEQ_MAINSEQ_STATUS_BUTTON );

	case BOX2UI_PTOUT_MAIN_MARKING:	//�u�}�[�L���O�v
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG3 );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU4, BOX2SEQ_MAINSEQ_MARKING_BUTTON );

	case BOX2UI_PTOUT_MAIN_FREE:		//�u�ɂ����v
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_FREE_BUTTON );

	case BOX2UI_PTOUT_MAIN_CLOSE:		//�u�Ƃ���v
		{
			u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
//		Snd_SePlay( SE_BOX2_CANCEL );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_CLOSE_BTN, BOX2SEQ_MAINSEQ_PARTYOUT_CLOSE_BUTTON );

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos <= BOX2UI_PTOUT_MAIN_RETURN ){
				BOX2MAIN_PokeInfoPut( syswk, pos+BOX2OBJ_POKEICON_TRAY_MAX );
			}
		}
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;

	case CURSORMOVE_CURSOR_MOVE:	// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos <= BOX2UI_PTOUT_MAIN_RETURN ){
				BOX2MAIN_PokeInfoPut( syswk, pos+BOX2OBJ_POKEICON_TRAY_MAX );
			}
		}
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_PARTYOUT_MAIN_CURMOVE_RET );

	case BOX2UI_PTOUT_MAIN_RETURN:		//�u���ǂ�v
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeInfoOff( syswk );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_RET_BTN, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case CURSORMOVE_CANCEL:				// �L�����Z��
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_RET_BTN, BOX2SEQ_MAINSEQ_BOXEND_CANCEL_INIT );

	case CURSORMOVE_NONE:			// ����Ȃ�
		break;

	default:
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_DECIDE );
			BOX2BMP_MenuStrPrint( syswk, PartyOutMenuStrTbl, 4 );
			BOX2MAIN_PokeInfoPut( syswk, ret );

			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_PTOUT_MAIN_AZUKERU, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_PTOUT_MAIN_AZUKERU );
			return PartyPokeGetKey( syswk, ret );
		}
	}

	return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a�����邩���`�F�b�N
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutStartCheck( BOX2_SYS_WORK * syswk )
{
	POKEMON_PARAM * pp;
	u32	pos;
	u16	item;

	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;

	// �莝���P
	if( BOX2MAIN_BattlePokeCheck( syswk, pos ) == FALSE ){
//		Snd_SePlay( SE_BOX2_WARNING );
		BOX2BMP_PokeFreeMsgPut( syswk, BOX2BMP_MSGID_POKEFREE_ONE, BOX2BMPWIN_ID_MSG1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_PARTYOUT_START_ERROR;
		return BOX2SEQ_MAINSEQ_TRGWAIT;
	}
	// ���[��
	pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, pos );
	item = PP_Get( pp, ID_PARA_item, NULL );
	if( ITEM_CheckMail( item ) == TRUE ){
//		Snd_SePlay( SE_BOX2_WARNING );
		BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_MAIL, BOX2BMPWIN_ID_MSG1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_PARTYOUT_START_ERROR;
		return BOX2SEQ_MAINSEQ_TRGWAIT;
	}
/*	�J�X�^���{�[���͂Ȃ��n�Y
	// �J�v�Z��
	if( PP_Get( pp, ID_PARA_cb_id, NULL ) != 0 ){
//		Snd_SePlay( SE_BOX2_WARNING );
		BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_CAPSULE, BOX2BMPWIN_ID_MSG1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_PARTYOUT_START_ERROR;
		return BOX2SEQ_MAINSEQ_TRGWAIT;
	}
*/
	syswk->box_mv_pos = syswk->tray;

	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, TRUE );

	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	return VFuncSet( syswk, BOX2MAIN_VFuncPokeOutBoxFrmIn, BOX2SEQ_MAINSEQ_PARTYOUT_START_RET_FRM_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a����G���[
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutStartError( BOX2_SYS_WORK * syswk )
{
	return VFuncSet( syswk, BOX2MAIN_VFuncPokeFreeMenuMove, BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a����ꏊ�I���u��߂�v�{�^���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutStartRetFrmIn( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_PokeOutBoxObjInit( syswk );
	BOX2OBJ_BoxMoveCursorSet( syswk );
	BOX2OBJ_BoxMoveNameSet( syswk );
	BOX2BMP_BoxMoveNameNumWrite( syswk );
	return BoxMoveExitButtonInSet( syswk, BOX2SEQ_MAINSEQ_PARTYOUT_MENUFRM_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a����ꏊ�I�����j���[�C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutMenuFrmIn( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_PartyOutMenuPrint( syswk );
	BOX2BGWFRM_BoxMoveMenuInSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveMenuIn, BOX2SEQ_MAINSEQ_PARTYOUT_BOXSELECT_START );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a����ꏊ�I���J�n
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutBoxSelectStart( BOX2_SYS_WORK * syswk )
{
	syswk->app->old_cur_pos = syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;
	BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_PTOUT_BOXSEL, syswk->app->old_cur_pos );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );

	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_INIT, BOX2BMPWIN_ID_MSG3 );
	return BOX2SEQ_MAINSEQ_PARTYOUT_BOXSELECT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a����ꏊ�I�����C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutBoxSelectMain( BOX2_SYS_WORK * syswk )
{
	switch( CURSORMOVE_MainCont( syswk->app->cmwk ) ){
	case BOX2UI_PTOUT_BOXSEL_TRAY1:	// �g���C�A�C�R��
		return PokeOutTraySelect( syswk, 0 );

	case BOX2UI_PTOUT_BOXSEL_TRAY2:	// �g���C�A�C�R��
		return PokeOutTraySelect( syswk, 1 );

	case BOX2UI_PTOUT_BOXSEL_TRAY3:	// �g���C�A�C�R��
		return PokeOutTraySelect( syswk, 2 );

	case BOX2UI_PTOUT_BOXSEL_TRAY4:	// �g���C�A�C�R��
		return PokeOutTraySelect( syswk, 3 );

	case BOX2UI_PTOUT_BOXSEL_TRAY5:	// �g���C�A�C�R��
		return PokeOutTraySelect( syswk, 4 );

	case BOX2UI_PTOUT_BOXSEL_TRAY6:	// �g���C�A�C�R��
		return PokeOutTraySelect( syswk, 5 );

	case BOX2UI_PTOUT_BOXSEL_LEFT:		// �g���C���i���j
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		return PokeOutTrayIconChg( syswk, -1 );

	case BOX2UI_PTOUT_BOXSEL_RIGHT:	// �g���C���i�E�j
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		return PokeOutTrayIconChg( syswk, 1 );

	case BOX2UI_PTOUT_BOXSEL_ENTER:	//�u�|�P��������������v
		return PartyOutEnter( syswk );

	case CURSORMOVE_CURSOR_MOVE:	// �ړ�
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		if( syswk->app->vfunk.freq != NULL ){
			return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_PARTYOUT_BOXSEL_CURMOVE_RET );
		}
		break;

	case BOX2UI_PTOUT_BOXSEL_CANCEL:	//�u��߂�v
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );

	case CURSORMOVE_CANCEL:				// �L�����Z��
//		Snd_SePlay( SE_BOX2_CANCEL );
		return PartyOutEnd( syswk, FALSE );
	}

	return BOX2SEQ_MAINSEQ_PARTYOUT_BOXSELECT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a����ꏊ�I�����j���[�A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutBoxMenuFrmOut( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	BOX2BGWFRM_BoxMoveMenuOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveMenuOut, BOX2SEQ_MAINSEQ_PARTYOUT_END_RET_FRM_OUT );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a����ꏊ�I���u��߂�v�{�^���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutEndRetFrmOut( BOX2_SYS_WORK * syswk )
{
	return BoxMoveExitButtonOutSet( syswk, BOX2SEQ_MAINSEQ_PARTYOUT_BOXMOVE_FRM_OUT );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a����ꏊ�I���g���C�t���[���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutBoxMoveFrmOut( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_PokeOutBoxObjVanish( syswk );
	return VFuncSet( syswk, BOX2MAIN_VFuncPokeOutBoxFrmOut, BOX2SEQ_MAINSEQ_PARTYOUT_END_RET_FRM_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a���郁�C�������֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutEndRetFrmIn( BOX2_SYS_WORK * syswk )
{
	BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_PTOUT_MAIN, BOX2UI_PTOUT_MAIN_AZUKERU );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

	BOX2OBJ_PokeCursorAdd( syswk );
	BOX2OBJ_PokeCursorVanish( syswk, TRUE );

	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );

	return BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a����ꏊ����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutEnterButton( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	BOX2BGWFRM_BoxMoveMenuOutSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncPartyOutButtonMove, BOX2SEQ_MAINSEQ_PARTYOUT_CHECK );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a�����邩���`�F�b�N
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutCheck( BOX2_SYS_WORK * syswk )
{

	if( BOXDAT_GetPokeExistCount( syswk->dat->sv_box, syswk->box_mv_pos ) == BOX_MAX_POS ){
		BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYOUT_BOXMAX, BOX2BMPWIN_ID_MSG1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_PARTYOUT_START_RET_FRM_IN;
		return BOX2SEQ_MAINSEQ_TRGWAIT;
	}

	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );

	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG3 );

	BOX2OBJ_PokeOutBoxObjVanish( syswk );
	return VFuncSet( syswk, BOX2MAIN_VFuncPokeOutBoxFrmOut, BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_PARTYFRM_RIGHT );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���|�P�����t���[�����E�ֈړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutEnterPartyFrmRight( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_AzukeruPartyPokeFrameRightMoveSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrmRight, BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_TRAY_SCROLL );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a����g���C�փX�N���[��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutEnterTrayScroll( BOX2_SYS_WORK * syswk )
{
	void * func;
	u32	tmp;

	tmp = syswk->tray;
	syswk->tray = syswk->box_mv_pos;

	if( tmp == syswk->box_mv_pos ){
		return BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_POKE_MOVE;
	}else if( tmp > syswk->box_mv_pos ){
		BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
		BOX2MAIN_WallPaperSet(
			syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), BOX2MAIN_TRAY_SCROLL_L );

		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
		func = BOX2MAIN_VFuncTrayScrollLeft;
	}else{
		BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
		BOX2MAIN_WallPaperSet(
			syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), BOX2MAIN_TRAY_SCROLL_R );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
		func = BOX2MAIN_VFuncTrayScrollRight;
	}
	return VFuncSet( syswk, func, BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_POKE_MOVE );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�|�P�����ړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutEnterPokeMove( BOX2_SYS_WORK * syswk )
{
	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;

	PokeMoveWorkAlloc( syswk );
	return VFuncSet( syswk, BOX2MAIN_VFuncPartyOutPokeMove, BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_PARTYFRM_LEFT );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���|�P�����t���[�������ֈړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutEnterPartyFrmLeft( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_POKEMOVE_WORK * work = syswk->app->vfunk.work;

//	Snd_SePlay( SE_BOX2_POKE_PUT );

	BOX2MAIN_PokeDataMove( syswk );

	BOX2MAIN_FormChangeSheimi( syswk, work->get_pos, work->put_pos );

	PokeMoveWorkFree( syswk );

	BOX2BGWFRM_AzukeruPartyPokeFrameLeftMoveSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrmLeft, BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_RET_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u���ǂ�v�{�^���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutEnterRetIn( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_ButtonPutModoru( syswk );
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncExitButtonMove, BOX2SEQ_MAINSEQ_PARTYOUT_COMP );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a���銮��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutComp( BOX2_SYS_WORK * syswk )
{
	BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_PTOUT_MAIN, BOX2UI_PTOUT_MAIN_POKE1 );

	BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );

	BOX2MAIN_PokeInfoPut( syswk, BOX2UI_PTOUT_MAIN_POKE1+BOX2OBJ_POKEICON_TRAY_MAX );

	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );

	return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�a����|�P�������h���b�v
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutPokeChgEnd( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_POKEMOVE_WORK * work;
	u32	pos;

	work = syswk->app->vfunk.work;
	pos  = work->get_pos;

//	Snd_SePlay( SE_BOX2_POKE_PUT );

	BOX2MAIN_PokeDataMove( syswk );
	BOX2MAIN_FormChangeSheimi( syswk, work->get_pos, work->put_pos );
	PokeMoveWorkFree( syswk );

	BOX2BMP_SysWinVanishEz( syswk->app, BOX2BMPWIN_ID_MSG4 );

	if( syswk->get_pos == BOX2MAIN_GETPOS_NONE ){
		BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );
		CURSORMOVE_PosSet( syswk->app->cmwk, 0 );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		BOX2MAIN_PokeInfoPut( syswk, BOX2OBJ_POKEICON_TRAY_MAX );
	}else{
		BOX2BMP_PokePartyInMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG3 );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_PTOUT_MAIN_AZUKERU );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	}

	return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�{�b�N�X������v���C�������֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeMainCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�{�b�N�X������v�|�P�����ړ����C�������֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxPartyChgMainCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�{�b�N�X������v�莝���|�P�������C�������֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyMainCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������������v���C�������֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutMainCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������������v���C�������֖߂�i���b�Z�[�W�\������j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutMenuCloseEnd( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );
	return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������������v�{�b�N�X�I�������֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutBoxSelCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_PARTYOUT_BOXSELECT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�}�[�L���O�����֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MarkingCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_ARRANGE_MARKING;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�e�[�}�ύX�����֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaChgCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�ǎ��ύX�����֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_WallPaperChgCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������Ă����v���C�������֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInMainCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������Ă����v���C�������֖߂�i���b�Z�[�W����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInMenuCloseEnd( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );
	return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;
}


//============================================================================================
//	�����
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F��������C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	ret = BOX2UI_HitTrayPokeTrg();
	if( ret != GFL_UI_TP_HIT_NONE ){
//��[GS_CONVERT_TAG]
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
//��[GS_CONVERT_TAG]
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
			return TrayItemGet( syswk, ret );
		}else{
			if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
				u8	pos = syswk->get_pos;
				BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
				CURSORMOVE_PosSet( syswk->app->cmwk, pos );
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
				BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );
				BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
				BOX2OBJ_PokeCursorVanish( syswk, FALSE );
				if( syswk->app->get_item != ITEM_DUMMY_DATA ){
					BOX2BGWFRM_SubDispItemFrmOutSet( syswk->app->wfrm );
				}
				return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeMenuClose, BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN_CURMOVE_RET );
			}else{
				CURSORMOVE_PosSet( syswk->app->cmwk, ret );
				CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
				BOX2MAIN_PokeInfoOff( syswk );
				return BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN;
			}
		}
	}

	ret = BOX2UI_BoxItemArrangeMain( syswk );

	switch( ret ){
	case BOX2UI_ITEM_A_MAIN_NAME:		// 30: �{�b�N�X��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		return BoxThemaChgStart( syswk );

	case BOX2UI_ITEM_A_MAIN_LEFT:		// 31: �g���C�؂�ւ����E��
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_A_MAIN_NAME );
		return TrayLeftScrollSet( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN );

	case BOX2UI_ITEM_A_MAIN_RIGHT:		// 32: �g���C�؂�ւ����E�E
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_A_MAIN_NAME );
		return TrayRightScrollSet( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN );

	case BOX2UI_ITEM_A_MAIN_PARTY:		// 33: �莝���|�P����
//		Snd_SePlay( SE_BOX2_OPEN_PARTY_TRAY );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_POKE_BTN, BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_BUTTON );

	case BOX2UI_ITEM_A_MAIN_CHANGE:	// 34: ����������
//		Snd_SePlay( SE_BOX2_OPEN_PARTY_TRAY );
		return YStatusButtonAnmSet( syswk, BOX2MAIN_WINFRM_MV_BTN, BOX2SEQ_MAINSEQ_ITEMARRANGE_MOVE_BUTTON );

	case BOX2UI_ITEM_A_MAIN_MENU1:		// 36: �o�b�O�� or ��������
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_ITEMARRANGE_BAG_BUTTON );

	case BOX2UI_ITEM_A_MAIN_CLOSE:		// 37: �Ƃ���
//		Snd_SePlay( SE_BOX2_CANCEL );
		{
			u8	pos = syswk->get_pos;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_CLOSE_BTN, BOX2SEQ_MAINSEQ_ITEMARRANGE_CLOSE_BUTTON );

	case CURSORMOVE_NONE:				// ����Ȃ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos == BOX2UI_ARRANGE_MAIN_NAME ){
				if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT ){
//��[GS_CONVERT_TAG]
//					Snd_SePlay( SE_BOX2_CHANGE_TRAY );
					BOX2MAIN_PokeSelectOff( syswk );
					return TrayLeftScrollSet( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN );
				}
				if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT ){
//��[GS_CONVERT_TAG]
//					Snd_SePlay( SE_BOX2_CHANGE_TRAY );
					BOX2MAIN_PokeSelectOff( syswk );
					return TrayRightScrollSet( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN );
				}
			}
		}
		break;

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_ITEM_A_MAIN_NAME ){
				BOX2MAIN_PokeInfoPut( syswk, pos );
			}else{
				if( pos != BOX2UI_ITEM_A_MAIN_MENU1 &&
					pos != BOX2UI_ITEM_A_MAIN_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN_CURMOVE_RET );

	case BOX2UI_ITEM_A_MAIN_RETURN:	// 35: ���ǂ�
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeInfoOff( syswk );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_RET_BTN, BOX2SEQ_MAINSEQ_BOXEND_INIT );

	case CURSORMOVE_CANCEL:				// �L�����Z��
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_RET_BTN, BOX2SEQ_MAINSEQ_BOXEND_CANCEL_INIT );

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_ITEM_A_MAIN_NAME ){
				BOX2MAIN_PokeInfoPut( syswk, pos );
			}else{
				if( pos != BOX2UI_ITEM_A_MAIN_MENU1 &&
					pos != BOX2UI_ITEM_A_MAIN_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		break;

	default:
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_DECIDE );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_ITEM_A_MAIN_MENU1, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_A_MAIN_MENU1 );
			return TrayItemGetKey( syswk, ret );
		}
		break;
	}

	return BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�����������Ƃ̏���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeRetBagSet( BOX2_SYS_WORK * syswk )
{
	if( BOX2OBJ_VanishCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
		BOX2OBJ_PokeCursorVanish( syswk, FALSE );
		BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );
		BOX2BGWFRM_SubDispItemFrmOutSet( syswk->app->wfrm );
	}

	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );

	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
		BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
	}else{
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncPokeFreeMenuMove, BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�������̂�����v���C�������֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeMainCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F���������o�b�O�ɖ߂��邩���`�F�b�N
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeRetBagCheck( BOX2_SYS_WORK * syswk )
{
	if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
		// ���[��
		if( ITEM_CheckMail( syswk->app->get_item ) == TRUE ){
			const CURSORMOVE_DATA * pw;
			u16	pos;
			u8	x, y;
			pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
			pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, pos );
			x = pw->px;
			y = pw->py;
			BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );

//			Snd_SePlay( SE_BOX2_WARNING );
			BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_MAIL, BOX2BMPWIN_ID_MSG1 );
			syswk->next_seq = BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_SET;
			return BOX2SEQ_MAINSEQ_TRGWAIT;
		}
	}

	BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_RET_CHECK, BOX2BMPWIN_ID_MSG1 );
	return YesNoSet( syswk, YESNO_ID_ITEM_RET_BAG );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�o�b�O�ɖ߂��Ȃ��Ƃ��̃G���[����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeRetBagEnter( BOX2_SYS_WORK * syswk )
{
	if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == FALSE ){
		const CURSORMOVE_DATA * pw;
		u16	pos;
		u8	x, y;

		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			pos = syswk->get_pos;
		}else{
			pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
		}
		CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, pos );
		x = pw->px;
		y = pw->py;
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );

		BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_RET, BOX2BMPWIN_ID_MSG1 );
		BOX2BMP_ItemArrangeMenuStrPrint( syswk, 0 );
		syswk->app->get_item = ITEM_DUMMY_DATA;
		syswk->next_seq = BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_SET;
		return BOX2SEQ_MAINSEQ_TRGWAIT;
	}
	return BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_ENTER;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�o�b�O�ɖ߂��L�����Z������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeRetBagCancel( BOX2_SYS_WORK * syswk )
{
	const CURSORMOVE_DATA * pw;
	u16	pos;
	u8	x, y;

	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		pos = syswk->get_pos;
		BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
		BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
	}else{
		pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
	}
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );

	CURSORMOVE_PosSet( syswk->app->cmwk, pos );
	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, pos );
	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );

	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );
	BOX2BGWFRM_SubDispItemFrmOutSet( syswk->app->wfrm );

	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );

	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, TRUE );
	}else{
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncPokeFreeMenuMove, BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�o�b�O�֖߂��I��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeRetBagEnd( BOX2_SYS_WORK * syswk )
{
	if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_END;
	}
	BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );

	if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN;
	}
	return BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�^�}�S�͎��ĂȂ����b�Z�[�W�\��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeGetBagCheck( BOX2_SYS_WORK * syswk )
{
	const CURSORMOVE_DATA * pw;
	u16	pos;
	u8	x, y;

	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		pos = syswk->get_pos;
	}else{
		pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
	}
	CURSORMOVE_PosSet( syswk->app->cmwk, pos );
	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, pos );
	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );

//	Snd_SePlay( SE_BOX2_WARNING );
	BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_EGG, BOX2BMPWIN_ID_MSG1 );
	syswk->next_seq = BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_SET;
	return BOX2SEQ_MAINSEQ_TRGWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�����������b�Z�[�W�\��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeItemSetMsg( BOX2_SYS_WORK * syswk )
{
	const CURSORMOVE_DATA * pw;
	u16	pos;
	u8	x, y;

	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		pos = syswk->get_pos;
	}else{
		pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
	}
	CURSORMOVE_PosSet( syswk->app->cmwk, pos );
	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, pos );
	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );

	BOX2BMP_ItemSetMsgPut( syswk, syswk->subRet, BOX2BMPWIN_ID_MSG1 );
	syswk->next_seq = BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_SET;
	return BOX2SEQ_MAINSEQ_TRGWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�A�C�e���A�C�R���ړ���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeIconMoveRet( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_ITEMMOVE_WORK * mvwk;

	mvwk = syswk->app->vfunk.work;

	if( mvwk->put_pos == syswk->get_pos || syswk->app->get_item == ITEM_DUMMY_DATA ){
		ItemMoveWorkFree( syswk );
		if( syswk->app->get_item != ITEM_DUMMY_DATA ){
//			Snd_SePlay( SE_BOX2_POKE_PUT );
			BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );
		}
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_A_MAIN_MENU1 );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN;
	}

//	Snd_SePlay( SE_BOX2_POKE_PUT );

	BOX2OBJ_PokeIconBlendSet( syswk->app, mvwk->put_pos, FALSE );
	BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, TRUE );

	// �A�E�g���C������
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	// �A�C�e���A�C�R���k��
	BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );

	return BOX2SEQ_MAINSEQ_ITEMARRANGE_ICONMOVE_POKEADD;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�A�C�e�����Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeIconMovePokeAdd( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_ITEMMOVE_WORK * mvwk;
	POKEMON_PASO_PARAM * ppp;
	u16	item;

	if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_ICONMOVE_POKEADD;
	}

	BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );

	mvwk = syswk->app->vfunk.work;

	// �ړ���ɃA�C�e�����Z�b�g
	item = BOX2MAIN_PokeParaGet( syswk, mvwk->put_pos, ID_PARA_item, NULL );
	BOX2MAIN_PokeParaPut( syswk, mvwk->put_pos, ID_PARA_item, syswk->app->get_item );
	ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, mvwk->put_pos );
	if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
		BOX2OBJ_PokeIconChange(
			syswk, syswk->tray, mvwk->put_pos,
			syswk->app->pokeicon_id[mvwk->put_pos] );
	}
	BOX2MAIN_PokeInfoPut( syswk, mvwk->put_pos );

	// �ړ����ɃA�C�e�����Z�b�g
	syswk->app->get_item = item;
	BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, ID_PARA_item, syswk->app->get_item );
	ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );
	if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
		BOX2OBJ_PokeIconChange(
			syswk, syswk->tray, syswk->get_pos,
			syswk->app->pokeicon_id[syswk->get_pos] );
	}

	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		syswk->get_pos = mvwk->put_pos;
		ItemMoveWorkFree( syswk );
		BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
		BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );
		BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		return VFuncSet( syswk, BOX2MAIN_VFuncUnderButtonMove, BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN );
	}

//	Snd_SePlay( SE_BOX2_POKE_CATCH );

	BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
	BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 1 );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
	BOX2OBJ_ItemIconCursorAdd( syswk->app );
	return VFuncSet( syswk, BOX2MAIN_VFuncItemIconChange, BOX2SEQ_MAINSEQ_ITEMARRANGE_ICONMOVE_CHANGE );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�A�C�e���A�C�R����������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeIconMoveChange( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_ITEMMOVE_WORK * mvwk;

	mvwk = syswk->app->vfunk.work;

	BOX2OBJ_PokeIconBlendSet( syswk->app, mvwk->put_pos, FALSE );

//	Snd_SePlay( SE_BOX2_POKE_PUT );

	// �A�E�g���C������
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	// �A�C�e���A�C�R���k��
	BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );

	CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );

	syswk->get_pos = mvwk->put_pos;

	ItemMoveWorkFree( syswk );

	return BOX2SEQ_MAINSEQ_ITEMARRANGE_ICONMOVE_CHANGE_END;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�A�C�e�������I��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeIconMoveChangeEnd( BOX2_SYS_WORK * syswk )
{
	if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_ICONMOVE_CHANGE_END;
	}

	BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );

	BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncUnderButtonMove, BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�������̂�����v���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeBoxPartyChgMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	// �莝��
	ret = BOX2UI_HitCheckTrgPartyPokeRight();
	if( ret != GFL_UI_TP_HIT_NONE ){
//��[GS_CONVERT_TAG]
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
			return BoxPartyItemGet( syswk, ret );
		}else{
			BOX2MAIN_PokeInfoOff( syswk );
			CURSORMOVE_PosSet( syswk->app->cmwk, ret );
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
			if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == TRUE ){
				return BoxMoveButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN );
			}else if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
				return YStatusButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN );
			}
			return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
		}
	}

	// �{�b�N�X
	ret = BOX2UI_HitTrayPokeTrg();
	if( ret != GFL_UI_TP_HIT_NONE ){
//��[GS_CONVERT_TAG]
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
			return BoxPartyItemGet( syswk, ret );
		}else{
			BOX2MAIN_PokeInfoOff( syswk );
			CURSORMOVE_PosSet( syswk->app->cmwk, ret );
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
			if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == TRUE ){
				return BoxMoveButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN );
			}else if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
				return YStatusButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN );
			}
			return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
		}
	}

	// �x�X�e�[�^�X�{�^��
	if( BOX2UI_YStatusButtonCheck( syswk ) == TRUE ){
		ret = CURSORMOVE_PosGet( syswk->app->cmwk );
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_DECIDE );
			syswk->get_pos = ret;
			syswk->y_status_flg = 1;
			return YStatusButtonAnmSet( syswk, BOX2MAIN_WINFRM_Y_ST_BTN, BOX2SEQ_MAINSEQ_STATUS_BUTTON );
		}
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
	}

	ret = BOX2UI_BoxArrangePokeMove( syswk );

	switch( ret ){
	case BOX2UI_ARRANGE_MOVE_TRAY1:	// 37: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveButtonFrmIn( syswk, 0 );

	case BOX2UI_ARRANGE_MOVE_TRAY2:	// 38: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveButtonFrmIn( syswk, 1 );

	case BOX2UI_ARRANGE_MOVE_TRAY3:	// 39: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveButtonFrmIn( syswk, 2 );

	case BOX2UI_ARRANGE_MOVE_TRAY4:	// 40: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveButtonFrmIn( syswk, 3 );

	case BOX2UI_ARRANGE_MOVE_TRAY5:	// 41: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveButtonFrmIn( syswk, 4 );

	case BOX2UI_ARRANGE_MOVE_TRAY6:	// 42: �g���C�A�C�R��
//		Snd_SePlay( SE_BOX2_DECIDE );
		BOX2MAIN_PokeSelectOff( syswk );
		return BoxMoveButtonFrmIn( syswk, 5 );

	case BOX2UI_ARRANGE_MOVE_LEFT:		// 43: �g���C���i���j
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		BoxMoveTrayIconChgOnly( syswk, -1 );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == TRUE ){
			return BoxMoveButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN );
		}else if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
			return YStatusButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN );
		}
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;

	case BOX2UI_ARRANGE_MOVE_RIGHT:	// 44: �g���C���i�E�j
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		BOX2MAIN_PokeSelectOff( syswk );
		BoxMoveTrayIconChgOnly( syswk, 1 );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == TRUE ){
			return BoxMoveButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN );
		}else if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
			return YStatusButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN );
		}
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;

	case BOX2UI_ARRANGE_MOVE_CHANGE:	// 45: �{�b�N�X�����肩����
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_BOXMV_BTN, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOX_MOVE_BUTTON );

	case CURSORMOVE_CURSOR_MOVE:	// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos < BOX2UI_ARRANGE_MOVE_RETURN ){
				if( BOX2MAIN_PokeInfoPut( syswk, pos ) == TRUE ){
					if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == FALSE ){
						BOX2BMP_ButtonPutYStatus( syswk );
						BOX2BGWFRM_YStatusButtonInSet( syswk->app->wfrm );
					}
				}else{
					if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
						BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
					}
				}
			}else{
				BOX2MAIN_PokeInfoOff( syswk );
				if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
					BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
				}
			}
		}
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		if( syswk->app->vfunk.freq != NULL ){
			return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN_CURMOVE_RET );
		}
		break;

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
	case CURSORMOVE_NONE:			// ����Ȃ�
		break;

	case BOX2UI_ARRANGE_MOVE_RETURN:	// 36: ��߂�
//		Snd_SePlay( SE_BOX2_CLOSE_PARTY_TRAY );
		return PartyButtonAnmSet( syswk, 1, BOX2SEQ_MAINSEQ_BOX_MOVE_END_BUTTON );

	case CURSORMOVE_CANCEL:				// �L�����Z��
		if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == FALSE ){
//			Snd_SePlay( SE_BOX2_CLOSE_PARTY_TRAY );
			return PartyButtonAnmSet( syswk, 1, BOX2SEQ_MAINSEQ_BOX_MOVE_END_BUTTON );
		}else{
//			Snd_SePlay( SE_BOX2_CANCEL );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX );
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
			return BoxMoveButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN );
		}
		break;

	default:
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
			BOX2MAIN_PokeInfoPut( syswk, ret );
			return BoxPartyItemGetKey( syswk, ret );
		}
	}

	return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�������̂�����v���C���֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeBoxPartyChgMainCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F���ʃA�C�e���t���[���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeBoxPartyIconMoveSubFrmOut( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BGWFRM_SubDispItemFrmOutSet( syswk->app->wfrm );
	return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_ICONMOVE_POKEADD;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F����������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeBoxPartyIconMovePokeAdd( BOX2_SYS_WORK * syswk )
{
	POKEMON_PASO_PARAM * ppp;
	BOX2MAIN_ITEMMOVE_WORK * mvwk;
	u16	item;

	mvwk = syswk->app->vfunk.work;

	if( BGWINFRM_MoveCheck( syswk->app->wfrm, BOX2MAIN_WINFRM_SUBDISP ) != 0 ){
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_ICONMOVE_POKEADD;
	}

	if( mvwk->put_pos == syswk->get_pos ){
		u32	set_pos = mvwk->set_pos;
		ItemMoveWorkFree( syswk );
		BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
		if( ITEM_CheckMail( syswk->app->get_item ) == TRUE && set_pos != syswk->get_pos ){
//			Snd_SePlay( SE_BOX2_WARNING );
			BOX2BMP_MailMoveErrorPut( syswk, BOX2BMPWIN_ID_MSG1 );
			syswk->next_seq = BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_ERROR;
			return BOX2SEQ_MAINSEQ_TRGWAIT;
		}else{
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
			return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
		}
	}

	BOX2OBJ_PokeIconBlendSet( syswk->app, mvwk->put_pos, FALSE );
	BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, TRUE );

	// �ړ���ɃA�C�e�����Z�b�g
	item = BOX2MAIN_PokeParaGet( syswk, mvwk->put_pos, ID_PARA_item, NULL );
	BOX2MAIN_PokeParaPut( syswk, mvwk->put_pos, ID_PARA_item, syswk->app->get_item );
	ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, mvwk->put_pos );
	if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
		BOX2OBJ_PokeIconChange(
			syswk, syswk->tray, mvwk->put_pos,
			syswk->app->pokeicon_id[mvwk->put_pos] );
	}
	BOX2MAIN_PokeInfoPut( syswk, mvwk->put_pos );
	CURSORMOVE_PosSet( syswk->app->cmwk, mvwk->put_pos );

	// �ړ����ɃA�C�e�����Z�b�g
	syswk->app->get_item = item;
	BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, ID_PARA_item, syswk->app->get_item );
	ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );
	if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
		BOX2OBJ_PokeIconChange(
			syswk, syswk->tray, syswk->get_pos,
			syswk->app->pokeicon_id[syswk->get_pos] );
	}

	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		syswk->get_pos = mvwk->put_pos;
		ItemMoveWorkFree( syswk );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
	}

	BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
	BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 1 );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
	BOX2OBJ_ItemIconCursorAdd( syswk->app );
//	Snd_SePlay( SE_BOX2_POKE_CATCH );
	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeBoxPartyIconChgTouch, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_ICONMOVE_END );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�����������I��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeBoxPartyIconMoveEnd( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_ITEMMOVE_WORK * mvwk;
	u16	item;
	u16	get, set;

	mvwk = syswk->app->vfunk.work;

	BOX2OBJ_PokeIconBlendSet( syswk->app, mvwk->put_pos, FALSE );

	get = syswk->get_pos;
	set = mvwk->set_pos;

	syswk->get_pos = mvwk->put_pos;

	ItemMoveWorkFree( syswk );

	item = syswk->app->get_item;
	syswk->app->get_item = ITEM_DUMMY_DATA;

	if( ITEM_CheckMail( item ) == TRUE && set != get ){
//		Snd_SePlay( SE_BOX2_WARNING );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, FALSE );
		BOX2BMP_MailMoveErrorPut( syswk, BOX2BMPWIN_ID_MSG1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_ERROR;
		return BOX2SEQ_MAINSEQ_TRGWAIT;
	}

	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );

	return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F����ւ��G���[���b�Z�[�W�\����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeBoxPartyChgError( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_SysWinVanishTrans( syswk->app, BOX2BMPWIN_ID_MSG1 );
	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�L�[�ŃA�C�e������ւ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeKeyGetMoveMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	ret = BOX2UI_BoxItemArrangeMoveHand( syswk );

	if( CURSORMOVE_CursorOnOffGet( syswk->app->cmwk ) == FALSE ){
		ret = CURSORMOVE_CANCEL;
	}

	switch( ret ){
	case CURSORMOVE_CURSOR_MOVE:	// �ړ�
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_KEYGET_MOVE_MAIN_CURMOVE_RET );

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		break;

	case CURSORMOVE_NONE:			// ����Ȃ�
		break;

	case BOX2UI_ARRANGE_MOVE_RETURN:	// 36: ��߂�
	case CURSORMOVE_CANCEL:				// �L�����Z��
//		Snd_SePlay( SE_BOX2_POKE_PUT );
		return BoxPartyItemPutKey( syswk, BOX2MAIN_GETPOS_NONE );

	default:
//		Snd_SePlay( SE_BOX2_POKE_PUT );
		return BoxPartyItemPutKey( syswk, ret );
	}

	return BOX2SEQ_MAINSEQ_ITEMARRANGE_KEYGET_MOVE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�������̂�����v�L�[�擾���C���֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeKeyGetMoveMainCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_ITEMARRANGE_KEYGET_MOVE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�����������i�L�[����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeKeyGetChangeCkack( BOX2_SYS_WORK * syswk )
{
	POKEMON_PASO_PARAM * ppp;
	u16	item;

	BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->app->poke_put_key, FALSE );

	if( syswk->app->poke_put_key == syswk->app->get_item_init_pos ){
		syswk->app->get_item = ITEM_DUMMY_DATA;
		BOX2MAIN_PokeInfoPut( syswk, syswk->app->poke_put_key );
		BOX2BGWFRM_YStatusButtonInSet( syswk->app->wfrm );
		return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveButtonYStatusButtonMove, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN );
	}

	// �����Ă���A�C�e�����擾
	item = BOX2MAIN_PokeParaGet( syswk, syswk->app->poke_put_key, ID_PARA_item, NULL );
	// �A�C�e������������
	BOX2MAIN_PokeParaPut( syswk, syswk->app->poke_put_key, ID_PARA_item, syswk->app->get_item );
	ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->app->poke_put_key );
	if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
		BOX2OBJ_PokeIconChange(
			syswk, syswk->tray, syswk->app->poke_put_key,
			syswk->app->pokeicon_id[syswk->app->poke_put_key] );
	}
	BOX2MAIN_PokeInfoPut( syswk, syswk->app->poke_put_key );

	// �擾�����A�C�e�������̃|�P�����Ɏ�������
	syswk->app->get_item = item;
	BOX2MAIN_PokeParaPut( syswk, syswk->app->get_item_init_pos, ID_PARA_item, syswk->app->get_item );
	ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->app->get_item_init_pos );
	if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
		BOX2OBJ_PokeIconChange(
			syswk, syswk->tray, syswk->app->get_item_init_pos,
			syswk->app->pokeicon_id[syswk->app->get_item_init_pos] );
	}

	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		BOX2BGWFRM_YStatusButtonInSet( syswk->app->wfrm );
		return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveButtonYStatusButtonMove, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN );
	}

	syswk->app->poke_get_key = 1;

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
	BOX2OBJ_ItemIconPokePutHand( syswk->app, syswk->app->poke_put_key, BOX2MAIN_POKEMOVE_MODE_ALL );
	BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );

	{
		u32 winID = BOX2BMP_PokeItemInfoWrite( syswk, syswk->app->get_item );
		BGWINFRM_BmpWinOn( syswk->app->wfrm, BOX2MAIN_WINFRM_SUBDISP, syswk->app->win[winID].win );
		BGWINFRM_BmpWinOn( syswk->app->wfrm, BOX2MAIN_WINFRM_SUBDISP, syswk->app->win[winID+1].win );
		BOX2OBJ_ItemIconChangeSub( syswk->app, syswk->app->get_item );
		BOX2BGWFRM_SubDispItemFrmInSet( syswk->app->wfrm );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeBoxPartyIconGetKey, BOX2SEQ_MAINSEQ_ITEMARRANGE_KEYGET_MOVE_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�����������E�莝�����C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangePartyMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	ret = BOX2UI_HitCheckTrgPartyPokeLeft();
	if( ret != GFL_UI_TP_HIT_NONE ){
//��[GS_CONVERT_TAG]
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_POKE_CATCH );
			GFL_UI_TP_GetPointTrg( &syswk->app->tpx, &syswk->app->tpy );
//��[GS_CONVERT_TAG]
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
			return PartyItemGet( syswk, ret );
		}else{
			if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
				u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
				BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
				CURSORMOVE_PosSet( syswk->app->cmwk, pos );
				BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
				BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
				BOX2OBJ_PokeCursorVanish( syswk, FALSE );
				if( syswk->app->get_item != ITEM_DUMMY_DATA ){
					BOX2BGWFRM_SubDispItemFrmOutSet( syswk->app->wfrm );
				}
				return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeMenuClose, BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN_CURMOVE_RET );
			}else{
				CURSORMOVE_PosSet( syswk->app->cmwk, ret - BOX2OBJ_POKEICON_TRAY_MAX );
				CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
				BOX2MAIN_PokeInfoOff( syswk );
				return BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN;
			}
		}
	}

	ret = BOX2UI_BoxItemArrangePartyMoveMain( syswk );

	switch( ret ){

	case BOX2UI_ITEM_A_PARTY_CHANGE:	//�u���ꂩ���v
//		Snd_SePlay( SE_BOX2_DECIDE );
		return PartyButtonAnmSet( syswk, 0, BOX2SEQ_MAINSEQ_ITEM_CHG_BUTTON );

	case BOX2UI_ITEM_A_PARTY_MENU1:	// �o�b�O�� or ��������
//		Snd_SePlay( SE_BOX2_DECIDE );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_MENU5, BOX2SEQ_MAINSEQ_ITEMARRANGE_BAG_BUTTON );

	case CURSORMOVE_CURSOR_MOVE:		// �ړ�
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos <= BOX2UI_ARRANGE_PARTY_POKE6 ){
				BOX2MAIN_PokeInfoPut( syswk, pos+BOX2OBJ_POKEICON_TRAY_MAX );
			}else{
				if( pos != BOX2UI_ITEM_A_PARTY_MENU1 &&
					pos != BOX2UI_ITEM_A_PARTY_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN_CURMOVE_RET );

	case BOX2UI_ITEM_A_PARTY_RETURN:	//�u���ǂ�v
	case CURSORMOVE_CANCEL:				// �L�����Z��
//		Snd_SePlay( SE_BOX2_CLOSE_PARTY_TRAY );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_RET_BTN, BOX2SEQ_MAINSEQ_PARTY_END_BUTTON );

	case BOX2UI_ITEM_A_PARTY_CLOSE:	//�u�Ƃ���v
//		Snd_SePlay( SE_BOX2_CANCEL );
		{
			u8	pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, pos, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, pos );
		}
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_CLOSE_BTN, BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_CLOSE_BUTTON );

	case CURSORMOVE_CURSOR_ON:			// �J�[�\���\��
		{
			u8	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
			if( pos <= BOX2UI_ARRANGE_PARTY_POKE6 ){
				BOX2MAIN_PokeInfoPut( syswk, pos+BOX2OBJ_POKEICON_TRAY_MAX );
			}else{
				if( pos != BOX2UI_ITEM_A_PARTY_MENU1 &&
					pos != BOX2UI_ITEM_A_PARTY_CLOSE ){
					BOX2MAIN_PokeInfoOff( syswk );
				}
			}
		}
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		break;

	case CURSORMOVE_NONE:				// ����Ȃ�
		break;

	default:
		ret += BOX2OBJ_POKEICON_TRAY_MAX;
		if( BOX2MAIN_PokeParaGet( syswk, ret, ID_PARA_poke_exist, NULL ) != 0 ){
//			Snd_SePlay( SE_BOX2_DECIDE );
			BOX2MAIN_PokeInfoPut( syswk, ret );
			BOX2UI_CursorMoveVFuncWorkSet( syswk->app, BOX2UI_ITEM_A_PARTY_MENU1, CURSORMOVE_PosGet(syswk->app->cmwk) );
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_A_PARTY_MENU1 );
			return PartyItemGetKey( syswk, ret );
		}
	}

	return BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�������������肷��v�莝���|�P�����I�����C���֖߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangePartyMainCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���|�P�����̃A�C�e���ړ��J�n
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangePartyIconMoveRet( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_ITEMMOVE_WORK * mvwk;

	mvwk = syswk->app->vfunk.work;

	if( mvwk->put_pos == syswk->get_pos || syswk->app->get_item == ITEM_DUMMY_DATA ){
		u32	set_pos = mvwk->set_pos;
		ItemMoveWorkFree( syswk );
		if( syswk->app->party_frm == 0 ){
			if( ITEM_CheckMail( syswk->app->get_item ) == TRUE && set_pos != syswk->get_pos ){
//				Snd_SePlay( SE_BOX2_WARNING );
				BGWINFRM_FrameOff( syswk->app->wfrm, BOX2MAIN_WINFRM_MENU5 );
				BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );
				BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_MAIL_MOVE, BOX2BMPWIN_ID_MSG4 );
				syswk->next_seq = BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_ERR;
				return BOX2SEQ_MAINSEQ_TRGWAIT;
			}
		}else{
			BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG4 );
			syswk->app->party_frm = 0;
		}
		if( syswk->app->get_item != ITEM_DUMMY_DATA ){
//			Snd_SePlay( SE_BOX2_POKE_PUT );
			BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );
		}
		CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_A_PARTY_MENU1 );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN;
	}

	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG4 );

//	Snd_SePlay( SE_BOX2_POKE_PUT );

	BOX2OBJ_PokeIconBlendSet( syswk->app, mvwk->put_pos, FALSE );
	BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, TRUE );

	// �A�E�g���C������
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	// �A�C�e���A�C�R���k��
	BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );

	return BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_POKEADD;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���|�P�����̃A�C�e���ړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangePartyIconMovePokeAdd( BOX2_SYS_WORK * syswk )
{
	POKEMON_PASO_PARAM * ppp;
	BOX2MAIN_ITEMMOVE_WORK * mvwk;
	u16	item;

	if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_POKEADD;
	}

	BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );

	mvwk = syswk->app->vfunk.work;

	// �ړ���ɃA�C�e�����Z�b�g
	item = BOX2MAIN_PokeParaGet( syswk, mvwk->put_pos, ID_PARA_item, NULL );
	BOX2MAIN_PokeParaPut( syswk, mvwk->put_pos, ID_PARA_item, syswk->app->get_item );
	ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, mvwk->put_pos );
	if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
		BOX2OBJ_PokeIconChange(
			syswk, syswk->tray, mvwk->put_pos,
			syswk->app->pokeicon_id[mvwk->put_pos] );
	}
	BOX2MAIN_PokeInfoPut( syswk, mvwk->put_pos );

	// �ړ����ɃA�C�e�����Z�b�g
	syswk->app->get_item = item;
	BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, ID_PARA_item, syswk->app->get_item );
	ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );
	if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
		BOX2OBJ_PokeIconChange(
			syswk, syswk->tray, syswk->get_pos,
			syswk->app->pokeicon_id[syswk->get_pos] );
	}

	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		if( syswk->app->party_frm == 0 ){
			syswk->get_pos = mvwk->put_pos;
			ItemMoveWorkFree( syswk );
			CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX );
			BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
			return VFuncSet( syswk, BOX2MAIN_VFuncUnderButtonMove, BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_POKEADD_END );
		}else{
			syswk->app->party_frm = 0;
			ItemMoveWorkFree( syswk );
			CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX );
			BOX2BGWFRM_PartyPokeFrameInSet( syswk->app->wfrm );
			BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
			return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrameMove, BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_POKEADD_END );
		}
	}

//	Snd_SePlay( SE_BOX2_POKE_CATCH );

	BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
	BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 1 );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, TRUE );
	BOX2OBJ_ItemIconCursorAdd( syswk->app );

	return VFuncSet( syswk, BOX2MAIN_VFuncItemIconPartyChange, BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_CHANGE );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���|�P�����̃A�C�e���ړ��I��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangePartyIconMovePokeAddEnd( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	return BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���|�P�����̃A�C�e������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangePartyIconMoveChange( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_ITEMMOVE_WORK * mvwk;

	mvwk = syswk->app->vfunk.work;

	BOX2OBJ_PokeIconBlendSet( syswk->app, mvwk->put_pos, FALSE );

//	Snd_SePlay( SE_BOX2_POKE_PUT );

	// �A�E�g���C������
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	// �A�C�e���A�C�R���k��
	BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );

	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		syswk->get_pos = mvwk->put_pos;
	}
	CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX );

	ItemMoveWorkFree( syswk );

	return BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_CHANGE_END;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���|�P�����̃A�C�e�������I��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangePartyIconMoveChangeEnd( BOX2_SYS_WORK * syswk )
{
	if( BOX2OBJ_AnmCheck( syswk->app, BOX2OBJ_ID_ITEMICON ) == TRUE ){
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_CHANGE_END;
	}

	BOX2OBJ_ItemIconAffineSet( syswk->app, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );

	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncUnderButtonMove, BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_POKEADD_END );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���|�P�����̃A�C�e���ړ��G���[
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangePartyIconMoveErr( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG4 );
	BGWINFRM_FrameOn( syswk->app->wfrm, BOX2MAIN_WINFRM_MENU5 );
	CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ITEM_A_PARTY_MENU1 );
	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	return BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�����F�^�}�S�ɃA�C�e�������������Ȃ����b�Z�[�W�\��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeItemSetEggError( BOX2_SYS_WORK * syswk )
{
//	Snd_SePlay( SE_BOX2_WARNING );
	BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_EGG, BOX2BMPWIN_ID_MSG1 );
	syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
	return BOX2SEQ_MAINSEQ_TRGWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�����F���[����a����Ȃ����b�Z�[�W�\��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeItemGetMailError( BOX2_SYS_WORK * syswk )
{
//	Snd_SePlay( SE_BOX2_WARNING );
	BOX2BMP_MailGetErrorPut( syswk, BOX2BMPWIN_ID_MSG1 );
	syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
	return BOX2SEQ_MAINSEQ_TRGWAIT;
}


//============================================================================================
//	�ݒ�֐�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�C���ݒ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	next	�t�F�[�h��̃V�[�P���X
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
 * �t�F�[�h�A�E�g�ݒ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	next	�t�F�[�h��̃V�[�P���X
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
 * �E�F�C�g�ݒ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	next	�E�F�C�g��̃V�[�P���X
 * @param	wait	�E�F�C�g
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
 * VBLANK�֐��Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	func	VBLANK�֐�
 * @param	next	VBLANK�֐��I����̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int VFuncSet( BOX2_SYS_WORK * syswk, void * func, int next )
{
	syswk->next_seq = next;
	BOX2MAIN_VFuncSet( syswk->app, func );
	return BOX2SEQ_MAINSEQ_VFUNC;
}

//--------------------------------------------------------------------------------------------
/**
 * �ݒ�ς݂�VBLANK�֐��J�n���N�G�X�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	next	VBLANK�֐��I����̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int VFuncReqSet( BOX2_SYS_WORK * syswk, int next )
{
	syswk->next_seq = next;
	BOX2MAIN_VFuncReqSet( syswk->app );
	return BOX2SEQ_MAINSEQ_VFUNC;
}

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	id		�͂��E�������h�c
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int YesNoSet( BOX2_SYS_WORK * syswk, u32 id )
{
/*
	if( id == YESNO_ID_POKEFREE ){
		BOX2MAIN_YesNoWinSet( syswk, 1 );
	}else{
		BOX2MAIN_YesNoWinSet( syswk, 0 );
	}
	syswk->app->ynID = id;
*/
	syswk->app->ynID = id;
	BOX2MAIN_YesNoWinSet( syswk, 0 );
	return BOX2SEQ_MAINSEQ_YESNO;
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u�v���Z�X�Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	type	�T�u�v���Z�X�^�C�v
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int SubProcSet( BOX2_SYS_WORK * syswk, u8 type )
{
	syswk->subProcType = type;
	syswk->next_seq = BOX2SEQ_MAINSEQ_SUB_PROCCALL;
	return FadeOutSet( syswk, BOX2SEQ_MAINSEQ_RELEASE );
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�؂�ւ��E��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	next	�؂�ւ���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayLeftScrollSet( BOX2_SYS_WORK * syswk, int next )
{
	if( syswk->tray == 0 ){
		syswk->tray = syswk->trayMax-1;
	}else{
		syswk->tray--;
	}

	BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
	BOX2MAIN_WallPaperSet(
		syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), BOX2MAIN_TRAY_SCROLL_L );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_L_ARROW, BOX2OBJ_ANM_L_ARROW_ON );

	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		if( syswk->dat->callMode == BOX_MODE_SEIRI || syswk->dat->callMode == BOX_MODE_ITEM ){
			BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
			BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );
		}
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncTrayScrollLeft, next );
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�؂�ւ��E�E
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	next	�؂�ւ���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayRightScrollSet( BOX2_SYS_WORK * syswk, int next )
{
	if( syswk->tray == (syswk->trayMax-1) ){
		syswk->tray = 0;
	}else{
		syswk->tray++;
	}

	BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
	BOX2MAIN_WallPaperSet(
		syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), BOX2MAIN_TRAY_SCROLL_R );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_R_ARROW, BOX2OBJ_ANM_R_ARROW_ON );

	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		if( syswk->dat->callMode == BOX_MODE_SEIRI || syswk->dat->callMode == BOX_MODE_ITEM ){
			BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
			BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );
		}
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncTrayScrollRight, next );
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�̃|�P�������擾
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayPokeGet( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_PokeCursorAdd( syswk );

	PokeMoveWorkAlloc( syswk );

	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == FALSE ){
		BOX2BGWFRM_TemochiButtonOutSet( syswk->app->wfrm );
		BOX2BGWFRM_IdouButtonOutSet( syswk->app->wfrm );
		BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );
	}
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );

	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_TRAY;
	return VFuncSet( syswk, BOX2MAIN_VFuncGetPokeMove, BOX2SEQ_MAINSEQ_ARRANGE_TRAYPOKE_CHG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�̃|�P�������擾�i�莝���ɗ��Ƃ��郂�[�h�j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayPokeGetDrop( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_PokeCursorAdd( syswk );
	BOX2OBJ_PartyPokeIconFrmInSetRight( syswk );

	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, TRUE );
	BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );

	BOX2BMP_PokePartyInMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG3 );
	if( PokeParty_GetPokeCount( syswk->dat->pokeparty ) != TEMOTI_POKEMAX ){
		BOX2BMP_VBlankMsgSet( syswk, BOX2BMPWIN_ID_MSG4, BOX2BMP_MSGID_VBLANK_TSURETEIKU );
	}else{
		BOX2BMP_VBlankMsgSet( syswk, BOX2BMPWIN_ID_MSG4, BOX2BMP_MSGID_VBLANK_TSURETEIKU_ERR );
	}

	PokeMoveWorkAlloc( syswk );
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == FALSE ){
		BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );
	}

	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;
	return VFuncSet( syswk, BOX2MAIN_GetTrayPokeMoveDrop, BOX2SEQ_MAINSEQ_PARTYIN_POKECHG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�̃|�P�������擾�i�L�[����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayPokeGetKey( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
	BOX2OBJ_PokeCursorAdd( syswk );

	BOX2BMP_PokePartyInMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG3 );

	BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );
	BOX2BGWFRM_SubDispWazaFrmInSet( syswk->app );

	return VFuncSet( syswk, BOX2MAIN_VFuncMenuOpenKey, BOX2SEQ_MAINSEQ_PARTYIN_MAIN_CURMOVE_RET );
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�̃|�P�������擾�i�L�[����E�{�b�N�X�����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayPokeGetKeyArrange( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
	BOX2OBJ_PokeCursorAdd( syswk );

	BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
	BOX2BGWFRM_TemochiButtonOutSet( syswk->app->wfrm );
	BOX2BGWFRM_IdouButtonOutSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );

	BOX2BGWFRM_SubDispWazaFrmInSet( syswk->app );

	return VFuncSet( syswk, BOX2MAIN_VFuncMenuOpenKeyArrange, BOX2SEQ_MAINSEQ_ARRANGE_MAIN_CURMOVE_RET );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�������擾
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyPokeGet( BOX2_SYS_WORK * syswk, u32 pos )
{
	POKEMON_PARAM * pp;

	syswk->get_pos = pos;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_PokeCursorAdd( syswk );

	pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, pos-BOX2OBJ_POKEICON_TRAY_MAX );

	// �莝���P�C
	if( BOX2MAIN_BattlePokeCheck( syswk, pos - BOX2OBJ_POKEICON_TRAY_MAX ) == FALSE &&
		BOXDAT_GetPokeExistCount2( syswk->dat->sv_box, syswk->tray ) == 0 ){
		BOX2BMP_VBlankMsgSet( syswk, BOX2BMPWIN_ID_MSG4, BOX2BMP_MSGID_VBLANK_BATTLE_POKE_ERR );
	// ���[��
	}else if( ITEM_CheckMail( PP_Get(pp,ID_PARA_item,NULL) ) == TRUE ){
		BOX2BMP_VBlankMsgSet( syswk, BOX2BMPWIN_ID_MSG4, BOX2BMP_MSGID_VBLANK_MAIL_ERR );
/*	�J�X�^���{�[���͂Ȃ��n�Y
	// �J�v�Z��
	}else if( PP_Get( pp, ID_PARA_cb_id, NULL ) != 0 ){
		BOX2BMP_VBlankMsgSet( syswk, BOX2BMPWIN_ID_MSG4, BOX2BMP_MSGID_VBLANK_CAPSULE_ERR );
*/
	}else{
		BOX2BMP_VBlankMsgSet( syswk, BOX2BMPWIN_ID_MSG4, BOX2BMP_MSGID_VBLANK_AZUKERU );
	}

	PokeMoveWorkAlloc( syswk );
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );
	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_PARTY;
	return VFuncSet( syswk, BOX2MAIN_VFuncGetPokeMoveParty, BOX2SEQ_MAINSEQ_ARRANGE_PARTYPOKE_CHG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�������擾�i�g���C�ɗ��Ƃ��郂�[�h�j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyPokeGetDrop( BOX2_SYS_WORK * syswk, u32 pos )
{
	POKEMON_PARAM * pp;
	u32	i;

	syswk->get_pos = pos;

	for( i=BOX2OBJ_POKEICON_TRAY_MAX; i<BOX2OBJ_POKEICON_MAX; i++ ){
		if( i == syswk->get_pos ){
			BOX2OBJ_PokeIconPriChg( syswk->app, i, BOX2OBJ_POKEICON_PRI_CHG_GET );
		}else{
			BOX2OBJ_PokeIconPriChg( syswk->app, i, BOX2OBJ_POKEICON_PRI_CHG_PUT );
		}
	}
	BOX2OBJ_PokeCursorAdd( syswk );

	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, TRUE );
	BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, FALSE );

	BOX2BMP_PokePartyInMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG3 );

	pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, pos - BOX2OBJ_POKEICON_TRAY_MAX );

	// �莝���P�C
	if( BOX2MAIN_BattlePokeCheck( syswk, pos - BOX2OBJ_POKEICON_TRAY_MAX ) == FALSE ){
		BOX2BMP_VBlankMsgSet( syswk, BOX2BMPWIN_ID_MSG4, BOX2BMP_MSGID_VBLANK_BATTLE_POKE_ERR );
	// ���[��
	}else if( ITEM_CheckMail( PP_Get(pp,ID_PARA_item,NULL) ) == TRUE ){
		BOX2BMP_VBlankMsgSet( syswk, BOX2BMPWIN_ID_MSG4, BOX2BMP_MSGID_VBLANK_MAIL_ERR );
/*	�J�X�^���{�[���͂Ȃ��n�Y
	// �J�v�Z��
	}else if( PP_Get( pp, ID_PARA_cb_id, NULL ) != 0 ){
		BOX2BMP_VBlankMsgSet( syswk, BOX2BMPWIN_ID_MSG4, BOX2BMP_MSGID_VBLANK_CAPSULE_ERR );
*/
	}else{
		if( BOXDAT_GetEmptySpaceTray( syswk->dat->sv_box, syswk->tray ) == 0 ){
			BOX2BMP_VBlankMsgSet( syswk, BOX2BMPWIN_ID_MSG4, BOX2BMP_MSGID_VBLANK_AZUKERU_ERR );
		}else{
			BOX2BMP_VBlankMsgSet( syswk, BOX2BMPWIN_ID_MSG4, BOX2BMP_MSGID_VBLANK_AZUKERU );
		}
	}

	PokeMoveWorkAlloc( syswk );
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == FALSE ){
		BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );
	}

	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_PARTY;
	return VFuncSet( syswk, BOX2MAIN_GetPartyPokeMoveDrop, BOX2SEQ_MAINSEQ_PARTYOUT_POKECHG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�������擾�i�L�[����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyPokeGetKey( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
	BOX2OBJ_PokeCursorAdd( syswk );

	BOX2BMP_PokePartyInMsgPut( syswk, syswk->get_pos, BOX2BMP_MSGID_PARTYIN_MENU, BOX2BMPWIN_ID_MSG3 );

	BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );
	BOX2BGWFRM_SubDispWazaFrmInSet( syswk->app );

	return VFuncSet( syswk, BOX2MAIN_VFuncMenuOpenKey, BOX2SEQ_MAINSEQ_PARTYOUT_MAIN_CURMOVE_RET );
}

// �莝���|�P�������擾�i�L�[����E�{�b�N�X�����j
static int PartyPokeGetKeyArrange( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
	BOX2OBJ_PokeCursorAdd( syswk );

	BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );

	BOX2BGWFRM_SubDispWazaFrmInSet( syswk->app );

	return VFuncSet( syswk, BOX2MAIN_VFuncMenuOpenKeyArrange, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN_CURMOVE_RET );
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C/�莝���|�P�������擾
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxPartyPokeGet( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_PokeCursorAdd( syswk );

	if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == TRUE ){
		BOX2BGWFRM_BoxMoveButtonOutSet( syswk->app->wfrm );
	}else if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
		BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
	}

	BOX2BGWFRM_SubDispWazaFrmInSet( syswk->app );

	BOX2BMP_ButtonPutYStatus( syswk );

	PokeMoveWorkAlloc( syswk );
	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;
	return VFuncSet( syswk, BOX2MAIN_VFuncGetPokeMoveBoxParty, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTYPOKE_CHG_END );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R�����샏�[�N�擾
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeMoveWorkAlloc( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_POKEMOVE_WORK * work;
	u32	i;

	work = GFL_HEAP_AllocClearMemory( HEAPID_BOX_APP, sizeof(BOX2MAIN_POKEMOVE_WORK) );

/*
	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
		work->dat[i].pp = PokemonParam_AllocWork( HEAPID_BOX_APP );
	}
	work->cnt = 0;
*/

	syswk->app->vfunk.work = work;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R�����샏�[�N���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeMoveWorkFree( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_POKEMOVE_WORK * work;
	u32	i;

	work = syswk->app->vfunk.work;

/*
	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX+1; i++ ){
		GFL_HEAP_FreeMemory( work->dat[i].pp );
	}
*/

	GFL_HEAP_FreeMemory( work );
//��[GS_CONVERT_TAG]
}

//--------------------------------------------------------------------------------------------
/**
 * �������̃��j���[
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PokeItemCheck( BOX2_SYS_WORK * syswk )
{
	u32 item;

	// �^�}�S
	if( BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_tamago_flag, NULL ) != 0 ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeFreeMenuMove, BOX2SEQ_MAINSEQ_ARRANGE_ITEMSET_EGG_ERROR );
	}
	
	item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_item, NULL );

	if( item == ITEM_DUMMY_DATA ){
		syswk->cur_rcv_pos = BOX2UI_ARRANGE_MAIN_ITEM;
		return SubProcSet( syswk, SUB_PROC_TYPE_BAG );
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		if( ITEM_CheckMail( item ) == TRUE ){
			BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
			return VFuncSet( syswk, BOX2MAIN_VFuncPokeFreeMenuMove, BOX2SEQ_MAINSEQ_ARRANGE_ITEMGET_MAIL_ERROR );
		}else{
			BOX2BGWFRM_PokeMenuOff( syswk->app->wfrm );
		}
	}
	BOX2BMP_ItemGetCheckMsgPut( syswk, item, BOX2BMPWIN_ID_MSG1 );
	return YesNoSet( syswk, YESNO_ID_ITEMGET );
}

//--------------------------------------------------------------------------------------------
/**
 * �}�[�L���O�t���[���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MarkingScrollOutSet( BOX2_SYS_WORK * syswk )
{
	// �L�[����ݒ�
	switch( syswk->dat->callMode ){
	case BOX_MODE_AZUKERU:
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_PTOUT_MAIN, BOX2UI_PTOUT_MAIN_MARKING );
		break;
	case BOX_MODE_TURETEIKU:
		BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_PTIN_MAIN, BOX2UI_PTIN_MAIN_MARKING );
		break;
	case BOX_MODE_SEIRI:
	default:
		if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_MAIN, BOX2UI_ARRANGE_MAIN_MARKING );
		}else{
			BOX2UI_CursorMoveChange( syswk, BOX2UI_INIT_ID_ARRANGE_PARTY, BOX2UI_ARRANGE_PARTY_MARKING );
		}
		break;
	}
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG2 );
	if( syswk->box_mv_flg != 0 ){
		BOX2BMP_BoxMoveNameNumWrite( syswk );
	}

	if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
	}
	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );

	BOX2BGWFRM_MarkingFrameOutSet( syswk->app->wfrm );

	if( syswk->dat->callMode == BOX_MODE_SEIRI && syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		return VFuncSet( syswk, BOX2MAIN_VFuncMarkingFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_MARKING_RCV );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncMarkingFrameMove, BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV );
}

//--------------------------------------------------------------------------------------------
/**
 * �}�[�L���O�J�n
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MarkingStartSet( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_ARRANGE_MARKING_INIT );
}

//--------------------------------------------------------------------------------------------
/**
 * �������J�n
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PokeFreeStartSet( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );

	return VFuncSet( syswk, BOX2MAIN_VFuncPokeFreeMenuMove, BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_START );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X/�莝���{�^���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxPartyButtonOutSet( BOX2_SYS_WORK * syswk )
{
	syswk->party_sel_flg = 1;

	BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

	BOX2BGWFRM_TemochiButtonOutSet( syswk->app->wfrm );
	BOX2BGWFRM_IdouButtonOutSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncUnderButtonMove, BOX2SEQ_MAINSEQ_ARRANGE_PARTYPOKE_FRMIN );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���t���[���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyFrmOutSet( BOX2_SYS_WORK * syswk )
{
	syswk->party_sel_flg = 0;

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == FALSE ){
		return MainSeq_ArrangePartyPokeFrmOut( syswk );
	}

	BOX2MAIN_PokeSelectOff( syswk );
	return PokeMenuOutSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_PARTYPOKE_FRMOUT );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���t���[���Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyFrmSet( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PartyPokeFrameLoadArrange( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY );
	BOX2BMP_PartyCngButtonFrmPut( syswk->app );
	BOX2BMP_PartyChangeButtonPut( syswk->app );

	BOX2BGWFRM_BoxPartyButtonVanish( syswk->app->wfrm );
	BOX2BGWFRM_ButtonPutYameru( syswk );
	BOX2BGWFRM_PartyPokeFramePut( syswk->app->wfrm );
	BOX2OBJ_PartyPokeIconFrmSet( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���t���[���Z�b�g�i���j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyFrmSetRight( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PartyPokeFrameLoadArrange( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY );
	BOX2BMP_PartyCngButtonFrmPut( syswk->app );
	BOX2BMP_PartySelectButtonPut( syswk->app );

	BOX2BGWFRM_BoxPartyButtonVanish( syswk->app->wfrm );
	BOX2BGWFRM_PartyPokeFramePutRight( syswk->app->wfrm );
	BOX2OBJ_PartyPokeIconFrmSetRight( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���t���[���Z�b�g�i�|�P��������������j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
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
 * �莝���t���[�����E�ֈړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyFrmMoveRight( BOX2_SYS_WORK * syswk )
{
	syswk->get_pos = BOX2MAIN_GETPOS_NONE;
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	BOX2BGWFRM_PartyPokeFrameRightMoveSet( syswk->app->wfrm );
	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	}
	return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrmRight, BOX2SEQ_MAINSEQ_ARRANGE_PARTYFRM_MOVE_R_END );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���t���[�������ֈړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyFrmMoveLeft( BOX2_SYS_WORK * syswk )
{
	syswk->get_pos = BOX2MAIN_GETPOS_NONE;
	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
	BOX2BGWFRM_PartyPokeFrameLeftMoveSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncPartyFrmLeft, BOX2SEQ_MAINSEQ_ARRANGE_PARTYFRM_MOVE_L_END );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X/�莝���{�^���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxMoveButtonOutSet( BOX2_SYS_WORK * syswk )
{
	syswk->box_mv_flg = 1;		// �{�b�N�X�ړ��t���O
	syswk->pokechg_mode = 0;

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

	BOX2BGWFRM_TemochiButtonOutSet( syswk->app->wfrm );
	BOX2BGWFRM_IdouButtonOutSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );

	return VFuncSet( syswk, BOX2MAIN_VFuncUnderButtonMove, BOX2SEQ_MAINSEQ_ARRANGE_BOXMOVE_FRMIN );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�ړ��I���J�n
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxMoveEndStart( BOX2_SYS_WORK * syswk )
{
	syswk->box_mv_flg = 0;		// �{�b�N�X�ړ��t���O

	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

	if( BOX2BGWFRM_BoxMoveButtonCheck(syswk->app->wfrm) == TRUE ){
		return BoxMoveButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXMOVE_FRMOUT_SET );
	}else if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
		return YStatusButtonFrmOut( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXMOVE_FRMOUT_SET );
	}

	return BoxMoveFrmOutSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXMOVE_FRMOUT );
}

//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^���t���[���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int YStatusButtonFrmOut( BOX2_SYS_WORK * syswk, int next )
{
	BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveButtonYStatusButtonMove, next );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�����肩����v�t���[���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxMoveMenuFrmOut( BOX2_SYS_WORK * syswk, u32 pos )
{
	int	next;

	syswk->box_mv_pos = pos;
	BOX2OBJ_BoxMoveNameSet( syswk );
	BOX2BMP_BoxMoveNameNumWrite( syswk );

	if( syswk->party_sel_flg == 0 ){
		next = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
	}else{
		next = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
	}
	return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveMenuOut, next );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�����肩����v�{�^���t���[���C��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxMoveButtonFrmIn( BOX2_SYS_WORK * syswk, u32 ret )
{
	int	next;
	u32	tmp;

	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		next = BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
	}else{
		next = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
	}

	// �g���C���Ɛ��\��
	tmp = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos) + ret;
	if( tmp != syswk->box_mv_pos ){
		syswk->box_mv_pos = tmp;
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2BMP_BoxMoveNameNumWrite( syswk );
	}

	// ���݂̃g���C�Ɠ���
	if( syswk->box_mv_pos == syswk->tray ){
		// �{�^�����\���ς݂Ȃ�
		if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == TRUE ){
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX );
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
			return BoxMoveButtonFrmOut( syswk, next );
		}
		if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX );
			CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
			BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
			return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveButtonYStatusButtonMove, next );
		}
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
		return next;
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_CHANGE );
	if( CURSORMOVE_CursorOnOffGet( syswk->app->cmwk ) == TRUE ){
		const CURSORMOVE_DATA * pw;
		u8	x, y;
		pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_CHANGE );
		x = pw->px;
		y = pw->py;
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );
	}

	if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == TRUE ){
		return next;
	}

	BOX2BMP_BoxMoveButtonPrint( syswk );

	if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
		BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
	}
	return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveButtonMove, next );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�����肩����v�{�^���t���[���A�E�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxMoveButtonFrmOut( BOX2_SYS_WORK * syswk, int next )
{
	BOX2BGWFRM_BoxMoveButtonOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveButtonYStatusButtonMove, next );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�ړ��E�g���C�؂�ւ��Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	mode	�X�N���[�����[�h
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxMoveTrayScrollSet( BOX2_SYS_WORK * syswk, u32 mode )
{
	void * func;
	u32	tmp;

	tmp = syswk->tray;
	syswk->tray = syswk->box_mv_pos;

	if( tmp > syswk->box_mv_pos ){
		BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
		BOX2MAIN_WallPaperSet(
			syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), BOX2MAIN_TRAY_SCROLL_L );
		func = BOX2MAIN_VFuncTrayScrollLeft;
	}else{
		BOX2OBJ_PokeIconBufLoad( syswk, syswk->tray );
		BOX2MAIN_WallPaperSet(
			syswk, BOX2MAIN_GetWallPaperNumber(syswk,syswk->tray), BOX2MAIN_TRAY_SCROLL_R );
		func = BOX2MAIN_VFuncTrayScrollRight;
	}

	switch( mode ){
	case 0:		// �ʏ�
		BOX2OBJ_BoxMoveCursorSet( syswk );
		if( syswk->party_sel_flg == 0 ){
			syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
		}else{
			syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN;
		}
		break;

	case 1:		// �{�b�N�X�e�[�}�ύX
		BOX2OBJ_BoxMoveCursorSet( syswk );
		syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_CHG_MAIN;
		break;

	case 2:		// �ǎ��ύX
		syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_WHITE_OUT;
		break;

	case 3:		// �|�P�����ړ�
		BOX2OBJ_BoxMoveCursorSet( syswk );
		if( BOX2BGWFRM_BoxMoveButtonCheck(syswk->app->wfrm) == TRUE ){
			BOX2BGWFRM_BoxMoveButtonOutSet( syswk->app->wfrm );
		}
		syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN;
		break;

	case 4:		// ����������
		BOX2OBJ_BoxMoveCursorSet( syswk );
		if( BOX2BGWFRM_BoxMoveButtonCheck(syswk->app->wfrm) == TRUE ){
			BOX2BGWFRM_BoxMoveButtonOutSet( syswk->app->wfrm );
		}
		syswk->next_seq = BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
		break;
	}

	return VFuncSet( syswk, func, syswk->next_seq );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X���\��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	ret		�I���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxMoveNameChange( BOX2_SYS_WORK * syswk, u32 ret )
{
	u32	tmp;

	tmp = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos) + ret;
	if( syswk->box_mv_pos == tmp ){
		return;
	}
	syswk->box_mv_pos = tmp;

	BOX2OBJ_BoxMoveNameSet( syswk );
	BOX2BMP_BoxMoveNameNumWrite( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�ړ��E�g���C�؂�ւ����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	mv		�ړ�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxMoveTrayIconChgOnly( BOX2_SYS_WORK * syswk, s8 mv )
{
	s32	pos;

	pos = syswk->box_mv_pos + BOX2OBJ_TRAYICON_MAX * mv;
	if( pos < 0 ){
		pos += syswk->trayMax;
	}else if( pos >= syswk->trayMax ){
		pos -= syswk->trayMax;
	}
	syswk->box_mv_pos = pos;

	BOX2OBJ_TrayIconChange( syswk );
	BOX2OBJ_BoxMoveNameSet( syswk );
	BOX2OBJ_BoxMoveCursorSet( syswk );
	BOX2BMP_BoxMoveNameNumWrite( syswk );

	if( mv > 0 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_RA, BOX2OBJ_ANM_R_ARROW_ON );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_LA, BOX2OBJ_ANM_L_ARROW_ON );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�ړ��t���[���C���Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	next	�t���[���ړ���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxMoveFrmInSet( BOX2_SYS_WORK * syswk, int next )
{
	BOX2OBJ_BoxMoveObjInit( syswk );
	BOX2OBJ_BoxMoveCursorSet( syswk );
	BOX2OBJ_BoxMoveNameSet( syswk );
	BOX2BMP_BoxMoveNameNumWrite( syswk );

	BOX2BGWFRM_BoxMoveFrmInSet( syswk->app->wfrm );

	return VFuncSet( syswk, BOX2MAIN_VFuncBoxMoveFrmIn, next );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�ړ��t���[���A�E�g�Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	next	�t���[���ړ���̃V�[�P���X
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
 * �u��߂�v�t���[���C���Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	next	�t���[���ړ���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxMoveExitButtonInSet( BOX2_SYS_WORK * syswk, int next )
{
	BOX2BMP_ButtonPutYameru( syswk );
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncExitButtonMove, next );
}

//--------------------------------------------------------------------------------------------
/**
 * �u��߂�vor�u���ǂ�v�t���[���A�E�g�Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	next	�t���[���ړ���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxMoveExitButtonOutSet( BOX2_SYS_WORK * syswk, int next )
{
	BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncExitButtonMove, next );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�A�E�g�Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	next	�t���[���ړ���̃V�[�P���X
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
 * �{�b�N�X�e�[�}�ύX�J�n�Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	next	�t���[���ړ���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxThemaChgStart( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

	syswk->app->old_cur_pos = syswk->tray % BOX2OBJ_TRAYICON_MAX;
	syswk->cur_rcv_pos = syswk->app->old_cur_pos;

	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
		return MainSeq_ArrangeBoxThemaButtonOut( syswk );
	}
	return PokeMenuOutSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_TRAY_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�e�[�}�ύX�g���C�A�C�R���I��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	ret		�I���ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxThemaTrayIconSel( BOX2_SYS_WORK * syswk, u32 ret )
{
	const CURSORMOVE_DATA * pw;
	u8	pos;
	u8	x, y;

	syswk->box_mv_pos = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos) + ret;
	BOX2OBJ_BoxMoveNameSet( syswk );
	BOX2BMP_BoxMoveNameNumWrite( syswk );

	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
	syswk->app->old_cur_pos = pos;
	CURSORMOVE_PosSetEx( syswk->app->cmwk, BOX2UI_BOXTHEMA_CHG_MOVE, pos, pos );
	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, BOX2UI_BOXTHEMA_CHG_MOVE );
	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );

	return BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�e�[�}�ύX�E�g���C�؂�ւ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	mv		�ړ�����
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxThemaTrayIconChg( BOX2_SYS_WORK * syswk, s8 mv )
{
	BoxMoveTrayIconChgOnly( syswk, mv );
	CURSORMOVE_PosSet( syswk->app->cmwk, (syswk->box_mv_pos%BOX2OBJ_TRAYICON_MAX) );
	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	return BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_CHG_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��y�[�W�؂�ւ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	mv		�ړ�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void WallPaperPageChange( BOX2_SYS_WORK * syswk, s32 mv )
{
	s32	pos;

	pos = syswk->app->wallpaper_pos + BOX2OBJ_WPICON_MAX * mv;

	if( pos < 0 ){
		pos += BOX_TOTAL_WALLPAPER_MAX;
	}else if( pos >= BOX_TOTAL_WALLPAPER_MAX ){
		pos -= BOX_TOTAL_WALLPAPER_MAX;
	}
	syswk->app->wallpaper_pos = pos;

	if( mv > 0 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_RA, BOX2OBJ_ANM_R_ARROW_ON );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_LA, BOX2OBJ_ANM_L_ARROW_ON );
	}

	// OBJ�ύX
	BOX2OBJ_WallPaperObjChange( syswk );
	// �J�[�\���ʒu
	BOX2OBJ_WallPaperCursorSet( syswk );
	// ���O�ύX
	BOX2BMP_WallPaperNameWrite( syswk );

	CURSORMOVE_PosSet( syswk->app->cmwk, (syswk->app->wallpaper_pos%BOX2OBJ_WPICON_MAX) );
	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��I���ʒu�ύX
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�I���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void WallPaperPosSet( BOX2_SYS_WORK * syswk, u32 pos )
{
	const CURSORMOVE_DATA * pw;
	u8	x, y;

	syswk->app->wallpaper_pos = syswk->app->wallpaper_pos / BOX2OBJ_WPICON_MAX * BOX2OBJ_WPICON_MAX + pos;
	BOX2OBJ_WallPaperNameSet( syswk );
	BOX2BMP_WallPaperNameWrite( syswk );

	syswk->app->old_cur_pos = CURSORMOVE_PosGet( syswk->app->cmwk );
	CURSORMOVE_PosSetEx( syswk->app->cmwk, BOX2UI_WALLPAPER_CHG_ENTER, syswk->app->old_cur_pos, syswk->app->old_cur_pos );
	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, BOX2UI_WALLPAPER_CHG_ENTER );
	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int WallPaperChange( BOX2_SYS_WORK * syswk )
{
	if( syswk->app->wallpaper_pos >= BOX_NORMAL_WALLPAPER_MAX ){
		if( BOXDAT_GetDaisukiKabegamiFlag( syswk->dat->sv_box, syswk->app->wallpaper_pos-BOX_NORMAL_WALLPAPER_MAX ) == FALSE ){
//			Snd_SePlay( SE_BOX2_DISABLE_BUTTON );
			return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_BOXMV_MENU, BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_MAIN );
		}
	}

	if( syswk->app->wallpaper_pos == BOX2MAIN_GetWallPaperNumber(syswk,syswk->box_mv_pos) ){
//		Snd_SePlay( SE_BOX2_DISABLE_BUTTON );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_BOXMV_MENU, BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_MAIN );
	}
	if( syswk->box_mv_pos != syswk->tray ){
//		Snd_SePlay( SE_BOX2_CHANGE_TRAY );
		return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_BOXMV_MENU, BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_BUTTON );
	}
//	Snd_SePlay( SE_BOX2_DECIDE );
	return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_BOXMV_MENU, BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_WHITE_OUT );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������擾�i�|�P��������Ă���/��������j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyInPokeCheck( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

	// �莝���ɋ󂫂����邩
	if( PokeParty_GetPokeCount( syswk->dat->pokeparty ) != TEMOTI_POKEMAX ){
		BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG3 );
		BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
		return VFuncSet( syswk, BOX2MAIN_VFuncPokeMenuMove, BOX2SEQ_MAINSEQ_PARTYIN_PARTY_FRM_IN );
	}
	BOX2BGWFRM_PokeMenuOff( syswk->app->wfrm );
	BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_ERR, BOX2BMPWIN_ID_MSG1 );
	syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
	return BOX2SEQ_MAINSEQ_TRGWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����a���郁�j���[�J�n
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyOutInit( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG3 );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncPokeFreeMenuMove, BOX2SEQ_MAINSEQ_PARTYOUT_START_CHECK );
}

//--------------------------------------------------------------------------------------------
/**
 * �a����g���C����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	ret		�w��ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PokeOutTraySelect( BOX2_SYS_WORK * syswk, u32 ret )
{
	const CURSORMOVE_DATA * pw;
	u8	pos;
	u8	x, y;

	syswk->box_mv_pos = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos) + ret;

	if( GFL_UI_TP_GetTrg() == FALSE ){
//��[GS_CONVERT_TAG]
		if( BOXDAT_GetPokeExistCount( syswk->dat->sv_box, syswk->box_mv_pos ) == BOX_MAX_POS ){
//			Snd_SePlay( SE_BOX2_DISABLE_BUTTON );
		}else{
//			Snd_SePlay( SE_BOX2_DECIDE );
		}
		return BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_BUTTON;
	}

//	Snd_SePlay( SE_BOX2_DECIDE );

	BOX2OBJ_BoxMoveNameSet( syswk );
	BOX2BMP_BoxMoveNameNumWrite( syswk );

	pos = CURSORMOVE_PosGet( syswk->app->cmwk );
	syswk->app->old_cur_pos = pos;
	CURSORMOVE_PosSetEx( syswk->app->cmwk, BOX2UI_PTOUT_BOXSEL_ENTER, pos, pos );
	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, BOX2UI_PTOUT_BOXSEL_ENTER );
	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );

	return BOX2SEQ_MAINSEQ_PARTYOUT_BOXSELECT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * �a����g���C�؂�ւ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	mv		�؂�ւ�����
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PokeOutTrayIconChg( BOX2_SYS_WORK * syswk, s8 mv )
{
	BoxMoveTrayIconChgOnly( syswk, mv );
	CURSORMOVE_PosSet( syswk->app->cmwk, (syswk->box_mv_pos%BOX2OBJ_TRAYICON_MAX) );
	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
	return BOX2SEQ_MAINSEQ_PARTYOUT_BOXSELECT_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * �a����u��߂�v�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	flg		�����g�p
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyOutEnd( BOX2_SYS_WORK * syswk, BOOL flg )
{
	return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_RET_BTN, BOX2SEQ_MAINSEQ_PARTYOUT_BOXMENU_FRM_OUT );
}

//--------------------------------------------------------------------------------------------
/**
 * �a������s
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyOutEnter( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );

	if( BOXDAT_GetPokeExistCount( syswk->dat->sv_box, syswk->box_mv_pos ) == BOX_MAX_POS ){
//		Snd_SePlay( SE_BOX2_DISABLE_BUTTON );
	}else{
//		Snd_SePlay( SE_BOX2_DECIDE );
	}
	return BgButtonAnmSet( syswk, BOX2MAIN_WINFRM_BOXMV_MENU, BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_BUTTON );
}

//--------------------------------------------------------------------------------------------
/**
 * ��J�[�\���Ń|�P�����擾
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

	syswk->app->poke_get_key = 1;

	BOX2OBJ_PokeIconPriChg( syswk->app, syswk->get_pos, BOX2OBJ_POKEICON_PRI_CHG_GET );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );

	if( syswk->quick_mode == 0 ){
		BOX2BGWFRM_SubDispWazaFrmInSet( syswk->app );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncPokeMoveGetKey, BOX2SEQ_MAINSEQ_KEYGET_POKEMOVE_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * ��J�[�\���Ń|�P�����z�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�z�u�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PokeMovePutKey( BOX2_SYS_WORK * syswk, u32 pos )
{
	if( pos == BOX2MAIN_GETPOS_NONE ){
		syswk->app->cancel_flg = TRUE;
	}else{
		syswk->app->cancel_flg = FALSE;
	}

	{	// �|�P�����A�C�R���̈ʒu��␳����
		s16	x, y;
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, &x, &y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_SetPos(
			syswk->app, syswk->app->pokeicon_id[syswk->get_pos], x, y+4, CLSYS_DEFREND_MAIN );
	}

	// ���݂̃g���Cor�莝���ɔz�u
	if( pos < BOX2OBJ_POKEICON_MAX ){
		// �莝��
		if( pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
			u32	ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );
			if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
				if( ( pos - BOX2OBJ_POKEICON_TRAY_MAX ) > ppcnt ){
					BOX2MAIN_PokeInfoOff( syswk );
					BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
				}else{
					BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
				}
			}else{
				if( ( pos - BOX2OBJ_POKEICON_TRAY_MAX ) >= ppcnt ){
					BOX2MAIN_PokeInfoOff( syswk );
					BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
				}else{
					BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
				}
			}
		// ���݂̃g���C
		}else{
			BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
		}
	// �ʃg���C
	}else if( pos != BOX2MAIN_GETPOS_NONE ){
		syswk->box_mv_pos = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos) + pos - BOX2UI_ARRANGE_MOVE_TRAY1;
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2BMP_BoxMoveNameNumWrite( syswk );
		if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
			u32	party_pos;
			POKEMON_PARAM * pp;

			party_pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			pp = PokeParty_GetMemberPointer( syswk->dat->pokeparty, party_pos );

			if( ITEM_CheckMail( PP_Get(pp,ID_PARA_item,NULL) ) == TRUE ){
				BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
/*	�J�X�^���{�[���͂Ȃ��n�Y
			}else if( PP_Get( pp, ID_PARA_cb_id, NULL ) != 0 ){
				BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
*/
			}else if( BOX2MAIN_BattlePokeCheck( syswk, party_pos ) == FALSE ){
				BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
			}else if( syswk->box_mv_pos == syswk->tray ){
				BOX2MAIN_PokeInfoOff( syswk );
				BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
			}else if( BOXDAT_GetEmptySpaceTray( syswk->dat->sv_box, syswk->box_mv_pos ) == 0 ){
				BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
			}else{
				BOX2MAIN_PokeInfoOff( syswk );
				BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
			}
		}else if( syswk->box_mv_pos == syswk->tray ){
			BOX2MAIN_PokeInfoOff( syswk );
			BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
		}else if( BOXDAT_GetEmptySpaceTray( syswk->dat->sv_box, syswk->box_mv_pos ) == 0 ){
			BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
		}else{
			BOX2MAIN_PokeInfoOff( syswk );
			BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
		}
	// �L�����Z��
	}else{
		u8	cur_pos = CURSORMOVE_PosGet( syswk->app->cmwk );
		if( cur_pos < BOX2UI_ARRANGE_MOVE_RETURN ){
			if( BOX2MAIN_PokeInfoPut( syswk, cur_pos ) == FALSE ){
				BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
			}
		}else{
			BOX2MAIN_PokeInfoOff( syswk );
			BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
		}
	}

	syswk->app->poke_put_key = pos;
	syswk->app->poke_get_key = 0;

	PokeMoveWorkAlloc( syswk );
	syswk->move_mode = BOX2MAIN_POKEMOVE_MODE_ALL;
	return VFuncSet( syswk, BOX2MAIN_VFuncPokeMovePutKey, BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTYPOKE_CHG_END_KEY );
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�̃A�C�e�����擾�i�L�[����j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayItemGetKey( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;

	syswk->app->get_item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_item, NULL );

	BOX2BMP_ItemArrangeMenuStrPrint( syswk, syswk->app->get_item );

	BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
	BOX2BGWFRM_TemochiButtonOutSet( syswk->app->wfrm );
	BOX2BGWFRM_IdouButtonOutSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );

	if( syswk->app->get_item != ITEM_DUMMY_DATA ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_TRAY );
		BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
		BOX2OBJ_ItemIconCursorAdd( syswk->app );

		BOX2BGWFRM_SubDispItemFrmInSet( syswk->app->wfrm );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeGetKey, BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN_CURMOVE_RET );
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�̃A�C�e�����擾�i�^�b�`�j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int TrayItemGet( BOX2_SYS_WORK * syswk, u32 pos )
{
	u16	tmp;

	tmp = syswk->app->get_item;

	syswk->get_pos = pos;

	syswk->app->get_item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_item, NULL );

	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
		if( tmp != 0 || syswk->app->get_item != 0 ){
			BOX2BGWFRM_PokeMenuOff( syswk->app->wfrm );
		}else if( syswk->app->get_item != 0 ){
			BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		}
	}else{
		BOX2BGWFRM_TemochiButtonOutSet( syswk->app->wfrm );
		BOX2BGWFRM_IdouButtonOutSet( syswk->app->wfrm );
		BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );
	}
	BOX2BMP_ItemArrangeMenuStrPrint( syswk, syswk->app->get_item );

	if( syswk->app->get_item != ITEM_DUMMY_DATA ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_TRAY );
		BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
		BOX2OBJ_ItemIconCursorAdd( syswk->app );
		BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, TRUE );
		BOX2BGWFRM_SubDispItemFrmInSet( syswk->app->wfrm );
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

	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeGetTouch, BOX2SEQ_MAINSEQ_ITEMARRANGE_ICONMOVE_RET );
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C/�莝���A�C�e�����擾
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxPartyItemGet( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;

	syswk->app->get_item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_item, NULL );

	BOX2BMP_ButtonPutYStatus( syswk );

	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		CURSORMOVE_PosSet( syswk->app->cmwk, syswk->get_pos );
		CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );
		if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
			return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
		}else if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == TRUE ){
			BOX2BGWFRM_BoxMoveButtonOutSet( syswk->app->wfrm );
		}
		return VFuncSet( syswk, BOX2MAIN_VFuncYStatusButtonMove, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN );
	}

//	Snd_SePlay( SE_BOX2_POKE_CATCH );

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
	BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_ALL );
	BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
	BOX2OBJ_ItemIconCursorAdd( syswk->app );

	BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, TRUE );

	BOX2BGWFRM_SubDispItemFrmInSet( syswk->app->wfrm );

	if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == TRUE ){
		BOX2BGWFRM_BoxMoveButtonOutSet( syswk->app->wfrm );
	}else if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
		BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
	}

	{
		s16	x, y;

		BOX2OBJ_PokeIconDefaultPosGet( syswk->get_pos, &x, &y, BOX2MAIN_POKEMOVE_MODE_ALL );
		syswk->app->tpx = x + 8;
		syswk->app->tpy = y + 8;
	}

	ItemMoveWorkAlloc( syswk );

	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeBoxPartyIconSetTouch, BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_ICONMOVE_SUBFRM_OUT );
}

//--------------------------------------------------------------------------------------------
/**
 * ��J�[�\���Ńg���C/�莝���A�C�e�����擾
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxPartyItemGetKey( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;
	syswk->app->get_item_init_pos = pos;

	syswk->app->get_item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_item, NULL );

	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN;
	}

//	Snd_SePlay( SE_BOX2_POKE_CATCH );

	syswk->app->poke_get_key = 1;

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
	BOX2OBJ_ItemIconPokePutHand( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_ALL );
	BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );

	BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, TRUE );

	BOX2BGWFRM_SubDispItemFrmInSet( syswk->app->wfrm );

	if( BOX2BGWFRM_YStatusButtonCheck( syswk->app->wfrm ) == TRUE ){
		BOX2BGWFRM_YStatusButtonOutSet( syswk->app->wfrm );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeBoxPartyIconGetKey, BOX2SEQ_MAINSEQ_ITEMARRANGE_KEYGET_MOVE_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * ��J�[�\���ŃA�C�e����z�u�E�L�����Z������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxPartyItemPutKeyCancel( BOX2_SYS_WORK * syswk )
{
	u32	pos = CURSORMOVE_PosGet( syswk->app->cmwk );

	if( pos < BOX2UI_ARRANGE_MOVE_RETURN ){
		if( BOX2MAIN_PokeInfoPut( syswk, pos ) == TRUE ){
			BOX2BGWFRM_YStatusButtonInSet( syswk->app->wfrm );
		}
	}else{
		BOX2MAIN_PokeInfoOff( syswk );
	}
	ItemMoveWorkAlloc( syswk );
	return VFuncSet(
			syswk,
			BOX2MAIN_VFuncItemArrangeBoxPartyIconPutKeyCancel,
			BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_ICONMOVE_END );
}

//--------------------------------------------------------------------------------------------
/**
 * ��J�[�\���ŃA�C�e����z�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�z�u�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxPartyItemPutKey( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->app->poke_put_key = pos;
	syswk->app->poke_get_key = 0;

	{	// �A�C�e���A�C�R���̈ʒu��␳����
		s16	x, y;
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, &x, &y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_ITEMICON, x, y+8, CLSYS_DEFREND_MAIN );
	}

	if( syswk->app->poke_put_key == BOX2MAIN_GETPOS_NONE ){
		return BoxPartyItemPutKeyCancel( syswk );
	}else if( BOX2MAIN_PokeItemMoveCheck( syswk, syswk->get_pos, pos ) == FALSE ){
		return BoxPartyItemPutKeyCancel( syswk );
	}

	BOX2BGWFRM_SubDispItemFrmOutSet( syswk->app->wfrm );

	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeBoxPartyIconPutKey, BOX2SEQ_MAINSEQ_ITEMARRANGE_KEYGET_CHANGE_CHECK );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����̃A�C�e�����擾�i�L�[����E�����������j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyItemGetKey( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->get_pos = pos;

	syswk->app->get_item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_item, NULL );

	BOX2BMP_ItemArrangeMenuStrPrint( syswk, syswk->app->get_item );

	BOX2BGWFRM_PokeMenuInSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );

	if( syswk->app->get_item != ITEM_DUMMY_DATA ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_PARTY );
		BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
		BOX2OBJ_ItemIconCursorAdd( syswk->app );
		BOX2BGWFRM_SubDispItemFrmInSet( syswk->app->wfrm );
	}

	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeGetKey, BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN_CURMOVE_RET );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����̃A�C�e�����擾�i�^�b�`�j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�擾�ʒu
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyItemGet( BOX2_SYS_WORK * syswk, u32 pos )
{
	u16	tmp;

	syswk->get_pos = pos;

	tmp = syswk->app->get_item;
	syswk->app->get_item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_item, NULL );

	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == TRUE ){
		if( tmp != 0 || syswk->app->get_item != 0 ){
			BOX2BGWFRM_PokeMenuOff( syswk->app->wfrm );
		}else if( syswk->app->get_item != 0 ){
			BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
		}
	}else{
		BOX2BGWFRM_RetButtonOutSet( syswk->app->wfrm );
	}
	BOX2BMP_ItemArrangeMenuStrPrint( syswk, syswk->app->get_item );

	if( syswk->app->get_item != ITEM_DUMMY_DATA ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
		BOX2OBJ_ItemIconPokePut( syswk->app, syswk->get_pos, BOX2MAIN_POKEMOVE_MODE_PARTY );
		BOX2OBJ_ItemIconChange( syswk->app, syswk->app->get_item );
		BOX2OBJ_ItemIconCursorAdd( syswk->app );
		BOX2OBJ_PokeIconBlendSet( syswk->app, syswk->get_pos, TRUE );
		BOX2BGWFRM_SubDispItemFrmInSet( syswk->app->wfrm );
		if( ITEM_CheckMail( syswk->app->get_item ) == TRUE ){
			BOX2BMP_VBlankMsgSet( syswk, BOX2BMPWIN_ID_MSG4, BOX2BMP_MSGID_VBLANK_MAIL_MOVE );
		}else{
			BOX2BMP_VBlankMsgSet( syswk, BOX2BMPWIN_ID_MSG4, BOX2BMP_MSGID_VBLANK_ITEM_MOVE );
		}
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

	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangePartyGetTouch, BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_RET );
}

//--------------------------------------------------------------------------------------------
/**
 * �����������E���j���[����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int ItemArrangeMenuStart( BOX2_SYS_WORK * syswk )
{
	int	next;

	if( syswk->app->get_item == ITEM_DUMMY_DATA ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		{
			POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );
			if( PPP_Get( ppp, ID_PARA_tamago_flag, NULL ) == 0 ){
				syswk->cur_rcv_pos = BOX2UI_ITEM_A_MAIN_MENU1;
				return SubProcSet( syswk, SUB_PROC_TYPE_BAG );
			}
		}
		next = BOX2SEQ_MAINSEQ_ITEMARRANGE_GET_BAG_CHECK;
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
		next = BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_CHECK;
	}

	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	return VFuncSet( syswk, BOX2MAIN_VFuncPokeFreeMenuMove, next );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R�����샏�[�N�擾
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemMoveWorkAlloc( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_ITEMMOVE_WORK * work;

	work = GFL_HEAP_AllocMemory( HEAPID_BOX_APP, sizeof(BOX2MAIN_ITEMMOVE_WORK) );
//��[GS_CONVERT_TAG]

	syswk->app->vfunk.work = work;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R�����샏�[�N���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemMoveWorkFree( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
}


//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F��J�[�\���Ŏ擾���̃|�P�������L�[�ňړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_KeyGetPokeMoveMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	// �x�X�e�[�^�X�{�^��
	if( BOX2UI_YStatusButtonCheck( syswk ) == TRUE ){
//		Snd_SePlay( SE_BOX2_DECIDE );
		syswk->y_status_flg  = 1;
		syswk->y_status_hand = 1;
		syswk->y_status_pos  = CURSORMOVE_PosGet( syswk->app->cmwk );
		return YStatusButtonAnmSet( syswk, BOX2MAIN_WINFRM_Y_ST_BTN, BOX2SEQ_MAINSEQ_STATUS_BUTTON );
	}

	ret = BOX2UI_BoxArrangePokeMoveHand( syswk );

	if( CURSORMOVE_CursorOnOffGet( syswk->app->cmwk ) == FALSE ){
		ret = CURSORMOVE_CANCEL;
	}

#ifdef	BTS_2299_TEST
	if( test_flg_2299 != 0 ){
		ret = CURSORMOVE_CANCEL;
	}
#endif	// BTS_2299_TEST

	switch( ret ){
	case CURSORMOVE_CURSOR_MOVE:	// �ړ�
//		Snd_SePlay( SE_BOX2_MOVE_CURSOR );
		return VFuncReqSet( syswk, BOX2SEQ_MAINSEQ_KEYGET_POKEMOVE_MAIN_CURMOVE_RET );

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
	case CURSORMOVE_NONE:			// ����Ȃ�
		break;

	case BOX2UI_ARRANGE_MOVE_RETURN:	// 36: ��߂�
	case CURSORMOVE_CANCEL:				// �L�����Z��
		return PokeMovePutKey( syswk, BOX2MAIN_GETPOS_NONE );

	case BOX2UI_ARRANGE_MOVE_TRAY1:	// 37: �g���C�A�C�R��
	case BOX2UI_ARRANGE_MOVE_TRAY2:	// 38: �g���C�A�C�R��
	case BOX2UI_ARRANGE_MOVE_TRAY3:	// 39: �g���C�A�C�R��
	case BOX2UI_ARRANGE_MOVE_TRAY4:	// 40: �g���C�A�C�R��
	case BOX2UI_ARRANGE_MOVE_TRAY5:	// 41: �g���C�A�C�R��
	case BOX2UI_ARRANGE_MOVE_TRAY6:	// 42: �g���C�A�C�R��
	default:
		return PokeMovePutKey( syswk, ret );
	}

	return BOX2SEQ_MAINSEQ_KEYGET_POKEMOVE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F��擾���̃|�P�����ړ���ɃL�[����ɖ߂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_KeyGetPokeMoveMainCurMoveRet( BOX2_SYS_WORK * syswk )
{
	GFL_HEAP_FreeMemory( syswk->app->vfunk.work );
//��[GS_CONVERT_TAG]
	return BOX2SEQ_MAINSEQ_KEYGET_POKEMOVE_MAIN;
}


//============================================================================================
//	�͂��E������
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �����a����F�͂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxArg_ItemGetYes( BOX2_SYS_WORK * syswk )
{
	u32 item = BOX2MAIN_PokeParaGet( syswk, syswk->get_pos, ID_PARA_item, NULL );

	// �A�C�e���`�F�b�N
	if( MYITEM_AddItem( syswk->dat->myitem, item, 1, HEAPID_BOX_APP ) == TRUE ){
		POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );

		BOX2BMP_ItemGetMsgPut( syswk, item, BOX2BMPWIN_ID_MSG1 );
		item = 0;
		BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, ID_PARA_item, item );
		if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
			BOX2OBJ_PokeIconChange(
				syswk, syswk->tray, syswk->get_pos,
				syswk->app->pokeicon_id[syswk->get_pos] );
		}
		BOX2MAIN_PokeInfoRewrite( syswk, syswk->get_pos );
	}else{
//		Snd_SePlay( SE_BOX2_WARNING );
		BOX2BMP_ItemGetErrorPut( syswk, BOX2BMPWIN_ID_MSG1 );
	}
	syswk->next_seq = BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV;
	return BOX2SEQ_MAINSEQ_TRGWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����𓦂����F�͂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxArg_PokeFreeYes( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2MAIN_PokeFreeCreate( syswk );
	BOX2OBJ_PokeIconFreeStart( syswk->app->seqwk );

	BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );

	return BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * �o�b�O�֖߂��F�͂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxItemArg_RetBagYes( BOX2_SYS_WORK * syswk )
{
	u16	item;

	if( MYITEM_AddItem( syswk->dat->myitem, syswk->app->get_item, 1, HEAPID_BOX_APP ) == FALSE ){
//		Snd_SePlay( SE_BOX2_WARNING );
		BOX2BMP_ItemArrangeMsgPut( syswk, BOX2BMP_MSGID_ITEM_A_MAX, BOX2BMPWIN_ID_MSG1 );
		syswk->next_seq = BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_CANCEL;
		return BOX2SEQ_MAINSEQ_TRGWAIT;
	}

	item = ITEM_DUMMY_DATA;
	BOX2MAIN_PokeParaPut( syswk, syswk->get_pos, ID_PARA_item, item );

	{
		POKEMON_PASO_PARAM * ppp = BOX2MAIN_PPPGet( syswk, syswk->tray, syswk->get_pos );
		if( BOX2MAIN_PokeItemFormChange( ppp ) == TRUE ){
			BOX2OBJ_PokeIconChange(
				syswk, syswk->tray, syswk->get_pos,
				syswk->app->pokeicon_id[syswk->get_pos] );
		}
		BOX2MAIN_PokeInfoPut( syswk, syswk->get_pos );
	}

	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	BOX2OBJ_ItemIconAffineSet( syswk->app, TRUE );
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_ITEMICON, 2 );
	BOX2BGWFRM_SubDispItemFrmOutSet( syswk->app->wfrm );
	return BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_ENTER;
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�I���F�͂�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxArg_BoxEndYes( BOX2_SYS_WORK * syswk )
{
//	Snd_SePlay( SE_BOX2_LOG_OFF );
	syswk->next_seq = BOX2SEQ_MAINSEQ_END;
	return FadeOutSet( syswk, BOX2SEQ_MAINSEQ_RELEASE );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�I���F������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BoxArg_BoxEndNo( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG1 );
	CURSORMOVE_CursorOnOffSet( syswk->app->cmwk, TRUE );

	switch( syswk->dat->callMode ){
	case BOX_MODE_AZUKERU:		// ��������
		BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYPOKE, FALSE );
		return BOX2SEQ_MAINSEQ_PARTYOUT_MAIN;

	case BOX_MODE_TURETEIKU:	// ��Ă���
		BOX2BMP_PokePartyInMsgPut( syswk, 0, BOX2BMP_MSGID_PARTYIN_INIT, BOX2BMPWIN_ID_MSG3 );
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
		return BOX2SEQ_MAINSEQ_PARTYIN_MAIN;

	case BOX_MODE_SEIRI:		// ������
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
		return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;

	case BOX_MODE_ITEM:			// �ǂ���������
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, TRUE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, FALSE );
		return BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN;
	}

	return BOX2SEQ_MAINSEQ_ARRANGE_MAIN;
}


//============================================================================================
//	�{�^���֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �ʏ�̂a�f�{�^���A�j���Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	wfrmID	�a�f�E�B���h�E�t���[���h�c
 * @param	next	�{�^���A�j����̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BgButtonAnmSet( BOX2_SYS_WORK * syswk, u32 wfrmID, int next )
{
	u16	sx, sy;
	s8	px, py;

	BGWINFRM_PosGet( syswk->app->wfrm, wfrmID, &px, &py );
	BGWINFRM_SizeGet( syswk->app->wfrm, wfrmID, &sx, &sy );

	syswk->app->bawk.btn_mode = BOX2MAIN_BTN_ANM_MODE_BG;
	syswk->app->bawk.btn_id   = BGWINFRM_BGFrameGet( syswk->app->wfrm, wfrmID );
	syswk->app->bawk.btn_pal1 = BOX2MAIN_BG_PAL_SELWIN + 1;
	syswk->app->bawk.btn_pal2 = BOX2MAIN_BG_PAL_SELWIN;
	syswk->app->bawk.btn_seq  = 0;
	syswk->app->bawk.btn_cnt  = 0;

	syswk->app->bawk.btn_px   = px;
	syswk->app->bawk.btn_py   = py;
	syswk->app->bawk.btn_sx   = sx;
	syswk->app->bawk.btn_sy   = sy;

	syswk->next_seq = next;
	return BOX2SEQ_MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�p�̂a�f�{�^���A�j���Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	wfrmID	�a�f�E�B���h�E�t���[���h�c
 * @param	next	�{�^���A�j����̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int YStatusButtonAnmSet( BOX2_SYS_WORK * syswk, u32 wfrmID, int next )
{
	u16	sx, sy;
	s8	px, py;

	BGWINFRM_PosGet( syswk->app->wfrm, wfrmID, &px, &py );
	BGWINFRM_SizeGet( syswk->app->wfrm, wfrmID, &sx, &sy );

	syswk->app->bawk.btn_mode = BOX2MAIN_BTN_ANM_MODE_BG;
	syswk->app->bawk.btn_id   = BGWINFRM_BGFrameGet( syswk->app->wfrm, wfrmID );
	syswk->app->bawk.btn_pal1 = 3;
	syswk->app->bawk.btn_pal2 = 2;
	syswk->app->bawk.btn_seq  = 0;
	syswk->app->bawk.btn_cnt  = 0;

	syswk->app->bawk.btn_px   = px;
	syswk->app->bawk.btn_py   = py;
	syswk->app->bawk.btn_sx   = sx;
	syswk->app->bawk.btn_sy   = sy;

	syswk->next_seq = next;
	return BOX2SEQ_MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * �}�[�L���O�t���[���̂a�f�{�^���A�j���Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	no		�{�^���ԍ�
 * @param	next	�{�^���A�j����̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MarkingButtonAnmSet( BOX2_SYS_WORK * syswk, u32 no, int next )
{
	syswk->app->bawk.btn_mode = BOX2MAIN_BTN_ANM_MODE_BG;
	syswk->app->bawk.btn_id   = BGWINFRM_BGFrameGet( syswk->app->wfrm, BOX2MAIN_WINFRM_MARK );
	syswk->app->bawk.btn_pal1 = 3;
	syswk->app->bawk.btn_pal2 = 2;
	syswk->app->bawk.btn_seq  = 0;
	syswk->app->bawk.btn_cnt  = 0;

	if( no == 0 ){
		syswk->app->bawk.btn_py = 16;
	}else{
		syswk->app->bawk.btn_py = 20;
	}
	syswk->app->bawk.btn_px = 22;
	syswk->app->bawk.btn_sx = 9;
	syswk->app->bawk.btn_sy = 4;

	syswk->next_seq = next;
	return BOX2SEQ_MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���t���[���̂a�f�{�^���A�j���Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	no		�{�^���ԍ�
 * @param	next	�{�^���A�j����̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PartyButtonAnmSet( BOX2_SYS_WORK * syswk, u32 no, int next )
{
	syswk->app->bawk.btn_mode = BOX2MAIN_BTN_ANM_MODE_BG;
	syswk->app->bawk.btn_id   = BGWINFRM_BGFrameGet( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY );
	syswk->app->bawk.btn_pal1 = 3;
	syswk->app->bawk.btn_pal2 = 2;
	syswk->app->bawk.btn_seq  = 0;
	syswk->app->bawk.btn_cnt  = 0;

	if( no == 0 ){
		syswk->app->bawk.btn_px = 3;
	}else{
		syswk->app->bawk.btn_px = 22;
	}
	syswk->app->bawk.btn_py = 20;
	syswk->app->bawk.btn_sx = 9;
	syswk->app->bawk.btn_sy = 4;

	syswk->next_seq = next;
	return BOX2SEQ_MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�I���m�F���̃p�b�V�u�Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
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
	case BOX_MODE_SEIRI:		// ������
	case BOX_MODE_ITEM:			// �ǂ���������
		BOX2OBJ_TrayMoveArrowVanish( syswk->app, FALSE );
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�I���u�͂��E�������v�Z�b�g�i�{�^���j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxEndInit( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	BOX2BMP_BoxEndMsgPut( syswk, BOX2BMPWIN_ID_MSG1, TRUE );
	BoxEndPassiveSet( syswk );
	return YesNoSet( syswk, YESNO_ID_BOX_END );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�I���u�͂��E�������v�Z�b�g�i�L�����Z���j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxEndCancelInit( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	BOX2BMP_BoxEndMsgPut( syswk, BOX2BMPWIN_ID_MSG1, FALSE );
	BoxEndPassiveSet( syswk );
	return YesNoSet( syswk, YESNO_ID_BOX_END_CANCEL );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�{�b�N�X������v�莝���|�P�����{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyButton( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_PokeSelectOff( syswk );
	return BoxPartyButtonOutSet( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�{�b�N�X������v�|�P�����ړ��{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeMoveButton( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_PokeSelectOff( syswk );
	return BoxMoveButtonOutSet( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P�����ړ��v���j���[�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeMoveMenuButton( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );

	syswk->box_mv_flg = 1;		// �{�b�N�X�ړ��t���O
	syswk->pokechg_mode = 0;

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );

	return PokeMenuOutSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_BOXMOVE_FRMIN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�悤�����݂�v�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_StatusButton( BOX2_SYS_WORK * syswk )
{
	return SubProcSet( syswk, SUB_PROC_TYPE_POKESTATUS );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�������́v�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemButton( BOX2_SYS_WORK * syswk )
{
	return PokeItemCheck( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�}�[�L���O�v�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MarkingButton( BOX2_SYS_WORK * syswk )
{
	return MarkingStartSet( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�}�[�L���O�v�I���{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MarkingEndButton( BOX2_SYS_WORK * syswk )
{
	return MarkingScrollOutSet( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�ɂ����v�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_FreeButton( BOX2_SYS_WORK * syswk )
{
	return PokeFreeStartSet( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�{�b�N�X������v����{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeCloseButton( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	return VFuncSet( syswk, BOX2MAIN_VFuncMenuCloseKeyArrange, BOX2SEQ_MAINSEQ_ARRANGE_MAIN_CURMOVE_RET );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���|�P�����u���ꂩ���v�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyChgButton( BOX2_SYS_WORK * syswk )
{
	if( syswk->quick_get == 0 ){
		BOX2MAIN_PokeSelectOff( syswk );
	}else{
		BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	}
	return PartyFrmMoveRight( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���|�P��������{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangePartyCloseButton( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	return VFuncSet( syswk, BOX2MAIN_VFuncMenuCloseKeyArrange, BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN_CURMOVE_RET );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�莝���|�P�����u��߂�v�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyEndButton( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_PokeSelectOff( syswk );
	return PartyFrmOutSet( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�{�b�N�X������v�{�b�N�X�؂�ւ��{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeBoxMoveButton( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	return BoxMoveTrayScrollSet( syswk, 3 );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�|�P��������ւ��I���{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxMoveEndButton( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_PokeSelectOff( syswk );
	return BoxMoveEndStart( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�e�[�}�ݒ�u�{�b�N�X�����肩����v�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaTrayChgButton( BOX2_SYS_WORK * syswk )
{
	return BoxMoveTrayScrollSet( syswk, 1 );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�e�[�}�ݒ�u���ׂ��݁v�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaWallPaperButton( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG3 );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, TRUE );
	return BoxMoveFrmOutSet( syswk, BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_FRM_IN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�e�[�}�ݒ�u�Ȃ܂��v�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BoxThemaNameButton( BOX2_SYS_WORK * syswk )
{
	return SubProcSet( syswk, SUB_PROC_TYPE_NAMEIN );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�e�[�}�ݒ�u���ׂ��݂��͂肩����v�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeWallPaperChgButton( BOX2_SYS_WORK * syswk )
{
	return BoxMoveTrayScrollSet( syswk, 2 );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�{�b�N�X�e�[�}�ݒ�ǎ��u��߂�v�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ArrangeWallPaperChgCancelButton( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_SysWinVanish( syswk->app, BOX2BMPWIN_ID_MSG3 );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYITEM, FALSE );
	}else{
		BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_TRAYPOKE, FALSE );
	}
	syswk->app->old_cur_pos = syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;
	syswk->cur_rcv_pos = BOX2UI_BOXTHEMA_CHG_WP;
	return BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_MENU_OUT;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������Ă����v�A��čs���{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInButton( BOX2_SYS_WORK * syswk )
{
	return PartyInPokeCheck( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������Ă����v����{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyInCloseButton( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	return VFuncSet( syswk, BOX2MAIN_VFuncMenuCloseKey, BOX2SEQ_MAINSEQ_PARTYIN_MENU_CLOSE_END );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������������v�a����{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutButton( BOX2_SYS_WORK * syswk )
{
	return PartyOutInit( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�|�P��������������v����{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PartyOutCloseButton( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_ButtonPutModoru( syswk );
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_SubDispWazaFrmOutSet( syswk->app );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	return VFuncSet( syswk, BOX2MAIN_VFuncMenuCloseKey, BOX2SEQ_MAINSEQ_PARTYOUT_MENU_CLOSE_END );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�������̂�����v�莝���|�P�����{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangePartyButton( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_PokeSelectOff( syswk );
	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
	return BoxPartyButtonOutSet( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�������̂�����v�|�P�����ړ��{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeMoveButton( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_PokeSelectOff( syswk );
	BOX2OBJ_PokeIconBlendSetAll( syswk, BOX2OBJ_BLENDTYPE_PARTYITEM, TRUE );
	return BoxMoveButtonOutSet( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�������̂�����v���j���[�{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeBagButton( BOX2_SYS_WORK * syswk )
{
	return ItemArrangeMenuStart( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�������̂�����v����{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeCloseButton( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2BGWFRM_TemochiButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_IdouButtonInSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	if( syswk->app->get_item != ITEM_DUMMY_DATA ){
		BOX2BGWFRM_SubDispItemFrmOutSet( syswk->app->wfrm );
	}
	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeMenuClose, BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN_CURMOVE_RET );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�������̂�����v�{�b�N�X�؂�ւ��{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangeBoxMoveButton( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	return BoxMoveTrayScrollSet( syswk, 4 );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�������̂�����v�莝�����ꂩ���{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemChgButton( BOX2_SYS_WORK * syswk )
{
	BOX2MAIN_PokeSelectOff( syswk );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_ITEMICON, FALSE );
	return PartyFrmMoveRight( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�u�������̂�����v�{�b�N�X�؂�ւ��{�^��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemArrangePartyCloseButton( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_PokeMenuOutSet( syswk->app->wfrm );
	BOX2BGWFRM_RetButtonInSet( syswk->app->wfrm );
	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
	if( syswk->app->get_item != ITEM_DUMMY_DATA ){
		BOX2BGWFRM_SubDispItemFrmOutSet( syswk->app->wfrm );
	}
	return VFuncSet( syswk, BOX2MAIN_VFuncItemArrangeMenuClose, BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN_CURMOVE_RET );
}
