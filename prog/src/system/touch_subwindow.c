//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		touch_subwindow.c
 *	@brief		�T�u��ʁ@�G���E�B���h�E�{�^���V�X�e��
 *	@author		tomoya takahashi & miyuki iwasawa
 *	@data		2008.06.23
 *				2009.01.15 GS���ڐA Ariizumi
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include <string.h>

#include "system/gfl_use.h"
#include "arc_def.h"
#include "yn_touch.naix"

#define __TOUCH_SUBWINDOW_H_GLOBAL 
#include "include/system/touch_subwindow.h"

#define TSW_USE_SND (0)

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
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
// �e�o�b�t�@�E���[�N�̐�
enum{		
	TOUCH_SW_BUF_YES,
	TOUCH_SW_BUF_NO,
	TOUCH_SW_BUF_NUM,


	TOUCH_SW_BUF_NO_TOUCH,	// �^�b�`���ꂽ���`�F�b�N���̐G���Ă��Ȃ��萔
};
#define TOUCH_SW_Y_OFS	( 32 )	// NO�̕��̃I�t�Z�b�g

// �A�j���[�V�����֌W
#define TOUCH_SW_GRA_ID	(ARC_YNTOUCH_GRA)
#define TOUCH_SW_ANIME_NUM		( 2 )		// �A�j���[�V������
#define TOUCH_SW_ANIME_SP		( FX32_ONE * 2 )// �A�j���X�s�[�h
#define TOUCH_SW_ANIME_TIMING	( FX32_ONE * 1 )

//��������L�[
#define TOUCH_SW_KEY_TRG	(PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL|PAD_KEY_UP|PAD_KEY_DOWN)

// ���̃��[�N���̏��
enum{
	TOUCH_SW_PARAM_NODATA,	// ��
	TOUCH_SW_PARAM_DO,		// ���s�f�[�^�i�[
};

// VRAM�]���^�X�N
#define TOUCH_SW_VRAM_TRANS_TSK_PRI	( 128 )

#define TOUCH_FLG_INIT	( 0x8 )


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	�A�j���f�[�^
//=====================================
typedef struct {
	const fx32* cp_anime_tbl;
	u32 tbl_num;
	fx32 frame;
} TOUCH_SW_ANIME;


//-------------------------------------
//	�{�^���f�[�^�������f�[�^
//=====================================
typedef struct {
	const fx32* cp_anime_tbl;	// �A�j���e�[�u��
	u16 fileidx;	// �A�[�J�C�u�t�@�C��
	u16 scrn_arcidx[ TOUCH_SW_ANIME_NUM ];	// �X�N���[���f�[�^
	u16 char_offs;	// �L�����N�^�I�t�Z�b�g
	u16 pltt_offs;	// �p���b�g�I�t�Z�b�g
	u8 bg_frame;			// BG�i���o�[
	u8 tbl_num;				// �e�[�u����
	u8 ofs_x;		// ��
	u8 ofs_y;		// ��
} TOUCH_SW_BUTTON_PARAM;

//-------------------------------------
//	1�{�^���f�[�^
//=====================================
typedef struct {
	u32 bg_frame;			// BG�i���o�[
	void* p_scrn_buff[ TOUCH_SW_ANIME_NUM ];		// �X�N���[���o�b�t�@
	NNSG2dScreenData* p_scrn[ TOUCH_SW_ANIME_NUM ];	// �X�N���[���f�[�^
	TOUCH_SW_ANIME anime;
	u8 ofs_x;	// �����W
	u8 ofs_y;	// �����W
	u8 anm_idx;	// ���̔��f����Ă���X�N���[��IDX
	u8 dummy1;
} TOUCH_SW_BUTTON;

//-------------------------------------
//	�L�����N�^�f�[�^�]���^�X�N
//=====================================
typedef struct {
	NNSG2dCharacterData* p_char;
	void* p_buff;	
	u16 bg_frame;
	u16 char_ofs;
} TOUCH_SW_CHARACTER_TRANS;

//-------------------------------------
//	V�u�����N�@�p���b�g�f�[�^�]���^�X�N
//=====================================
typedef struct {
	NNSG2dPaletteData* p_pltt;
	void* p_buff;	
	u16 pltype;	// �p���b�g�]����
	u16 ofs;
	u16 size;
} TOUCH_SW_PALETTE_TRANS;


//-------------------------------------
//	�V�X�e�����[�N
//=====================================
struct _TOUCH_SW_SYS
{
	GFL_BUTTON_MAN* p_button_man;// �{�^���}�l�[�W��
	GFL_UI_TP_HITTBL hit_tbl[ TOUCH_SW_BUF_NUM+1 ];		// �{�^���f�[�^(�I�[�f�[�^
	TOUCH_SW_BUTTON button[ TOUCH_SW_BUF_NUM ];	// �{�^���f�[�^
	u32 bg_frame;			// BG�i���o�[
	u32 heapid;				// �g�p�q�[�vID
	fx32 anime_tbl[ TOUCH_SW_ANIME_NUM ];	// �A�j���e�[�u��
	u8 x;					// x���W�i��׸��P�ʁj
	u8 y;					// y���W�i��׸��P�ʁj
	u8 sizx;				// x�T�C�Y�i��׸��P�ʁj
	u8 sizy;				// y�T�C�Y�i��׸��P�ʁj
	
	u8 kt_st:1;				// �L�[or�^�b�`�̃X�e�[�^�X
	u8 type:1;				// �\���^�C�v(TOUCH_SW_TYPE_S,TOUCH_SW_TYPE_L)
	u8 wait:6;				// �E�F�C�g
	u8 key_pos;				// �L�[�̃|�W�V����
	u8 button_move_flag;	// �{�^������t���O
	u8 now_work_param : 4;	// ���݂̃��[�N���̏��
	u8 touch_flg: 4;		// �^�b�`�����u�Ԋ��m
};


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static void TouchSW_CleanWork( TOUCH_SW_SYS* p_touch_sw, u32 heapid );
static void touchsw_ktst_view_set(TOUCH_SW_SYS* p_touch_sw);

//-------------------------------------
//	������x�������܂Ƃ߂��֐�
//=====================================
static void TouchSW_SYS_CharTrans( TOUCH_SW_SYS* p_touch_sw, const TOUCH_SW_PARAM* cp_param );
static void TouchSW_SYS_PlttTrans( TOUCH_SW_SYS* p_touch_sw, const TOUCH_SW_PARAM* cp_param );
static void TouchSW_SYS_ButtonInit( TOUCH_SW_SYS* p_touch_sw, const TOUCH_SW_PARAM* cp_param );
static void TouchSW_SYS_ButtonManaInit( TOUCH_SW_SYS* p_touch_sw, const TOUCH_SW_PARAM* cp_param );

//-------------------------------------
//	�{�^������V�X�e��
//=====================================
static void TouchSW_ButtonCallBack( u32 button_no, u32 event, void* p_work );

//-------------------------------------
//	�{�^�����[�N����֐��S
//=====================================
static void TouchSW_ButtonInit( TOUCH_SW_BUTTON* p_bttn, const TOUCH_SW_BUTTON_PARAM* cp_param, u32 heapid );
static void TouchSW_ButtonDelete( TOUCH_SW_BUTTON* p_bttn );

//-------------------------------------
//	�L�����N�^�f�[�^�@�p���b�g�f�[�^�]��
//=====================================
static void TouchSW_CharTransReq( u32 fileIdx, u32 dataIdx, u32 frm, u32 offs, u32 heapID );
static void TouchSW_PlttTransReq( u32 fileIdx, u32 dataIdx, PALTYPE palType, u32 offs, u32 transSize, u32 heapID );
static void TouchSW_CharTransTsk( GFL_TCB *tcb, void* p_work );
static void TouchSW_PlttTransTsk( GFL_TCB *tcb, void* p_work );


//-------------------------------------
//	�X�N���[���̔��f���s��
//=====================================
static void TouchSW_ScrnSet( int frame, const NNSG2dScreenData* scrn, int x, int y );
static void TouchSW_ScrnCharOfsSet( const NNSG2dScreenData* scrn, int char_offs );
static void TouchSW_ScrnPlttOfsSet( const NNSG2dScreenData* scrn, int pltt_offs );

//----------------------------------------------------------------------------
/**
 *	@brief	�V�X�e�����[�N�쐬
 *	
 *	@param	heapid	�g�p�q�[�vID 
 *	
 *	@return	���[�N�|�C���^
 */
//-----------------------------------------------------------------------------
TOUCH_SW_SYS* TOUCH_SW_AllocWork( u32 heapid )
{
	TOUCH_SW_SYS* p_touch_sw;
	p_touch_sw = GFL_HEAP_AllocMemory( heapid, sizeof(TOUCH_SW_SYS) );

	// ���[�N������
	TouchSW_CleanWork( p_touch_sw, heapid );

	return p_touch_sw;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�X�e�����[�N�j��
 *	
 *	@param	p_touch_sw	�V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void TOUCH_SW_FreeWork( TOUCH_SW_SYS* p_touch_sw )
{
	// ���s�f�[�^�i�[�ς݂Ȃ烊�Z�b�g
	if( p_touch_sw->now_work_param == TOUCH_SW_PARAM_DO ){
		TOUCH_SW_Reset( p_touch_sw );
	}
	GFL_HEAP_FreeMemory( p_touch_sw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�X�e��������(�������ʏ���)
 *
 *	@param	p_touch_sw	�V�X�e�����[�N
 *	@param	cp_param	�V�X�e������f�[�^
 */
//-----------------------------------------------------------------------------
static void touch_sw_init_com( TOUCH_SW_SYS* p_touch_sw, const TOUCH_SW_PARAM* cp_param )
{
	static const u8 btn_siz[][2] = {
		{TOUCH_SW_USE_SCRN_SX,TOUCH_SW_USE_SCRN_SY},
		{TOUCH_SW_USE_SCRN_LX,TOUCH_SW_USE_SCRN_LY},
	};
	
	// �f�[�^�i�[
	p_touch_sw->bg_frame	= cp_param->bg_frame;
	p_touch_sw->x			= cp_param->x;
	p_touch_sw->y			= cp_param->y;

	//�����L�[or�^�b�`���[�h
	p_touch_sw->kt_st		= cp_param->kt_st;
	//YesNo�I���̏����|�W�V����
	if(cp_param->key_pos < 2){
		p_touch_sw->key_pos		= cp_param->key_pos;
	}else{
		p_touch_sw->key_pos = 0;
	}
	
	//�{�^���T�C�Y�i�[
	p_touch_sw->sizx = btn_siz[cp_param->type][0];
	p_touch_sw->sizy = btn_siz[cp_param->type][1];
	
	// �L�����N�^�f�[�^�]��
	TouchSW_SYS_CharTrans( p_touch_sw, cp_param );

	// �p���b�g�f�[�^�]��
	TouchSW_SYS_PlttTrans( p_touch_sw, cp_param );
	
	// �{�^���f�[�^������
	TouchSW_SYS_ButtonInit( p_touch_sw, cp_param );

	// �{�^���}�l�[�W��������
	TouchSW_SYS_ButtonManaInit( p_touch_sw, cp_param );

	// ���s�f�[�^�ێ�
	p_touch_sw->now_work_param = TOUCH_SW_PARAM_DO;

	// �^�b�`�t���O
	p_touch_sw->touch_flg = TOUCH_FLG_INIT;

	//���샂�[�h�ɂ���āA����View��ύX
	touchsw_ktst_view_set(p_touch_sw);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�X�e��������
 *
 *	@param	p_touch_sw	�V�X�e�����[�N
 *	@param	cp_param	�V�X�e������f�[�^
 */
//-----------------------------------------------------------------------------
void TOUCH_SW_Init( TOUCH_SW_SYS* p_touch_sw, const TOUCH_SW_PARAM* cp_param )
{
	touch_sw_init_com(p_touch_sw,cp_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�X�e��������EX �p���b�g�A�j�����[�N��������������
 *
 *	@param	p_touch_sw	�V�X�e�����[�N
 *	@param	cp_param	�V�X�e������f�[�^
 *	@param	palASys		�p���b�g�A�j���V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
void TOUCH_SW_InitEx( TOUCH_SW_SYS* p_touch_sw, const TOUCH_SW_PARAM* cp_param,PALETTE_FADE_PTR palASys)
{
	NNSG2dPaletteData*	pPal;
	void* pSrc;

	touch_sw_init_com(p_touch_sw,cp_param );

	if(palASys == NULL){
		return;
	}
	//�p���b�g�A�j�����[�N�Ƀf�[�^�o�^
	pSrc = GFL_ARC_UTIL_LoadPalette(ARCID_YN_TOUCH,
			NARC_yn_touch_yes_no_touch_NCLR,
			&pPal,p_touch_sw->heapid);

	PaletteWorkSet(palASys,pPal->pRawData,p_touch_sw->bg_frame/4,
			cp_param->pltt_offs*16,TOUCH_SW_USE_PLTT_NUM*32);

	GFL_HEAP_FreeMemory(pSrc);
}


//----------------------------------------------------------------------------
/**
 *	@brief	�V�X�e�����C������
 *
 *	@param	p_touch_sw	�V�X�e�����[�N
 *
 *	@retval	TOUCH_SW_RET_NORMAL	// �����Ȃ�
 *	@retval	TOUCH_SW_RET_YES	// �͂�
 *	@retval	TOUCH_SW_RET_NO	// ������
 *	@retval	TOUCH_SW_RET_YES_TOUCH	// �͂��������u��
 *	@retval	TOUCH_SW_RET_NO_TOUCH	// �������������u��
 */
//-----------------------------------------------------------------------------
u32 TOUCH_SW_MainMC( TOUCH_SW_SYS* p_touch_sw )
{
	u32 ret;

	// �^�b�`�t���O������
	p_touch_sw->touch_flg = TOUCH_FLG_INIT;

	ret = TOUCH_SW_Main( p_touch_sw );

	// �����ꂽ�u�Ԃ��Ԃ�
	if( ret == TOUCH_SW_RET_NORMAL ){
		if( p_touch_sw->touch_flg == GFL_BMN_EVENT_TOUCH ){
			if( p_touch_sw->button_move_flag == TOUCH_SW_BUF_YES ){
				return TOUCH_SW_RET_YES_TOUCH;
			}else{
				return TOUCH_SW_RET_NO_TOUCH;
			}
		}
	}
	return ret;
}


//------------------------------------------------------------------
/*
 *	@brief	�{�^���̏�ԏ�������
 */
//------------------------------------------------------------------
static void touchsw_btn_draw(TOUCH_SW_SYS* p_touch_sw,u8 pos,u8 state)
{
	TOUCH_SW_BUTTON* p_bttn;
	
	p_bttn = &p_touch_sw->button[pos];
	
	TouchSW_ScrnSet( p_bttn->bg_frame,
			p_bttn->p_scrn[state], p_bttn->ofs_x, p_bttn->ofs_y );
	
	GFL_BG_LoadScreenV_Req( p_bttn->bg_frame);
}

//------------------------------------------------------------------
/*
 *	@brief	�L�[���^�b�`�ؑ֎��̕`��ؑ֏���
 */
//------------------------------------------------------------------
static void touchsw_ktst_view_set(TOUCH_SW_SYS* p_touch_sw)
{
	if(p_touch_sw->kt_st == GFL_APP_KTST_KEY){
		//�L�[�ɐ؂�ւ����
		touchsw_btn_draw(p_touch_sw,p_touch_sw->key_pos,TRUE);
		touchsw_btn_draw(p_touch_sw,p_touch_sw->key_pos^1,FALSE);
	}else{
		//�^�b�`�ɐ؂�ւ����
		touchsw_btn_draw(p_touch_sw,p_touch_sw->key_pos,FALSE);
	}
}

//------------------------------------------------------------------
/*
 *	@brief	���͎擾�@�L�[
 */
//------------------------------------------------------------------
static int touchsw_ktst_input_key(TOUCH_SW_SYS* p_touch_sw)
{
	const int keyTrg = GFL_UI_KEY_GetTrg();
	if(keyTrg & PAD_BUTTON_DECIDE){
		// �G�����{�^���ۑ�
		p_touch_sw->button_move_flag = p_touch_sw->key_pos;
#if TSW_USE_SND
		Snd_SePlay( SE_TOUCH_SUB_WIN );
#endif
		return 1;
	}else if(keyTrg & PAD_BUTTON_CANCEL){
		p_touch_sw->button_move_flag = TOUCH_SW_BUF_NO;
#if TSW_USE_SND
		Snd_SePlay( SE_TOUCH_SUB_WIN );
#endif
		return 1;
	}
	if(keyTrg & (PAD_KEY_UP|PAD_KEY_DOWN)){
		p_touch_sw->key_pos ^= 1;
		touchsw_ktst_view_set(p_touch_sw);
#if TSW_USE_SND
		Snd_SePlay( SE_SELECT_SUB_WIN );
#endif
		return 0;
	}
	return 0;
}

//------------------------------------------------------------------
/*
 *	@brief	���͎擾�@���X�g�I��
 */
//------------------------------------------------------------------
static int touchsw_ktst_input(TOUCH_SW_SYS* p_touch_sw)
{
	BOOL  ret;
	const int keyTrg = GFL_UI_KEY_GetTrg();
	
	ret = GFL_BMN_Main( p_touch_sw->p_button_man );
	if(ret){
		p_touch_sw->kt_st = GFL_APP_KTST_TOUCH;
		return 1;
	}
	if(p_touch_sw->kt_st == GFL_APP_KTST_TOUCH){
		if(keyTrg & TOUCH_SW_KEY_TRG){
			p_touch_sw->kt_st = GFL_APP_KTST_KEY;
			touchsw_ktst_view_set(p_touch_sw);
			return 0;
		}
	}
	return touchsw_ktst_input_key(p_touch_sw);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�X�e�����C������
 *
 *	@param	p_touch_sw	�V�X�e�����[�N
 *
 *	@retval	TOUCH_SW_RET_NORMAL	// �����Ȃ�
 *	@retval	TOUCH_SW_RET_YES	// �͂�
 *	@retval	TOUCH_SW_RET_NO		// ������
 */
//-----------------------------------------------------------------------------
u32 TOUCH_SW_Main( TOUCH_SW_SYS* p_touch_sw )
{
	const int anmSpdRate = 2;
	BOOL check;
	
	// ���s�f�[�^�ݒ�ς�
	GF_ASSERT( p_touch_sw->now_work_param == TOUCH_SW_PARAM_DO );

	//�^�b�`���L�[�̃��[�h�ؑւ��`�F�b�N
	// �G�����{�^���������Ƃ��͓��͎擾�����s
	if( p_touch_sw->button_move_flag == TOUCH_SW_BUF_NO_TOUCH ){
		check = touchsw_ktst_input(p_touch_sw);
		if(!check){
			return TOUCH_SW_RET_NORMAL;
		}
		//���肳�ꂽ�u�Ԃ̕\����Ԃ��Z�b�g
		touchsw_btn_draw(p_touch_sw,p_touch_sw->button_move_flag,TRUE);
		touchsw_btn_draw(p_touch_sw,p_touch_sw->button_move_flag^1,FALSE);
		return TOUCH_SW_RET_NORMAL;
	}

	if(p_touch_sw->wait == 8*anmSpdRate){
		//�A�j���I��
		if( p_touch_sw->button_move_flag == TOUCH_SW_BUF_YES ){
			return TOUCH_SW_RET_YES;
		}else{
			return TOUCH_SW_RET_NO;
		}
	}
	//�A�j����
	if(p_touch_sw->wait % (2*anmSpdRate) == 0){
		if((p_touch_sw->wait / (2*anmSpdRate)) % 2 == 0){
			touchsw_btn_draw(p_touch_sw,p_touch_sw->button_move_flag,TRUE);
		}else{
			touchsw_btn_draw(p_touch_sw,p_touch_sw->button_move_flag,FALSE);
		}
	}
	p_touch_sw->wait++;	
	return TOUCH_SW_RET_NORMAL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���݂̑��샂�[�h���擾(�V�X�e���f�[�^���Z�b�g�O�Ɏ擾���邱��)
 *
 *	@param	p_touch_sw	�V�X�e�����[�N
 *
 *	@retval GFL_APP_END_KEY
 *	@retval GFL_APP_END_TOUCH	
 */
//-----------------------------------------------------------------------------
int TOUCH_SW_GetKTStatus( TOUCH_SW_SYS* p_touch_sw )
{
	return p_touch_sw->kt_st;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�X�e���f�[�^���Z�b�g	(Init�̑O�̏�Ԃɂ���@����Ȃ���������Ȃ�)
 *
 *	@param	p_touch_sw	�V�X�e�����[�N
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
void TOUCH_SW_Reset( TOUCH_SW_SYS* p_touch_sw )
{
	int i;
	
	// �{�^���}�l�[�W���j��
	GFL_BMN_Delete( p_touch_sw->p_button_man );

	// �{�^���f�[�^�j��
	for( i=0; i<TOUCH_SW_BUF_NUM; i++ ){
		TouchSW_ButtonDelete( &p_touch_sw->button[ i ] );
	}

	// ���[�N������
	TouchSW_CleanWork( p_touch_sw, p_touch_sw->heapid );
}



//-----------------------------------------------------------------------------
/**
 *		static �֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	���[�N�̏�����
 *
 *	@param	p_touch_sw 
 *	@param	heapid
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void TouchSW_CleanWork( TOUCH_SW_SYS* p_touch_sw, u32 heapid )
{
	memset( p_touch_sw, 0, sizeof(TOUCH_SW_SYS) );

	p_touch_sw->now_work_param = TOUCH_SW_PARAM_NODATA;	// ��ID
	p_touch_sw->heapid = heapid;
	p_touch_sw->button_move_flag = TOUCH_SW_BUF_NO_TOUCH;
	p_touch_sw->touch_flg = TOUCH_FLG_INIT;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^�f�[�^�]������
 *
 *	@param	p_touch_sw
 *	@param	cp_param 
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void TouchSW_SYS_CharTrans( TOUCH_SW_SYS* p_touch_sw, const TOUCH_SW_PARAM* cp_param )
{
	// �L�����N�^�f�[�^�]��
	TouchSW_CharTransReq( ARCID_YN_TOUCH, NARC_yn_touch_yes_no_touch_NCGR, 
			p_touch_sw->bg_frame, cp_param->char_offs,
			p_touch_sw->heapid );
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�f�[�^�]��
 *
 *	@param	p_touch_sw
 *	@param	cp_param 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void TouchSW_SYS_PlttTrans( TOUCH_SW_SYS* p_touch_sw, const TOUCH_SW_PARAM* cp_param )
{
	u32 pal_type;
	if( p_touch_sw->bg_frame < GFL_BG_FRAME0_S ){
		pal_type = PALTYPE_MAIN_BG;
	}else{
		pal_type = PALTYPE_SUB_BG;
	}
	TouchSW_PlttTransReq( ARCID_YN_TOUCH, NARC_yn_touch_yes_no_touch_NCLR,
			pal_type,
			cp_param->pltt_offs * 32, TOUCH_SW_USE_PLTT_NUM * 32,
			p_touch_sw->heapid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���f�[�^������
 *
 *	@param	p_touch_sw
 *	@param	cp_param 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void TouchSW_SYS_ButtonInit( TOUCH_SW_SYS* p_touch_sw, const TOUCH_SW_PARAM* cp_param )
{
	int i,j,datIdx;
	TOUCH_SW_BUTTON_PARAM bt_pr;

	// �{�^���A�j���f�[�^�쐬
	for( i=0; i<TOUCH_SW_ANIME_NUM; i++ ){
		p_touch_sw->anime_tbl[ i ] = TOUCH_SW_ANIME_TIMING * (i+1);
	}

	// �{�^���f�[�^�쐬
	bt_pr.bg_frame	= p_touch_sw->bg_frame;
	bt_pr.fileidx	= ARCID_YN_TOUCH;
	bt_pr.char_offs = cp_param->char_offs;
	bt_pr.pltt_offs	= cp_param->pltt_offs;
	bt_pr.cp_anime_tbl = p_touch_sw->anime_tbl;
	bt_pr.tbl_num	= TOUCH_SW_ANIME_NUM;
	bt_pr.ofs_x		= p_touch_sw->x;

	for( i=0; i<TOUCH_SW_BUF_NUM; i++ ){

		if(cp_param->type == TOUCH_SW_TYPE_S)
		{
			if( i == TOUCH_SW_BUF_YES )
				datIdx = NARC_yn_touch_yes_touch_s01_NSCR;
			else
				datIdx = NARC_yn_touch_no_touch_s01_NSCR;
		}
		else
		{
			if( i == TOUCH_SW_BUF_YES )
				datIdx = NARC_yn_touch_yes_touch_l01_NSCR;
			else
				datIdx = NARC_yn_touch_no_touch_l01_NSCR;
		}

		// �X�N���[���f�[�^�C���f�b�N�X�o�b�t�@�쐬
		for( j=0; j<TOUCH_SW_ANIME_NUM; j++ ){
			bt_pr.scrn_arcidx[ j ] = datIdx + j;
		}

		// Y�I�t�Z�b�g�l
		bt_pr.ofs_y = (i*p_touch_sw->sizy) + p_touch_sw->y;

		// �p���b�g�I�t�Z�b�g
		bt_pr.pltt_offs += i;

		// �{�^��������
		TouchSW_ButtonInit( &p_touch_sw->button[i], &bt_pr, p_touch_sw->heapid );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���}�l�[�W��������
 *
 *	@param	p_touch_sw
 *	@param	cp_param 
 *
 *	@return	
 */
//-----------------------------------------------------------------------------
static void TouchSW_SYS_ButtonManaInit( TOUCH_SW_SYS* p_touch_sw, const TOUCH_SW_PARAM* cp_param )
{
	int i;
	
	// �{�^�������蔻��f�[�^�쐬
	for( i=0; i<TOUCH_SW_BUF_NUM; i++ ){
		p_touch_sw->hit_tbl[ i ].rect.top	= (p_touch_sw->y*8) + (i*p_touch_sw->sizy*8);
		p_touch_sw->hit_tbl[ i ].rect.left	= (p_touch_sw->x*8);
		p_touch_sw->hit_tbl[ i ].rect.bottom = 
			(p_touch_sw->y*8) + (i*p_touch_sw->sizy*8) + (p_touch_sw->sizy*8);
		p_touch_sw->hit_tbl[ i ].rect.right	= (p_touch_sw->x*8) + (p_touch_sw->sizx*8);
	}
	p_touch_sw->hit_tbl[ TOUCH_SW_BUF_NUM ].circle.code = GFL_UI_TP_HIT_END;
	
	p_touch_sw->p_button_man = GFL_BMN_Create( p_touch_sw->hit_tbl, 
							TouchSW_ButtonCallBack, p_touch_sw, p_touch_sw->heapid );
}


//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���}�l�[�W���R�[���o�b�N
 *
 *	@param	button_no
 *	@param	event
 *	@param	p_work 
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void TouchSW_ButtonCallBack( u32 button_no, u32 event, void* p_work )
{
	TOUCH_SW_SYS* p_touch_sw = p_work;
	TOUCH_SW_BUTTON* p_bttn;

	p_touch_sw->touch_flg = event;

	if( event == GFL_BMN_EVENT_TOUCH ){	
		// �G�����{�^���ۑ�
		p_touch_sw->button_move_flag = button_no;
		p_bttn = &p_touch_sw->button[button_no];
		TouchSW_ScrnSet( p_bttn->bg_frame,
			p_bttn->p_scrn[1], p_bttn->ofs_x, p_bttn->ofs_y );
#if TSW_USE_SND
		Snd_SePlay( SE_TOUCH_SUB_WIN );
#endif
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�����[�N������
 *
 *	@param	p_bttn		�{�^�����[�N
 *	@param	cp_param	�{�^���������f�[�^
 *	@param	heapid		�q�[�vID
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void TouchSW_ButtonInit( TOUCH_SW_BUTTON* p_bttn, const TOUCH_SW_BUTTON_PARAM* cp_param, u32 heapid )
{
	int i;
	
	// �A�j���f�[�^�o�^
	{
		TOUCH_SW_ANIME* p_anime = &p_bttn->anime;
		
		p_anime->cp_anime_tbl	= cp_param->cp_anime_tbl;
		p_anime->tbl_num		= cp_param->tbl_num;
		p_anime->frame			= 0;
	}
	// �f�[�^���
	p_bttn->bg_frame= cp_param->bg_frame;
	p_bttn->ofs_x	= cp_param->ofs_x;
	p_bttn->ofs_y	= cp_param->ofs_y;

	// �X�N���[�����[�N�ǂݍ���
	for( i=0; i<TOUCH_SW_ANIME_NUM; i++ ){
		p_bttn->p_scrn_buff[i] = GFL_ARC_UTIL_LoadScreen( cp_param->fileidx, cp_param->scrn_arcidx[i], 
				FALSE, &p_bttn->p_scrn[ i ], heapid );

		// �L�����N�^�I�t�Z�b�g��ݒ�
		TouchSW_ScrnCharOfsSet( p_bttn->p_scrn[ i ], cp_param->char_offs );
		// �p���b�g�I�t�Z�b�g��ݒ�
		TouchSW_ScrnPlttOfsSet( p_bttn->p_scrn[ i ], cp_param->pltt_offs );
	}
	
	//�����X�N���[���̔��f���s��
	TouchSW_ScrnSet( p_bttn->bg_frame,
			p_bttn->p_scrn[ 0 ], p_bttn->ofs_x, p_bttn->ofs_y );

	p_bttn->anm_idx = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���f�[�^�j��
 *
 *	@param	p_bttn		�{�^�����[�N
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void TouchSW_ButtonDelete( TOUCH_SW_BUTTON* p_bttn )
{
	int i;
	
	// �X�N���[���̈��������
	GFL_BG_FillScreen( p_bttn->bg_frame, 0, 
			p_bttn->ofs_x, p_bttn->ofs_y,
			p_bttn->p_scrn[0]->screenWidth/8, p_bttn->p_scrn[0]->screenHeight/8, 0 );
	GFL_BG_LoadScreenV_Req( p_bttn->bg_frame );

	// ���[�N�j��
	for( i=0; i<TOUCH_SW_ANIME_NUM; i++ ){
		GFL_HEAP_FreeMemory( p_bttn->p_scrn_buff[ i ] );
	}

	memset( p_bttn, 0, sizeof( TOUCH_SW_BUTTON ) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^�f�[�^�]�����N�G�X�g
 *
 *	@param	fileIdx		�A�[�J�C�u�t�@�C���C���f�b�N�X
 *	@param	dataIdx		�f�[�^�C���f�b�N�X
 *	@param	bgl			BG�f�[�^
 *	@param	frm			�ڰуi���o�[
 *	@param	offs		�I�t�Z�b�g
 *	@param	heapID		�q�[�v
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void TouchSW_CharTransReq( u32 fileIdx, u32 dataIdx, u32 frm, u32 offs, u32 heapID )
{
	TOUCH_SW_CHARACTER_TRANS* p_tw;

	p_tw = GFL_HEAP_AllocClearMemory( heapID, sizeof(TOUCH_SW_CHARACTER_TRANS) );

	// �L�����N�^�f�[�^�ǂݍ���
	p_tw->p_buff = GFL_ARC_UTIL_LoadBGCharacter( fileIdx, dataIdx, FALSE, &p_tw->p_char, heapID );

	// �]���f�[�^�i�[
	p_tw->bg_frame = frm;
	p_tw->char_ofs = offs;

	// �^�X�N�o�^
	GFUser_VIntr_CreateTCB( TouchSW_CharTransTsk, p_tw, TOUCH_SW_VRAM_TRANS_TSK_PRI );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�f�[�^�]�����N�G�X�g
 *
 *	@param	fileIdx		�A�[�J�C�u�t�@�C���C���f�b�N�X
 *	@param	dataIdx		�f�[�^�C���f�b�N�X
 *	@param	palType		�p���b�g�]����
 *	@param	offs		�I�t�Z�b�g
 *	@param	transSize	�]��size
 *	@param	heapID		�q�[�v
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void TouchSW_PlttTransReq( u32 fileIdx, u32 dataIdx, PALTYPE palType, u32 offs, u32 transSize, u32 heapID )
{
	TOUCH_SW_PALETTE_TRANS* p_tw;

	p_tw = GFL_HEAP_AllocClearMemory( heapID, sizeof(TOUCH_SW_PALETTE_TRANS) );

	// �L�����N�^�f�[�^�ǂݍ���
	p_tw->p_buff = GFL_ARC_UTIL_LoadPalette( fileIdx, dataIdx, &p_tw->p_pltt, heapID );

	// �]���f�[�^�i�[
	p_tw->pltype = palType;
	p_tw->ofs = offs;
	p_tw->size = transSize;

	// �^�X�N�o�^
	GFUser_VIntr_CreateTCB( TouchSW_PlttTransTsk, p_tw, TOUCH_SW_VRAM_TRANS_TSK_PRI );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����N�^�f�[�^�]���^�X�N
 *
 *	@param	tcb
 *	@param	p_work 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void TouchSW_CharTransTsk( GFL_TCB *tcb, void* p_work )
{
	TOUCH_SW_CHARACTER_TRANS* p_tw = p_work;


	DC_FlushRange( p_tw->p_char->pRawData, p_tw->p_char->szByte );
	GFL_BG_LoadCharacter( p_tw->bg_frame, p_tw->p_char->pRawData, 
						p_tw->p_char->szByte, p_tw->char_ofs );

	// ���������
	GFL_TCB_DeleteTask( tcb );
	GFL_HEAP_FreeMemory( p_tw->p_buff );
	GFL_HEAP_FreeMemory( p_tw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�f�[�^�]���^�X�N
 *
 *	@param	tcb
 *	@param	p_work 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void TouchSW_PlttTransTsk( GFL_TCB *tcb, void* p_work )
{
	TOUCH_SW_PALETTE_TRANS* p_tw = p_work;


	DC_FlushRange( p_tw->p_pltt->pRawData, p_tw->size );

	if( p_tw->pltype == PALTYPE_MAIN_BG ){
		GX_LoadBGPltt( p_tw->p_pltt->pRawData, p_tw->ofs, p_tw->size );
	}else if( p_tw->pltype == PALTYPE_SUB_BG ){
		GXS_LoadBGPltt( p_tw->p_pltt->pRawData, p_tw->ofs, p_tw->size );
	}

	// ���������
	GFL_TCB_DeleteTask( tcb );
	GFL_HEAP_FreeMemory( p_tw->p_buff );
	GFL_HEAP_FreeMemory( p_tw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[���f�[�^�]��
 *
 *	@param	frame		BG�i���o�[
 *	@param	scrn		�X�N���[�����[�N
 *	@param	x			�����W
 *	@param	y			�����W
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void TouchSW_ScrnSet( int frame, const NNSG2dScreenData* scrn, int x, int y )
{
	// ��������
	GFL_BG_WriteScreenExpand(
				frame,
				x, y,
				scrn->screenWidth / 8, scrn->screenHeight / 8,
				scrn->rawData,
				0, 0,
				scrn->screenWidth / 8, scrn->screenHeight / 8 );
	
	GFL_BG_LoadScreenV_Req( frame );
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	�X�N���[���f�[�^�̃L�����N�^�l�[���̒l���I�t�Z�b�g�����炷
 *
 *	@param	scrn			�X�N���[���f�[�^
 *	@param	char_offs		�L�����N�^�I�t�Z�b�g	�L�����N�^�P��
 *
 *	@return	none
 *
 * �L�����N�^�l�[���ő�l�`�F�b�N�����Ă��Ȃ��̂Œ��ӂ��Ă�������
 *
 */
//-----------------------------------------------------------------------------
static void TouchSW_ScrnCharOfsSet( const NNSG2dScreenData* scrn, int char_offs )
{
	int i;							// ���[�v�p
	u16* scrndata;					// �X�N���[���f�[�^
	int size = scrn->szByte / 2;	// ���[�v��	�Q=2byte
	
	// �X�N���[���f�[�^���
	scrndata = (u16*)scrn->rawData;
	for(i=0; i<size; i++){

		scrndata[ i ] += char_offs;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�p���b�g�I�t�Z�b�g���X�N���[���o�b�t�@�ɐݒ�
 *
 *	@param	scrn		�X�N���[���o�b�t�@
 *	@param	pltt_offs	�p���b�g�I�t�Z�b�g	(�p���b�g�{���P��)
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void TouchSW_ScrnPlttOfsSet( const NNSG2dScreenData* scrn, int pltt_offs )
{
	int i;							// ���[�v�p
	u16* scrndata;					// �X�N���[���f�[�^
	int size = scrn->szByte / 2;	// ���[�v��	�Q=2byte
	
	// �X�N���[���f�[�^���
	scrndata = (u16*)scrn->rawData;
	for(i=0; i<size; i++){

		scrndata[ i ] &= 0x0fff;
		scrndata[ i ] |= pltt_offs << 12;
	}
}
