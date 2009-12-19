//============================================================================================
/**
 * @file	event_fieldmap_control.c
 * @brief	�C�x���g�F�t�B�[���h�}�b�v����c�[��
 * @author	tamada GAMEFREAK inc.
 * @date	2008.12.10
 */
//============================================================================================

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/fieldmap_call.h"  //FIELDMAP_IsReady,FIELDMAP_ForceUpdate
#include "field/fieldmap.h"

#include "./event_fieldmap_control.h"

#include "system/main.h"      // HEAPID_PROC

#include "event_season_display.h"  // for EVENT_SeasonDisplay
#include "field_status_local.h"  // for FIELD_STATUS

#include "system/screentex.h"

#include "gamesystem/pm_season.h"  // for PMSEASON_xxxx

FS_EXTERN_OVERLAY(pokelist);
FS_EXTERN_OVERLAY(poke_status);

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
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
static GMEVENT* EVENT_FieldBrightIn( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                                     FIELD_FADE_TYPE type, 
                                     FIELD_FADE_SEASON_FLAG season, 
                                     FIELD_FADE_WAIT_TYPE wait )
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldBrightInEvent, sizeof(FADE_EVENT_WORK));
	FADE_EVENT_WORK * few = GMEVENT_GetEventWork(event);
  few->gsys        = gsys;
  few->fieldmap    = fieldmap;
	few->fade_type   = type;
  few->season_flag = season;
  few->wait_type   = wait;
  few->brightFadeMode = GetBrightFadeMode( type );

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
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
GMEVENT* EVENT_FieldFadeIn( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                            FIELD_FADE_TYPE type, 
                            FIELD_FADE_SEASON_FLAG season,
                            FIELD_FADE_WAIT_TYPE wait )
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
    event = EVENT_FieldBrightIn( gsys, fieldmap, type, season, wait );
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


//============================================================================================
// ���t�B�[���h�}�b�v�̊J�n/�I��
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	FIELDMAP_WORK * fieldmap;
	GAMEDATA * gamedata;
}FIELD_OPENCLOSE_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FieldCloseEvent(GMEVENT * event, int * seq, void *work)
{
	FIELD_OPENCLOSE_WORK * focw = work;
	GAMESYS_WORK * gsys = focw->gsys;
	FIELDMAP_WORK * fieldmap = focw->fieldmap;
	switch (*seq) {
	case 0:
		FIELDMAP_Close(fieldmap);
		(*seq) ++;
		break;
	case 1:
		if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_FieldClose(GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap)
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldCloseEvent, sizeof(FIELD_OPENCLOSE_WORK));
	FIELD_OPENCLOSE_WORK * focw = GMEVENT_GetEventWork(event);
	focw->gsys = gsys;
	focw->fieldmap = fieldmap;
	focw->gamedata = GAMESYSTEM_GetGameData(gsys);
	return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FieldOpenEvent(GMEVENT * event, int *seq, void*work)
{
	FIELD_OPENCLOSE_WORK * focw = work;
	GAMESYS_WORK * gsys = focw->gsys;
	FIELDMAP_WORK * fieldmap;	// = focw->fieldmap;
	switch(*seq) {
	case 0:
		if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
#if 0
		GFL_FADE_SetMasterBrightReq(
				GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,
        16, 16, 0);
#endif
		GAMESYSTEM_CallFieldProc(gsys);
		(*seq) ++;
		break;
	case 1:
		fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
		if (FIELDMAP_IsReady(fieldmap) == FALSE) break;
		(*seq) ++;
		break;
	case 2:
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_FieldOpen(GAMESYS_WORK *gsys)
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldOpenEvent, sizeof(FIELD_OPENCLOSE_WORK));
	FIELD_OPENCLOSE_WORK * focw = GMEVENT_GetEventWork(event);
	focw->gsys = gsys;
	focw->fieldmap = NULL;
	focw->gamedata = GAMESYSTEM_GetGameData(gsys);
	return event;
}


//============================================================================================
// ���T�u�v���Z�X�Ăяo��
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct
{
	GAMESYS_WORK*             gsys;
	FIELDMAP_WORK*        fieldmap;
	FSOverlayID              ov_id;
	const GFL_PROC_DATA* proc_data;
	void*                proc_work;

} CHANGE_SAMPLE_WORK;
//------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�
 */
//------------------------------------------------------------------
static GMEVENT_RESULT GameChangeEvent( GMEVENT* event, int* seq, void* work )
{
	CHANGE_SAMPLE_WORK* csw = work;
	GAMESYS_WORK*      gsys = csw->gsys;

	switch( *seq )
  {
	case 0:
    { // �t�F�[�h�A�E�g
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut_Black(gsys, csw->fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
		(*seq) ++;
		break;
	case 1:
    { // �T�u�v���Z�X�Ăяo���C�x���g
      GMEVENT* ev_sub;
      ev_sub = EVENT_FieldSubProcNoFade( csw->gsys, csw->fieldmap, 
                                        csw->ov_id, csw->proc_data, csw->proc_work );
      GMEVENT_CallEvent( event, ev_sub );
    }
		(*seq) ++;
		break;
	case 2:
    { // �t�F�[�h�C��
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, csw->fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
		(*seq) ++;
		break;
	case 3:
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
}
//------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�(�t�F�[�h�Ȃ�)
 */
//------------------------------------------------------------------
static GMEVENT_RESULT GameChangeEvent_NoFade(GMEVENT * event, int * seq, void * work)
{
	CHANGE_SAMPLE_WORK* csw = work;
	GAMESYS_WORK*      gsys = csw->gsys;

	switch( *seq )
  {
	case 0:  // �t�B�[���h�}�b�v�j��
		GMEVENT_CallEvent( event, EVENT_FieldClose(gsys, csw->fieldmap) );
		(*seq) ++;
		break;
	case 1:  // �T�u�v���Z�X�Ăяo��
		GAMESYSTEM_CallProc( gsys, csw->ov_id, csw->proc_data, csw->proc_work );
		(*seq) ++;
		break;
	case 2:  // �T�u�v���Z�X�I���҂�
		if( GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL ) break;
		(*seq) ++;
		break;
	case 3:  // �t�B�[���h�}�b�v�ĊJ
		GMEVENT_CallEvent( event, EVENT_FieldOpen(gsys) );
		(*seq) ++;
		break;
	case 4:
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X�Ăяo���C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	ov_id		  �J�ڂ���T�u�v���Z�X�̃I�[�o�[���C�w��
 * @param	proc_data	�J�ڂ���T�u�v���Z�X�̃v���Z�X�f�[�^�ւ̃|�C���^
 * @param	proc_work	�J�ڂ���T�u�v���Z�X�Ŏg�p���郏�[�N�ւ̃|�C���^
 * @return ���������C�x���g�ւ̃|�C���^
 *
 * �t�F�[�h�A�E�g���t�B�[���h�}�b�v�I�����T�u�v���Z�X�Ăяo��
 * ���t�B�[���h�}�b�v�ĊJ���t�F�[�h�C������������
 */
//------------------------------------------------------------------
GMEVENT* EVENT_FieldSubProc( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap,
		                         FSOverlayID ov_id, 
                             const GFL_PROC_DATA* proc_data, void* proc_work )
{
	GMEVENT* event;
	CHANGE_SAMPLE_WORK* csw; 

  // ����
  event = GMEVENT_Create( gsys, NULL, GameChangeEvent, sizeof(CHANGE_SAMPLE_WORK) );
  // ������
	csw = GMEVENT_GetEventWork( event );
	csw->gsys      = gsys;
	csw->fieldmap  = fieldmap;
	csw->ov_id     = ov_id;
	csw->proc_data = proc_data;
	csw->proc_work = proc_work;
	return event;
}

//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X�Ăяo���C�x���g����(�t�F�[�h�Ȃ�)
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	ov_id		  �J�ڂ���T�u�v���Z�X�̃I�[�o�[���C�w��
 * @param	proc_data	�J�ڂ���T�u�v���Z�X�̃v���Z�X�f�[�^�ւ̃|�C���^
 * @param	proc_work	�J�ڂ���T�u�v���Z�X�Ŏg�p���郏�[�N�ւ̃|�C���^
 * @return ���������C�x���g�ւ̃|�C���^
 *
 * �t�B�[���h�}�b�v�I�� �� �T�u�v���Z�X�Ăяo�� �� �t�B�[���h�}�b�v�ĊJ
 */
//------------------------------------------------------------------
GMEVENT* EVENT_FieldSubProcNoFade( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap,
		                               FSOverlayID ov_id, 
                                   const GFL_PROC_DATA* proc_data, void* proc_work )
{
	GMEVENT* event;
	CHANGE_SAMPLE_WORK* csw; 

  // ����
  event = GMEVENT_Create( gsys, NULL, GameChangeEvent_NoFade, sizeof(CHANGE_SAMPLE_WORK) );
  // ������
	csw = GMEVENT_GetEventWork( event );
	csw->gsys      = gsys;
	csw->fieldmap  = fieldmap;
	csw->ov_id     = ov_id;
	csw->proc_data = proc_data;
	csw->proc_work = proc_work;
	return event;
}


//============================================================================================
//
//	�C�x���g�F�ʉ�ʌĂяo��(�R�[���o�b�N�Ăяo���t��)
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct
{
	GAMESYS_WORK*             gsys;
	FIELDMAP_WORK*        fieldmap;
	FSOverlayID              ov_id;
	const GFL_PROC_DATA* proc_data;
	void*                proc_work;
  void ( *callback )( void* );      // �R�[���o�b�N�֐�
  void*            callback_work;   // �R�[���o�b�N�֐��̈���

} SUBPROC_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT GameChangeEvent_Callback(GMEVENT * event, int * seq, void * work)
{
	SUBPROC_WORK * spw = work;
	GAMESYS_WORK *gsys = spw->gsys;

	switch(*seq) 
  {
	case 0:
    { // �T�u�v���Z�X�Ăяo���C�x���g
      GMEVENT* ev_sub;
      ev_sub = EVENT_FieldSubProc( spw->gsys, spw->fieldmap, 
                                  spw->ov_id, spw->proc_data, spw->proc_work );
      GMEVENT_CallEvent( event, ev_sub );
    }
		(*seq) ++;
		break;
	case 1: // �R�[���o�b�N�֐��Ăяo��
    if( spw->callback ) spw->callback( spw->callback_work );
		(*seq) ++;
    break;
  case 2:  // �R�[���o�b�N���[�N�j��
    if( spw->callback_work ){ GFL_HEAP_FreeMemory( spw->callback_work ); }
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
} 
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT GameChangeEventNoFade_Callback(GMEVENT * event, int * seq, void * work)
{
	SUBPROC_WORK * spw = work;
	GAMESYS_WORK *gsys = spw->gsys;

	switch(*seq) 
  {
	case 0:
    { // �T�u�v���Z�X�Ăяo���C�x���g
      GMEVENT* ev_sub;
      ev_sub = EVENT_FieldSubProcNoFade( spw->gsys, spw->fieldmap, 
                                        spw->ov_id, spw->proc_data, spw->proc_work );
      GMEVENT_CallEvent( event, ev_sub );
    }
		(*seq) ++;
		break;
	case 1: // �R�[���o�b�N�֐��Ăяo��
    if( spw->callback ) spw->callback( spw->callback_work );
		(*seq) ++;
    break;
  case 2:  // �R�[���o�b�N���[�N�j��
    if( spw->callback_work ){ GFL_HEAP_FreeMemory( spw->callback_work ); }
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X�Ăяo���C�x���g����(�R�[���o�b�N�֐��t��)
 * @param	gsys		      GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	    �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	ov_id		      �J�ڂ���T�u�v���Z�X�̃I�[�o�[���C�w��
 * @param	proc_data	    �J�ڂ���T�u�v���Z�X�̃v���Z�X�f�[�^�ւ̃|�C���^
 * @param	proc_work	    �J�ڂ���T�u�v���Z�X�Ŏg�p���郏�[�N�ւ̃|�C���^
 * @param callback      �R�[���o�b�N�֐�
 * @param callback_work �R�[���o�b�N�֐��ɂ킽���|�C���^
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 *
 * �t�F�[�h�A�E�g���t�B�[���h�}�b�v�I�����T�u�v���Z�X�Ăяo��
 * ���t�B�[���h�}�b�v�ĊJ���t�F�[�h�C������������
 */
//------------------------------------------------------------------
GMEVENT * EVENT_FieldSubProc_Callback(
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
		FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void * proc_work,
    void (*callback)(void*), void* callback_work )
{
	GMEVENT *     event = GMEVENT_Create(gsys, NULL, GameChangeEvent_Callback, sizeof(SUBPROC_WORK));
	SUBPROC_WORK *  spw = GMEVENT_GetEventWork(event);
	spw->gsys          = gsys;
	spw->fieldmap      = fieldmap;
	spw->ov_id         = ov_id;
	spw->proc_data     = proc_data;
	spw->proc_work     = proc_work;
  spw->callback      = callback;
  spw->callback_work = callback_work;
	return event;
}

//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X�Ăяo���C�x���g����(�t�F�[�h�Ȃ�, �R�[���o�b�N�֐��t��)
 * @param	gsys		      GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	    �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	ov_id		      �J�ڂ���T�u�v���Z�X�̃I�[�o�[���C�w��
 * @param	proc_data	    �J�ڂ���T�u�v���Z�X�̃v���Z�X�f�[�^�ւ̃|�C���^
 * @param	proc_work	    �J�ڂ���T�u�v���Z�X�Ŏg�p���郏�[�N�ւ̃|�C���^
 * @param callback      �R�[���o�b�N�֐�
 * @param callback_work �R�[���o�b�N�֐��ɂ킽���|�C���^
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 *
 * �t�F�[�h�A�E�g���t�B�[���h�}�b�v�I�����T�u�v���Z�X�Ăяo��
 * ���t�B�[���h�}�b�v�ĊJ���t�F�[�h�C������������
 */
//------------------------------------------------------------------
GMEVENT * EVENT_FieldSubProcNoFade_Callback(
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
		FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void * proc_work,
    void (*callback)(void*), void* callback_work )
{
	GMEVENT*     event = GMEVENT_Create(gsys, NULL, GameChangeEventNoFade_Callback, sizeof(SUBPROC_WORK));
	SUBPROC_WORK*  spw = GMEVENT_GetEventWork(event);
	spw->gsys          = gsys;
	spw->fieldmap      = fieldmap;
	spw->ov_id         = ov_id;
	spw->proc_data     = proc_data;
	spw->proc_work     = proc_work;
  spw->callback      = callback;
  spw->callback_work = callback_work;
	return event;
}



//============================================================================================
//
//	�C�x���g�F�|�P�����I��
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	FIELDMAP_WORK * fieldmap;
  PLIST_DATA* plData;
  PSTATUS_DATA* psData;
}POKE_SELECT_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_PokeSelect(GMEVENT * event, int * seq, void * work)
{
	POKE_SELECT_WORK * psw = work;
	GAMESYS_WORK *gsys = psw->gsys;

  // �V�[�P���X��`
  enum
  {
    SEQ_CALL_POKELIST = 0,
    SEQ_WAIT_POKELIST,
    SEQ_EXIT_POKELIST,
    SEQ_CALL_POKESTATUS,
    SEQ_WAIT_POKESTATUS,
    SEQ_END,
  };

	switch( *seq ) 
  {
	case SEQ_CALL_POKELIST: //// �|�P�������X�g�Ăяo��
		GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(pokelist), &PokeList_ProcData, psw->plData);
    *seq = SEQ_WAIT_POKELIST;
		break;
	case SEQ_WAIT_POKELIST: //// �v���b�N�I���҂�
		if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
    *seq = SEQ_EXIT_POKELIST;
		break;
  case SEQ_EXIT_POKELIST: //// �|�P�������X�g�̏I����Ԃŕ���
    if( psw->plData->ret_mode == PL_RET_NORMAL )   // �I���I��
    {
      *seq = SEQ_END;
    }
    else if( psw->plData->ret_mode == PL_RET_STATUS )  //�u�悳���݂�v��I��
    {
      psw->psData->pos = psw->plData->ret_sel;   // �\������f�[�^�ʒu = �I���|�P
      *seq = SEQ_CALL_POKESTATUS;
    }
    else  // ���Ή��ȍ��ڂ�I��
    {
      OBATA_Printf( "----------------------------------------------------\n" );
      OBATA_Printf( "�|�P�����I���C�x���g: ���Ή��ȍ��ڂ��I������܂����B\n" );
      OBATA_Printf( "----------------------------------------------------\n" );
      *seq = SEQ_END;
    }
    break;
  case SEQ_CALL_POKESTATUS: //// �|�P�����X�e�[�^�X�Ăяo��
		GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(poke_status), &PokeStatus_ProcData, psw->psData);
    *seq = SEQ_WAIT_POKESTATUS;
		break;
	case SEQ_WAIT_POKESTATUS: //// �v���b�N�I���҂�
		if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
    psw->plData->ret_sel = psw->psData->pos;  // �X�e�[�^�X��ʏI�����̃|�P�����ɃJ�[�\�������킹��
    *seq = SEQ_CALL_POKELIST;
		break;
	case SEQ_END: //// �C�x���g�I��
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
}


//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_PokeSelect( 
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    PLIST_DATA* list_data, PSTATUS_DATA* status_data )
{
	GMEVENT* event;
	POKE_SELECT_WORK* psw;

  // �C�x���g����
  event = GMEVENT_Create(gsys, NULL, EVENT_FUNC_PokeSelect, sizeof(POKE_SELECT_WORK));
  psw   = GMEVENT_GetEventWork(event);
	psw->gsys      = gsys;
	psw->fieldmap  = fieldmap;
  psw->plData    = list_data;
  psw->psData    = status_data;
	return event;
}
