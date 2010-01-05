//======================================================================
/*
 * @file	event_battle.c
 * @brief	�C�x���g�F�t�B�[���h�G���J�E���g�G�t�F�N�g
 * @author kagaya
 */
//======================================================================
#include <gflib.h>

#include "fieldmap.h"
#include "event_encount_effect.h"

#include "system/screentex.h"
#include "arc/texViewTest.naix"
//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// ENCEFF_WORK
//--------------------------------------------------------------
typedef struct
{
  int count;
  FIELDMAP_WORK *fieldWork;

	GXVRamTex						vTex;
	BOOL								vTexRecover;
	DRAW3DMODE					dMode;
	GFL_G3D_RES*				g3DmdlEff;
	GFL_G3D_RES*				g3DtexEff;
	GFL_G3D_RES*				g3DicaEff;
	GFL_G3D_RND*				g3DrndEff;
	GFL_G3D_ANM*				g3DanmEff;
	GFL_G3D_OBJ*				g3DobjEff;
}ENCEFF_WORK;

ENCEFF_WORK* encEffwork;
//======================================================================
//  proto
//======================================================================
static GMEVENT_RESULT ev_encEffectFunc( GMEVENT *event, int *seq, void *wk );

//======================================================================
//  �C�x���g�@�G���J�E���g�G�t�F�N�g
//======================================================================
//--------------------------------------------------------------
/**
 * �C�x���g�����@�G���J�E���g�G�t�F�N�g
 * @param gsys  GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_FieldEncountEffect(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  GMEVENT *event;
  ENCEFF_WORK *work;
  
  event = GMEVENT_Create( gsys, NULL, ev_encEffectFunc, sizeof(ENCEFF_WORK) );
  
  work = GMEVENT_GetEventWork( event );
	MI_CpuClear8( work, sizeof(ENCEFF_WORK) );

	encEffwork = work;	// ��
  
  work->fieldWork = fieldWork;
  return( event );
}

//--------------------------------------------------------------
/**
 * �C�x���g�@�G���J�E���g�G�t�F�N�g
 * @param event GMEVENT
 * @param seq �C�x���g�V�[�P���X
 * @param wk �C�x���g���[�N
 * @retval GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT ev_encEffectFunc( GMEVENT *event, int *seq, void *wk )
{
  ENCEFF_WORK *work = wk;
  
  switch( (*seq) )
  {
  case 0:
    GFL_FADE_SetMasterBrightReq(
        GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 0, 16, -1 );
    (*seq)++;
  case 1:
    if( GFL_FADE_CheckFade() == FALSE ){
      GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 16, 0, -1 );
      (*seq)++;
    }
    break;
  case 2:
    if( GFL_FADE_CheckFade() == FALSE ){
      work->count++;
      
      if( work->count < 3 ){
        GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 0, 16, -1 );
        (*seq) = 1;
      }else{
        #if 0
        GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 0, 16, -1 );
        (*seq)++;
        #else //�P�x�ύX��Ή� �o�g�����
        //return( GMEVENT_RES_FINISH );
				(*seq)++;
				(*seq)++;
        #endif
      }
    }
    break;
  case 3:
    if( GFL_FADE_CheckFade() == FALSE ){
      //return( GMEVENT_RES_FINISH );
      (*seq)++;
    }
    break;

  case 4:
		// VRAM�o���N�Z�b�g�A�b�v(VRAM_D���e�N�X�`���Ƃ��Ďg�p)
		work->vTex = GX_GetBankForTex(); 
		work->vTexRecover = FALSE;
		if(!(work->vTex & GX_VRAM_D)){
			work->vTexRecover = TRUE;
			work->vTex |= GX_VRAM_D;
			GX_SetBankForTex(work->vTex); 
		}

		// ���\�[�X�쐬
		work->g3DmdlEff = GFL_G3D_CreateResourceArc(ARCID_CAPTEX, NARC_texViewTest_effect1_nsbmd);
		work->g3DicaEff = GFL_G3D_CreateResourceArc(ARCID_CAPTEX, NARC_texViewTest_effect1_nsbca);
		{
			HEAPID heapID = FIELDMAP_GetHeapID(work->fieldWork);
			work->g3DtexEff = GFL_SCRNTEX_CreateG3DresTex(heapID, SCRNTEX_VRAM_D);
		}
		// �����_�[�쐬
		work->g3DrndEff = GFL_G3D_RENDER_Create(work->g3DmdlEff, 0, work->g3DtexEff); 
		// �A�j���쐬
		work->g3DanmEff = GFL_G3D_ANIME_Create(work->g3DrndEff, work->g3DicaEff, 0); 
		// �I�u�W�F�N�g�쐬
		work->g3DobjEff = GFL_G3D_OBJECT_Create(work->g3DrndEff, &work->g3DanmEff, 1); 
		GFL_G3D_OBJECT_EnableAnime(work->g3DobjEff, 0); 

		// �X�N���[���e�N�X�`���쐬
		GFL_SCRNTEX_Load(SCRNTEX_VRAM_D, SCRNTEX_VRAM_D);	// ��VRAM�o���N�ւ̓]���͂Ȃ�

		work->dMode = FIELDMAP_GetDraw3DMode(work->fieldWork);
		FIELDMAP_SetDraw3DMode(work->fieldWork, DRAW3DMODE_SCRNTEX);

    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 0, 16, 3 );
    (*seq)++;
    break;
	case 5:
		if(GFL_G3D_OBJECT_IncAnimeFrame(work->g3DobjEff, 0, FX32_ONE*2) == FALSE){
			(*seq)++;
		}
    break;
	case 6:
		// VRAM�o���N���A
		if(work->vTexRecover == TRUE){
			work->vTex &= (GX_VRAM_D^0xffffffff);
			GX_SetBankForTex(work->vTex); 
		}
		// �g�p���\�[�X�֘A�j��
		GFL_G3D_OBJECT_Delete(work->g3DobjEff); 
		GFL_G3D_ANIME_Delete(work->g3DanmEff); 
		GFL_G3D_RENDER_Delete(work->g3DrndEff); 
		GFL_G3D_DeleteResource(work->g3DicaEff);
		GFL_G3D_DeleteResource(work->g3DtexEff);
		GFL_G3D_DeleteResource(work->g3DmdlEff);

		FIELDMAP_SetDraw3DMode(work->fieldWork, work->dMode);
    return( GMEVENT_RES_FINISH );

  }
  
  return( GMEVENT_RES_CONTINUE );
}


//======================================================================
//  �C�x���g�@�G���J�E���g fieldMap���@Draw3D�֐�
//======================================================================
void ENCEFF_Draw3D(void);
void ENCEFF_Draw3D(void)
{
	GFL_SCRNTEX_DrawDefault(encEffwork->g3DobjEff);
}
