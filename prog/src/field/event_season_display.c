////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �G�ߕ\���C�x���g
 * @file   event_season_display.c
 * @author obata
 * @date   2009.11.25
 */
////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "fieldmap.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "event_season_display.h"

#include "system/main.h"           // for HEAPID_xxxx
#include "gamesystem/pm_season.h"  // for PMSEASON_xxxx
#include "arc/arc_def.h"           // for ARCID_SEASON_DISPLAY
#include "arc/season_display.naix" // for datid


extern void FIELDMAP_InitBGMode( void );
extern void FIELDMAP_InitBG( FIELDMAP_WORK* fieldWork );



//==============================================================================
// ��BG
//==============================================================================
#define BG_FRAME_NUM       (8)                       // BG�t���[����(���C���{�T�u)
#define BG_FRAME_FIELD     (GFL_BG_FRAME0_M)         // �t�B�[���h��\�����Ă���BG��
#define BG_FRAME_SEASON    (GFL_BG_FRAME3_M)         // �G�ߕ\���Ɏg�p����BG��
#define BG_PRIORITY_OTHER  (3)                       // �g�p���Ȃ�BG�ʂ̕\���D�揇��
#define BG_PRIORITY_FIELD  (1)                       // �t�B�[���h�\���ʂ̕\���D�揇��
#define BG_PRIORITY_SEASON (0)                       // �G�ߕ\���ʂ̕\���D�揇��
#define ALPHA_MASK_SEASON  (GX_BLEND_PLANEMASK_BG3)  // ���u�����f�B���O���Ώۖ�(�G�ߕ\��)
#define ALPHA_MASK_FIELD   (GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ)  // ���u�����f�B���O���Ώۖ�(�t�B�[���h)
#define ALPHA_MAX          (16)                      // ���ő�l
#define ALPHA_MIN          (0)                       // ���ŏ��l
#define BRIGHT_MAX         (0)                       // �P�x�ő�l
#define BRIGHT_MIN         (-16)                     // �P�x�ŏ��l

// BG�w�b�_
static const GFL_BG_SYS_HEADER BGHeader = {
	GX_DISPMODE_GRAPHICS, 
  GX_BGMODE_0, 
  GX_BGMODE_0, 
  GX_BG0_AS_3D
};
// BG�R���g���[��
static const GFL_BG_BGCNT_HEADER BGControl = 
{
  0, 0,					          // �����\���ʒu
  0x800,						      // �X�N���[���o�b�t�@�T�C�Y
  0,							        // �X�N���[���o�b�t�@�I�t�Z�b�g
  GFL_BG_SCRSIZ_256x256,	// �X�N���[���T�C�Y
  GX_BG_COLORMODE_16,			// �J���[���[�h
  GX_BG_SCRBASE_0x0800,		// �X�N���[���x�[�X�u���b�N
  GX_BG_CHARBASE_0x04000,	// �L�����N�^�x�[�X�u���b�N
  GFL_BG_CHRSIZ_256x256,	// �L�����N�^�G���A�T�C�Y
  GX_BG_EXTPLTT_01,			  // BG�g���p���b�g�X���b�g�I��
  BG_PRIORITY_SEASON,     // �\���v���C�I���e�B�[
  0,							        // �G���A�I�[�o�[�t���O
  0,							        // DUMMY
  FALSE,						      // ���U�C�N�ݒ�
}; 

//--------------------------------------------------------------
/// �f�B�X�v���C���f�[�^
//--------------------------------------------------------------
static const GFL_DISP_VRAM dispVram =
{
	GX_VRAM_BG_128_D, //���C��2D�G���W����BG�Ɋ��蓖�� 
	GX_VRAM_BGEXTPLTT_NONE, //���C��2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_SUB_BG_32_H, //�T�u2D�G���W����BG�Ɋ��蓖��
	GX_VRAM_SUB_BGEXTPLTT_NONE, //�T�u2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_OBJ_64_E, //���C��2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_OBJEXTPLTT_NONE, //���C��2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_SUB_OBJ_16_I, //�T�u2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_SUB_OBJEXTPLTT_NONE, //�T�u2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_TEX_012_ABC, //�e�N�X�`���C���[�W�X���b�g�Ɋ��蓖��
	GX_VRAM_TEXPLTT_0_G, //�e�N�X�`���p���b�g�X���b�g�Ɋ��蓖��
	GX_OBJVRAMMODE_CHAR_1D_64K, // ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_32K, // �T�uOBJ�}�b�s���O���[�h
};


//==============================================================================
// ���萔
//==============================================================================
// �G�ߑJ�ڂ̕\�� ( EVENT_MODE_MAPCHANGE )
#define MAPCHANGE_FI_FRAME_SHORT   (10)  // �t�F�[�h�C������
#define MAPCHANGE_WAIT_FRAME_SHORT (30)  // �ҋ@����
#define MAPCHANGE_FO_FRAME_SHORT   (10)  // �t�F�[�h�A�E�g����
// ���G�߂̕\�� ( EVENT_MODE_MAPCHANGE )
#define MAPCHANGE_FI_FRAME_LONG   (10)  // �t�F�[�h�C������
#define MAPCHANGE_WAIT_FRAME_LONG (60)  // �ҋ@����
#define MAPCHANGE_FO_FRAME_LONG   (20)  // �t�F�[�h�A�E�g����
// ���G�߂̕\�� ( EVENT_MODE_GAMESTART )
#define GAMESTART_FI_FRAME   (30)  // �t�F�[�h�C������
#define GAMESTART_WAIT_FRAME (120) // �ҋ@����
#define GAMESTART_FO_FRAME   (60)  // �t�F�[�h�A�E�g����

// �C�x���g�V�[�P���X
enum {
  SEQ_INIT,    // ������
  SEQ_PREPARE, // �\������
  SEQ_FADEIN,  // �t�F�[�h�C��
  SEQ_WAIT,    // �ҋ@
  SEQ_FADEOUT, // �t�F�[�h�A�E�g
  SEQ_EXIT,    // �I��
};

// ���샂�[�h
typedef enum {
  EVENT_MODE_MAPCHANGE, // �}�b�v�`�F���W�Ƃ��Ă̋G�ߕ\��
  EVENT_MODE_GAMESTART, //�w�������傩��x�ŃQ�[�����n�߂��Ƃ��̋G�ߕ\��
} EVENT_MODE;


//==============================================================================
// ���C�x���g���[�N
//==============================================================================
typedef struct {
  GAMESYS_WORK* gameSystem;
  FIELDMAP_WORK* fieldmap;
  HEAPID heapID;

  EVENT_MODE mode;       // ���샂�[�h
  BOOL       skipFlag;   // �X�L�b�v���邩�ǂ���
  u8         endSeason;  // �\������Ō�̋G��
  u8         dispSeason; // �\�����̋G��
  u32        count;      // �J�E���^
  u32        maxCount;   // �J�E���^�ő�l
} EVENT_WORK;


//==============================================================================
// ���֐��C���f�b�N�X
//==============================================================================
static void SetupBG( EVENT_WORK* work ); // BG ���Z�b�g�A�b�v����
static void SetupBG_MAPCHANGE( EVENT_WORK* work ); // BG ���Z�b�g�A�b�v���� ( EVENT_MODE_MAPCHANGE )
static void SetupBG_GAMESTART( EVENT_WORK* work ); // BG ���Z�b�g�A�b�v���� ( EVENT_MODE_GAMESTART )
static void CleanUpBG( EVENT_WORK* work ); // BG ���N���[���A�b�v����
static void CleanUpBG_MAPCHANGE( EVENT_WORK* work ); // BG ���N���[���A�b�v���� ( EVENT_MODE_MAPCHANGE )
static void CleanUpBG_GAMESTART( EVENT_WORK* work ); // BG ���N���[���A�b�v���� ( EVENT_MODE_GAMESTART )
static void LoadGraphicData( u8 season, HEAPID heap_id ); // �O���t�B�b�N�f�[�^��ǂݍ���
static void UpdateAlpha( const EVENT_WORK* work, int seq ); // ���l���X�V����
static void UpdateAlpha_MAPCHANGE( const EVENT_WORK* work, int seq ); // ���l���X�V���� ( EVENT_MODE_MAPCHANGE )
static void UpdateAlpha_GAMESTART( const EVENT_WORK* work, int seq ); // ���l���X�V���� ( EVENT_MODE_GAMESTART )
static int GetNextSeq( const EVENT_WORK* work, int seq ); // ���̃V�[�P���X���擾����
static void SetSeq( EVENT_WORK* work, int* seq, int next ); // �V�[�P���X��ݒ肷��
static void StepNextSeq( EVENT_WORK* work, int* seq ); // �V�[�P���X��ύX����
static void EventInit( EVENT_WORK* work ); // �C�x���g�J�n���̏���
static void EventExit( EVENT_WORK* work ); // �C�x���g�I�����̏���
static EVENT_MODE GetMode( const EVENT_WORK* work ); // ���샂�[�h���擾����
static BOOL IsSkipOn( const EVENT_WORK* work ); // �X�L�b�v���邩�ǂ������擾����
static void SetSkipOn( EVENT_WORK* work ); // �X�L�b�vON�ɐݒ肷��
static BOOL CheckCountOver( const EVENT_WORK* work ); // �J�E���^���ő�l���I�[�o�[�������ǂ����𔻒肷��
static int GetMaxCountOfFadeIn( const EVENT_WORK* work ); // �t�F�[�h�C���̍ő�t���[�������擾����
static int GetMaxCountOfFadeOut( const EVENT_WORK* work ); // �t�F�[�h�A�E�g�̍ő�t���[�������擾����
static int GetMaxCountOfWait( const EVENT_WORK* work ); // �ҋ@�V�[�P���X�̍ő�t���[�������擾����


//==============================================================================
// ���C�x���g�����֐�
//==============================================================================
static GMEVENT_RESULT SeasonDisplay( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  // �V�[�P���X�ŗL����
  switch( *seq ) {
  // ������
  case SEQ_INIT:
    EventInit( work );
    break;

  // �\������
  case SEQ_PREPARE:
    break;

  // �t�F�[�h�C��
  case SEQ_FADEIN:
    UpdateAlpha( work, *seq );  // ���l�X�V
    break;

  // �ҋ@
  case SEQ_WAIT:
    break;

  // �t�F�[�h�A�E�g
  case SEQ_FADEOUT:
    UpdateAlpha( work, *seq );  // ���l�X�V
    break;

  // �I��
  case SEQ_EXIT:
    EventExit( work );
    return GMEVENT_RES_FINISH;
  }

  // ���ʏ���
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ) {
    // �}�b�v�`�F���W���[�h�̓X�L�b�v��
    if( GetMode(work) == EVENT_MODE_MAPCHANGE ) { SetSkipOn(work); }
  }
  work->count++;
  StepNextSeq( work, seq ); 
  return GMEVENT_RES_CONTINUE;
}


//==============================================================================
// ���O�����J�֐�
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief �l�G�\���C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param fieldmap 
 * @param start    �J�n�G��
 * @param end      �ŏI�G�� 
 *
 * @return �}�b�v�`�F���W�Ƃ��ċ@�\����l�G�\���C�x���g
 */
//------------------------------------------------------------------------------
GMEVENT* EVENT_SeasonDisplay( 
    GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, u8 start, u8 end )
{
  GMEVENT* event;
  EVENT_WORK* work;
  int* seq;

  // �C�x���g����
  event = GMEVENT_Create( gameSystem, NULL, SeasonDisplay, sizeof(EVENT_WORK) ); 
  work  = GMEVENT_GetEventWork( event );
  seq   = GMEVENT_GetSequenceWork( event );

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = fieldmap;
  work->heapID     = FIELDMAP_GetHeapID( fieldmap );
  work->mode       = EVENT_MODE_MAPCHANGE;
  work->skipFlag   = FALSE;
  work->endSeason  = end;
  work->dispSeason = PMSEASON_GetPrevSeasonID( start );

  // �V�[�P���X������
  SetSeq( work, seq, SEQ_INIT );

  return event;
}

//------------------------------------------------------------------------------
/**
 * @brief �l�G�\���C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param start    �J�n�G��
 * @param end      �ŏI�G�� 
 *
 * @return �P�Ɠ����z�肵���l�G�\���C�x���g
 */
//------------------------------------------------------------------------------
GMEVENT* EVENT_SimpleSeasonDisplay( GAMESYS_WORK* gameSystem, u8 start, u8 end )
{
  GMEVENT* event;
  EVENT_WORK* work;
  int* seq;

  // �C�x���g����
  event = GMEVENT_Create( gameSystem, NULL, SeasonDisplay, sizeof(EVENT_WORK) ); 
  work  = GMEVENT_GetEventWork( event );
  seq   = GMEVENT_GetSequenceWork( event );

  // �C�x���g���[�N������
  work->gameSystem = gameSystem;
  work->fieldmap   = NULL;
  work->heapID     = HEAPID_PROC;
  work->mode       = EVENT_MODE_GAMESTART;
  work->skipFlag   = FALSE;
  work->endSeason  = end;
  work->dispSeason = PMSEASON_GetPrevSeasonID( start );

  // �V�[�P���X������
  SetSeq( work, seq, SEQ_INIT );

  return event;
}


//==============================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief BG�Ɋւ��鏉���ݒ���s��
 *
 * @param work �C�x���g���[�N
 */
//------------------------------------------------------------------------------
static void SetupBG( EVENT_WORK* work )
{ 
  switch( GetMode(work) ) {
  case EVENT_MODE_MAPCHANGE: SetupBG_MAPCHANGE( work ); break;
  case EVENT_MODE_GAMESTART: SetupBG_GAMESTART( work ); break;
  default: GF_ASSERT(0);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief BG�Ɋւ��鏉���ݒ���s�� ( EVENT_MODE_MAPCHANGE )
 *
 * @param work �C�x���g���[�N
 */
//------------------------------------------------------------------------------
static void SetupBG_MAPCHANGE( EVENT_WORK* work )
{ 
  // �s���Ăяo��
  GF_ASSERT( GetMode(work) == EVENT_MODE_MAPCHANGE );

  // BG���[�h�ݒ�ƕ\���ݒ�̕��A
  {
    int mv = GFL_DISP_GetMainVisible();
    FIELDMAP_InitBGMode();  // ���̒��ŕ\���ݒ肪�N���A�����
    GFL_DISP_GX_SetVisibleControlDirect( mv );
  }

  // �v���C�I���e�B��ݒ�
  GFL_BG_SetPriority( BG_FRAME_FIELD,  BG_PRIORITY_FIELD );
  GFL_BG_SetPriority( BG_FRAME_SEASON, BG_PRIORITY_SEASON ); 
  GFL_BG_SetPriority( GFL_BG_FRAME1_M, BG_PRIORITY_OTHER ); 
  GFL_BG_SetPriority( GFL_BG_FRAME2_M, BG_PRIORITY_OTHER ); 

  // �L����VRAM�E�X�N���[��VRAM���N���A
  GFL_BG_ClearFrame( BG_FRAME_SEASON ); 
}

//------------------------------------------------------------------------------
/**
 * @brief BG�Ɋւ��鏉���ݒ���s�� ( EVENT_MODE_GAMESTART )
 *
 * @param work �C�x���g���[�N
 */
//------------------------------------------------------------------------------
static void SetupBG_GAMESTART( EVENT_WORK* work )
{ 
  // �s���Ăяo��
  GF_ASSERT( GetMode(work) == EVENT_MODE_GAMESTART );

  GFL_DISP_SetBank( &dispVram );
  GFL_BG_Init( work->heapID );
  GFL_BG_SetBGMode( &BGHeader );
  GFL_BG_SetBGControl( BG_FRAME_SEASON, &BGControl, GFL_BG_MODE_TEXT );

  // �v���C�I���e�B��ݒ�
  GFL_BG_SetPriority( BG_FRAME_FIELD,  BG_PRIORITY_FIELD );
  GFL_BG_SetPriority( BG_FRAME_SEASON, BG_PRIORITY_SEASON ); 
  GFL_BG_SetPriority( GFL_BG_FRAME1_M, BG_PRIORITY_OTHER ); 
  GFL_BG_SetPriority( GFL_BG_FRAME2_M, BG_PRIORITY_OTHER ); 

  // �L����VRAM�E�X�N���[��VRAM���N���A
  GFL_BG_ClearFrame( BG_FRAME_SEASON ); 
}

//------------------------------------------------------------------------------
/**
 * @brief BG�Ɋւ����n�����s��
 *
 * @param work
 */
//------------------------------------------------------------------------------
static void CleanUpBG( EVENT_WORK* work )
{
  switch( GetMode(work) ) {
  case EVENT_MODE_MAPCHANGE: CleanUpBG_MAPCHANGE( work ); break;
  case EVENT_MODE_GAMESTART: CleanUpBG_GAMESTART( work ); break;
  default: GF_ASSERT(0);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief BG�Ɋւ����n�����s�� ( EVENT_MODE_MAPCHANGE )
 *
 * @param work
 */
//------------------------------------------------------------------------------
static void CleanUpBG_MAPCHANGE( EVENT_WORK* work )
{
  // �s���Ăяo��
  GF_ASSERT( GetMode(work) == EVENT_MODE_MAPCHANGE );
}

//------------------------------------------------------------------------------
/**
 * @brief BG�Ɋւ����n�����s�� ( EVENT_MODE_GAMESTART )
 *
 * @param work
 */
//------------------------------------------------------------------------------
static void CleanUpBG_GAMESTART( EVENT_WORK* work )
{
  // �s���Ăяo��
  GF_ASSERT( GetMode(work) == EVENT_MODE_GAMESTART );

  GFL_BG_FreeBGControl( BG_FRAME_SEASON );
}

//------------------------------------------------------------------------------
/**
 * @brief �w�肵���G�߂̕\���ɕK�v�ȃO���t�B�b�N�f�[�^��ǂݍ���
 *
 * @param season  �ǂݍ��݂��s���G��
 * @param haep_id �g�p����q�[�vID
 */
//------------------------------------------------------------------------------
static void LoadGraphicData( u8 season, HEAPID heap_id )
{
  ARCDATID datid_pltt, datid_char, datid_scrn;

  // �f�[�^ID������
  switch( season ) {
  default:
  case PMSEASON_SPRING:
    datid_pltt = NARC_season_display_spring_nclr;
    datid_char = NARC_season_display_spring_ncgr;
    datid_scrn = NARC_season_display_spring_nscr;
    break;
  case PMSEASON_SUMMER:
    datid_pltt = NARC_season_display_summer_nclr;
    datid_char = NARC_season_display_summer_ncgr;
    datid_scrn = NARC_season_display_summer_nscr;
    break;
  case PMSEASON_AUTUMN:
    datid_pltt = NARC_season_display_autumn_nclr;
    datid_char = NARC_season_display_autumn_ncgr;
    datid_scrn = NARC_season_display_autumn_nscr;
    break;
  case PMSEASON_WINTER:
    datid_pltt = NARC_season_display_winter_nclr;
    datid_char = NARC_season_display_winter_ncgr;
    datid_scrn = NARC_season_display_winter_nscr;
    break;
  }

  // �ǂݍ���
  {
    ARCHANDLE* handle;
    handle = GFL_ARC_OpenDataHandle( ARCID_SEASON_DISPLAY, heap_id ); 
    { // �p���b�g�f�[�^
      void* src;
      NNSG2dPaletteData* pltt_data;
      src = GFL_ARC_LoadDataAllocByHandle( handle, datid_pltt, heap_id );
      NNS_G2dGetUnpackedPaletteData( src, &pltt_data );
      GFL_BG_LoadPalette( BG_FRAME_SEASON, pltt_data->pRawData, 0x20, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    { // �L�����N�^�f�[�^
      void* src;
      NNSG2dCharacterData* char_data;
      src = GFL_ARC_LoadDataAllocByHandle( handle, datid_char, heap_id );
      NNS_G2dGetUnpackedBGCharacterData( src, &char_data );
      GFL_BG_LoadCharacter( BG_FRAME_SEASON, char_data->pRawData, char_data->szByte, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    { // �X�N���[���f�[�^
      void* src;
      NNSG2dScreenData* scrn_data;
      src = GFL_ARC_LoadDataAllocByHandle( handle, datid_scrn, heap_id );
      NNS_G2dGetUnpackedScreenData( src, &scrn_data );
      GFL_BG_LoadScreen( BG_FRAME_SEASON, scrn_data->rawData, scrn_data->szByte, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    GFL_ARC_CloseDataHandle( handle );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief ���u�����f�B���O���X�V����
 *
 * @param work �C�x���g���[�N
 * @param seq  ���݂̃V�[�P���X
 */
//------------------------------------------------------------------------------
static void UpdateAlpha( const EVENT_WORK* work, int seq )
{
  switch( GetMode(work) ) {
  case EVENT_MODE_MAPCHANGE: UpdateAlpha_MAPCHANGE( work, seq ); break;
  case EVENT_MODE_GAMESTART: UpdateAlpha_GAMESTART( work, seq ); break;
  default: GF_ASSERT(0);
  }
}
//------------------------------------------------------------------------------
/**
 * @brief ���u�����f�B���O���X�V���� ( EVENT_MODE_MAPCHANGE )
 *
 * @param work �C�x���g���[�N
 * @param seq  ���݂̃V�[�P���X
 */
//------------------------------------------------------------------------------
static void UpdateAlpha_MAPCHANGE( const EVENT_WORK* work, int seq )
{
  // �s���Ăяo��
  GF_ASSERT( GetMode(work) == EVENT_MODE_MAPCHANGE );

  // �t�F�[�h�C��
  if( seq == SEQ_FADEIN ) { 
    float rate   = (float)work->count / (float)work->maxCount;
    int   bright = BRIGHT_MIN + (BRIGHT_MAX - BRIGHT_MIN) * rate;
    G2_SetBlendBrightness( ALPHA_MASK_SEASON, bright );
  }
  // �t�F�[�h�A�E�g
  else if( seq == SEQ_FADEOUT ) { 
    // �Ō�̃t�F�[�h�A�E�g
    if( work->dispSeason == work->endSeason ) { 
      // �Ō�̃t�F�[�h�̓��u�����h
      {
        float rate   = (float)work->count / (float)work->maxCount;
        int   alpha2 = ALPHA_MAX * rate;
        int   alpha1 = ALPHA_MAX - alpha2;
        G2_SetBlendAlpha( ALPHA_MASK_SEASON, ALPHA_MASK_FIELD, alpha1, alpha2 );
      }
      // �Ō�̃t�F�[�h�͉���ʂ��P�x�t�F�[�h�C��������
      {
        float rate   = (float)work->count / (float)work->maxCount;
        int   bright = BRIGHT_MIN + (BRIGHT_MAX - BRIGHT_MIN) * rate;
        GXS_SetMasterBrightness( bright );
      }
    }
    // �Ō�ȊO�̃t�F�[�h�A�E�g
    else { 
      float rate   = (float)work->count / (float)work->maxCount;
      int   bright = BRIGHT_MAX - (BRIGHT_MAX - BRIGHT_MIN) * rate;
      G2_SetBlendBrightness( ALPHA_MASK_SEASON, bright );
    }
  }
}
//------------------------------------------------------------------------------
/**
 * @brief ���u�����f�B���O���X�V���� ( EVENT_MODE_GAMESTART )
 *
 * @param work �C�x���g���[�N
 * @param seq  ���݂̃V�[�P���X
 */
//------------------------------------------------------------------------------
static void UpdateAlpha_GAMESTART( const EVENT_WORK* work, int seq )
{
  // �s���Ăяo��
  GF_ASSERT( GetMode(work) == EVENT_MODE_GAMESTART );

  // �t�F�[�h�C��
  if( seq == SEQ_FADEIN ) { 
    float rate   = (float)work->count / (float)work->maxCount;
    int   bright = BRIGHT_MIN + (BRIGHT_MAX - BRIGHT_MIN) * rate;
    G2_SetBlendBrightness( ALPHA_MASK_SEASON, bright );
  }
  // �t�F�[�h�A�E�g
  else if( seq == SEQ_FADEOUT ) { 
    float rate   = (float)work->count / (float)work->maxCount;
    int   bright = BRIGHT_MAX - (BRIGHT_MAX - BRIGHT_MIN) * rate;
    G2_SetBlendBrightness( ALPHA_MASK_SEASON, bright );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief ���̏����V�[�P���X���擾����
 *
 * @param work �C�x���g���[�N
 * @param seq  ���݂̃V�[�P���X
 *
 * @return ���̃V�[�P���X�ԍ�( �ω����Ȃ���Ό��݂Ɠ����V�[�P���X�ԍ� )
 */
//------------------------------------------------------------------------------
static int GetNextSeq( const EVENT_WORK* work, int seq )
{
  int next = seq;

  switch( seq ) {
  // ������
  case SEQ_INIT:
    next = SEQ_PREPARE;
    break;

  // �\������
  case SEQ_PREPARE:
    next = SEQ_FADEIN;
    break;

  // �t�F�[�h�C��
  case SEQ_FADEIN:
    if( CheckCountOver(work) == TRUE ) {
      next = SEQ_WAIT;
    }
    break;

  // �ҋ@
  case SEQ_WAIT:
    if( (CheckCountOver(work) == TRUE) || (IsSkipOn(work) == TRUE) ) {
      next = SEQ_FADEOUT; 
    }
    break;

  // �t�F�[�h�A�E�g
  case SEQ_FADEOUT:
    if( CheckCountOver(work) == TRUE ) {
      // ���݂̋G�߂�\�����I������, �C�x���g�I��
      if( work->endSeason == work->dispSeason ) {
        next = SEQ_EXIT; 
      }
      else { 
        next = SEQ_PREPARE;
      }
    }
    break;

  // �I��
  case SEQ_EXIT:
    break;
  } 

  return next;
}

//------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X�̐ݒ��, ����ɔ������������s��
 *
 * @param work �C�x���g���[�N
 * @param seq  �V�[�P���X���[�N�ւ̃|�C���^
 * @param next ���̃V�[�P���X
 */
//------------------------------------------------------------------------------
static void SetSeq( EVENT_WORK* work, int* seq, int next )
{
  switch( next ) {
  // ������
  case SEQ_INIT:
    SetupBG( work ); // BG�����ݒ�
    break;

  // �\������
  case SEQ_PREPARE:
    // �\������G�߂�����
    work->dispSeason = PMSEASON_GetNextSeasonID( work->dispSeason );
    // �O���t�B�b�N�f�[�^�ǂݍ���
    LoadGraphicData( work->dispSeason, work->heapID );
    break;

  // �t�F�[�h�C��
  case SEQ_FADEIN:
    // BG�\���J�n
    GFL_BG_SetVisible( BG_FRAME_SEASON, VISIBLE_ON ); 
    // �t�B�[���h��\��
    GFL_BG_SetVisible( BG_FRAME_FIELD, VISIBLE_OFF ); 
    // ��ʃt�F�[�h������
    GX_SetMasterBrightness(0);
    G2_SetBlendBrightness( ALPHA_MASK_SEASON, BRIGHT_MIN );
    // �t�F�[�h���Ԃ�����
    work->maxCount = GetMaxCountOfFadeIn( work );
    break;

  // �ҋ@
  case SEQ_WAIT:
    // �ҋ@���Ԃ�����
    work->maxCount = GetMaxCountOfWait( work );
    break;

  // �t�F�[�h�A�E�g
  case SEQ_FADEOUT:
    // �t�F�[�h���Ԃ�����
    work->maxCount = GetMaxCountOfFadeOut( work );
    // �Ō�̕\�����t�F�[�h�A�E�g�����鎞��, ���Ƀt�B�[���h��\������
    if( (GetMode(work) == EVENT_MODE_MAPCHANGE) && 
        (work->dispSeason == work->endSeason) ) {
      FIELDMAP_InitBG( work->fieldmap );
      GFL_BG_SetVisible( BG_FRAME_SEASON, VISIBLE_ON ); 
    }
    break;

  // �I��
  case SEQ_EXIT:
    break;
  }

  // �V�[�P���X�ύX
  *seq = next;
  work->count = 0;
}

//------------------------------------------------------------------------------
/**
 * @brief ���̃V�[�P���X�Ɉڍs����
 *
 * @param work �C�x���g���[�N
 * @param seq  �V�[�P���X���[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------------------
static void StepNextSeq( EVENT_WORK* work, int* seq )
{
  int next;
  
  // ���̃V�[�P���X���擾
  next = GetNextSeq( work, *seq );

  // �ω�������J�ڂ���
  if( *seq != next ) {
    SetSeq( work, seq, next );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief �C�x���g�J�n���̏���
 *
 * @param work �C�x���g���[�N
 */
//------------------------------------------------------------------------------
static void EventInit( EVENT_WORK* work )
{
  // �G�ߕ\�����t���O���Z�b�g
  if( work->fieldmap ) {
    FIELDMAP_SetSeasonDispFlag( work->fieldmap, TRUE );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief �C�x���g�I������
 *
 * @param work �C�x���g���[�N
 */
//------------------------------------------------------------------------------
static void EventExit( EVENT_WORK* work )
{
  // BG���\���ɐݒ�
  GFL_BG_SetVisible( BG_FRAME_SEASON, VISIBLE_OFF ); 

  // �G�ߕ\�����t���O�����Z�b�g
  if( work->fieldmap ) {
    FIELDMAP_SetSeasonDispFlag( work->fieldmap, FALSE );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief ���샂�[�h���擾����
 *
 * @param work
 *
 * @return ���샂�[�h ( EVENT_MODE_xxxx )
 */
//------------------------------------------------------------------------------
static EVENT_MODE GetMode( const EVENT_WORK* work )
{
  return work->mode;
}

//------------------------------------------------------------------------------
/**
 * @brief �X�L�b�v���邩�ǂ������擾����
 *
 * @param work
 *
 * @return �X�L�b�v����ꍇ TRUE, �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------
static BOOL IsSkipOn( const EVENT_WORK* work )
{
  return work->skipFlag;
}

//------------------------------------------------------------------------------
/**
 * @brief �X�L�b�vON�ɐݒ肷��
 *
 * @param work
 */
//------------------------------------------------------------------------------
static void SetSkipOn( EVENT_WORK* work )
{
  work->skipFlag = TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief �J�E���^���ő�l���I�[�o�[�������ǂ����𔻒肷��
 *
 * @param work
 *
 * @return �J�E���^���ő�l�𒴂��Ă���ꍇ TRUE, �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------
static BOOL CheckCountOver( const EVENT_WORK* work )
{
  return ( work->maxCount < work->count );
}

//------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�C���̍ő�t���[�������擾����
 *
 * @param work
 *
 * @return �t�F�[�h�C���̍ő�t���[����
 */
//------------------------------------------------------------------------------
static int GetMaxCountOfFadeIn( const EVENT_WORK* work )
{
  //�w�������傩��x�̎��͌Œ�
  if( GetMode(work) == EVENT_MODE_GAMESTART ) {
    return GAMESTART_FI_FRAME;
  }

  // �}�b�v�`�F���W���Ȃ�, �\������G�߂ɉ����ĕω�
  if( work->dispSeason == work->endSeason ) {
    return MAPCHANGE_FI_FRAME_LONG; // ���݂̋G�߂͒���
  }
  else {
    return MAPCHANGE_FI_FRAME_SHORT; // �ڂ낤�G�߂͒Z��
  }
}
//------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�A�E�g�̍ő�t���[�������擾����
 *
 * @param work
 *
 * @return �t�F�[�h�A�E�g�̍ő�t���[����
 */
//------------------------------------------------------------------------------
static int GetMaxCountOfFadeOut( const EVENT_WORK* work )
{
  //�w�������傩��x�̎��͌Œ�
  if( GetMode(work) == EVENT_MODE_GAMESTART ) {
    return GAMESTART_FO_FRAME;
  }

  // �t�F�[�h���Ԃ�����
  if( work->dispSeason == work->endSeason ) {
    return MAPCHANGE_FO_FRAME_LONG; // ���݂̋G�߂͒���
  }
  else {
    return MAPCHANGE_FO_FRAME_SHORT; // �ڂ낤�G�߂͒Z��
  }
}
//------------------------------------------------------------------------------
/**
 * @brief �ҋ@�V�[�P���X�̍ő�t���[�������擾����
 *
 * @param work
 *
 * @return �ҋ@�V�[�P���X�̍ő�t���[����
 */
//------------------------------------------------------------------------------
static int GetMaxCountOfWait( const EVENT_WORK* work )
{
  //�w�������傩��x�̎��͌Œ�
  if( GetMode(work) == EVENT_MODE_GAMESTART ) {
    return GAMESTART_WAIT_FRAME;
  }

  // �}�b�v�`�F���W���Ȃ�, �\������G�߂ɉ����ĕω�
  if( work->dispSeason == work->endSeason ) {
    return MAPCHANGE_WAIT_FRAME_LONG; // ���݂̋G�߂͒���
  }
  else {
    return MAPCHANGE_WAIT_FRAME_SHORT; // �ڂ낤�G�߂͒Z�� 
  }
}
