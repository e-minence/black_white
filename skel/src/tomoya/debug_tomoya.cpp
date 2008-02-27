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
#include "main.h"		//HEAPID�Q�Ƃ̂���

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
#define CLACT_WKNUM	(8)		// CLACT_UNIT���ɍ쐬���郏�[�N�̐�

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
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
	GFL_CLUNIT* p_unit;
	DEBUG_CLACT_RES res[ 1 ];
	GFL_CLWK*	p_wk;
	GFL_CLWK*	p_subwk[ CLACT_WKNUM ];
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

extern const GFL_PROC_DATA DebugClactProcData = {
	DEBUG_ClactProcInit,
	DEBUG_ClactProcMain,
	DEBUG_ClactProcEnd,
};

static void DEBUG_ClactWorkResLoad( DEBUG_CLACT_RES* p_wk, u32 heapID );
static void DEBUG_ClactWorkResRelease( DEBUG_CLACT_RES* p_wk );
static GFL_CLWK* DEBUG_ClactWorkAdd( GFL_CLUNIT* p_unit, DEBUG_CLACT_RES* p_res, const GFL_CLWK_DATA* cp_data, u32 heapID );
static void DEBUG_ClactWorkDel( GFL_CLWK* p_wk );
static void DEBUG_ClactWorkKeyMove( GFL_CLWK* p_wk, int trg, int cont );



//----------------------------------------------------------------------------
/**
 *	@brief	��{�I�ȉ�ʐݒ�
 *
 *	@param	heapID	�q�[�vID
 */
//-----------------------------------------------------------------------------
static void DEBUG_CommonDispInit( void )
{
	GFL_DISP_GX_InitVisibleControl();
	GFL_DISP_GXS_InitVisibleControl();
	GFL_DISP_SetDispOn();
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


	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//----------------------------------------------------------------------------
/**
 *	@brief	��{�I�ȉ�ʐݒ�̔j��
 */
//-----------------------------------------------------------------------------
static void DEBUG_CommonDispExit( void )
{
	GFL_DISP_SetDispOff();
	GFL_DISP_GX_InitVisibleControl();
	GFL_DISP_GXS_InitVisibleControl();
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
	DEBUG_CLACT*		p_clactw;

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TOMOYA_DEBUG, 0x10000 );
	// ��{�I�ȉ�ʐݒ�
	DEBUG_CommonDispInit();
	
	// clact�T���v�����[�N�쐬
	p_clactw = (DEBUG_CLACT*)GFL_PROC_AllocWork( p_proc, sizeof(DEBUG_CLACT), HEAPID_TOMOYA_DEBUG );
	GFL_STD_MemFill( p_clactw, 0, sizeof(DEBUG_CLACT) );
	

	// �Z���A�N�^�[�V�X�e��������
	// �܂����̏������s���K�v������܂��B
	{
		static const GFL_CLSYS_INIT	param = {
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
		GFL_CLACT_Init( &GFL_CLSYSINIT_DEF_DIVSCREEN, HEAPID_TOMOYA_DEBUG );
	}
	
	// �Z���A�N�^�[���j�b�g�쐬
	// �e�A�v���P�[�V�����P�ʂō쐬����B
	p_clactw->p_unit = GFL_CLACT_UNIT_Create( CLACT_WKNUM, HEAPID_TOMOYA_DEBUG );

	
	// �Z���A�N�^�[���[�N����
	// �Z���A�N�^�[���[�N�𐶐�����菇
	// �P�F�L�����A�p���b�g�A�Z���A�Z���A�j���i�}���`�Z���A�}���`�Z���A�j���j��ǂݍ���
	// �Q�F�L�����A�p���b�g��VRAM�ɓ]������
	// �R�F�o�^�f�[�^���쐬����
	// �S�F�o�^
	// 1.2�ǂݍ��݂Ɠ]��
	{
		static const GFL_CLWK_DATA data = {
			64, 64,	//���W(x,y)
			0,		//�A�j���[�V�����V�[�P���X
			4,		//�D�揇��
			0,		//bg�D�揇��
		};
		DEBUG_ClactWorkResLoad( &p_clactw->res[0], HEAPID_TOMOYA_DEBUG );
		p_clactw->p_wk = DEBUG_ClactWorkAdd( p_clactw->p_unit, &p_clactw->res[0], &data, HEAPID_TOMOYA_DEBUG );

		{
			static GFL_CLWK_DATA sub_data = {
				0, 64,	//���W(x,y)
				0,		//�A�j���[�V�����V�[�P���X
				0,		//�D�揇��
				0,		//bg�D�揇��
			};
			int i;

			for( i=0; i<4; i++ ){
				sub_data.pos_x += i*16;
				sub_data.softpri = i*4;
				p_clactw->p_subwk[i] = DEBUG_ClactWorkAdd( p_clactw->p_unit, &p_clactw->res[0], &sub_data, HEAPID_TOMOYA_DEBUG );
			}
		}
	}


	// �������
	{
		OS_TPrintf( "********�L�[����********\n" );
		OS_TPrintf( "�\���L�[	���@�ړ�\n" );
		OS_TPrintf( "A�{�^��	�A�j���[�V�����X�C�b�`\n" );
		OS_TPrintf( "B�{�^��	�A�j���[�V�����`�F���W\n" );
		OS_TPrintf( "Y�{�^��	���C����ʂ���T�u��ʂɈړ�\n" );
		OS_TPrintf( "X�{�^��	�T�u��ʂ��烁�C����ʂɈړ�\n" );
		OS_TPrintf( "START�{�^��	���j���[�ɖ߂�\n" );
		
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
	DEBUG_CLACT*		p_clactw = (DEBUG_CLACT*)p_work;
	int trg, cont;


	trg = GFL_UI_KEY_GetTrg();
	cont = GFL_UI_KEY_GetCont();
	// �L�[����
	DEBUG_ClactWorkKeyMove( p_clactw->p_wk, trg, cont );

	// �Z���A�N�^�[���j�b�g�`�揈��
	GFL_CLACT_ClearOamBuff();
	GFL_CLACT_UNIT_Draw( p_clactw->p_unit );

	// �Z���A�N�^�[�V�X�e�����C������
	// �S���j�b�g�`�悪�������Ă���s���K�v������܂��B
	GFL_CLACT_Main();

/*		GFLUser_VIntr�֐�����VBlank�֐����Ă΂��̂ŃR�����g�A�E�g
	// Vintr�҂�
	OS_WaitIrq(TRUE,OS_IE_V_BLANK);
	// V�u�����N���ԂŎ��s���܂��B
	// �������A���̊֐��͊��荞�݂Ȃ��Ŏg�p���Ȃ��ł��������B
	GFL_CLACT_VblankFunc();
	// ���荞�ݓ��Ŏg�p����Ƃ��́AGFL_CLACT_VBlankFuncTransOnly()��
	// GFL_CLACT_ClearOamBuff()���g�p���Ă��������B
//*/
	if( trg & PAD_BUTTON_START ){
		return GFL_PROC_RES_FINISH;
	} else {
		return GFL_PROC_RES_CONTINUE;
	}
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
	DEBUG_CLACT*		p_clactw = (DEBUG_CLACT*)p_work;

	// DISPLAYOFF
	DEBUG_CommonDispExit();

	// �Z���A�N�^�[���[�N�j��
	DEBUG_ClactWorkDel( p_clactw->p_wk );

	// �Z���A�N�^�[���\�[�X�j��
	DEBUG_ClactWorkResRelease( &p_clactw->res[0] );

	// �Z���A�N�^�[���j�b�g�j��
	GFL_CLACT_UNIT_Delete( p_clactw->p_unit );

	// �Z���A�N�^�[�V�X�e���j��
	// �S�ẴZ���A�N�^�[���j�b�g��j�����Ă���Ă�ł��������B
	GFL_CLACT_Exit();

	// ���[�N�j��
	GFL_PROC_FreeWork( p_proc );
	//GFL_HEAP_FreeMemory( p_param );

	GFL_HEAP_DeleteHeap( HEAPID_TOMOYA_DEBUG );

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
static GFL_CLWK* DEBUG_ClactWorkAdd( GFL_CLUNIT* p_unit, DEBUG_CLACT_RES* p_res, const GFL_CLWK_DATA* cp_data, u32 heapID )
{
	GFL_CLWK_RES resdat;
	GFL_CLWK* p_wk;

	// ���\�[�X�f�[�^���
	GFL_CLACT_WK_SetCellResData( &resdat, 
			&p_res->imageproxy, &p_res->plttproxy,
			p_res->p_cell, p_res->p_cellanm );
	// �o�^
	p_wk = GFL_CLACT_WK_Add( p_unit, 
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
	GFL_CLACT_WK_SetAutoAnmFlag( p_wk, TRUE );

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�N�^�[�j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void DEBUG_ClactWorkDel( GFL_CLWK* p_wk )
{
	GFL_CLACT_WK_Remove( p_wk );
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
static void DEBUG_ClactWorkKeyMove( GFL_CLWK* p_wk, int trg, int cont )
{
	GFL_CLACTPOS pos;
	BOOL auto_anm;
	u16 anm_seq;
	
	// ���W����ɂ́A��΍��W�ƃT�[�t�F�[�X�����΍��W��
	// �Q�p�^�[��������܂��B
	// GFL_CLACT_WK_SetWldPos	Wld�Ƃ��̂���΍��W
	// GFL_CLACT_WK_SetPos		�������Ȃ��̂��T�[�t�F�[�X�����΍��W�ł��B
	// �T�[�t�F�[�X�����΍��W�́AGFL_CLACT_WK_Add���Ɠ����ŁA
	// setsf���w�肷��K�v������܂��B
	GFL_CLACT_WK_GetWldPos( p_wk, &pos );	
	
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
	GFL_CLACT_WK_SetWldPos( p_wk, &pos );

	// �I�[�g�A�j���[�V����ON
	if( trg & PAD_BUTTON_A ){
		auto_anm = GFL_CLACT_WK_GetAutoAnmFlag( p_wk );
		GFL_CLACT_WK_SetAutoAnmFlag( p_wk, auto_anm ^ 1 );
	}

	// �A�j���[�V������ς���
	if( trg & PAD_BUTTON_B ){
		anm_seq = GFL_CLACT_WK_GetAnmSeq( p_wk );
		anm_seq ^= 1;
		GFL_CLACT_WK_SetAnmSeq( p_wk, anm_seq );

		// �V�[�P���X����\��
		OS_Printf( "seq %d\n", GFL_CLACT_WK_GetAnmSeqNum( p_wk ) );
		// �V�[�P���X�f�[�^�擾
		OS_Printf( "seq data %d\n", GFL_CLACT_WK_GetUserAttrAnmSeqNow(p_wk) );
	}

	// ���C������T�u��
	if( trg & PAD_BUTTON_Y ){
#if 1
		GFL_CLACT_WK_GetPos( p_wk, &pos, CLSYS_DRAW_MAIN );
		GFL_CLACT_WK_SetPos( p_wk, &pos, CLSYS_DRAW_SUB );
#else
		int pal;
		pal = GFL_CLACT_WK_GetSoftPri( p_wk );
		pal ++;
		GFL_CLACT_WK_SetSoftPri( p_wk, pal );
		OS_Printf( "pri %d\n", pal );
#endif
	}

	// �T�u���烁�C����
	if( trg & PAD_BUTTON_X ){
#if 1
		GFL_CLACT_WK_GetPos( p_wk, &pos, CLSYS_DRAW_SUB );
		GFL_CLACT_WK_SetPos( p_wk, &pos, CLSYS_DRAW_MAIN );
#else
		int pal;
		pal = GFL_CLACT_WK_GetSoftPri( p_wk );
		pal --;
		GFL_CLACT_WK_SetSoftPri( p_wk, pal );
		OS_Printf( "pri %d\n", pal );
#endif
	}
}
