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

#include "handler\hand_waza.h"

#include "btl_event.h"
#include "btl_event_factor.h"




/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  FACTOR_PER_POKE = EVENT_WAZA_STICK_MAX + 3,            ///< ���U�\��t���ő吔+�g�p���U+�Ƃ�����+�A�C�e�� = 11
  FACTOR_MAX_FOR_POKE = FACTOR_PER_POKE * BTL_POS_MAX,   ///< �|�P�������ƍő吔(11) * �ꏊ��(6) = 66
  FACTOR_MAX_FOR_SIDEEFF = BTL_SIDEEFF_MAX * 2,          ///< �T�C�h�G�t�F�N�g�ő吔(13)�~�w�c��(2) = 26
  FACTOR_MAX_FOR_POSEFF = BTL_POSEFF_MAX * BTL_POS_MAX,  ///< �ʒu�G�t�F�N�g�ő吔(5) * �ꏊ��(6) = 30
  FACTOR_MAX_FOR_FIELD = BTL_FLDEFF_MAX,                 ///< �t�B�[���h�G�t�F�N�g�ő吔 = 6

  // �o�^�ł���C�x���g�t�@�N�^�[�ő吔�i66 + 26 + 30 + 6) = 128
  FACTOR_REGISTER_MAX = FACTOR_MAX_FOR_POKE + FACTOR_MAX_FOR_SIDEEFF + FACTOR_MAX_FOR_FIELD + FACTOR_MAX_FOR_POSEFF,
  EVENTVAL_STACK_DEPTH = 96,   ///< �C�x���g�ϐ��X�^�b�N�̗e��

};

/**
 * �l�̏��������ۏ��
 */

typedef enum {

  REWRITE_FREE = 0,   ///< ���R�ɉ\
  REWRITE_ONCE,       ///< �P�񂾂��\
  REWRITE_END,        ///< �P�񏑂��������̂ł����I��
  REWRITE_MUL,        ///< ��Z�̂݉\
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
  BtlEventSkipCheckHandler  skipCheckHandler;
  BtlEventFactorType  factorType;
  u32       priority         : 20;  ///< �\�[�g�v���C�I���e�B
  u32       numHandlers      :  8;  ///< �n���h���e�[�u���v�f��
  u32       callingFlag      :  1;  ///< �Ăяo�������ČĂяo�����Ȃ����߂̃t���O
  u32       sleepFlag        :  1;  ///< �x���t���O
  u32       tmpItemFlag      :  1;  ///< �A�C�e���p�ꎞ���p�t���O
  u32       rmReserveFlag    :  1;  ///< �폜�\��t���O

  u32       currentStackPtr   :16;   ///< �o�^���C�x���g�X�^�b�N�|�C���^
  u32       recallEnableFlag  : 1;   ///< �ċA�Ăяo������
  u32       existFlag         : 1;   ///< ���ݏ������C�x���g�ɂ��Add���ꂽ
  u32       rotationSleepFlag : 1;   ///< ���ݏ������C�x���g�ɂ��Add���ꂽ
  u32       _padd            : 28;
  int       work[ EVENT_HANDLER_WORK_ELEMS ];
  u16       subID;      ///< �C�x���g����ID�B���U�Ȃ烏�UID, �Ƃ������Ȃ�Ƃ�����ID�Ȃ�
  u8        dependID;   ///< �ˑ��Ώە�ID�B���U�E�Ƃ������E�A�C�e���Ȃ�|�PID�A�ꏊ�ˑ��Ȃ�ꏊid�ȂǁB
  u8        pokeID;     ///< �|�P�ˑ��C�x���g�Ȃ�|�PID�^����ȊOBTL_POKEID_NULL

};

/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/
static BTL_EVENT_FACTOR  Factors[ FACTOR_REGISTER_MAX ];
static BTL_EVENT_FACTOR* FactorStack[ FACTOR_REGISTER_MAX ];
static BTL_EVENT_FACTOR* FirstFactorPtr;
static u32 StackPtr;
static u32 EventStackPtr;

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
static inline BOOL isDependPokeFactorType( BtlEventFactorType factorType );
static void printLinkDebug( void );
static void CallHandlersSub( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID, BtlEventFactorType type, BOOL fSkipCheck );
static void CallHandlersCore( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID, BtlEventFactorType targetType, BOOL fSkipCheck );
static BOOL check_handler_skip( BTL_SVFLOW_WORK* flowWork, BTL_EVENT_FACTOR* factor, BtlEventType eventID );
static BTL_EVENT_FACTOR* pushFactor( void );
static void popFactor( BTL_EVENT_FACTOR* factor );
static void clearFactorWork( BTL_EVENT_FACTOR* factor );
static inline u32 calcFactorPriority( BtlEventPriority mainPri, u16 subPri );
static inline u16 getFactorMainPriority( u32 pri );
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
  EventStackPtr = 0;

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

/**
 *  �t�@�N�^�[�^�C�v���|�P�ˑ�����^�C�v������
 */
static inline BOOL isDependPokeFactorType( BtlEventFactorType factorType )
{
  if( (factorType == BTL_EVENT_FACTOR_WAZA)
  ||  (factorType == BTL_EVENT_FACTOR_TOKUSEI)
  ||  (factorType == BTL_EVENT_FACTOR_ITEM)
  ){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * �����N���X�g����Print�i�f�o�b�O�p�j
 */
//----------------------------------------------------------------------------------
static void printLinkDebug( void )
{
#if 0
  BTL_EVENT_FACTOR* fp;
  u32 cnt = 0;

  BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_LinkInfoHeader );
  for(fp=FirstFactorPtr; fp!=NULL; fp=fp->next)
  {
    BTL_N_PrintfSimple( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_LinkPtr, fp );
    ++cnt;
    if( cnt % 4 == 0 ){
      BTL_N_PrintfSimpleEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_LF );
    }
  }
  if( !cnt ){
    BTL_N_PrintfSimpleEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_LinkEmpty );
  }else if( cnt % 4 ){
    BTL_N_PrintfSimpleEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_LF );
  }
  BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_LinkInfoFooder );
#endif
}

//=============================================================================================
/**
 * �C�x���g�����v�f��ǉ�
 *
 * @param   factorType      �t�@�N�^�[�^�C�v
 * @param   subID           �^�C�v���Ƃ̌�ID�i�Ƃ�����ID�C�A�C�e��ID���ƈ�v�j
 * @param   priority        �v���C�I���e�B ( = ���l�������قǐ�ɌĂяo����� )
 * @param   pokeID
 * @param   handlerTable
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR* BTL_EVENT_AddFactor( BtlEventFactorType factorType, u16 subID,
  BtlEventPriority mainPri, u16 subPri, u8 dependID,
  const BtlEventHandlerTable* handlerTable, u16 numHandlers )
{
  BTL_EVENT_FACTOR* newFactor;

  newFactor = pushFactor();
  if( newFactor )
  {
    newFactor->priority = calcFactorPriority( mainPri, subPri );

    BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_AddFactorInfo, factorType, dependID, newFactor->priority, newFactor );

    newFactor->factorType = factorType;
    newFactor->prev = NULL;
    newFactor->next = NULL;
    newFactor->handlerTable = handlerTable;
    newFactor->numHandlers = numHandlers;

    newFactor->subID = subID;
    newFactor->callingFlag = FALSE;
    newFactor->sleepFlag = FALSE;
    newFactor->rotationSleepFlag = FALSE;
    newFactor->tmpItemFlag = FALSE;
    newFactor->skipCheckHandler = NULL;
    newFactor->dependID = dependID;
    newFactor->rmReserveFlag = FALSE;
    newFactor->recallEnableFlag = FALSE;
    newFactor->currentStackPtr = EventStackPtr;
    newFactor->existFlag = TRUE;
    if( isDependPokeFactorType(factorType) ){
      newFactor->pokeID = dependID;
    }else{
      newFactor->pokeID = BTL_POKEID_NULL;
    }

    GFL_STD_MemClear( newFactor->work, sizeof(newFactor->work) );

//    BTL_Printf("�C�x���g�o�^ �ˑ��|�PID=%d\n", newFactor->pokeID);

    // �ŏ��̓o�^
    if( FirstFactorPtr == NULL )
    {
      FirstFactorPtr = newFactor;
    }
    // ���ݐ擪��荂�v���C�I���e�B
    else if( newFactor->priority > FirstFactorPtr->priority )
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
        if( newFactor->priority > find->priority )
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
    BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_AddFactor,
        newFactor, newFactor->dependID, newFactor->factorType, newFactor->priority );
    printLinkDebug();
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
 * �����n���h����S�폜
 *
 * @param   none
 */
//=============================================================================================
void BTL_EVENT_RemoveIsolateFactors( void )
{
  BTL_EVENT_FACTOR *factor, *next;

  factor = FirstFactorPtr;
  while( factor != NULL )
  {
    next = factor->next;
    if( factor->factorType == BTL_EVENT_FACTOR_ISOLATE )
    {
      BTL_EVENT_FACTOR_Remove( factor );
    }
    factor = next;
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
  if( factor->callingFlag )
  {
    if( factor!=NULL ){
      BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_DelReserveFactor, factor, factor->dependID, factor->factorType );
    }
    factor->rmReserveFlag = TRUE;
    return;
  }

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


  BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EV_DelFactor, factor, factor->dependID, factor->factorType,
          factor->prev, factor->next );
  printLinkDebug();
  popFactor( factor );
}
//=============================================================================================
/**
 * �t�@�N�^�[�̎��
 *
 * @param   factor
 */
//=============================================================================================
void BTL_EVENT_FACTOR_ConvertForIsolate( BTL_EVENT_FACTOR* factor )
{
  factor->factorType = BTL_EVENT_FACTOR_ISOLATE;
}
//=============================================================================================
/**
 * ����ID�i���UID�A�Ƃ�����ID�C�A�C�e��ID���A�v�f�^�C�v�ɏ�����j��Ԃ�
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
 * �֘A�|�P����ID��Ԃ�
 *
 * @param   factor
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_EVENT_FACTOR_GetPokeID( const BTL_EVENT_FACTOR* factor )
{
  return factor->pokeID;
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
 * �A�C�e���p�ꎞ���p�t���O��ON�ɂ���iBTL_EVENT_USE_ITEM_TMP �ɔ�������悤�ɂȂ�j
 *
 * @param   factor
 */
//=============================================================================================
void BTL_EVENT_FACTOR_SetTmpItemFlag( BTL_EVENT_FACTOR* factor )
{
  factor->tmpItemFlag = TRUE;
}
//=============================================================================================
/**
 * ����n���h���̍ċA�Ăяo�����i�A�C�e���p�j
 *
 * @param   factor
 */
//=============================================================================================
void BTL_EVENT_FACTOR_SetRecallEnable( BTL_EVENT_FACTOR* factor )
{
  if( factor->callingFlag ){
    factor->recallEnableFlag = TRUE;
  }
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
 * �S�o�^�v�f�̑΂��A�w��C�x���g�̋����ʒm�i�X�L�b�v���������Ȃ��j
 *
 * @param   flowWork
 * @param   eventID
 */
//=============================================================================================
void BTL_EVENT_ForceCallHandlers( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID )
{
  CallHandlersSub( flowWork, eventID, BTL_EVENT_FACTOR_MAX, FALSE );
}
//=============================================================================================
/**
 * �S�o�^�v�f�ɑ΂��A�w��C�x���g�̒ʒm�i�X�L�b�v�������`�F�b�N�F�ʏ�͂�������Ăԁj
 *
 * @param   flowWork
 * @param   eventID
 */
//=============================================================================================
void BTL_EVENT_CallHandlers( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID )
{
  CallHandlersSub( flowWork, eventID, BTL_EVENT_FACTOR_MAX, TRUE );
}
//=============================================================================================
/**
 * �S�o�^�v�f�ɑ΂��A�w��C�x���g�̒ʒm�i�X�L�b�v�������`�F�b�N������^�C�v�̃n���h���̂݁j
 *
 * @param   flowWork
 * @param   eventID
 * @param   type
 */
//=============================================================================================
void BTL_EVENT_CallHandlersTargetType( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID, BtlEventFactorType type )
{
  CallHandlersSub( flowWork, eventID, type, TRUE );
}

/**
 *  �C�x���g�ʒm���ʉ�����
 */
static void CallHandlersSub( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID, BtlEventFactorType type, BOOL fSkipCheck )
{
  ++EventStackPtr;
  CallHandlersCore( flowWork, eventID, type, fSkipCheck );
  --EventStackPtr;

  if( EventStackPtr == 0 )
  {
    BTL_EVENT_FACTOR* factor;
    for( factor=FirstFactorPtr; factor!=NULL; factor = factor->next )
    {
      factor->currentStackPtr = 0;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �w��C�x���g�ʒm�R�A
 *
 * @param   flowWork
 * @param   eventID
 * @param   fSkipCheck
 */
//----------------------------------------------------------------------------------
static void CallHandlersCore( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID, BtlEventFactorType targetType, BOOL fSkipCheck )
{
  BTL_EVENT_FACTOR* factor;
  BTL_EVENT_FACTOR* next_factor;


  for( factor=FirstFactorPtr; factor!=NULL; )
  {
    next_factor = factor->next;

    do {

      if( (factor->callingFlag) && (factor->recallEnableFlag == FALSE) ){
         BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_SkipByCallingFlg, factor->factorType, factor->dependID, factor );
         break;
      }
      if( factor->sleepFlag ){
        break;
      }
      if( factor->rotationSleepFlag ){
        break;
      }
      if( (targetType!=BTL_EVENT_FACTOR_MAX) && (factor->factorType != targetType) ){
        break;
      }
      if( (factor->currentStackPtr != 0) && (factor->currentStackPtr >= EventStackPtr) ){
       BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_SkipByStackLayer, factor->factorType, factor->dependID, factor );
        break;
      }
      if( factor->existFlag == FALSE ){
        break;
      }
      if( (eventID == BTL_EVENT_USE_ITEM_TMP) && (factor->tmpItemFlag == FALSE) ){
        break;
      }

      {
        const BtlEventHandlerTable* tbl = factor->handlerTable;
        u32 i;

        BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_CheckFactorStart, factor->factorType, factor->dependID, factor->numHandlers, eventID );
        for(i=0; i<factor->numHandlers; i++)
        {
          BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_CheckFactorTbl, i, tbl[i].eventType );
          if( tbl[i].eventType == eventID )
          {
            if( !fSkipCheck || !check_handler_skip(flowWork, factor, eventID) )
            {
              factor->callingFlag = TRUE;
              BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_CallFactorStart, factor, factor->factorType, factor->dependID );
              tbl[i].handler( factor, flowWork, factor->dependID, factor->work );
              BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_CallFactorEnd, factor, factor->factorType, factor->dependID );


              if( factor->recallEnableFlag ){
                factor->recallEnableFlag = FALSE;
              }else{
                factor->callingFlag = FALSE;
              }
              // �Ăяo�����ɍ폜���ꂽ
              if( factor->rmReserveFlag )
              {
                BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_RmvFactorCalling, factor->dependID, factor );
                BTL_EVENT_FACTOR_Remove( factor );
              }
            }
            break;
          }
        }
      }
    }while(0);

    if( next_factor != NULL )
    {
      if( next_factor->existFlag )
      {
        factor = next_factor;
      }else{
        factor = factor->next;
      }
    }
    else{
      break;
    }
  }
}
/**
 *  ����n���h�����X�L�b�v���邩�`�F�b�N
 */
static BOOL check_handler_skip( BTL_SVFLOW_WORK* flowWork, BTL_EVENT_FACTOR* factor, BtlEventType eventID )
{
  const BTL_POKEPARAM* bpp = NULL;

  if( factor->pokeID != BTL_POKEID_NULL ){
    bpp = BTL_SVFTOOL_GetPokeParam( flowWork, factor->pokeID );
  }

  // �Ƃ������n���h��
  if( factor->factorType == BTL_EVENT_FACTOR_TOKUSEI )
  {
    if( bpp && BPP_CheckSick(bpp, WAZASICK_IEKI) ){  // �u�������v��Ԃ̃|�P�����͌Ăяo���Ȃ�
      return TRUE;
    }
  }

  // �A�C�e���n���h��
  if( factor->factorType == BTL_EVENT_FACTOR_ITEM )
  {
    if( BTL_FIELD_CheckEffect(BTL_FLDEFF_MAGICROOM) ){  // �u�}�W�b�N���[���v������ԂȂ�Ăяo���Ȃ�
      return TRUE;
    }
    if( bpp && BPP_CheckSick(bpp, WAZASICK_SASIOSAE) ){  // �u�����������v��Ԃ̃|�P�����͌Ăяo���Ȃ�
      return TRUE;
    }
    if( bpp && BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_ITEM_CANT_USE) ){  // �����A�C�e���g���Ȃ��t���OON�Ȃ�Ăяo���Ȃ�
      return TRUE;
    }

  }


  // �X�L�b�v�`�F�b�N�n���h�����o�^����Ă����画�f������
  {
    BTL_EVENT_FACTOR* fp;
    for(fp=FirstFactorPtr; fp!=NULL; fp=fp->next)
    {
      if( fp->skipCheckHandler != NULL )
      {
        if( fp->factorType == BTL_EVENT_FACTOR_TOKUSEI )
        {
          const BTL_POKEPARAM* skipBpp = BTL_SVFTOOL_GetPokeParam( flowWork, fp->pokeID );
          if( skipBpp && BPP_CheckSick(skipBpp, WAZASICK_IEKI) ){
            // ��������Ԃ̓X�L�b�v�`�F�b�N���s���Ȃ��悤�ɂ���
            continue;
          }
        }
        if( fp->rotationSleepFlag ){
          continue;
        }

        if( (fp->skipCheckHandler)( fp, flowWork, factor->factorType, eventID, factor->subID, factor->dependID ) ){
          BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_EVENT_SkipByCheckHandler, factor->factorType, factor->dependID );
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

//=============================================================================================
/**
 * �X�L�b�v�`�F�b�N�n���h�����A�^�b�`����
 *
 * @param   factor
 * @param   handler
 */
//=============================================================================================
void BTL_EVENT_FACTOR_AttachSkipCheckHandler( BTL_EVENT_FACTOR* factor, BtlEventSkipCheckHandler handler )
{
  factor->skipCheckHandler = handler;
}

//=============================================================================================
/**
 * �X�L�b�v�`�F�b�N�n���h�����f�^�b�`����
 *
 * @param   factor
 */
//=============================================================================================
void BTL_EVENT_FACTOR_DettachSkipCheckHandler( BTL_EVENT_FACTOR* factor )
{
  factor->skipCheckHandler = NULL;
}


BTL_EVENT_FACTOR* BTL_EVENT_SeekFactor( BtlEventFactorType factorType, u8 dependID )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->factorType == factorType) && (factor->dependID == dependID) && (factor->rmReserveFlag==FALSE) )
    {
      return factor;
    }
  }
  return NULL;
}

BTL_EVENT_FACTOR* BTL_EVENT_GetNextFactor( BTL_EVENT_FACTOR* factor )
{
  if( factor ){
    BtlEventFactorType type = factor->factorType;
    u8 dependID = factor->dependID;

    factor = factor->next;
    while( factor ){
      if( (factor->factorType == type) && (factor->dependID == dependID) && (factor->rmReserveFlag==FALSE) ){
        return factor;
      }
      factor = factor->next;
    }
  }
  return NULL;
}

//=============================================================================================
/**
 * �w��|�P�����̓\��t����ԃ��U�n���h�����x�~������i�}�W�b�N�~���[�Ώ��Ƃ��āj
 *
 * @param   pokeID
 *
 */
//=============================================================================================
void BTL_EVENT_SleepFactorMagicMirrorUser( u16 pokeID )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->factorType == BTL_EVENT_FACTOR_WAZA) && (factor->pokeID == pokeID) )
    {
      if( BTL_HANDLER_Waza_IsStick(factor, factor->work) )
      {
        factor->sleepFlag = TRUE;
      }
    }
  }
}
void BTL_EVENT_WakeFactorMagicMirrorUser( u16 pokeID )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->factorType == BTL_EVENT_FACTOR_WAZA) && (factor->pokeID == pokeID) )
    {
      factor->sleepFlag = FALSE;
    }
  }
}

//=============================================================================================
/**
 * �w��|�P�����́u�Ƃ������v�u�ǂ����v�n���h�����x�~�i���[�e�[�V�����Ώ��j
 *
 * @param   pokeID
 */
//=============================================================================================
void BTL_EVENT_SleepFactorRotation( u16 pokeID, BtlEventFactorType factorType )
{
  BTL_EVENT_FACTOR* factor;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->pokeID == pokeID)
    &&  (factor->factorType == factorType)
    ){
      factor->rotationSleepFlag = TRUE;
    }
  }
}
//=============================================================================================
/**
 * �w��|�P�����́u�Ƃ������v�u�ǂ����v�n���h�����x�~���畜�A�i���[�e�[�V�����Ώ��j
 *
 * @param   pokeID
 * @param   factorType
 *
 * @retval  BOOL    ���A�������n���h���������TRUE
 */
//=============================================================================================
BOOL BTL_EVENT_WakeFactorRotation( u16 pokeID, BtlEventFactorType factorType )
{
  BTL_EVENT_FACTOR* factor;
  BOOL result = FALSE;

  for( factor=FirstFactorPtr; factor!=NULL; factor=factor->next )
  {
    if( (factor->pokeID == pokeID)
    &&  (factor->factorType == factorType)
    ){
      factor->rotationSleepFlag = FALSE;
      result = TRUE;
    }
  }

  return result;
}



static BTL_EVENT_FACTOR* pushFactor( void )
{
  if( StackPtr == FACTOR_REGISTER_MAX )
  {
    return NULL;
  }
  return FactorStack[ StackPtr++ ];
}


static void popFactor( BTL_EVENT_FACTOR* factor )
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
  GFL_STD_MemClear( factor, sizeof(*factor) );
}

static inline u32 calcFactorPriority( BtlEventPriority mainPri, u16 subPri )
{
  mainPri = BTL_EVPRI_MAX - mainPri - 1;
  return (mainPri << 16) | subPri;
}
static inline u16 getFactorMainPriority( u32 pri )
{
  return (pri >> 16) & 0xffff;
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

//------------------------------------------------------------------------------
/**
 * �X�^�b�N����ɂȂ��Ă��邩�`�F�b�N�i���^�[���s���j
 *
 * ��ɂȂ��Ă��Ȃ���� �� �e�X�g����ASSERT��~����
 *                       ����e�X�g���͎��P�̍�Ƃ��ăX�^�b�N����ɂ���
 */
//------------------------------------------------------------------------------
void BTL_EVENTVAR_CheckStackCleared( void )
{
  VAR_STACK* stack = &VarStack;

  if( stack->sp ){
    GF_ASSERT_MSG(0, "sp=%d", stack->sp);
    varStack_Init();
  }
}

//------------------------------------------------------------------------------
/**
 * �X�^�b�NPush
 *
 * @param   line    Push�����s���� btl_server_flow.c �s�ԍ��i�f�o�b�O�p�j
 */
//------------------------------------------------------------------------------
void BTL_EVENTVAR_PushInpl( u32 line )
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
    stack->label[ stack->sp++ ] = BTL_EVAR_SYS_SEPARATE;
    #ifdef PM_DEBUG
    if( stack->sp >= (NELEMS(stack->label)/8*7) ){
      GF_ASSERT_MSG(0, "Event StackPointer =%d �댯����ł��I�I", stack->sp);
    }
    #endif
    BTL_N_PrintfEx( EVAR_PRINT_FLG, DBGSTR_EVAR_Push, line, stack->sp);
  }
  else
  {
    GF_ASSERT(0);   // stack overflow
  }
}

void BTL_EVENTVAR_PopInpl( u32 line )
{
  VAR_STACK* stack = &VarStack;

  if( stack->sp > 0 )
  {
    u16 p = stack->sp;
    while( (p < NELEMS(stack->label)) && (stack->label[p] != BTL_EVAR_NULL) ){
      stack->label[p++] = BTL_EVAR_NULL;
    }

    stack->sp--;
    stack->label[ stack->sp ] = BTL_EVAR_NULL;

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
    BTL_N_PrintfEx( EVAR_PRINT_FLG, DBGSTR_EVAR_Pop, line, stack->sp);

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
 * �㏑���֎~���x���̒l��V�K�Z�b�g�i�T�[�o�t���[�p�j
 *
 * @param   label
 * @param   value
 */
//=============================================================================================
void BTL_EVENTVAR_SetConstValue( BtlEvVarLabel label, int value )
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
    stack->rewriteState[p] = REWRITE_LOCK;
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
    stack->rewriteState[p] = REWRITE_MUL;
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
      if( (stack->rewriteState[p] == REWRITE_FREE)
      ||  (stack->rewriteState[p] == REWRITE_ONCE)
      ){
        stack->label[p] = label;
        stack->value[p] = (int)value;
        if( stack->rewriteState[p] == REWRITE_ONCE ){
          stack->rewriteState[p] = REWRITE_END;
        }
        return TRUE;
      }
      else if(  (stack->rewriteState[p] == REWRITE_LOCK)
      ||        (stack->rewriteState[p] == REWRITE_MUL)
      ){
        GF_ASSERT_MSG(0, "this label(%d) can't rewrite\n", label);
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
      if( stack->rewriteState[p] == REWRITE_MUL )
      {
        value = FX_Mul( stack->value[p], value );
        value = evar_mulValueRound( stack, p, value );
        stack->value[p] = (int)value;
      }else{
        GF_ASSERT_MSG(0, "this label(%d) can't mul\n", label);
      }
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
    GF_ASSERT_MSG(0,"label-%d not found",label);  // label not found
    return 0;
  }
}
//=============================================================================================
/**
 * �w�胉�x�������݂���΂��̒l���擾�A�Ȃ���Ή������Ȃ�
 *
 * @param   label
 * @param   value   [out]
 *
 * @retval  BOOL    ���x�������TRUE
 */
//=============================================================================================
BOOL BTL_EVENTVAR_GetValueIfExist( BtlEvVarLabel label, int* value )
{
  GF_ASSERT(label!=BTL_EVAR_NULL);
  GF_ASSERT(label!=BTL_EVAR_SYS_SEPARATE);
  {
    VAR_STACK* stack = &VarStack;

    int p = stack->sp;
    while( p < NELEMS(stack->label) )
    {
      if( stack->label[p] == label ){
        *value = stack->value[p];
        return TRUE;
      }
      if( stack->label[p] == BTL_EVAR_NULL ){
        break;
      }
      ++p;
    }
    return FALSE;
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

