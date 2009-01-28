//=============================================================================================
/**
 * @file	btl_client.c
 * @brief	�|�P����WB �o�g���V�X�e��	�N���C�A���g���W���[��
 * @author	taya
 *
 * @date	2008.09.06	�쐬
 */
//=============================================================================================
#include <gflib.h>

#include "poke_tool/pokeparty.h"

#include "btl_common.h"
#include "btl_adapter.h"
#include "btl_action.h"
#include "btl_pokeselect.h"
#include "btl_server_cmd.h"
#include "btlv_core.h"

#include "btl_client.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
	FRONTPOKE_EMPTY = 0xff,		///< �퓬�|�P�̃C���f�b�N�X�Ƃ��Ė����i���S���ȂǂɎg�p�j
};


/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/
typedef BOOL (*ClientSubProc)( BTL_CLIENT*, int* );
typedef BOOL (*ServerCmdProc)( BTL_CLIENT*, int*, const int* );

/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/


//--------------------------------------------------------------
/**
 *	���U�����p�p�����[�^
 */
//--------------------------------------------------------------
struct _BTL_WAZA_EXE_PARAM {

	const BTL_POKEPARAM*	userPokeParam;
	WazaID	waza;			///< �g�����U�i���o�[
	u8		numTargetClients;				///< �^�[�Q�b�g�ƂȂ�N���C�A���g��
	u8		targetClientID[BTL_CLIENT_MAX];	///< �^�[�Q�b�g�ƂȂ�N���C�A���gID

};

//--------------------------------------------------------------
/**
 *	�N���C�A���g���W���[���\����`
 */
//--------------------------------------------------------------
struct _BTL_CLIENT {

	BTL_MAIN_MODULE*	mainModule;

	BTL_ADAPTER*	adapter;
	BTLV_CORE*		viewCore;

	ClientSubProc	subProc;
	int				subSeq;

	const void*			returnDataPtr;
	u32					returnDataSize;


	const BTL_PARTY*	myParty;
	u8					pokeIdxOrder[ BTL_PARTY_MEMBER_MAX ];		///< �p�[�e�B�����o�[���ǂ̏��Ԃŏo���Ă��邩
	u8					frontPokeEmpty[ BTL_POSIDX_MAX ];				///< �S�����Ă���퓬�ʒu�ɂ����o���Ȃ�����TRUE�ɂ���
	u8					numCoverPos;	///< �S������퓬�|�P������
	u8					procPokeIdx;	///< �������|�P�����C���f�b�N�X
	BtlPokePos	basePos;			///< �퓬�|�P�����̈ʒuID

	BTL_ACTION_PARAM	actionParam[ BTL_POSIDX_MAX ];
	BTL_SERVER_CMD_QUE*	cmdQue;
	int					cmdArgs[ BTL_SERVERCMD_ARG_MAX ];
	ServerCmd			serverCmd;
	ServerCmdProc		scProc;
	int					scSeq;

	BTL_POKESELECT_PARAM		pokeSelParam;
	BTL_POKESELECT_RESULT		pokeSelResult;


	BTL_WAZA_EXE_PARAM	wazaExeParam[ BTL_POS_MAX ];

	u8	myID;
	u8	myType;
	u8	myState;

};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static ClientSubProc getSubProc( BTL_CLIENT* wk, BtlAdapterCmd cmd );
static BOOL SubProc_UI_NotifyPokeData( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_NotifyPokeData( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_UI_Initialize( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_Initialize( BTL_CLIENT* wk, int *seq );
static BOOL SubProc_UI_SelectAction( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_SelectAction( BTL_CLIENT* wk, int* seq );
static u8 calc_empty_pos( BTL_CLIENT* wk );
static void abandon_cover_pos( BTL_CLIENT* wk, u8 numPos );
static u8 calc_puttable_pokemons( BTL_CLIENT* wk, u8* list );
static void setup_pokesel_param_dead( BTL_CLIENT* wk, u8 numSelect, BTL_POKESELECT_PARAM* param );
static void setup_pokesel_param_change( BTL_CLIENT* wk, BTL_POKESELECT_PARAM* param );
static void store_pokesel_to_action( BTL_CLIENT* wk, const BTL_POKESELECT_RESULT* res );
static BOOL SubProc_UI_SelectPokemon( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_SelectPokemon( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_ServerCmd( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_UI_ServerCmd( BTL_CLIENT* wk, int* seq );
static BOOL scProc_DATA_WazaExe( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_DATA_MemberIn( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_MSG_Std( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_MSG_Set( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_MSG_Waza( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaDmg( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Dead( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_RankDownEffect( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_TOKWIN_In( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_TOKWIN_Out( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_HpMinus( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_HpPlus( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_PPMinus( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_PPPlus( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_RankDown( BTL_CLIENT* wk, int* seq, const int* args );



BTL_CLIENT* BTL_CLIENT_Create(
	BTL_MAIN_MODULE* mainModule, BtlCommMode commMode, GFL_NETHANDLE* netHandle,
	u16 clientID, u16 numCoverPos, BtlThinkerType clientType, HEAPID heapID )
{
	BTL_CLIENT* wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_CLIENT) );
	int i;

	wk->myID = clientID;
	wk->myType = clientType;
	wk->adapter = BTL_ADAPTER_Create( netHandle, heapID, clientID );
	wk->myParty = BTL_MAIN_GetPartyDataConst( mainModule, clientID );
	wk->mainModule = mainModule;
	wk->numCoverPos = numCoverPos;
	wk->procPokeIdx = 0;
	wk->basePos = clientID & 1;
	for(i=0; i<NELEMS(wk->frontPokeEmpty); i++)
	{
		wk->frontPokeEmpty[i] = FALSE;
	}
	wk->cmdQue = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_SERVER_CMD_QUE) );

	wk->myState = 0;

	return wk;
}

void BTL_CLIENT_Delete( BTL_CLIENT* wk )
{
	GFL_HEAP_FreeMemory( wk->cmdQue );
	BTL_ADAPTER_Delete( wk->adapter );

	GFL_HEAP_FreeMemory( wk );
}

void BTL_CLIENT_AttachViewCore( BTL_CLIENT* wk, BTLV_CORE* viewCore )
{
	wk->viewCore = viewCore;
}


BTL_ADAPTER* BTL_CLIENT_GetAdapter( BTL_CLIENT* wk )
{
	GF_ASSERT(wk);
	return wk->adapter;
}


void BTL_CLIENT_Main( BTL_CLIENT* wk )
{
	switch( wk->myState ){
	case 0:
		{
			BtlAdapterCmd  cmd = BTL_ADAPTER_RecvCmd(wk->adapter);
			if( cmd != BTL_ACMD_NONE )
			{
				wk->subProc = getSubProc( wk, cmd );
				wk->subSeq = 0;
				wk->myState = 1;
				BTL_Printf("  [CL]  %d, �R�}���h%d���󂯎��܂���\n", wk->myID, cmd);
			}
		}
		break;

	case 1:
		if( wk->subProc(wk, &wk->subSeq) )
		{
			BTL_ADAPTER_ReturnCmd( wk->adapter, wk->returnDataPtr, wk->returnDataSize );
			wk->myState = 0;
			BTL_Printf("  [CL]  %d, �ԐM���܂���\n", wk->myID );
		}
		break;
	}
}



//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------


static ClientSubProc getSubProc( BTL_CLIENT* wk, BtlAdapterCmd cmd )
{
	static const struct {
		BtlAdapterCmd   cmd;
		ClientSubProc   procs[ BTL_THINKER_TYPE_MAX ];
	}procTbl[] = {

		{ BTL_ACMD_NOTIFY_POKEDATA,	{ SubProc_UI_NotifyPokeData, SubProc_AI_NotifyPokeData }, },
		{ BTL_ACMD_WAIT_INITIALIZE,	{ SubProc_UI_Initialize, SubProc_AI_Initialize } },
		{ BTL_ACMD_SELECT_ACTION,		{ SubProc_UI_SelectAction, SubProc_AI_SelectAction } },
		{ BTL_ACMD_SELECT_POKEMON,	{ SubProc_UI_SelectPokemon,SubProc_AI_SelectPokemon } },
		{ BTL_ACMD_SERVER_CMD,			{ SubProc_UI_ServerCmd, SubProc_AI_ServerCmd } },

	};

	int i;

	for(i=0; i<NELEMS(procTbl); i++)
	{
		if( procTbl[i].cmd == cmd )
		{
			return procTbl[i].procs[ wk->myType ];
		}
	}

	GF_ASSERT(0);
	return NULL;
}


static BOOL SubProc_UI_NotifyPokeData( BTL_CLIENT* wk, int* seq )
{
//	wk->returnDataPtr = 
	return TRUE;
}
static BOOL SubProc_AI_NotifyPokeData( BTL_CLIENT* wk, int* seq )
{
	return TRUE;
}
static BOOL SubProc_UI_Initialize( BTL_CLIENT* wk, int* seq )
{
	switch( *seq ){
	case 0:
//		BTL_Printf(" [CL] ��ʍ\�z���܂�\n");
		BTLV_StartCommand( wk->viewCore, BTLV_CMD_SETUP );
//		BTLV_StartSetup( wk->viewCore );
		(*seq)++;
		break;
	case 1:
		if( BTLV_WaitCommand(wk->viewCore) )
		{
			BTL_Printf(" [CL] ��ʍ\�z�����܂���\n");
			return TRUE;
		}
		break;
	}
	return FALSE;
}

static BOOL SubProc_AI_Initialize( BTL_CLIENT* wk, int *seq )
{
	
	return TRUE;
}

//--------------
static BOOL SubProc_UI_SelectAction( BTL_CLIENT* wk, int* seq )
{
	enum {
		SEQ_INIT = 0,
		SEQ_SELECT_ACTION,
		SEQ_CHECK_ACTION,
		SEQ_SELECT_POKEMON,
		SEQ_CHECK_DONE,
	};

	switch( *seq ){
	case SEQ_INIT:
		setup_pokesel_param_change( wk, &wk->pokeSelParam );
		wk->procPokeIdx = 0;
		(*seq) = SEQ_SELECT_ACTION;
		/* fallthru */
	case SEQ_SELECT_ACTION:
		BTL_Printf(" [CL] �A�N�V�����I��(%d�̖ځj�J�n���܂�\n", wk->procPokeIdx);
//		BTLV_StartCommand( wk->viewCore, BTLV_CMD_SELECT_ACTION );
		BTLV_UI_SelectAction_Start( wk->viewCore, &wk->actionParam[wk->procPokeIdx] );
		(*seq) = SEQ_CHECK_ACTION;
		break;
	case SEQ_CHECK_ACTION:
		if( BTLV_UI_SelectAction_Wait(wk->viewCore) )
		{
			// ����ւ��|�P�����I���̏ꍇ�͂܂��A�N�V�����p�����[�^���s�\��->�|�P�����I����
			if( BTL_ACTION_GetAction( &wk->actionParam[wk->procPokeIdx] ) == BTL_ACTION_CHANGE )
			{
				BTLV_StartPokeSelect( wk->viewCore, &wk->pokeSelParam, &wk->pokeSelResult );
				(*seq) = SEQ_SELECT_POKEMON;
				break;
			}
			// ����ȊO�͒S�����̑I�����I���Ă邩�`�F�b�N��
			else
			{
				(*seq) = SEQ_CHECK_DONE;
			}
		}
		break;

	// ����ւ��|�P�����I����
	case SEQ_SELECT_POKEMON:
		if( BTLV_WaitPokeSelect(wk->viewCore) )
		{
			u8 idx = BTL_POKESELECT_RESULT_GetLast( &wk->pokeSelResult );
			BTL_ACTION_SetChangeParam( &wk->actionParam[wk->procPokeIdx], wk->procPokeIdx, idx );
			BTL_POKESELECT_PARAM_SetProhibit( &wk->pokeSelParam, BTL_POKESEL_CANT_SELECTED, idx );
			(*seq) =  SEQ_CHECK_DONE;
		}
		break;

	case SEQ_CHECK_DONE:
		wk->procPokeIdx++;
		if( wk->procPokeIdx >= wk->numCoverPos )
		{
			wk->returnDataPtr = &(wk->actionParam[0]);
			wk->returnDataSize = sizeof(wk->actionParam[0]) * wk->numCoverPos;
			return TRUE;
		}
		else
		{
			(*seq) = SEQ_SELECT_ACTION;
		}
		break;
	}

	return FALSE;
}

static BOOL SubProc_AI_SelectAction( BTL_CLIENT* wk, int* seq )
{
	const BTL_POKEPARAM* pp;
	u8 wazaCount, i;

	for(i=0; i<wk->numCoverPos; i++)
	{
		pp = BTL_CLIENT_GetFrontPokeData( wk, 0 );
		wazaCount = BTL_POKEPARAM_GetWazaCount( pp );
		BTL_ACTION_SetFightParam( &wk->actionParam[i], GFL_STD_MtRand(wazaCount), 0 );
	}
	wk->returnDataPtr = &(wk->actionParam[0]);
	wk->returnDataSize = sizeof(wk->actionParam[0]) * wk->numCoverPos;

	return TRUE;
}
//---------------------------------------------------
// �����̃|�P����������ł����玟��I�����鏈��
//---------------------------------------------------

// �m���ɂȂ��āA���̃|�P�������o���Ȃ���΂Ȃ�Ȃ��S���ʒu����Ԃ�
static u8 calc_empty_pos( BTL_CLIENT* wk )
{
	u8 cnt, i;
	for(i=0, cnt=0; i<wk->numCoverPos; i++)
	{
		if( wk->frontPokeEmpty[i] == FALSE )
		{
			const BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, i );
			if( BTL_POKEPARAM_IsDead(bpp) )
			{
				cnt++;
			}
		}
	}
	return cnt;
}

// �S���ʒu���������  numPos : ��������ʒu�̐�
static void abandon_cover_pos( BTL_CLIENT* wk, u8 numPos )
{
	u8 i = wk->numCoverPos - 1;
	while( numPos )
	{
		if( wk->frontPokeEmpty[i] == FALSE )
		{
			wk->frontPokeEmpty[i] =  TRUE;
			numPos--;
		}
		if( i )	{
			i--;
		}	else {
			break;
		}
	}
}

// 
//--------------------------------------------------------------------------
/**
 * ���A��ɏo�Ă���|�P�����ȊO�ŁA�����Ă���|�P�����̐���Ԃ�
 *
 * @param   wk			
 * @param   list		[out] ��ɏo�Ă���ȊO�Ő����Ă���|�P�����̃p�[�e�B���C���f�b�N�X���i�[����z��
 *
 * @retval  u8		��ɏo�Ă���ȊO�Ő����Ă���|�P�����̐�
 */
//--------------------------------------------------------------------------
static u8 calc_puttable_pokemons( BTL_CLIENT* wk, u8* list )
{
	u8 cnt, numMembers, i, j;

	numMembers = BTL_PARTY_GetMemberCount( wk->myParty );
	for(i=wk->numCoverPos, cnt=0; i<numMembers; i++)
	{
		{
			const BTL_POKEPARAM* pp = BTL_PARTY_GetMemberDataConst(wk->myParty, i);
			if( !BTL_POKEPARAM_IsDead(pp) )
			{
				if( list )
				{
					list[cnt] = i;
				}
				cnt++;
			}
		}
	}
	return cnt;
}
// 
//--------------------------------------------------------------------------
/**
 * �|�P�����I����ʗp�p�����[�^�Z�b�g�i�����̃|�P�������m���ɂȂ����Ƃ��j
 *
 * @param   wk					�N���C�A���g���W���[���n���h��
 * @param   numSelect		�I�����Ȃ���΂����Ȃ���
 * @param   param				[out] �I����ʗp�p�����[�^���i�[����
 *
 */
//--------------------------------------------------------------------------
static void setup_pokesel_param_dead( BTL_CLIENT* wk, u8 numSelect, BTL_POKESELECT_PARAM* param )
{
	BTL_POKESELECT_PARAM_Init( param, wk->myParty, numSelect, TRUE );
	BTL_POKESELECT_PARAM_SetProhibitFighting( param, wk->numCoverPos );
}
//--------------------------------------------------------------------------
/**
 * �|�P�����I����ʗp�p�����[�^�Z�b�g�i�A�N�V�����u�|�P�����v�œ���ւ��鎞�j
 *
 * @param   wk					�N���C�A���g���W���[���n���h��
 * @param   param				[out] �I����ʗp�p�����[�^���i�[����
 *
 */
//--------------------------------------------------------------------------
static void setup_pokesel_param_change( BTL_CLIENT* wk, BTL_POKESELECT_PARAM* param )
{
	BTL_POKESELECT_PARAM_Init( param, wk->myParty, 1, TRUE );
	BTL_POKESELECT_PARAM_SetProhibitFighting( param, wk->numCoverPos );
}

// �|�P�����I����ʌ��� -> ����A�N�V�����p�����[�^�ɕϊ�
static void store_pokesel_to_action( BTL_CLIENT* wk, const BTL_POKESELECT_RESULT* res )
{
	u8 i, selIdx;
	for(i=0; i<res->cnt; i++)
	{
		selIdx = res->selIdx[i];
		BTL_Printf("[CL] �|�P��������ւ� %d�̖� <-> %d�̖�\n", i, selIdx );
		BTL_ACTION_SetChangeParam( &wk->actionParam[i], i, selIdx );
	}

	wk->returnDataPtr = &(wk->actionParam[0]);
	wk->returnDataSize = sizeof(wk->actionParam[0]) * res->cnt;
}

static BOOL SubProc_UI_SelectPokemon( BTL_CLIENT* wk, int* seq )
{
	switch( *seq ){
	case 0:
		{
			u8 numEmpty = calc_empty_pos( wk );
			if( numEmpty )
			{
				u8 numPuttable = calc_puttable_pokemons( wk, NULL );
				if( numPuttable )
				{
					u8 numSelect = numEmpty;

					// �����Ă�|�P���o���Ȃ���΂����Ȃ����ɑ���Ȃ��ꍇ�A��������߂�
					if( numSelect > numPuttable )
					{
						abandon_cover_pos( wk, numSelect - numPuttable );
						numSelect = numPuttable;
					}

					BTL_Printf("[CL] myID=%d �퓬�|�P�����񂾂̂Ń|�P����%d�̑I��\n", wk->myID, numSelect);
					setup_pokesel_param_dead( wk, numSelect, &wk->pokeSelParam );
					BTLV_StartPokeSelect( wk->viewCore, &wk->pokeSelParam, &wk->pokeSelResult );
					(*seq)++;
				}
				else
				{
					BTL_Printf("[CL] myID=%d �����킦��|�P�������Ȃ����Ƃ�ԐM\n", wk->myID);
					BTL_ACTION_SetChangeDepleteParam( &wk->actionParam[0] );
					wk->returnDataPtr = &(wk->actionParam[0]);
					wk->returnDataSize = sizeof(wk->actionParam[0]);
				}
			}
			else
			{
					BTL_Printf("[CL] myID=%d �N������łȂ��̂őI�ԕK�v�Ȃ�\n", wk->myID);
					BTL_ACTION_SetNULL( &wk->actionParam[0] );
					wk->returnDataPtr = &(wk->actionParam[0]);
					wk->returnDataSize = sizeof(wk->actionParam[0]);
					return TRUE;
			}
		}
		break;

	case 1:
		if( BTLV_WaitPokeSelect(wk->viewCore) )
		{
			store_pokesel_to_action( wk, &wk->pokeSelResult );
			return TRUE;
		}
		break;
	}

	return FALSE;
}
static BOOL SubProc_AI_SelectPokemon( BTL_CLIENT* wk, int* seq )
{
	u8 numEmpty = calc_empty_pos( wk );
	if( numEmpty )
	{
		u8 numPuttable;
		u8 puttableList[ BTL_PARTY_MEMBER_MAX ];

		numPuttable = calc_puttable_pokemons( wk, puttableList );
		if( numPuttable )
		{
			u8 numSelect = numEmpty;
			u8 i;

			// �����Ă�|�P���o���Ȃ���΂����Ȃ����ɑ���Ȃ��ꍇ�A��������߂�
			if( numSelect > numPuttable )
			{
				abandon_cover_pos( wk, numSelect - numPuttable );
				numSelect = numPuttable;
			}
			BTL_Printf("[CL ai] myID=%d �퓬�|�P�����񂾂̂Ń|�P����%d�̑I��\n", wk->myID, numSelect);
			for(i=0; i<numSelect; i++)
			{
				BTL_ACTION_SetChangeParam( &wk->actionParam[i], i, puttableList[i] );
			}
			wk->returnDataPtr = &(wk->actionParam[0]);
			wk->returnDataSize = sizeof(wk->actionParam[0]) * numSelect;
		}
		else
		{
			BTL_Printf("[CL] myID=%d �����킦��|�P�������Ȃ����Ƃ�ԐM\n", wk->myID);
			BTL_ACTION_SetChangeDepleteParam( &wk->actionParam[0] );
			wk->returnDataPtr = &(wk->actionParam[0]);
			wk->returnDataSize = sizeof(wk->actionParam[0]);
		}
	}
	else
	{
			BTL_Printf("[CL] myID=%d �N������łȂ��̂őI�ԕK�v�Ȃ�\n", wk->myID);
			BTL_ACTION_SetNULL( &wk->actionParam[0] );
			wk->returnDataPtr = &(wk->actionParam[0]);
			wk->returnDataSize = sizeof(wk->actionParam[0]);
	}
	return TRUE;
}

//---------------------------------------------------
// �T�[�o�R�}���h����
//---------------------------------------------------
static BOOL SubProc_AI_ServerCmd( BTL_CLIENT* wk, int* seq )
{
	// AI�͉���������_��
	return TRUE;
}

static BOOL SubProc_UI_ServerCmd( BTL_CLIENT* wk, int* seq )
{
	static const struct {
		u32				cmd;
		ServerCmdProc	proc;
	}scprocTbl[] = {
		{	SC_DATA_WAZA_EXE,		scProc_DATA_WazaExe			},
		{	SC_DATA_MEMBER_IN,	scProc_DATA_MemberIn		},
		{	SC_MSG_STD,					scProc_MSG_Std				},
		{	SC_MSG_SET,					scProc_MSG_Set				},
		{	SC_MSG_WAZA,				scProc_MSG_Waza				},
		{	SC_ACT_WAZA_DMG,		scProc_ACT_WazaDmg			},
		{	SC_ACT_DEAD,				scProc_ACT_Dead				},
		{	SC_ACT_RANKDOWN,		scProc_ACT_RankDownEffect	},
		{	SC_TOKWIN_IN,				scProc_TOKWIN_In			},
		{	SC_TOKWIN_OUT,			scProc_TOKWIN_Out			},
		{	SC_OP_HP_MINUS,			scProc_OP_HpMinus			},
		{	SC_OP_HP_PLUS,			scProc_OP_HpPlus			},
		{	SC_OP_PP_MINUS,			scProc_OP_PPMinus			},
		{	SC_OP_PP_PLUS,			scProc_OP_PPPlus			},
		{	SC_OP_RANK_DOWN,		scProc_OP_RankDown			},
	};




restart:

	switch( *seq ){
	case 0:
		{
			u16 cmdSize;
			const void* cmdBuf;

			cmdSize = BTL_ADAPTER_GetRecvData( wk->adapter, &cmdBuf );
			SCQUE_Setup( wk->cmdQue, cmdBuf, cmdSize );
			(*seq)++;
		}
		/* fallthru */
	case 1:
		if( SCQUE_IsFinishRead(wk->cmdQue) )
		{
			BTL_Printf("[CL] �T�[�o�[�R�}���h�ǂݏI���܂��������\n");
			return TRUE;
		}
		(*seq)++;
		/* fallthru */
	case 2:
		{
			int i;

			wk->serverCmd = SCQUE_Read( wk->cmdQue, wk->cmdArgs );

			for(i=0; i<NELEMS(scprocTbl); i++)
			{
				if( scprocTbl[i].cmd == wk->serverCmd ){ break; }
			}

			if( i == NELEMS(scprocTbl) )
			{
				BTL_Printf("[CL] �p�ӂ���Ă��Ȃ��R�}���h�����I\n");
				return TRUE;
			}

			BTL_Printf( "[CL] serverCmd=%d\n", wk->serverCmd );
			wk->scProc = scprocTbl[i].proc;
			wk->scSeq = 0;
			(*seq)++;
		}
		/* fallthru */
	case 3:
		if( wk->scProc(wk, &wk->scSeq, wk->cmdArgs) )
		{
			(*seq) = 1;
			goto restart;
		}
		break;
	}

	return FALSE;
}


// �f�[�^�ݒ�F���U����
static BOOL scProc_DATA_WazaExe( BTL_CLIENT* wk, int* seq, const int* args )
{
	BTL_WAZA_EXE_PARAM* wep;
	const BTL_POKEPARAM* poke;
	int i;
	u8 userPos;

	userPos = args[0];
	wep = &wk->wazaExeParam[ userPos ];
	wep->userPokeParam = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, userPos );
	poke = wep->userPokeParam;
	wep->waza = BTL_POKEPARAM_GetWazaNumber( poke, args[1] );
	wep->numTargetClients = args[2];

	for(i=0; i<wep->numTargetClients; i++)
	{
		wep->targetClientID[i] = args[3 + i];
	}

	return TRUE;
}

static BOOL scProc_DATA_MemberIn( BTL_CLIENT* wk, int* seq, const int* args )
{
	switch( *seq ){
	case 0:
		{
			u8 clientID = wk->cmdArgs[0];
			u8 posIdx = wk->cmdArgs[1];
			u8 memberIdx = wk->cmdArgs[2];
			BtlPokePos  pokePos = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx );

			BTL_Printf("[CL] MEMBER IN .... client=%d, posIdx=%d, memberIdx=%d\n", clientID, posIdx, memberIdx);

			BTL_MAIN_CLIENTDATA_SwapPartyMembers( wk->mainModule, clientID, posIdx, memberIdx );

			BTLV_StartMemberChangeAct( wk->viewCore, pokePos, clientID, memberIdx );
			(*seq)++;
		}
		break;
	case 1:
		if( BTLV_WaitMemberChangeAct(wk->viewCore) )
		{
			return TRUE;
		}
		break;
	}
	return FALSE;
}

static BOOL scProc_MSG_Std( BTL_CLIENT* wk, int* seq, const int* args )
{
	switch( *seq ){
	case 0:
		BTLV_StartMsgStd( wk->viewCore, args[0], &args[1] );
		(*seq)++;
		break;
	case 1:
		if( BTLV_WaitMsg(wk->viewCore) )
		{
			(*seq)++;
		}
		break;
	case 2:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
		{
			return TRUE;
		}
		break;
	}
	return FALSE;
}

static BOOL scProc_MSG_Set( BTL_CLIENT* wk, int* seq, const int* args )
{
	switch( *seq ){
	case 0:
		BTLV_StartMsgSet( wk->viewCore, args[0], &args[1] );
		(*seq)++;
		break;
	case 1:
		if( BTLV_WaitMsg(wk->viewCore) )
		{
			(*seq)++;
		}
		break;
	case 2:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
		{
			return TRUE;
		}
		break;
	}
	return FALSE;
}

static BOOL scProc_MSG_Waza( BTL_CLIENT* wk, int* seq, const int* args )
{
	switch( *seq ){
	case 0:
		{
			u8  pokePos = args[0];
			u16 wazaIdx  = args[1];
			const BTL_POKEPARAM* poke = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, pokePos );
			wazaIdx = BTL_POKEPARAM_GetWazaNumber( poke, wazaIdx );
			BTLV_StartMsgWaza( wk->viewCore, pokePos, wazaIdx );
		}
		(*seq)++;
		break;
	case 1:
		if( BTLV_WaitMsg(wk->viewCore) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL scProc_ACT_WazaDmg( BTL_CLIENT* wk, int* seq, const int* args )
{
	switch( *seq ) {
	case 0:
	{
		WazaID waza;
		u8 atPokePos, defPokePos, affinity, wazaIdx;
		u16 damage;
		const BTL_PARTY* party;
		const BTL_POKEPARAM* poke;

		atPokePos		= args[0];
		defPokePos	= args[1];
		damage		= args[2];
		wazaIdx		= args[3];
		affinity	= args[4];

		poke = BTL_MAIN_GetFrontPokeDataConst( wk->mainModule, atPokePos );
		waza = BTL_POKEPARAM_GetWazaNumber( poke, wazaIdx );

		BTL_Printf("[CL] WazaAct aff=%d, damage=%d\n", affinity, damage);

		BTLV_StartWazaAct( wk->viewCore, atPokePos, defPokePos, damage, waza, affinity );
		(*seq)++;
	}
	break;

	case 1:
		if( BTLV_WaitWazaAct(wk->viewCore) )
		{
			(*seq)++;
		}
		break;

	case 2:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
		{
			return TRUE;
		}
		break;
	}

	return FALSE;
}

static BOOL scProc_ACT_Dead( BTL_CLIENT* wk, int* seq, const int* args )
{
	switch( *seq ){
	case 0:
		BTLV_StartDeadAct( wk->viewCore, args[0] );
		(*seq)++;
		break;
	case 1:
		if( BTLV_WaitDeadAct(wk->viewCore) )
		{
			return TRUE;
		}
		break;
	}
	return FALSE;
}

static BOOL scProc_ACT_RankDownEffect( BTL_CLIENT* wk, int* seq, const int* args )
{
	BTLV_StartRankDownEffect( wk->viewCore, args[0], args[1] );
	return TRUE;
}


static BOOL scProc_TOKWIN_In( BTL_CLIENT* wk, int* seq, const int* args )
{
	BTLV_StartTokWin( wk->viewCore, args[0] );
	return TRUE;
}

static BOOL scProc_TOKWIN_Out( BTL_CLIENT* wk, int* seq, const int* args )
{
	BTLV_QuitTokWin( wk->viewCore, args[0] );
	return TRUE;
}

static BOOL scProc_OP_HpMinus( BTL_CLIENT* wk, int* seq, const int* args )
{
	BTL_POKEPARAM* pp = BTL_MAIN_GetFrontPokeData( wk->mainModule, args[0] );
	BTL_POKEPARAM_HpMinus( pp, args[1] );
	return TRUE;
}
static BOOL scProc_OP_HpPlus( BTL_CLIENT* wk, int* seq, const int* args )
{
	BTL_POKEPARAM* pp = BTL_MAIN_GetFrontPokeData( wk->mainModule, args[0] );
	BTL_POKEPARAM_HpPlus( pp, args[1] );
	return TRUE;
}
static BOOL scProc_OP_PPMinus( BTL_CLIENT* wk, int* seq, const int* args )
{
	u8 pokeID = args[0];
	u8 wazaIdx = args[1];
	u8 value = args[2];
	u8 pokePos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, pokeID );

	BTL_POKEPARAM* pp = BTL_MAIN_GetFrontPokeData( wk->mainModule, pokePos );
	BTL_POKEPARAM_PPMinus( pp, wazaIdx, value );
	BTL_Printf(" [CL] �|�P������PP ���炵�܂� pokeID=%d, waza=%d, val=%d\n", pokeID, wazaIdx, value);

	return TRUE;
}
static BOOL scProc_OP_PPPlus( BTL_CLIENT* wk, int* seq, const int* args )
{
	BTL_POKEPARAM* pp = BTL_MAIN_GetFrontPokeData( wk->mainModule, args[0] );
	BTL_POKEPARAM_PPPlus( pp, args[1], args[2] );
	return TRUE;
}
static BOOL scProc_OP_RankDown( BTL_CLIENT* wk, int* seq, const int* args )
{
	BTL_POKEPARAM* pp = BTL_MAIN_GetFrontPokeData( wk->mainModule, args[0] );
	TAYA_Printf("[CL] OP - RankDown PokePos=%d, ppDat=%p\n", args[0], pp);
	BTL_POKEPARAM_RankDown( pp, args[1], args[2] );
	return TRUE;
}













//------------------------------------------------------------------------------------------------------
// �O�����W���[������̏��擾����
//------------------------------------------------------------------------------------------------------

u8 BTL_CLIENT_GetClientID( const BTL_CLIENT* client )
{
	return client->myID;
}

const BTL_PARTY* BTL_CLIENT_GetParty( const BTL_CLIENT* client )
{
	return client->myParty;
}

//=============================================================================================
/**
 * ���݁A�s���I���������̃|�P�����f�[�^���擾
 *
 * @param   client		
 *
 * @retval  const BTL_POKEPARAM*		
 */
//=============================================================================================
const BTL_POKEPARAM* BTL_CLIENT_GetProcPokeData( const BTL_CLIENT* client )
{
	return BTL_CLIENT_GetFrontPokeData( client, client->procPokeIdx );
}
//=============================================================================================
/**
 * ���݁A�s���I���������̃|�P�����ʒuID���擾
 *
 * @param   client		
 *
 * @retval  BtlPokePos		
 */
//=============================================================================================
BtlPokePos BTL_CLIENT_GetProcPokePos( const BTL_CLIENT* client )
{
	return client->basePos + client->procPokeIdx*2;
}
//=============================================================================================
/**
 * �퓬�ɏo�Ă���|�P�����̃|�P�����f�[�^���擾
 *
 * @param   client		
 * @param   posIdx		
 *
 * @retval  const BTL_POKEPARAM*		
 */
//=============================================================================================
const BTL_POKEPARAM* BTL_CLIENT_GetFrontPokeData( const BTL_CLIENT* client, u8 posIdx )
{
	GF_ASSERT_MSG(posIdx<client->numCoverPos, "posIdx=%d, numCoverPos=%d", posIdx, client->numCoverPos);
	return BTL_PARTY_GetMemberDataConst( client->myParty, client->pokeIdxOrder[posIdx] );
}



const BTL_WAZA_EXE_PARAM* BTL_CLIENT_GetWazaExeParam( const BTL_CLIENT* client, u8 clientID )
{
	return &client->wazaExeParam[ clientID ];
}

//----------

const BTL_POKEPARAM*  BTL_CLIENT_WEP_GetUserPokeParam( const BTL_WAZA_EXE_PARAM* wep )
{
	return wep->userPokeParam;
}

WazaID  BTL_CLIENT_WEP_GetWazaNumber( const BTL_WAZA_EXE_PARAM* wep )
{
	return wep->waza;
}


