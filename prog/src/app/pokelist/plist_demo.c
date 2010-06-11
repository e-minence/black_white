//======================================================================
/**
 * @file	plist_demo.c
 * @brief	�|�P�������X�g�F�t�H�����`�F���W���o
 * @author	ariizumi
 * @data	10/02/22
 *
 * ���W���[�����FPLIST_DEMO
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/rtc_tool.h"
#include "item/item.h"
#include "sound/pm_wb_voice.h"

#include "pokelist_gra.naix"
#include "msg/msg_pokelist.h"

#include "plist_sys.h"
#include "plist_plate.h"
#include "plist_message.h"
#include "plist_demo.h"
#include "plist_snd_def.h"

#include "pokelist_particle_lst.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define PLIST_DEMO_EFFECT_WORK_SIZE (0x5000)

///�p�[�e�B�N�����o�āA���̎��Ԃ����o������|�P�����A�C�R���ύX
#define PLIST_DEMO_CHANGE_WAIT_GIRATHINA		(65)
///�p�[�e�B�N�����o�āA���̎��Ԃ����o������|�P�����A�C�R���ύX
#define PLIST_DEMO_CHANGE_WAIT_SHEIMI			(38)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void PLIST_DEMO_InitEffect( PLIST_WORK *work );
static void PLIST_DEMO_TermEffect( PLIST_WORK *work );


//--------------------------------------------------------------
//	�f��������
//--------------------------------------------------------------
void PLIST_DEMO_DemoInit( PLIST_WORK *work )
{
  GF_ASSERT_MSG(work->demoType!=PDT_NONE,"Plz set demotype!\n");
  
  work->demoCnt = 0;
  work->demoIsChange = FALSE;
  work->demoIsPlayVoice = FALSE;
  
  //�`���c�L���������߈���\��
  GFL_BG_SetVisible( PLIST_BG_3D , FALSE ); 
  //BG��3D�p�ɐ؂�ւ���
  PLIST_TermBG0_2DMenu( work );
  PLIST_InitBG0_3DParticle( work );

  PLIST_DEMO_InitEffect( work );
	//G2_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE, 0x3f, 31, 0);
  
  work->mainSeq = PSMS_FORM_CHANGE_MAIN;
}

//--------------------------------------------------------------
//	�f���J��
//--------------------------------------------------------------
void PLIST_DEMO_DemoTerm( PLIST_WORK *work )
{
  PLIST_DEMO_TermEffect( work );
  //3D��BG�p�ɐ؂�ւ���
  PLIST_TermBG0_3DParticle( work );
  PLIST_InitBG0_2DMenu( work );

  //����2D�̏������ŏ����Ă�̂Ń����[�h
  PLIST_MSG_ReloadWinFrame( work , work->msgWork );

  if( work->demoType == PDT_GIRATHINA_TO_ORIGIN )
  {
    PLIST_CallbackFunc cbFunc = PLIST_MSGCB_ExitCommon;
    
    if( work->plData->mode == PL_MODE_FIELD )
    {
      //���X�g���玝�����遨�o�b�O�A��
      cbFunc = PLIST_MSGCB_ReturnSelectCommon;
    }
    
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    PLIST_MessageWaitInit( work , mes_pokelist_12_01 , TRUE , cbFunc );
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
  }
  else
  if( work->demoType == PDT_GIRATHINA_TO_ANOTHER )
  {
    PLIST_CallbackFunc cbFunc = PLIST_MSGCB_ExitCommon;
    
    if( work->plData->mode == PL_MODE_FIELD )
    {
      //���X�g���玝�����遨�o�b�O�A��
      cbFunc = PLIST_MSGCB_ReturnSelectCommon;
    }
    
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    PLIST_MessageWaitInit( work , mes_pokelist_12_01 , TRUE , cbFunc );
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
  }
  else
	if( work->demoType == PDT_SHEIMI_TO_SKY )
  {
    PLIST_CallbackFunc cbFunc = PLIST_MSGCB_ExitCommon;

    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    PLIST_MessageWaitInit( work , mes_pokelist_12_01 , TRUE , cbFunc );
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
  }
  else
  {
    work->mainSeq = PSMS_FADEOUT;
  }
}

//--------------------------------------------------------------
//	�f���X�V
//--------------------------------------------------------------
void PLIST_DEMO_DemoMain( PLIST_WORK *work )
{
  if( work->demoIsChange == FALSE )
  {
    work->demoCnt++;
    if( work->demoCnt == 1 )
    {
      //�`���c�L���������߈���\���ɂ����ʂ̍ĕ\��
      GFL_BG_SetVisible( PLIST_BG_3D , TRUE ); 

      //SE�̍Đ�
      if( work->demoType == PDT_GIRATHINA_TO_ORIGIN ||
          work->demoType == PDT_GIRATHINA_TO_ANOTHER )
      {
        PMSND_PlaySystemSE(PLIST_SND_DEMO_GRATHINA);
      }
      else
      if( work->demoType == PDT_SHEIMI_TO_SKY )
      {
        PMSND_PlaySystemSE(PLIST_SND_DEMO_SHEIMI);
      }
    }
    if( work->demoCnt > work->demoChangeTimming )
    {
      PLIST_PLATE_ResetParam( work , work->plateWork[work->pokeCursor] , work->selectPokePara , 0 );
      work->demoIsChange = TRUE;
    }
  }
  if( GFL_PTC_GetEmitterNum( work->ptcWork ) <= 0 &&
      PMSND_CheckPlaySE() == FALSE )
  {
    if( work->demoIsPlayVoice == FALSE )
    {
      const u32 monsNo = PP_Get( work->selectPokePara , ID_PARA_monsno , NULL );
      const u32 formNo = PP_Get( work->selectPokePara , ID_PARA_form_no , NULL );
      work->demoVoicePlayer = PMVOICE_Play( monsNo , formNo , 64 , FALSE , 0 , 0 , FALSE , 0 );
      work->demoIsPlayVoice = TRUE;
    }
    else
    {
      if( PMVOICE_CheckPlay( work->demoVoicePlayer ) == FALSE )
      {
        work->mainSeq = PSMS_FORM_CHANGE_TERM;
      }
    }
  }
  
  GFL_PTC_CalcAll();
  
  //3D�`��  
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    GFL_PTC_DrawAll();
  }
  GFL_G3D_DRAW_End();

  //GFL_PTC_Main();
}
  

//--------------------------------------------------------------
//	�G�t�F�N�g�t�@�C���̏�����
//--------------------------------------------------------------
static void PLIST_DEMO_InitEffect( PLIST_WORK *work )
{
  u8 i;
  u8 emmitNum = 0;
  void* effRes;
  u32 fileIdx = NARC_pokelist_gra_demo_sheimi_spa;
  //���\�[�X�ǂݍ���
  if( work->demoType == PDT_GIRATHINA_TO_ORIGIN ||
      work->demoType == PDT_GIRATHINA_TO_ANOTHER )
  {
    fileIdx = NARC_pokelist_gra_demo_girathina_spa;
    emmitNum = DEMO_GIRATHINA_SPAMAX;
    work->demoChangeTimming = PLIST_DEMO_CHANGE_WAIT_GIRATHINA;
  }
  else
	if( work->demoType == PDT_SHEIMI_TO_SKY )
  {
    fileIdx = NARC_pokelist_gra_demo_sheimi_spa;
    emmitNum = DEMO_SHEIMI_SPAMAX;
    work->demoChangeTimming = PLIST_DEMO_CHANGE_WAIT_SHEIMI;
  }
  effRes = GFL_PTC_LoadArcResource( ARCID_POKELIST , fileIdx , work->heapId );
  
  
  //�܂��}�l�[�W���̍쐬
  work->effTempWork = GFL_NET_Align32Alloc( work->heapId , PLIST_DEMO_EFFECT_WORK_SIZE );
  work->ptcWork = GFL_PTC_Create( work->effTempWork,PLIST_DEMO_EFFECT_WORK_SIZE,FALSE,work->heapId);
  
	//���\�[�X�ƃ}�l�[�W���̊֘A�t��
	GFL_PTC_SetResource( work->ptcWork , effRes , FALSE , GFUser_VIntr_GetTCBSYS() );
	
  //�G�~�b�^�[�̍쐬
  {
    GFL_CLACTPOS pos2d;
    VecFx32 pos;
    PLIST_PLATE_GetPlatePosition( work , work->plateWork[work->pokeCursor] , &pos2d );
    pos2d.x = pos2d.x-( (PLIST_PLATE_WIDTH /2)*8 ) + PLIST_PLATE_POKE_POS_X;
    pos2d.y = pos2d.y-( (PLIST_PLATE_HEIGHT/2)*8 ) + PLIST_PLATE_POKE_POS_Y + 12;
    pos.x = FX32_CONST(pos2d.x)/PLIST_DEMO_SCALE;
    pos.y = FX32_CONST(pos2d.y)/PLIST_DEMO_SCALE;
    pos.z = FX32_CONST(64);
    for( i=0;i<emmitNum;i++ )
    {
      GFL_PTC_CreateEmitter( work->ptcWork , i , &pos );
    }
  }
}

//--------------------------------------------------------------
//	�G�t�F�N�g�t�@�C���̊J��
//--------------------------------------------------------------
static void PLIST_DEMO_TermEffect( PLIST_WORK *work )
{
  u8 i;

  GFL_PTC_Delete( work->ptcWork );
  GFL_NET_Align32Free( work->effTempWork );
}


//--------------------------------------------------------------
//	�M���e�B�i�F�A�i�U�[���I���W���`�F�b�N
//--------------------------------------------------------------
const BOOL PLIST_DEMO_CheckGirathnaToOrigin( PLIST_WORK *work , POKEMON_PARAM *pp )
{
  if( PP_Get( work->selectPokePara , ID_PARA_monsno , NULL ) != MONSNO_GIRATHINA )
  {
    //�M���e�B�i����Ȃ��I
    return FALSE;
  }
  if( PP_Get( work->selectPokePara , ID_PARA_item , NULL ) != ITEM_HAKKINDAMA )
  {
    //�͂����񂾂܎����ĂȂ��I
    return FALSE;
  }
  if( PP_Get( work->selectPokePara , ID_PARA_form_no , NULL ) != FORMNO_GIRATHINA_ANOTHER )
  {
    //�A�i�U�[�t�H��������Ȃ��I
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
//	�M���e�B�i�F�A�i�U�[���I���W���ύX(�ĕ`��͂��Ȃ�
//--------------------------------------------------------------
void PLIST_DEMO_ChangeGirathinaToOrigin( PLIST_WORK *work , POKEMON_PARAM *pp )
{
  PP_ChangeFormNo( pp , FORMNO_GIRATHINA_ORIGIN );
  ZUKANSAVE_SetPokeGet( work->plData->zkn , pp );
}

//--------------------------------------------------------------
//	�M���e�B�i�F�I���W�����A�i�U�[�`�F�b�N
//--------------------------------------------------------------
const BOOL PLIST_DEMO_CheckGirathnaToAnother( PLIST_WORK *work , POKEMON_PARAM *pp )
{
  if( PP_Get( work->selectPokePara , ID_PARA_item , NULL ) == ITEM_HAKKINDAMA )
  {
    //�͂����񂾂܎����Ă�I
    return FALSE;
  }
  if( PP_Get( work->selectPokePara , ID_PARA_monsno , NULL ) != MONSNO_GIRATHINA )
  {
    //�M���e�B�i����Ȃ��I
    return FALSE;
  }
  if( PP_Get( work->selectPokePara , ID_PARA_form_no , NULL ) != FORMNO_GIRATHINA_ORIGIN )
  {
    //�I���W���t�H��������Ȃ��I
    return FALSE;
  }
  return TRUE;
}
//--------------------------------------------------------------
//	�M���e�B�i�F�I���W�����A�i�U�[�ύX(�ĕ`��͂��Ȃ�
//--------------------------------------------------------------
void PLIST_DEMO_ChangeGirathinaToAnother( PLIST_WORK *work , POKEMON_PARAM *pp )
{
  PP_ChangeFormNo( pp , FORMNO_GIRATHINA_ANOTHER );
  ZUKANSAVE_SetPokeGet( work->plData->zkn , pp );
}

//--------------------------------------------------------------
//	�V�F�C�~�F�����h���X�J�C�`�F�b�N
//--------------------------------------------------------------
const BOOL PLIST_DEMO_CheckSheimiToSky( PLIST_WORK *work , POKEMON_PARAM *pp )
{
  if( PP_Get( work->selectPokePara , ID_PARA_monsno , NULL ) != MONSNO_SHEIMI )
  {
    //�V�F�C�~����Ȃ��I
    return FALSE;
  }
  if( PP_Get( work->selectPokePara , ID_PARA_form_no , NULL ) != FORMNO_SHEIMI_LAND )
  {
    //�����h�t�H��������Ȃ��I
    return FALSE;
  }
  if( PP_Get( work->selectPokePara , ID_PARA_event_get_flag , NULL ) != 1 )
  {
    //�C�x���g�z�z����Ȃ��I
    return FALSE;
  }
  if( PP_GetSick( work->selectPokePara ) & PTL_CONDITION_KOORI )
  {
    //�����Ă�I
    return FALSE;
  }
  {
    const int startTime = PM_RTC_GetTimeZoneChangeHour( work->plData->season , TIMEZONE_MORNING );
    const int endTime = PM_RTC_GetTimeZoneChangeHour( work->plData->season , TIMEZONE_NIGHT );
    
    RTCTime time;
    GFL_RTC_GetTime( &time );
    if( time.hour<startTime || time.hour>=endTime )
    {
      //���ԊO�I
      return FALSE;
    }
  }
  
  return TRUE;
}
//--------------------------------------------------------------
//	�V�F�C�~�F�����h���X�J�C�ύX(�ĕ`��͂��Ȃ�
//--------------------------------------------------------------
void PLIST_DEMO_ChangeSheimiToSky( PLIST_WORK *work , POKEMON_PARAM *pp )
{
  PP_ChangeFormNo( pp , FORMNO_SHEIMI_SKY );
  ZUKANSAVE_SetPokeGet( work->plData->zkn , pp );
}
