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
typedef struct {
  GAMESYS_WORK* gsys;
  FIELDMAP_WORK* fieldmap;

	FIELD_FADE_TYPE fade_type;
  FIELD_FADE_SEASON_FLAG season_flag;
  FIELD_FADE_WAIT_TYPE wait_type; 

  int brightFadeMode;  // �P�x�t�F�[�h���[�h

	int alphaWork;

  BOOL bg_init_flag;

} FADE_EVENT_WORK;

//------------------------------------------------------------------
/**
 * @brief �P�x�t�F�[�h���[�h���擾����
 *
 * @param FIELD_FADE_TYPE �t�F�[�h�^�C�v
 *
 * @return �w��t�F�[�h�^�C�v�ɉ������P�x�t�F�[�h���[�h
 */
//------------------------------------------------------------------
static int GetBrightFadeMode( FIELD_FADE_TYPE type )
{
  int mode;
  switch( type )
  {
  default:
  case FIELD_FADE_BLACK: // �P�x�t�F�[�h(�u���b�N)
    mode = GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB;
    break;
  case FIELD_FADE_WHITE: // �P�x�t�F�[�h(�z���C�g)
    mode = GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN | GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB;
    break;
  }
  return mode;
}


//============================================================================================
// ���t�F�[�h�A�E�g
//============================================================================================ 

//------------------------------------------------------------------
/**
 * @brief �P�x�t�F�[�h�A�E�g�C�x���g
 */
//------------------------------------------------------------------
static GMEVENT_RESULT FieldBrightOutEvent(GMEVENT * event, int *seq, void * work)
{
	FADE_EVENT_WORK * few = work;
	switch (*seq) {
	case 0:
    GFL_FADE_SetMasterBrightReq( few->brightFadeMode, 0, 16, few->wait_type);
		(*seq) ++;
		break;
	case 1:
    if( few->wait_type == FIELD_FADE_NO_WAIT ){ return GMEVENT_RES_FINISH; }
		if( GFL_FADE_CheckFade() == FALSE ){ return GMEVENT_RES_FINISH; }
		break;
	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �N���X�t�F�[�h�A�E�g�C�x���g
 */
//------------------------------------------------------------------
static GMEVENT_RESULT FieldCrossOutEvent(GMEVENT * event, int *seq, void * work)
{
	FADE_EVENT_WORK * few = work;

	switch (*seq)
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

		return GMEVENT_RES_FINISH;
	}

	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �f�o�b�O�p �����A�E�g�C�x���g
 */
//------------------------------------------------------------------
static GMEVENT_RESULT QuickFadeOutEvent(GMEVENT * event, int *seq, void * work)
{
  GFL_FADE_SetMasterBrightReq( 
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 
      0, 16, -16 );
  return GMEVENT_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	�N���X�t�F�[�h�A�E�g�C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @return GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
static GMEVENT* EVENT_FieldCrossOut( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap )
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldCrossOutEvent, sizeof(FADE_EVENT_WORK));
	FADE_EVENT_WORK * few = GMEVENT_GetEventWork(event);
  few->gsys      = gsys;
  few->fieldmap  = fieldmap;
  few->fade_type = FIELD_FADE_CROSS;

	return event;
}

//------------------------------------------------------------------
/**
 * @brief	�u���C�g�l�X�A�E�g�C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		  �t�F�[�h�̎�ގw��
 * @param wait      �t�F�[�h������҂��ǂ���
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
static GMEVENT* EVENT_FieldBrightOut( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                                      FIELD_FADE_TYPE type, 
                                      FIELD_FADE_WAIT_TYPE wait )
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldBrightOutEvent, sizeof(FADE_EVENT_WORK));
	FADE_EVENT_WORK * few = GMEVENT_GetEventWork(event);
  few->gsys        = gsys;
  few->fieldmap    = fieldmap;
  few->fade_type   = type;
  few->wait_type   = wait;
  few->brightFadeMode = GetBrightFadeMode( type );

	return event;
}

//------------------------------------------------------------------
/**
 * @brief	�t�F�[�h�A�E�g�C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		  �t�F�[�h�̎�ގw��
 * @param wait      �t�F�[�h������҂��ǂ���(�P�x�t�F�[�h���̂ݗL��)
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
GMEVENT* EVENT_FieldFadeOut( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                             FIELD_FADE_TYPE type, 
                             FIELD_FADE_WAIT_TYPE wait ) 
{ 
  GMEVENT* event;

  // �t�F�[�h�C�x���g����
  switch( type )
  {
  default:
  case FIELD_FADE_CROSS: // �N���X�t�F�[�h
    event = EVENT_FieldCrossOut( gsys, fieldmap );
    break;
  case FIELD_FADE_BLACK: // �P�x�t�F�[�h(�u���b�N)
  case FIELD_FADE_WHITE: // �P�x�t�F�[�h(�z���C�g)
    event = EVENT_FieldBrightOut( gsys, fieldmap, type, wait );
    break;
  }
	return event;
}

//------------------------------------------------------------------
/**
 * @brief	����ԃu���C�g�l�X�A�E�g�C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		  �t�F�[�h�̎�ގw��
 * @param wait      �t�F�[�h������҂��ǂ���
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
GMEVENT* EVENT_FlySkyBrightOut( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                                FIELD_FADE_TYPE type, FIELD_FADE_WAIT_TYPE wait )
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldBrightOutEvent, sizeof(FADE_EVENT_WORK));
	FADE_EVENT_WORK * few = GMEVENT_GetEventWork(event);
  few->gsys      = gsys;
  few->fieldmap  = fieldmap;
	few->fade_type = type;
  few->wait_type = wait;
  few->brightFadeMode = GetBrightFadeMode( type );

	return event;
}

//------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p �����A�E�g
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_QuickFadeOut( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap)
{
	GMEVENT* event;
	FADE_EVENT_WORK* few;

  event = GMEVENT_Create(gsys, NULL, QuickFadeOutEvent, sizeof(FADE_EVENT_WORK));
	few = GMEVENT_GetEventWork(event);
  few->gsys     = gsys;
  few->fieldmap = fieldmap;
  return event;
}


//============================================================================================
// ���t�F�[�h�C��
//============================================================================================ 

//------------------------------------------------------------------
/**
 * @brief �N���X�t�F�[�h�C���C�x���g
 */
//------------------------------------------------------------------
static GMEVENT_RESULT FieldCrossInEvent(GMEVENT * event, int *seq, void * work)
{
	FADE_EVENT_WORK* few = work;

	switch (*seq)
  {
	case 0:
    few->alphaWork = 16;
    GFL_BG_SetPriority(BG_FRAME_CROSS_FADE, 0);
    GFL_BG_SetPriority(FLDBG_MFRM_3D, 1);
    GFL_BG_SetVisible( FLDBG_MFRM_3D, VISIBLE_ON );
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG0, few->alphaWork, 0 );

		// �T�u��ʋP�x���A
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, -2);
    ++ *seq;
    break;

	case 1:	// �N���X�t�F�[�h
		if(few->alphaWork){
			few->alphaWork--;
			G2_ChangeBlendAlpha( few->alphaWork, 16 - few->alphaWork );
		} else {
			GFL_BG_SetVisible( BG_FRAME_CROSS_FADE, VISIBLE_OFF );

			//VRAM�N���A
			GX_SetBankForLCDC(GX_VRAM_LCDC_D);
			MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_VRAM_D_SIZE);
			(void)GX_DisableBankForLCDC();

			// ���C��BG�ւ̊��蓖�ĕ��A(fieldmap.c�Ɛ��������Ƃ邱��)
			GX_SetBankForBG(GX_VRAM_LCDC_D);	
			++ *seq;
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
		++ *seq;
		break;

	case 3:	// �N���X�t�F�[�hEND
		FIELDMAP_InitBG(few->fieldmap);
		return GMEVENT_RES_FINISH;
	} 
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �P�x�t�F�[�h�C���C�x���g
 */ 
//------------------------------------------------------------------
static GMEVENT_RESULT FieldBrightInEvent(GMEVENT * event, int *seq, void * work)
{
	FADE_EVENT_WORK* few = work;
	switch (*seq) {
	case 0:
    { 
      GAMEDATA*       gdata = GAMESYSTEM_GetGameData( few->gsys );
      FIELD_STATUS* fstatus = GAMEDATA_GetFieldStatus( gdata );

      if( FIELD_STATUS_GetSeasonDispFlag(fstatus) )  // if(�G�ߕ\���t���OON)
      { // �l�G�\��
        u8 start, end;
        start = (FIELD_STATUS_GetSeasonDispLast( fstatus ) + 1) % PMSEASON_TOTAL;
        end   = GAMEDATA_GetSeasonID( gdata );
        GMEVENT_CallEvent( event, EVENT_SeasonDisplay( few->gsys, few->fieldmap, start, end ) );
        FIELD_STATUS_SetSeasonDispFlag( fstatus, FALSE );
        FIELD_STATUS_SetSeasonDispLast( fstatus, end );
        *seq = 2;
        break;
      }
      else
      {
        GFL_FADE_SetMasterBrightReq(few->brightFadeMode, 16, 0, 0);

        if ( few->bg_init_flag )
        {
          // BG���[�h�ݒ�ƕ\���ݒ�̕��A
          {
            int mv = GFL_DISP_GetMainVisible();
            FIELDMAP_InitBGMode();
            GFL_DISP_GX_SetVisibleControlDirect( mv );
          }
          FIELDMAP_InitBG(few->fieldmap);
        }
      }
    }
		(*seq) ++;
		break;
	case 1:
    {
      BOOL rc = FALSE;
      if( few->wait_type == FIELD_FADE_NO_WAIT ) rc = TRUE;
		  if( GFL_FADE_CheckFade() == FALSE ) rc = TRUE; 
      if( rc ){ (*seq) ++; }
    }
		break;
  case 2:
    return GMEVENT_RES_FINISH;
	}

	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief ����ԋP�x�t�F�[�h�A�E�g�C�x���g
 */
//------------------------------------------------------------------
static GMEVENT_RESULT FlySkyBrightInEvent(GMEVENT * event, int *seq, void * work)
{
	FADE_EVENT_WORK* few = work;
	switch (*seq) {
	case 0:
    { 
      GAMEDATA*       gdata = GAMESYSTEM_GetGameData( few->gsys );
      FIELD_STATUS* fstatus = GAMEDATA_GetFieldStatus( gdata );
      // if(�l�G�\���w��L && �G�ߕ\���t���OON)
      if( (few->season_flag == FIELD_FADE_SEASON_ON) && FIELD_STATUS_GetSeasonDispFlag(fstatus) )  
      { // �l�G�\��
        u8 start, end;
        start = (FIELD_STATUS_GetSeasonDispLast( fstatus ) + 1) % PMSEASON_TOTAL;
        end   = GAMEDATA_GetSeasonID( gdata );
        GMEVENT_CallEvent( event, EVENT_SeasonDisplay( few->gsys, few->fieldmap, start, end ) );
        FIELD_STATUS_SetSeasonDispFlag( fstatus, FALSE );
        FIELD_STATUS_SetSeasonDispLast( fstatus, end );
        *seq = 2;
        break;
      }
      else
      {
        GFL_FADE_SetMasterBrightReq(few->brightFadeMode, 16, 0, few->wait_type);
        // BG���[�h�ݒ�ƕ\���ݒ�̕��A
        {
          int mv = GFL_DISP_GetMainVisible();
          FIELDMAP_InitBGMode();
          GFL_DISP_GX_SetVisibleControlDirect( mv );
        }
        FIELDMAP_InitBG(few->fieldmap);
      }
    }
		(*seq) ++;
		break;
	case 1:
    {
      BOOL rc = FALSE;
      if( few->wait_type == FIELD_FADE_NO_WAIT ) rc = TRUE;
		  if( GFL_FADE_CheckFade() == FALSE ) rc = TRUE; 
      if( rc ){ (*seq) ++; }
    }
		break;
  case 2:
    return GMEVENT_RES_FINISH;
	}

	return GMEVENT_RES_CONTINUE;
} 

//------------------------------------------------------------------
/**
 * @brief �f�o�b�O�p �����t�F�[�h�C��
 */ 
//------------------------------------------------------------------
static GMEVENT_RESULT QuickFadeInEvent(GMEVENT * event, int *seq, void * work)
{
	FADE_EVENT_WORK* few = work;

  GFL_FADE_SetMasterBrightReq(
      GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 
      16, 0, -16);

  // BG���[�h�ݒ�ƕ\���ݒ�̕��A
  {
    int mv = GFL_DISP_GetMainVisible();
    FIELDMAP_InitBGMode();
    GFL_DISP_GX_SetVisibleControlDirect( mv );
  }
  FIELDMAP_InitBG(few->fieldmap);

  return GMEVENT_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	�N���X�t�F�[�h�C���C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap  �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
static GMEVENT* EVENT_FieldCrossIn( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap )
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldCrossInEvent, sizeof(FADE_EVENT_WORK));
	FADE_EVENT_WORK * few = GMEVENT_GetEventWork(event);
  few->gsys      = gsys;
  few->fieldmap  = fieldmap;
  few->fade_type = FIELD_FADE_CROSS;

	return event;
}

//------------------------------------------------------------------
/**
 * @brief	�u���C�g�l�X�C���C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap  �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		  �t�F�[�h�̎�ގw��
 * @param season    �G�ߕ\���������邩�ǂ���
 * @param wait      �t�F�[�h������҂��ǂ���
 * @param bg_init_flag  BG������������ł����Ȃ����ǂ���
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
static GMEVENT* EVENT_FieldBrightIn( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                                     FIELD_FADE_TYPE type, 
                                     FIELD_FADE_SEASON_FLAG season, 
                                     FIELD_FADE_WAIT_TYPE wait,
                                     BOOL bg_init_flag )
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldBrightInEvent, sizeof(FADE_EVENT_WORK));
	FADE_EVENT_WORK * few = GMEVENT_GetEventWork(event);
  few->gsys        = gsys;
  few->fieldmap    = fieldmap;
	few->fade_type   = type;
  few->season_flag = season;
  few->wait_type   = wait;
  few->brightFadeMode = GetBrightFadeMode( type );
  few->bg_init_flag = bg_init_flag;

	return event;
}

//------------------------------------------------------------------
/**
 * @brief	�t�F�[�h�C���C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap  �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		  �t�F�[�h�̎�ގw��
 * @param season    �G�ߕ\���������邩�ǂ���(�P�x�t�F�[�h���̂ݗL��)
 * @param wait      �t�F�[�h������҂��ǂ���(�P�x�t�F�[�h���̂ݗL��)
 * @param bg_init_flag  BG������������ł����Ȃ����ǂ���(�P�x�t�F�[�h���̂ݗL��)
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
GMEVENT* EVENT_FieldFadeIn( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                            FIELD_FADE_TYPE type, 
                            FIELD_FADE_SEASON_FLAG season,
                            FIELD_FADE_WAIT_TYPE wait,
                            BOOL bg_init_flag )
{
  GMEVENT* event;

  // �t�F�[�h�C�x���g����
  switch( type )
  {
  default:
  case FIELD_FADE_CROSS: // �N���X�t�F�[�h
    event = EVENT_FieldCrossIn( gsys, fieldmap );
    break;
  case FIELD_FADE_BLACK: // �P�x�t�F�[�h(�u���b�N)
  case FIELD_FADE_WHITE: // �P�x�t�F�[�h(�z���C�g)
    event = EVENT_FieldBrightIn( gsys, fieldmap, type, season, wait, bg_init_flag );
    break;
  }
	return event; 
}

//------------------------------------------------------------------
/**
 * @brief	����ԃu���C�g�l�X�C���C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap  �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		  �t�F�[�h�̎�ގw��
 * @param wait      �t�F�[�h������҂��ǂ���
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
GMEVENT * EVENT_FlySkyBrightIn( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                               FIELD_FADE_TYPE type, FIELD_FADE_WAIT_TYPE wait )
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FlySkyBrightInEvent, sizeof(FADE_EVENT_WORK));
	FADE_EVENT_WORK * few = GMEVENT_GetEventWork(event);
  few->gsys      = gsys;
  few->fieldmap  = fieldmap;
	few->fade_type = type;
  few->wait_type = wait;
  few->brightFadeMode = GetBrightFadeMode( type );

	return event;
}

//------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p �����A�E�g
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_QuickFadeIn( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap)
{
	GMEVENT* event;
  FADE_EVENT_WORK* few;

  event = GMEVENT_Create(gsys, NULL, QuickFadeInEvent, sizeof(FADE_EVENT_WORK));
	few = GMEVENT_GetEventWork(event);
  few->gsys      = gsys;
  few->fieldmap = fieldmap;
  return event;
}


