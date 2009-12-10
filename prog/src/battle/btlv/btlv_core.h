//=============================================================================================
/**
 * @file  btlv_core.h
 * @brief �|�P����WB �o�g���`��S�����C�����W���[��
 * @author  taya
 *
 * @date  2008.10.02  �쐬
 */
//=============================================================================================
#ifndef __BTLV_CORE_H__
#define __BTLV_CORE_H__


#include "btlv_core_def.h"

//----------------------------------------------------------------------
// �Q�ƃw�b�_ include
//----------------------------------------------------------------------
#include "../app/b_bag.h"
#include "../app/b_plist.h"

#include "../btl_string.h"
#include "../btl_main_def.h"
#include "../btl_client.h"
#include "../btl_action.h"
#include "../btl_calc.h"
#include "../btl_pokeselect.h"


#include "btlv_effect.h"
#include "btlv_common.h"

//----------------------------------------------------------------------
/**
 *  �`��R�}���h
 */
//----------------------------------------------------------------------
typedef enum {

  BTLV_CMD_SETUP,
  BTLV_CMD_SELECT_ACTION,
  BTLV_CMD_MSG,

  BTLV_CMD_MAX,
  BTLV_CMD_NULL = BTLV_CMD_MAX,

}BtlvCmd;

//----------------------------------------------------------------------
/**
 *  �߂�l�`�F�b�N
 */
//----------------------------------------------------------------------
typedef enum {

  BTLV_RESULT_NONE = 0,
  BTLV_RESULT_DONE,
  BTLV_RESULT_CANCEL,

}BtlvResult;


//--------------------------------------------------------------
/**
 *  ���b�Z�[�W�E�F�C�g�w��
 */
//--------------------------------------------------------------
enum {

  BTLV_MSGWAIT_NONE = 0,
  BTLV_MSGWAIT_STD = 80,

};

//----------------------------------------------------------------------
/**
 *  �����o�̓p�����[�^
 */
//----------------------------------------------------------------------
typedef struct {
  u16   strID;          ///< ��{������ID
  u8    wait;           ///< ���s�E�\���I�����E�F�C�g
  u8    strType : 4;    ///< BtlStrType
  u8    argCnt  : 4;    ///< �����̐�
  int   args[BTL_STR_ARG_MAX];  ///< ����
}BTLV_STRPARAM;

static inline void BTLV_STRPARAM_Setup( BTLV_STRPARAM* sp, BtlStrType strType, u16 strID )
{
  int i;
  for(i=0; i<NELEMS(sp->args); ++i){
    sp->args[i] = 0;
  }
  sp->argCnt = 0;
  sp->strID = strID;
  sp->strType = strType;
  sp->wait = BTLV_MSGWAIT_STD;
}
static inline void BTLV_STRPARAM_AddArg( BTLV_STRPARAM* sp, int arg )
{
  if( sp->argCnt < NELEMS(sp->args) ){
    sp->args[ sp->argCnt++ ] = arg;
  }
}
static inline void BTLV_STRPARAM_SetWait( BTLV_STRPARAM* sp, u8 wait )
{
  sp->wait = wait;
}



//=============================================================================================
/**
 * �`�惁�C�����W���[���̐���
 *
 * @param   mainModule    �V�X�e�����C�����W���[���̃n���h��
 * @param   heapID      ������q�[�vID
 *
 * @retval  BTLV_CORE*    �������ꂽ�`�惁�C�����W���[���̃n���h��
 */
//=============================================================================================
extern BTLV_CORE*  BTLV_Create( BTL_MAIN_MODULE* mainModule, const BTL_CLIENT* client, const BTL_POKE_CONTAINER* pokeCon, HEAPID heapID );

//=============================================================================================
/**
 * �`�惁�C�����W���[���̔j��
 *
 * @param   core    �`�惁�C�����W���[���̃n���h��
 */
//=============================================================================================
extern void BTLV_Delete( BTLV_CORE* core );

//=============================================================================================
/**
 * �`�惁�C�����[�v
 *
 * @param   core    �`�惁�C�����W���[���̃n���h��
 */
//=============================================================================================
extern void BTLV_CORE_Main( BTLV_CORE* core );

//=============================================================================================
/**
 * BTLV_StartCommand �Ŏ󂯕t�����`��R�}���h�̏I����҂�
 *
 * @param   wk      �`�惁�C�����W���[���̃n���h��
 *
 * @retval  BOOL    �I�����Ă�����TRUE
 */
//=============================================================================================
extern void BTLV_StartCommand( BTLV_CORE* btlv, BtlvCmd cmd );

//=============================================================================================
/**
 * BTLV_StartCommand �Ŏ󂯕t�����`��R�}���h�̏I����҂�
 *
 * @param   wk      �`�惁�C�����W���[���̃n���h��
 *
 * @retval  BOOL    �I�����Ă�����TRUE
 */
//=============================================================================================
extern BOOL BTLV_WaitCommand( BTLV_CORE* btlv );



extern void BTLV_UI_SelectAction_Start( BTLV_CORE* core, const BTL_POKEPARAM* bpp, BOOL fReturnable, BTL_ACTION_PARAM* dest );
extern BtlAction  BTLV_UI_SelectAction_Wait( BTLV_CORE* core );

extern void BTLV_UI_SelectWaza_Start( BTLV_CORE* core, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest );
extern BOOL BTLV_UI_SelectWaza_Wait( BTLV_CORE* core );

extern void BTLV_UI_SelectTarget_Start( BTLV_CORE* core, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest );
extern BtlvResult BTLV_UI_SelectTarget_Wait( BTLV_CORE* core );

extern void BTLV_UI_Restart( BTLV_CORE* wk );


extern void BTLV_StartMemberChangeAct( BTLV_CORE* wk, BtlPokePos pos, u8 clientID, u8 memberIdx );
extern BOOL BTLV_WaitMemberChangeAct( BTLV_CORE* wk );


//=============================================================================================
/**
 * �|�P�����I���������J�n
 *
 * @param   core      [in]  �`�惁�C�����W���[���n���h��
 * @param   param     [in]  �I�������p�����[�^�|�C���^
 * @param   fCantEsc  [in]
 * @param   result    [out] �I�����ʊi�[�\���̃|�C���^
 */
//=============================================================================================
extern void BTLV_StartPokeSelect( BTLV_CORE* core, const BTL_POKESELECT_PARAM* param, BOOL fCantEsc, BTL_POKESELECT_RESULT* result );

//=============================================================================================
/**
 * �|�P�����I�������̏I���҂�
 *
 * @param   core      [in]  �`�惁�C�����W���[���n���h��
 *
 * @retval  BOOL      �I��������TRUE
 */
//=============================================================================================
extern BOOL BTLV_WaitPokeSelect( BTLV_CORE* core );



//=============================================================================================
/**
 *
 *
 * @param   wk
 * @param   strID
 * @param   clientID
 *
 * @retval  extern void
 */
//=============================================================================================
extern void BTLV_StartMsg( BTLV_CORE* wk, const BTLV_STRPARAM* param );
extern void BTLV_StartMsgWaza( BTLV_CORE* wk, u8 pokeID, u16 waza );

extern void BTLV_StartMsgStd( BTLV_CORE* wk, u16 strID, const int* args );
extern void BTLV_StartMsgSet( BTLV_CORE* wk, u16 strID, const int* args );


extern BOOL BTLV_WaitMsg( BTLV_CORE* wk );
extern BOOL BTLV_IsJustDoneMsg( BTLV_CORE* wk );


extern void BTLV_ACT_WazaEffect_Start( BTLV_CORE* wk, BtlPokePos atPokePos, BtlPokePos defPokePos, WazaID waza,
  BtlvWazaEffect_TurnType turnType, u8 continueCount );

extern BOOL BTLV_ACT_WazaEffect_Wait( BTLV_CORE* wk );

extern void BTLV_ACT_DamageEffectSingle_Start( BTLV_CORE* wk, WazaID waza, BtlPokePos defPokePos, u16 damage, BtlTypeAff aff );
extern BOOL BTLV_ACT_DamageEffectSingle_Wait( BTLV_CORE* wk );


//=============================================================================================
/**
 * �����̈�ă_���[�W���� �J�n
 *
 * @param   wk
 * @param   pokeCnt   �|�P������
 * @param   affAbout  ����ID
 * @param   pokeID    �ΏۂƂȂ�|�P����ID�z��
 */
//=============================================================================================
extern void BTLV_ACT_DamageEffectPlural_Start( BTLV_CORE* wk, u32 pokeCnt, BtlTypeAffAbout affAbout, const u8* pokeID, WazaID waza );
extern BOOL BTLV_ACT_DamageEffectPlural_Wait( BTLV_CORE* wk );


extern void BTLV_StartDeadAct( BTLV_CORE* wk, BtlPokePos pokePos );
extern BOOL BTLV_WaitDeadAct( BTLV_CORE* wk );
extern void BTLV_ACT_MemberOut_Start( BTLV_CORE* wk, BtlvMcssPos vpos );
extern BOOL BTLV_ACT_MemberOut_Wait( BTLV_CORE* wk );
extern void BTLV_ACT_SimpleHPEffect_Start( BTLV_CORE* wk, BtlPokePos pokePos );
extern BOOL BTLV_ACT_SimpleHPEffect_Wait( BTLV_CORE* wk );


extern void BTLV_TokWin_DispStart( BTLV_CORE* wk, BtlPokePos pos, BOOL fFlash );
extern BOOL BTLV_TokWin_DispWait( BTLV_CORE* wk, BtlPokePos pos );
extern void BTLV_QuitTokWin( BTLV_CORE* wk, BtlPokePos pos );
extern BOOL BTLV_QuitTokWinWait( BTLV_CORE* wk, BtlPokePos pos );
extern void BTLV_TokWin_Renew_Start( BTLV_CORE* wk, BtlPokePos pos );
extern BOOL BTLV_TokWin_Renew_Wait( BTLV_CORE* wk, BtlPokePos pos );

extern void BTLV_StartRankDownEffect( BTLV_CORE* wk, u8 pokeID, BppValueID statusType );
extern void BTLV_StartCommWait( BTLV_CORE* wk );
extern BOOL BTLV_WaitCommWait( BTLV_CORE* wk );
extern void BTLV_ResetCommWaitInfo( BTLV_CORE* wk );

extern void BTLV_KinomiAct_Start( BTLV_CORE* wk, BtlPokePos pos );
extern BOOL BTLV_KinomiAct_Wait( BTLV_CORE* wk, BtlPokePos pos );
extern void BTLV_FakeDisable_Start( BTLV_CORE* wk, BtlPokePos pos );
extern BOOL BTLV_FakeDisable_Wait( BTLV_CORE* wk );

extern void BTLV_ACT_MoveMember_Start( BTLV_CORE* wk, u8 clientID, u8 vpos1, u8 vpos2, u8 posIdx1, u8 posIdx2 );
extern BOOL BTLV_ACT_MoveMember_Wait( BTLV_CORE* wk );


extern void BTLV_ITEMSELECT_Start( BTLV_CORE* wk, u8 bagMode, u8 energy, u8 reserved_energy );
extern BOOL BTLV_ITEMSELECT_Wait( BTLV_CORE* wk );
extern u16 BTLV_ITEMSELECT_GetItemID( BTLV_CORE* wk );
extern u8  BTLV_ITEMSELECT_GetCost( BTLV_CORE* wk );
extern u8  BTLV_ITEMSELECT_GetTargetIdx( BTLV_CORE* wk );
extern void BTLV_ITEMSELECT_ReflectUsedItem( BTLV_CORE* wk );


extern void BTLV_UI_SelectRotation_Start( BTLV_CORE* wk, BtlRotateDir prevDir );
extern BOOL BTLV_UI_SelectRotation_Wait( BTLV_CORE* wk, BtlRotateDir* result );
extern void BTLV_RotationMember_Start( BTLV_CORE* wk, u8 clientID, BtlRotateDir dir );
extern BOOL BTLV_RotationMember_Wait( BTLV_CORE* wk );

extern void BTLV_YESNO_Start( BTLV_CORE* wk, BTLV_STRPARAM* yes_msg, BTLV_STRPARAM* no_msg );
extern BOOL BTLV_YESNO_Wait( BTLV_CORE* wk, BtlYesNo* result );

extern void BTLV_WAZAWASURE_Start( BTLV_CORE* wk, u8 pos, WazaID waza );
extern BOOL BTLV_WAZAWASURE_Wait( BTLV_CORE* wk, u8* result );

//-----------------------------------------------------------
// �e�퉺��������Ăяo�����֐�

extern u8 BTLV_CORE_GetPlayerClientID( const BTLV_CORE* core );

#endif
