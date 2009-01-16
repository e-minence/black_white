//=============================================================================================
/**
 * @file	btlv_core.c
 * @brief	�|�P����WB �o�g���`�惁�C�����W���[��
 * @author	taya
 *
 * @date	2008.10.02	�쐬
 */
//=============================================================================================
#include <tcbl.h>

#include "print/gf_font.h"

#include "arc_def.h"
#include "font/font.naix"

#include "btl_common.h"
#include "btl_main.h"
#include "btl_action.h"
#include "btl_calc.h"
#include "btl_util.h"

#include "btlv_scu.h"
#include "btlv_scd.h"

#include "btlv_core.h"

#include "btl_effect.h"	//soga
#include "waza_tool/wazano_def.h"		//soga


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
	STR_BUFFER_SIZE = 384,
	GENERIC_WORK_SIZE = 128,
};

/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/
typedef BOOL (*pCmdProc)( BTLV_CORE*, int*, void* );

/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/
struct _BTLV_CORE {

	BTL_MAIN_MODULE*	mainModule;
	const BTL_CLIENT*	myClient;
	u8					myClientID;

	BtlvCmd		processingCmd;
	pCmdProc	mainProc;
	int			mainSeq;
	BTL_PROC	subProc;
	u8			genericWork[ GENERIC_WORK_SIZE ];

	BTL_ACTION_PARAM	actionParam;
	STRBUF*				strBuf;
	GFL_FONT*			fontHandle;

	GFL_TCBLSYS*	tcbl;
	BTLV_SCU*		scrnU;
	BTLV_SCD*		scrnD;

	HEAPID	heapID;
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void* getGenericWork( BTLV_CORE* core, u32 size );
static BOOL CmdProc_Setup( BTLV_CORE* core, int* seq, void* workBuffer );
static BOOL CmdProc_SelectAction( BTLV_CORE* core, int* seq, void* workBufer );
static BOOL CmdProc_SelectPokemon( BTLV_CORE* core, int* seq, void* workBufer );
static BOOL subprocWazaAct( int* seq, void* wk_adrs );
static BOOL subprocMemberIn( int* seq, void* wk_adrs );
static void setup_core( BTLV_CORE* wk, HEAPID heapID );
static void cleanup_core( BTLV_CORE* wk );



//=============================================================================================
/**
 * �`�惁�C�����W���[���̐���
 *
 * @param   mainModule		�V�X�e�����C�����W���[���̃n���h��
 * @param   heapID			������q�[�vID
 *
 * @retval  BTLV_CORE*		�������ꂽ�`�惁�C�����W���[���̃n���h��
 */
//=============================================================================================
BTLV_CORE*  BTLV_Create( BTL_MAIN_MODULE* mainModule, const BTL_CLIENT* client, HEAPID heapID )
{
	BTLV_CORE* core = GFL_HEAP_AllocMemory( heapID, sizeof(BTLV_CORE) );

	core->mainModule = mainModule;
	core->myClient = client;
	core->myClientID = BTL_CLIENT_GetClientID( client );
	core->processingCmd = BTLV_CMD_NULL;
	core->heapID = heapID;
	core->strBuf = GFL_STR_CreateBuffer( STR_BUFFER_SIZE, heapID );
	core->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_nftr,
						GFL_FONT_LOADTYPE_FILE, FALSE, heapID );


	core->tcbl = GFL_TCBL_Init( heapID, heapID, 64, 128 );
	core->scrnU = BTLV_SCU_Create( core, core->mainModule, core->tcbl, core->fontHandle, heapID );
	core->scrnD = BTLV_SCD_Create( core, core->mainModule, core->tcbl, core->fontHandle, heapID );

	core->mainProc = NULL;
	core->mainSeq = 0;

	BTL_STR_InitSystem( mainModule, client, heapID );
	GFL_UI_TP_Init( heapID );


	return core;
}

//=============================================================================================
/**
 * �`�惁�C�����W���[���̔j��
 *
 * @param   core		�`�惁�C�����W���[��
 *
 */
//=============================================================================================
void BTLV_Delete( BTLV_CORE* core )
{
	GFL_UI_TP_Exit();
	BTL_STR_QuitSystem();

	//�G�t�F�N�g�폜 soga
	BTL_EFFECT_Exit();

	BTLV_SCD_Delete( core->scrnD );
	BTLV_SCU_Delete( core->scrnU );
	cleanup_core( core );

	GFL_TCBL_Exit( core->tcbl );
	GFL_STR_DeleteBuffer( core->strBuf );
	GFL_FONT_Delete( core->fontHandle );
	GFL_HEAP_FreeMemory( core );
}

//=============================================================================================
/**
 * �`�惁�C�����[�v
 *
 * @param   core		
 *
 */
//=============================================================================================
void BTLV_CORE_Main( BTLV_CORE* core )
{
	GFL_UI_TP_Main();
	GFL_TCBL_Main( core->tcbl );
	BTL_EFFECT_Main();		//soga
}


//=============================================================================================
/**
  * �`��R�}���h���s
 *
 * @param   core		�`�惁�C�����W���[���̃n���h��
 * @param   client		�֘A�N���C�A���g���W���[���n���h��
 * @param   cmd			�`��R�}���h
 *
 */
//=============================================================================================
void BTLV_StartCommand( BTLV_CORE* core, BtlvCmd cmd )
{
	GF_ASSERT( core->processingCmd == BTLV_CMD_NULL );

	{
		static const struct {
			BtlvCmd		cmd;
			pCmdProc	proc;
		}procTbl[] = {
			{ BTLV_CMD_SETUP,			CmdProc_Setup },
			{ BTLV_CMD_SELECT_ACTION,	CmdProc_SelectAction },
			{ BTLV_CMD_SELECT_POKEMON,	CmdProc_SelectPokemon },
		};

		int i;

		for(i=0; i<NELEMS(procTbl); i++)
		{
			if( procTbl[i].cmd == cmd )
			{
				core->processingCmd = cmd;
				core->mainProc = procTbl[i].proc;
				core->mainSeq = 0;
				return;
			}
		}

		GF_ASSERT(0);
	}
}

//=============================================================================================
/**
 * BTLV_StartCommand �Ŏ󂯕t�����`��R�}���h�̏I����҂�
 *
 * @param   core			�`�惁�C�����W���[���̃n���h��
 *
 * @retval  BOOL		�I�����Ă�����TRUE
 */
//=============================================================================================
BOOL BTLV_WaitCommand( BTLV_CORE* core )
{
	if( core->processingCmd != BTLV_CMD_NULL )
	{
		if( core->mainProc(core, &core->mainSeq, core->genericWork) )
		{
			core->processingCmd = BTLV_CMD_NULL;
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

//=============================================================================================
/**
 * �v���C���[�I���A�N�V�������e���擾
 *
 * @param   core		�`�惁�C�����W���[���n���h��
 * @param   dst			[out] �A�N�V�������e�擾�\���̃A�h���X
 *
 */
//=============================================================================================
void BTLV_GetActionParam( const BTLV_CORE* core, BTL_ACTION_PARAM* dst )
{
	*dst = core->actionParam;
}

static void* getGenericWork( BTLV_CORE* core, u32 size )
{
	GF_ASSERT(size<sizeof(core->genericWork));

	return core->genericWork;
}


static BOOL CmdProc_Setup( BTLV_CORE* core, int* seq, void* workBuffer )
{
	switch( *seq ){
	case 0:
		setup_core( core, core->heapID );
		BTLV_SCU_Setup( core->scrnU );
		BTLV_SCD_Setup( core->scrnD );
		BTL_EFFECT_Init( 0, core->heapID );		//soga
		(*seq)++;
		break;

	case 1:
		BTLV_SCU_StartBtlIn( core->scrnU );
		(*seq)++;
		break;

	case 2:
		if( BTLV_SCU_WaitBtlIn( core->scrnU ) )
		{
			return TRUE;
		}
		break;
	}

	return FALSE;
}

static BOOL CmdProc_SelectAction( BTLV_CORE* core, int* seq, void* workBufer )
{
	enum {
		SEQ_INIT=0,
		SEQ_SELECT_MAIN,
		SEQ_SEL_FIGHT  = 100,
		SEQ_SEL_ITEM   = 200,
		SEQ_SEL_CHANGE = 300,
		SEQ_SEL_ESCAPE = 400,
	};

	typedef struct {
		u16 idx1;
		u16 idx2;
		u16 maxElems;
	}SEQ_WORK;

	SEQ_WORK* wk = workBufer;
	BOOL ret = FALSE;

	switch( *seq ){
	case 0:
		BTL_STR_MakeStringStd( core->strBuf, BTL_STRID_STD_SelectAction );
		BTLV_SCU_StartMsg( core->scrnU, core->strBuf );
		BTL_Printf("[BTLV_CORE] ���ʂփA�N�V�����I�����Ȃ������b�Z�[�W\n");
		(*seq)++;
		break;
	case 1:
		if( BTLV_SCU_WaitMsg(core->scrnU) )
		{
			const BTL_POKEPARAM* bpp = BTL_CLIENT_GetFrontPokeData( core->myClient, 0 );
			BTL_Printf("[BTLV_CORE] ����ʂփA�N�V�����I����������\n");
			BTLV_SCD_StartActionSelect( core->scrnD, bpp );
			(*seq)++;
		}
		break;
	case 2:
		if( BTLV_SCD_WaitActionSelect(core->scrnD) )
		{
			BTLV_SCD_GetSelectAction( core->scrnD, &core->actionParam );
			return TRUE;
		}
	}
	return FALSE;
}
//--------------------------------------------------------------------------
/**
 * �|�P�����I��
 *
 * @param   core		
 * @param   seq			
 * @param   workBufer	
 *
 * @retval  BOOL		
 */
//--------------------------------------------------------------------------
static BOOL CmdProc_SelectPokemon( BTLV_CORE* core, int* seq, void* workBufer )
{
	switch( *seq ){
	case 0:
		BTLV_SCD_StartPokemonSelect( core->scrnD );
		(*seq)++;
		break;
	case 1:
		if( BTLV_SCD_WaitPokemonSelect(core->scrnD) )
		{
			BTLV_SCD_GetSelectAction( core->scrnD, &core->actionParam );
			return TRUE;
		}
		break;
	}
	return FALSE;
}



//--------------------------
typedef struct {
	BtlTypeAff	affinity;
	u16			timer;
}WAZA_ACT_WORK;



//=============================================================================================
/**
 * ���U�A�N�V�����J�n
 *
 * @param   core		
 * @param   dst			
 *
 */
//=============================================================================================
void BTLV_StartWazaAct( BTLV_CORE* wk, BtlPokePos atPokePos, BtlPokePos defPokePos, u16 damage, WazaID waza, BtlTypeAff affinity )
{
//	const BTL_POKEPARAM* pp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, atPokePos );

	WAZA_ACT_WORK* subwk = getGenericWork(wk, sizeof(WAZA_ACT_WORK));

	BTL_Printf("[BTLVC] StartWazaAct aff = %d\n", affinity);

	BTLV_SCU_StartWazaAct( wk->scrnU, atPokePos, defPokePos, waza, affinity );

	subwk->affinity = affinity;
	subwk->timer = 0;

	BTL_UTIL_SetupProc( &wk->subProc, wk, NULL, subprocWazaAct );

	//�Z�G�t�F�N�g�o���Ă݂� soga
	if( atPokePos ){
		if( waza == WAZANO_MIZUDEPPOU ){
			BTL_EFFECT_Add( BTL_EFFECT_BB2AAMIZUDEPPOU );
		}
		else{
			BTL_EFFECT_Add( BTL_EFFECT_BB2AAGANSEKI );
		}
	}
	else{
		if( waza == WAZANO_MIZUDEPPOU ){
			BTL_EFFECT_Add( BTL_EFFECT_AA2BBMIZUDEPPOU );
		}
		else{
			BTL_EFFECT_Add( BTL_EFFECT_AA2BBGANSEKI );
		}
	}
}
//=============================================================================================
/**
 * ���U�A�N�V�����I���҂�
 *
 * @param   wk		
 *
 * @retval  BOOL		
 */
//=============================================================================================
BOOL BTLV_WaitWazaAct( BTLV_CORE* wk )
{
	return BTL_UTIL_CallProc( &wk->subProc );
}

static BOOL subprocWazaAct( int* seq, void* wk_adrs )
{
	BTLV_CORE* wk = wk_adrs;
	WAZA_ACT_WORK* subwk = getGenericWork(wk, sizeof(WAZA_ACT_WORK));

	switch( *seq ){
	case 0:
		//soga
		if( BTL_EFFECT_CheckExecute() == FALSE )
#if 0
		if( ++(subwk->timer) > 30 )
#endif
		{
			do {
				if( subwk->affinity < BTL_TYPEAFF_100 )
				{
					BTL_STR_MakeStringStd( wk->strBuf, BTL_STRID_STD_AffBad );
					BTLV_SCU_StartMsg( wk->scrnU, wk->strBuf );
					break;;
				}
				if ( subwk->affinity > BTL_TYPEAFF_100 )
				{
					BTL_STR_MakeStringStd( wk->strBuf, BTL_STRID_STD_AffGood );
					BTLV_SCU_StartMsg( wk->scrnU, wk->strBuf );
					break;
				}

			}while(0);

			(*seq)++;
		}
		break;

	case 1:
		if(	BTLV_SCU_WaitWazaAct(wk->scrnU)
		&&	BTLV_SCU_WaitMsg(wk->scrnU)
		){
			return TRUE;
		}
		break;

	}
	return FALSE;
}

//=============================================================================================
/**
 * �|�P�����Ђ񂵃A�N�V�����J�n
 *
 * @param   wk		
 * @param   clientID		
 *
 */
//=============================================================================================
void BTLV_StartDeadAct( BTLV_CORE* wk, u8 clientID )
{
	BTLV_SCU_StartDeadAct( wk->scrnU, clientID );
}
BOOL BTLV_WaitDeadAct( BTLV_CORE* wk )
{
	return BTLV_SCU_WaitDeadAct( wk->scrnU );
}

//------------------------------------------
typedef struct {
	u8 clientID;
}MEMBER_IN_WORK;

//=============================================================================================
/**
 * �|�P��������ւ��A�N�V�����J�n
 *
 * @param   wk		
 * @param   clientID		
 * @param   memberIdx		
 *
 */
//=============================================================================================
void BTLV_StartMemberChangeAct( BTLV_CORE* wk, u8 clientID, u8 memberIdx )
{
	MEMBER_IN_WORK* subwk = getGenericWork( wk, sizeof(MEMBER_IN_WORK) );

	subwk->clientID = clientID;
	BTL_UTIL_SetupProc( &wk->subProc, wk, NULL, subprocMemberIn );
//	const BTL_POKEPARAM* pp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, clientID );
//	printf("�䂯���I%s�I\n", BTRSTR_GetMonsName( BTL_POKEPARAM_GetMonsNo(pp)) );
}

BOOL BTLV_WaitMemberChangeAct( BTLV_CORE* wk )
{
	return BTL_UTIL_CallProc( &wk->subProc );
}

static BOOL subprocMemberIn( int* seq, void* wk_adrs )
{
	BTLV_CORE* wk = wk_adrs;
	MEMBER_IN_WORK* subwk = getGenericWork( wk, sizeof(MEMBER_IN_WORK) );

	switch( *seq ){
	case 0:
		{
			u16 strID = BTL_MAIN_IsOpponentClientID(wk->mainModule, wk->myClientID, subwk->clientID)?
					BTL_STRID_STD_PutSingle_Enemy : BTL_STRID_STD_PutSingle;
			BTL_STR_MakeStringStd( wk->strBuf, strID );
			BTLV_SCU_StartMsg( wk->scrnU, wk->strBuf );
			(*seq)++;
		}
		break;

	case 1:
		if( BTLV_SCU_WaitMsg(wk->scrnU) )
		{
			BTLV_SCU_StartPokeIn( wk->scrnU, subwk->clientID );
			(*seq)++;
		}
		break;
	case 2:
		if( BTLV_SCU_WaitPokeIn(wk->scrnU) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

//=============================================================================================
/**
 * ���b�Z�[�W�\���J�n
 *
 * @param   wk		
 * @param   strID		
 *
 */
//=============================================================================================
void BTLV_StartMsgStd( BTLV_CORE* wk, u16 strID, const int* args )
{
	BTL_STR_MakeStringStd( wk->strBuf, strID );
	BTLV_SCU_StartMsg( wk->scrnU, wk->strBuf );
//	printf( wk->strBuf );
}




//=============================================================================================
/**
 * ���b�Z�[�W�\���J�n
 *
 * @param   wk		
 * @param   strID		
 *
 */
//=============================================================================================
void BTLV_StartMsgSet( BTLV_CORE* wk, u16 strID, const int* args )
{
	BTL_STR_MakeStringSet( wk->strBuf, strID, args );
	BTLV_SCU_StartMsg( wk->scrnU, wk->strBuf );
//	printf( wk->strBuf );
}

void BTLV_StartMsgWaza( BTLV_CORE* wk, BtlPokePos pokePos, u16 waza )
{
	BTL_STR_MakeStringWaza( wk->strBuf, pokePos, waza );
	BTLV_SCU_StartMsg( wk->scrnU, wk->strBuf );
}

BOOL BTLV_WaitMsg( BTLV_CORE* wk )
{
	return BTLV_SCU_WaitMsg( wk->scrnU );
}



//=============================================================================================
/**
 * �Ƃ������E�B���h�E�\���J�n
 *
 * @param   wk		
 * @param   clientID		
 *
 */
//=============================================================================================
void BTLV_StartTokWin( BTLV_CORE* wk, u8 clientID )
{
	BTLV_SCU_DispTokWin( wk->scrnU, clientID );
}

void BTLV_QuitTokWin( BTLV_CORE* wk, u8 clientID )
{
	BTLV_SCU_HideTokWin( wk->scrnU, clientID );
}


void BTLV_StartRankDownEffect( BTLV_CORE* wk, u8 clientID, BppValueID statusType )
{
//	printf("�������Ђ���c\n");
}


//-------------------------------------------

static void setup_core( BTLV_CORE* wk, HEAPID heapID )
{
//soga
#if 0
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
#else
	static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_D,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_01_AB,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_01_FG,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_64K,		// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
	};
#endif

	// BGsystem������
	GFL_BG_Init( heapID );
	GFL_BMPWIN_Init( heapID );
	GFL_FONTSYS_Init();

	// VRAM�o���N�ݒ�
	GFL_DISP_SetBank( &vramBank );

	// �e����ʃ��W�X�^������
	G2_BlendNone();
	G2S_BlendNone();

	// �㉺��ʐݒ�
	GX_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );

	//�a�f���[�h�ݒ�
	{
		static const GFL_BG_SYS_HEADER sysHeader = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_3D,
		};
		GFL_BG_SetBGMode( &sysHeader );
	}

	//3D�֘A������ soga
	{
		GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K, GFL_G3D_VMANLNK, GFL_G3D_PLT16K, 0, heapID, NULL );
		GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
		G3X_AlphaBlend( TRUE );
		G3X_EdgeMarking( TRUE );
		G2_SetBG0Priority( 1 );
	}
	//�E�C���h�}�X�N�ݒ�i��ʗ��[�̃G�b�W�}�[�L���O�̃S�~�������jsoga
	{
		G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0 |
							   GX_WND_PLANEMASK_BG1 |
							   GX_WND_PLANEMASK_BG2 |
							   GX_WND_PLANEMASK_BG3 |
							   GX_WND_PLANEMASK_OBJ,
							   FALSE );
		G2_SetWndOutsidePlane( GX_WND_PLANEMASK_NONE, FALSE );
		G2_SetWnd0Position( 1, 0, 255, 192 );
		GX_SetVisibleWnd( GX_WNDMASK_W0 );
	}
}

static void cleanup_core( BTLV_CORE* wk )
{
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();

	//3D�֘A�폜 soga
	GFL_G3D_Exit();
}



u8 BTLV_CORE_GetPlayerClientID( const BTLV_CORE* core )
{
	return core->myClientID;
}
