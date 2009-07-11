//=============================================================================================
/**
 * @file  btl_event.c
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g���n���h������
 * @author  taya
 *
 * @date  2008.10.23  �쐬
 */
//=============================================================================================

#include "poke_tool\poketype.h"

#include "btl_common.h"
#include "btl_pokeparam.h"
#include "btl_sideeff.h"
#include "btl_field.h"

#include "btl_event.h"
#include "btl_event_factor.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  FACTOR_PER_POKE = EVENT_WAZA_STICK_MAX + 3,       ///< ���U�\��t���ő吔+�g�p���U+�Ƃ�����+�A�C�e�� = 11
  FACTOR_MAX_FOR_POKE = FACTOR_PER_POKE * BTL_POS_MAX, ///< �|�P�������ƍő吔(11) * �ꏊ��(6) = 66
  FACTOR_MAX_FOR_SIDEEFF = BTL_SIDEEFF_MAX * 2,     ///< �T�C�h�G�t�F�N�g�ő吔(9)�~�w�c��(2) = 18
  FACTOR_MAX_FOR_FIELD = BTL_FLDEFF_MAX,            ///< �t�B�[���h�G�t�F�N�g�ő吔 = 6

  // �o�^�ł���C�x���g�t�@�N�^�[�ő吔�i66 + 18 + 6) = 90
  FACTOR_REGISTER_MAX = FACTOR_MAX_FOR_POKE + FACTOR_MAX_FOR_SIDEEFF + FACTOR_MAX_FOR_FIELD,
  EVENTVAL_STACK_DEPTH = 128,   ///< �C�x���g�ϐ��X�^�b�N�̗e��

};

/**
 * �l�̏��������ۏ��
 */

typedef enum {

  REWRITE_FREE = 0,   ///< ���R�ɉ\
  REWRITE_ONCE,       ///< �P�񂾂��\
  REWRITE_LOCK,       ///< �s��

}RewriteState;

/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/

/**
* �C�x���g�f�[�^����
*/
struct _BTL_EVENT_FACTOR {
  BTL_EVENT_FACTOR* prev;
  BTL_EVENT_FACTOR* next;
  const BtlEventHandlerTable* handlerTable;
  BtlEventFactor  factorType;
  u32       priority;
  int       work[ EVENT_HANDLER_WORK_ELEMS ];
  u16       subID;
  u8        pokeID;
  u8        callingFlag : 1;
  u8        sleepFlag : 1;
};

/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/
static BTL_EVENT_FACTOR  Factors[ FACTOR_REGISTER_MAX ];
static BTL_EVENT_FACTOR* FactorStack[ FACTOR_REGISTER_MAX ];
static BTL_EVENT_FACTOR* FirstFactorPtr;
static u16 StackPtr;


/**
* �C�x���g�ϐ��X�^�b�N
*/
typedef struct {
  u32   sp;                                 ///< �X�^�b�N�|�C���^
  u16   label[ EVENTVAL_STACK_DEPTH ];      ///< �ϐ����x��
  int   value[ EVENTVAL_STACK_DEPTH ];      ///< �ϐ�
  fx32  mulMax[ EVENTVAL_STACK_DEPTH ];     ///< ��Z�Ή������̍ő�l
  fx32  mulMin[ EVENTVAL_STACK_DEPTH ];     ///< ��Z�Ή������̍ŏ��l
  u8    rewriteState[ EVENTVAL_STACK_DEPTH ];///< ���������ۏ��
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
static int evar_getNewPoint( const VAR_STACK* stack, BtlEvVarLabel label );
static int evar_getExistPoint( const VAR_STACK* stack, BtlEvVarLabel label );
static fx32 evar_mulValueRound( const VAR_STACK* stack, int ptr, fx32 value );


//=============================================================================================
/**
 * �V�X�e�������� �i�o�g���J�n���A�T�[�o�}�V������P��Ăяo���j
 */
//=============================================================================================
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


//=============================================================================================
/**
 * �^�[�����Ƃ̏���������
 */
//=============================================================================================
void BTL_EVENT_StartTurn( void )
{
  // �X���[�v�t���O�𗎂Ƃ�
  BTL_EVENT_FACTOR* factor;

  for(factor=FirstFactorPtr; factor!=NULL; factor=factor->next)
  {
    factor->sleepFlag = FALSE;
  }
}


//=============================================================================================
/**
 * �C�x���g�����v�f��ǉ�
 *
 * @param   factorType      �t�@�N�^�[�^�C�v
 * @param   subID           �^�C�v���Ƃ̌�ID�i�Ƃ�����ID�C�A�C�e��ID���ƈ�v�j
 * @param   priority
 * @param   pokeID
 * @param   handlerTable
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR* BTL_EVENT_AddFactor( BtlEventFactor factorType, u16 subID, u16 priority, u8 pokeID, const BtlEventHandlerTable* handlerTable )
{
  BTL_EVENT_FACTOR* newFactor;

  newFactor = popFactor();
  if( newFactor )
  {
    newFactor->priority = calcFactorPriority( factorType, priority );
    newFactor->factorType = factorType;
    newFactor->prev = NULL;
    newFactor->next = NULL;
    newFactor->handlerTable = handlerTable;
    newFactor->pokeID = pokeID;
    newFactor->subID = subID;
    newFactor->callingFlag = FALSE;
    newFactor->sleepFlag = FALSE;
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
//=============================================================================================
/**
 * �C�x���g�����v�f���폜
 *
 * @param   factor
 *
 */
//=============================================================================================
void BTL_EVENT_FACTOR_Remove( BTL_EVENT_FACTOR* factor )
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
//=============================================================================================
/**
 * �����v�f�̑Ή�����|�P�����������ւ�
 *
 * @param   factor
 * @param   pokeID
 * @param   pri
 *
 */
//=============================================================================================
void BTL_EVENT_FACTOR_ChangePokeParam( BTL_EVENT_FACTOR* factor, u8 pokeID, u16 pri )
{
  const BtlEventHandlerTable* handlerTable = factor->handlerTable;
  BtlEventFactor type = factor->factorType;
  u16 subID = factor->subID;

  BTL_EVENT_FACTOR_Remove( factor );
  BTL_EVENT_AddFactor( type, subID, pri, pokeID, handlerTable );
}
//=============================================================================================
/**
 * �T�uID�i�Ƃ�����ID�C�A�C�e��ID���A�v�f�^�C�v�ɏ�����j��Ԃ�
 *
 * @param   factor
 *
 * @retval  u16
 */
//=============================================================================================
u16 BTL_EVENT_FACTOR_GetSubID( const BTL_EVENT_FACTOR* factor )
{
  return factor->subID;
}
//=============================================================================================
/**
 * ���[�N���e���擾
 *
 * @param   factor
 * @param   workIdx
 *
 * @retval  int
 */
//=============================================================================================
int BTL_EVENT_FACTOR_GetWorkValue( const BTL_EVENT_FACTOR* factor, u8 workIdx )
{
  GF_ASSERT(workIdx < EVENT_HANDLER_WORK_ELEMS);
  return factor->work[ workIdx ];
}
//=============================================================================================
/**
 * ���[�N���e��ݒ�
 *
 * @param   factor
 * @param   workIdx
 * @param   value
 */
//=============================================================================================
void BTL_EVENT_FACTOR_SetWorkValue( BTL_EVENT_FACTOR* factor, u8 workIdx, int value )
{
  GF_ASSERT(workIdx < EVENT_HANDLER_WORK_ELEMS);
  factor->work[ workIdx ] = value;
}

//=============================================================================================
/**
 * �S�o�^�v�f�ɑ΂��A�w��C�x���g�̒ʒm
 *
 * @param   flowWork
 * @param   eventID
 */
//=============================================================================================
void BTL_EVENT_CallHandlers( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID )
{
  BTL_EVENT_FACTOR* factor;
  BTL_EVENT_FACTOR* next_factor;

  for( factor=FirstFactorPtr; factor!=NULL; )
  {
    next_factor = factor->next;
    callHandlers( factor, eventID, flowWork );
    factor = next_factor;
  }
}

BTL_EVENT_FACTOR* BTL_EVENT_SeekFactorCore( BtlEventFactor factorType )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( factor->factorType == factorType ){
      return factor;
    }
  }
  return NULL;
}
BTL_EVENT_FACTOR* BTL_EVENT_SeekFactor( BtlEventFactor factorType, u8 pokeID )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->factorType == factorType) && (factor->pokeID == pokeID) )
    {
      return factor;
    }
  }
  return NULL;
}

BTL_EVENT_FACTOR* BTL_EVENT_GetNextFactor( BTL_EVENT_FACTOR* factor )
{
  if( factor ){
    BtlEventFactor type = factor->factorType;
    u8 pokeID = factor->pokeID;

    factor = factor->next;
    while( factor ){
      if( (factor->factorType == type) && (factor->pokeID == pokeID) ){
        BTL_Printf("�C�x���g�^�C�v[%d], �|�P[%d]�̎��̃n���h��������������\n", type, pokeID );
        return factor;
      }
      factor = factor->next;
    }
  }
  return NULL;
}

//=============================================================================================
/**
 * ����^�C�v���T�uID�̃t�@�N�^�[���P�^�[���x�~������
 *
 * @param   type
 * @param   subID
 *
 */
//=============================================================================================
void BTL_EVENT_SleepFactor( BtlEventFactor type, u16 subID )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->factorType == type) && (factor->subID == subID) )
    {
      factor->sleepFlag = TRUE;
    }
  }
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
  clearFactorWork( factor );  //�l���N���A���Ă���X�^�b�N�ɐς�
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

static void callHandlers( BTL_EVENT_FACTOR* factor, BtlEventType eventType, BTL_SVFLOW_WORK* flowWork )
{
  if( (factor->callingFlag == FALSE) && (factor->sleepFlag == FALSE) )
  {
    const BtlEventHandlerTable* tbl = factor->handlerTable;

    int i;
    for(i=0; tbl[i].eventType!=BTL_EVENT_NULL; i++)
    {
      if( tbl[i].eventType == eventType )
      {
        factor->callingFlag = TRUE;
        tbl[i].handler( factor, flowWork, factor->pokeID, factor->work );
        factor->callingFlag = FALSE;
      }
    }
  }
}


//======================================================================================================
//  �C�x���g�ϐ��X�^�b�N�\��
//======================================================================================================
enum {
  EVAR_PRINT_FLG = FALSE,
};

static void varStack_Init( void )
{
  VAR_STACK* stack = &VarStack;
  int i;

  stack->sp = 0;
  for(i=0; i<NELEMS(stack->label); ++i)
  {
    stack->label[i] = BTL_EVAR_NULL;
  }
}

void BTL_EVENTVAR_Push( void )
{
  VAR_STACK* stack = &VarStack;

  while( stack->sp < NELEMS(stack->label) )
  {
    if( stack->label[stack->sp] == BTL_EVAR_NULL ){
      break;
    }
    stack->sp++;
  }

  if( stack->sp < (NELEMS(stack->label)-1) )
  {
    BTL_PrintfEx( EVAR_PRINT_FLG, "[EVAR] PUSH sp:%d\n", stack->sp );
    stack->label[ stack->sp++ ] = BTL_EVAR_SYS_SEPARATE;
    #ifdef PM_DEBUG
    if( stack->sp >= (NELEMS(stack->label)/8*7) ){
      BTL_Printf("Var Stack sp=%d �댯����ł��I�I\n", stack->sp);
    }
    #endif
  }
  else
  {
    GF_ASSERT(0);   // stack overflow
  }
}

void BTL_EVENTVAR_Pop( void )
{
  VAR_STACK* stack = &VarStack;

  if( stack->sp > 0 )
  {
    u16 p = stack->sp;
    while( (p < NELEMS(stack->label)) && (stack->label[p] != BTL_EVAR_NULL) ){
      BTL_PrintfEx( EVAR_PRINT_FLG, "[EVAR]  <prePOPv clear:%d\n", p);
      stack->label[p++] = BTL_EVAR_NULL;
    }

    stack->sp--;
    stack->label[ stack->sp ] = BTL_EVAR_NULL;
    BTL_PrintfEx( EVAR_PRINT_FLG, "[EVAR]  <prePOPs clear:%d\n", stack->sp);


    if( stack->sp )
    {
      while( stack->sp )
      {
        stack->sp--;
        if( stack->label[stack->sp] == BTL_EVAR_SYS_SEPARATE ){
          stack->sp++;
          break;
        }
      }
    }
    BTL_PrintfEx( EVAR_PRINT_FLG, "[EVAR] POP  sp:%d\n", stack->sp);
  }
  else
  {
    GF_ASSERT(0);
  }
}

//=============================================================================================
/**
 * ���x���̒l��V�K�Z�b�g�i�T�[�o�t���[�p�j
 *
 * @param   label
 * @param   value
 */
//=============================================================================================
void BTL_EVENTVAR_SetValue( BtlEvVarLabel label, int value )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &VarStack;
    int p = evar_getNewPoint( stack, label );
    stack->label[p] = label;
    stack->value[p] = value;
    stack->mulMin[p] = 0;
    stack->mulMax[p] = 0;
    stack->rewriteState[p] = REWRITE_FREE;
  }
}
//=============================================================================================
/**
 * �㏑���P��̂ݗL���ȃ��x���̒l��V�K�Z�b�g�i�T�[�o�t���[�p�j
 *
 * @param   label
 * @param   value
 */
//=============================================================================================
void BTL_EVENTVAR_SetRewriteOnceValue( BtlEvVarLabel label, int value )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &VarStack;
    int p = evar_getNewPoint( stack, label );
    stack->label[p] = label;
    stack->value[p] = value;
    stack->mulMin[p] = 0;
    stack->mulMax[p] = 0;
    stack->rewriteState[p] = REWRITE_ONCE;
  }
}
//=============================================================================================
/**
 * ��Z�Ή����x���̒l��V�K�Z�b�g�i�T�[�o�t���[�p�j
 *
 * @param   label
 * @param   value
 * @param   mulMin
 * @param   mulMax
 */
//=============================================================================================
void BTL_EVENTVAR_SetMulValue( BtlEvVarLabel label, int value, fx32 mulMin, fx32 mulMax )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &VarStack;
    int p = evar_getNewPoint( stack, label );
    stack->label[p] = label;
    stack->value[p] = value;
    stack->mulMin[p] = mulMin;
    stack->mulMax[p] = mulMax;
    stack->rewriteState[p] = REWRITE_FREE;
  }
}
//=============================================================================================
/**
 * �������x���̒l���㏑���i�n���h���p�j
 *
 * @param   label
 * @param   value
 *
 * @retval  BOOL    �㏑���o������TRUE
 */
//=============================================================================================
BOOL BTL_EVENTVAR_RewriteValue( BtlEvVarLabel label, int value )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &VarStack;
    int p = evar_getExistPoint( stack, label );
    if( p >= 0 )
    {
      if( stack->rewriteState[p] != REWRITE_LOCK )
      {
        stack->label[p] = label;
        stack->value[p] = (int)value;
        if( stack->rewriteState[p] == REWRITE_ONCE ){
          stack->rewriteState[p] = REWRITE_LOCK;
        }
        return TRUE;
      }
    }
    return FALSE;
  }
}
//=============================================================================================
/**
 * �����̏�Z�Ή����x���̒l�ɐ��l����Z����
 *
 * @param   label   ���x��ID
 * @param   value   ��Z����l
 */
//=============================================================================================
void BTL_EVENTVAR_MulValue( BtlEvVarLabel label, fx32 value )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);

  {
    VAR_STACK* stack = &VarStack;
    int p = evar_getExistPoint( stack, label );
    if( p >= 0 )
    {
      value = FX_Mul( stack->value[p], value );
      value = evar_mulValueRound( stack, p, value );
      stack->value[p] = (int)value;
    }
  }
}
//=============================================================================================
/**
 * �������x���̒l���擾
 *
 * @param   label
 *
 * @retval  int
 */
//=============================================================================================
int BTL_EVENTVAR_GetValue( BtlEvVarLabel label )
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
    GF_ASSERT_MSG(0,"label=%d",label);  // label not found
    return 0;
  }
}
// �V�K���x���p�Ɋm�ۂ����ʒu�̃|�C���^��Ԃ�
static int evar_getNewPoint( const VAR_STACK* stack, BtlEvVarLabel label )
{
  int p = stack->sp;
  while( p < NELEMS(stack->label) ){
    if( stack->label[p] == BTL_EVAR_NULL ){ break; }
    if( stack->label[p] == label ){
      GF_ASSERT_MSG(0, "Find Same Label ID : %d\n", label);
      break;
    }
    ++p;
  }

  if( p < NELEMS(stack->value) )
  {
    #ifdef PM_DEBUG
    if( p >= (NELEMS(stack->label)/8*7) ){
      BTL_Printf("Var Stack sp=%d �댯����ł��I�I\n", p);
    }
    #endif
    BTL_PrintfEx( EVAR_PRINT_FLG, "[EVAR]  >regist pos:%d label=%d\n", p, label);
    return p;
  }
  GF_ASSERT(0); // stack overflow
  return 0;
}
// �������x���ʒu�̃|�C���^��Ԃ�
static int evar_getExistPoint( const VAR_STACK* stack, BtlEvVarLabel label )
{
  int p = stack->sp;
  while( (p < NELEMS(stack->label)) && (stack->label[p] != BTL_EVAR_NULL) )
  {
    if( stack->label[p] == label ){
      return p;
    }
    ++p;
  }
  GF_ASSERT_MSG(0, "label[%d] not found", label);
  return -1;
}
// ��Z�Ή��ϐ��̏㉺�����z���Ă�����ۂ߂ĕԂ�
static fx32 evar_mulValueRound( const VAR_STACK* stack, int ptr, fx32 value )
{
  if( stack->mulMax[ptr] || stack->mulMin[ptr] )  // �ǂ��炩��0�ȊO�Ȃ��Z�Ή��ƌ��Ȃ�
  {
    if( stack->mulMin[ptr] > value ){
      value = stack->mulMin[ptr];
    }
    if( stack->mulMax[ptr] < value ){
      value = stack->mulMax[ptr];
    }
  }
  return value;
}

