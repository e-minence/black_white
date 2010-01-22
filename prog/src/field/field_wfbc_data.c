//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_wfbc_data.c
 *	@brief  White Forest  Black City ��{�f�[�^ �풓
 *	@author	tomoya takahashi
 *	@date		2009.11.10
 *
 *	���W���[�����FFIELD_WFBC_CORE
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "field/field_wfbc_data.h"
#include "field/zonedata.h"

#include "system/net_err.h"


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	�ʐM�o�b�t�@�}�X�N
//=====================================
typedef enum {
  FIELD_WFBC_COMM_BUFF_MSK_RECV_REQ_0,
  FIELD_WFBC_COMM_BUFF_MSK_RECV_REQ_END = FIELD_WFBC_COMM_BUFF_MSK_RECV_REQ_0 + FIELD_COMM_MEMBER_MAX,
  FIELD_WFBC_COMM_BUFF_MSK_RECV_ANS = FIELD_WFBC_COMM_BUFF_MSK_RECV_REQ_END,
  FIELD_WFBC_COMM_BUFF_MSK_SEND_REQ,
  FIELD_WFBC_COMM_BUFF_MSK_SEND_ANS_0,
  FIELD_WFBC_COMM_BUFF_MSK_SEND_ANS_END = FIELD_WFBC_COMM_BUFF_MSK_SEND_ANS_0 + FIELD_COMM_MEMBER_MAX,
    
} FIELD_WFBC_COMM_BUFF_MSK;
#define FIELD_WFBC_COMM_BUFF_MSK_CHECK(val, x)   ((val) & (1<<(x)))
#define FIELD_WFBC_COMM_BUFF_MSK_ON(val, x)   ((val) | (1<<(x)))
#define FIELD_WFBC_COMM_BUFF_MSK_OFF(val, x)   ((val) & ~(1<<(x)))

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	WFBC_COMM_DATA
//=====================================
static BOOL WFBC_COMM_DATA_IsError( const WFBC_COMM_DATA* cp_wk );

static void WFBC_COMM_DATA_MainOya( WFBC_COMM_DATA* p_wk, FIELD_WFBC_CORE* p_mywfbc );

static BOOL WFBC_COMM_DATA_Oya_AnserThere( WFBC_COMM_DATA* p_wk, FIELD_WFBC_CORE* p_mywfbc, const FIELD_WFBC_COMM_NPC_REQ* cp_req, FIELD_WFBC_COMM_NPC_ANS* p_ans );
static BOOL WFBC_COMM_DATA_Oya_AnserWishTake( WFBC_COMM_DATA* p_wk, FIELD_WFBC_CORE* p_mywfbc, const FIELD_WFBC_COMM_NPC_REQ* cp_req, FIELD_WFBC_COMM_NPC_ANS* p_ans );
static BOOL WFBC_COMM_DATA_Oya_AnserTake( WFBC_COMM_DATA* p_wk, FIELD_WFBC_CORE* p_mywfbc, const FIELD_WFBC_COMM_NPC_REQ* cp_req, FIELD_WFBC_COMM_NPC_ANS* p_ans );



static void WFBC_COMM_DATA_SetRecvCommAnsData( WFBC_COMM_DATA* p_wk, const FIELD_WFBC_COMM_NPC_ANS* cp_ans );
static void WFBC_COMM_DATA_SetRecvCommReqData( WFBC_COMM_DATA* p_wk, u16 netID, const FIELD_WFBC_COMM_NPC_REQ* cp_req );
static BOOL WFBC_COMM_DATA_GetRecvCommAnsData( const WFBC_COMM_DATA* cp_wk, FIELD_WFBC_COMM_NPC_ANS* p_ans );
static BOOL WFBC_COMM_DATA_GetRecvCommReqData( const WFBC_COMM_DATA* cp_wk, u16 netID, FIELD_WFBC_COMM_NPC_REQ* p_req );
static void WFBC_COMM_DATA_SetSendCommAnsData( WFBC_COMM_DATA* p_wk, u16 netID, u16 npc_id, FIELD_WFBC_COMM_NPC_ANS_TYPE ans_type );
static void WFBC_COMM_DATA_SetSendCommReqData( WFBC_COMM_DATA* p_wk, u16 npc_id, FIELD_WFBC_COMM_NPC_REQ_TYPE req_type );
static BOOL WFBC_COMM_DATA_GetSendCommAnsData( const WFBC_COMM_DATA* cp_wk, u16 netID, FIELD_WFBC_COMM_NPC_ANS* p_ans );
static BOOL WFBC_COMM_DATA_GetSendCommReqData( const WFBC_COMM_DATA* cp_wk, FIELD_WFBC_COMM_NPC_REQ* p_req );

static void WFBC_COMM_DATA_ClearRecvCommAnsData( WFBC_COMM_DATA* p_wk );
static void WFBC_COMM_DATA_ClearRecvCommReqData( WFBC_COMM_DATA* p_wk, u16 netID );
static void WFBC_COMM_DATA_ClearSendCommAnsData( WFBC_COMM_DATA* p_wk, u16 netID );
static void WFBC_COMM_DATA_ClearSendCommReqData( WFBC_COMM_DATA* p_wk );




//----------------------------------------------------------------------------
/**
 *	@brief  ZONEDATA��WFBC�̐ݒ���s��
 *
 *	@param	cp_wk   ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_SetUpZoneData( const FIELD_WFBC_CORE* cp_wk )
{
  if( cp_wk->type == FIELD_WFBC_CORE_TYPE_WHITE_FOREST )
  {
    ZONEDATA_SetChangeZoneID( ZONEDATA_CHANGE_BC_WF_ID, TRUE );
  }
  if( cp_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    ZONEDATA_SetChangeZoneID( ZONEDATA_CHANGE_BC_WF_ID, FALSE );
  }
}


//-------------------------------------
///	FIELD_WFBC_CORE�p�֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  WFBC���[�N�̃N���A
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_Clear( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  p_wk->data_in = FALSE;
  p_wk->type    = FIELD_WFBC_CORE_TYPE_MAX;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->back_people[i] );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �l��T��  NPCID�̐l
 *
 *	@param	p_wk      ���[�N
 *	@param	npc_id    NPC�@ID
 *	@param  mapmode   �}�b�v���[�h
 *
 *	mapmode
 *	  �}�b�v���[�h
 *      MAPMODE_NORMAL,     ///<�ʏ���
 *      MAPMODE_INTRUDE,    ///<�N����
 *      MAPMODE_MAX,        ///<�������`�F�b�N
 *
 *	@retval �l���[�N
 *	@retval NULL    ���Ȃ�
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_CORE_PEOPLE* FIELD_WFBC_CORE_GetNpcIDPeople( FIELD_WFBC_CORE* p_wk, u32 npc_id, MAPMODE mapmode )
{
  int i;

  GF_ASSERT( p_wk );
  GF_ASSERT( npc_id < FIELD_WFBC_NPCID_MAX );

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( (mapmode == MAPMODE_NORMAL) || (mapmode == MAPMODE_MAX) )
    {
      if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->people[i] ) )
      {
        if( p_wk->people[i].npc_id == npc_id )
        {
          return &p_wk->people[i];
        }
      }
    }

    if( (mapmode == MAPMODE_INTRUDE) || (mapmode == MAPMODE_MAX) )
    {
      if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->back_people[i] ) )
      {
        if( p_wk->back_people[i].npc_id == npc_id )
        {
          return &p_wk->back_people[i];
        }
      }
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ����̂��`�F�b�N
 *
 *	@param	p_wk      ���[�N
 *	@param	npc_id    NPC�@ID
 *	@param  mapmode   �}�b�v���[�h
 *
 *	mapmode
 *	  �}�b�v���[�h
 *      MAPMODE_NORMAL,     ///<�ʏ���
 *      MAPMODE_INTRUDE,    ///<�N����
 *      MAPMODE_MAX,        ///<�������`�F�b�N
 *
 *	@retval TRUE  ����
 *	@retval FALSE ���Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_IsOnNpcIDPeople( const FIELD_WFBC_CORE* cp_wk, u32 npc_id, MAPMODE mapmode )
{
  // �����Ń��[�N������������킯�ł͂Ȃ��̂ŁA����
  if( FIELD_WFBC_CORE_GetNpcIDPeople( (FIELD_WFBC_CORE*)cp_wk, npc_id, mapmode ) == NULL )
  {
    return FALSE;
  }
  return TRUE;
}

//-------------------------------------
///	FIELD_WFBC_CORE_PEOPLE�p�֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �l�����[�N�̃N���A
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_Clear( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  GF_ASSERT( p_wk );

  p_wk->data_in = FALSE;
  p_wk->npc_id  = 0;
  p_wk->mood    = 0;
  p_wk->one_day_msk  = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���̐������`�F�b�N
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �s��
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_PEOPLE_IsConfomity( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^����  �s���f�[�^�̏ꍇ�́A����ȏ��ɏ��������܂��B
 *
 *	@param	p_wk  �l�����[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_Management( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  GF_ASSERT( p_wk );

  // npc_id���͈͓��H

  // �@���l���ő�l�ȏ�H

  // �e�̖��O�ɏI�[�R�[�h������H

}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�̗L��   �s���f�[�^�̏ꍇ�AFALSE��Ԃ��܂��B
 *
 *	@param	cp_wk   �l�����[�N
 *
 *	@retval TRUE    �f�[�^����
 *	@retval FALSE   �f�[�^�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_PEOPLE_IsInData( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );
  
  if( !cp_wk->data_in )
  {
    return FALSE;
  }
  if( FIELD_WFBC_CORE_PEOPLE_IsConfomity( cp_wk ) == FALSE )
  {
    return FALSE;
  }

  return TRUE;
}





//----------------------------------------------------------------------------
/**
 *	@brief  �A�C�e���z�u��񂷂ׂĂ��肠�@
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
void WFBC_CORE_ITEM_ClaerAll( FIELD_WFBC_CORE_ITEM* p_wk )
{
  int i;
  
  // �A�C�e���Ȃ�
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    p_wk->scr_item[i] = FIELD_WFBC_ITEM_NONE;
  }
}




//----------------------------------------------------------------------------
/**
 *	@brief  ���[�N�̃N���A
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_EVENT_Clear( FIELD_WFBC_EVENT* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(FIELD_WFBC_EVENT) );
  p_wk->bc_npc_win_target = FIELD_WFBC_EVENT_NPC_WIN_TARGET_INIT;
}





//-----------------------------------------------------------------------------
/**
 *					�ʐM���
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �ʐM���̃N���A
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_COMM_DATA_Init( WFBC_COMM_DATA* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(WFBC_COMM_DATA) );

  p_wk->netID = GFL_NET_SystemGetCurrentID();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ʐM���̔j��
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_COMM_DATA_Exit( WFBC_COMM_DATA* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(WFBC_COMM_DATA) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ʐM���̃��C������
 *
 *	@param	p_wk      ���[�N
 *	@param	p_wfbc    WFBC���
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_COMM_DATA_Oya_Main( WFBC_COMM_DATA* p_wk, FIELD_WFBC_CORE* p_mywfbc )
{
  // �G���[���͋���
  if( WFBC_COMM_DATA_IsError( p_wk ) ){
    return ;
  }
  
  // �e�̏���
  if( GFL_NET_IsParentMachine() )
  {
    WFBC_COMM_DATA_MainOya( p_wk, p_mywfbc );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �q�@���A�e�̃Z�[�u�f�[�^����q����NPC�̐l���ڂ�
 *
 *	@param	p_wk        ���[�N
 *	@param	p_mywfbc    ������WFBC
 *	@param	p_oyawfbc   �e��WFBC
 *	@param  npc_id      NPCID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_COMM_DATA_Ko_ChangeNpc( WFBC_COMM_DATA* p_wk, FIELD_WFBC_CORE* p_mywfbc, FIELD_WFBC_CORE* p_oyawfbc, const MYSTATUS* cp_mystatus, u16 npc_id )
{
  FIELD_WFBC_CORE_PEOPLE* p_people;

  p_people = FIELD_WFBC_CORE_GetNpcIDPeople( p_oyawfbc, npc_id, MAPMODE_INTRUDE );

  // ���Ȃ��̂͂�������
  GF_ASSERT( p_people );

  // �o�^
  FIELD_WFBC_CORE_AddPeople( p_mywfbc, cp_mystatus, p_people );

  // �e�̏�񂩂�͔j��
  FIELD_WFBC_CORE_PEOPLE_Clear( p_people );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ��M�������̐ݒ�  �i�q���̏����j
 *
 *	@param	p_wk        ���[�N
 *	@param	cp_ans      ��M�������
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_COMM_DATA_SetRecvCommAnsData( WFBC_COMM_DATA* p_wk, const FIELD_WFBC_COMM_NPC_ANS* cp_ans )
{
  // �G���[���͋���
  if( WFBC_COMM_DATA_IsError( p_wk ) ){
    return ;
  }

  WFBC_COMM_DATA_SetRecvCommAnsData( p_wk, cp_ans );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��M���N�G�X�g���̐ݒ�  �i�e���̏����j
 *
 *	@param	p_wk        ���[�N
 *	@param	netID       ���M�҃l�b�gID
 *	@param	cp_req      ��M���N�G�X�g���
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_COMM_DATA_SetRecvCommReqData( WFBC_COMM_DATA* p_wk, u16 netID, const FIELD_WFBC_COMM_NPC_REQ* cp_req )
{
  // �G���[���͋���
  if( WFBC_COMM_DATA_IsError( p_wk ) ){
    return ;
  }

  WFBC_COMM_DATA_SetRecvCommReqData( p_wk, netID, cp_req );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M�������̎擾    �i�e���̏����j
 *
 *	@param	cp_wk     ���[�N
 *	@param	netID     ���M��l�b�gID
 *	@param	p_ans     ���M�������i�[��
 *
 *	@retval TRUE    ��񂪂���
 *	@retval FLASE   ���Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_COMM_DATA_GetSendCommAnsData( const WFBC_COMM_DATA* cp_wk, u16 netID, FIELD_WFBC_COMM_NPC_ANS* p_ans )
{
  return WFBC_COMM_DATA_GetSendCommAnsData( cp_wk, netID, p_ans );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M���N�G�X�g���̎擾  �i�q���̏����j
 *
 *	@param	cp_wk   ���[�N
 *	@param	p_req   ���M���N�G�X�g���̊i�[��
 *
 *	@retval TRUE  ��񂪂���
 *	@retval FALSE ���Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_COMM_DATA_GetSendCommReqData( const WFBC_COMM_DATA* cp_wk, FIELD_WFBC_COMM_NPC_REQ* p_req )
{
  return WFBC_COMM_DATA_GetSendCommReqData( cp_wk, p_req );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M�������o�b�t�@�̃N���A  �i���M�ł�����N���A���Ă��������j
 *
 *	@param	p_wk    ���[�N
 *	@param	netID   �l�b�gID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_COMM_DATA_ClearSendCommAnsData( WFBC_COMM_DATA* p_wk, u16 netID )
{
  WFBC_COMM_DATA_ClearSendCommAnsData( p_wk, netID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M���N�G�X�g���o�b�t�@�̃N���A  �i���M�ł�����N���A���Ă��������j
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_COMM_DATA_ClearSendCommReqData( WFBC_COMM_DATA* p_wk )
{
  WFBC_COMM_DATA_ClearSendCommReqData( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M���N�G�X�g���̐ݒ�
 *
 *	@param	p_wk      ���[�N
 *	@param	npc_id    �Ώۂ�NPCID
 *	@param	req_type  ���N�G�X�g�^�C�v
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_COMM_DATA_SetReqData( WFBC_COMM_DATA* p_wk, u16 npc_id, FIELD_WFBC_COMM_NPC_REQ_TYPE req_type )
{
  
  if( WFBC_COMM_DATA_IsError(p_wk) == FALSE )
  {
    WFBC_COMM_DATA_SetSendCommReqData( p_wk, npc_id, req_type );
  }
  else
  {
    static const FIELD_WFBC_COMM_NPC_ANS_TYPE sc_REQ_TO_ANS[ FIELD_WFBC_COMM_NPC_REQ_TYPE_NUM ] = 
    {
      FIELD_WFBC_COMM_NPC_ANS_OFF,
      FIELD_WFBC_COMM_NPC_ANS_TAKE_NG,
      FIELD_WFBC_COMM_NPC_ANS_TYPE_NUM,
    };
    FIELD_WFBC_COMM_NPC_ANS ans;
    // �G���[���̏���
    // req_type�ɑ΂���NG����M�������Ƃɂ���
    if( sc_REQ_TO_ANS[ req_type ] != FIELD_WFBC_COMM_NPC_ANS_TYPE_NUM )
    {
      ans.net_id  = p_wk->netID;
      ans.npc_id  = npc_id;
      ans.ans_type= sc_REQ_TO_ANS[ req_type ];
      WFBC_COMM_DATA_SetRecvCommAnsData( p_wk, &ans );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������҂�
 *
 *	@param	cp_wk   ���[�N
 *	@param	npc_id  ���N�G�X�g���o����NPCID
 *
 *	@retval TRUE  �����������I
 *	@retval FLASE ���������Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_COMM_DATA_WaitAnsData( const WFBC_COMM_DATA* cp_wk, u16 npc_id )
{
  if(FIELD_WFBC_COMM_BUFF_MSK_CHECK( cp_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_RECV_ANS ))
  {
    // ���ꂪ�����Ƃ������Ƃ́A�����i�s��Ԃɂ��ꂪ����܂��B
    GF_ASSERT( npc_id == cp_wk->recv_ans.npc_id );
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������e���擾
 *
 *	@param	cp_wk   ���[�N
 *
 *	@return �������e
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_COMM_NPC_ANS_TYPE FIELD_WFBC_GetAnsData( const WFBC_COMM_DATA* cp_wk )
{
  GF_ASSERT( FIELD_WFBC_COMM_BUFF_MSK_CHECK( cp_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_RECV_ANS ) );
  return cp_wk->recv_ans.ans_type;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���N�G�X�g���o���O�ɌĂ�ł�������
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_COMM_DATA_ClearReqAnsData( WFBC_COMM_DATA* p_wk )
{
  WFBC_COMM_DATA_ClearSendCommReqData( p_wk );
  WFBC_COMM_DATA_ClearRecvCommAnsData( p_wk );
}










//-----------------------------------------------------------------------------
/**
 *    private�֐�
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *    �ʐM�p�f�[�^
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  �G���[��ԃ`�F�b�N
 *
 *	@retval TRUE    �G���[
 *	@retval FALSE   �ʏ�
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_COMM_DATA_IsError( const WFBC_COMM_DATA* cp_wk )
{
  
  
  if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE )
  {
    return TRUE;
  }

  if( cp_wk->netID == GFL_NET_NO_PARENTMACHINE )
  {
    return TRUE;
  }

  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �ʐM���̃��C�������@�e
 *
 *	@param	p_wk      ���[�N
 *	@param	p_wfbc    WFBC���
 */
//-----------------------------------------------------------------------------
static void WFBC_COMM_DATA_MainOya( WFBC_COMM_DATA* p_wk, FIELD_WFBC_CORE* p_mywfbc )
{
  int i;
  FIELD_WFBC_COMM_NPC_ANS ans;
  FIELD_WFBC_COMM_NPC_REQ req;
  BOOL result;
  BOOL ans_result;
  BOOL npc_in;
  static BOOL (*pAnser[FIELD_WFBC_COMM_NPC_REQ_TYPE_NUM])( WFBC_COMM_DATA* p_wk, FIELD_WFBC_CORE* p_mywfbc, const FIELD_WFBC_COMM_NPC_REQ* cp_req, FIELD_WFBC_COMM_NPC_ANS* p_ans ) = 
  {
    WFBC_COMM_DATA_Oya_AnserThere,
    WFBC_COMM_DATA_Oya_AnserWishTake,
    WFBC_COMM_DATA_Oya_AnserTake,
  };
  
  // �q����̃��N�G�X�g�ɓ�����B
  for( i=0; i<FIELD_COMM_MEMBER_MAX; i++ )
  {
    result = WFBC_COMM_DATA_GetRecvCommReqData( p_wk, i, &req );

    if( result )
    {
      ans_result = pAnser[ req.req_type ]( p_wk, p_mywfbc, &req, &ans );
    }

    WFBC_COMM_DATA_ClearRecvCommReqData( p_wk, i );
    if( ans_result )
    {
      WFBC_COMM_DATA_SetSendCommAnsData( p_wk, i, ans.npc_id, ans.ans_type );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ����̂��̉�������
 *
 *	@param	p_wk        ���[�N
 *	@param	p_mywfbc    ������WFBC���
 *	@param	cp_req      ���N�G�X�g���
 *	@param	p_ans       �����i�[��
 *
 *	@retval TRUE    ����������
 *	@retval FALSE   �������Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_COMM_DATA_Oya_AnserThere( WFBC_COMM_DATA* p_wk, FIELD_WFBC_CORE* p_mywfbc, const FIELD_WFBC_COMM_NPC_REQ* cp_req, FIELD_WFBC_COMM_NPC_ANS* p_ans )
{
  
  p_ans->net_id = cp_req->net_id;
  p_ans->npc_id = cp_req->npc_id;

  if( FIELD_WFBC_CORE_GetNpcIDPeople( p_mywfbc, cp_req->npc_id, MAPMODE_INTRUDE ) )
  {
    // ����
    p_ans->ans_type = FIELD_WFBC_COMM_NPC_ANS_ON;
  }
  else
  {
    // ���Ȃ�
    p_ans->ans_type = FIELD_WFBC_COMM_NPC_ANS_OFF;
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A��čs���Ă������H
 *
 *	@param	p_wk        ���[�N
 *	@param	p_mywfbc    ������WFBC���
 *	@param	cp_req      ���N�G�X�g���
 *	@param	p_ans       �����i�[��
 *
 *	@retval TRUE    ����������
 *	@retval FALSE   �������Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_COMM_DATA_Oya_AnserWishTake( WFBC_COMM_DATA* p_wk, FIELD_WFBC_CORE* p_mywfbc, const FIELD_WFBC_COMM_NPC_REQ* cp_req, FIELD_WFBC_COMM_NPC_ANS* p_ans )
{
  p_ans->net_id = cp_req->net_id;
  p_ans->npc_id = cp_req->npc_id;

  if( FIELD_WFBC_CORE_GetNpcIDPeople( p_mywfbc, cp_req->npc_id, MAPMODE_INTRUDE ) )
  {
    // ��Ă����Ă�������
    p_ans->ans_type = FIELD_WFBC_COMM_NPC_ANS_TAKE_OK;
  }
  else
  {
    // ��Ă����܂���
    p_ans->ans_type = FIELD_WFBC_COMM_NPC_ANS_TAKE_NG;
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �l��A��čs����Ă��܂��������B
 *
 *	@param	p_wk        ���[�N
 *	@param	p_mywfbc    ������WFBC���
 *	@param	cp_req      ���N�G�X�g���
 *	@param	p_ans       �����i�[��
 *
 *	@retval TRUE    ����������
 *	@retval FALSE   �������Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_COMM_DATA_Oya_AnserTake( WFBC_COMM_DATA* p_wk, FIELD_WFBC_CORE* p_mywfbc, const FIELD_WFBC_COMM_NPC_REQ* cp_req, FIELD_WFBC_COMM_NPC_ANS* p_ans )
{
  FIELD_WFBC_CORE_PEOPLE* p_people;

  // ���̐l�̏��������B
  p_people = FIELD_WFBC_CORE_GetNpcIDPeople( p_mywfbc, cp_req->npc_id, MAPMODE_INTRUDE );
  if( p_people )
  {
    FIELD_WFBC_CORE_PEOPLE_Clear( p_people );
  }
  
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ��M�������̐ݒ�    �i�q�p�j
 *
 *	@param	p_wk      ���[�N
 *	@param	cp_ans    �����f�[�^
 */
//-----------------------------------------------------------------------------
static void WFBC_COMM_DATA_SetRecvCommAnsData( WFBC_COMM_DATA* p_wk, const FIELD_WFBC_COMM_NPC_ANS* cp_ans )
{
  // �������H
  if( cp_ans->net_id == p_wk->netID )
  {
    GF_ASSERT( FIELD_WFBC_COMM_BUFF_MSK_CHECK(p_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_RECV_ANS) == 0 );
    GFL_STD_MemCopy( cp_ans, &p_wk->recv_ans, sizeof(FIELD_WFBC_COMM_NPC_ANS) );
    p_wk->buff_msk = FIELD_WFBC_COMM_BUFF_MSK_ON( p_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_RECV_ANS );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��M���N�G�X�g���̐ݒ�  �i�e�p�j
 *
 *	@param	p_wk      ���[�N
 *	@param	netID     �l�b�gID
 *	@param	cp_req    ���N�G�X�g�f�[�^
 */
//-----------------------------------------------------------------------------
static void WFBC_COMM_DATA_SetRecvCommReqData( WFBC_COMM_DATA* p_wk, u16 netID, const FIELD_WFBC_COMM_NPC_REQ* cp_req )
{
  GF_ASSERT( FIELD_WFBC_COMM_BUFF_MSK_CHECK(p_wk->buff_msk, (FIELD_WFBC_COMM_BUFF_MSK_RECV_REQ_0+netID)) == 0 );
  GF_ASSERT( netID < FIELD_COMM_MEMBER_MAX );
  GFL_STD_MemCopy( cp_req, &p_wk->recv_req_que[netID], sizeof(FIELD_WFBC_COMM_NPC_REQ) );
  p_wk->buff_msk = FIELD_WFBC_COMM_BUFF_MSK_ON(p_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_RECV_REQ_0+netID);
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��M�������̎擾  �i�q�p�j
 *
 *	@param	p_wk    ���[�N
 *	@param	p_ans   �������̎擾��
 *
 *	@retval TRUE    ��M�f�[�^����
 *	@retval FALSE   ��M�f�[�^�Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_COMM_DATA_GetRecvCommAnsData( const WFBC_COMM_DATA* cp_wk, FIELD_WFBC_COMM_NPC_ANS* p_ans )
{
  BOOL ret = FALSE;

  if( FIELD_WFBC_COMM_BUFF_MSK_CHECK(cp_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_RECV_ANS) )
  {
    GFL_STD_MemCopy( &cp_wk->recv_ans, p_ans, sizeof(FIELD_WFBC_COMM_NPC_ANS) );
    ret = TRUE;
  }
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��M���N�G�X�g���̎擾  �i�e�p�j
 *
 *	@param	cp_wk     ���[�N
 *	@param	netID     �l�b�gID
 *	@param	p_req     ���N�G�X�g���̊i�[��
 *
 *	@retval TRUE    ��M�f�[�^����
 *	@retval FALSE   ��M�f�[�^�Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_COMM_DATA_GetRecvCommReqData( const WFBC_COMM_DATA* cp_wk, u16 netID, FIELD_WFBC_COMM_NPC_REQ* p_req )
{
  BOOL ret = FALSE;
  GF_ASSERT( netID < FIELD_COMM_MEMBER_MAX );

  if( FIELD_WFBC_COMM_BUFF_MSK_CHECK( cp_wk->buff_msk, (FIELD_WFBC_COMM_BUFF_MSK_RECV_REQ_0+netID) ) )
  {
    GFL_STD_MemCopy( &cp_wk->recv_req_que[netID], p_req, sizeof(FIELD_WFBC_COMM_NPC_REQ) );
    ret = TRUE;
  }
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M�������̐ݒ�  (�e�p)
 *
 *	@param	p_wk    ���[�N
 *	@param	netID   �l�b�gID
 *	@param	cp_ans  �������
 */
//-----------------------------------------------------------------------------
static void WFBC_COMM_DATA_SetSendCommAnsData( WFBC_COMM_DATA* p_wk, u16 netID, u16 npc_id, FIELD_WFBC_COMM_NPC_ANS_TYPE ans_type )
{
  GF_ASSERT( FIELD_WFBC_COMM_BUFF_MSK_CHECK( p_wk->buff_msk, (FIELD_WFBC_COMM_BUFF_MSK_SEND_ANS_0+netID) ) == 0 );
  GF_ASSERT( netID < FIELD_COMM_MEMBER_MAX );
  
  p_wk->send_ans_que[netID].net_id    = netID;
  p_wk->send_ans_que[netID].npc_id    = npc_id;
  p_wk->send_ans_que[netID].ans_type  = ans_type;
  
  p_wk->buff_msk = FIELD_WFBC_COMM_BUFF_MSK_ON( p_wk->buff_msk, (FIELD_WFBC_COMM_BUFF_MSK_SEND_ANS_0+netID) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M���N�G�X�g���̐ݒ�  �i�q�p�j
 *
 *	@param	p_wk      ���[�N
 *	@param  npc_id    NPCID
 *	@param  req_type  ���N�G�X�g�^�C�v
 */
//-----------------------------------------------------------------------------
static void WFBC_COMM_DATA_SetSendCommReqData( WFBC_COMM_DATA* p_wk, u16 npc_id, FIELD_WFBC_COMM_NPC_REQ_TYPE req_type )
{
  GF_ASSERT( FIELD_WFBC_COMM_BUFF_MSK_CHECK( p_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_SEND_REQ ) == 0 );

  p_wk->send_req.net_id     = p_wk->netID;
  p_wk->send_req.npc_id     = npc_id;
  p_wk->send_req.req_type   = req_type;

  p_wk->buff_msk = FIELD_WFBC_COMM_BUFF_MSK_ON( p_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_SEND_REQ );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M�������̎擾  �i�e�p�j
 *
 *	@param	cp_wk   ���[�N
 *	@param	netID   �l�b�gID
 *	@param	p_ans   �������i�[��
 *
 *	@retval TRUE    ������񂪂���
 *	@retval FALSE   ������񂪂Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_COMM_DATA_GetSendCommAnsData( const WFBC_COMM_DATA* cp_wk, u16 netID, FIELD_WFBC_COMM_NPC_ANS* p_ans )
{
  BOOL ret = FALSE;

  GF_ASSERT( netID < FIELD_COMM_MEMBER_MAX );

  if( FIELD_WFBC_COMM_BUFF_MSK_CHECK( cp_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_SEND_ANS_0+netID ) )
  {
    GFL_STD_MemCopy( &cp_wk->send_ans_que[netID], p_ans, sizeof(FIELD_WFBC_COMM_NPC_ANS) );
    ret = TRUE;
  }
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M���N�G�X�g���̎擾  �i�q�p�j
 *
 *	@param	cp_wk   ���[�N
 *	@param	p_req   ���N�G�X�g���i�[��
 *
 *	@retval TRUE    ������񂪂���
 *	@retval FALSE   ������񂪂Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_COMM_DATA_GetSendCommReqData( const WFBC_COMM_DATA* cp_wk, FIELD_WFBC_COMM_NPC_REQ* p_req )
{
  BOOL ret = FALSE;

  if( FIELD_WFBC_COMM_BUFF_MSK_CHECK( cp_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_SEND_REQ ) )
  {
    GFL_STD_MemCopy( &cp_wk->send_req, p_req, sizeof(FIELD_WFBC_COMM_NPC_REQ) );
    ret = TRUE;
  }
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��M�������̃N���A
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void WFBC_COMM_DATA_ClearRecvCommAnsData( WFBC_COMM_DATA* p_wk )
{
  GFL_STD_MemClear( &p_wk->recv_ans, sizeof(FIELD_WFBC_COMM_NPC_ANS) );
  p_wk->buff_msk  = FIELD_WFBC_COMM_BUFF_MSK_OFF( p_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_RECV_ANS );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��M���N�G�X�g���̃N���A
 *
 *	@param	p_wk    ���[�N
 *	@param	netID   �l�b�gID
 */
//-----------------------------------------------------------------------------
static void WFBC_COMM_DATA_ClearRecvCommReqData( WFBC_COMM_DATA* p_wk, u16 netID )
{
  GF_ASSERT( netID < FIELD_COMM_MEMBER_MAX );
  
  GFL_STD_MemClear( &p_wk->recv_req_que[netID], sizeof(FIELD_WFBC_COMM_NPC_REQ) );
  p_wk->buff_msk  = FIELD_WFBC_COMM_BUFF_MSK_OFF( p_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_RECV_REQ_0+netID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M�������̃N���A
 *
 *	@param	p_wk    ���[�N
 *	@param	netID   �l�b�gID
 */
//-----------------------------------------------------------------------------
static void WFBC_COMM_DATA_ClearSendCommAnsData( WFBC_COMM_DATA* p_wk, u16 netID )
{
  GF_ASSERT( netID < FIELD_COMM_MEMBER_MAX );
  
  GFL_STD_MemClear( &p_wk->send_ans_que[netID], sizeof(FIELD_WFBC_COMM_NPC_ANS) );
  p_wk->buff_msk  = FIELD_WFBC_COMM_BUFF_MSK_OFF( p_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_SEND_ANS_0+netID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M���N�G�X�g���̃N���A
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void WFBC_COMM_DATA_ClearSendCommReqData( WFBC_COMM_DATA* p_wk )
{
  GFL_STD_MemClear( &p_wk->send_req, sizeof(FIELD_WFBC_COMM_NPC_REQ) );
  p_wk->buff_msk  = FIELD_WFBC_COMM_BUFF_MSK_OFF( p_wk->buff_msk, FIELD_WFBC_COMM_BUFF_MSK_SEND_REQ );
}



