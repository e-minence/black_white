//=============================================================================
/**
 *
 *	@file		debug_hosaka_menu.c
 *	@brief  �ۍ�f�o�b�O���j���[
 *	@author		hosaka genya
 *	@data		2009.10.21
 *
 */
//=============================================================================

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
//static BOOL TESTMODE_ITEM_SelectIntro( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoEnd( TESTMODE_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoEndMulti( TESTMODE_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStart( TESTMODE_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStartMulti( TESTMODE_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectPmsSelect( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectMicTest( TESTMODE_WORK *work , const int idx );

// ���j���[���X�g
static TESTMODE_MENU_LIST menuHosaka[] = 
{
//	{L"�C���g���f��",TESTMODE_ITEM_SelectIntro },
  {L"�ʐM�o�g����f��",TESTMODE_ITEM_SelectCmmBtlDemoEnd},
  {L"�ʐM�o�g����}���`",TESTMODE_ITEM_SelectCmmBtlDemoEndMulti},
  {L"�ʐM�o�g���O�f��",TESTMODE_ITEM_SelectCmmBtlDemoStart},
  {L"�ʐM�o�g���O�}���`",TESTMODE_ITEM_SelectCmmBtlDemoStartMulti},
	{L"���񂢉�b�I��",TESTMODE_ITEM_SelectPmsSelect },
	{L"�}�C�N�e�X�g",TESTMODE_ITEM_SelectMicTest },
	
	{L"���ǂ�"				,TESTMODE_ITEM_BackTopMenu },
};

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

#if 0
// �C���g���f��
#include "demo/intro.h"
static BOOL TESTMODE_ITEM_SelectIntro( TESTMODE_WORK *work , const int idx )
{
  INTRO_PARAM* initParam = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(INTRO_PARAM) );

  initParam->save_ctrl = SaveControl_GetPointer();

	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(intro), &ProcData_Intro, initParam);

  return TRUE;
}
#endif
  
#include "demo/comm_btl_demo.h"

// �f�o�b�O�p�̃p�����[�^�ݒ�
//@TODO ALLOC�����ςȂ�
// ���[�N����
static void debug_param( COMM_BTL_DEMO_PARAM* prm )
{ 
  int i;

  HOSAKA_Printf("in param type = %d \n", prm->type);
  
  prm->result = GFUser_GetPublicRand( COMM_BTL_DEMO_RESULT_MAX );

  HOSAKA_Printf( "result = %d \n", prm->result );

  for( i=0; i<COMM_BTL_DEMO_TRDATA_MAX; i++ )
  {
    prm->trainer_data[i].server_version = GFUser_GetPublicRand(2);
    prm->trainer_data[i].mystatus = SaveData_GetMyStatus( SaveControl_GetPointer() );

#if 0
    prm->trainer_data[i].trsex = (i!=0) ? PM_MALE : PM_FEMALE; //@TODO �Ƃ肠����01�͒j�Œ�
    // �g���[�i�[��
    {
      //�I�[�R�[�h��ǉ����Ă���STRBUF�ɕϊ�
      STRCODE debugname[32] = L"�Ƃ���";
      
      debugname[3] = GFL_STR_GetEOMCode();

      prm->trainer_data[i].str_trname = GFL_STR_CreateBuffer( sizeof(STRCODE)*10, GFL_HEAPID_APP );
      GFL_STR_SetStringCode( prm->trainer_data[i].str_trname, debugname );
    }
#endif
    
    // �f�o�b�O�|�P�p�[�e�B�[
    {
      POKEPARTY *party;
      int poke_cnt;
      int p;

      party = PokeParty_AllocPartyWork( GFL_HEAPID_APP );

      if( prm->type == COMM_BTL_DEMO_TYPE_NORMAL_END || prm->type == COMM_BTL_DEMO_TYPE_NORMAL_START )
      {
        Debug_PokeParty_MakeParty( party );
      }
      else
      {
        static const int pokemax=3;
        PokeParty_Init(party, pokemax);
        for (p = 0; p < pokemax; p++) 
        {
          POKEMON_PARAM* pp = GFL_HEAP_AllocMemoryLo( GFL_HEAPID_APP , POKETOOL_GetWorkSize() );
          PP_Clear(pp);
          PP_Setup( pp, 392+p, 99, 0 );
          PokeParty_Add(party, pp);
          GFL_HEAP_FreeMemory(pp);
        }
      }
      
      prm->trainer_data[i].party = party;

      poke_cnt = PokeParty_GetPokeCount( prm->trainer_data[i].party );

      if( prm->type == COMM_BTL_DEMO_TYPE_NORMAL_END || prm->type == COMM_BTL_DEMO_TYPE_MULTI_END )
      {
        // �ΐ��̃|�P�����̏�Ԉُ�
        for( p=0; p<poke_cnt; p++ )
        {
          POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, p );
          switch( GFUser_GetPublicRand(3) )
          {
          case 0: PP_SetSick( pp, POKESICK_DOKU ); break; // ��
          case 1: PP_Put(pp, ID_PARA_hp, 0 ); break; // �m��
          }
          HOSAKA_Printf("poke [%d] condition=%d \n",p, PP_Get( pp, ID_PARA_condition, NULL ) );
        }
      }

      HOSAKA_Printf("[%d] server_version=%d trsex=%d poke_cnt=%d \n",i, 
          prm->trainer_data[i].server_version,
          prm->trainer_data[i].trsex,
          poke_cnt );
    }
  }
}


static BOOL TESTMODE_ITEM_SelectCmmBtlDemoEnd( TESTMODE_WORK* work, const int idx )
{
  COMM_BTL_DEMO_PARAM* initParam = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(COMM_BTL_DEMO_PARAM) );

  initParam->type = COMM_BTL_DEMO_TYPE_NORMAL_END;
  debug_param(initParam);

	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(comm_btl_demo), &CommBtlDemoProcData, initParam);

  return TRUE;
}

static BOOL TESTMODE_ITEM_SelectCmmBtlDemoEndMulti( TESTMODE_WORK* work, const int idx )
{
  COMM_BTL_DEMO_PARAM* initParam = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(COMM_BTL_DEMO_PARAM) );

  initParam->type = COMM_BTL_DEMO_TYPE_MULTI_END;
  debug_param(initParam);

	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(comm_btl_demo), &CommBtlDemoProcData, initParam);

  return TRUE;
}

static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStart( TESTMODE_WORK* work, const int idx )
{ 
  COMM_BTL_DEMO_PARAM* initParam = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(COMM_BTL_DEMO_PARAM) );

  initParam->type = COMM_BTL_DEMO_TYPE_NORMAL_START;
  debug_param(initParam);

	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(comm_btl_demo), &CommBtlDemoProcData, initParam);

  return TRUE;
}

static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStartMulti( TESTMODE_WORK* work, const int idx )
{
  COMM_BTL_DEMO_PARAM* initParam = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(COMM_BTL_DEMO_PARAM) );

  initParam->type = COMM_BTL_DEMO_TYPE_MULTI_START;
  debug_param(initParam);

	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(comm_btl_demo), &CommBtlDemoProcData, initParam);

  return TRUE;
}

// �ȈՉ�b �I��
#include "app/pms_select.h"
FS_EXTERN_OVERLAY(pmsinput);

static BOOL TESTMODE_ITEM_SelectPmsSelect( TESTMODE_WORK *work , const int idx )
{
  PMS_SELECT_PARAM* initParam = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(PMS_SELECT_PARAM) );

  initParam->save_ctrl = SaveControl_GetPointer();

	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(pmsinput), &ProcData_PMSSelect, initParam);
	return TRUE;
}

// �}�C�N�e�X�g
FS_EXTERN_OVERLAY(mictest);
extern const GFL_PROC_DATA TitleMicTestProcData;
static BOOL TESTMODE_ITEM_SelectMicTest( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(mictest), &TitleMicTestProcData, NULL );
	return TRUE;
}

