//////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �G�ߕ\���C�x���g
 * @file   event_season_display.c
 * @author obata
 * @date   2009.11.25
 */
//////////////////////////////////////////////////////////////////////////////////////////
#include "fieldmap.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "event_season_display.h"

#include "gamesystem/pm_season.h"  // for PMSEASON_xxxx
#include "arc/arc_def.h"  // for ARCID_SEASON_DISPLAY
#include "arc/season_display.naix"  // for datid
#include "field_status_local.h"  // for FIELD_STATUS


//========================================================================================
// ��BG
//========================================================================================
#define BG_FRAME_FIELD    (GFL_BG_FRAME0_M)         // �t�B�[���h��\�����Ă���BG��
#define BG_FRAME_SEASON   (GFL_BG_FRAME3_M)         // �G�ߕ\���Ɏg�p����BG��
#define BG_PRIORITY       (1)                       // �G��BG�ʂ̕\���D�揇��
#define ALPHA_MASK_SEASON (GX_BLEND_PLANEMASK_BG3)  // ���u�����f�B���O���Ώۖ�(�G�ߕ\��)
#define ALPHA_MASK_FIELD  (GX_BLEND_PLANEMASK_BG0)  // ���u�����f�B���O���Ώۖ�(�t�B�[���h)
#define ALPHA_MAX         (16)                      // ���ő�l
#define ALPHA_MIN         (0)                       // ���ŏ��l
#define BRIGHT_MAX        (0)                       // �P�x�ő�l
#define BRIGHT_MIN        (-16)                     // �P�x�ŏ��l

// BG�R���g���[��
static const GFL_BG_BGCNT_HEADER bg_header = 
{
  0, 0,					          // �����\���ʒu
  0,						          // �X�N���[���o�b�t�@�T�C�Y
  0,							        // �X�N���[���o�b�t�@�I�t�Z�b�g
  GFL_BG_SCRSIZ_256x256,	// �X�N���[���T�C�Y
  GX_BG_COLORMODE_16,			// �J���[���[�h
  GX_BG_SCRBASE_0x1800,		// �X�N���[���x�[�X�u���b�N
  GX_BG_CHARBASE_0x08000,	// �L�����N�^�x�[�X�u���b�N
  GFL_BG_CHRSIZ_256x256,	// �L�����N�^�G���A�T�C�Y
  GX_BG_EXTPLTT_01,			  // BG�g���p���b�g�X���b�g�I��
  BG_PRIORITY,		        // �\���v���C�I���e�B�[
  0,							        // �G���A�I�[�o�[�t���O
  0,							        // DUMMY
  FALSE,						      // ���U�C�N�ݒ�
}; 


//========================================================================================
// ���萔
//========================================================================================
// �G�߃X�L�b�v
#define FADEIN_FRAME_SHORT  (10)  // �t�F�[�h�C������
#define WAIT_FRAME_SHORT    (30)  // �ҋ@����
#define FADEOUT_FRAME_SHORT (10)  // �t�F�[�h�A�E�g����
// ���G�ߕ\��
#define FADEIN_FRAME_LONG  (10)  // �t�F�[�h�C������
#define WAIT_FRAME_LONG    (60)  // �ҋ@����
#define FADEOUT_FRAME_LONG (20)  // �t�F�[�h�A�E�g����

// �C�x���g�V�[�P���X
enum
{
  SEQ_INIT,    // ������
  SEQ_PREPARE, // �\������
  SEQ_FADEIN,  // �t�F�[�h�C��
  SEQ_WAIT,    // �ҋ@
  SEQ_FADEOUT, // �t�F�[�h�A�E�g
  SEQ_EXIT,    // �I��
};


//========================================================================================
// ���C�x���g���[�N
//========================================================================================
typedef struct
{
  GAMESYS_WORK* gsys;
  FIELDMAP_WORK* fieldmap;

  u8 currentSeason;  // ���݂̋G��
  u8 dispSeason;     // �\�����̋G��
  u32 count;         // �J�E���^
  u32 maxCount;      // �J�E���^�ő�l

} EVENT_WORK;


//========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//========================================================================================
static void InitBG();
static void LoadGraphicData( u8 season, HEAPID heap_id );
static void UpdateAlpha( const EVENT_WORK* work, int seq );
static int GetNextSeq( const EVENT_WORK* work, int seq );
static void SetSeq( EVENT_WORK* work, int* seq, int next );
static void StepNextSeq( EVENT_WORK* work, int* seq );


//========================================================================================
// ���C�x���g�����֐�
//========================================================================================
static GMEVENT_RESULT SeasonDisplay( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  // �V�[�P���X�ŗL����
  switch( *seq )
  {
  // ������
  case SEQ_INIT:
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
    return GMEVENT_RES_FINISH;
  }

  // ���ʏ���
  work->count++;
  StepNextSeq( work, seq );
  return GMEVENT_RES_CONTINUE;
}


//========================================================================================
// ���O�����J�֐�
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �l�G�\���C�x���g���쐬����
 *
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 * @param start    �J�n�G��
 * @param end      �ŏI�G�� 
 *
 * @return �쐬�����C�x���g
 */
//----------------------------------------------------------------------------------------
GMEVENT* EVENT_SeasonDisplay( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap,
                              u8 start, u8 end )
{
  GMEVENT* event;
  EVENT_WORK* work;
  int* seq;

  // �C�x���g����
  event = GMEVENT_Create( gsys, NULL, SeasonDisplay, sizeof(EVENT_WORK) ); 
  // �C�x���g���[�N������
  seq                 = GMEVENT_GetSequenceWork( event );
  work                = GMEVENT_GetEventWork( event );
  work->gsys          = gsys;
  work->fieldmap      = fieldmap;
  work->currentSeason = end;
  work->dispSeason    = (start + PMSEASON_TOTAL - 1) % PMSEASON_TOTAL;
  SetSeq( work, seq, SEQ_INIT );
  return event;
}

//----------------------------------------------------------------------------------------
/**
 * @brief �G�ߕ\���C�x���g�ɗv����t���[�������擾����
 *
 * @param gsys �Q�[���V�X�e��
 *
 * @return �C�x���g�N������I���܂łɂ�����t���[����
 */
//----------------------------------------------------------------------------------------
u32 GetSeasonDispEventFrame( GAMESYS_WORK* gsys )
{
  GAMEDATA* gdata;
  FIELD_STATUS* fstatus;
  u32 frame;
  u8 last, now, num;

  // �\������G�߂̐������߂�
  gdata   = GAMESYSTEM_GetGameData( gsys );
  fstatus = GAMEDATA_GetFieldStatus( gdata );
  last    =  FIELD_STATUS_GetSeasonDispLast( fstatus );
  now     = GAMEDATA_GetSeasonID( gdata );
  num     = (now - last + PMSEASON_TOTAL) % PMSEASON_TOTAL;

  // �t���[�������v�Z
  frame  = 0;
  frame += 2;  // INIT, EXIT �̕�
  frame += (num - 1) *   // �X�L�b�v�\���̕� (PREPARE, FADEIN, WAIT, FADEOUT)
           (1 + FADEIN_FRAME_SHORT + WAIT_FRAME_SHORT + FADEOUT_FRAME_SHORT);
  frame += (1 + FADEIN_FRAME_LONG + WAIT_FRAME_LONG + FADEOUT_FRAME_LONG); // ���G�ߕ\���̕�
  return frame;
}


//========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief BG�Ɋւ��鏉���ݒ���s��
 */
//----------------------------------------------------------------------------------------
static void InitBG()
{
  // �����ݒ�
  //GFL_BG_SetBGControl( BG_FRAME_SEASON, &bg_header, GFL_BG_MODE_TEXT );
  //GFL_BG_SetPriority( BG_FRAME_SEASON, BG_PRIORITY );
  //G2_SetBlendAlpha( ALPHA_MASK_SEASON, ALPHA_MASK_FIELD, ALPHA_MIN, ALPHA_MAX ); 
  // �L����VRAM�E�X�N���[��VRAM���N���A
  GFL_BG_ClearFrame( BG_FRAME_SEASON ); 
}

//----------------------------------------------------------------------------------------
/**
 * @brief �w�肵���G�߂̕\���ɕK�v�ȃO���t�B�b�N�f�[�^��ǂݍ���
 *
 * @param season  �ǂݍ��݂��s���G��
 * @param haep_id �g�p����q�[�vID
 */
//----------------------------------------------------------------------------------------
static void LoadGraphicData( u8 season, HEAPID heap_id )
{
  ARCDATID datid_pltt, datid_char, datid_scrn;

  // �f�[�^ID������
  switch( season )
  {
  default:
    OBATA_Printf( "====================================================\n" );
    OBATA_Printf( "EVENT-SEASON-DISPLAY: �͈͊O�̋G�߂��w�肳��܂����B\n" );
    OBATA_Printf( "====================================================\n" );
  case PMSEASON_SPRING:
    OBATA_Printf( "EVENT-SEASON-DISPLAY: load graphic data (spring)\n" );
    datid_pltt = NARC_season_display_spring_nclr;
    datid_char = NARC_season_display_spring_ncgr;
    datid_scrn = NARC_season_display_spring_nscr;
    break;
  case PMSEASON_SUMMER:
    OBATA_Printf( "EVENT-SEASON-DISPLAY: load graphic data (summer)\n" );
    datid_pltt = NARC_season_display_summer_nclr;
    datid_char = NARC_season_display_summer_ncgr;
    datid_scrn = NARC_season_display_summer_nscr;
    break;
  case PMSEASON_AUTUMN:
    OBATA_Printf( "EVENT-SEASON-DISPLAY: load graphic data (autumn)\n" );
    datid_pltt = NARC_season_display_autumn_nclr;
    datid_char = NARC_season_display_autumn_ncgr;
    datid_scrn = NARC_season_display_autumn_nscr;
    break;
  case PMSEASON_WINTER:
    OBATA_Printf( "EVENT-SEASON-DISPLAY: load graphic data (winter)\n" );
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

//----------------------------------------------------------------------------------------
/**
 * @brief ���u�����f�B���O���X�V����
 *
 * @param work �C�x���g���[�N
 * @param seq  ���݂̃V�[�P���X
 */
//----------------------------------------------------------------------------------------
static void UpdateAlpha( const EVENT_WORK* work, int seq )
{
  if( seq == SEQ_FADEIN )
  { // �t�F�[�h�C��
    int bright;
    float rate;
    rate = (float)work->count / (float)work->maxCount;
    bright = BRIGHT_MIN + (BRIGHT_MAX - BRIGHT_MIN) * rate;
    G2_SetBlendBrightness( ALPHA_MASK_SEASON, bright );
  }
  else if( seq == SEQ_FADEOUT )
  { // �t�F�[�h�A�E�g
    if( work->dispSeason == work->currentSeason )
    { // �Ō�̋G�ߕ\���̓��u�����h
      int alpha1, alpha2;
      float rate;
      rate = (float)work->count / (float)work->maxCount;
      alpha2 = ALPHA_MAX * rate;
      alpha1 = ALPHA_MAX - alpha2;
      G2_SetBlendAlpha( ALPHA_MASK_SEASON, ALPHA_MASK_FIELD, alpha1, alpha2 );
    }
    else
    { // �X�L�b�v���͋P�x
      int bright;
      float rate;
      rate = (float)work->count / (float)work->maxCount;
      bright = BRIGHT_MAX - (BRIGHT_MAX - BRIGHT_MIN) * rate;
      G2_SetBlendBrightness( ALPHA_MASK_SEASON, bright );
    }
  }
}

//----------------------------------------------------------------------------------------
/**
 * @brief ���̏����V�[�P���X���擾����
 *
 * @param work �C�x���g���[�N
 * @param seq  ���݂̃V�[�P���X
 *
 * @return ���̃V�[�P���X�ԍ�( �ω����Ȃ���Ό��݂Ɠ����V�[�P���X�ԍ� )
 */
//----------------------------------------------------------------------------------------
static int GetNextSeq( const EVENT_WORK* work, int seq )
{
  int next = seq;

  switch( seq )
  {
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
    if( work->maxCount < work->count ){ next = SEQ_WAIT; }
    break;
  // �ҋ@
  case SEQ_WAIT:
    if( work->maxCount < work->count ){ next = SEQ_FADEOUT; }
    break;
  // �t�F�[�h�A�E�g
  case SEQ_FADEOUT:
    if( work->maxCount < work->count )
    {
      // ���݂̋G�߂�\�����I������, �C�x���g�I��
      if( work->currentSeason == work->dispSeason ){ next = SEQ_EXIT; }
      else{ next = SEQ_PREPARE; }
    }
    break;
  // �I��
  case SEQ_EXIT:
    break;
  } 

  return next;
}

//----------------------------------------------------------------------------------------
/**
 * @brief �����V�[�P���X�̐ݒ��, ����ɔ������������s��
 *
 * @param work �C�x���g���[�N
 * @param seq  �V�[�P���X���[�N�ւ̃|�C���^
 * @param next ���̃V�[�P���X
 */
//----------------------------------------------------------------------------------------
static void SetSeq( EVENT_WORK* work, int* seq, int next )
{
  switch( next )
  {
  // ������
  case SEQ_INIT:
    // BG�����ݒ�
    InitBG();
    break;
  // �\������
  case SEQ_PREPARE:
    // �\������G�߂�����
    work->dispSeason = (work->dispSeason + 1) % PMSEASON_TOTAL;
    // �O���t�B�b�N�f�[�^�ǂݍ���
    {
      HEAPID heap_id = FIELDMAP_GetHeapID( work->fieldmap );
      LoadGraphicData( work->dispSeason, heap_id );
    }
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
    if( work->dispSeason == work->currentSeason ){ work->maxCount = FADEIN_FRAME_LONG; }
    else{ work->maxCount = FADEIN_FRAME_SHORT; }
    break;
  // �ҋ@
  case SEQ_WAIT:
    // �ҋ@���Ԃ�����
    if( work->dispSeason == work->currentSeason ){ work->maxCount = WAIT_FRAME_LONG; }
    else{ work->maxCount = WAIT_FRAME_SHORT; }
    break;
  // �t�F�[�h�A�E�g
  case SEQ_FADEOUT:
    // �t�F�[�h���Ԃ�����
    if( work->dispSeason == work->currentSeason ){ work->maxCount = FADEOUT_FRAME_LONG; }
    else{ work->maxCount = FADEOUT_FRAME_SHORT; }
    // �Ō�̕\�����t�F�[�h�A�E�g�����鎞��, ���Ƀt�B�[���h��\������
    if( work->dispSeason == work->currentSeason )
    {
      GFL_BG_SetVisible( BG_FRAME_FIELD, VISIBLE_ON ); 
    }
    break;
  // �I��
  case SEQ_EXIT:
    GFL_BG_SetVisible( BG_FRAME_SEASON, VISIBLE_OFF ); 
    break;
  }

  // �V�[�P���X�ύX
  *seq = next;
  work->count = 0;

  // DEBUG:
  OBATA_Printf( "EVENT-SEASON-DISPLAY: seq ==> " );
  switch( next )
  {
  case SEQ_INIT:    OBATA_Printf( "INIT\n" );    break;
  case SEQ_PREPARE: OBATA_Printf( "PREPARE\n" ); break;
  case SEQ_FADEIN:  OBATA_Printf( "FADEIN\n" );  break;
  case SEQ_WAIT:    OBATA_Printf( "WAIT\n" );    break;
  case SEQ_FADEOUT: OBATA_Printf( "FADEOUT\n" ); break;
  case SEQ_EXIT:    OBATA_Printf( "EXIT\n" );    break;
  }
}

//----------------------------------------------------------------------------------------
/**
 * @brief ���̃V�[�P���X�Ɉڍs����
 *
 * @param work �C�x���g���[�N
 * @param seq  �V�[�P���X���[�N�ւ̃|�C���^
 */
//----------------------------------------------------------------------------------------
static void StepNextSeq( EVENT_WORK* work, int* seq )
{
  int next;
  
  // ���̃V�[�P���X���擾
  next = GetNextSeq( work, *seq );

  // �ω�������J�ڂ���
  if( *seq != next )
  {
    SetSeq( work, seq, next );
  }
}
