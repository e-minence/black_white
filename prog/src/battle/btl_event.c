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

//--------------------------------------------------------------
/**
*	�C�x���g�n���h���e�[�u��
*/
//--------------------------------------------------------------
/*
typedef struct {
	BtlEventType	eventType;
	void*			handlerAdrs;
}BtlEventHandler;
*/

enum {
	FACTOR_REGISTER_MAX = 32,
	FACTOR_COUNTER_MAX = 10000,
};

//--------------------------------------------------------------
/**
*	�C�x���g�f�[�^����
*/
//--------------------------------------------------------------
struct _BTL_EVENT_FACTOR {
	BTL_EVENT_FACTOR*	prev;
	BTL_EVENT_FACTOR*	next;
	const BtlEventHandlerTable*	handlerTable;
	BtlEventFactor	factorType;
	u32				priority;
	int				work[ EVENT_HANDLER_WORK_ELEMS ];
	u8				pokeID;
};



static BTL_EVENT_FACTOR  Factors[ FACTOR_REGISTER_MAX ];
static BTL_EVENT_FACTOR* FactorStack[ FACTOR_REGISTER_MAX ];
static BTL_EVENT_FACTOR* FirstFactorPtr;
static u16 StackPtr;




/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BTL_EVENT_FACTOR* popFactor( void );
static void pushFactor( BTL_EVENT_FACTOR* factor );
static void clearFactorWork( BTL_EVENT_FACTOR* factor );
static inline u32 calcFactorPriority( BtlEventFactor factorType, u16 subPri );
static void callHandlers( const BtlEventHandlerTable* tbl, BtlEventType type, BTL_SVFLOW_WORK* flowWork, u8 pokeID, int* work );




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

static void callHandlers( const BtlEventHandlerTable* tbl, BtlEventType type, BTL_SVFLOW_WORK* flowWork, u8 pokeID, int* work )
{
	int i;
	for(i=0; tbl[i].eventType!=BTL_EVENT_NULL; i++)
	{
		if( tbl[i].eventType == type )
		{
			tbl[i].handler( flowWork, pokeID, work );
		}
	}
}



void BTL_EVENT_CallHandlers( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID )
{
	BTL_EVENT_FACTOR* factor;

	for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
	{
		callHandlers( factor->handlerTable, eventID, flowWork, factor->pokeID, factor->work );
	}
}


