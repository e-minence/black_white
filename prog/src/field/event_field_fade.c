//============================================================================================
/**
 * @file	event_field_fade.c
 * @brief	�C�x���g�F�t�B�[���h�t�F�[�h����c�[��
 * @author	tamada GAMEFREAK inc.
 * @date	2008.12.10
 *
 * 2009.12.22 tamada event_fieldmap_control���番��
 */
//============================================================================================

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/fieldmap_call.h"  //FIELDMAP_IsReady,FIELDMAP_ForceUpdate
#include "field/fieldmap.h"

#include "event_field_fade.h"
#include "vblank_bg_scale_expand.h"

#include "system/main.h"      // HEAPID_PROC

#include "event_season_display.h"  // for EVENT_SeasonDisplay
#include "field_status_local.h"  // for FIELD_STATUS

#include "system/screentex.h"

#include "gamesystem/pm_season.h"  // for PMSEASON_xxxx

#ifdef PM_DEBUG
BOOL DebugBGInitEnd = FALSE;    //BG�������Ď��t���O
#endif


#define BG_FRAME_CROSS_FADE ( FLDBG_MFRM_EFF1 ) 


//============================================================================================
// ���t�F�[�h�C�x���g���[�N
//============================================================================================ 
typedef struct 
{
  GAMESYS_WORK*      gameSystem;
  FIELDMAP_WORK*     fieldmap;

	FIELD_FADE_TYPE fadeType;  // �t�F�[�h�̎�� ( �N���X�t�F�[�h, �P�x�t�F�[�h�Ȃ� )

  // �P�x�t�F�[�h
  int                     brightFadeMode;      // �t�F�[�h���[�h
  FIELD_FADE_WAIT_FLAG    fadeFinishWaitFlag;  // �t�F�[�h�����̊�����҂��ǂ���
  FIELD_FADE_BG_INIT_FLAG BGInitFlag;          // BG�ʂ̏��������s�����ǂ���

  // �N���X�t�F�[�h
	int alphaWork;

  // �G�߃t�F�[�h
  u8 startSeason;  // �ŏ��ɕ\������G��
  u8 endSeason;    // �Ō�ɕ\������G��

} FADE_EVENT_WORK;


//-------------------------------------------------------------------------------------------
/**
 * @brief �P�x�t�F�[�h���[�h���擾����
 *
 * @param fadeType �t�F�[�h�^�C�v ( �N���X�t�F�[�h, �P�x�t�F�[�h �Ȃ� )
 *
 * @return �w��t�F�[�h�^�C�v�ɉ������P�x�t�F�[�h���[�h
 */
//-------------------------------------------------------------------------------------------
static int GetBrightFadeMode( FIELD_FADE_TYPE fadeType )
{
  int brightFadeMode;

  switch( fadeType )
  {
  default:
  case FIELD_FADE_BLACK: // �P�x�t�F�[�h(�u���b�N)
    brightFadeMode = GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB;
    break;
  case FIELD_FADE_WHITE: // �P�x�t�F�[�h(�z���C�g)
    brightFadeMode = GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN | GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB;
    break;
  }

  return brightFadeMode;
}


//============================================================================================
// ���t�F�[�h�A�E�g
//============================================================================================ 

//--------------------------------------------------------------------------------------------
/**
 * @brief �P�x�t�F�[�h�A�E�g �C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT FieldBrightOutEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work = wk;

	switch( *seq )
  {
	case 0:
    GFL_FADE_SetMasterBrightReq( work->brightFadeMode, 0, 16, 0 );
		(*seq)++;
		break;
	case 1:
    if( work->fadeFinishWaitFlag == FIELD_FADE_NO_WAIT ){ return GMEVENT_RES_FINISH; } // �t�F�[�h�҂��Ȃ�
		if( GFL_FADE_CheckFade() == FALSE ){ return GMEVENT_RES_FINISH; }  // �t�F�[�h����
		break;
	}
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�P�x�t�F�[�h�A�E�g �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 * @param	fadeType		        �t�F�[�h�̎�ގw�� ( FIELD_FADE_BLACK or FIELD_FADE_WHITE )
 * @param fadeFinishWaitFlag  �t�F�[�h������҂��ǂ���
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_FieldBrightOut( GAMESYS_WORK* gameSystem, 
                                      FIELDMAP_WORK* fieldmap, 
                                      FIELD_FADE_TYPE fadeType, 
                                      FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, FieldBrightOutEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem         = gameSystem;
  work->fieldmap           = fieldmap;
  work->fadeType           = fadeType;
  work->fadeFinishWaitFlag = fadeFinishWaitFlag;
  work->brightFadeMode     = GetBrightFadeMode( fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �N���X�t�F�[�h�A�E�g �C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT FieldCrossOutEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work = wk;

	switch( *seq )
  {
	case 0:
		// �T�u��ʂ����u���b�N�A�E�g
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, -8);

		// ��ʃL���v�`����VRAM_D
		GX_SetBankForLCDC(GX_VRAM_LCDC_D);
		GX_SetCapture(GX_CAPTURE_SIZE_256x192,				// Capture size
				          GX_CAPTURE_MODE_A,							// Capture mode
				          GX_CAPTURE_SRCA_2D3D,						// Blend src A
				          GX_CAPTURE_SRCB_VRAM_0x00000,		// Blend src B
									GX_CAPTURE_DEST_VRAM_D_0x00000,	// dst
								  16, 0);             // Blend parameter for src A, B

		OS_WaitVBlankIntr();	// 0���C���҂��E�G�C�g
		OS_WaitVBlankIntr();	// �L���v�`���[�҂��E�G�C�g
#if 0
		(*seq)++;
		break;

	case 1:	// 0���C���҂��E�G�C�g
	case 2:	// �L���v�`���[�҂��E�G�C�g
		(*seq)++;
		break;

	case 3:
#endif
		//�`�惂�[�h�ύX
		{
			const GFL_BG_SYS_HEADER bg_sys_header = 
						{ GX_DISPMODE_GRAPHICS,GX_BGMODE_5,GX_BGMODE_0,GX_BG0_AS_3D };
			GFL_BG_SetBGMode( &bg_sys_header );
		}

		//�a�f�Z�b�g�A�b�v
		G2_SetBG2ControlDCBmp
			(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);
		GX_SetBankForBG(GX_VRAM_BG_128_D);

		//�A���t�@�u�����h
		//G2_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE, GX_BLEND_PLANEMASK_BG2, 0,0);
		GFL_BG_SetPriority(BG_FRAME_CROSS_FADE, 0);
		GFL_BG_SetVisible( BG_FRAME_CROSS_FADE, VISIBLE_ON );

    // �L���v�`���ʂ̊g��J�n
    StartVBlankBGExpand();

		return GMEVENT_RES_FINISH;
	}

	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�N���X�t�F�[�h�A�E�g �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_FieldCrossOut( GAMESYS_WORK *gameSystem, FIELDMAP_WORK * fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, FieldCrossOutEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
  work->fadeType   = FIELD_FADE_CROSS;

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �f�o�b�O�p �����A�E�g�C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT QuickFadeOutEvent( GMEVENT* event, int* seq, void* wk )
{
  GFL_FADE_SetMasterBrightReq( 
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, -16 );

  return GMEVENT_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p �����A�E�g �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_QuickFadeOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

  event = GMEVENT_Create( gameSystem, NULL, QuickFadeOutEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;

  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	����� �P�x�t�F�[�h�A�E�g �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 * @param	fadeType	         �t�F�[�h�̎�ގw�� ( FIELD_FADE_BLACK or FIELD_FADE_WHITE )
 * @param fadeFinishWaitFlag �t�F�[�h������҂��ǂ���
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
GMEVENT* EVENT_FlySkyBrightOut( GAMESYS_WORK *gameSystem, FIELDMAP_WORK * fieldmap, 
                                FIELD_FADE_TYPE fadeType, FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, FieldBrightOutEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem         = gameSystem;
  work->fieldmap           = fieldmap;
	work->fadeType           = fadeType;
  work->fadeFinishWaitFlag = fadeFinishWaitFlag;
  work->brightFadeMode     = GetBrightFadeMode( fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�t�F�[�h�A�E�g�C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 * @param	fadeType		       �t�F�[�h�̎�ގw��
 * @param fadeFinishWaitFlag �t�F�[�h������҂��ǂ���(�P�x�t�F�[�h���̂ݗL��)
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
GMEVENT* EVENT_FieldFadeOut( GAMESYS_WORK *gameSystem, FIELDMAP_WORK * fieldmap, 
                             FIELD_FADE_TYPE fadeType, 
                             FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag ) 
{ 
  GMEVENT* event;

  // �t�F�[�h�C�x���g����
  switch( fadeType )
  {
  default:
  case FIELD_FADE_CROSS: // �N���X�t�F�[�h
    event = EVENT_FieldCrossOut( gameSystem, fieldmap );
    break;
  case FIELD_FADE_BLACK:  // �P�x�t�F�[�h(�u���b�N)
  case FIELD_FADE_WHITE:  // �P�x�t�F�[�h(�z���C�g)
  case FIELD_FADE_SEASON: // �G�߃t�F�[�h
    event = EVENT_FieldBrightOut( gameSystem, fieldmap, fadeType, fadeFinishWaitFlag );
    break;
  }
	return event;
}


//============================================================================================
// ���t�F�[�h�C��
//============================================================================================ 

//--------------------------------------------------------------------------------------------
/**
 * @brief �N���X�t�F�[�h�C���C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT FieldCrossInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work = wk;

	switch( *seq )
  {
	case 0:
    work->alphaWork = 16;
    GFL_BG_SetPriority(BG_FRAME_CROSS_FADE, 0);
    GFL_BG_SetPriority(FLDBG_MFRM_3D, 1);
    GFL_BG_SetVisible( FLDBG_MFRM_3D, VISIBLE_ON );
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG0, work->alphaWork, 0 );

		// �T�u��ʋP�x���A
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, -2);
    (*seq)++;
    break;

	case 1:	// �N���X�t�F�[�h
		if(work->alphaWork){
			work->alphaWork--;
			G2_ChangeBlendAlpha( work->alphaWork, 16 - work->alphaWork );
		} else {
			GFL_BG_SetVisible( BG_FRAME_CROSS_FADE, VISIBLE_OFF );

			//VRAM�N���A
			GX_SetBankForLCDC(GX_VRAM_LCDC_D);
			MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_VRAM_D_SIZE);
			(void)GX_DisableBankForLCDC();

			// ���C��BG�ւ̊��蓖�ĕ��A(fieldmap.c�Ɛ��������Ƃ邱��)
			GX_SetBankForBG(GX_VRAM_LCDC_D);	

      // �L���v�`��BG�ʂ̊g��I��
      EndVBlankBGExpand();

      (*seq)++;
		}
		break;

	case 2:	// �N���X�t�F�[�hEND
		//OS_WaitVBlankIntr();	// ��ʂ�����h�~�p�E�G�C�g
    // BG���[�h�ݒ�ƕ\���ݒ�̕��A
    {
      int mv = GFL_DISP_GetMainVisible();
      FIELDMAP_InitBGMode();
      GFL_DISP_GX_SetVisibleControlDirect( mv );
    }
    (*seq)++;
		break;

	case 3:	// �N���X�t�F�[�hEND
		FIELDMAP_InitBG(work->fieldmap);
		return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�N���X�t�F�[�h�C�� �C�x���g����
 * @param	gameSystem
 * @param	fieldmap 
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_FieldCrossIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, FieldCrossInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
  work->fadeType   = FIELD_FADE_CROSS;

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �G�߃t�F�[�h�C�� �C�x���g
 */ 
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT FieldSeasonInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;

	switch( *seq )
  {
	case 0:
    // �G�ߕ\���C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_SeasonDisplay( gameSystem, fieldmap, work->startSeason, work->endSeason ) );
		(*seq)++;
		break;
  case 1:
    return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�G�߃t�F�[�h�C�� �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 * @param startSeason �\������ŏ��̋G��
 * @param endSeason   �\������Ō�̋G��
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_FieldSeasonIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, 
                                     u8 startSeason, u8 endSeason )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, FieldSeasonInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem  = gameSystem;
  work->fieldmap    = fieldmap;
	work->fadeType    = FIELD_FADE_SEASON;
  work->startSeason = startSeason;
  work->endSeason   = endSeason;

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �P�x�t�F�[�h�C�� �C�x���g
 */ 
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT FieldBrightInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work = wk;

	switch( *seq )
  {
  case 0:
    // BG���[�h�ݒ�ƕ\���ݒ�̕��A
    if ( work->BGInitFlag )
    {
      int mv = GFL_DISP_GetMainVisible();
      FIELDMAP_InitBGMode();
      GFL_DISP_GX_SetVisibleControlDirect( mv );
      FIELDMAP_InitBG(work->fieldmap);
    }
    GFL_FADE_SetMasterBrightReq(work->brightFadeMode, 16, 0, 0);
		(*seq)++;
		break;
	case 1:
    if( work->fadeFinishWaitFlag == FIELD_FADE_NO_WAIT ){ return GMEVENT_RES_FINISH; }  // �t�F�[�h�҂��Ȃ�
    if( GFL_FADE_CheckFade() == FALSE ){ return GMEVENT_RES_FINISH; }  // �t�F�[�h����
		break;
	} 
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�P�x�t�F�[�h�C�� �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 * @param	fadeType		       �t�F�[�h�̎�ގw�� ( FADE_TYPE_BLACK or FADE_TYPE_WHITE )
 * @param fadeFinishWaitFlag �t�F�[�h������҂��ǂ���
 * @param BGInitFlag         BG������������ł����Ȃ����ǂ���
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT* EVENT_FieldBrightIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, 
                                     FIELD_FADE_TYPE fadeType, 
                                     FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag,
                                     FIELD_FADE_BG_INIT_FLAG BGInitFlag )
{
	GMEVENT* event;
	FADE_EVENT_WORK* work;

	event = GMEVENT_Create( gameSystem, NULL, FieldBrightInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem          = gameSystem;
  work->fieldmap            = fieldmap;
	work->fadeType            = fadeType;
  work->fadeFinishWaitFlag  = fadeFinishWaitFlag;
  work->brightFadeMode      = GetBrightFadeMode( fadeType );
  work->BGInitFlag          = BGInitFlag;

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ����� �P�x�t�F�[�h�C�� �C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT FlySkyBrightInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work = wk;

	switch( *seq )
  {
	case 0:
    GFL_FADE_SetMasterBrightReq(work->brightFadeMode, 16, 0, work->fadeFinishWaitFlag);
    // BG���[�h�ݒ�ƕ\���ݒ�̕��A
    {
      int mv = GFL_DISP_GetMainVisible();
      FIELDMAP_InitBGMode();
      GFL_DISP_GX_SetVisibleControlDirect( mv );
      FIELDMAP_InitBG(work->fieldmap);
    }
		(*seq)++;
		break;
	case 1:
    if( work->fadeFinishWaitFlag == FIELD_FADE_NO_WAIT ){ return GMEVENT_RES_FINISH; }  // �t�F�[�h�҂��Ȃ�
    if( GFL_FADE_CheckFade() == FALSE ){ return GMEVENT_RES_FINISH; }  // �t�F�[�h����
		break;
	} 
	return GMEVENT_RES_CONTINUE;
} 

//--------------------------------------------------------------------------------------------
/**
 * @brief	����� �P�x�t�F�[�h�C�� �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap
 * @param	fadeType		       �t�F�[�h�̎�ގw�� ( FADE_TYPE_BLACK or FADE_TYPE_WHITE )
 * @param fadeFinishWaitFlag �t�F�[�h������҂��ǂ���
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
GMEVENT* EVENT_FlySkyBrightIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, 
                               FIELD_FADE_TYPE fadeType, FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag )
{
	GMEVENT * event;
	FADE_EVENT_WORK * work;

	event = GMEVENT_Create( gameSystem, NULL, FlySkyBrightInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem         = gameSystem;
  work->fieldmap           = fieldmap;
	work->fadeType           = fadeType;
  work->fadeFinishWaitFlag = fadeFinishWaitFlag;
  work->brightFadeMode     = GetBrightFadeMode( fadeType );

	return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �f�o�b�O�p �����t�F�[�h�C��
 */ 
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT QuickFadeInEvent( GMEVENT* event, int* seq, void* wk )
{
	FADE_EVENT_WORK* work = wk;

  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 
      16, 0, -16);

  // BG���[�h�ݒ�ƕ\���ݒ�̕��A
  {
    int mv = GFL_DISP_GetMainVisible();
    FIELDMAP_InitBGMode();
    GFL_DISP_GX_SetVisibleControlDirect( mv );
  }
  FIELDMAP_InitBG(work->fieldmap);

  return GMEVENT_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p �����A�E�g
 *
 * @param	gameSystem
 * @param	fieldmap
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_QuickFadeIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
	GMEVENT* event;
  FADE_EVENT_WORK* work;

  event = GMEVENT_Create( gameSystem, NULL, QuickFadeInEvent, sizeof(FADE_EVENT_WORK) );
	work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;

  return event;
} 

//--------------------------------------------------------------------------------------------
/**
 * @brief	�t�F�[�h�C�� �C�x���g����
 *
 * @param	gameSystem
 * @param	fieldmap 
 * @param	fadeType		       �t�F�[�h�̎�ގw��
 * @param fadeFinishWaitFlag �t�F�[�h������҂��ǂ��� ( �P�x�t�F�[�h���̂ݗL�� )
 * @param BGInitFlag         BG������������ł����Ȃ����ǂ��� ( �P�x�t�F�[�h���̂ݗL�� )
 * @param startSeason        �ŏ��ɕ\������G�� ( �G�߃t�F�[�h���̂ݗL�� )
 * @param endSeason          �Ō�ɕ\������G�� ( �G�߃t�F�[�h���̂ݗL�� )
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
GMEVENT* EVENT_FieldFadeIn( GAMESYS_WORK *gameSystem, FIELDMAP_WORK * fieldmap, 
                            FIELD_FADE_TYPE fadeType, 
                            FIELD_FADE_WAIT_FLAG fadeFinishWaitFlag, 
                            FIELD_FADE_BG_INIT_FLAG BGInitFlag,
                            u8 startSeason, u8 endSeason )
{
  GMEVENT* event;

  switch( fadeType )
  {
  default:
  case FIELD_FADE_CROSS: // �N���X�t�F�[�h
    event = EVENT_FieldCrossIn( gameSystem, fieldmap );
    break;
  case FIELD_FADE_BLACK: // �P�x�t�F�[�h(�u���b�N)
  case FIELD_FADE_WHITE: // �P�x�t�F�[�h(�z���C�g)
    event = EVENT_FieldBrightIn( gameSystem, fieldmap, fadeType, fadeFinishWaitFlag, BGInitFlag );
    break;
  case FIELD_FADE_SEASON:
    event = EVENT_FieldSeasonIn( gameSystem, fieldmap, startSeason, endSeason );
    break;
  }
	return event; 
}
