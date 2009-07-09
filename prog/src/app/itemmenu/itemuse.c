//============================================================================================
/**
 * @file	  itemuse.c
 * @brief	  �t�B�[���h�A�C�e���g�p����
 * @author	Hiroyuki Nakamura
 * @date	  05.12.12
 */
//============================================================================================
#include "gflib.h"


#include "app/itemuse.h"
#include "itemuse_local.h"

enum{	//�֗��{�^���@�\�֐��Ăяo�����[�h
	CONV_MODE_FUNC,
	CONV_MODE_EVENT,
};

//============================================================================================
//	�萔��`
//============================================================================================
typedef void * (*CNVFUNC_CALL)(void*);

// �֗��{�^�����[�N
typedef struct {
	//FIELDSYS_WORK * fsys;
	ITEMCHECK_WORK	icwk;
	CNVFUNC_CALL	call;
	void * app_wk;
	u16	item;
	u8	seq;
	u8	mode;	//CONV_MODE_FUNC,CONV_MODE_EVENT
}CONV_WORK;

typedef BOOL (*ITEMCNV_FUNC)(CONV_WORK*);	// �֗��{�^���g�p�֐�

typedef struct {
	ITEMUSE_FUNC	use_func;		// �o�b�O����g�p
	ITEMCNV_FUNC	cnv_func;		// �֗��{�^������g�p
	ITEMCHECK_FUNC	check_func;		// �g�p�`�F�b�N
}ITEMUSE_FUNC_DATA;


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void ItemUse_Recover( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_Townmap( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_PhotoAlbum( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_Cycle( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_WazaMachine( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_Mail( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_Kinomi( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_FriendBook( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_SweetHoney( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_FishingRodBad( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_FishingRodGood( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_FishingRodGreat( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_Evolution( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_Ananuke( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_BonguriCase( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_KinomiPlanter( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_UnknownNote( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_DowsingMachine( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_Grashidea( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);
static void ItemUse_BattleRec( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk);


static BOOL ItemCnv_Cycle( CONV_WORK * wk );
static BOOL ItemCnv_FishingRodBad( CONV_WORK * wk );
static BOOL ItemCnv_FishingRodGood( CONV_WORK * wk );
static BOOL ItemCnv_FishingRodGreat( CONV_WORK * wk );
static BOOL ItemCnv_BagMsgItem( CONV_WORK * wk );
static BOOL ItemCnv_Townmap( CONV_WORK * wk );
static BOOL ItemCnv_FriendBook( CONV_WORK * wk );
static BOOL ItemCnv_PhotoAlbum( CONV_WORK * wk );
static void * ConvCall_FriendBook( void * fsys );
static void * ConvCall_Townmap( void * fsys );
static void * ConvCall_BonguriCase( void * fsys );
static void * ConvCall_KinomiPlanter( void * fsys );
static void * ConvCall_UnknownNote( void * fsys );
static void * ConvCall_PhotoAlbum( void * fsys );
static BOOL ItemCnv_BonguriCase( CONV_WORK * wk );
static BOOL ItemCnv_KinomiPlanter( CONV_WORK * wk );
static BOOL ItemCnv_UnknownNote( CONV_WORK * wk );
static BOOL ItemCnv_DowsingMachine( CONV_WORK * wk );
static BOOL ItemCnv_GB_Player( CONV_WORK * wk );
static BOOL ItemCnv_Grashidea( CONV_WORK * wk );
static void * ConvCall_Grashidea( void * fsys );
static BOOL ItemCnv_BattleRec( CONV_WORK * wk );
static void * ConvCall_BattleRec( void * fsys );


static u32 ItemUseCheck_Cycle( const ITEMCHECK_WORK * icwk );
static u32 ItemUseCheck_NoUse( const ITEMCHECK_WORK * icwk );
static u32 ItemUseCheck_Kinomi( const ITEMCHECK_WORK * icwk );
static u32 ItemUseCheck_FishingRod( const ITEMCHECK_WORK * icwk );
static u32 ItemUseCheck_Ananuke( const ITEMCHECK_WORK * icwk );

static BOOL GMEVENT_ItemUseCycle( GMEVENT * event );
static BOOL GMEVENT_BagMsgItem( GMEVENT * event );

static void ConvFuncCallSet( CONV_WORK * wk, CNVFUNC_CALL call ,u8 mode);
static BOOL GMEVENT_ConvButtonCommon( GMEVENT * event );

static BOOL GMEVENT_AnanukeMapChangeInit( GMEVENT *event );


//============================================================================================
//	�O���[�o���ϐ�
//============================================================================================
// �g�p�֐��e�[�u��
static const ITEMUSE_FUNC_DATA ItemFuncTable[] =
{
	{	// 000:�_�~�[
		NULL,				// �o�b�O����̎g�p����
		ItemCnv_BagMsgItem,	// �֗��{�^������̎g�p����
		NULL				// �g�p�`�F�b�N
		},
	{	// 001:��
		ItemUse_Recover,
		NULL,
		NULL
		},
	{	// 002:�^�E���}�b�v
		NULL,
		NULL,
		ItemUseCheck_NoUse,
	},
	{	// 003:�t�H�g�A���o��
		NULL,
		NULL,
		ItemUseCheck_NoUse,
	},
	{	// 004:���]��
		ItemUse_Cycle,
		ItemCnv_Cycle,
		ItemUseCheck_Cycle,
	},
	{	// 005:�`���m�[�g
		NULL,
		NULL,
		ItemUseCheck_NoUse,
	},
	{	// 006:�Z�}�V��
		ItemUse_WazaMachine,
		NULL,
		NULL
		},
	{	// 007:���[��
		ItemUse_Mail,
		NULL,
		NULL
		},
	{	// 008:���̂�
		ItemUse_Kinomi,
		NULL,
		ItemUseCheck_Kinomi
		},
	{	// 009:�|���g�P�[�X�@PORU_APP_DEL 08.06.24
		NULL,
		NULL,
		ItemUseCheck_NoUse,
	},
	{	// 010:�F�B�蒠
		ItemUse_FriendBook,
		ItemCnv_FriendBook,
		NULL
		},
	{	// 011:�|�P�T�[�`���[
		NULL,
		NULL,
		ItemUseCheck_NoUse,
	},
	{	// 012:���傤��
		NULL,
		NULL,
		ItemUseCheck_NoUse,
	},
	{	// 013:�R���V
		NULL,
		NULL,
		ItemUseCheck_NoUse,
	},
	{	// 014:�Â���
		ItemUse_SweetHoney,
		NULL,
		NULL
		},
	{	// 015:�o�g���T�[�`���[
		NULL,
		NULL,
		ItemUseCheck_NoUse,
	},
	{	// 016:�{���̒ފ�
		ItemUse_FishingRodBad,
		ItemCnv_FishingRodBad,
		ItemUseCheck_FishingRod
		},
	{	// 017:�ǂ��ފ�
		ItemUse_FishingRodGood,
		ItemCnv_FishingRodGood,
		ItemUseCheck_FishingRod
		},
	{	// 018:�����ފ�
		ItemUse_FishingRodGreat,
		ItemCnv_FishingRodGreat,
		ItemUseCheck_FishingRod
		},
	{	// 019:�o�b�O���Ŏg�p���铹��
		NULL,
		ItemCnv_BagMsgItem,
		NULL
		},
	{	// 020:�i��
		ItemUse_Evolution,
		NULL,
		NULL
		},
	{	// 021:�������̕R
		ItemUse_Ananuke,
		NULL,
		ItemUseCheck_Ananuke
		},
	{	// 022:�V�E�̓J
		NULL,
		NULL,
		ItemUseCheck_NoUse,
	},
	{	// 023:�ڂ񂮂�P�[�X
		ItemUse_BonguriCase,
		ItemCnv_BonguriCase,
		NULL,
	},
	{	// 024:���̂݃v�����^�[
		ItemUse_KinomiPlanter,
		ItemCnv_KinomiPlanter,
		NULL,
	},
	{	// 025:�A���m�[���m�[�g
		ItemUse_UnknownNote,
		ItemCnv_UnknownNote,
		NULL,
	},
	{	// 026:�_�E�W���O�}�V��
		ItemUse_DowsingMachine,
		ItemCnv_DowsingMachine,
		NULL,
	},
	{	// 027:�f�a�v���C���[
		NULL,
		ItemCnv_GB_Player,
		NULL,
	},
	{	// 028:�O���V�f�A�̂͂�
		ItemUse_Grashidea,
		ItemCnv_Grashidea,
		NULL,
	},
	{	// 029:�o�g�����R�[�_�[
		ItemUse_BattleRec,
		ItemCnv_BattleRec,
		NULL
		},
};



//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��擾
 *
 * @param	prm		�擾�p�����[�^
 * @param	id		�֐�ID
 *
 * @return	�g�p�֐�
 */
//--------------------------------------------------------------------------------------------
u32 ITEMUSE_GetUseFunc( u16 prm, u16 id )
{
	if( prm == ITEMUSE_PRM_USEFUNC ){
		return (u32)ItemFuncTable[id].use_func;
	}else if( prm == ITEMUSE_PRM_CNVFUNC ){
		return (u32)ItemFuncTable[id].cnv_func;
	}
	return (u32)ItemFuncTable[id].check_func;
}

//--------------------------------------------------------------------------------------------
/**
 * �g�p�`�F�b�N���[�N�쐬
 *
 * @param	fsys	�t�B�[���h���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ItemUse_CheckWorkMake( GAMESYS_WORK * gsys, ITEMCHECK_WORK * icwk )
{

	icwk->gsys = gsys;

	//�]�[���h�c
//	icwk->zone_id = fsys->location->zone_id;

	//�A�����
//	icwk->Companion = FALSE; //SysFlag_PairCheck(SaveData_GetEventWork(fsys->savedata));
	//���P�b�g�c�ϑ���
//	icwk->Disguise = FALSE; //SysFlag_DisguiseCheck(SaveData_GetEventWork(fsys->savedata));

	//���@�`��
//	icwk->PlayerForm = Player_FormGet( fsys->player );
//	x = Player_NowGPosXGet( fsys->player );
//	z = Player_NowGPosZGet( fsys->player );
	//���݃A�g���r���[�g
//	icwk->NowAttr = GetAttributeLSB(fsys, x, z);

	//�O���A�g���r���[�g
//	icwk->FrontAttr = GetAttributeLSB(fsys, x, z);
//	SXY_HeroFrontObjGet(fsys, &obj);
	//���̂݊֘A�ǂ����̎g�p�`�F�b�N
	///	icwk->SeedInfo = SeedSys_ItemUseCheck(fsys, obj);

	//���@���Q�ƃ|�C���^
//	icwk->player = fsys->player;
}

//============================================================================================
//
//	�X�N���v�g�Ăяo��
//
//============================================================================================
static BOOL GMEVENT_ItemScript(GMEVENT * event);
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
typedef struct {
	u32 scr_id;		///<�Ăяo���X�N���v�g��ID
	u16 param0;		///<�����n���p�����[�^���̂P
	u16 param1;		///<�����n���p�����[�^���̂Q
	u16 param2;		///<�����n���p�����[�^���̂R
	u16 param3;		///<�����n���p�����[�^���̂S
}CALL_SCR_WORK;

//--------------------------------------------------------------------------------------------
/**
 * @brief
 * @param scr_id;		///<�Ăяo���X�N���v�g��ID
 * @param param0;		///<�����n���p�����[�^���̂P
 * @param param1;		///<�����n���p�����[�^���̂Q
 * @param param2;		///<�����n���p�����[�^���̂R
 * @param param3;		///<�����n���p�����[�^���̂S
 * @return	CALL_SCR_WORK	�X�N���v�g�Ăяo���p���[�N
 */
//--------------------------------------------------------------------------------------------
static CALL_SCR_WORK * CSW_Create(HEAPID heapID, u32 scr_id, u16 param0, u16 param1, u16 param2, u16 param3)
{
	CALL_SCR_WORK * csw;
	csw = GFL_HEAP_AllocMemory(heapID, sizeof(CALL_SCR_WORK));
	csw->scr_id = scr_id;
	csw->param0 = param0;
	csw->param1 = param1;
	csw->param2 = param2;
	csw->param3 = param3;
	return csw;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�A�C�e���p�X�N���v�g�N���̐ݒ�
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�A�C�e���g�p�`�F�b�N���[�N
 * @param	scr_id	�X�N���v�gID
 */
//--------------------------------------------------------------------------------------------
static void SetUpItemScript(ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk, u32 scr_id,HEAPID heapID)
{

//	FieldEvent_Cmd_SetMapProc( fsys );

//	mwk->app_func = GMEVENT_ItemScript;
	//mwk->app_wk   = CSW_Create(HEAPID heapID,scr_id, iuwk->item, 0, 0, 0);
	//mwk->seq      = FLD_MENU_SEQ_BLACK_IN_EVCHG;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�A�C�e���p�X�N���v�g�N���̐ݒ�i�֗��{�^���j
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�A�C�e���g�p�`�F�b�N���[�N
 * @param	scr_id	�X�N���v�gID
 */
//--------------------------------------------------------------------------------------------
static void SetUpItemScriptCnv( CONV_WORK * wk, u32 scr_id )
{
#if 0
	void * work = CSW_Create(scr_id, wk->item, 0, 0, 0);
	FieldEvent_Set( wk->fsys, GMEVENT_ItemScript, work );
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�A�C�e���p�X�N���v�g�̌Ăяo���C�x���g
 * @param	event	�C�x���g���䃏�[�N�ւ̃|�C���^
 * @return	BOOL	TRUE�ŃC�x���g�I���AFALSE�ŃC�x���g���s
 */
//--------------------------------------------------------------------------------------------
static BOOL GMEVENT_ItemScript(GMEVENT * event)
{
#if 0
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	CALL_SCR_WORK * csw = FieldEvent_GetSpecialWork(event);
	int * seq = FieldEvent_GetSeqWork(event);
	FIELD_OBJ_PTR obj;
	switch (*seq) {
	case 0:
		SXY_HeroFrontObjGet(fsys, &obj);
		EventCall_Script(event, csw->scr_id, obj, NULL);
		*(u16*)(GetEvScriptWorkMemberAdrs(fsys,ID_EVSCR_WK_PARAM0)) = csw->param0;
		*(u16*)(GetEvScriptWorkMemberAdrs(fsys,ID_EVSCR_WK_PARAM1)) = csw->param1;
		*(u16*)(GetEvScriptWorkMemberAdrs(fsys,ID_EVSCR_WK_PARAM2)) = csw->param2;
		*(u16*)(GetEvScriptWorkMemberAdrs(fsys,ID_EVSCR_WK_PARAM3)) = csw->param3;

		(*seq) ++;
		break;
	case 1:
		sys_FreeMemoryEz(csw);
		return TRUE;
	}
#endif
	return FALSE;
}

//============================================================================================
//	001:��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F��
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_Recover( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;
	PLIST_DATA * pld;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );
	pld  = sys_AllocMemory( HEAPID_WORLD, sizeof(PLIST_DATA) );

	memset( pld, 0, sizeof(PLIST_DATA) );
	pld->pp     = SaveData_GetTemotiPokemon( fsys->savedata );
	pld->myitem = SaveData_GetMyItem( fsys->savedata );
	pld->mailblock = SaveData_GetMailBlock( fsys->savedata );
	pld->cfg = SaveData_GetConfig( fsys->savedata );
	pld->tvwk = SaveData_GetTvWork( fsys->savedata );
	pld->scwk = &mwk->skill_check;
	pld->type = PL_TYPE_SINGLE;
	pld->mode = PL_MODE_ITEMUSE;
	pld->fsys = fsys;
	pld->item = iuwk->item;
	pld->ret_sel = iuwk->use_poke;
	pld->kt_status = &fsys->KeyTouchStatus;
	GameSystem_StartSubProc( fsys, &PokeListProcData, pld );

	mwk->app_wk = pld;
	FldMenu_AppWaitFuncSet( mwk, FldMenu_PokeListEnd );
#endif
}


//============================================================================================
//	002:�^�E���}�b�v
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F�^�E���}�b�v
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_Townmap( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

	mwk->app_wk =FieldPGearSky_SetProc(fsys ,PGSKY_CALL_BG);
	FldMenu_AppWaitFuncSet( mwk, FldMenu_SubAppFreeEnd );
#endif
}


//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���g�p�֐��F�^�E���}�b�v
 *
 * @param	wk	�֗��{�^�����[�N
 *
 * @return	TRUE�i�֗��{�^�����[�N��������Ȃ��j
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemCnv_Townmap( CONV_WORK * wk )
{
#if 0
	ConvFuncCallSet( wk, ConvCall_Townmap, CONV_MODE_FUNC);
#endif
	return TRUE;
}

static void * ConvCall_Townmap( void * fsys )
{
#if 0
	return FieldPGearSky_SetProc(fsys ,PGSKY_CALL_BG);
#endif
	return NULL;
}



//============================================================================================
//	003:�t�H�g�A���o��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F�t�H�g�A���o��
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_PhotoAlbum( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

	EventCmd_CallFieldAlbum(fsys);

	FldMenu_EvCallWaitFuncSet( mwk, FldMenu_AlbumEnd);
	fsys->startmenu_pos = 0;		// ���j���[�ʒu�N���A
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���g�p�֐��F�t�H�g�A���o��
 *
 * @param	wk	�֗��{�^�����[�N
 *
 * @return	FALSE�i�֗��{�^�����[�N����j
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemCnv_PhotoAlbum( CONV_WORK * wk )
{
#if 0
	ConvFuncCallSet( wk, ConvCall_PhotoAlbum, CONV_MODE_EVENT );
	wk->fsys->startmenu_pos = 0;		// ���j���[�ʒu�N���A
#endif
	return TRUE;
}

static void * ConvCall_PhotoAlbum( void * fsys )
{
#if 0
	EventCmd_CallFieldAlbum(fsys);
#endif
	return NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�`�F�b�N�֐��F�ėp
 *
 * @param	icwk	�A�C�e���g�p�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static u32 ItemUseCheck_NoUse( const ITEMCHECK_WORK * icwk )
{
	return ITEMCHECK_FALSE;	//�A�C�e���Ƃ��Ă͎g���Ȃ�
}


//============================================================================================
//	004:���]��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F���]��
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_Cycle( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
//	FIELDSYS_WORK * fsys;
//	FLD_MENU * mwk;

//	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
//	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

//	FieldEvent_Cmd_SetMapProc( fsys );
//	mwk->app_func = GMEVENT_ItemUseCycle;
//	mwk->app_wk   = NULL;
//	mwk->seq      = FLD_MENU_SEQ_BLACK_IN_EVCHG;
}

//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���g�p�֐��F���]��
 *
 * @param	wk	�֗��{�^�����[�N
 *
 * @return	FALSE�i�֗��{�^�����[�N����j
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemCnv_Cycle( CONV_WORK * wk )
{
//	FieldEvent_Set( wk->fsys, GMEVENT_ItemUseCycle, NULL );
//	wk->fsys->sublcd_menu_lock_passive = 1;
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�C�x���g�F���]��
 *
 * @param	event	�t�B�[���h�C�x���g�p�p�����[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static BOOL GMEVENT_ItemUseCycle( GMEVENT * event )
{
#if 0
	FIELDSYS_WORK * fsys;
	int * seq;
	fsys = FieldEvent_GetFieldSysWork( event );
	seq = FieldEvent_GetSeqWork( event );

	switch (*seq) {
	case 0:
		if (PairPoke_IsPairPoke(fsys)){
			//�|�P�����̈ړ��A�j�����I��点��
			if ( FieldOBJ_AcmdEndCheck( PairPoke_GetFldObjPtr(fsys) ) ){
				int code;
				if( Player_FormGet( fsys->player ) == HERO_FORM_CYCLE ){
					code = MV_PAIR;
				}else{
					//code = MV_PAIR_NO_RATE;
					code = MV_PAIR_NO_RATE_CP_MV;
				}
				//�R�[�h�`�F���W
				FieldOBJ_MoveCodeChange( PairPoke_GetFldObjPtr(fsys), code );
				(*seq) ++;
			}
		}else{
			(*seq) ++;
		}
		break;
	case 1:
		if( Player_FormGet( fsys->player ) == HERO_FORM_CYCLE ){

			FieldOBJ_MovePauseClear( Player_FieldOBJGet( fsys->player ) );
			Player_RequestSet( fsys->player, HERO_REQBIT_NORMAL );
			Player_Request( fsys->player );

			//��Ɍ`���߂��Ă���ABGM�������ĂԁI(060803)

			Snd_FieldBgmSetSpecial( fsys, 0 );					//�N���A
			// ���W�I���Đ�����Ă��鎞�͎��]�ԋȂ���Ȃ�
			if(PMGS_GetRadioMusicID()==0){
				Snd_FadeOutNextPlayCall(fsys,
																Snd_FieldBgmNoGet(fsys,fsys->location->zone_id), BGM_FADE_FIELD_MODE );

			}
			//�|�P��������l���̈ʒu��
			PairPoke_SetPlayerPos(fsys, Player_DirGet( fsys->player ));
			//�A������|�P�o���A�j�����N�G�X�g
			if (PairPoke_IsPairPoke(fsys)){
				PairPoke_SetAppAnmPrm(PairPoke_GetFldObjPtr(fsys), 1);
				//����ړ��Ńo�j�b�V��������Ԃɂ���
				PairPoke_StartVanish(PairPoke_GetFldObjPtr(fsys), 1);
			}
		}else{
			//���BGM�������Ă�ł���A�`������]�Ԃɂ���I(060803)

			// ���W�I���Đ�����Ă��鎞�͎��]�ԋȂ���Ȃ�
			if(PMGS_GetRadioMusicID()==0){
				Snd_FieldBgmSetSpecial( fsys, SEQ_GS_BICYCLE );		//�Z�b�g
				Snd_FadeOutNextPlayCall( fsys, SEQ_GS_BICYCLE, BGM_FADE_FIELD_MODE );
			}

			FieldOBJ_MovePauseClear( Player_FieldOBJGet( fsys->player ) );
			Player_RequestSet( fsys->player, HERO_REQBIT_CYCLE );
			Player_Request( fsys->player );

			PairPokeEvAnm_BallIn(fsys);
			//�A������|�P�o���A�j���N���A
			if (PairPoke_IsPairPoke(fsys)){
				PairPoke_SetAppAnmPrm(PairPoke_GetFldObjPtr(fsys), 0);
			}
		}
		(*seq) ++;
		break;
	case 2:
		FieldOBJSys_MovePauseAllClear( fsys->fldobjsys );
		return TRUE;
	}
#endif
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�`�F�b�N�֐��F���]��
 *
 * @param	icwk	�A�C�e���g�p�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static u32 ItemUseCheck_Cycle( const ITEMCHECK_WORK * icwk )
{
#if 0

	// �A��������͎g�p�s��
	if( icwk->Companion == TRUE ){
		return ITEMCHECK_ERR_COMPANION;
	}
	// ���P�b�g�c�ϑ����͎g�p�s��
	if( icwk->Disguise == TRUE ){
		return ITEMCHECK_ERR_DISGUISE;
	}

	// �T�C�N�����O���[�h�`�F�b�N
	if( Player_MoveBitCheck_CyclingRoad( icwk->player ) == TRUE ){
		return ITEMCHECK_ERR_CYCLE_OFF;
	}

	// ������ / ��
	if( MATR_IsLongGrass( icwk->NowAttr ) == TRUE ||
			MATR_IsSwamp( icwk->NowAttr ) == TRUE ){
		return ITEMCHECK_FALSE;
	}

	// �]�[���`�F�b�N
	if( ZoneData_GetEnableBicycleFlag(icwk->zone_id) == FALSE ){
		return ITEMCHECK_FALSE;
	}

	//�Ȃ݂̂��Ԃ��ǂ���
	if( icwk->PlayerForm == HERO_FORM_SWIM ){
		return ITEMCHECK_FALSE;
	}
#endif
	return ITEMCHECK_TRUE;
}


//============================================================================================
//	005:�`���m�[�g
//============================================================================================

//============================================================================================
//	006:�Z�}�V��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F�Z�}�V��
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_WazaMachine( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;
	PLIST_DATA * pld;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );
	pld  = sys_AllocMemory( HEAPID_WORLD, sizeof(PLIST_DATA) );

	memset( pld, 0, sizeof(PLIST_DATA) );
	pld->pp     = SaveData_GetTemotiPokemon( fsys->savedata );
	pld->myitem = SaveData_GetMyItem( fsys->savedata );
	pld->mailblock = SaveData_GetMailBlock( fsys->savedata );
	pld->cfg = SaveData_GetConfig( fsys->savedata );
	pld->scwk = &mwk->skill_check;
	pld->type = PL_TYPE_SINGLE;
	pld->mode = PL_MODE_WAZASET;
	pld->fsys = fsys;
	pld->item = iuwk->item;
	pld->ret_sel = iuwk->use_poke;
	pld->waza = GetWazaNoItem( iuwk->item );
	pld->kt_status = &fsys->KeyTouchStatus;
	GameSystem_StartSubProc( fsys, &PokeListProcData, pld );

	mwk->app_wk = pld;
	FldMenu_AppWaitFuncSet( mwk, FldMenu_PokeListEnd );
#endif
}


//============================================================================================
//	007:���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F���[��
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_Mail( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;
	MAIL_PARAM * mail;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );
	mail = FieldMailView_InitCall(
		fsys, MAILBLOCK_NULL, ItemMailDesignGet(iuwk->item), HEAPID_WORLD );

	mwk->tmp_wk = FieldMenu_MailWorkMake( iuwk->item, FM_MAIL_MODE_VIEW, 0 );
	mwk->app_wk = mail;
	FldMenu_AppWaitFuncSet( mwk, FldMenu_MailEnd );
#endif
}


//============================================================================================
//	008:���̂�
//============================================================================================
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
static u32 ItemUseCheck_Kinomi(const ITEMCHECK_WORK * icwk)
{
	return ITEMCHECK_TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F���̂�
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_Kinomi( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;
	FIELD_OBJ_PTR	obj;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );
	ItemUse_Recover( iuwk, icwk );
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * ���̂ݎg�p�^�C�v�擾
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 *
 * @retval	"TRUE = ���߂�"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ItemUse_KinomiTypeCheck( const ITEMCHECK_WORK * icwk )
{
	return FALSE;
}

//============================================================================================
//	010:�F�B�蒠
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F�F�B�蒠
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_FriendBook( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
//	FIELDSYS_WORK * fsys;
//	FLD_MENU * mwk;

	//fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	//mwk  = FieldEvent_GetSpecialWork( iuwk->event );

//	mwk->app_wk = WifiNote_SetProc( fsys, fsys->savedata ,HEAPID_WORLD);

	///// @@ WifiNoteProcData

	//FldMenu_AppWaitFuncSet( mwk, FldMenu_SubAppFreeEnd );
}

//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���g�p�֐��F�F�B�蒠
 *
 * @param	wk	�֗��{�^�����[�N
 *
 * @return	TRUE�i�֗��{�^�����[�N��������Ȃ��j
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemCnv_FriendBook( CONV_WORK * wk )
{
#if 0
	ConvFuncCallSet( wk, ConvCall_FriendBook,CONV_MODE_FUNC );
#endif
	return TRUE;
}

static void * ConvCall_FriendBook( void * fsys )
{
#if 0
	return WifiNote_SetProc( fsys, ((FIELDSYS_WORK *)fsys)->savedata ,HEAPID_WORLD);
#endif
	return NULL;
}

//============================================================================================
//	014:�Â���
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * @brief	�A�C�e���g�p�֐��F�Â���
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_SweetHoney( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;
	EVENT_AMAIKAORI_EFF_WORK	*work;
	int size;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

	FieldEvent_Cmd_SetMapProc( fsys );

	size = FLDMAP_Amaikaori_WorkSize();
	work = sys_AllocMemoryLo( HEAPID_WORLD, size );
	memset(work, 0, size);

	mwk->app_func = FLDMAP_AmaikaoriEvent;
	mwk->app_wk   = work;
	mwk->seq      = FLD_MENU_SEQ_BLACK_IN_EVCHG;
	//�A�C�e�����炷
	MyItem_SubItem( SaveData_GetMyItem(fsys->savedata), iuwk->item, 1, HEAPID_WORLD );
#endif
}

//============================================================================================
//	016:�{�����ފ�
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * @brief	�A�C�e���g�p�֐��F�{�����ފ�
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_FishingRodBad( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
	#if 0

	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

	FieldEvent_Cmd_SetMapProc( fsys );
	mwk->app_func = GMEVENT_Fishing;
	mwk->app_wk   = EventWorkSet_Fishing( fsys, HEAPID_WORLD, FISHINGROD_BAD );
	mwk->seq      = FLD_MENU_SEQ_BLACK_IN_EVCHG;
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���g�p�֐��F�{�����ފ�
 * @param	wk	�֗��{�^�����[�N
 * @return	FALSE�i�֗��{�^�����[�N����j
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemCnv_FishingRodBad( CONV_WORK * wk )
{
#if 0
	void *work = EventWorkSet_Fishing( wk->fsys, HEAPID_FIELD, FISHINGROD_BAD );
	FieldEvent_Set( wk->fsys, GMEVENT_Fishing, work );
#endif
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�A�C�e���g�p�֐��F�ǂ��ފ�
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_FishingRodGood( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

	FieldEvent_Cmd_SetMapProc( fsys );
	mwk->app_func = GMEVENT_Fishing;
	mwk->app_wk   = EventWorkSet_Fishing( fsys, HEAPID_WORLD, FISHINGROD_GOOD );
	mwk->seq      = FLD_MENU_SEQ_BLACK_IN_EVCHG;
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���g�p�֐��F�ǂ��ފ�
 * @param	wk	�֗��{�^�����[�N
 * @return	FALSE�i�֗��{�^�����[�N����j
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemCnv_FishingRodGood( CONV_WORK * wk )
{
#if 0
	void *work = EventWorkSet_Fishing( wk->fsys, HEAPID_FIELD, FISHINGROD_GOOD );
	FieldEvent_Set( wk->fsys, GMEVENT_Fishing, work );
#endif
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�A�C�e���g�p�֐��F�����ފ�
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_FishingRodGreat( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

	FieldEvent_Cmd_SetMapProc( fsys );
	mwk->app_func = GMEVENT_Fishing;
	mwk->app_wk   = EventWorkSet_Fishing( fsys, HEAPID_WORLD, FISHINGROD_GREAT );
	mwk->seq      = FLD_MENU_SEQ_BLACK_IN_EVCHG;
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���g�p�֐��F�����ފ�
 * @param	wk	�֗��{�^�����[�N
 * @return	FALSE�i�֗��{�^�����[�N����j
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemCnv_FishingRodGreat( CONV_WORK * wk )
{
#if 0
	void *work = EventWorkSet_Fishing( wk->fsys, HEAPID_FIELD, FISHINGROD_GREAT );
	FieldEvent_Set( wk->fsys, GMEVENT_Fishing, work );
#endif
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�`�F�b�N�֐��F�ފ�
 *
 * @param	icwk	�A�C�e���g�p�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static u32 ItemUseCheck_FishingRod( const ITEMCHECK_WORK * icwk )
{
#if 0
	// �A��������͎g�p�s��
	if( icwk->Companion == TRUE ){
		return ITEMCHECK_ERR_COMPANION;
	}
	// ���P�b�g�c�ϑ����͎g�p�s��
	if( icwk->Disguise == TRUE ){
		return ITEMCHECK_ERR_DISGUISE;
	}

	// �ڂ̑O����
	if( MATR_IsWater(icwk->FrontAttr) == TRUE ){
		// ���̏�ɂ��Ȃ����OK
		if( MATR_IsBridge( icwk->NowAttr ) == TRUE ||
				MATR_IsBridgeFlag( icwk->NowAttr ) == TRUE ){
			FIELD_OBJ_PTR	fldobj = Player_FieldOBJGet( icwk->player );
			if( FieldOBJ_StatusBitCheck_Bridge(fldobj) == TRUE ){
				return ITEMCHECK_FALSE;
			}
		}
		return ITEMCHECK_TRUE;
	}
#endif
	return ITEMCHECK_FALSE;
}


//============================================================================================
//	019:�o�b�O���Ŏg�p���铹��
//		�R�C���P�[�X�A�A�N�Z�T���[����A�V�[������A�|�C���g�J�[�h
//============================================================================================
typedef struct {
	GFL_BMPWIN *win;	// �E�B���h�E�f�[�^
	STRBUF * msg;		// ���b�Z�[�W
	u16	midx;			// ���b�Z�[�W�C���f�b�N�X
	u16	seq;			// �V�[�P���X
}BAGMSG_WORK;

#define	BAGMSG_TMP_SIZ		( 128 )


//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���g�p�֐��F�o�b�O���Ŏg�p���铹��
 *
 * @param	wk	�֗��{�^�����[�N
 *
 * @return	FALSE�i�֗��{�^�����[�N������j
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemCnv_BagMsgItem( CONV_WORK * wk )
{
#if 0
	BAGMSG_WORK * bmwk = sys_AllocMemory( HEAPID_WORLD, sizeof(BAGMSG_WORK) );
	bmwk->seq = 0;

	bmwk->msg = STRBUF_Create( BAGMSG_TMP_SIZ, HEAPID_WORLD );
	BAG_ItemUseMsgSet( wk->fsys->savedata, bmwk->msg, wk->item, HEAPID_WORLD );

	FieldEvent_Set( wk->fsys, GMEVENT_BagMsgItem, bmwk );
#endif
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �o�b�O���Ŏg�p���铹��F�֗��{�^������N�����ꂽ�ꍇ�̃C�x���g
 *
 * @param	event	�C�x���g�f�[�^
 *
 * @retval	"TRUE = �I��"
 * @retval	"FALSE = ������"
 */
//--------------------------------------------------------------------------------------------
static BOOL GMEVENT_BagMsgItem( GMEVENT * event )
{
#if 0
	FIELDSYS_WORK * fsys;
	BAGMSG_WORK * wk;

	fsys = FieldEvent_GetFieldSysWork( event );
	wk   = FieldEvent_GetSpecialWork( event );

	switch( wk->seq ){
	case 0:
		fsys->sublcd_menu_next_flag = 1;

		FieldOBJSys_MovePauseAll( fsys->fldobjsys );

		/*
		wk->msg = STRBUF_Create( BAGMSG_TMP_SIZ, HEAPID_WORLD );

		BAG_ItemUseMsgSet(
			fsys->savedata, wk->msg,
			MyItem_CnvButtonItemGet( SaveData_GetMyItem(fsys->savedata) ), HEAPID_WORLD );
		 */

		FldTalkBmpAdd( fsys->bgl, &wk->win, FLD_MBGFRM_FONT );
		{
			const CONFIG * cfg = SaveData_GetConfig( fsys->savedata );
			FieldTalkWinPut( &wk->win, cfg );
			wk->midx = FieldTalkMsgStart( &wk->win, wk->msg, cfg, 1 );
		}
		wk->seq++;
		break;

	case 1:
		if( FldTalkMsgEndCheck( wk->midx ) == 1 ){
			if(sys.trg & (PAD_KEY_UP|PAD_KEY_DOWN|PAD_KEY_LEFT|PAD_KEY_RIGHT|PAD_BUTTON_A|PAD_BUTTON_B) || sys.tp_trg){
				fsys->sublcd_menu_next_flag = 0;
				BmpTalkWinClear( &wk->win, WINDOW_TRANS_ON );
				wk->seq++;
			}
		}
		break;

	case 2:
		FieldOBJSys_MovePauseAllClear( fsys->fldobjsys );
		GF_BGL_BmpWinDel( &wk->win );
		STRBUF_Delete( wk->msg );
		sys_FreeMemoryEz( wk );
		return TRUE;
	}
#endif
	return FALSE;
}


//============================================================================================
//	020:�i��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F�i���A�C�e��
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_Evolution( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk )
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;
	PLIST_DATA * pld;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );

	//�A���t�̈�Ձ@�B�������I�[�v���M�~�b�N�@���̐Ύg�p�`�F�b�N
	if(	iuwk->item == ITEM_MIZUNOISI &&
			AlfGimmick_D24R0206MizunoisiCheck(fsys)){
		SetUpItemScript(iuwk, icwk, SCRID_D24R0206_MIZU_ACTION);
		return;
	}

	mwk  = FieldEvent_GetSpecialWork( iuwk->event );
	pld  = sys_AllocMemory( HEAPID_WORLD, sizeof(PLIST_DATA) );

	memset( pld, 0, sizeof(PLIST_DATA) );
	pld->pp     = SaveData_GetTemotiPokemon( fsys->savedata );
	pld->myitem = SaveData_GetMyItem( fsys->savedata );
	pld->mailblock = SaveData_GetMailBlock( fsys->savedata );
	pld->cfg = SaveData_GetConfig( fsys->savedata );
	pld->tvwk = SaveData_GetTvWork( fsys->savedata );
	pld->scwk = &mwk->skill_check;
	pld->type = PL_TYPE_SINGLE;
	pld->mode = PL_MODE_SHINKA;
	pld->item = iuwk->item;
	pld->ret_sel = iuwk->use_poke;
	pld->fsys    = fsys;
	pld->kt_status = &fsys->KeyTouchStatus;
	GameSystem_StartSubProc( fsys, &PokeListProcData, pld );

	mwk->app_wk = pld;
	FldMenu_AppWaitFuncSet( mwk, FldMenu_PokeListEnd );]
#endif
}


//============================================================================================
//	021	:�������̕R
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * @brief	�A�C�e���g�p�֐��F�������̕R
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_Ananuke( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;

	//�A���t�̈�Ձ@�������M�~�b�N�`�F�b�N
	if(AlfGimmick_D24R0202AnanukeCheck(icwk->fsys)){
		SetUpItemScript(iuwk, icwk, SCRID_D24R0202_ANANUKE_ACTION);
		return;
	}
	//�ʏ폈��
	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

	FieldEvent_Cmd_SetMapProc( fsys );
	mwk->app_func = GMEVENT_AnanukeMapChangeInit;
	mwk->app_wk   = NULL;
	mwk->seq      = FLD_MENU_SEQ_BLACK_IN_EVCHG;
	//�A�C�e�����炷
	MyItem_SubItem( SaveData_GetMyItem(fsys->savedata), iuwk->item, 1, HEAPID_WORLD );
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�`�F�b�N�֐��F�������̕R
 *
 * @param	icwk	�A�C�e���g�p�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static u32 ItemUseCheck_Ananuke( const ITEMCHECK_WORK * icwk )
{
#if 0
	// �A��������͎g�p�s��
	if( icwk->Companion == TRUE ){
		return ITEMCHECK_ERR_COMPANION;
	}
	// ���P�b�g�c�ϑ����͎g�p�s��
	if( icwk->Disguise == TRUE ){
		return ITEMCHECK_ERR_DISGUISE;
	}
	if(AlfGimmick_D24R0202AnanukeCheck(icwk->fsys)){
		return ITEMCHECK_TRUE;
	}
	if( ZoneData_IsDungeon(icwk->zone_id) == TRUE &&
			ZoneData_GetEnableEscapeFlag(icwk->zone_id) == TRUE ){
		return ITEMCHECK_TRUE;
	}
#endif

	return ITEMCHECK_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�������̕R���[�N���������������C�x���g��
 */
//--------------------------------------------------------------------------------------------
static BOOL GMEVENT_AnanukeMapChangeInit( GMEVENT *event )
{
#if 0
	FIELDSYS_WORK *fsys = FieldEvent_GetFieldSysWork( event );
	void *work = EventWorkSet_AnanukeMapChange( fsys, HEAPID_WORLD );
	FieldEvent_Change( event, GMEVENT_AnanukeMapChange, work );
#endif
	return( FALSE );
}

//============================================================================================
//	023:�ڂ񂮂�P�[�X
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F�ڂ񂮂�P�[�X
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_BonguriCase( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

	mwk->app_wk = FieldBongCase_SetProc( fsys, BONGCASE_POT);

	FldMenu_AppWaitFuncSet( mwk, FldMenu_SubAppFreeEnd );
#endif
}


//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���g�p�֐��F�ڂ񂮂�P�[�X
 *
 * @param	wk	�֗��{�^�����[�N
 *
 * @return	TRUE�i�֗��{�^�����[�N��������Ȃ��j
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemCnv_BonguriCase( CONV_WORK * wk )
{
#if 0
	ConvFuncCallSet( wk, ConvCall_BonguriCase, CONV_MODE_FUNC );
#endif
	return TRUE;
}

static void * ConvCall_BonguriCase( void * fsys )
{
#if 0
	return FieldBongCase_SetProc( fsys, BONGCASE_POT);
#endif
	return NULL;
}

//============================================================================================
//	024:���̂݃v�����^�[
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F���̂݃v�����^�[
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_KinomiPlanter( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

	mwk->app_wk = FieldNutPlanter_SetProc( fsys );

	FldMenu_AppWaitFuncSet( mwk, FldMenu_SubAppFreeEnd );
#endif
}


//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���g�p�֐��F���̂݃v�����^�[
 *
 * @param	wk	�֗��{�^�����[�N
 *
 * @return	TRUE�i�֗��{�^�����[�N��������Ȃ��j
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemCnv_KinomiPlanter( CONV_WORK * wk )
{
#if 0
	ConvFuncCallSet( wk, ConvCall_KinomiPlanter, CONV_MODE_FUNC );
#endif
	return TRUE;
}

static void * ConvCall_KinomiPlanter( void * fsys )
{
#if 0
	return FieldNutPlanter_SetProc( fsys );
#endif
	return NULL;
}

//============================================================================================
//	024:�A���m�[���m�[�g
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F�A���m�[���m�[�g
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_UnknownNote( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

	mwk->app_wk = FieldAnNote_SetProc( fsys );

	FldMenu_AppWaitFuncSet( mwk, FldMenu_SubAppFreeEnd );
#endif
}


//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���g�p�֐��F�A���m�[���m�[�g
 *
 * @param	wk	�֗��{�^�����[�N
 *
 * @return	TRUE�i�֗��{�^�����[�N��������Ȃ��j
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemCnv_UnknownNote( CONV_WORK * wk )
{
#if 0
	ConvFuncCallSet( wk, ConvCall_UnknownNote, CONV_MODE_FUNC );
#endif
	return TRUE;
}

static void * ConvCall_UnknownNote( void * fsys )
{
#if 0
	return FieldAnNote_SetProc( fsys );
#endif
	return NULL;
}


//============================================================================================
//	026:�_�E�W���O�}�V��
//============================================================================================
//#include "field_poketch.h"

static BOOL GMEVENT_ItemUseDowsingMachine( GMEVENT * event );

static void ItemUse_DowsingMachine( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

	FieldEvent_Cmd_SetMapProc( fsys );
	mwk->app_func = GMEVENT_ItemUseDowsingMachine;
	mwk->app_wk   = NULL;
	mwk->seq      = FLD_MENU_SEQ_BLACK_IN_EVCHG;
#endif

	//	FieldSubLcdControler_ChangeSubSeq( fsys, SUBLCD_MODE_DOWSING_MENU, 7, NULL );
}

static BOOL ItemCnv_DowsingMachine( CONV_WORK * wk )
{
#if 0
	// �͂��E�����������ɐ؂�ւ����N�G�X�g
	//	FieldSubLcdControler_ChangeSubSeq( fsys, SUBLCD_MODE_DOWSING_MENU, 3, NULL );
	FieldEvent_Set( wk->fsys, GMEVENT_ItemUseDowsingMachine, NULL );
#endif
	return FALSE;
}

static BOOL GMEVENT_ItemUseDowsingMachine( GMEVENT * event )
{
#if 0
	FIELDSYS_WORK * fsys;
	int * seq;

	fsys = FieldEvent_GetFieldSysWork( event );
	seq  = FieldEvent_GetSeqWork( event );

	switch( *seq ){
	case 0:
		FieldSubLcdControler_ChangeApp( fsys, SUBLCD_MODE_DOWSING_MENU, NULL );
		fsys->sublcd_menu_lock_passive = 1;
		(*seq)++;
		break;

	case 1:
		if( FieldSubLcdControler_GetSequence(fsys) == SUBLCD_SEQ_MAIN ){
			FieldOBJSys_MovePauseAllClear( fsys->fldobjsys );
			return TRUE;
		}
		break;
	}
#endif

	return FALSE;

}


static BOOL ItemCnv_GB_Player( CONV_WORK * wk )
{
#if 0
	BAGMSG_WORK * bmwk = sys_AllocMemory( HEAPID_WORLD, sizeof(BAGMSG_WORK) );
	bmwk->seq = 0;


	{
		MSGDATA_MANAGER * mman;
		mman = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_bag_dat, HEAPID_WORLD );

		// ���b�Z�[�W�\��
		if(Snd_GameBoyFlagCheck() == TRUE){
			// �uGB�v���C���[��ON�ɂ����I�v
			bmwk->msg = MSGMAN_AllocString( mman, mes_bag_111 );
		}else{
			// �uGB�v���C���[��OFF�ɂ����I�v
			bmwk->msg = MSGMAN_AllocString( mman, mes_bag_110 );
		}
		MSGMAN_Delete( mman );
	}
	// �Q�[���{�[�C���؂�ւ�
	Snd_GameBoyItemUse();


	FieldEvent_Set( wk->fsys, GMEVENT_BagMsgItem, bmwk );

#endif

	return FALSE;
}

//============================================================================================
//	028:�O���V�f�A�̂͂�
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F�O���V�f�A�̂͂�
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_Grashidea( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;
	PLIST_DATA * pld;


	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

	mwk->app_wk = FieldFormChangePokeList_SetProc(fsys, HEAPID_WORLD, ITEM_GURASIDEANOHANA);

	FldMenu_AppWaitFuncSet( mwk, FldMenu_PokeListEnd );
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���g�p�֐��F�O���V�f�A�̂͂�
 *
 * @param	wk	�֗��{�^�����[�N
 *
 * @return	TRUE�i�֗��{�^�����[�N��������Ȃ��j
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemCnv_Grashidea( CONV_WORK * wk )
{
#if 0
	ConvFuncCallSet( wk, ConvCall_Grashidea, CONV_MODE_FUNC );
#endif
	return TRUE;
}

static void * ConvCall_Grashidea( void * fsys )
{
#if 0
	return FieldFormChangePokeList_SetProc(fsys, HEAPID_WORLD, ITEM_GURASIDEANOHANA);
#else
	return NULL;
#endif
}

//============================================================================================
//	029:�o�g�����R�[�_�[
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�֐��F�o�g�����R�[�_�[
 *
 * @param	iuwk	�A�C�e���g�p���[�N
 * @param	icwk	�`�F�b�N���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemUse_BattleRec( ITEMUSE_WORK * iuwk, const ITEMCHECK_WORK * icwk)
{
#if 0
	FIELDSYS_WORK * fsys;
	FLD_MENU * mwk;

	fsys = FieldEvent_GetFieldSysWork( iuwk->event );
	mwk  = FieldEvent_GetSpecialWork( iuwk->event );

	BattleRecorder_SetProc( fsys, fsys->savedata );

	mwk->app_wk = NULL;
	FldMenu_AppWaitFuncSet( mwk, FldMenu_BattleRecoderEnd );
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * �֗��{�^���g�p�֐��F�o�g�����R�[�_�[
 *
 * @param	wk	�֗��{�^�����[�N
 *
 * @return	TRUE�i�֗��{�^�����[�N��������Ȃ��j
 */
//--------------------------------------------------------------------------------------------
static BOOL ItemCnv_BattleRec( CONV_WORK * wk )
{
#if 0
	ConvFuncCallSet( wk, ConvCall_BattleRec, CONV_MODE_FUNC );
#endif
	return TRUE;
}

static void * ConvCall_BattleRec( void * fsys )
{
#if 0
	//�r�f�I�̏������A�폜�ŃZ�[�u���s����̂Ŏ��O�Ƀp�����[�^�Z�b�g 2008.05.12(��) matsuda
	Field_SaveParam_BattleRecorder(fsys);

	BattleRecorder_SetProc( fsys, ((FIELDSYS_WORK *)fsys)->savedata );
#endif
	return NULL;
}



//--------------------------------------------------------------------------------------------
/**
 * ��ʐ؂�ւ��A�C�e���p���ʃC�x���g
 *
 * @param	event
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static BOOL GMEVENT_ConvButtonCommon( GMEVENT * event )
{
#if 0
	FIELDSYS_WORK * fsys;
	CONV_WORK * wk;

	fsys = FieldEvent_GetFieldSysWork( event );
	wk   = FieldEvent_GetSpecialWork( event );

	switch( wk->seq ){
	case SEQ_FADEOUT_SET:	// �t�F�[�h�A�E�g�Z�b�g
		FieldOBJSys_MovePauseAll( fsys->fldobjsys );
		FieldFadeWipeSet( FLD_DISP_BRIGHT_BLACKOUT );
		wk->seq = SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:	// �t�F�[�h�A�E�g�҂�
		if( WIPE_SYS_EndCheck() ){
			wk->app_wk = wk->call( fsys );
			if(wk->mode == CONV_MODE_FUNC){
				wk->seq = SEQ_FUNC_WAIT;
			}else{
				wk->seq = SEQ_END_WAIT;
			}
		}
		break;

	case SEQ_FUNC_WAIT:		// �T�u�v���Z�X�I���҂�
		if( FieldEvent_Cmd_WaitSubProcEnd( fsys ) ) {
			break;
		}
		//�����̓X���[
	case SEQ_END_WAIT:
		if( wk->app_wk != NULL ){
			sys_FreeMemoryEz( wk->app_wk );
		}
		FieldEvent_Cmd_SetMapProc( fsys );
		wk->seq = SEQ_RCV_WAIT;
		break;

	case SEQ_RCV_WAIT:		// �t�B�[���h�}�b�v�v���Z�X�J�n�҂�
		if( FieldEvent_Cmd_WaitMapProcStart(fsys) ) {
			FieldOBJSys_MovePauseAll( fsys->fldobjsys );
			FieldFadeWipeSet( FLD_DISP_BRIGHT_BLACKIN );
			wk->seq = SEQ_FADEIN_WAIT;
		}
		break;

	case SEQ_FADEIN_WAIT:	// �t�F�[�h�C���҂�
		if( WIPE_SYS_EndCheck() ){
			FieldOBJSys_MovePauseAllClear( fsys->fldobjsys );
			sys_FreeMemoryEz( wk );
			return TRUE;
		}
	}
#endif
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * ��ʐ؂�ւ��A�C�e���p���ʃC�x���g�Z�b�g
 *
 * @param	wk		�֗��{�^�����[�N
 * @param	call	�t�F�[�h�A�E�g��ɌĂ΂�鏈��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ConvFuncCallSet( CONV_WORK * wk, CNVFUNC_CALL call,u8 mode)
{
#if 0
	wk->call = call;
	wk->mode = mode;
	FieldEvent_Set( wk->fsys, GMEVENT_ConvButtonCommon, wk );
#endif
}
