//=============================================================================================
/**
 * @file	btlv_core.c
 * @brief	�|�P����WB �o�g���`�惁�C�����W���[��
 * @author	taya
 *
 * @date	2008.10.02	�쐬
 */
//=============================================================================================

#include "btl_common.h"
#include "btl_main.h"
#include "btl_action.h"
#include "btl_calc.h"

#include "btlv_core.h"



/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
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
	u8			genericWork[ GENERIC_WORK_SIZE ];

	BTL_ACTION_PARAM	actionParam;
	STRBUF*				strBuf;

	HEAPID	heapID;
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static int getch_upr( void );
static BOOL CmdProc_Setup( BTLV_CORE* core, int* seq, void* workBuffer );
static BOOL CmdProc_SelectAction( BTLV_CORE* core, int* seq, void* workBufer );
static BOOL CmcProc_SelectPokemon( BTLV_CORE* core, int* seq, void* workBufer );


//---------------------------------------
// getch ��S���啶���ɂ��ĕԂ�
//---------------------------------------
static int getch_upr( void )
{
	int key = getchar();
	if( key >= 'a' && key <= 'z' )
	{
		key = 'A' + (key - 'a');
	}
	return key;
}


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
	core->strBuf = GFL_STR_CreateBuffer( 1024, heapID );

	core->mainProc = NULL;
	core->mainSeq = 0;

	BTL_STR_InitSystem( mainModule, client, heapID );

	return core;
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
			{ BTLV_CMD_SELECT_POKEMON,	CmcProc_SelectPokemon },
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
 * 
 *
 * @param   core		
 * @param   dst			
 *
 */
//=============================================================================================
void BTLV_GetActionParam( const BTLV_CORE* core, BTL_ACTION_PARAM* dst )
{
	*dst = core->actionParam;
	BTL_Printf("*** action param copied cmd=%d ... core[%08x]\n", dst->gen.cmd, (u32)core);
}


static BOOL CmdProc_Setup( BTLV_CORE* core, int* seq, void* workBuffer )
{
	BTL_STR_MakeStringGeneric( core->strBuf, BTL_STRFMT_ENCOUNT );
	printf( core->strBuf );
	return TRUE;
}

static BOOL CmdProc_SelectAction( BTLV_CORE* core, int* seq, void* workBufer )
{
	enum {
		SEQ_INIT=0,
		SEQ_SELECT_MAIN,
		SEQ_SEL_FIGHT  = 100,
		SEQ_SEL_ITEM   = 200 ,
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
	case SEQ_INIT:
		BTL_STR_MakeStringGeneric( core->strBuf, BTL_STRFMT_SELECT_ACTION_READY );
		printf( core->strBuf );
		printf("[A]��������  [B]�ǂ���  [C]�|�P����  [D]�ɂ���\n");
		(*seq) = SEQ_SELECT_MAIN;
		break;
	case SEQ_SELECT_MAIN:
		switch( getch_upr() ){
		case 'A':
			printf("�����̂�\n");
			(*seq) = SEQ_SEL_FIGHT;
			break;

		case 'B':
			printf("�A�C�e����\n");
			(*seq) = SEQ_SEL_ITEM;
			break;

		case 'C':
			printf("��ւ���̂�\n");
			(*seq) = SEQ_SEL_CHANGE;
			break;

		case 'D':
			printf("������̂�\n");
			(*seq) = SEQ_SEL_ESCAPE;
			break;

		default:
			return FALSE;
		}
		break;

	case SEQ_SEL_FIGHT:
		{
			const BTL_POKEPARAM* p = BTL_MAIN_GetFrontPokeDataConst( core->mainModule, core->myClientID );
			int i, max = BTL_POKEPARAM_GetWazaCount( p );
			printf("���U��I���\n");
			for(i=0; i<max; i++)
			{
				printf("[%c]%s  ", 'A'+i, BTRSTR_GetWazaName( BTL_POKEPARAM_GetWazaNumber(p, i) ));
			}
			printf("[R]���ǂ�\n");
			printf("\n");
		}
		(*seq)++;
		/* fallthru */
	case SEQ_SEL_FIGHT+1:
		{
			int key = getch_upr();

			if( key == 'R' )
			{
				(*seq) = SEQ_INIT;
				break;
			}

			if( key >= 'A' && key <= 'D' )
			{
				wk->idx1 = (key - 'A');
				if( BTL_MAIN_GetRule(core->mainModule) == BTL_RULE_SINGLE )
				{
					// �V���O����Ȃ�ΏۑI���V�[�P���X�s�v
					BTL_ACTION_SetFightParam( &core->actionParam, wk->idx1, 0 );
					return TRUE;
				}
				else
				{
					(*seq)++;
					break;
				}
			}
		}
		break;
	case SEQ_SEL_FIGHT+2:
		printf("���̃��U��N�Ɏg���H  A:������ B:������ C:������ D:�h�C�c R:���ǂ�\n");
		(*seq)++;
		/* fallthru */
	case SEQ_SEL_FIGHT+3:
		{
			int key = getch_upr();

			if( key == 'R' )
			{
				(*seq) = SEQ_SEL_FIGHT;
				break;
			}

			if( key >= 'A' && key <= 'D' )
			{
				wk->idx2 = (key - 'A');
				BTL_ACTION_SetFightParam( &core->actionParam, wk->idx1, wk->idx2 );
				ret = TRUE;
			}
		}
		break;

	case SEQ_SEL_ITEM:
		printf("�ȂɎg���H A:���������� B:�₭���� C:�Ђ̂��̂ڂ� D:���ɂ��� R:���ǂ�\n");
		(*seq)++;
		/* fallthru */
	case SEQ_SEL_ITEM+1:
		{
			int key = getch_upr();

			if( key == 'R' )
			{
				(*seq) = SEQ_INIT;
				break;
			}

			if( key >= 'A' && key <= 'D' )
			{
				wk->idx1 = (key - 'A');
				(*seq)++;
				break;
			}
		}
		break;
	case SEQ_SEL_ITEM+2:
		printf("����Ɏg���H A:���Ԃ�  B:�Ȃ���  C:������  D:���Ȃ�  R:���ǂ�\n");
		(*seq)++;
		/* fallthru */
	case SEQ_SEL_ITEM+3:
		{
			int key = getch_upr();

			if( key == 'R' )
			{
				(*seq) = SEQ_SEL_ITEM;
				break;
			}

			if( key >= 'A' && key <= 'D' )
			{
				wk->idx2 = (key - 'A');
				BTL_ACTION_SetItemParam( &core->actionParam, wk->idx1, wk->idx2 );
				ret = TRUE;
			}
		}
		break;

	case SEQ_SEL_CHANGE:
		printf("�|�P������I��ł�\n");
		{
			const BTL_PARTY* party = BTL_MAIN_GetPartyDataConst( core->mainModule, core->myClientID );
			int i;

			wk->maxElems = BTL_PARTY_GetMemberCount( party );
			for(i=0; i<wk->maxElems; i++)
			{
				const BTL_POKEPARAM* pp = BTL_PARTY_GetMemberDataConst( party, i );
				printf("[%c]%s  ", 'A'+i, BTRSTR_GetMonsName( BTL_POKEPARAM_GetMonsNo(pp)));
			}
			printf("[R]���ǂ�\n");
		}
		(*seq)++;
		/* fallthru */
	case SEQ_SEL_CHANGE+1:
		{
			int key = getch_upr();

			if( key == 'R' )
			{
				(*seq) = SEQ_INIT;
				break;
			}

			if( key >= 'A' && key < 'A'+wk->maxElems )
			{
				u16 idx = (key - 'A');

				if( idx != BTL_MAIN_GetFrontMemberIdx(core->mainModule, core->myClientID) )
				{
					BTL_ACTION_SetChangeParam( &core->actionParam, idx );
					return TRUE;
				}
				else
				{
					const BTL_POKEPARAM* pp = BTL_MAIN_GetFrontPokeDataConst( core->mainModule, core->myClientID );
					printf("%s�͂����o�Ă��\n", BTRSTR_GetMonsName(BTL_POKEPARAM_GetMonsNo(pp)));
					(*seq)=SEQ_SEL_CHANGE;
				}
			}
		}
		break;

	case SEQ_SEL_ESCAPE:
		core->actionParam.escape.cmd = BTL_ACTION_ESCAPE;
		ret = TRUE;
		break;

	}

	return ret;
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
static BOOL CmcProc_SelectPokemon( BTLV_CORE* core, int* seq, void* workBufer )
{
	typedef struct {
		u16 idx1;
		u16 idx2;
		u16 maxElems;
	}SEQ_WORK;

	SEQ_WORK* wk = workBufer;

	switch( *seq ){

	case 0:
		printf("�|�P������I��ł�\n");
		{
			const BTL_PARTY* party = BTL_MAIN_GetPartyDataConst( core->mainModule, core->myClientID );
			int i;

			wk->maxElems = BTL_PARTY_GetMemberCount( party );
			for(i=0; i<wk->maxElems; i++)
			{
				const BTL_POKEPARAM* pp = BTL_PARTY_GetMemberDataConst( party, i );
				printf("[%c]%s  ", 'A'+i, BTRSTR_GetMonsName( BTL_POKEPARAM_GetMonsNo(pp)));
			}
		}
		(*seq)++;
		/* fallthru */
	case 1:
		{
			int key = getch_upr();

			if( key >= 'A' && key < 'A'+wk->maxElems )
			{
				u16 idx = (key - 'A');

				if( idx == BTL_MAIN_GetFrontMemberIdx(core->mainModule, core->myClientID) )
				{
					const BTL_POKEPARAM* pp = BTL_MAIN_GetFrontPokeDataConst( core->mainModule, core->myClientID );
					printf("%s�͂����o�Ă��\n", BTRSTR_GetMonsName(BTL_POKEPARAM_GetMonsNo(pp)));
					(*seq)=0;
				}
				else
				{
					const BTL_PARTY* party = BTL_MAIN_GetPartyDataConst( core->mainModule, core->myClientID );
					const BTL_POKEPARAM* pp = BTL_PARTY_GetMemberDataConst( party, idx );
					if( BTL_POKEPARAM_GetValue(pp, BPP_HP) == 0 )
					{
						printf("%s�͂��������܂���I\n", BTRSTR_GetMonsName(BTL_POKEPARAM_GetMonsNo(pp)));
						(*seq)=0;
					}
					else
					{
						BTL_ACTION_SetChangeParam( &core->actionParam, idx );
						return TRUE;
					}
				}
			}
		}
		break;
	}

	return FALSE;
}



//=============================================================================================
/**
 * ���U�A�N�V�����J�n
 *
 * @param   core		
 * @param   dst			
 *
 */
//=============================================================================================
void BTLV_StartWazaAct( BTLV_CORE* wk, u8 atClientID, u8 defClientID, WazaID waza, BtlTypeAff affinity )
{
//	const BTL_POKEPARAM* pp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, atClientID );

	if( affinity < BTL_TYPEAFF_100 )
	{
		printf("�������� ���܂ЂƂ̂悤��\n");
	}
	else if( affinity > BTL_TYPEAFF_100 )
	{
		printf("�������� �΂��񂾁I\n");
	}
}

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
	const BTL_POKEPARAM* pp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, clientID );
	printf("�䂯���I%s�I\n", BTRSTR_GetMonsName( BTL_POKEPARAM_GetMonsNo(pp)) );
}

BOOL BTLV_WaitMemberChangeAct( BTLV_CORE* wk )
{
	return TRUE;
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
void BTLV_StartMsgSpecific( BTLV_CORE* wk, BtlSpStrID strID, const int* args )
{
	BTL_STR_MakeStringSpecific( wk->strBuf, strID, args );
	printf( wk->strBuf );
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
	const BTL_POKEPARAM* pp = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, clientID );
	u16 tok = BTL_POKEPARAM_GetValue( pp, BPP_TOKUSEI );
	u16 monsno = BTL_POKEPARAM_GetMonsNo( pp );

	printf("����%s��%s�I����\n",
		BTRSTR_GetMonsName(monsno), BTRSTR_GetTokuseiName(tok)
	);
}

void BTLV_QuitTokWin( BTLV_CORE* wk, u8 clientID )
{
	
}


void BTLV_StartRankDownEffect( BTLV_CORE* wk, u8 clientID, BppValueID statusType )
{
	printf("�������Ђ���c\n");
}
