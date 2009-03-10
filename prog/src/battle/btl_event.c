//=============================================================================================
/**
 * @file	btl_event.c
 * @brief	�|�P����WB �o�g���V�X�e��	�C�x���g���n���h������
 * @author	taya
 *
 * @date	2008.10.23	�쐬
 */
//=============================================================================================
#include "btl_common.h"
#include "btl_pokeparam.h"

#include "btl_event.h"
#include "btl_event_factor.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
	FACTOR_REGISTER_MAX = 32,			///< �o�^�ł���C�x���g�t�@�N�^�[�ő吔
	EVENTVAL_STACK_DEPTH = 128,		///< �C�x���g�ϐ��X�^�b�N�̗e��
};

/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/

/**
*	�C�x���g�f�[�^����
*/
struct _BTL_EVENT_FACTOR {
	BTL_EVENT_FACTOR*	prev;
	BTL_EVENT_FACTOR*	next;
	const BtlEventHandlerTable*	handlerTable;
	BtlEventFactor	factorType;
	u32				priority;
	int				work[ EVENT_HANDLER_WORK_ELEMS ];
	u8				pokeID;
};

/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/
static BTL_EVENT_FACTOR  Factors[ FACTOR_REGISTER_MAX ];
static BTL_EVENT_FACTOR* FactorStack[ FACTOR_REGISTER_MAX ];
static BTL_EVENT_FACTOR* FirstFactorPtr;
static u16 StackPtr;

/**
*	�C�x���g�ϐ��X�^�b�N
*/
typedef struct {
	u32 sp;
	u16 label[ EVENTVAL_STACK_DEPTH ];
	u32 value[ EVENTVAL_STACK_DEPTH ];
}VAR_STACK;

static VAR_STACK VarStack = {0};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BTL_EVENT_FACTOR* popFactor( void );
static void pushFactor( BTL_EVENT_FACTOR* factor );
static void clearFactorWork( BTL_EVENT_FACTOR* factor );
static inline u32 calcFactorPriority( BtlEventFactor factorType, u16 subPri );
static void callHandlers( BTL_EVENT_FACTOR* factor, BtlEventType eventType, BTL_SVFLOW_WORK* flowWork );
static void varStack_Init( void );




void BTL_EVENT_InitSystem( void )
{
	int i;

	for(i=0; i<NELEMS(Factors); i++)
	{
		clearFactorWork( &Factors[i] );
		FactorStack[i] = &Factors[i];
	}

	FirstFactorPtr = NULL;
	StackPtr = 0;

	varStack_Init();
}


static BTL_EVENT_FACTOR* popFactor( void )
{
	if( StackPtr == FACTOR_REGISTER_MAX )
	{
		return NULL;
	}
	return FactorStack[ StackPtr++ ];
}


static void pushFactor( BTL_EVENT_FACTOR* factor )
{
	if( StackPtr == 0 )
	{
		GF_ASSERT(0);
		return;
	}
	clearFactorWork( factor );	//�l���N���A���Ă���X�^�b�N�ɐς�
	--StackPtr;
	FactorStack[ StackPtr ] = factor;
}

static void clearFactorWork( BTL_EVENT_FACTOR* factor )
{
	GFL_STD_MemClear( factor, sizeof(factor) );
}

static inline u32 calcFactorPriority( BtlEventFactor factorType, u16 subPri )
{
	return (factorType << 16) | subPri;
}


BTL_EVENT_FACTOR* BTL_EVENT_AddFactor( BtlEventFactor factorType, u16 subPri, u8 pokeID, const BtlEventHandlerTable* handlerTable )
{
	BTL_EVENT_FACTOR* newFactor;

	newFactor = popFactor();
	if( newFactor )
	{
		newFactor->priority = calcFactorPriority( factorType, subPri );
		newFactor->prev = NULL;
		newFactor->next = NULL;
		newFactor->handlerTable = handlerTable;
		newFactor->pokeID = pokeID;
		GFL_STD_MemClear( newFactor->work, sizeof(newFactor->work) );

		// �ŏ��̓o�^
		if( FirstFactorPtr == NULL )
		{
			FirstFactorPtr = newFactor;
		}
		// ���ݐ擪��荂�v���C�I���e�B
		else if( newFactor->priority < FirstFactorPtr->priority )
		{
			FirstFactorPtr->prev = newFactor;
			newFactor->next = FirstFactorPtr;
			FirstFactorPtr = newFactor;
		}
		// ����ȊO�͂ӂ��̃����N���X�g�ڑ�
		else
		{
			BTL_EVENT_FACTOR *find, *last;

			last = FirstFactorPtr;
			for( find=FirstFactorPtr->next; find!=NULL; find=find->next )
			{
				if( newFactor->priority < find->priority )
				{
					newFactor->next = find;
					newFactor->prev = find->prev;
					find->prev->next = newFactor;
					find->prev = newFactor;
					break;
				}
				last = find;
			}
			// �Ō�܂Ō�����Ȃ��ꍇ
			if( find == NULL )
			{
				last->next = newFactor;
				newFactor->prev = last;
			}
		}

		return newFactor;

	}
	// �X�^�b�N���猩����Ȃ�
	else
	{
		GF_ASSERT(0);
		return NULL;
	}
}


void BTL_EVENT_RemoveFactor( BTL_EVENT_FACTOR* factor )
{
	if( factor == FirstFactorPtr )
	{
		FirstFactorPtr = factor->next;
	}

	if( factor->prev )
	{
		factor->prev->next = factor->next;
	}

	if( factor->next )
	{
		factor->next->prev = factor->prev;
	}

	pushFactor( factor );
}

static void callHandlers( BTL_EVENT_FACTOR* factor, BtlEventType eventType, BTL_SVFLOW_WORK* flowWork )
{
	const BtlEventHandlerTable* tbl = factor->handlerTable;

	int i;
	for(i=0; tbl[i].eventType!=BTL_EVENT_NULL; i++)
	{
		if( tbl[i].eventType == eventType )
		{
			tbl[i].handler( factor, flowWork, factor->pokeID, factor->work );
		}
	}
}



void BTL_EVENT_CallHandlers( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID )
{
	BTL_EVENT_FACTOR* factor;

	for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
	{
//		callHandlers( factor->handlerTable, eventID, flowWork, factor->pokeID, factor->work );
		callHandlers( factor, eventID, flowWork );
	}
}


//======================================================================================================
//	�C�x���g�ϐ��X�^�b�N�\��
//======================================================================================================


static void varStack_Init( void )
{
	VAR_STACK* stack = &VarStack;
	int i;

	stack->label[0] = BTL_EVAR_SYS_SEPARATE;
	stack->sp = 0;
	for(i=0; i<NELEMS(stack->label); ++i)
	{
		stack->label[i] = BTL_EVAR_NULL;
	}
}


void BTL_EVENTVAR_Push( void )
{
	VAR_STACK* stack = &VarStack;

	if( stack->sp < NELEMS(stack->label) )
	{
		stack->label[ stack->sp++ ] = BTL_EVAR_SYS_SEPARATE;
		#ifdef PM_DEBUG
		if( stack->sp >= (NELEMS(stack->label)/8*7) )
		{
			BTL_Printf("Var Stack sp=%d �댯����ł��I�I\n", stack->sp);
		}
		#endif
	}
	else
	{
		GF_ASSERT(0);		// stack overflow
	}
}

void BTL_EVENTVAR_Pop( void )
{
	VAR_STACK* stack = &VarStack;

	while( stack->sp )
	{
		stack->sp--;
		if( stack->label[stack->sp] == BTL_EVAR_SYS_SEPARATE ){ break; }
	}
}

void BTL_EVENTVAR_SetValue( BtlEvVarLabel label, u32 value )
{
	GF_ASSERT(label!=BTL_EVAR_NULL);
	GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

	{
		VAR_STACK* stack = &VarStack;

		int p = stack->sp;
		while( p < NELEMS(stack->label) )
		{
			if( (stack->label[p] == BTL_EVAR_NULL)
			||	(stack->label[p] == label)
			){
				break;
			}
			++p;
		}
		if( p < NELEMS(stack->value) )
		{
			stack->label[p] = label;
			stack->value[p] = value;
			#ifdef PM_DEBUG
			if( p >= (NELEMS(stack->label)/8*7) )
			{
				BTL_Printf("Var Stack sp=%d �댯����ł��I�I\n", p);
			}
			#endif
		}
		else
		{
			GF_ASSERT(0);	// stack overflow
		}
	}
}

u32 BTL_EVENTVAR_GetValue( BtlEvVarLabel label )
{
	GF_ASSERT(label!=BTL_EVAR_NULL);
	GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);
	{
		VAR_STACK* stack = &VarStack;

		int p = stack->sp;
		while( p < NELEMS(stack->label) )
		{
			if( stack->label[p] == label ){
				return stack->value[p];
			}
			if( stack->label[p] == BTL_EVAR_NULL ){
				break;
			}
			++p;
		}
		GF_ASSERT_MSG(0,"label=%d",label);	// label not found
		return 0;
	}
}





