//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		debug_tomoya.c
 *	@brief		�l�̍쐬�����V�X�e���̃T���v���\�[�X
 *	@author		tomoya takahashi
 *	@data		2007.01.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include "gflib.h"
#include "procsys.h"
#include "ui.h"
#include "clact.h"

#define __DEBUG_TOMOYA_H_GLOBAL
#include "debug_tomoya.h"

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
// �Z���A�N�^�[�T���v��
#define CLACT_WKNUM	(1)		// CLACT_UNIT���ɍ쐬���郏�[�N�̐�

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�v���Z�X�ɓn����郏�[�N
//=====================================
typedef struct {
	u32 heapID;
} DEBUG_TOMOYA_PROCW;



//-------------------------------------
///	�Z���A�N�^�[�T���v���v���Z�X���[�N
//=====================================
typedef struct {
	void* p_cellbuff;
	void* p_cellanmbuff;
	NNSG2dCellDataBank*		p_cell;
	NNSG2dCellAnimBankData*	p_cellanm;
	NNSG2dImageProxy		imageproxy;
	NNSG2dImagePaletteProxy	plttproxy;
} DEBUG_CLACT_RES;

typedef struct {
	CLUNIT* p_unit;
	DEBUG_CLACT_RES res[ CLACT_WKNUM ];
	CLWK*	p_wk;
} DEBUG_CLACT;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
// ��{�I�ȂƂ���
static void DEBUG_CommonDispInit( void );
static void DEBUG_CommonDispExit( void );
static void* DEBUG_CommonFileLoad( char* path, u32 heapID );


// �Z���A�N�^�[�T���v��
static GFL_PROC_RESULT DEBUG_ClactProcInit( GFL_PROC* p_proc, int* p_seq, void* p_param, void* p_work );
static GFL_PROC_RESULT DEBUG_ClactProcMain( GFL_PROC* p_proc, int* p_seq, void* p_param, void* p_work );
static GFL_PROC_RESULT DEBUG_ClactProcEnd( GFL_PROC* p_proc, int* p_seq, void* p_param, void* p_work );
static const GFL_PROC_DATA DEBUG_ClactProcData = {
	DEBUG_ClactProcInit,
	DEBUG_ClactProcMain,
	DEBUG_ClactProcEnd,
};

static void DEBUG_ClactWorkResLoad( DEBUG_CLACT_RES* p_wk, u32 heapID );
static void DEBUG_ClactWorkResRelease( DEBUG_CLACT_RES* p_wk );
static CLWK* DEBUG_ClactWorkAdd( CLUNIT* p_unit, DEBUG_CLACT_RES* p_res, const CLWK_DATA* cp_data, u32 heapID );
static void DEBUG_ClactWorkDel( CLWK* p_wk );
static void DEBUG_ClactWorkKeyMove( CLWK* p_wk, int trg, int cont );



//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�N	������
 *
 *	@param	heapID	�q�[�vID
 */
//-----------------------------------------------------------------------------
void DEBUG_TomoyaInit( u32 heapID )
{
	DEBUG_TOMOYA_PROCW* p_procw;

	// ���[�N�m��
	p_procw = GFL_HEAP_AllocMemory( heapID, sizeof(DEBUG_TOMOYA_PROCW) );
	p_procw->heapID = heapID;

	GFL_PROC_SysCallProc(NO_OVERLAY_ID, &DEBUG_ClactProcData, p_procw);
}



//----------------------------------------------------------------------------
/**
 *	@brief	��{�I�ȉ�ʐݒ�
 *
 *	@param	heapID	�q�[�vID
 */
//-----------------------------------------------------------------------------
static void DEBUG_CommonDispInit( void )
{
	GFL_DISP_GX_VisibleControlInit();
	GFL_DISP_GXS_VisibleControlInit();
	GFL_DISP_DispOn();
	{
		static const GFL_BG_DISPVRAM param = {
			GX_VRAM_BG_128_B,				// ���C��2D�G���W����BG
			GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
			GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
			GX_VRAM_OBJ_80_EF,				// ���C��2D�G���W����OBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
			GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
			GX_VRAM_TEX_0_A,				// �e�N�X�`���C���[�W�X���b�g
			GX_VRAM_TEXPLTT_0_G				// �e�N�X�`���p���b�g�X���b�g
		};
		GFL_DISP_SetBank( &param );
	}

	// OBJ�}�b�s���O���[�h
	GX_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );
	GXS_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );


	GFL_DISP_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GFL_DISP_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//----------------------------------------------------------------------------
/**
 *	@brief	��{�I�ȉ�ʐݒ�̔j��
 */
//-----------------------------------------------------------------------------
static void DEBUG_CommonDispExit( void )
{
	GFL_DISP_DispOff();
	GFL_DISP_GX_VisibleControlInit();
	GFL_DISP_GXS_VisibleControlInit();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�t�@�C���ǂݍ���
 *
 *	@param	path		�p�X
 *	@param	heapID		�q�[�vID
 *
 *	@return	�f�[�^�|�C���^
 */
//-----------------------------------------------------------------------------
static void* DEBUG_CommonFileLoad( char* path, u32 heapID )
{
	int size, result;
	void* buff;

	// �T�C�Y�擾
	size = GF_GetFileSize( path );

	// �o�b�t�@�쐬
	buff = GFL_HEAP_AllocMemory( heapID, size );

	// �ǂݍ���
	result = GF_ReadFile( path, buff );
	GF_ASSERT( size == result );
	return buff;
}

//-----------------------------------------------------------------------------
/**
 *			�Z���A�N�^�[�T���v���\�[�X
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�T���v���\�[�X
 *
 *	@param	p_proc		�v���Z�X���[�N
 *	@param	p_seq		�V�[�P���X
 *	@param	p_param		�p�����[�^
 *	@param	p_work		�v���Z�X�����[�N
 *
 *	@retval	GFL_PROC_RES_CONTINUE	����p����
 *	@retval	GFL_PROC_RES_FINISH		����I��
 *
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_ClactProcInit( GFL_PROC* p_proc, int* p_seq, void* p_param, void* p_work )
{
	DEBUG_TOMOYA_PROCW* p_procw = p_param;
	DEBUG_CLACT*		p_clactw;

	// ��{�I�ȉ�ʐݒ�
	DEBUG_CommonDispInit();
	
	// clact�T���v�����[�N�쐬
	p_clactw = GFL_PROC_AllocWork( p_proc, sizeof(DEBUG_CLACT), p_procw->heapID );
	memset( p_clactw, 0, sizeof(DEBUG_CLACT) );
	

	// �Z���A�N�^�[�V�X�e��������
	// �܂����̏������s���K�v������܂��B
	{
		static const CLSYS_INIT	param = {
			// ���C���ƃT�u�̃T�[�t�F�[�X������W��ݒ肵�܂��B
			// �T�[�t�F�[�X�̃T�C�Y�́i256,192�j�ɂ���̂����ʂȂ̂ŁA
			// �����o�ɂ͓���܂���ł����B
			// �㉺�̉�ʂ��Ȃ��Ďg�p����Ƃ��́A
			// �T�u�T�[�t�F�[�X�̍�����W��(0, 192)�Ȃǂɂ���K�v������Ǝv���܂��B
			0, 0,		// ���C���T�[�t�F�[�X�̍�����W�ix,y�j
			0, 256,		// �T�u�T�[�t�F�[�X�̍�����W�ix,y�j
			
			// ���̓t����OAMAttr���g�p����ꍇ�̐ݒ�
			// �ʐM�A�C�R���Ȃǂ�0�`3��Oam�̈���g���Ȃ��Ƃ��Ȃǂ́A
			// OAMAttr�Ǘ����ݒ��ύX����K�v������܂��B
			0, 128,		// ���C��OAM�}�l�[�W����OamAttr�Ǘ���(�J�nNo,�Ǘ���)
			0, 128,		// �T�uOAM�}�l�[�W����OamAttr�Ǘ���(�J�nNo,�Ǘ���)
		};
		GFL_CLACT_SysInit( &param, p_procw->heapID );
	}
	
	// �Z���A�N�^�[���j�b�g�쐬
	// �e�A�v���P�[�V�����P�ʂō쐬����B
	p_clactw->p_unit = GFL_CLACT_UnitCreate( CLACT_WKNUM, p_procw->heapID );

	
	// �Z���A�N�^�[���[�N����
	// �Z���A�N�^�[���[�N�𐶐�����菇
	// �P�F�L�����A�p���b�g�A�Z���A�Z���A�j���i�}���`�Z���A�}���`�Z���A�j���j��ǂݍ���
	// �Q�F�L�����A�p���b�g��VRAM�ɓ]������
	// �R�F�o�^�f�[�^���쐬����
	// �S�F�o�^
	// 1.2�ǂݍ��݂Ɠ]��
	{
		static const CLWK_DATA data = {
			64, 64,	//���W(x,y)
			0,		//�A�j���[�V�����V�[�P���X
			0,		//�D�揇��
			0,		//bg�D�揇��
		};
		DEBUG_ClactWorkResLoad( &p_clactw->res[0], p_procw->heapID );
		p_clactw->p_wk = DEBUG_ClactWorkAdd( p_clactw->p_unit, &p_clactw->res[0], &data, p_procw->heapID );
	}


	// �������
	{
		OS_TPrintf( "********�L�[����********\n" );
		OS_TPrintf( "�\���L�[	���@�ړ�\n" );
		OS_TPrintf( "A�{�^��	�A�j���[�V�����X�C�b�`\n" );
		OS_TPrintf( "B�{�^��	�A�j���[�V�����`�F���W\n" );
		OS_TPrintf( "Y�{�^��	���C����ʂ���T�u��ʂɈړ�\n" );
		OS_TPrintf( "X�{�^��	�T�u��ʂ��烁�C����ʂɈړ�\n" );
		
	}
	
	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�T���v���\�[�X
 *
 *	@param	p_proc		�v���Z�X���[�N
 *	@param	p_seq		�V�[�P���X
 *	@param	p_param		�p�����[�^
 *	@param	p_work		�v���Z�X�����[�N
 *
 *	@retval	GFL_PROC_RES_CONTINUE	����p����
 *	@retval	GFL_PROC_RES_FINISH		����I��
 *
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_ClactProcMain( GFL_PROC* p_proc, int* p_seq, void* p_param, void* p_work )
{
	DEBUG_TOMOYA_PROCW* p_procw = p_param;
	DEBUG_CLACT*		p_clactw = p_work;
	int trg, cont;


	trg = GFL_UI_KeyGetTrg();
	cont = GFL_UI_KeyGetCont();
	// �L�[����
	DEBUG_ClactWorkKeyMove( p_clactw->p_wk, trg, cont );

	// �Z���A�N�^�[���j�b�g�`�揈��
	GFL_CLACT_UnitDraw( p_clactw->p_unit );

	// �Z���A�N�^�[�V�X�e�����C������
	// �S���j�b�g�`�悪�������Ă���s���K�v������܂��B
	GFL_CLACT_SysMain();

	// Vintr�҂�
	OS_WaitIrq(TRUE,OS_IE_V_BLANK);
	// V�u�����N���ԂŎ��s���܂��B
	// �����A���j�b�g�̕`�悪�s���Ă��Ȃ��̂�
	// ���̊֐������s����ƁA�`�悵�Ă���OBJ�������Ă��܂�����
	// ���荞�݂Ȃ��ŌĂ΂Ȃ��ق����ǂ���������܂���B
	GFL_CLACT_SysVblank();


	return GFL_PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[�T���v���\�[�X
 *
 *	@param	p_proc		�v���Z�X���[�N
 *	@param	p_seq		�V�[�P���X
 *	@param	p_param		�p�����[�^
 *	@param	p_work		�v���Z�X�����[�N
 *
 *	@retval	GFL_PROC_RES_CONTINUE	����p����
 *	@retval	GFL_PROC_RES_FINISH		����I��
 *
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_ClactProcEnd( GFL_PROC* p_proc, int* p_seq, void* p_param, void* p_work )
{
	DEBUG_TOMOYA_PROCW* p_procw = p_param;
	DEBUG_CLACT*		p_clactw = p_work;

	// DISPLAYOFF
	DEBUG_CommonDispExit();

	// �Z���A�N�^�[���[�N�j��
	DEBUG_ClactWorkDel( p_clactw->p_wk );

	// �Z���A�N�^�[���\�[�X�j��
	DEBUG_ClactWorkResRelease( &p_clactw->res[0] );

	// �Z���A�N�^�[���j�b�g�j��
	GFL_CLACT_UnitDelete( p_clactw->p_unit );

	// �Z���A�N�^�[�V�X�e���j��
	// �S�ẴZ���A�N�^�[���j�b�g��j�����Ă���Ă�ł��������B
	GFL_CLACT_SysExit();

	// ���[�N�j��
	GFL_PROC_FreeWork( p_proc );
	GFL_HEAP_FreeMemory( p_param );

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z���A�N�^�[���\�[�X�ǂݍ���
 *
 *	@param	p_wk		���[�N
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void DEBUG_ClactWorkResLoad( DEBUG_CLACT_RES* p_wk, u32 heapID )
{
	BOOL result;
	void* p_buff;
	NNSG2dCharacterData* p_char;
	NNSG2dPaletteData* p_pltt;
	
	// �L�����N�^�f�[�^�ǂݍ��݁��]��
	{
		p_buff = DEBUG_CommonFileLoad( "src/sample_graphic/hero.NCGR", heapID );
		result = NNS_G2dGetUnpackedCharacterData( p_buff, &p_char );
		GF_ASSERT( result );
		NNS_G2dInitImageProxy( &p_wk->imageproxy );
		NNS_G2dLoadImage1DMapping( 
				p_char, 
				0, 
				NNS_G2D_VRAM_TYPE_2DMAIN, 
				&p_wk->imageproxy );
		NNS_G2dLoadImage1DMapping( 
				p_char, 
				0, 
				NNS_G2D_VRAM_TYPE_2DSUB, 
				&p_wk->imageproxy );
		GFL_HEAP_FreeMemory( p_buff );
	}

	// �p���b�g�f�[�^�ǂݍ��݁��]��
	{
		p_buff = DEBUG_CommonFileLoad( "src/sample_graphic/hero.NCLR", heapID );
		result = NNS_G2dGetUnpackedPaletteData( p_buff, &p_pltt );
		GF_ASSERT( result );
		NNS_G2dInitImagePaletteProxy( &p_wk->plttproxy );
		NNS_G2dLoadPalette( 
				p_pltt, 
				0, 
				NNS_G2D_VRAM_TYPE_2DMAIN, 
				&p_wk->plttproxy );
		NNS_G2dLoadPalette( 
				p_pltt, 
				0, 
				NNS_G2D_VRAM_TYPE_2DSUB, 
				&p_wk->plttproxy );
		GFL_HEAP_FreeMemory( p_buff );
	}

	// �Z���f�[�^�ǂݍ���
	{
		p_wk->p_cellbuff = DEBUG_CommonFileLoad( "src/sample_graphic/hero.NCER", heapID );
		result = NNS_G2dGetUnpackedCellBank( p_wk->p_cellbuff, &p_wk->p_cell );
		GF_ASSERT( result );
	}

	// �Z���A�j���f�[�^�ǂݍ���
	{
		p_wk->p_cellanmbuff = DEBUG_CommonFileLoad( "src/sample_graphic/hero.NANR", heapID );
		result = NNS_G2dGetUnpackedAnimBank( p_wk->p_cellanmbuff, &p_wk->p_cellanm );
		GF_ASSERT( result );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�f�[�^�j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void DEBUG_ClactWorkResRelease( DEBUG_CLACT_RES* p_wk )
{
	GFL_HEAP_FreeMemory( p_wk->p_cellbuff );
	GFL_HEAP_FreeMemory( p_wk->p_cellanmbuff );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�N�^�[�o�^
 *	
 *	@param	p_unit		�Z���A�N�^�[���j�b�g
 *	@param	p_res		���\�[�X���[�N
 *	@param	cp_data		�o�^�f�[�^
 *	@param	heapID		�q�[�v
 *
 *	@return	�����������[�N
 */
//-----------------------------------------------------------------------------
static CLWK* DEBUG_ClactWorkAdd( CLUNIT* p_unit, DEBUG_CLACT_RES* p_res, const CLWK_DATA* cp_data, u32 heapID )
{
	CLWK_RES resdat;
	CLWK* p_wk;

	// ���\�[�X�f�[�^���
	GFL_CLACT_WkSetCellResData( &resdat, 
			&p_res->imageproxy, &p_res->plttproxy,
			p_res->p_cell, p_res->p_cellanm );
	// �o�^
	p_wk = GFL_CLACT_WkAdd( p_unit, 
			cp_data, &resdat,
			// **�o�^��T�[�t�F�[�X�w��{setsf}**
			// �Z���A�N�^�[���j�b�g�ɓƎ������_���[��
			// �ݒ肵�Ă��Ȃ��ꍇ��
			// CLSYS_DRAW_MAIN	���C����ʂɓo�^
			// CLSYS_DRAW_SUB	�T�u��ʂɓo�^
			// �Ǝ������_���[��ݒ肵�Ă���ꍇ��
			// �T�[�t�F�[�X�C���f�b�N�X���w��
			// ��
			CLWK_SETSF_NONE,
			heapID );

	// �I�[�g�A�j���[�V�����ݒ�
	GFL_CLACT_WkSetAutoAnmFlag( p_wk, TRUE );

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�N�^�[�j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void DEBUG_ClactWorkDel( CLWK* p_wk )
{
	GFL_CLACT_WkDel( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�[����
 *
 *	@param	p_wk	���[�N
 *	@param	trg		trg�L�[
 *	@param	cont	cont�L�[
 */
//-----------------------------------------------------------------------------
static void DEBUG_ClactWorkKeyMove( CLWK* p_wk, int trg, int cont )
{
	CLSYS_POS pos;
	BOOL auto_anm;
	u16 anm_seq;
	
	// ���W����ɂ́A��΍��W�ƃT�[�t�F�[�X�����΍��W��
	// �Q�p�^�[��������܂��B
	// GFL_CLACT_WkSetWldPos	Wld�Ƃ��̂���΍��W
	// GFL_CLACT_WkSetPos		�������Ȃ��̂��T�[�t�F�[�X�����΍��W�ł��B
	// �T�[�t�F�[�X�����΍��W�́AGFL_CLACT_WkAdd���Ɠ����ŁA
	// setsf���w�肷��K�v������܂��B
	GFL_CLACT_WkGetWldPos( p_wk, &pos );	
	
	// �\���L�[�ňړ�
	if( cont & PAD_KEY_UP ){
		pos.y --;
	}
	if( cont & PAD_KEY_DOWN ){
		pos.y ++;
	}
	if( cont & PAD_KEY_LEFT ){
		pos.x --;
	}
	if( cont & PAD_KEY_RIGHT ){
		pos.x ++;
	}
	GFL_CLACT_WkSetWldPos( p_wk, &pos );

	// �I�[�g�A�j���[�V����ON
	if( trg & PAD_BUTTON_A ){
		auto_anm = GFL_CLACT_WkGetAutoAnmFlag( p_wk );
		GFL_CLACT_WkSetAutoAnmFlag( p_wk, auto_anm ^ 1 );
	}

	// �A�j���[�V������ς���
	if( trg & PAD_BUTTON_B ){
		anm_seq = GFL_CLACT_WkGetAnmSeq( p_wk );
		anm_seq ^= 1;
		GFL_CLACT_WkSetAnmSeq( p_wk, anm_seq );
	}

	// ���C������T�u��
	if( trg & PAD_BUTTON_Y ){
		GFL_CLACT_WkGetPos( p_wk, &pos, CLSYS_DRAW_MAIN );
		GFL_CLACT_WkSetPos( p_wk, &pos, CLSYS_DRAW_SUB );
	}

	// �T�u���烁�C����
	if( trg & PAD_BUTTON_X ){
		GFL_CLACT_WkGetPos( p_wk, &pos, CLSYS_DRAW_SUB );
		GFL_CLACT_WkSetPos( p_wk, &pos, CLSYS_DRAW_MAIN );
	}
}
