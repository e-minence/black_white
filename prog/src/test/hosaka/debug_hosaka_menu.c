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
static BOOL TESTMODE_ITEM_SelectUNSelect( TESTMODE_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoEnd( TESTMODE_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoEndMulti( TESTMODE_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStart( TESTMODE_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStartMulti( TESTMODE_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectPmsSelect( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectPmsDirect( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectPmsPeculiar( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectPmsSingle( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectPmsDouble( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectMicTest( TESTMODE_WORK *work , const int idx );

// ���j���[���X�g
static TESTMODE_MENU_LIST menuHosaka[] = 
{
//	{L"�C���g���f��",TESTMODE_ITEM_SelectIntro },
  {L"���A�t���A�I��",TESTMODE_ITEM_SelectUNSelect},
  {L"�ʐM�o�g����f��",TESTMODE_ITEM_SelectCmmBtlDemoEnd},
  {L"�ʐM�o�g����}���`",TESTMODE_ITEM_SelectCmmBtlDemoEndMulti},
  {L"�ʐM�o�g���O�f��",TESTMODE_ITEM_SelectCmmBtlDemoStart},
  {L"�ʐM�o�g���O�}���`",TESTMODE_ITEM_SelectCmmBtlDemoStartMulti},
	{L"���񂢉�b�I��",TESTMODE_ITEM_SelectPmsSelect },
	{L"���񂢉�b���Ă�",TESTMODE_ITEM_SelectPmsDirect },
	{L"���񂢉�b�Œ�",TESTMODE_ITEM_SelectPmsPeculiar },
	{L"���񂢉�b��P��",TESTMODE_ITEM_SelectPmsSingle },
	{L"���񂢉�b��P��",TESTMODE_ITEM_SelectPmsDouble },
	{L"�}�C�N�e�X�g",TESTMODE_ITEM_SelectMicTest },
	
	{L"���ǂ�"				,TESTMODE_ITEM_BackTopMenu },
};

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================
#include "app/un_select.h"

static BOOL TESTMODE_ITEM_SelectUNSelect( TESTMODE_WORK* work, const int idx )
{
  UN_SELECT_PARAM* initParam = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(UN_SELECT_PARAM) );

  //�e�X�g�f�[�^�Z�b�g
  {
    int i;
    //���֍��R�[�h���e�X�g�Z�b�g
    for( i=0; i<WIFI_COUNTRY_MAX-1; i++)
    {
      if ( GFUser_GetPublicRand0( 2 ) ) initParam->OpenCountryFlg[i] = 1;
      else initParam->OpenCountryFlg[i] = 0;
    }
    
    //�؍ݍ��R�[�h���e�X�g�Z�b�g
    for(i=0;i<FLOOR_MARKING_MAX;i++){
      int code;
      code = GFUser_GetPublicRand(UN_LIST_MAX) + 1;
      initParam->StayCountry[i] = code;
    }

    initParam->InFloor = 5;
  }

	TESTMODE_COMMAND_ChangeProc(work, FS_OVERLAY_ID(un_select), &UNSelectProcData, initParam);

  return TRUE;
}

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

    {
      //@TODO ALLOC�����ςȂ�
      MYSTATUS* st = GFL_HEAP_AllocMemoryLo( GFL_HEAPID_APP, MyStatus_GetWorkSize() );

      MyStatus_Copy( SaveData_GetMyStatus( SaveControl_GetPointer() ), st );

      if( i == 0 )
      {
        STRCODE debugname[8] = L"A";
        debugname[1] = GFL_STR_GetEOMCode();
        MyStatus_SetMySex( st, PM_MALE );
        MyStatus_SetMyName( st, debugname);
      }
      else if ( i == 1 )
      {
        STRCODE debugname[8] = L"B";
        debugname[1] = GFL_STR_GetEOMCode();
        MyStatus_SetMySex( st, PM_FEMALE );
        MyStatus_SetMyName( st, debugname);
      }
      else if ( i == 2 )
      {
        STRCODE debugname[8] = L"C";
        debugname[1] = GFL_STR_GetEOMCode();
        MyStatus_SetMySex( st, PM_MALE );
        MyStatus_SetMyName( st, debugname);
      }
      else if ( i == 3 )
      {
        STRCODE debugname[8] = L"D";
        debugname[1] = GFL_STR_GetEOMCode();
        MyStatus_SetMySex( st, PM_FEMALE );
        MyStatus_SetMyName( st, debugname);
      }

      prm->trainer_data[i].mystatus = st;
    }

#if 0
 // �p�~�\��
//    prm->trainer_data[i].trsex = PM_FEMALE;//(GFUser_GetPublicRand(2)==0) ? PM_MALE : PM_FEMALE;  
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

#if 0
      if( prm->type == COMM_BTL_DEMO_TYPE_NORMAL_END || prm->type == COMM_BTL_DEMO_TYPE_NORMAL_START )
      {
        Debug_PokeParty_MakeParty( party );
      }
      else
#endif
      {
        int pokemax = 3;
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
          MyStatus_GetMySex( prm->trainer_data[i].mystatus ),
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


// �ȈՉ�b
#include "app/pms_select.h"
#include "app/pms_input.h"
FS_EXTERN_OVERLAY(pmsinput);

//#define TESTMODE_PMS_USE_PROC

#ifdef TESTMODE_PMS_USE_PROC
typedef struct
{
  HEAPID                parent_heap_id;
  TestMode_SelectFunc   func;
}
TESTMODE_PMS_PARAM;

typedef struct
{
  HEAPID                heap_id;
  GFL_PROCSYS*          local_procsys;
  void*                 local_proc_param;
}
TESTMODE_PMS_WORK;

static TESTMODE_PMS_PARAM*  TESTMODE_PMS_CreateParam( HEAPID heap_id, TestMode_SelectFunc func );
static void                 TESTMODE_PMS_DeleteParam( TESTMODE_PMS_PARAM* param );

static TESTMODE_PMS_PARAM*  TESTMODE_PMS_CreateParam( HEAPID heap_id, TestMode_SelectFunc func )
{
  TESTMODE_PMS_PARAM* param = GFL_HEAP_AllocClearMemory( heap_id, sizeof(TESTMODE_PMS_PARAM) );
  param->parent_heap_id   = heap_id;
  param->func             = func;
  return param;
}
static void                 TESTMODE_PMS_DeleteParam( TESTMODE_PMS_PARAM* param )
{
  GFL_HEAP_FreeMemory( param );
}

static GFL_PROC_RESULT TESTMODE_PMS_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT TESTMODE_PMS_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT TESTMODE_PMS_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
const GFL_PROC_DATA  TESTMODE_PMS_ProcData =
{
  TESTMODE_PMS_ProcInit,
  TESTMODE_PMS_ProcMain,
  TESTMODE_PMS_ProcExit,
};

static GFL_PROC_RESULT TESTMODE_PMS_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  TESTMODE_PMS_PARAM*  param    = (TESTMODE_PMS_PARAM*)pwk;
  TESTMODE_PMS_WORK*   work;
  
  GFL_HEAP_CreateHeap( param->parent_heap_id, HEAPID_UI_DEBUG, 0x10000 );
  work = GFL_PROC_AllocWork( proc, sizeof(TESTMODE_PMS_WORK), HEAPID_UI_DEBUG );
  
  work->heap_id = HEAPID_UI_DEBUG;
  work->local_procsys = GFL_PROC_LOCAL_boot( work->heap_id );
  
  return GFL_PROC_RES_FINISH;
}
static GFL_PROC_RESULT TESTMODE_PMS_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  TESTMODE_PMS_PARAM*  param    = (TESTMODE_PMS_PARAM*)pwk;
  TESTMODE_PMS_WORK*   work     = (TESTMODE_PMS_WORK*)mywk;

  GFL_PROC_LOCAL_Exit( work->local_procsys );

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_UI_DEBUG );
  
  return GFL_PROC_RES_FINISH;
}
static GFL_PROC_RESULT TESTMODE_PMS_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  TESTMODE_PMS_PARAM*  param    = (TESTMODE_PMS_PARAM*)pwk;
  TESTMODE_PMS_WORK*   work     = (TESTMODE_PMS_WORK*)mywk;

  GFL_PROC_MAIN_STATUS  local_proc_status   =  GFL_PROC_LOCAL_Main( work->local_procsys );
  if( local_proc_status == GFL_PROC_MAIN_VALID ) return GFL_PROC_RES_CONTINUE;

  switch(*seq)
  {
  case 0:
    {
      if( param->func == TESTMODE_ITEM_SelectPmsSelect )
      {
        PMS_SELECT_PARAM* initParam = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(PMS_SELECT_PARAM) );
        initParam->save_ctrl = SaveControl_GetPointer();
        GFL_PROC_LOCAL_CallProc( work->local_procsys, FS_OVERLAY_ID(pmsinput), &ProcData_PMSSelect, initParam );
        work->local_proc_param = initParam;
      }
      else if( param->func == TESTMODE_ITEM_SelectPmsDirect )
      {
        PMS_DATA data;
        PMSI_PARAM* pmsi;
        PMSDAT_Init( &data, 0 );
        pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, NULL, TRUE, SaveControl_GetPointer(), work->heap_id );  // �f�R����OK
        //pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, NULL, FALSE, SaveControl_GetPointer(), work->heap_id );  // �f�R�����֎~
        GFL_PROC_LOCAL_CallProc( work->local_procsys, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );
        work->local_proc_param = pmsi;
      }
      else if( param->func == TESTMODE_ITEM_SelectPmsPeculiar )
      {
        PMS_DATA data;
        PMSI_PARAM* pmsi;
        PMSDAT_Init( &data, 0 );
        //pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, &data, TRUE, SaveControl_GetPointer(), work->heap_id );  // �f�R����OK
        pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, &data, FALSE, SaveControl_GetPointer(), work->heap_id );  // �f�R�����֎~
        GFL_PROC_LOCAL_CallProc( work->local_procsys, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );
        work->local_proc_param = pmsi;
      }
      else if( param->func == TESTMODE_ITEM_SelectPmsSingle )
      {
        PMS_DATA data;
        PMSI_PARAM* pmsi;
        PMSDAT_Init( &data, 0 );
        pmsi = PMSI_PARAM_Create( PMSI_MODE_SINGLE, PMSI_GUIDANCE_DEFAULT, NULL, FALSE, SaveControl_GetPointer(), work->heap_id );  // data�͑���Ȃ�
        GFL_PROC_LOCAL_CallProc( work->local_procsys, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );
        work->local_proc_param = pmsi;
      }
      else if( param->func == TESTMODE_ITEM_SelectPmsDouble )
      {
        PMS_DATA data;
        PMSI_PARAM* pmsi;
        PMSDAT_Init( &data, 0 );
        //pmsi = PMSI_PARAM_Create( PMSI_MODE_DOUBLE, PMSI_GUIDANCE_DEFAULT, &data, TRUE, SaveControl_GetPointer(), work->heap_id );  // data�𑗂��Ă݂�
        pmsi = PMSI_PARAM_Create( PMSI_MODE_DOUBLE, PMSI_GUIDANCE_DEFAULT, NULL, TRUE, SaveControl_GetPointer(), work->heap_id );  // �P�ꃂ�[�h�͕��͌Œ�ɖ��Ή��炵�� pmsi_param.c
        GFL_PROC_LOCAL_CallProc( work->local_procsys, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );
        work->local_proc_param = pmsi;
      }
      (*seq)++;
    }
    break;
  case 1:
    {
      if( local_proc_status != GFL_PROC_MAIN_VALID )
      {
        if( param->func == TESTMODE_ITEM_SelectPmsSelect )
        {
          PMS_SELECT_PARAM* initParam = (PMS_SELECT_PARAM*)work->local_proc_param;
          GFL_HEAP_FreeMemory( initParam );
        }
        else if( param->func == TESTMODE_ITEM_SelectPmsDirect )
        {
          PMSI_PARAM* pmsi = (PMSI_PARAM*)work->local_proc_param;
          PMSI_PARAM_Delete( pmsi );
        }
        else if( param->func == TESTMODE_ITEM_SelectPmsPeculiar )
        {
          PMSI_PARAM* pmsi = (PMSI_PARAM*)work->local_proc_param;
          PMSI_PARAM_Delete( pmsi );
        }
        else if( param->func == TESTMODE_ITEM_SelectPmsSingle )
        {
          PMSI_PARAM* pmsi = (PMSI_PARAM*)work->local_proc_param;
          PMSI_PARAM_Delete( pmsi );
        }
        else if( param->func == TESTMODE_ITEM_SelectPmsDouble )
        {
          PMSI_PARAM* pmsi = (PMSI_PARAM*)work->local_proc_param;
          PMSI_PARAM_Delete( pmsi );
        }
        (*seq)++;
      }
    }
    break;
  case 2:
    {
      return GFL_PROC_RES_FINISH;
    }
    break;
  }

  return GFL_PROC_RES_CONTINUE;
}
#endif  // #ifdef TESTMODE_PMS_USE_PROC

// �ȈՉ�b �I��
static BOOL TESTMODE_ITEM_SelectPmsSelect( TESTMODE_WORK *work , const int idx )
{
#ifndef TESTMODE_PMS_USE_PROC
  PMS_SELECT_PARAM* initParam = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(PMS_SELECT_PARAM) );

  initParam->save_ctrl = SaveControl_GetPointer();

	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(pmsinput), &ProcData_PMSSelect, initParam);
	return TRUE;
#else
  TESTMODE_PMS_PARAM* param = TESTMODE_PMS_CreateParam( GFL_HEAPID_APP, TESTMODE_ITEM_SelectPmsSelect );
	TESTMODE_COMMAND_ChangeProc( work, FS_OVERLAY_ID(testmode), &TESTMODE_PMS_ProcData, param );
	return TRUE;
#endif
}


// �ȈՉ�b ���Ă�
static BOOL TESTMODE_ITEM_SelectPmsDirect( TESTMODE_WORK *work , const int idx )
{
#ifndef TESTMODE_PMS_USE_PROC
  PMS_DATA data;
  PMSI_PARAM* pmsi;

/*
  {
    PMSDAT_Init( &data, 0 );
    pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, NULL, TRUE, SaveControl_GetPointer(), GFL_HEAPID_APP );  // �f�R����OK
  }
*/
/*
  {
    PMSDAT_Init( &data, 0 );
    pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, NULL, FALSE, SaveControl_GetPointer(), GFL_HEAPID_APP );  // �f�R�����֎~
  }
*/
  {
    PMSDAT_Init( &data, PMS_TYPE_BATTLE_WON );  // �퓬�����̒�^���ŃX�^�[�g
    pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, NULL, FALSE, SaveControl_GetPointer(), GFL_HEAPID_APP );  // �f�R�����֎~
    PMSI_PARAM_SetInitializeDataSentence( pmsi, &data );  // �퓬�����̒�^���ŃX�^�[�g
  }

	TESTMODE_COMMAND_ChangeProc( work, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );

  return TRUE;
#else

  TESTMODE_PMS_PARAM* param = TESTMODE_PMS_CreateParam( GFL_HEAPID_APP, TESTMODE_ITEM_SelectPmsDirect );
	TESTMODE_COMMAND_ChangeProc( work, FS_OVERLAY_ID(testmode), &TESTMODE_PMS_ProcData, param );
	return TRUE;
#endif
}

// �ȈՉ�b �Œ�
#include "msg\msg_pmss_peculiar.h"
static BOOL TESTMODE_ITEM_SelectPmsPeculiar( TESTMODE_WORK *work , const int idx )
{
#ifndef TESTMODE_PMS_USE_PROC
  PMS_DATA data;
  PMSI_PARAM* pmsi;

  //PMSDAT_Init( &data, PMS_TYPE_PECULIAR );  // �ŗL��0�Ԗڂ̒�^���ŁA�^�O�͋�ŏ����������
  //PMSDAT_SetSentence( &data, PMS_TYPE_PECULIAR, pmss_peculiar_08 );  // �ŗL��7�Ԗڂ̒�^���ɂ���(�^�O�ɑ΂��Ă͉�������Ȃ�)
  PMSDAT_InitAndSetSentence( &data, PMS_TYPE_PECULIAR, pmss_peculiar_08 );  // �ŗL��7�Ԗڂ̒�^���ŁA�^�O�͋�ŏ���������
  pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, &data, TRUE, SaveControl_GetPointer(), GFL_HEAPID_APP );  // �f�R����OK
  //pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, &data, FALSE, SaveControl_GetPointer(), GFL_HEAPID_APP );  // �f�R�����֎~

	TESTMODE_COMMAND_ChangeProc( work, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );

  return TRUE;
#else
  TESTMODE_PMS_PARAM* param = TESTMODE_PMS_CreateParam( GFL_HEAPID_APP, TESTMODE_ITEM_SelectPmsPeculiar );
	TESTMODE_COMMAND_ChangeProc( work, FS_OVERLAY_ID(testmode), &TESTMODE_PMS_ProcData, param );
	return TRUE;
#endif
}
	
// �ȈՉ�b ��P��
static BOOL TESTMODE_ITEM_SelectPmsSingle( TESTMODE_WORK *work , const int idx )
{
#ifndef TESTMODE_PMS_USE_PROC
  PMS_DATA data;
  PMSI_PARAM* pmsi;

  PMSDAT_Init( &data, 0 );
  pmsi = PMSI_PARAM_Create( PMSI_MODE_SINGLE, PMSI_GUIDANCE_DEFAULT, NULL, FALSE, SaveControl_GetPointer(), GFL_HEAPID_APP );  // data�͑���Ȃ�

	TESTMODE_COMMAND_ChangeProc( work, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );

  return TRUE;
#else

  TESTMODE_PMS_PARAM* param = TESTMODE_PMS_CreateParam( GFL_HEAPID_APP, TESTMODE_ITEM_SelectPmsSingle );
	TESTMODE_COMMAND_ChangeProc( work, FS_OVERLAY_ID(testmode), &TESTMODE_PMS_ProcData, param );
	return TRUE;
#endif
}

// �ȈՉ�b ��P��
static BOOL TESTMODE_ITEM_SelectPmsDouble( TESTMODE_WORK *work , const int idx )
{
#ifndef TESTMODE_PMS_USE_PROC
  PMS_DATA data;
  PMSI_PARAM* pmsi;

  PMSDAT_Init( &data, 0 );
  //pmsi = PMSI_PARAM_Create( PMSI_MODE_DOUBLE, PMSI_GUIDANCE_DEFAULT, &data, TRUE, SaveControl_GetPointer(), GFL_HEAPID_APP );  // data�𑗂��Ă݂�
  pmsi = PMSI_PARAM_Create( PMSI_MODE_DOUBLE, PMSI_GUIDANCE_DEFAULT, NULL, TRUE, SaveControl_GetPointer(), GFL_HEAPID_APP );  // �P�ꃂ�[�h�͕��͌Œ�ɖ��Ή��炵�� pmsi_param.c

	TESTMODE_COMMAND_ChangeProc( work, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );

  return TRUE;
#else
  TESTMODE_PMS_PARAM* param = TESTMODE_PMS_CreateParam( GFL_HEAPID_APP, TESTMODE_ITEM_SelectPmsDouble );
	TESTMODE_COMMAND_ChangeProc( work, FS_OVERLAY_ID(testmode), &TESTMODE_PMS_ProcData, param );
	return TRUE;
#endif
}


// �}�C�N�e�X�g
#include "app/mictest.h"
static BOOL TESTMODE_ITEM_SelectMicTest( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(mictest), &TitleMicTestProcData, NULL );
	return TRUE;
}

