//============================================================================================
/**
 * @file		zknsearch_list.c
 * @brief		�}�ӌ������ �����I�����X�g����
 * @author	Hiroyuki Nakamura
 * @date		10.02.10
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/main.h"
#include "msg/msg_zukan_search.h"

#include "zknsearch_main.h"
#include "zknsearch_bmp.h"
#include "zknsearch_obj.h"
#include "zknsearch_list.h"
#include "zukan_gra.naix"


//============================================================================================
//============================================================================================
static void RowListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void RowListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void RowListCallBack_Scroll( void * work, s8 mv );

static void NameListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void NameListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void NameListCallBack_Scroll( void * work, s8 mv );

static void TypeListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void TypeListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void TypeListCallBack_Scroll( void * work, s8 mv );

static void ColorListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void ColorListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void ColorListCallBack_Scroll( void * work, s8 mv );

static void FormListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void FormListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void FormListCallBack_Scroll( void * work, s8 mv );


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
// �Ȃ��
static const FRAMELIST_TOUCH_DATA RowTouchHitTbl[] =
{
	{ {   0,  23, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_ITEM },
	{ {  24,  47, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_ITEM },
	{ {  48,  71, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_ITEM },
	{ {  72,  95, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_ITEM },
	{ {  96, 119, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_ITEM },
	{ { 120, 143, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_ITEM },

	{ { GFL_UI_TP_HIT_END, 0, 0, 0 }, 0 },
};

static const FRAMELIST_CALLBACK	RowListCallBack = {
	RowListCallBack_Draw,
	RowListCallBack_Move,
	RowListCallBack_Scroll,
};

static const FRAMELIST_HEADER	RowListHeader = {
	GFL_BG_FRAME1_M,							// ����ʂa�f
	GFL_BG_FRAME1_S,							// ���ʂa�f

	16,					// ���ڃt���[���\���J�n�w���W
	0,					// ���ڃt���[���\���J�n�x���W
	16,					// ���ڃt���[���\���w�T�C�Y
	3,					// ���ڃt���[���\���x�T�C�Y

	3,							// �t���[�����ɕ\������BMPWIN�̕\���w���W			
	0,							// �t���[�����ɕ\������BMPWIN�̕\���x���W
	11,							// �t���[�����ɕ\������BMPWIN�̕\���w�T�C�Y
	3,							// �t���[�����ɕ\������BMPWIN�̕\���x�T�C�Y
	1,							// �t���[�����ɕ\������BMPWIN�̃p���b�g

	{ 24, 12, 8, 6, 4, 3 },		// �X�N���[�����x [0] = �ő�

	3,							// �I�����ڂ̃p���b�g

	8,							// �X�N���[���o�[�̂x�T�C�Y

	6,							// ���ړo�^��
	1,							// �w�i�o�^��

	0,							// �����ʒu
	6,							// �J�[�\���ړ��͈�
	0,							// �����X�N���[���l

	RowTouchHitTbl,			// �^�b�`�f�[�^

	&RowListCallBack,	// �R�[���o�b�N�֐�
	NULL,
};

//--------------------------------------------------------------------------------------------
// �Ȃ܂�
static const FRAMELIST_TOUCH_DATA NameTouchHitTbl[] =
{
	{ {   0,  23, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  24,  47, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  48,  71, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  72,  95, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  96, 119, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 120, 143, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 144, 167, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },

	{ {   8, 160, 232, 255 }, FRAMELIST_TOUCH_PARAM_RAIL },					// 07: ���[��

	{ { 168, 191, 184, 207 }, FRAMELIST_TOUCH_PARAM_PAGE_UP },			// 08: ��
	{ { 168, 191, 208, 231 }, FRAMELIST_TOUCH_PARAM_PAGE_DOWN },		// 09: �E

	{ { GFL_UI_TP_HIT_END, 0, 0, 0 }, 0 },
};

static const FRAMELIST_CALLBACK	NameListCallBack = {
	NameListCallBack_Draw,
	NameListCallBack_Move,
	NameListCallBack_Scroll,
};

static const FRAMELIST_HEADER	NameListHeader = {
	GFL_BG_FRAME1_M,							// ����ʂa�f
	GFL_BG_FRAME1_S,							// ���ʂa�f

	16,					// ���ڃt���[���\���J�n�w���W
	0,					// ���ڃt���[���\���J�n�x���W
	16,					// ���ڃt���[���\���w�T�C�Y
	3,					// ���ڃt���[���\���x�T�C�Y

	3,							// �t���[�����ɕ\������BMPWIN�̕\���w���W			
	0,							// �t���[�����ɕ\������BMPWIN�̕\���x���W
	11,							// �t���[�����ɕ\������BMPWIN�̕\���w�T�C�Y
	3,							// �t���[�����ɕ\������BMPWIN�̕\���x�T�C�Y
	1,							// �t���[�����ɕ\������BMPWIN�̃p���b�g

	{ 24, 12, 8, 6, 4, 3 },		// �X�N���[�����x [0] = �ő�

	3,							// �I�����ڂ̃p���b�g

	8,							// �X�N���[���o�[�̂x�T�C�Y

	50,							// ���ړo�^��
	1,							// �w�i�o�^��

	0,							// �����ʒu
	7,							// �J�[�\���ړ��͈�
	0,							// �����X�N���[���l

	NameTouchHitTbl,			// �^�b�`�f�[�^

	&NameListCallBack,	// �R�[���o�b�N�֐�
	NULL,
};

//--------------------------------------------------------------------------------------------
// �^�C�v
static const FRAMELIST_TOUCH_DATA TypeTouchHitTbl[] =
{
	{ {   0,  23, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  24,  47, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  48,  71, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  72,  95, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  96, 119, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 120, 143, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 144, 167, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },

	{ {   8, 160, 232, 255 }, FRAMELIST_TOUCH_PARAM_RAIL },					// 07: ���[��

	{ { 168, 191, 184, 207 }, FRAMELIST_TOUCH_PARAM_PAGE_UP },			// 08: ��
	{ { 168, 191, 208, 231 }, FRAMELIST_TOUCH_PARAM_PAGE_DOWN },		// 09: �E

	{ { GFL_UI_TP_HIT_END, 0, 0, 0 }, 0 },
};

static const FRAMELIST_CALLBACK	TypeListCallBack = {
	TypeListCallBack_Draw,
	TypeListCallBack_Move,
	TypeListCallBack_Scroll,
};

static const FRAMELIST_HEADER	TypeListHeader = {
	GFL_BG_FRAME1_M,							// ����ʂa�f
	GFL_BG_FRAME1_S,							// ���ʂa�f

	16,					// ���ڃt���[���\���J�n�w���W
	0,					// ���ڃt���[���\���J�n�x���W
	16,					// ���ڃt���[���\���w�T�C�Y
	3,					// ���ڃt���[���\���x�T�C�Y

	3,							// �t���[�����ɕ\������BMPWIN�̕\���w���W			
	0,							// �t���[�����ɕ\������BMPWIN�̕\���x���W
	11,							// �t���[�����ɕ\������BMPWIN�̕\���w�T�C�Y
	3,							// �t���[�����ɕ\������BMPWIN�̕\���x�T�C�Y
	1,							// �t���[�����ɕ\������BMPWIN�̃p���b�g

	{ 24, 12, 8, 6, 4, 3 },		// �X�N���[�����x [0] = �ő�

	3,							// �I�����ڂ̃p���b�g

	8,							// �X�N���[���o�[�̂x�T�C�Y

	50,							// ���ړo�^��
	1,							// �w�i�o�^��

	0,							// �����ʒu
	7,							// �J�[�\���ړ��͈�
	0,							// �����X�N���[���l

	TypeTouchHitTbl,			// �^�b�`�f�[�^

	&TypeListCallBack,	// �R�[���o�b�N�֐�
	NULL,
};

//--------------------------------------------------------------------------------------------
// ����
static const FRAMELIST_TOUCH_DATA ColorTouchHitTbl[] =
{
	{ {   0,  23, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  24,  47, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  48,  71, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  72,  95, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  96, 119, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 120, 143, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 144, 167, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },

	{ {   8, 160, 232, 255 }, FRAMELIST_TOUCH_PARAM_RAIL },					// 07: ���[��

	{ { 168, 191, 184, 207 }, FRAMELIST_TOUCH_PARAM_PAGE_UP },			// 08: ��
	{ { 168, 191, 208, 231 }, FRAMELIST_TOUCH_PARAM_PAGE_DOWN },		// 09: �E

	{ { GFL_UI_TP_HIT_END, 0, 0, 0 }, 0 },
};

static const FRAMELIST_CALLBACK	ColorListCallBack = {
	ColorListCallBack_Draw,
	ColorListCallBack_Move,
	ColorListCallBack_Scroll,
};

static const FRAMELIST_HEADER	ColorListHeader = {
	GFL_BG_FRAME1_M,							// ����ʂa�f
	GFL_BG_FRAME1_S,							// ���ʂa�f

	16,					// ���ڃt���[���\���J�n�w���W
	0,					// ���ڃt���[���\���J�n�x���W
	16,					// ���ڃt���[���\���w�T�C�Y
	3,					// ���ڃt���[���\���x�T�C�Y

	3,							// �t���[�����ɕ\������BMPWIN�̕\���w���W			
	0,							// �t���[�����ɕ\������BMPWIN�̕\���x���W
	11,							// �t���[�����ɕ\������BMPWIN�̕\���w�T�C�Y
	3,							// �t���[�����ɕ\������BMPWIN�̕\���x�T�C�Y
	1,							// �t���[�����ɕ\������BMPWIN�̃p���b�g

	{ 24, 12, 8, 6, 4, 3 },		// �X�N���[�����x [0] = �ő�

	3,							// �I�����ڂ̃p���b�g

	8,							// �X�N���[���o�[�̂x�T�C�Y

	50,							// ���ړo�^��
	1,							// �w�i�o�^��

	0,							// �����ʒu
	7,							// �J�[�\���ړ��͈�
	0,							// �����X�N���[���l

	ColorTouchHitTbl,			// �^�b�`�f�[�^

	&ColorListCallBack,	// �R�[���o�b�N�֐�
	NULL,
};

//--------------------------------------------------------------------------------------------
// �t�H����
static const FRAMELIST_TOUCH_DATA FormTouchHitTbl[] =
{
	{ {   0,  39, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  40,  79, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  80,  71, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 120, 159, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },

	{ {   8, 160, 232, 255 }, FRAMELIST_TOUCH_PARAM_RAIL },					// 07: ���[��

	{ { 168, 191, 184, 207 }, FRAMELIST_TOUCH_PARAM_PAGE_UP },			// 08: ��
	{ { 168, 191, 208, 231 }, FRAMELIST_TOUCH_PARAM_PAGE_DOWN },		// 09: �E

	{ { GFL_UI_TP_HIT_END, 0, 0, 0 }, 0 },
};

static const FRAMELIST_CALLBACK	FormListCallBack = {
	FormListCallBack_Draw,
	FormListCallBack_Move,
	FormListCallBack_Scroll,
};

static const FRAMELIST_HEADER	FormListHeader = {
	GFL_BG_FRAME1_M,							// ����ʂa�f
	GFL_BG_FRAME1_S,							// ���ʂa�f

	16,					// ���ڃt���[���\���J�n�w���W
	0,					// ���ڃt���[���\���J�n�x���W
	16,					// ���ڃt���[���\���w�T�C�Y
	5,					// ���ڃt���[���\���x�T�C�Y

	4,							// �t���[�����ɕ\������BMPWIN�̕\���w���W			
	1,							// �t���[�����ɕ\������BMPWIN�̕\���x���W
	1,							// �t���[�����ɕ\������BMPWIN�̕\���w�T�C�Y
	1,							// �t���[�����ɕ\������BMPWIN�̕\���x�T�C�Y
	1,							// �t���[�����ɕ\������BMPWIN�̃p���b�g

	{ 40, 20, 10, 8, 5, 4 },		// �X�N���[�����x [0] = �ő�

	3,							// �I�����ڂ̃p���b�g

	8,							// �X�N���[���o�[�̂x�T�C�Y

	ZKNSEARCHOBJ_FORM_MAX,	// ���ړo�^��
	1,											// �w�i�o�^��

	0,							// �����ʒu
	4,							// �J�[�\���ړ��͈�
	0,							// �����X�N���[���l

	FormTouchHitTbl,			// �^�b�`�f�[�^

	&FormListCallBack,	// �R�[���o�b�N�֐�
	NULL,
};





void ZKNSEARCHLIST_MakeRowList( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	// ���X�g���[�N�쐬
	{
		FRAMELIST_HEADER	hed;

		hed = RowListHeader;
		hed.initPos    = 0;
		hed.initScroll = 0;
		hed.cbWork     = wk;

		wk->lwk = FRAMELIST_Create( &hed, HEAPID_ZUKAN_SEARCH );
	}

	// ���ڔw�i�ݒ�
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_search_searchframe_NSCR, FALSE, 0 );

	// �_�ŃA�j���p���b�g�ݒ�
	FRAMELIST_LoadBlinkPalette( wk->lwk, ah, NARC_zukan_gra_search_search_bgd_NCLR, 2, 3 );

	GFL_ARC_CloseDataHandle( ah );

	{	// ���X�g�p�����[�^�ݒ�
		u32	i;

		for( i=0; i<6; i++ ){
			FRAMELIST_AddItem( wk->lwk, 0, i );
			wk->item[i] = GFL_MSG_CreateString( wk->mman, ZKN_SEARCH_SORT_01+i );
		}
	}
}

void ZKNSEARCHLIST_MakeNameList( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	// ���X�g���[�N�쐬
	{
		FRAMELIST_HEADER	hed;

		hed = NameListHeader;
		hed.initPos    = 0;
		hed.initScroll = 0;
		hed.cbWork     = wk;

		wk->lwk = FRAMELIST_Create( &hed, HEAPID_ZUKAN_SEARCH );
	}

	// ���ڔw�i�ݒ�
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_search_searchframe_NSCR, FALSE, 0 );

	// �_�ŃA�j���p���b�g�ݒ�
	FRAMELIST_LoadBlinkPalette( wk->lwk, ah, NARC_zukan_gra_search_search_bgd_NCLR, 2, 3 );

	GFL_ARC_CloseDataHandle( ah );

	{	// ���X�g�p�����[�^�ݒ�
		u32	i;

		for( i=ZKN_SEARCH_INITIAL_01; i<=ZKN_SEARCH_INITIAL_44; i++ ){
			FRAMELIST_AddItem( wk->lwk, 0, i-ZKN_SEARCH_INITIAL_01 );
			wk->item[i-ZKN_SEARCH_INITIAL_01] = GFL_MSG_CreateString( wk->mman, i );
		}
	}
}

void ZKNSEARCHLIST_MakeTypeList( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	// ���X�g���[�N�쐬
	{
		FRAMELIST_HEADER	hed;

		hed = TypeListHeader;
		hed.initPos    = 0;
		hed.initScroll = 0;
		hed.cbWork     = wk;

		wk->lwk = FRAMELIST_Create( &hed, HEAPID_ZUKAN_SEARCH );
	}

	// ���ڔw�i�ݒ�
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_search_searchframe_NSCR, FALSE, 0 );

	// �_�ŃA�j���p���b�g�ݒ�
	FRAMELIST_LoadBlinkPalette( wk->lwk, ah, NARC_zukan_gra_search_search_bgd_NCLR, 2, 3 );

	GFL_ARC_CloseDataHandle( ah );

	{	// ���X�g�p�����[�^�ݒ�
		u32	i;

		for( i=ZKN_SEARCH_TYPE_01; i<=ZKN_SEARCH_TYPE_17; i++ ){
			FRAMELIST_AddItem( wk->lwk, 0, i-ZKN_SEARCH_TYPE_01 );
			wk->item[i-ZKN_SEARCH_TYPE_01] = GFL_MSG_CreateString( wk->mman, i );
		}
	}
}

void ZKNSEARCHLIST_MakeColorList( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	// ���X�g���[�N�쐬
	{
		FRAMELIST_HEADER	hed;

		hed = ColorListHeader;
		hed.initPos    = 0;
		hed.initScroll = 0;
		hed.cbWork     = wk;

		wk->lwk = FRAMELIST_Create( &hed, HEAPID_ZUKAN_SEARCH );
	}

	// ���ڔw�i�ݒ�
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_search_searchframe_NSCR, FALSE, 0 );

	// �_�ŃA�j���p���b�g�ݒ�
	FRAMELIST_LoadBlinkPalette( wk->lwk, ah, NARC_zukan_gra_search_search_bgd_NCLR, 2, 3 );

	GFL_ARC_CloseDataHandle( ah );

	{	// ���X�g�p�����[�^�ݒ�
		u32	i;

		for( i=ZKN_SEARCH_COLOR_00; i<=ZKN_SEARCH_COLOR_09; i++ ){
			FRAMELIST_AddItem( wk->lwk, 0, i-ZKN_SEARCH_COLOR_00 );
			wk->item[i-ZKN_SEARCH_COLOR_00] = GFL_MSG_CreateString( wk->mman, i );
		}
	}
}

void ZKNSEARCHLIST_MakeFormList( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	// ���X�g���[�N�쐬
	{
		FRAMELIST_HEADER	hed;

		hed = FormListHeader;
		hed.initPos    = 0;
		hed.initScroll = 0;
		hed.cbWork     = wk;

		wk->lwk = FRAMELIST_Create( &hed, HEAPID_ZUKAN_SEARCH );
	}

	// ���ڔw�i�ݒ�
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_search_searchframe2_NSCR, FALSE, 0 );

	// �_�ŃA�j���p���b�g�ݒ�
	FRAMELIST_LoadBlinkPalette( wk->lwk, ah, NARC_zukan_gra_search_search_bgd_NCLR, 2, 3 );

	GFL_ARC_CloseDataHandle( ah );

	{	// ���X�g�p�����[�^�ݒ�
		u32	i;

		for( i=0; i<ZKNSEARCHOBJ_FORM_MAX; i++ ){
			FRAMELIST_AddItem( wk->lwk, 0, i );
		}
	}
}



//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���X�g�폜
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHLIST_FreeList( ZKNSEARCHMAIN_WORK * wk )
{
	u32	i;

	FRAMELIST_Exit( wk->lwk );

	// ���ڍ폜
	for( i=0; i<ZKNSEARCHMAIN_ITEM_MAX; i++ ){
		if( wk->item[i] != NULL ){
			GFL_STR_DeleteBuffer( wk->item[i] );
			wk->item[i] = NULL;
		}
	}
}



static void RowListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHBMP_PutListItem( wk, util, wk->item[itemNum] );

	if( wk->dat->sort->row == itemNum ){
		ZKNSEARCHOBJ_PutMark( wk, 0, py, disp );
	}
}

static void RowListCallBack_Move( void * work, u32 listPos, BOOL flg )
{
}

static void RowListCallBack_Scroll( void * work, s8 mv )
{
}


static void NameListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHBMP_PutListItem( wk, util, wk->item[itemNum] );

	if( wk->dat->sort->name == itemNum ){
		ZKNSEARCHOBJ_PutMark( wk, 0, py, disp );
	}
}

static void NameListCallBack_Move( void * work, u32 listPos, BOOL flg )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
}

static void NameListCallBack_Scroll( void * work, s8 mv )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHOBJ_ScrollList( wk, mv*-1 );

	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
	ZKNSEARCHOBJ_SetListPageArrowAnime( wk, TRUE );
}

static void TypeListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHBMP_PutListItem( wk, util, wk->item[itemNum] );

	if( wk->dat->sort->type1 == itemNum ){
		ZKNSEARCHOBJ_PutMark( wk, 0, py, disp );
	}
	if( wk->dat->sort->type2 == itemNum ){
		ZKNSEARCHOBJ_PutMark( wk, 1, py, disp );
	}
}

static void TypeListCallBack_Move( void * work, u32 listPos, BOOL flg )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
}

static void TypeListCallBack_Scroll( void * work, s8 mv )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHOBJ_ScrollList( wk, mv*-1 );

	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
	ZKNSEARCHOBJ_SetListPageArrowAnime( wk, TRUE );
}

static void ColorListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHBMP_PutListItem( wk, util, wk->item[itemNum] );

	if( wk->dat->sort->color == itemNum ){
		ZKNSEARCHOBJ_PutMark( wk, 0, py, disp );
	}
}

static void ColorListCallBack_Move( void * work, u32 listPos, BOOL flg )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
}

static void ColorListCallBack_Scroll( void * work, s8 mv )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHOBJ_ScrollList( wk, mv*-1 );

	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
	ZKNSEARCHOBJ_SetListPageArrowAnime( wk, TRUE );
}

static void FormListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHBMP_PutFormListItem( wk, util );

	if( wk->dat->sort->form == itemNum ){
		ZKNSEARCHOBJ_PutFormMark( wk, py, disp );
	}
	ZKNSEARCHOBJ_PutFormList( wk, itemNum, py, disp );
}

static void FormListCallBack_Move( void * work, u32 listPos, BOOL flg )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
}

static void FormListCallBack_Scroll( void * work, s8 mv )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHOBJ_ScrollFormList( wk, mv*-1 );

	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
	ZKNSEARCHOBJ_SetListPageArrowAnime( wk, TRUE );
}
