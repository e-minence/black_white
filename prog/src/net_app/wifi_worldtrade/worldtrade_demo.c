//============================================================================================
/**
 * @file	worldtrade_status.c
 * @brief	���E�����|�P���������f���Ăяo��
 * @author	Akito Mori
 * @date	06.05.10
 */
//============================================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "arc_def.h"
#include <dwc.h>
#include <dpw_tr.h>
#include "print/wordset.h"
#include "message.naix"
#include "system/wipe.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"
#include "savedata/wifilist.h"
#include "savedata/worldtrade_data.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"
#include "savedata/box_savedata.h"

#include "item/itemsym.h"

#include "net_app/worldtrade.h"
#include "worldtrade_local.h"

#include "net_app/pokemontrade.h"
#include "demo/shinka_demo.h"
#include "poke_tool/shinka_check.h"

FS_EXTERN_OVERLAY(pokemon_trade);


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
/*** �֐��v���g�^�C�v ***/
static MYSTATUS *MakePartnerStatusData( Dpw_Tr_Data *dtd );
static POKEMON_PARAM *RecvPokemonParamPointerGet( WORLDTRADE_WORK *wk, int mode );
static void EvoPokemonUpdate( WORLDTRADE_WORK *wk );




//============================================================================================
//	GFL_PROC��`
//============================================================================================

//============================================================================================
//	��`
//============================================================================================
enum{
	DEMO_MODE_DEMO=0,
	DEMO_MODE_SHINKA,
};

//============================================================================================
//	�v���Z�X�֐�
//============================================================================================

//==============================================================================
/**
 * @brief   ���E�����������ʏ�����
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   seq		�i���g�p�j
 *
 * @retval  int		�A�v���V�[�P���X
 */
//==============================================================================
int WorldTrade_Demo_Init(WORLDTRADE_WORK *wk, int seq)
{
  POKEMONTRADE_DEMO_PARAM *p_param;
  const GFL_PROC_DATA *call_proc;

  WorldTrade_ExitSystem( wk );

  wk->sub_proc_wk = GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, sizeof(POKEMONTRADE_DEMO_PARAM) );
  GFL_STD_MemClear( wk->sub_proc_wk, sizeof(POKEMONTRADE_DEMO_PARAM) );
  p_param = wk->sub_proc_wk;
	wk->demoPokePara = PokemonParam_AllocWork( HEAPID_WORLDTRADE );

  p_param->gamedata = GAMESYSTEM_GetGameData( wk->param->gamesys );
  p_param->pMy      = wk->param->mystatus;

	// ���[�h���ƂɃp�����[�^���i�[����
	switch(wk->sub_process_mode){
	// �a����
	case MODE_UPLOAD:
		p_param->pMyPoke = (POKEMON_PARAM*)wk->UploadPokemonData.postData.data;
		p_param->pNPCPoke = p_param->pMyPoke;
		wk->pNPCStatus = MakePartnerStatusData( &wk->UploadPokemonData );
		p_param->pNPC  = wk->pNPCStatus;

    call_proc = &PokemonTradeGTSSendProcData;
		break;

	// �󂯎��
	case MODE_DOWNLOAD:
		p_param->pNPCPoke = (POKEMON_PARAM*)wk->UploadPokemonData.postData.data;
		p_param->pMyPoke = p_param->pNPCPoke;
		// �����MYSTATUS�������̂ŁA�ł������ł���������
		wk->pNPCStatus = MakePartnerStatusData( &wk->UploadPokemonData );
		p_param->pNPC  = wk->pNPCStatus;

    call_proc = &PokemonTradeGTSRecvProcData;
		break;

	// �󂯎��|�P��������������Ă���
	case MODE_DOWNLOAD_EX:
    p_param->pNPCPoke =(POKEMON_PARAM*)wk->UploadPokemonData.postData.data;

    WorldTradeData_GetPokemonData( wk->param->worldtrade_data, wk->demoPokePara );
    p_param->pMyPoke = wk->demoPokePara;

    // �����MYSTATUS�������̂ŁA�ł������ł���������
    wk->pNPCStatus 			= MakePartnerStatusData( &wk->UploadPokemonData );
    p_param->pNPC  = wk->pNPCStatus;

    call_proc = &PokemonTradeGTSProcData;
		break;

	// ��������
	case MODE_EXCHANGE:
		WorldTradeData_GetPokemonData( wk->param->worldtrade_data, wk->demoPokePara );
		p_param->pMyPoke = wk->demoPokePara;
		p_param->pNPCPoke = (POKEMON_PARAM*)wk->DownloadPokemonData[wk->TouchTrainerPos].postData.data;
		// �����MYSTATUS�������̂ŁA�ł������ł���������
		wk->pNPCStatus = MakePartnerStatusData( &wk->DownloadPokemonData[wk->TouchTrainerPos] );
		p_param->pNPC  = wk->pNPCStatus;

    call_proc = &PokemonTradeGTSProcData;
		break;
  default:
    GF_ASSERT(0);
	}

	GAMESYSTEM_CallProc( wk->param->gamesys,
		FS_OVERLAY_ID(pokemon_trade), call_proc, wk->sub_proc_wk );

	wk->subprocflag = 1;

	return SEQ_FADEIN;
}
//==============================================================================
/**
 * @brief   ���E�����������ʃ��C��
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   seq		�i���g�p�j
 *
 * @retval  int		�A�v���V�[�P���X
 */
//==============================================================================
int WorldTrade_Demo_Main(WORLDTRADE_WORK *wk, int seq)
{
	int ret=SEQ_MAIN;

	switch(wk->subprocess_seq){
	case DEMO_MODE_DEMO:
			if( 1 ){	

			// �i���`�F�b�N
			if(wk->sub_process_mode==MODE_EXCHANGE){
				// �����Ō�������
				POKEMON_PARAM *pp = RecvPokemonParamPointerGet( wk, wk->sub_process_mode );
				int item     = PP_Get( pp, ID_PARA_item, NULL );
				int shinkano;
				SHINKA_COND shinka_cond;
				OS_Printf( "�i���`�F�b�N %d\n",PP_Get(pp,ID_PARA_monsno,NULL));

				shinkano=SHINKA_Check( NULL, pp, SHINKA_TYPE_TUUSHIN, item, &shinka_cond, HEAPID_WORLDTRADE );
				if(shinkano!=0){

          SHINKA_DEMO_PARAM *p_param;

          //�i���`�F�b�N�̂��߂�����xPROC����邽�߈�U�폜
          if( wk->sub_proc_wk )
          { 
            GFL_HEAP_FreeMemory(wk->sub_proc_wk);
            wk->sub_proc_wk = NULL;
          }

          //�i���`�F�b�N�̂��߂�����x�쐬
          wk->sub_proc_wk = GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, sizeof(SHINKA_DEMO_PARAM) );
          GFL_STD_MemClear( wk->sub_proc_wk, sizeof(SHINKA_DEMO_PARAM) );
          p_param = wk->sub_proc_wk;
          p_param->gamedata = GAMESYSTEM_GetGameData( wk->param->gamesys );
          p_param->ppt      = PokeParty_AllocPartyWork(HEAPID_WORLDTRADE);
          PokeParty_Add( (POKEPARTY *)p_param->ppt, pp );
          p_param->after_mons_no  = shinkano;
          p_param->shinka_pos     = 0;
          p_param->shinka_cond    = shinka_cond;
          p_param->b_field        = FALSE;
          p_param->b_enable_cancel= TRUE;

          //�i���f��
          GAMESYSTEM_CallProc( wk->param->gamesys,
              FS_OVERLAY_ID(shinka_demo), &ShinkaDemoProcData, wk->sub_proc_wk );

					wk->subprocess_seq = DEMO_MODE_SHINKA;

				}else{
					// �i�������Ȃ炻�̂܂܏I��
					WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
					ret = SEQ_FADEOUT;
				}
			}else if(wk->sub_process_mode==MODE_DOWNLOAD || wk->sub_process_mode==MODE_DOWNLOAD_EX){
				// �����̃|�P������������������������Ă�?
				POKEMON_PARAM *pp     = RecvPokemonParamPointerGet( wk, wk->sub_process_mode );
				POKEMON_PARAM *backup = PokemonParam_AllocWork(HEAPID_WORLDTRADE);
				WorldTradeData_GetPokemonData( wk->param->worldtrade_data, backup );
				if(PP_Get(pp,ID_PARA_monsno,NULL)!=PP_Get(backup,ID_PARA_monsno,NULL)
					|| PP_Get(pp,ID_PARA_personal_rnd,NULL)!=PP_Get(backup,ID_PARA_personal_rnd,NULL)){

					int item     = PP_Get( pp, ID_PARA_item, NULL );
					int shinkano;
					SHINKA_COND shinka_cond;
					OS_Printf( "�i���`�F�b�N %d\n",PP_Get(pp,ID_PARA_monsno,NULL));

					shinkano=SHINKA_Check( NULL, pp, SHINKA_TYPE_TUUSHIN, item, &shinka_cond, HEAPID_WORLDTRADE );
					if(shinkano!=0){
            SHINKA_DEMO_PARAM *p_param;

            //�i���`�F�b�N�̂��߂�����xPROC����邽�߈�U�폜
            if( wk->sub_proc_wk )
            { 
              GFL_HEAP_FreeMemory(wk->sub_proc_wk);
              wk->sub_proc_wk = NULL;
            }

            //�i���`�F�b�N�̂��߂�����x�쐬
            wk->sub_proc_wk = GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, sizeof(SHINKA_DEMO_PARAM) );
            GFL_STD_MemClear( wk->sub_proc_wk, sizeof(SHINKA_DEMO_PARAM) );
            p_param = wk->sub_proc_wk;
            p_param->gamedata = GAMESYSTEM_GetGameData( wk->param->gamesys );
            p_param->ppt      = PokeParty_AllocPartyWork(HEAPID_WORLDTRADE);
            PokeParty_Add( (POKEPARTY *)p_param->ppt, pp );
            p_param->after_mons_no  = shinkano;
            p_param->shinka_pos     = 0;
            p_param->shinka_cond    = shinka_cond;
            p_param->b_field        = FALSE;
            p_param->b_enable_cancel= TRUE;

            //�i���f��
            GAMESYSTEM_CallProc( wk->param->gamesys,
                FS_OVERLAY_ID(shinka_demo), &ShinkaDemoProcData, wk->sub_proc_wk );						wk->subprocess_seq = DEMO_MODE_SHINKA;

          }else{
						// �i�������Ȃ炻�̂܂܏I��
						WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
						ret = SEQ_FADEOUT;
				
					}
					// ��r�p�Ɏg�����|�P�����f�[�^�����
				}else{
					// �����̃|�P����������������̂ł���΂��̂܂�
					WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
					ret = SEQ_FADEOUT;
				}
				GFL_HEAP_FreeMemory(backup);
							
			}else{
				// �i�������Ȃ炻�̂܂܏I��
				WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
				ret = SEQ_FADEOUT;
			}
		}

		break;
	case DEMO_MODE_SHINKA:
		if(1){

      if( wk->sub_proc_wk )
      {
        SHINKA_DEMO_PARAM *p_param  = wk->sub_proc_wk;
        GFL_HEAP_FreeMemory( (void*)p_param->ppt );
        //�p�����[�^�N���A
        GFL_HEAP_FreeMemory(wk->sub_proc_wk);
        wk->sub_proc_wk = NULL;
      }

			// �i�����POKEMON_PARAM���������i�[�����ꏊ�ɔ��f������
			EvoPokemonUpdate( wk );

			// �n�[�h�E�F�A�E�C���h�E������
			GX_SetVisibleWnd(GX_WNDMASK_NONE);

			// �Z�[�u��
//			WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
			WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_POKEMON_EVO_SAVE );
			ret = SEQ_FADEOUT;
		}
		break;
	}

	return ret;
}


//==============================================================================
/**
 * @brief   ���E�����������ʏI��
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   seq		�i���g�p�j
 *
 * @retval  int		�A�v���V�[�P���X
 */
//==============================================================================
int WorldTrade_Demo_End(WORLDTRADE_WORK *wk, int seq)
{

	GFL_HEAP_FreeMemory(wk->demoPokePara);
  if( wk->sub_proc_wk )
  { 
    GFL_HEAP_FreeMemory(wk->sub_proc_wk);
    wk->sub_proc_wk = NULL;
  }

  WorldTrade_InitSystem( wk );

	// �{�b�N�X��ʂɖ߂�
	WorldTrade_SubProcessUpdate( wk );

	return SEQ_INIT;
}


//------------------------------------------------------------------
/**
 * @brief   wifi�̑��肩���MYSTATUS����Ȃ��̂ŁA�Œ���̏��̂ݍ��
 *
 * @param   dtd			�T�[�o�[����Ⴄ�����f�[�^
 *
 * @retval  MYSTATUS *	�X�e�[�^�X���Alloc���ĕԂ�
 */
//------------------------------------------------------------------
static MYSTATUS *MakePartnerStatusData( Dpw_Tr_Data *dtd )
{
	MYSTATUS *status = MyStatus_AllocWork( HEAPID_WORLDTRADE );
	
	MyStatus_Init( status );
	
	// �g���[�i�[���i�[
	MyStatus_SetMyName(status, dtd->name );

	// ROM�E���[�W�����R�[�h�i�[
	MyStatus_SetRomCode( status,    dtd->versionCode );
	MyStatus_SetRegionCode( status, dtd->langCode );
	
	
	return status;
}


//------------------------------------------------------------------
/**
 * @brief   ���������|�P�����̃|�C���^��Ԃ��֐�
 *
 * @param   wk		
 * @param   mode	
 *
 * @retval  POKEMON_PARAM *		
 */
//------------------------------------------------------------------
static POKEMON_PARAM *RecvPokemonParamPointerGet( WORLDTRADE_WORK *wk, int mode )
{
	// �\���I�����ƁA�󓮓I�����̃|�P�����̃|�C���^
	if( mode == MODE_EXCHANGE){
		return (POKEMON_PARAM*)wk->DownloadPokemonData[wk->TouchTrainerPos].postData.data;
	}else if( mode == MODE_DOWNLOAD_EX ){
		return (POKEMON_PARAM*)wk->UploadPokemonData.postData.data;
	}else if( mode == MODE_DOWNLOAD ){
		return (POKEMON_PARAM*)wk->UploadPokemonData.postData.data;
	}

	GF_ASSERT_MSG(0,"�n�����[�h���Ԉ���Ă���");
	return NULL;
}

//------------------------------------------------------------------
/**
 * @brief   �i���|�P������������x���������ꂽ�ꏊ�Ɋi�[���Ȃ���
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void EvoPokemonUpdate( WORLDTRADE_WORK *wk )
{
	POKEMON_PARAM *pp = RecvPokemonParamPointerGet( wk, wk->sub_process_mode );
	
	if(wk->EvoPokeInfo.boxno==18){
		// �Ă����̎�
		WT_PokeCopyPPtoPP( pp, PokeParty_GetMemberPointer( wk->param->myparty, wk->EvoPokeInfo.pos ) );
	}else{
		int boxno=0, boxpos=0;
		// �{�b�N�X�̎�
		// �{�b�N�X�Ɋi�[�����|�P��������U����
		BOXDAT_ClearPokemon( wk->param->mybox, wk->EvoPokeInfo.boxno, wk->EvoPokeInfo.pos );

		// �i�[���Ȃ���
		BOXDAT_GetEmptyTrayNumberAndPos( wk->param->mybox, &boxno, &boxpos );
		BOXDAT_PutPokemonBox( wk->param->mybox, boxno, (POKEMON_PASO_PARAM *)PP_GetPPPPointerConst(pp) );
		
	}
}
