//=============================================================================================
/**
 * @file  btl_util.h
 * @brief �|�P����WB �o�g���V�X�e�� ���L�c�[���Q
 * @author  taya
 *
 * @date  2008.09.25  �쐬
 */
//=============================================================================================
#pragma once


#include "poke_tool/poketype.h"
#include "waza_tool/wazadata.h"
#include "btl_const.h"
#include "btl_common.h"

//===================================================================
// �P���ȃv���Z�X�����\��
//===================================================================

typedef BOOL (*BPFunc)(int*, void*);

typedef struct {
  BPFunc  initFunc;
  BPFunc  loopFunc;
  void* work;
  int   seq;
}BTL_PROC;


static inline void BTL_UTIL_ClearProc( BTL_PROC* proc )
{
  proc->initFunc  = NULL;
  proc->loopFunc  = NULL;
  proc->work    = NULL;
  proc->seq   = 0;
}

static inline BOOL BTL_UTIL_ProcIsCleared( const BTL_PROC* proc )
{
  return (proc->initFunc==NULL) && (proc->loopFunc==NULL);
}

static inline void BTL_UTIL_SetupProc( BTL_PROC* proc, void* work, BPFunc init, BPFunc loop )
{
  proc->initFunc = init;
  proc->loopFunc = loop;
  proc->work = work;
  proc->seq = 0;
}

static inline BOOL BTL_UTIL_CallProc( BTL_PROC* proc )
{
  if( proc->initFunc )
  {
    if( proc->initFunc( &(proc->seq), proc->work ) )
    {
      proc->initFunc = NULL;
      proc->seq = 0;
    }
    return FALSE;
  }
  else if( proc->loopFunc )
  {
    if( proc->loopFunc( &(proc->seq), proc->work ) )
    {
      proc->loopFunc = NULL;
      return TRUE;
    }
    return FALSE;
  }
  return TRUE;
}

//===================================================================
// ��Ԉُ퓙�̌p���p�����[�^
//===================================================================

//--------------------------------------------------------------
/**
 *  ��Ԉُ�p���p�����[�^
 */
//--------------------------------------------------------------
typedef struct  {

  union {
    u32    raw;
    struct {
      u32  type : 3;
      u32  _0   : 29;
    };
    struct {
      u32  type_turn : 3;   ///< �^�[�����^
      u32  count     : 6;   ///< �L���^�[����
      u32  param     : 16;  ///< �ėp�p�����[�^16bit
      u32  flag      : 1;   ///< �ėp�t���O 1bit
      u32  _1        : 6;
    }turn;
    struct {
      u32  type_poke : 3;   ///< �|�P�ˑ��^
      u32  ID        : 6;   ///< �Ώۃ|�PID
      u32  param     : 16;  ///< �ėp�p�����[�^16bit
      u32  flag      : 1;   ///< �ėp�t���O 1bit
      u32  _2        : 6;
    }poke;
    struct {
      u32  type_perm : 3;   ///< �i���^
      u32  count_max : 6;   ///< �^�[���J�E���g�A�b�v�ő吔
      u32  param     : 16;  ///< �ėp�p�����[�^16bit
      u32  flag      : 1;   ///< �ėp�t���O 1bit
      u32  _3        : 6;
    }permanent;
    struct {
      u32 type_poketurn : 3;  ///< �^�[�����E�|�P�ˑ������^
      u32 count         : 6;  ///< �L���^�[����
      u32 pokeID        : 6;  ///< �Ώۃ|�PID
      u32 param         : 16; ///< �ėp�p�����[�^16bit
      u32 flag          : 1;  ///< �ėp�t���O 1bit
    }poketurn;
  };

}BPP_SICK_CONT;

/**
 *  NULL�^ �쐬
 */
extern BPP_SICK_CONT  BPP_SICKCONT_MakeNull( void );

/**
 *  NULL�^���ǂ�������
 */
extern BOOL BPP_SICKCONT_IsNull( BPP_SICK_CONT cont );

/*
 *  �^�[�����^ �쐬
 */
extern BPP_SICK_CONT  BPP_SICKCONT_MakeTurn( u8 turns );

/*
 *  �^�[�����^�i�p�����[�^�t���j�쐬
 */
extern BPP_SICK_CONT BPP_SICKCONT_MakeTurnParam( u8 turns, u16 param );

/*
 *  �|�P�ˑ��^�쐬
 */
extern BPP_SICK_CONT BPP_SICKCONT_MakePoke( u8 pokeID );

/*
 *  �i���^�쐬
 */
extern BPP_SICK_CONT BPP_SICKCONT_MakePermanent( void );

/*
 *  �i���^�i�^�[�����J�E���g�A�b�v�j�쐬
 */
extern BPP_SICK_CONT BPP_SICKCONT_MakePermanentInc( u8 count_max );

/*
 *  �i���^�i�p�����[�^�t���j�쐬
 */
extern BPP_SICK_CONT BPP_SICKCONT_MakePermanentParam( u16 param );

/*
 *  �i���^�i�^�[�����J�E���g�A�b�v���p�����[�^�t���j�쐬
 */
extern BPP_SICK_CONT BPP_SICKCONT_MakePermanentIncParam( u8 count_max, u16 param );

/*
 *  �^�[�����E�|�P�ˑ������^�쐬
 */
extern BPP_SICK_CONT BPP_SICKCONT_MakePokeTurn( u8 pokeID, u8 turns );

/*
 *  �^�[�����E�|�P�ˑ������^�i�p�����[�^�t���j�쐬
 */
extern BPP_SICK_CONT BPP_SICKCONT_MakePokeTurnParam( u8 pokeID, u8 turns, u16 param );

/**
 * �����ǂ��p�^�쐬
 */
extern BPP_SICK_CONT BPP_SICKCONT_MakeMoudokuCont( void );

/**
 * �����ǂ��p�^���ǂ�������
 */
extern BOOL BPP_SICKCONT_IsMoudokuCont( BPP_SICK_CONT cont );

/*
 *  �ˑ��|�PID���擾�i�|�P�ˑ��łȂ���� BTL_POKEID_NULL�j
 */
extern u8 BPP_SICKCONT_GetPokeID( BPP_SICK_CONT cont );

/*
 *  �ˑ��|�PID�����������i�|�P�ˑ��łȂ���Ή������Ȃ�L�j
 */
extern void BPP_SICKCONT_SetPokeID( BPP_SICK_CONT* cont, u8 pokeID );


/*
 *  �L���^�[�������擾�i�^�[�����������Ȃ���� 0�j
 */
extern u8 BPP_SICCONT_GetTurnMax( BPP_SICK_CONT cont );

/*
 *  �ėp�p�����[�^��ݒ�
 */
extern void BPP_SICKCONT_AddParam( BPP_SICK_CONT* cont, u16 param );

/*
 *  �ݒ肵���ėp�p�����[�^���擾
 */
extern u16 BPP_SICKCONT_GetParam( BPP_SICK_CONT cont );

/*
 *  �ėp�t���O�Z�b�g
 */
extern void BPP_SICKCONT_SetFlag( BPP_SICK_CONT* cont, BOOL flag );

/*
 *  �ėp�t���O�擾
 */
extern BOOL BPP_SICKCONT_GetFlag( BPP_SICK_CONT cont );

/*
 *  �p���^�[��������
 */
extern void BPP_SICKCONT_IncTurn( BPP_SICK_CONT* cont, u8 inc );

/*
 *  �p���^�[�����Z�b�g
 */
extern void BPP_SICKCONT_SetTurn( BPP_SICK_CONT* cont, u8 turn );



//===================================================================
// �|�P�����^�C�v���������ĂP�ϐ��Ƃ��Ĉ������߂̑[�u
//===================================================================
typedef u16 PokeTypePair;

extern PokeTypePair PokeTypePair_Make( PokeType type1, PokeType type2 );
extern PokeTypePair PokeTypePair_MakePure( PokeType type );
extern PokeType PokeTypePair_GetType1( PokeTypePair pair );
extern PokeType PokeTypePair_GetType2( PokeTypePair pair );
extern void PokeTypePair_Split( PokeTypePair pair, PokeType* type1, PokeType* type2 );
extern BOOL PokeTypePair_IsMatch( PokeTypePair pair, PokeType type );
extern BOOL PokeTypePair_IsMatchEither( PokeTypePair pair1, PokeTypePair pair2 );
extern BOOL PokeTypePair_IsPure( PokeTypePair pair );


//===================================================================
// �o�g���pPrintf����
//===================================================================

#include "btl_debug_print.h"

extern void BTL_UTIL_PRINTSYS_Init( void );
extern void BTL_UTIL_PRINTSYS_Quit( void );

#if defined(DEBUG_ONLY_FOR_taya)
//    defined(DEBUG_ONLY_FOR_nishino) | defined(DEBUG_ONLY_FOR_morimoto)
#ifdef PM_DEBUG
  #define BTL_PRINT_SYSTEM_ENABLE
#endif
#endif


#ifdef BTL_PRINT_SYSTEM_ENABLE

extern void BTL_UTIL_SetPrintType( BtlPrintType type );
extern void BTL_UTIL_Printf( const char* filename, int line, u32 channel, u32 strID, ... );
extern void BTL_UTIL_PrintfSimple( const char* filename, u32 channel, u32 strID, ... );
extern void BTL_UTIL_DumpPrintf( const char* caption, const void* data, u32 size );

#define BTL_N_Printf( ... ) BTL_UTIL_Printf( __FILE__, __LINE__, 1, __VA_ARGS__ )
#define BTL_N_PrintfEx( ch, ... )  if( ch!=0 ){ BTL_UTIL_Printf( __FILE__, __LINE__, ch, __VA_ARGS__ ); }
#define BTL_N_PrintfSimple( ... ) BTL_UTIL_PrintfSimple( __FILE__, 1, __VA_ARGS__ )
#define BTL_N_PrintfSimpleEx( ch, ... ) if( ch!=0 ){ BTL_UTIL_PrintfSimple( __FILE__, ch, __VA_ARGS__ ); }

#else // #ifdef BTL_PRINT_SYSTEM_ENABLE

#define BTL_UTIL_SetPrintType(t)         /* */
#define BTL_N_Printf( ... )              /* */
#define BTL_N_PrintfEx( flg, ... )       /* */
#define BTL_N_PrintfSimple( ... )        /* */
#define BTL_N_PrintfSimpleEx( flg, ... ) /* */

#endif  // #ifdef BTL_PRINT_SYSTEM_ENABLE




/* ��Print���� */
#define BTL_Printf( ... )         /* */
#define BTL_PrintfEx( flg, ... )  /* */
#define BTL_PrintfSimple( ... )   /* */

