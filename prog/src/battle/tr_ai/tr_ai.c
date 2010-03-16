/**
 * @file  tr_ai.c
 * @brief �g���[�i�[AI�v���O����
 * @author  HisashiSogabe
 * @date  06.04.25
 */
//============================================================================================
#include <gflib.h>

#include "system/vm_cmd.h"

#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
#include "poke_tool/tokusyu_def.h"
#include "waza_tool/wazano_def.h"
#include "waza_tool/waza_tool.h"

#include "battle/battle.h"
#include "battle/btl_calc.h"
#include "battle/btl_field.h"
#include "battle/btl_pokeparam.h"
#include "battle/btl_sideeff.h"
#include "battle/btl_util.h"

#include "item/item.h"

#include "tr_ai.h"
#include "tr_ai_def.h"
#include "tr_ai.cdat"

#include "arc_def.h"

#ifdef PM_DEBUG
#if defined DEBUG_ONLY_FOR_sogabe | defined DEBUG_ONLY_FOR_morimoto
#define POINT_VIEW
#endif
#endif

//=========================================================================
//
//=========================================================================
#define TR_AI_WAZATBL_MAX ( 48 )

//=========================================================================
//  AI�p�̍\���̐錾
//=========================================================================

typedef struct{
  u8          seq_no;
  u8          waza_pos;
  WazaID      waza_no;

  int         waza_point[ PTL_WAZA_MAX ];

  int         calc_work;
  u32         ai_bit;
  u32         ai_bit_temp;

  u8          status_flag;
  u8          ai_no;
  u8          select_waza_pos;    //�I�������Z�̃|�W�V����
  u8          select_waza_dir;    //�I�������Z�𓖂Ă鑊��

  u8          damage_loss[ PTL_WAZA_MAX ];
                    //���������́AAI�`�F�b�N�̂��тɃN���A����
  u16         use_waza[ BTL_POS_MAX ][ PTL_WAZA_MAX ];  //�g�p�����Z���X�g�b�N���Ă���
                                                      //�i��{�����Z�����m��Ȃ��悤�ɂ��邽�߁j
  u8          look_tokusei[ BTL_FRONT_POKE_MAX ];       //����������ێ�

  u16         have_item[2][4];      //�g���[�i�[�̎����A�C�e��
  u8          have_item_cnt[2];     //�g���[�i�[�̎����A�C�e����
  BtlPokePos  atk_btl_poke_pos;   //�U�����|�P�����̃N���C�A���gID
  BtlPokePos  def_btl_poke_pos;   //�h�䑤�|�P�����̃N���C�A���gID

  BtlRule       rule;
  BtlCompetitor competitor;

  WazaID        wazaID[TR_AI_WAZATBL_MAX ];
  WAZA_DATA     *wd[ TR_AI_WAZATBL_MAX ];

  HEAPID        heapID;
  ARCHANDLE*    handle;
  ARCHANDLE*    waza_handle;
  VM_CODE*      sequence;

  const BTL_MAIN_MODULE*    wk;
  BTL_SVFLOW_WORK*          svfWork;
  const BTL_POKE_CONTAINER* pokeCon;
  const BTL_POKEPARAM*      atk_bpp;
  const BTL_POKEPARAM*      def_bpp;

  VMCMD_RESULT  vmcmd_result;   //�X�N���v�g���䃏�[�N

#if 0
  u8  AI_ITEM_TYPE[2];
  u8  AI_ITEM_CONDITION[2];

  u16 AI_ITEM_NO[2];

  u8  AI_DirSelectClient[CLIENT_MAX];     //AI�őI�������U���Ώ�

  ITEMDATA    *item;                      //�A�C�e���e�[�u��

  u16       AI_CALC_COUNT[CLIENT_MAX];    //�v�Z���s������
  u16       AI_CALC_CONTINUE[CLIENT_MAX]; //�v�Z�r�����ǂ���
#endif
}TR_AI_WORK;

enum
{
  TR_AI_VM_STACK_SIZE = 16, ///<�g�p����X�^�b�N�̃T�C�Y
  TR_AI_VM_REG_SIZE   = 8,    ///<�g�p���郌�W�X�^�̐�

  WAZAPOINT_FLAT = 100,     //�Z�J��o������p�_���̏����l
};

typedef enum{
  COND_UNDER = 0,   //<
  COND_OVER,        //>
  COND_EQUAL,       //==
  COND_NOTEQUAL,    //!=
  COND_BIT,         //
  COND_NBIT,        //
  COND_OR_UNDER,    //<=
  COND_OR_OVER,     //>=
}BranchCond;

//=========================================================================
//  AI_THINK_BIT�錾
//=========================================================================
enum
{
  AI_THINK_NO_BASIC = 0,    //��{AI
  AI_THINK_NO_STRONG,       //�U���^AI
  AI_THINK_NO_EXPERT,       //�G�L�X�p�[�gAI
  AI_THINK_NO_HOJO,         //�⏕�ZAI
  AI_THINK_NO_GAMBLE,       //�M�����u��AI
  AI_THINK_NO_DAMAGE,       //�_���[�W�d��AI
  AI_THINK_NO_RELAY,        //�����[�^AI
  AI_THINK_NO_DOUBLE,       //�_�u���o�g��AI
  AI_THINK_NO_HP_CHECK,     //HP�Ŕ��fAI
  AI_THINK_NO_WEATHER,      //�V��ZAI
  AI_THINK_NO_HARASS,       //���₪�点AI

  AI_THINK_NO_MV_POKE,      //�ړ��|�P����
  AI_THINK_NO_SAFARI,       //�T�t�@���]�[��
  AI_THINK_NO_GUIDE,        //�K�C�h�o�g��

  AI_THINK_NO_MAX,          //
};

enum
{
  AI_THINK_BIT_BASIC    = 1 << AI_THINK_NO_BASIC,     //��{AI
  AI_THINK_BIT_STRONG   = 1 << AI_THINK_NO_STRONG,    //�U���^AI
  AI_THINK_BIT_EXPERT   = 1 << AI_THINK_NO_EXPERT,    //�G�L�X�p�[�gAI
  AI_THINK_BIT_HOJO     = 1 << AI_THINK_NO_HOJO,      //�⏕�ZAI
  AI_THINK_BIT_GAMBLE   = 1 << AI_THINK_NO_GAMBLE,    //�M�����u��AI
  AI_THINK_BIT_DAMAGE   = 1 << AI_THINK_NO_DAMAGE,    //�_���[�W�d��AI
  AI_THINK_BIT_RELAY    = 1 << AI_THINK_NO_RELAY,     //�����[�^AI
  AI_THINK_BIT_DOUBLE   = 1 << AI_THINK_NO_DOUBLE,    //�_�u���o�g��AI
  AI_THINK_BIT_HP_CHECK = 1 << AI_THINK_NO_HP_CHECK,  //HP�Ŕ��fAI
  AI_THINK_BIT_WEATHER  = 1 << AI_THINK_NO_WEATHER,   //�V��ZAI
  AI_THINK_BIT_HARASS   = 1 << AI_THINK_NO_HARASS,    //���₪�点AI
  AI_THINK_BIT_MV_POKE  = 1 << AI_THINK_NO_MV_POKE,   //�ړ��|�P����
  AI_THINK_BIT_SAFARI   = 1 << AI_THINK_NO_SAFARI,    //�T�t�@���]�[��
  AI_THINK_BIT_GUIDE    = 1 << AI_THINK_NO_GUIDE,     //�K�C�h�o�g��
};


//-----------------------------------------------------------------------------
//          �v���g�^�C�v�錾
//-----------------------------------------------------------------------------
static  BOOL  waza_ai_single( VMHANDLE* vmh, TR_AI_WORK* taw );
static  BOOL  waza_ai_double( VMHANDLE* vmh, TR_AI_WORK* taw );
static  void  tr_ai_sequence( VMHANDLE* vmh, TR_AI_WORK* taw );

static  VMCMD_RESULT  AI_IF_RND_UNDER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_RND_OVER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_RND_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_RND_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_INCDEC( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HP_UNDER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HP_OVER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HP_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_HP_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_POKESICK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_POKESICK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_WAZASICK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_WAZASICK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_DOKUDOKU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_DOKUDOKU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_CONTFLG( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_CONTFLG( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_SIDEEFF( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_SIDEEFF( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_UNDER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_OVER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_BIT( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_BIT( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_WAZANO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_WAZANO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_TABLE_JUMP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_TABLE_JUMP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HAVE_DAMAGE_WAZA( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_HAVE_DAMAGE_WAZA( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_TURN( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_TYPE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_IRYOKU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_COMP_POWER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_LAST_WAZA( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_WAZA_TYPE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_WAZA_TYPE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_FIRST( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_BENCH_COUNT( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WAZANO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WAZASEQNO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_TOKUSEI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_AISYOU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WAZA_AISYOU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_BENCH_COND( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_BENCH_COND( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WEATHER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_WAZA_SEQNO_JUMP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_WAZA_SEQNO_JUMP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_PARA_UNDER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_PARA_OVER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_PARA_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_PARA_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_WAZA_HINSHI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_WAZA_HINSHI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HAVE_WAZA( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_HAVE_WAZA( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HAVE_WAZA_SEQNO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_HAVE_WAZA_SEQNO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_WAZA_CHECK_STATE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_ESCAPE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_SAFARI_ESCAPE_JUMP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_SAFARI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_SOUBI_ITEM( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_SOUBI_EQUIP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_POKESEX( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_NEKODAMASI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_TAKUWAERU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_BTL_RULE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_BTL_COMPETITOR( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_RECYCLE_ITEM( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WORKWAZA_TYPE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WORKWAZA_POW( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WORKWAZA_SEQNO( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_MAMORU_COUNT( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_JUMP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_AIEND( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_LEVEL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_CHOUHATSU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_CHOUHATSU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_MIKATA_ATTACK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_HAVE_TYPE( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_HAVE_TOKUSEI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_ALREADY_MORAIBI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HAVE_ITEM( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_FLDEFF_CHECK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_SIDEEFF_COUNT( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_BENCH_HPDEC( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_BENCH_PPDEC( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_NAGETSUKERU_IRYOKU( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_PP_REMAIN( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_TOTTEOKI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_WAZA_KIND( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_LAST_WAZA_KIND( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_AGI_RANK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_SLOWSTART_TURN( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_BENCH_DAMAGE_MAX( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HAVE_BATSUGUN( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_LAST_WAZA_DAMAGE_CHECK( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_STATUS_UP( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_CHECK_STATUS_DIFF( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_CHECK_STATUS_UNDER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_CHECK_STATUS_OVER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_CHECK_STATUS_EQUAL( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_COMP_POWER_WITH_PARTNER( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_HINSHI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IFN_HINSHI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_GET_TOKUSEI( VMHANDLE* vmh, void* context_work );
static  VMCMD_RESULT  AI_IF_MIGAWARI( VMHANDLE* vmh, void* context_work );

static  void  ai_if_rnd( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_hp( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_pokesick( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_wazasick( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_moudoku( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_contflg( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_sideeff( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_wazano( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_bench_cond( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_waza_seq_no_jump( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_para( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_waza_hinshi( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_have_waza( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_have_waza_seqno( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_chouhatsu( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  void  ai_if_hinshi( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond );
static  BOOL  branch_act( VMHANDLE* vmh, BranchCond cond, int src, int dst, int adrs );
static  BtlPokePos get_poke_pos( TR_AI_WORK* taw, int side );
static  u32   get_table_data( int* adrs );
static  int   get_tokusei( TR_AI_WORK* taw, int side, BtlPokePos pos );
static  const BTL_POKEPARAM*  get_bpp( TR_AI_WORK* taw, BtlPokePos pos );
static  const BTL_POKEPARAM*  get_bpp_from_party( const BTL_PARTY* pty, u8 idx );
static  void  waza_no_stock( TR_AI_WORK* taw );
static  int   get_waza_param( TR_AI_WORK* taw, WazaID waza_no, WazaDataParam param );

//============================================================================================
/**
 *  �X�N���v�g�e�[�u��
 */
//============================================================================================
static const VMCMD_FUNC tr_ai_command_table[]={
  AI_IF_RND_UNDER,
  AI_IF_RND_OVER,
  AI_IF_RND_EQUAL,
  AI_IFN_RND_EQUAL,
  AI_INCDEC,
  AI_IF_HP_UNDER,
  AI_IF_HP_OVER,
  AI_IF_HP_EQUAL,
  AI_IFN_HP_EQUAL,
  AI_IF_POKESICK,
  AI_IFN_POKESICK,
  AI_IF_WAZASICK,
  AI_IFN_WAZASICK,
  AI_IF_DOKUDOKU,
  AI_IFN_DOKUDOKU,
  AI_IF_CONTFLG,
  AI_IFN_CONTFLG,
  AI_IF_SIDEEFF,
  AI_IFN_SIDEEFF,
  AI_IF_UNDER,
  AI_IF_OVER,
  AI_IF_EQUAL,
  AI_IFN_EQUAL,
  AI_IF_BIT,
  AI_IFN_BIT,
  AI_IF_WAZANO,
  AI_IFN_WAZANO,
  AI_IF_TABLE_JUMP,
  AI_IFN_TABLE_JUMP,
  AI_IF_HAVE_DAMAGE_WAZA,
  AI_IFN_HAVE_DAMAGE_WAZA,
  AI_CHECK_TURN,
  AI_CHECK_TYPE,
  AI_CHECK_IRYOKU,
  AI_COMP_POWER,
  AI_CHECK_LAST_WAZA,
  AI_IF_EQUAL,          //��AI_IF_WAZA_TYPE�@���e�I�ɂ͈ꏏ�Ȃ̂ŁA�܂Ƃ߂�
  AI_IFN_EQUAL,         //��AI_IFN_WAZA_TYPE�@���e�I�ɂ͈ꏏ�Ȃ̂ŁA�܂Ƃ߂�
  AI_IF_FIRST,
  AI_CHECK_BENCH_COUNT,
  AI_CHECK_WAZANO,
  AI_CHECK_WAZASEQNO,
  AI_CHECK_TOKUSEI,
  AI_CHECK_AISYOU,
  AI_CHECK_WAZA_AISYOU,
  AI_IF_BENCH_COND,
  AI_IFN_BENCH_COND,
  AI_CHECK_WEATHER,
  AI_IF_WAZA_SEQNO_JUMP,
  AI_IFN_WAZA_SEQNO_JUMP,
  AI_IF_PARA_UNDER,
  AI_IF_PARA_OVER,
  AI_IF_PARA_EQUAL,
  AI_IFN_PARA_EQUAL,
  AI_IF_WAZA_HINSHI,
  AI_IFN_WAZA_HINSHI,
  AI_IF_HAVE_WAZA,
  AI_IFN_HAVE_WAZA,
  AI_IF_HAVE_WAZA_SEQNO,
  AI_IFN_HAVE_WAZA_SEQNO,
  AI_IF_WAZA_CHECK_STATE,
  AI_ESCAPE,
  AI_SAFARI_ESCAPE_JUMP,
  AI_SAFARI,
  AI_CHECK_SOUBI_ITEM,
  AI_CHECK_SOUBI_EQUIP,
  AI_CHECK_POKESEX,
  AI_CHECK_NEKODAMASI,
  AI_CHECK_TAKUWAERU,
  AI_CHECK_BTL_RULE,
  AI_CHECK_BTL_COMPETITOR,
  AI_CHECK_RECYCLE_ITEM,
  AI_CHECK_WORKWAZA_TYPE,
  AI_CHECK_WORKWAZA_POW,
  AI_CHECK_WORKWAZA_SEQNO,
  AI_CHECK_MAMORU_COUNT,
  AI_JUMP,
  AI_AIEND,
  AI_IF_LEVEL,
  AI_IF_CHOUHATSU,
  AI_IFN_CHOUHATSU,
  AI_IF_MIKATA_ATTACK,
  AI_CHECK_HAVE_TYPE,
  AI_CHECK_HAVE_TOKUSEI,
  AI_IF_ALREADY_MORAIBI,
  AI_IF_HAVE_ITEM,
  AI_FLDEFF_CHECK,
  AI_CHECK_SIDEEFF_COUNT,
  AI_IF_BENCH_HPDEC,
  AI_IF_BENCH_PPDEC,
  AI_CHECK_NAGETSUKERU_IRYOKU,
  AI_CHECK_PP_REMAIN,
  AI_IF_TOTTEOKI,
  AI_CHECK_WAZA_KIND,
  AI_CHECK_LAST_WAZA_KIND,
  AI_CHECK_AGI_RANK,
  AI_CHECK_SLOWSTART_TURN,
  AI_IF_BENCH_DAMAGE_MAX,
  AI_IF_HAVE_BATSUGUN,
  AI_IF_LAST_WAZA_DAMAGE_CHECK,
  AI_CHECK_STATUS_UP,
  AI_CHECK_STATUS_DIFF,
  AI_IF_CHECK_STATUS_UNDER,
  AI_IF_CHECK_STATUS_OVER,
  AI_IF_CHECK_STATUS_EQUAL,
  AI_COMP_POWER_WITH_PARTNER,
  AI_IF_HINSHI,
  AI_IFN_HINSHI,
  AI_GET_TOKUSEI,
  AI_IF_MIGAWARI,
};

enum{
  AI_SEQ_THINK_INIT=0,
  AI_SEQ_THINK,
  AI_SEQ_END,
};

//============================================================================================
/**
 *  VM�C�j�V�����C�U�e�[�u��
 */
//============================================================================================
static  const VM_INITIALIZER  vm_init={
  TR_AI_VM_STACK_SIZE,                 //u16 stack_size; ///<�g�p����X�^�b�N�̃T�C�Y
  TR_AI_VM_REG_SIZE,                   //u16 reg_size;   ///<�g�p���郌�W�X�^�̐�
  tr_ai_command_table,                 //const VMCMD_FUNC * command_table; ///<�g�p���鉼�z�}�V�����߂̊֐��e�[�u��
  NELEMS( tr_ai_command_table ),       //const u32 command_max;      ///<�g�p���鉼�z�}�V�����ߒ�`�̍ő吔
};

//============================================================================================
/**
 * @brief VM������
 *
 * @param[in] wk
 * @param[in] pokeCon
 * @param[in] ai_bit  �N������AI���r�b�g�Ŏw��
 * @param[in] heapID  �q�[�vID
 */
//============================================================================================
VMHANDLE* TR_AI_Init( const BTL_MAIN_MODULE* wk, BTL_SVFLOW_WORK* svfWork, const BTL_POKE_CONTAINER* pokeCon, u32 ai_bit, HEAPID heapID )
{
  VMHANDLE* vmh;
  TR_AI_WORK*  taw = GFL_HEAP_AllocClearMemory( heapID, sizeof( TR_AI_WORK ) );

  //AIBIT�̍ő�l�`�F�b�N
  GF_ASSERT( AI_THINK_NO_MAX < 32 );

  taw->heapID = heapID;
  taw->vmcmd_result = VMCMD_RESULT_CONTINUE;
  taw->ai_bit_temp = ai_bit;

  taw->handle = GFL_ARC_OpenDataHandle( ARCID_TR_AI, heapID );
  taw->waza_handle = WAZADATA_OpenDataHandle( heapID );

  {
    int i;

    for( i = 0 ; i < TR_AI_WAZATBL_MAX ; i++ )
    {
      taw->wd[ i ] = GFL_HEAP_AllocMemory( heapID, WAZADATA_GetWorkSize() );
    }
  }

  taw->wk = wk;
  taw->svfWork = svfWork;
  taw->pokeCon = pokeCon;

  taw->rule        = BTL_MAIN_GetRule( wk );
  taw->competitor  = BTL_MAIN_GetCompetitor( wk );

  vmh = VM_Create( heapID, &vm_init );
  VM_Init( vmh, taw );

  return vmh;
}

//============================================================================================
/**
 * @brief VM���C��
 *
 * @param[in] vmh ���z�}�V������\���̂ւ̃|�C���^
 *
 * @retval  TRUE:�v�Z�� FALSE:�v�Z�I��
 */
//============================================================================================
BOOL  TR_AI_Main( VMHANDLE* vmh )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)VM_GetContext( vmh );
  BOOL  ret = FALSE;

#if 0 //@todo �_�u�����쐬
  if( taw->rule == BTL_RULE_SINGLE )
  {
    taw->def_btl_poke_pos = taw->atk_btl_poke_pos ^ 1;
    ret = waza_ai_single( vmh, taw );
  }
  else if( taw->rule == BTL_RULE_DOUBLE )
  {
    ret = waza_ai_double( vmh, taw );
  }
#else
  taw->def_btl_poke_pos = taw->atk_btl_poke_pos ^ 1;
  ret = waza_ai_single( vmh, taw );
#endif

  return ret;
}

//============================================================================================
/**
 * @brief VM�I��
 *
 * @param[in] vmh ���z�}�V������\���̂ւ̃|�C���^
 */
//============================================================================================
void  TR_AI_Exit( VMHANDLE* vmh )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)VM_GetContext( vmh );
  int i;

  GFL_ARC_CloseDataHandle( taw->handle );
  GFL_ARC_CloseDataHandle( taw->waza_handle );

  for( i = 0 ; i < TR_AI_WAZATBL_MAX ; i++ )
  {
    GFL_HEAP_FreeMemory ( taw->wd[ i ] );
  }
  GFL_HEAP_FreeMemory ( taw );
  VM_Delete( vmh );
}

//============================================================================================
/**
 * @brief VM�N��
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] unselect  �I�ׂȂ��Z
 * @param[in] pos       �N������|�P�����N���C�A���gID
 */
//============================================================================================
void  TR_AI_Start( VMHANDLE* vmh, u8* unselect, BtlPokePos pos )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)VM_GetContext( vmh );
  int i;

  GF_ASSERT_MSG( taw->ai_bit == 0, "AI���v�Z���ł�" );

  taw->seq_no = 0;
  taw->waza_no = 0;
  taw->waza_pos = 0;
  taw->calc_work = 0;
  taw->status_flag = 0;
  taw->ai_no = 0;
  taw->atk_btl_poke_pos = pos;
  taw->ai_bit = taw->ai_bit_temp;

  for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
  {
    taw->waza_point[ i ] = WAZAPOINT_FLAT;
  }

  {
    //�J��o���Ȃ��Z�̃|�C���g�͂O�ɂ��ď��O
    for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
    {
      if( unselect[ i ] == 0 )
      {
        taw->waza_point[ i ] = 0;
      }
      taw->damage_loss[ i ] = 100 - GFL_STD_MtRand( 16 );
    }
  }
}

//============================================================================================
/**
 * @brief �I�������Z�̃|�W�V������Ԃ�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 */
//============================================================================================
u8  TR_AI_GetSelectWazaPos( VMHANDLE* vmh )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)VM_GetContext( vmh );

  return taw->select_waza_pos;    //�I�������Z�̃|�W�V����
}

//============================================================================================
/**
 * @brief �I�������Z�̍U���Ώۂ�Ԃ�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 */
//============================================================================================
u8  TR_AI_GetSelectWazaDir( VMHANDLE* vmh )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)VM_GetContext( vmh );

  return taw->select_waza_dir;    //�I�������Z�𓖂Ă鑊��
}

//============================================================================================
/**
 * @brief AI�V�[�P���X�i�V���O���j
 *
 * @param[in] taw
 *
 * @retval  TRUE:�v�Z�� FALSE:�v�Z�I��
 */
//============================================================================================
static  BOOL  waza_ai_single( VMHANDLE* vmh, TR_AI_WORK* taw )
{
  taw->atk_bpp = get_bpp( taw, taw->atk_btl_poke_pos );
  taw->def_bpp = get_bpp( taw, taw->def_btl_poke_pos );

  waza_no_stock( taw );

  while( taw->ai_bit )
  {
    if( taw->ai_bit & 1 )
    {
      if( ( taw->status_flag & AI_STATUSFLAG_CONTINUE ) == 0 )
      {
        taw->seq_no = AI_SEQ_THINK_INIT;
      }
      tr_ai_sequence( vmh, taw );
    }
    taw->ai_bit >>= 1;
    taw->ai_no++;
    taw->waza_pos = 0;
  }
  if( taw->status_flag & AI_STATUSFLAG_ESCAPE )
  {
    taw->select_waza_pos = AI_ENEMY_ESCAPE;
  }
  //@todo ����T�t�@���͂Ȃ��̂ŃR�����g
//  else if(sp->AIWT.AI_STATUSFLAG&AI_STATUSFLAG_SAFARI){
//    taw->select_waza_pos = AI_ENEMY_SAFARI;
//  }
  else{
    int i;
    int poscnt = 1;
    u8  point[ PTL_WAZA_MAX ];
    u8  poswork[ PTL_WAZA_MAX ];

    point[ 0 ] = taw->waza_point[ 0 ];
    poswork[ 0 ] = 0;

    for( i = 1 ; i < PTL_WAZA_MAX ; i++ )
    {
      //�Z���Ȃ��Ƃ���́A����
      if( i < BPP_WAZA_GetCount( taw->atk_bpp ) )
      {
        if( point[ 0 ] == taw->waza_point[ i ]){
          point[ poscnt ] = taw->waza_point[ i ];
          poswork[ poscnt++ ] = i;
        }
        if( point[ 0 ] < taw->waza_point[ i ] )
        {
          poscnt = 1;
          point[ 0 ] = taw->waza_point[ i ];
          poswork[ 0 ] = i;
        }
      }
    }
    taw->select_waza_pos = poswork[ GFL_STD_MtRand( poscnt ) ];
  }

#ifdef POINT_VIEW
  {
    int i;
    for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
    {
      OS_TPrintf( "waza%d:%d\n", i+1, taw->waza_point[ i ] );
    }
  }
#endif

  taw->select_waza_dir = taw->def_btl_poke_pos;

  return FALSE;
}

//============================================================================================
/**
 * @brief AI�V�[�P���X�i�_�u���j
 *
 * @param[in] taw
 *
 * @retval  TRUE:�v�Z�� FALSE:�v�Z�I��
 */
//============================================================================================
static  BOOL  waza_ai_double( VMHANDLE* vmh, TR_AI_WORK* taw )
{
  //@todo �_�u���͖��쐬
  return FALSE;
}

//============================================================================================
/**
 * @brief AI�V�[�P���X���C��
 *
 * @param[in] taw
 *
 * @retval  TRUE:�v�Z�� FALSE:�v�Z�I��
 */
//============================================================================================
static  void  tr_ai_sequence( VMHANDLE* vmh, TR_AI_WORK* taw )
{
  while( taw->seq_no != AI_SEQ_END )
  {
    switch( taw->seq_no ){
    case AI_SEQ_THINK_INIT:
      {
        //PP���Ȃ��ꍇ�͋Z�Ȃ� @todo PP���Ȃ���ԂȂ�waza_point��0�ɂȂ��Ă���͂��Ȃ̂ŁA�������Ń`�F�b�N
        if( BPP_WAZA_GetPP( taw->atk_bpp, taw->waza_pos ) == 0 )
        {
          taw->waza_no = 0;
        }
        else{
          taw->waza_no = BPP_WAZA_GetID( taw->atk_bpp, taw->waza_pos );
          taw->sequence = GFL_ARC_LoadDataAllocByHandle( taw->handle, taw->ai_no, taw->heapID );
          VM_Start( vmh, taw->sequence );
        }
      }
      taw->seq_no++;
      /*fallthru*/
    case AI_SEQ_THINK:
      if( taw->waza_no != 0 )
      {
        BOOL  ret = VM_Control( vmh );
        if( taw->status_flag & AI_STATUSFLAG_END )
        {
          GFL_HEAP_FreeMemory( taw->sequence );
          taw->sequence = NULL;
        }
      }
      else{
        taw->waza_point[ taw->waza_pos ] = 0;
        taw->status_flag |= AI_STATUSFLAG_END;
      }
      if( taw->status_flag & AI_STATUSFLAG_END )
      {
        taw->waza_pos++;
        if( ( taw->waza_pos < BPP_WAZA_GetCount( taw->atk_bpp ) ) &&
          ( ( taw->status_flag & AI_STATUSFLAG_FINISH ) == 0 ) )
        {
          taw->seq_no = AI_SEQ_THINK_INIT;
        }
        else
        {
          taw->seq_no++;
        }
        taw->status_flag &= AI_STATUSFLAG_END_OFF;
      }
      break;
    default:
    case AI_SEQ_END:
      taw->seq_no = AI_SEQ_END;
      break;
    }
  }
}

//------------------------------------------------------------
//  �����_������
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_RND_UNDER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_rnd( vmh, taw, COND_UNDER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_RND_OVER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_rnd( vmh, taw, COND_OVER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_RND_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_rnd( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_RND_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_rnd( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  �����_�����򃁃C��
//------------------------------------------------------------
static  void  ai_if_rnd( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int value = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  int rnd = GFL_STD_MtRand( 256 );

  branch_act( vmh, cond, rnd, value, adrs );
}

//------------------------------------------------------------
//  �|�C���g�����Z
//------------------------------------------------------------
static  VMCMD_RESULT  AI_INCDEC( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int value = ( int )VMGetU32( vmh );

  taw->waza_point[ taw->waza_pos ] += value;

  if( taw->waza_point[ taw->waza_pos ] < 0 )
  {
    taw->waza_point[ taw->waza_pos ] = 0;
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  HP�ł̕���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HP_UNDER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_hp( vmh, taw, COND_UNDER );

  return taw->vmcmd_result;
}

static  VMCMD_RESULT  AI_IF_HP_OVER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_hp( vmh, taw, COND_OVER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_HP_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_hp( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_HP_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_hp( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  HP�ł̕��򏈗����C��
//------------------------------------------------------------
static  void  ai_if_hp( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  int value = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  int hpratio = BPP_GetHPRatio( get_bpp( taw, pos ) );

  branch_act( vmh, cond, hpratio >> FX32_SHIFT, value, adrs );
}

//------------------------------------------------------------
//  �|�P���������炩�̏�Ԉُ�ɂ������Ă��邩�`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_POKESICK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_pokesick( vmh, context_work, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_POKESICK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_pokesick( vmh, context_work, COND_EQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �|�P���������炩�̏�Ԉُ�ɂ������Ă��邩�`�F�b�N���ĕ��򏈗����C��
//------------------------------------------------------------
static  void  ai_if_pokesick( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  PokeSick  sick  = BPP_GetPokeSick( get_bpp( taw, pos ) );

  branch_act( vmh, cond, sick, POKESICK_NULL, adrs );
}

//------------------------------------------------------------
//  �|�P���������炩�̏�Ԉُ�(WAZASICK)�ɂ������Ă��邩�`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_WAZASICK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_wazasick( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_WAZASICK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_wazasick( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �|�P���������炩�̏�Ԉُ�(WAZASICK)�ɂ������Ă��邩�`�F�b�N���ĕ��򏈗����C��
//------------------------------------------------------------
static  void  ai_if_wazasick( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  WazaSick  value = ( WazaSick )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  BOOL  sick = BPP_CheckSick( get_bpp( taw, pos ), value );

  branch_act( vmh, cond, sick, TRUE, adrs );
}

//------------------------------------------------------------
//  �|�P�������ǂ��ǂ��ɂ������Ă��邩�`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_DOKUDOKU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_moudoku( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_DOKUDOKU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_moudoku( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �|�P�������u�����ǂ��v�ɂ������Ă��邩���`�F�b�N���ĕ���
//------------------------------------------------------------
static  void  ai_if_moudoku( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  const BTL_POKEPARAM* bpp = get_bpp( taw, pos );
  BOOL result = FALSE;

  if( BPP_CheckSick(bpp, WAZASICK_DOKU) )
  {
     BPP_SICK_CONT cont = BPP_GetSickCont( bpp, WAZASICK_DOKU );
     if( BPP_SICKCONT_IsMoudokuCont(cont) ){
      result = TRUE;
    }
  }

  branch_act( vmh, cond, result, TRUE, adrs );
}

//------------------------------------------------------------
//  �|�P���������炩�̏�Ԉُ�(CONTFLG)�ɂ������Ă��邩�`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_CONTFLG( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_contflg( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_CONTFLG( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_contflg( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  �|�P���������炩�̏�Ԉُ�(CONTFLG)�ɂ������Ă��邩�`�F�b�N���ĕ��򏈗����C��
//------------------------------------------------------------
static  void  ai_if_contflg( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  BppContFlag value = ( BppContFlag )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  BOOL contflg = BPP_CONTFLAG_Get( get_bpp( taw, pos ), value );

  branch_act( vmh, cond, contflg, TRUE, adrs );
}

//------------------------------------------------------------
//  ��ɑ΂��ĉ��炩�̏�ԕω����������Ă��邩�`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_SIDEEFF( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_sideeff( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_SIDEEFF( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_sideeff( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  ��ɑ΂��ĉ��炩�̏�ԕω����������Ă��邩�`�F�b�N���ĕ��򏈗����C��
//------------------------------------------------------------
static  void  ai_if_sideeff( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  BtlSideEffect value = ( BtlSideEffect )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  BtlSideEffect sideeff = 0; //@todo �擾�֐��ɒu��������K�v������

  branch_act( vmh, cond, sideeff, value, adrs );
}

//------------------------------------------------------------
//  ���[�N�Ɣ�r���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_UNDER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if( vmh, taw, COND_UNDER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_OVER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if( vmh, taw, COND_OVER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_BIT( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if( vmh, taw, COND_BIT );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_BIT( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if( vmh, taw, COND_NBIT );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  ���[�N�Ɣ�r���ĕ��򏈗����C��
//------------------------------------------------------------
static  void  ai_if( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int value = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );

  //OS_TPrintf("ai_if:\nsrc:%d dst:%d cond:%d\n",taw->calc_work,value,cond);

  branch_act( vmh, cond, taw->calc_work, value, adrs );
}

//------------------------------------------------------------
//  ���v�Z���Ă���Z�i���o�[�Ɣ�r���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_WAZANO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_wazano( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_WAZANO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_wazano( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  ���v�Z���Ă���Z�i���o�[�Ɣ�r���ĕ��򏈗����C��
//------------------------------------------------------------
static  void  ai_if_wazano( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int value = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );

  branch_act( vmh, cond, taw->waza_no, value, adrs );
}

//------------------------------------------------------------
//  �w�肳�ꂽ�e�[�u�����Q�Ƃ��ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_TABLE_JUMP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int tbl_adrs  = ( int )VMGetU32( vmh );
  int jump_adrs = ( int )VMGetU32( vmh );
  int data;

  while( ( data = get_table_data( &tbl_adrs ) ) != TR_AI_TABLE_END )
  {
    if( taw->calc_work == data )
    {
      VMCMD_Jump( vmh, vmh->adrs + jump_adrs );
      break;
    }
  }

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_TABLE_JUMP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int tbl_adrs  = ( int )VMGetU32( vmh );
  int jump_adrs = ( int )VMGetU32( vmh );
  int data;

  while( ( data = get_table_data( &tbl_adrs ) ) != TR_AI_TABLE_END )
  {
    if( taw->calc_work == data )
    {
      return taw->vmcmd_result;
    }
  }
  VMCMD_Jump( vmh, vmh->adrs + jump_adrs );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �������_���[�W�Z�������Ă��邩�`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HAVE_DAMAGE_WAZA( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int adrs = ( int )VMGetU32( vmh );
  int i;

  for( i = 0 ; i < BPP_WAZA_GetCount( taw->atk_bpp ) ; i++ )
  {
    if( get_waza_param( taw, BPP_WAZA_GetID( taw->atk_bpp, i ), WAZAPARAM_POWER ) != 0 )
    {
      break;
    }
  }

  if( i < BPP_WAZA_GetCount( taw->atk_bpp ) )
  {
    VMCMD_Jump( vmh, vmh->adrs + adrs );
  }

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_HAVE_DAMAGE_WAZA( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int adrs = ( int )VMGetU32( vmh );
  int i;

  for( i = 0 ; i < BPP_WAZA_GetCount( taw->atk_bpp ) ; i++ )
  {
    if( get_waza_param( taw, BPP_WAZA_GetID( taw->atk_bpp, i ), WAZAPARAM_POWER ) != 0 )
    {
      break;
    }
  }

  if( i == BPP_WAZA_GetCount( taw->atk_bpp ) )
  {
    VMCMD_Jump( vmh, vmh->adrs + adrs );
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �^�[���̃`�F�b�N�i�����^�[���ڂ������[�N�ɓ����j
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_TURN( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = 1;  //@todo �擾�֐��ɒu��������K�v������܂�

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �^�C�v�̃`�F�b�N(�|�P�������邢�͋Z�̃^�C�v�����[�N�ɓ����j
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_TYPE( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  PokeTypePair  atk_type = BPP_GetPokeType( taw->atk_bpp );
  PokeTypePair  def_type = BPP_GetPokeType( taw->def_bpp );

  switch( side ){
  case CHECK_ATTACK_TYPE1:
    taw->calc_work = PokeTypePair_GetType1( atk_type );
    break;
  case CHECK_DEFENCE_TYPE1:
    taw->calc_work = PokeTypePair_GetType1( def_type );
    break;
  case CHECK_ATTACK_TYPE2:
    taw->calc_work = PokeTypePair_GetType2( atk_type );
    break;
  case CHECK_DEFENCE_TYPE2:
    taw->calc_work = PokeTypePair_GetType2( def_type );
    break;
  case CHECK_WAZA:
    taw->calc_work = get_waza_param( taw, taw->waza_no, WAZAPARAM_TYPE );
    break;
  case CHECK_ATTACK_FRIEND_TYPE1:
    {
      PokeTypePair  atk_type_f = BPP_GetPokeType( get_bpp( taw, get_poke_pos( taw, CHECK_ATTACK_FRIEND ) ) );
      taw->calc_work = PokeTypePair_GetType1( atk_type_f );
    }
    break;
  case CHECK_DEFENCE_FRIEND_TYPE1:
    {
      PokeTypePair  def_type_f = BPP_GetPokeType( get_bpp( taw, get_poke_pos( taw, CHECK_DEFENCE_FRIEND ) ) );
      taw->calc_work = PokeTypePair_GetType1( def_type_f );
    }
    break;
  case CHECK_ATTACK_FRIEND_TYPE2:
    {
      PokeTypePair  atk_type_f = BPP_GetPokeType( get_bpp( taw, get_poke_pos( taw, CHECK_ATTACK_FRIEND ) ) );
      taw->calc_work = PokeTypePair_GetType2( atk_type_f );
    }
    break;
  case CHECK_DEFENCE_FRIEND_TYPE2:
    {
      PokeTypePair  def_type_f = BPP_GetPokeType( get_bpp( taw, get_poke_pos( taw, CHECK_DEFENCE_FRIEND ) ) );
      taw->calc_work = PokeTypePair_GetType2( def_type_f );
    }
    break;
  default:
    GF_ASSERT(0);
    break;
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �U���Z���ǂ����̃`�F�b�N�i�Z�̈З͂����[�N�ɓ����)�iHGSS���_�Ŗ��g�p�R�}���h�j
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_IRYOKU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = WAZADATA_GetPower( taw->waza_no );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �З͂���ԍ������̃`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_COMP_POWER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int loss_flag = ( int )VMGetU32( vmh );

  //@todo �Z�̃_���[�W���擾����֐������K�v������܂��@�Ƃ肠�����Z�̈З͂Ŕ���
  {
    int i;
    int src_pow = get_waza_param( taw, taw->waza_no, WAZAPARAM_POWER );

    if( src_pow == 0 )
    {
      taw->calc_work = COMP_POWER_NONE;
    }
    else
    {
      taw->calc_work = COMP_POWER_TOP;

      for( i = 0 ; i < BPP_WAZA_GetCount( taw->atk_bpp ) ; i++ )
      {
        int pow = get_waza_param( taw, BPP_WAZA_GetID( taw->atk_bpp, i ), WAZAPARAM_POWER );

        if( i == taw->waza_pos ) continue;
        if( pow > src_pow )
        {
          taw->calc_work = COMP_POWER_NOTOP;
          break;
        }
      }
    }
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �O�̃^�[���Ɏg�����Z�̃`�F�b�N�i�Z�̃i���o�[�����[�N�ɓ����j
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_LAST_WAZA( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_GetPrevWazaID( get_bpp( taw, pos ) );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �ǂ��炪��s���`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_FIRST( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int cond  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );

  //@todo �ǂ��炪��U�����擾����֐������K�v������܂�

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �T�������̂��邩�`�F�b�N�i�������[�N�ɓ����j
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_BENCH_COUNT( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  u8  clientID = BTL_MAIN_BtlPosToClientID( taw->wk, pos );
  const BTL_PARTY*  pty = BTL_POKECON_GetPartyDataConst( taw->pokeCon, clientID );
  //�O�q�̐�
  u8  front_count = BTL_MAIN_GetClientCoverPosCount( taw->wk, clientID );
  int i;

  taw->calc_work = 0;

  for( i = front_count ; i < BTL_PARTY_GetMemberCount( pty ) ; i++ )
  {
    const BTL_POKEPARAM* bpp = get_bpp_from_party( pty, i );
    //@todo �^�}�S�̏ꍇ���l�����Ȃ���΂Ȃ�Ȃ��i�c�J�N�̘b���ƃ^�}�S�Ȃ炻������BTL_PARTY�ɓ���Ȃ���΂����Ƃ������Ƃ݂����j
    if( !BPP_IsDead( bpp ) )
    {
      taw->calc_work++;
    }
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ���݂̋Z�i���o�[�̃`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WAZANO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = taw->waza_no;

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ���݂̋Z�i���o�[�̃V�[�P���X�i���o�[�̃`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WAZASEQNO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = get_waza_param( taw, taw->waza_no, WAZAPARAM_AI_SEQNO );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �Ƃ������̃`�F�b�N�i�Ƃ������i���o�[�����[�N�ɓ����j
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_TOKUSEI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = get_tokusei( taw, side, pos );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �����Ƒ���̑����`�F�b�N�iHGSS���_�Ŗ��g�p�R�}���h�j
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_AISYOU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �Z�̃^�C�v�Ƒ���̑������`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WAZA_AISYOU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  BtlTypeAff  aisyou  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  PokeTypePair  def_type = BPP_GetPokeType( taw->def_bpp );
  PokeType  waza_type = get_waza_param( taw, taw->waza_no, WAZAPARAM_TYPE );
  BtlTypeAff  aff1 = BTL_CALC_TypeAff( waza_type, PokeTypePair_GetType1( def_type ) );
  BtlTypeAff  aff2 = BTL_CALC_TypeAff( waza_type, PokeTypePair_GetType2( def_type ) );
  BtlTypeAff  aff = BTL_CALC_TypeAffMul( aff1, aff2 );

  //@todo �Ƃ肠���������Ƃ��Ȃɂ��l�����ɒP���ɑ����`�F�b�N

  branch_act( vmh, COND_EQUAL, aff, aisyou, adrs );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �T���̏�ԃ`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_BENCH_COND( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_bench_cond( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_BENCH_COND( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_bench_cond( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  �T���̏�ԃ`�F�b�N���ĕ��򏈗����C��
//------------------------------------------------------------
static  void  ai_if_bench_cond( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  u8  clientID = BTL_MAIN_BtlPosToClientID( taw->wk, pos );
  const BTL_PARTY*  pty = BTL_POKECON_GetPartyDataConst( taw->pokeCon, clientID );
  //�O�q�̐�
  u8  front_count = BTL_MAIN_GetClientCoverPosCount( taw->wk, clientID );
  int i;

  for( i = front_count ; i < BTL_PARTY_GetMemberCount( pty ) ; i++ )
  {
    const BTL_POKEPARAM* bpp = get_bpp_from_party( pty, i );

    if( !BPP_IsDead( bpp ) )
    {
      PokeSick  bench_sick = BPP_GetPokeSick( bpp );
      if( branch_act( vmh, cond, bench_sick, POKESICK_NULL, adrs ) )
      {
        break;
      }
    }
  }
}

//------------------------------------------------------------
//  �V��`�F�b�N�i�V��i���o�[�����[�N�ɓ����j
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WEATHER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = BTL_FIELD_GetWeather();

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �Z�̃V�[�P���X�i���o�[���`�F�b�N���āA����
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_WAZA_SEQNO_JUMP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_waza_seq_no_jump( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_WAZA_SEQNO_JUMP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_waza_seq_no_jump( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  �Z�̃V�[�P���X�i���o�[���`�F�b�N���āA���򏈗����C��
//------------------------------------------------------------
static  void  ai_if_waza_seq_no_jump( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int seqno = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  int data  = get_waza_param( taw, taw->waza_no, WAZAPARAM_AI_SEQNO );

  branch_act( vmh, cond, data, seqno, adrs );
}

//------------------------------------------------------------
//  �����⑊��̃p�����[�^�ω��l���Q�Ƃ��ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_PARA_UNDER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_para( vmh, taw, COND_UNDER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_PARA_OVER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_para( vmh, taw, COND_OVER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_PARA_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_para( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_PARA_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_para( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �����⑊��̃p�����[�^�ω��l���Q�Ƃ��ĕ��򏈗����C��
//------------------------------------------------------------
static  void  ai_if_para( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  BppValueID para  = ( int )VMGetU32( vmh );
  int value = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  int data  = BPP_GetValue( get_bpp( taw, pos ), para );

  branch_act( vmh, cond, data, value, adrs );
}

//------------------------------------------------------------
//  �Z�̃_���[�W�v�Z�����đ��肪�m���ɂȂ邩�`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_WAZA_HINSHI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_waza_hinshi( vmh, taw, COND_OR_UNDER );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_WAZA_HINSHI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_waza_hinshi( vmh, taw, COND_OVER );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  �Z�̃_���[�W�v�Z�����đ��肪�m���ɂȂ邩�`�F�b�N���ĕ��򏈗����C��
//------------------------------------------------------------
static  void  ai_if_waza_hinshi( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int loss_flag = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  int hp = BPP_GetValue( taw->def_bpp, BPP_HP );
  int damage = 1; //@todo �_���[�W�擾�������K�v

  branch_act( vmh, cond, hp, damage, adrs );
}

//------------------------------------------------------------
//  ����̋Z�������Ă��邩�̃`�F�b�N�����ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HAVE_WAZA( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_have_waza( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_HAVE_WAZA( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_have_waza( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  ����̋Z�������Ă��邩�̃`�F�b�N�����ĕ��򏈗����C��
//------------------------------------------------------------
static  void  ai_if_have_waza( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  WazaID waza  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  int i;
  BOOL  ret = FALSE;

  switch( side ){
  case CHECK_ATTACK:
    for( i = 0 ; i < BPP_WAZA_GetCount( taw->atk_bpp ) ; i++ )
    {
      WazaID have_waza = BPP_WAZA_GetID( taw->atk_bpp, i );
      if( have_waza == waza )
      {
        ret = TRUE;
        break;
      }
    }
    break;
  case CHECK_ATTACK_FRIEND:
    {
      const BTL_POKEPARAM* bpp = get_bpp( taw, get_poke_pos( taw, CHECK_ATTACK_FRIEND ) );
      if( BPP_IsDead( bpp ) )
      {
        break;
      }
      for( i = 0 ; i < BPP_WAZA_GetCount( bpp ) ; i++ )
      {
        WazaID have_waza = BPP_WAZA_GetID( bpp, i );
        if( have_waza == waza )
        {
          ret = TRUE;
          break;
        }
      }
    }
    break;
  case CHECK_DEFENCE:
    for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
    {
      if( taw->use_waza[ pos ][ i ] == waza )
      {
        ret = TRUE;
        break;
      }
    }
    break;
  default:
    OS_Printf("�����ɂ���̂́A��������\n");
    break;
  }

  branch_act( vmh, cond, ret, TRUE, adrs );
}

//------------------------------------------------------------
//  ����̋Z�V�[�P���X�������Ă��邩�̃`�F�b�N�����ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HAVE_WAZA_SEQNO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_have_waza_seqno( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_HAVE_WAZA_SEQNO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_have_waza_seqno( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}
//------------------------------------------------------------
//  ����̋Z�V�[�P���X�������Ă��邩�̃`�F�b�N�����ĕ��򏈗����C��
//------------------------------------------------------------
static  void  ai_if_have_waza_seqno( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side  = ( int )VMGetU32( vmh );
  int seqno = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  int i;
  BOOL  ret = FALSE;

  switch( side ){
  case CHECK_ATTACK:
    for( i = 0 ; i < BPP_WAZA_GetCount( taw->atk_bpp ) ; i++ )
    {
      int have_seqno = get_waza_param( taw, BPP_WAZA_GetID( taw->atk_bpp, i ), WAZAPARAM_AI_SEQNO );
      if( have_seqno == seqno )
      {
        ret = TRUE;
        break;
      }
    }
    break;
  case CHECK_DEFENCE:
    for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
    {
      int have_seqno = get_waza_param( taw, taw->use_waza[ pos ][ i ], WAZAPARAM_AI_SEQNO );
      if( have_seqno == seqno )
      {
        ret = TRUE;
        break;
      }
    }
    break;
  default:
    OS_Printf("�����ɂ���̂́A��������\n");
    break;
  }

  branch_act( vmh, cond, ret, TRUE, adrs );
}


//------------------------------------------------------------
//  �Z�̏�Ԃ��`�F�b�N�����ĕ���iHGSS���_�Ŗ��g�p�R�}���h�j
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_WAZA_CHECK_STATE( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �ɂ�������񂽂�
//------------------------------------------------------------
static  VMCMD_RESULT  AI_ESCAPE( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �T�t�@���]�[���ł̓�����m�����v�Z���ē�����Ƃ��̃A�h���X���w��
//------------------------------------------------------------
static  VMCMD_RESULT  AI_SAFARI_ESCAPE_JUMP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �T�t�@���]�[���ł̓���A�N�V������I��
//------------------------------------------------------------
static  VMCMD_RESULT  AI_SAFARI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    �����A�C�e���̃`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_SOUBI_ITEM( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_GetItem( get_bpp( taw, pos ) );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    �������ʂ̃`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_SOUBI_EQUIP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  u32 item = BPP_GetItem( get_bpp( taw, pos ) );

  taw->calc_work = ITEM_GetParam( item, ITEM_PRM_EQUIP, taw->heapID );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    �|�P�����̐��ʂ̃`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_POKESEX( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_GetValue( get_bpp( taw, pos ), BPP_SEX );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    �˂����܂��ł��邩�`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_NEKODAMASI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  //@todo �̂��قǕύX
  taw->calc_work = ( BPP_GetTurnCount( get_bpp( taw, pos ) ) == 0 );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    �����킦��J�E���^�̃`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_TAKUWAERU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_COUNTER_Get( get_bpp( taw, pos ), BPP_COUNTER_TAKUWAERU );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    BTL_RULE�̃`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_BTL_RULE( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = taw->rule;

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    BTL_COMPETITOR�̃`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_BTL_COMPETITOR( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = taw->competitor;

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//    ���T�C�N���ł���A�C�e���̃`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_RECYCLE_ITEM( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_GetConsumedItem( get_bpp( taw, pos ) );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ���[�N�ɓ����Ă���Z�i���o�[�̃^�C�v���`�F�b�N�iHGSS���_�Ŗ��g�p�R�}���h�j
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WORKWAZA_TYPE( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ���[�N�ɓ����Ă���Z�i���o�[�̈З͂��`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WORKWAZA_POW( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT( taw->calc_work < WAZANO_MAX );

  taw->calc_work = get_waza_param( taw, taw->calc_work, WAZAPARAM_POWER );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ���[�N�ɓ����Ă���Z�i���o�[�̃V�[�P���X�i���o�[���`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WORKWAZA_SEQNO( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = get_waza_param( taw, taw->calc_work, WAZAPARAM_AI_SEQNO );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �܂���J�E���^���`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_MAMORU_COUNT( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  const BTL_POKEPARAM* bpp = get_bpp( taw, pos );

  if( ( BPP_GetPrevWazaID( bpp ) != WAZANO_MAMORU ) &&
      ( BPP_GetPrevWazaID( bpp ) != WAZANO_MIKIRI ) &&
      ( BPP_GetPrevWazaID( bpp ) != WAZANO_KORAERU ) )
  {
    taw->calc_work = 0;
  }
  else
  {
    taw->calc_work = BPP_GetWazaContCounter( bpp );
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �V�[�P���X�W�����v
//------------------------------------------------------------
static  VMCMD_RESULT  AI_JUMP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int adrs  = ( int )VMGetU32( vmh );

  VMCMD_Jump( vmh, vmh->adrs + adrs );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �V�[�P���X�I��
//------------------------------------------------------------
static  VMCMD_RESULT  AI_AIEND( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->status_flag |= AI_STATUSFLAG_END;

  //���z�}�V����~
  VM_End( vmh );

  return VMCMD_RESULT_SUSPEND;
}

//------------------------------------------------------------
//  ���݂��̃��x�����`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_LEVEL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int value  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  int cond[ 3 ] = { COND_OVER, COND_UNDER, COND_EQUAL };
  int atk_level = BPP_GetValue( taw->atk_bpp, BPP_LEVEL );
  int def_level = BPP_GetValue( taw->def_bpp, BPP_LEVEL );

  GF_ASSERT( value < LEVEL_EQUAL + 1 );

  branch_act( vmh, cond[ value ], atk_level, def_level, adrs );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ������Ԃ��`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_CHOUHATSU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_chouhatsu( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IFN_CHOUHATSU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_chouhatsu( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ������Ԃ��`�F�b�N���ĕ��򏈗����C��
//------------------------------------------------------------
static  void  ai_if_chouhatsu( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int adrs  = ( int )VMGetU32( vmh );
  BOOL  value = BPP_CheckSick( taw->def_bpp, WAZASICK_TYOUHATSU );

  branch_act( vmh, cond, value, TRUE, adrs );
}

//------------------------------------------------------------
//  �U���Ώۂ��������ǂ����`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_MIKATA_ATTACK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int adrs  = ( int )VMGetU32( vmh );

  branch_act( vmh, COND_EQUAL, ( taw->atk_btl_poke_pos & 1 ), ( taw->def_btl_poke_pos & 1 ), adrs );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �^�C�v�������Ă��邩�`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_HAVE_TYPE( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  PokeType  type  = ( PokeType )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  PokeTypePair  poke_type = BPP_GetPokeType( get_bpp( taw, pos ) );

  if( ( PokeTypePair_GetType1( poke_type ) == type ) ||
      ( PokeTypePair_GetType2( poke_type ) == type ) )
  {
    taw->calc_work = HAVE_YES;
  }
  else
  {
    taw->calc_work = HAVE_NO;
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �Ƃ����������Ă��邩�`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_HAVE_TOKUSEI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side    = ( int )VMGetU32( vmh );
  int tokusei = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  int have_tokusei = get_tokusei( taw, side, pos );

  if( have_tokusei == tokusei )
  {
    taw->calc_work = HAVE_YES;
  }
  else
  {
    taw->calc_work = HAVE_NO;
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ���łɁu���炢�сv�Ńp���[�A�b�v��Ԃɂ��邩�`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_ALREADY_MORAIBI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  //@todo
  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �A�C�e���������Ă��邩�`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HAVE_ITEM( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  int item = ( int )VMGetU32( vmh );
  int adrs = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  u32 have_item = BPP_GetItem( get_bpp( taw, pos ) );

  //@todo
  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  FLDEFF�`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_FLDEFF_CHECK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  BtlFieldEffect  fldeff = ( BtlFieldEffect )VMGetU32( vmh );
  int adrs = ( int )VMGetU32( vmh );
  BOOL data = BTL_FIELD_CheckEffect( fldeff );

  branch_act( vmh, COND_EQUAL, data, TRUE, adrs );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  SIDEEFF�̃J�E���g���擾
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_SIDEEFF_COUNT( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  int flag = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  //flag�ł܂��т��A�ǂ��т��𕪊�
  taw->calc_work = 0;//@todo �擾�֐�

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �T���|�P������HP�������`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_BENCH_HPDEC( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  u8  clientID = BTL_MAIN_BtlPosToClientID( taw->wk, pos );
  const BTL_PARTY*  pty = BTL_POKECON_GetPartyDataConst( taw->pokeCon, clientID );
  //�O�q�̐�
  u8  front_count = BTL_MAIN_GetClientCoverPosCount( taw->wk, clientID );
  int i;

  for( i = front_count ; i < BTL_PARTY_GetMemberCount( pty ) ; i++ )
  {
    const BTL_POKEPARAM* bpp = get_bpp_from_party( pty, i );

    if( !BPP_IsDead( bpp ) )
    {
      int hpratio = BPP_GetHPRatio( get_bpp( taw, pos ) );
      if( branch_act( vmh, COND_UNDER, hpratio, 100, adrs ) )
      {
        break;
      }
    }
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �T���|�P������PP�������`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_BENCH_PPDEC( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side  = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  u8  clientID = BTL_MAIN_BtlPosToClientID( taw->wk, pos );
  const BTL_PARTY*  pty = BTL_POKECON_GetPartyDataConst( taw->pokeCon, clientID );
  //�O�q�̐�
  u8  front_count = BTL_MAIN_GetClientCoverPosCount( taw->wk, clientID );
  int i, j;

  for( i = front_count ; i < BTL_PARTY_GetMemberCount( pty ) ; i++ )
  {
    const BTL_POKEPARAM* bpp = get_bpp_from_party( pty, i );

    if( !BPP_IsDead( bpp ) )
    {
      for( j = 0 ; j < BPP_WAZA_GetCount( get_bpp( taw, pos ) ) ; j++ )
      {
        int ppshort = BPP_WAZA_GetPPShort( get_bpp( taw, pos ), j );
        if( branch_act( vmh, COND_NOTEQUAL, ppshort, 0, adrs ) )
        {
          break;
        }
      }
      if( j != BPP_WAZA_GetCount( get_bpp( taw, pos ) ) )
      {
        break;
      }
    }
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �����A�C�e���̂Ȃ�����З͂��擾
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_NAGETSUKERU_IRYOKU( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = 0;

  //��������������Ă��邩�`�F�b�N
  if( !BPP_CheckSick( get_bpp( taw, pos ), WAZASICK_SASIOSAE ) )
  {
    u32 item = BPP_GetItem( get_bpp( taw, pos ) );
    taw->calc_work = ITEM_GetParam( item, ITEM_PRM_NAGETUKERU_ATC, taw->heapID );
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �c��PP���擾
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_PP_REMAIN( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = BPP_WAZA_GetPP( taw->atk_bpp, taw->waza_pos );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �Ƃ��Ă����`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_TOTTEOKI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  int adrs = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  const BTL_POKEPARAM* bpp = get_bpp( taw, pos );

  //�����Ă���Z���o���؂��Ă��Ȃ����A�����Ă���Z��2�ȏ�Ȃ��ꍇ�͎��s
  if( ( BPP_WAZA_GetUsedCount( bpp ) >= BPP_WAZA_GetCount( bpp ) ) && (BPP_WAZA_GetCount( bpp ) > 1) )
  {
    VMCMD_Jump( vmh, vmh->adrs + adrs );
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �Z�̕��ރ`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_WAZA_KIND( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = get_waza_param( taw, taw->waza_no, WAZAPARAM_DAMAGE_TYPE );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ���肪�Ō�ɏo�����Z�̕��ރ`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_LAST_WAZA_KIND( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  taw->calc_work = get_waza_param( taw, BPP_GetPrevWazaID( taw->def_bpp ), WAZAPARAM_DAMAGE_TYPE );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �f�����Ŏw�肵���������ʂ��`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_AGI_RANK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = 0;  //@todo �擾�֐�

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �X���[�X�^�[�g���^�[���ڂ�
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_SLOWSTART_TURN( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_GetTurnCount( get_bpp( taw, pos ) );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �T���ɂ�������_���[�W��^���邩�ǂ����`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_BENCH_DAMAGE_MAX( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  //@todo
  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  ���Q�̋Z�������Ă��邩�`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HAVE_BATSUGUN( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  //@todo
  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �w�肵������̍Ō�ɏo�����Z�Ǝ����̋Z�Ƃ̃_���[�W���`�F�b�N
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_LAST_WAZA_DAMAGE_CHECK( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  //@todo
  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �w�肵������̃X�e�[�^�X�㏸���̒l���擾
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_STATUS_UP( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  int i;
  BppValueID  id_tbl[]={
    BPP_ATTACK_RANK,
    BPP_DEFENCE_RANK,
    BPP_SP_ATTACK_RANK,
    BPP_SP_DEFENCE_RANK,
    BPP_AGILITY_RANK,
    BPP_HIT_RATIO,
    BPP_AVOID_RATIO,
  };

  taw->calc_work = 0;

  for( i = 0 ; i < NELEMS( id_tbl ) ; i++ )
  {
    int rank = BPP_GetValue( get_bpp( taw, pos ), id_tbl[ i ] );
    if( rank > 6 )
    {
      taw->calc_work += ( rank - 6 );
    }
  }

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �w�肵������Ƃ̃X�e�[�^�X�������擾
//------------------------------------------------------------
static  VMCMD_RESULT  AI_CHECK_STATUS_DIFF( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  int flag = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  taw->calc_work = BPP_GetValue( get_bpp( taw, pos ), flag ) - BPP_GetValue( taw->atk_bpp, flag );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �w�肵������Ƃ̃X�e�[�^�X���`�F�b�N���ĕ���iHGSS���_�Ŗ��g�p�R�}���h�j
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_CHECK_STATUS_UNDER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_CHECK_STATUS_OVER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}
static  VMCMD_RESULT  AI_IF_CHECK_STATUS_EQUAL( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �З͂���ԍ������̃`�F�b�N�i�p�[�g�i�[���܂ށj
//------------------------------------------------------------
static  VMCMD_RESULT  AI_COMP_POWER_WITH_PARTNER( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  //@todo
  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �w�肵�����肪�m�����`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_HINSHI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_hinshi( vmh, taw, COND_EQUAL );

  return taw->vmcmd_result;
}

static  VMCMD_RESULT  AI_IFN_HINSHI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  ai_if_hinshi( vmh, taw, COND_NOTEQUAL );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �w�肵�����肪�m�����`�F�b�N���ĕ��򏈗����C��
//------------------------------------------------------------
static  void  ai_if_hinshi( VMHANDLE* vmh, TR_AI_WORK* taw, BranchCond cond )
{
  int side = ( int )VMGetU32( vmh );
  int adrs = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );
  BOOL  hinshi = BPP_IsDead( get_bpp( taw, pos ) );

  branch_act( vmh, cond, hinshi, TRUE, adrs );
}

//------------------------------------------------------------
//  �Z���ʂ��l�����ē������擾�i�ړ��|�P������p�j
//------------------------------------------------------------
static  VMCMD_RESULT  AI_GET_TOKUSEI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;

  //@todo
  GF_ASSERT_MSG( 0, "���쐬" );

  return taw->vmcmd_result;
}

//------------------------------------------------------------
//  �݂���蒆���`�F�b�N���ĕ���
//------------------------------------------------------------
static  VMCMD_RESULT  AI_IF_MIGAWARI( VMHANDLE* vmh, void* context_work )
{
  TR_AI_WORK* taw = (TR_AI_WORK*)context_work;
  int side = ( int )VMGetU32( vmh );
  int adrs = ( int )VMGetU32( vmh );
  BtlPokePos  pos = get_poke_pos( taw, side );

  if( BPP_MIGAWARI_IsExist( get_bpp( taw, pos ) ) )
  {
    VMCMD_Jump( vmh, vmh->adrs + adrs );
  }

  return taw->vmcmd_result;
}

//============================================================================================
/**
 *  �����ɂ���ău�����`����
 *
 * @param[in] vmh
 * @param[in] cond  �u�����`����
 * @param[in] src   ��r�ΏۂP
 * @param[in] dst   ��r�ΏۂQ
 * @param[in] adrs  ��ѐ�
 *
 * @retval  TRUE:���򂵂��@FALSE:���򂵂Ȃ�
 */
//============================================================================================
static  BOOL  branch_act( VMHANDLE* vmh, BranchCond cond, int src, int dst, int adrs )
{
  BOOL  branch = FALSE;

  switch( cond ){
  case COND_UNDER:
    if( src < dst )
    {
      branch = TRUE;
    }
    break;
  case COND_OVER:
    if( src > dst )
    {
      branch = TRUE;
    }
    break;
  case COND_EQUAL:
    if( src == dst )
    {
      branch = TRUE;
    }
    break;
  case COND_NOTEQUAL:
    if( src != dst )
    {
      branch = TRUE;
    }
    break;
  case COND_BIT:
    if( src & dst )
    {
      branch = TRUE;
    }
    break;
  case COND_NBIT:
    if( ( src & dst ) == 0 )
    {
      branch = TRUE;
    }
    break;
  case COND_OR_UNDER:
    if( src <= dst )
    {
      branch = TRUE;
    }
    break;
  case COND_OR_OVER:
    if( src >= dst )
    {
      branch = TRUE;
    }
    break;
  }
  if( branch == TRUE )
  {
    VMCMD_Jump( vmh, vmh->adrs + adrs );
  }

  return branch;
}

//============================================================================================
/**
 *  �`�F�b�N����BtlPokePos���擾
 */
//============================================================================================
static  BtlPokePos  get_poke_pos( TR_AI_WORK* taw, int side )
{
  BtlPokePos btl_poke_pos;

  switch( side ){
  case CHECK_ATTACK:
    btl_poke_pos = taw->atk_btl_poke_pos;
    break;
  case CHECK_DEFENCE:
  default:
    btl_poke_pos = taw->def_btl_poke_pos;
    break;
  case CHECK_ATTACK_FRIEND:
    btl_poke_pos = taw->atk_btl_poke_pos ^ 2;
    break;
  case CHECK_DEFENCE_FRIEND:
    btl_poke_pos = taw->def_btl_poke_pos ^ 2;
    break;
  }
  return btl_poke_pos;
}

//============================================================================================
/**
 *  �e�[�u���f�[�^��u8�^�Ŏ擾
 */
//============================================================================================
static  inline  u8  get_u8_data( int* adrs )
{
  u8 ret = *(adrs);
  ++ adrs;
  return ret;
}

//============================================================================================
/**
 *  �e�[�u���f�[�^��u32�^�Ŏ擾
 */
//============================================================================================
static  u32 get_table_data( int* adrs )
{
  u32 val;
  u8  a,b,c,d;

  a = get_u8_data( adrs );
  b = get_u8_data( adrs );
  c = get_u8_data( adrs );
  d = get_u8_data( adrs );

  val = 0;
  val += (u32)d;
  val <<= 8;
  val += (u32)c;
  val <<= 8;
  val += (u32)b;
  val <<= 8;
  val += (u32)a;

  return val;
}

//============================================================================================
/**
 *  �����ʒu���w�肵�ē������擾
 */
//============================================================================================
static  int get_tokusei( TR_AI_WORK* taw, int side, BtlPokePos pos )
{
  int have_tokusei;
  const BTL_POKEPARAM*  bpp = get_bpp( taw, pos );

  //���������󂯂Ă��邩�`�F�b�N
  if( BPP_CheckSick( bpp, WAZASICK_IEKI ) )
  {
    have_tokusei = 0;
  }
  //�h�䑤�͎����Ă���\�����l���������������
  else if( ( side == CHECK_DEFENCE ) || ( side == CHECK_DEFENCE_FRIEND ) )
  {
    //�����̔��������Ă���ꍇ�͂�����擾
    if( taw->look_tokusei[ pos ] )
    {
      have_tokusei = taw->look_tokusei[ pos ];
    }
    else
    {
      have_tokusei = BPP_GetValue( bpp, BPP_TOKUSEI ); //�f���Ɏ擾
      //�����ӂ݁A����傭�A���肶�����́A�������������Ȃ����ƂŎ@�m�ł��Ă���͂��Ȃ̂ŁA���O����
      if( ( have_tokusei != TOKUSYU_KAGEHUMI ) &&
          ( have_tokusei != TOKUSYU_ZIRYOKU ) &&
          ( have_tokusei != TOKUSYU_ARIZIGOKU ) )
      {
        //�����̔��������Ă��Ȃ��ꍇ�̓p�[�\�i���Ŏ��������������擾
        int monsno = BPP_GetMonsNo( bpp );
        int formno = BPP_GetValue( bpp, BPP_TOKUSEI );
        int tokusei1;
        int tokusei2;

        tokusei1 = POKETOOL_GetPersonalParam( monsno, formno, POKEPER_ID_speabi1 );
        tokusei2 = POKETOOL_GetPersonalParam( monsno, formno, POKEPER_ID_speabi2 );

        //������2��ގ����Ă���|�P�����́A�\����T��
        if( ( tokusei1 ) && ( tokusei2 ) )
        {
          //�ǂ���ɂ���v���Ȃ��Ȃ玝���Ă��Ȃ����Ƃɂ���
          if( ( tokusei1 != have_tokusei ) && ( tokusei2 != have_tokusei ) )
          {
            have_tokusei = 0;
          }
        }
        //1��ނ��������Ă��Ȃ��Ȃ猈�߂���
        else if( tokusei1 )
        {
          have_tokusei = tokusei1;
        }
        else
        {
          have_tokusei = tokusei2;
        }
      }
    }
  }
  else
  {
    have_tokusei = BPP_GetValue( bpp, BPP_TOKUSEI ); //�������g�̓����͒m���Ă���̂ŁA�f���Ɏ擾
  }

  return have_tokusei;
}

//============================================================================================
/**
 *  BTL_POKEPARAM���擾
 */
//============================================================================================
static  const BTL_POKEPARAM*  get_bpp( TR_AI_WORK* taw, BtlPokePos pos )
{
  return  BTL_POKECON_GetFrontPokeDataConst( taw->pokeCon, pos );
}

static  const BTL_POKEPARAM*  get_bpp_from_party( const BTL_PARTY* pty, u8 idx )
{
  return  BTL_PARTY_GetMemberDataConst( pty, idx );
}


//============================================================================================
/**
 *  1�O�ɌJ��o�����Z�����[�N�Ɋi�[���Ă���
 */
//============================================================================================
static  void  waza_no_stock( TR_AI_WORK* taw )
{
  int i;
  const BTL_POKEPARAM*  bpp = get_bpp( taw, taw->def_btl_poke_pos );

  for( i = 0 ; i < PTL_WAZA_MAX ; i++ )
  {
    if( taw->use_waza[ taw->def_btl_poke_pos ][ i ] == BPP_GetPrevWazaID( bpp ) )
    {
      break;
    }
    if( taw->use_waza[ taw->def_btl_poke_pos ][ i ] == 0 )
    {
      taw->use_waza[ taw->def_btl_poke_pos ][ i ] = BPP_GetPrevWazaID( bpp );
      break;
    }
  }
}

//============================================================================================
/**
 *  �Z�̃p�����[�^��ǂݏo��
 */
//============================================================================================
static  int   get_waza_param( TR_AI_WORK* taw, WazaID waza_no, WazaDataParam param )
{
  int i;

  for( i = 0 ; i < TR_AI_WAZATBL_MAX ; i++ )
  {
    if( taw->wazaID[ i ] == 0 )
    {
      taw->wazaID[ i ] = waza_no;
      GFL_ARC_LoadDataByHandle( taw->waza_handle, waza_no, taw->wd[ i ] );
      break;
    }
    if( taw->wazaID[ i ] == waza_no )
    {
      if( i != 0 )
      {
        WAZA_DATA*  wd;
        WazaID      waza_no_temp;

        wd = taw->wd[ i - 1 ];
        waza_no_temp = taw->wazaID[ i - 1 ];
        taw->wd[ i - 1 ] =  taw->wd[ i ];
        taw->wazaID[ i - 1 ] = taw->wazaID[ i ];
        taw->wd[ i ] =  wd;
        taw->wazaID[ i ] = waza_no_temp;
      }
      break;
    }
  }
  if( i == TR_AI_WAZATBL_MAX )
  {
    i--;
    taw->wazaID[ i ] = waza_no;
    GFL_ARC_LoadDataByHandle( taw->waza_handle, waza_no, taw->wd[ i ] );
  }
  return WAZADATA_PTR_GetParam( taw->wd[ i ], param );
}

