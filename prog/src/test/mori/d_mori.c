//=============================================================================
/**
 *
 *  @file   d_mori.c
 *  @brief  �X�f�o�b�O���j���[
 *  @author mori 
 *  @data   2009.11.21
 *
 *  ���̃\�[�X��testmode.c��include���Ďg�p�����B
 *  testmode.c�̓f�o�b�O�p�A�v���Ȃ̂Ń������ɐς܂�ɂ����i�͂��j
 *
 */
//=============================================================================
#include "app/waza_oshie.h"

//=============================================================================
/**
 *                �萔��`
 */
//=============================================================================

//=============================================================================
/**
 *                �\���̒�`
 */
//=============================================================================

//=============================================================================
/**
 *              �v���g�^�C�v�錾
 */
//=============================================================================
static BOOL TESTMODE_ITEM_SelectWazaOshie( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectMailMake( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectMailView( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectMailbox( TESTMODE_WORK *work , const int idx );

static TESTMODE_MENU_LIST menuMori[] = 
{
  {L"�킴������",TESTMODE_ITEM_SelectWazaOshie },
  {L"���[���쐬",TESTMODE_ITEM_SelectMailMake },
  {L"���[���{��",TESTMODE_ITEM_SelectMailView },
  {L"���[���{�b�N�X",TESTMODE_ITEM_SelectMailbox },
  
  {L"���ǂ�"        ,TESTMODE_ITEM_BackTopMenu },
};

//=============================================================================
/**
 *                �O�����J�֐�
 */
//=============================================================================




//=============================================================================
/**
 *                static�֐�
 */
//=============================================================================


FS_EXTERN_OVERLAY(waza_oshie);
//----------------------------------------------------------------------------------
/**
 * @brief �Z�����Ăяo��
 *
 * @param   work    
 * @param   idx   
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL TESTMODE_ITEM_SelectWazaOshie( TESTMODE_WORK *work , const int idx )
{
  GAMEDATA *gamedata =  GAMEDATA_Create( GFL_HEAPID_APP );
  SAVE_CONTROL_WORK *sv;
  POKEPARTY *party;
  WAZAOSHIE_DATA *param;
  POKEMON_PARAM *pp = PP_Create( 3, 50, 0, GFL_HEAPID_APP );

  param = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(WAZAOSHIE_DATA) );
  party = GAMEDATA_GetMyPokemon( gamedata );
  sv = GAMEDATA_GetSaveControlWork(gamedata);
  
//  PokeParty_Add(party, pp);

  param->gsys = NULL;
  param->pp   = PokeParty_GetMemberPointer( party, 0 );
  param->myst = GAMEDATA_GetMyStatus( gamedata );   // �����f�[�^
  param->cfg  = SaveData_GetConfig(sv);     // �R���t�B�O�f�[�^
  param->waza_tbl   = WAZAOSHIE_GetRemaindWaza( param->pp, GFL_HEAPID_APP );


  TESTMODE_COMMAND_ChangeProc( work,FS_OVERLAY_ID(waza_oshie), &WazaOshieProcData, param );

  return TRUE;
}


#include "savedata/mail_util.h"
#include "app/mailtool.h"
//#include "app/mailsys.h"
//�I�[�o�[���CID��`
FS_EXTERN_OVERLAY(app_mail);
FS_EXTERN_OVERLAY(pmsinput);
FS_EXTERN_OVERLAY(ui_common);

static void MailSys_SetProc(TESTMODE_WORK *work,MAIL_PARAM* param )
{

  TESTMODE_COMMAND_ChangeProc( work, NO_OVERLAY_ID, &MailSysProcData, param );
}


//----------------------------------------------------------------------------------
/**
 * @brief ���[���쐬��ʌĂяo��
 *
 * @param   work    
 * @param   idx   
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL TESTMODE_ITEM_SelectMailMake( TESTMODE_WORK *work , const int idx )
{
  MAIL_PARAM *param; 
  POKEPARTY *party;
  POKEMON_PARAM *pp = PP_Create( 3, 50, 0, GFL_HEAPID_APP );
  GAMEDATA *gamedata =  GAMEDATA_Create( GFL_HEAPID_APP );
  party = GAMEDATA_GetMyPokemon( gamedata );
  GFL_OVERLAY_Load( FS_OVERLAY_ID(app_mail));

  PokeParty_Add(party, pp);


  
  param = MAILSYS_GetWorkCreate( gamedata, 0,0,0, GFL_HEAPID_APP);


  MailSys_SetProc( work, param );
  return TRUE;
  
}


//----------------------------------------------------------------------------------
/**
 * @brief ���[���{����ʌĂяo��
 *
 * @param   work    
 * @param   idx   
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL TESTMODE_ITEM_SelectMailView( TESTMODE_WORK *work , const int idx )
{
  MAIL_PARAM *param; 
  GAMEDATA *gamedata =  GAMEDATA_Create( GFL_HEAPID_APP );
  GFL_OVERLAY_Load( FS_OVERLAY_ID(app_mail));

  param = MailSys_GetWorkViewPrev( gamedata,1, GFL_HEAPID_APP);

  MailSys_SetProc( work, param );
  return TRUE;

}

#include "app/mailbox.h"
//----------------------------------------------------------------------------------
/**
 * @brief ���[���{�b�N�X�Ăяo��
 *
 * @param   work    
 * @param   idx   
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL TESTMODE_ITEM_SelectMailbox( TESTMODE_WORK *work , const int idx )
{
  // �I�[�o�[���CID�錾
//  FS_EXTERN_OVERLAY(overlay_mailbox);

  GAMEDATA *gamedata  = GAMEDATA_Create( GFL_HEAPID_APP );

  MAILBOX_PARAM * prm = GFL_HEAP_AllocMemory( GFL_HEAPID_APP, sizeof(MAILBOX_PARAM) );
  prm->gamedata  = gamedata;
  
  TESTMODE_COMMAND_ChangeProc( work, FS_OVERLAY_ID(app_mail), &MailBoxProcData, prm );
  
  return TRUE;

}

