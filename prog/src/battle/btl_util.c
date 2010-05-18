//=============================================================================================
/**
 * @file  btl_util.c
 * @brief �|�P����WB �o�g���V�X�e�� ���L�c�[���Q
 * @author  taya
 *
 * @date  2008.09.25  �쐬
 */
//=============================================================================================

#include "btl_common.h"
#include "btl_debug_print.h"
#include "btl_util.h"


//------------------------------------------------------
/*
 *  Overlay ID
 */
//------------------------------------------------------
FS_EXTERN_OVERLAY(battle_print);

/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/
static BOOL PrintSysEnableFlag = FALSE;

void BTL_UTIL_PRINTSYS_Init( void )
{
  #ifdef SDK_TWL
  u32 consoleMask = OS_CONSOLE_TWLDEBUGGER;
  #else
  u32 consoleMask = OS_CONSOLE_ISDEBUGGER;
  #endif
  if( OS_GetConsoleType() & consoleMask )
  {
    GFL_OVERLAY_Load( FS_OVERLAY_ID(battle_print) );
    PrintSysEnableFlag = TRUE;
  }
  else{
    PrintSysEnableFlag = FALSE;
  }
}

void BTL_UTIL_PRINTSYS_Quit( void )
{
  if( PrintSysEnableFlag ){
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(battle_print) );
    PrintSysEnableFlag = FALSE;
  }
}



#ifdef BTL_PRINT_SYSTEM_ENABLE

void BTL_UTIL_SetPrintType( BtlPrintType type )
{
  if( PrintSysEnableFlag ){
    BTL_DEBUGPRINT_SetType( type );
  }
}

void BTL_UTIL_Printf( const char* filename, int line, u32 strID, ... )
{
  if( PrintSysEnableFlag )
  {
    const char* fmt_str = BTL_DEBUGPRINT_GetFormatStr( strID );
    if( fmt_str )
    {
      va_list vlist;

      BTL_DEBUGPRINT_PrintHeader( filename, line );

      va_start( vlist, strID );
      OS_TVPrintf( fmt_str, vlist );
      va_end( vlist );
    }
  }
}

void BTL_UTIL_PrintfSimple( u32 strID, ... )
{
  if( PrintSysEnableFlag )
  {
    const char* fmt_str = BTL_DEBUGPRINT_GetFormatStr( strID );
    if( fmt_str )
    {
      va_list vlist;

      va_start( vlist, strID );
      OS_TVPrintf( fmt_str, vlist );
      va_end( vlist );
    }
  }
}

void BTL_UTIL_DumpPrintf( const char* caption, const void* data, u32 size )
{
  if( PrintSysEnableFlag )
  {
    BTL_DEBUGPRINT_PrintDump( caption, data, size );
  }
}

#endif  /* #ifdef BTL_PRINT_SYSTEM_ENABLE */


/*============================================================================================*/
/* ��Ԍp���p�^�[��                                                                           */
/*============================================================================================*/

/**
 *  NULL�^ �쐬
 */
BPP_SICK_CONT  BPP_SICKCONT_MakeNull( void )
{
  BPP_SICK_CONT  cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_NONE;
  return cont;
}
/**
 *  NULL�^���ǂ�������
 */
BOOL BPP_SICKCONT_IsNull( BPP_SICK_CONT cont )
{
  return cont.type == WAZASICK_CONT_NONE;
}
/*
 *  �^�[�����^ �쐬
 */
BPP_SICK_CONT BPP_SICKCONT_MakeTurn( u8 turns )
{
  BPP_SICK_CONT  cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_TURN;
  cont.turn.count = turns;
  cont.turn.param = 0;
  return cont;
}
/*
 *  �^�[�����^�i�p�����[�^�t���j�쐬
 */
BPP_SICK_CONT BPP_SICKCONT_MakeTurnParam( u8 turns, u16 param )
{
  BPP_SICK_CONT  cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_TURN;
  cont.turn.count = turns;
  cont.turn.param = param;
  return cont;
}
/*
 *  �|�P�ˑ��^�쐬
 */
BPP_SICK_CONT BPP_SICKCONT_MakePoke( u8 pokeID )
{
  BPP_SICK_CONT  cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_POKE;
  cont.poke.ID = pokeID;
  return cont;
}
/*
 *  �i���^�쐬
 */
BPP_SICK_CONT BPP_SICKCONT_MakePermanent( void )
{
  BPP_SICK_CONT cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_PERMANENT;
  return cont;
}
/*
 *  �i���^�i�^�[�����J�E���g�A�b�v�j�쐬
 */
BPP_SICK_CONT BPP_SICKCONT_MakePermanentInc( u8 count_max )
{
  BPP_SICK_CONT cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_PERMANENT;
  cont.permanent.count_max = count_max;
  return cont;
}
/*
 *  �i���^�i�p�����[�^�t���j�쐬
 */
BPP_SICK_CONT BPP_SICKCONT_MakePermanentParam( u16 param )
{
  BPP_SICK_CONT cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_PERMANENT;
  cont.permanent.param = param;
  return cont;
}
/*
 *  �i���^�i�^�[�����J�E���g�A�b�v���p�����[�^�t���j�쐬
 */
BPP_SICK_CONT BPP_SICKCONT_MakePermanentIncParam( u8 count_max, u16 param )
{
  BPP_SICK_CONT cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_PERMANENT;
  cont.permanent.count_max = count_max;
  cont.permanent.param = param;
  return cont;
}
/*
 *  �^�[�����E�|�P�ˑ������^�쐬
 */
BPP_SICK_CONT BPP_SICKCONT_MakePokeTurn( u8 pokeID, u8 turns )
{
  BPP_SICK_CONT  cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_POKETURN;
  cont.poketurn.count = turns;
  cont.poketurn.pokeID = pokeID;
  return cont;
}
/*
 *  �^�[�����E�|�P�ˑ������^�i�p�����[�^�t���j�쐬
 */
BPP_SICK_CONT BPP_SICKCONT_MakePokeTurnParam( u8 pokeID, u8 turns, u16 param )
{
  BPP_SICK_CONT  cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_POKETURN;
  cont.poketurn.count = turns;
  cont.poketurn.pokeID = pokeID;
  cont.poketurn.param = param;
  return cont;
}
/**
 * �����ǂ��p�^�쐬
 */
BPP_SICK_CONT BPP_SICKCONT_MakeMoudokuCont( void )
{
  BPP_SICK_CONT cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_PERMANENT;
  cont.permanent.count_max = BTL_MOUDOKU_COUNT_MAX;
  return cont;
}
/**
 * �����ǂ��p�^���ǂ�������
 */
BOOL BPP_SICKCONT_IsMoudokuCont( BPP_SICK_CONT cont )
{
  if( (cont.type == WAZASICK_CONT_PERMANENT)
  &&  (cont.permanent.count_max == BTL_MOUDOKU_COUNT_MAX)
  ){
    return TRUE;
  }
  return FALSE;
}
/*
 *  �ˑ��|�PID���擾�i�|�P�ˑ��łȂ���� BTL_POKEID_NULL�j
 */
u8 BPP_SICKCONT_GetPokeID( BPP_SICK_CONT cont )
{
  if( cont.type == WAZASICK_CONT_POKE ){
    return cont.poke.ID;
  }
  if( cont.type == WAZASICK_CONT_POKETURN ){
    return cont.poketurn.pokeID;
  }
  return BTL_POKEID_NULL;
}
/*
 *  �ˑ��|�PID�����������i�|�P�ˑ��łȂ���Ή������Ȃ�L�j
 */
void BPP_SICKCONT_SetPokeID( BPP_SICK_CONT* cont, u8 pokeID )
{
  if( cont->type == WAZASICK_CONT_POKE ){
    cont->poke.ID = pokeID;
  }
  else if( cont->type == WAZASICK_CONT_POKETURN ){
    cont->poketurn.pokeID = pokeID;
  }
}
/*
 *  �L���^�[�������擾�i�^�[�����������Ȃ���� 0�j
 */
u8 BPP_SICCONT_GetTurnMax( BPP_SICK_CONT cont )
{
  if( cont.type == WAZASICK_CONT_TURN ){
    return cont.turn.count;
  }
  if( cont.type == WAZASICK_CONT_POKETURN ){
    return cont.poketurn.count;
  }
  return 0;
}
/*
 *  �ėp�p�����[�^��ݒ�
 */
void BPP_SICKCONT_AddParam( BPP_SICK_CONT* cont, u16 param )
{
  if( cont->type == WAZASICK_CONT_PERMANENT ){
    cont->permanent.param = param;
    return;
  }
  else if( cont->type == WAZASICK_CONT_POKE ){
    cont->poke.param = param;
    return;
  }
  else if( cont->type == WAZASICK_CONT_POKETURN ){
    cont->poketurn.param = param;
    return;
  }
  else if( cont->type == WAZASICK_CONT_TURN ){
    cont->turn.param = param;
    return;
  }
}
/*
 *  �ݒ肵���ėp�p�����[�^���擾
 */
u16 BPP_SICKCONT_GetParam( BPP_SICK_CONT cont )
{
  if( cont.type == WAZASICK_CONT_PERMANENT ){
    return cont.permanent.param;
  }
  if( cont.type == WAZASICK_CONT_POKE ){
    return cont.poke.param;
  }
  if( cont.type == WAZASICK_CONT_POKETURN ){
    return cont.poketurn.param;
  }
  if( cont.type == WAZASICK_CONT_TURN ){
    return cont.turn.param;
  }
  return 0;
}
/*
 *  �ėp�t���O�Z�b�g
 */
void BPP_SICKCONT_SetFlag( BPP_SICK_CONT* cont, BOOL flag )
{
  if( cont->type == WAZASICK_CONT_PERMANENT ){
    cont->permanent.flag = flag;
    return;
  }
  if( cont->type == WAZASICK_CONT_POKE ){
    cont->poke.flag = flag;
    return;
  }
  if( cont->type == WAZASICK_CONT_POKETURN ){
    cont->poketurn.flag = flag;
    return;
  }
  if( cont->type == WAZASICK_CONT_TURN ){
    cont->turn.flag = flag;
    return;
  }
}
/*
 *  �ėp�t���O�擾
 */
BOOL BPP_SICKCONT_GetFlag( BPP_SICK_CONT cont )
{
  if( cont.type == WAZASICK_CONT_PERMANENT ){
    return cont.permanent.flag;
  }
  if( cont.type == WAZASICK_CONT_POKE ){
    return cont.poke.flag;
  }
  if( cont.type == WAZASICK_CONT_POKETURN ){
    return cont.poketurn.flag;
  }
  if( cont.type == WAZASICK_CONT_TURN ){
    return cont.turn.flag;
  }
  return 0;

}
/*
 *  �p���^�[��������
 */
void BPP_SICKCONT_IncTurn( BPP_SICK_CONT* cont, u8 inc )
{
  if( cont->type == WAZASICK_CONT_TURN ){
    if( cont->turn.count < 8 ){
      cont->turn.count += inc;
    }
  }
  if( cont->type == WAZASICK_CONT_POKETURN ){
    if( cont->poketurn.count < 8 ){
      cont->poketurn.count += inc;
    }
  }
}
/*
 *  �p���^�[�����Z�b�g
 */
void BPP_SICKCONT_SetTurn( BPP_SICK_CONT* cont, u8 turn )
{
  if( cont->type == WAZASICK_CONT_TURN ){
    if( cont->turn.count < 8 ){
      cont->turn.count = turn;
    }
  }
  if( cont->type == WAZASICK_CONT_POKETURN ){
    if( cont->poketurn.count < 8 ){
      cont->poketurn.count = turn;
    }
  }
}

//===================================================================
// �|�P�����^�C�v���������ĂP�ϐ��Ƃ��Ĉ������߂̑[�u
//===================================================================

PokeTypePair PokeTypePair_Make( PokeType type1, PokeType type2 )
{
  return ( (((type1)&0xff)<<8) | (type2&0xff) );
}

PokeTypePair PokeTypePair_MakePure( PokeType type )
{
  return PokeTypePair_Make( type, type );
}

PokeType PokeTypePair_GetType1( PokeTypePair pair )
{
  return (pair >> 8) & 0xff;
}

PokeType PokeTypePair_GetType2( PokeTypePair pair )
{
  return pair & 0xff;
}

void PokeTypePair_Split( PokeTypePair pair, PokeType* type1, PokeType* type2 )
{
  *type1 = PokeTypePair_GetType1( pair );
  *type2 = PokeTypePair_GetType2( pair );
}

BOOL PokeTypePair_IsMatch( PokeTypePair pair, PokeType type )
{
  if( PokeTypePair_GetType1(pair) == type ){
    return TRUE;
  }
  if( PokeTypePair_GetType2(pair) == type ){
    return TRUE;
  }
  return FALSE;
}
BOOL PokeTypePair_IsMatchEither( PokeTypePair pair1, PokeTypePair pair2 )
{
  if( PokeTypePair_GetType1(pair1) == PokeTypePair_GetType1(pair2) ){
    return TRUE;
  }
  if( PokeTypePair_GetType1(pair1) == PokeTypePair_GetType2(pair2) ){
    return TRUE;
  }
  if( PokeTypePair_GetType2(pair1) == PokeTypePair_GetType1(pair2) ){
    return TRUE;
  }
  if( PokeTypePair_GetType2(pair1) == PokeTypePair_GetType2(pair2) ){
    return TRUE;
  }
  return FALSE;
}

BOOL PokeTypePair_IsPure( PokeTypePair pair )
{
  return PokeTypePair_GetType1(pair) == PokeTypePair_GetType2(pair);
}
