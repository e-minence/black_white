///////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �p���b�g�A�j���[�V����
 * @file   palette_anime.c
 * @author obata
 * @date   2010.03.04
 */
///////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "palette_anime.h"
#include "system/palanm.h"


//=============================================================================
// ���萔
//============================================================================= 
// sin �t�F�[�h
#define SIN_FADE_ONE_CYCLE_FRAMES (60) // ����1��̃t���[����
#define SIN_FADE_MIN_EVY           (0) // �ŏ��t�F�[�h�W��[0, 16]
#define SIN_FADE_MAX_EVY          (10) // �ő�t�F�[�h�W��[0, 16]
// sin �t�F�[�h ( �� )
#define SIN_FADE_FAST_ONE_CYCLE_FRAMES (30) // ����1��̃t���[����
#define SIN_FADE_FAST_MIN_EVY           (0) // �ŏ��t�F�[�h�W��[0, 16]
#define SIN_FADE_FAST_MAX_EVY           (3) // �ő�t�F�[�h�W��[0, 16]
// sin �t�F�[�h ( �x )
#define SIN_FADE_SLOW_ONE_CYCLE_FRAMES (90) // ����1��̃t���[����
#define SIN_FADE_SLOW_MIN_EVY           (0) // �ŏ��t�F�[�h�W��[0, 16]
#define SIN_FADE_SLOW_MAX_EVY           (3) // �ő�t�F�[�h�W��[0, 16]
// �_��
#define BLINK_FRAMES      (20) // �A�j���[�V�����t���[����
#define BLINK_WAIT_FRAMES  (4) // �_�ő҂��t���[����
#define BLINK_MIN_EVY      (0) // �ŏ��t�F�[�h�W��[0, 16]
#define BLINK_MAX_EVY      (6) // �ő�t�F�[�h�W��[0, 16]
// �_�� ( �Z )
#define BLINK_SHORT_FRAMES      (10) // �A�j���[�V�����t���[����
#define BLINK_SHORT_WAIT_FRAMES  (4) // �_�ő҂��t���[����
#define BLINK_SHORT_MIN_EVY      (0) // �ŏ��t�F�[�h�W��[0, 16]
#define BLINK_SHORT_MAX_EVY      (6) // �ő�t�F�[�h�W��[0, 16]
// �_�� ( �� )
#define BLINK_LONG_FRAMES      (30) // �A�j���[�V�����t���[����
#define BLINK_LONG_WAIT_FRAMES  (4) // �_�ő҂��t���[����
#define BLINK_LONG_MIN_EVY      (0) // �ŏ��t�F�[�h�W��[0, 16]
#define BLINK_LONG_MAX_EVY      (6) // �ő�t�F�[�h�W��[0, 16]
// �_�� ( �� )
#define BLINK_BRIGHT_FRAMES      (20) // �A�j���[�V�����t���[����
#define BLINK_BRIGHT_WAIT_FRAMES  (4) // �_�ő҂��t���[����
#define BLINK_BRIGHT_MIN_EVY      (0) // �ŏ��t�F�[�h�W��[0, 16]
#define BLINK_BRIGHT_MAX_EVY      (9) // �ő�t�F�[�h�W��[0, 16]
// �Ó]
#define HOLD_FRAMES (20) // �Ó]�܂ł̃t���[����
#define HOLD_MIN_EVY (0) // �ŏ��t�F�[�h�W��[0, 16]
#define HOLD_MAX_EVY (7) // �ő�t�F�[�h�W��[0, 16]
// �Ó]����̕��A
#define RECOVER_FRAMES (30) // �A�j���[�V�����t���[����
#define RECOVER_MIN_EVY (0) // �ŏ��t�F�[�h�W��[0, 16]
#define RECOVER_MAX_EVY (7) // �ő�t�F�[�h�W��[0, 16]


//=============================================================================
// ���p���b�g�A�j���[�V�������s���[�N
//=============================================================================
struct _PALETTE_ANIME
{
  HEAPID heapID;

  BOOL setupFlag;     // �Z�b�g�A�b�v����Ă��邩�ǂ���
  u16* transDest;     // �]����A�h���X
  u16* originalColor; // �A�j���[�V�������f�[�^
  u8   colorNum;      // �A�j���[�V��������J���[�� ( �p���b�g1�{ = 16 )
  u16  fadeColor;     // �t�F�[�h�J���[

  BOOL       animeFlag;  // �A�j���[�V���������ǂ���
  ANIME_TYPE animeType;  // �A�j���[�V�����^�C�v
  u32        frameCount; // �t���[�����J�E���^
};




//=============================================================================
// ���֐��C���f�b�N�X
//=============================================================================
//-----------------------------------------------------------------------------
// ��LAYER 2 �A�j���[�V����
//-----------------------------------------------------------------------------
static void UpdateAnime( PALETTE_ANIME* work ); // �A�j���[�V�������X�V����
static void UpdateAnime_SIN_FADE( PALETTE_ANIME* work ); // �A�j���[�V�������X�V���� ( sin �t�F�[�h )
static void UpdateAnime_SIN_FADE_FAST( PALETTE_ANIME* work ); // �A�j���[�V�������X�V���� ( sin �t�F�[�h�E�� )
static void UpdateAnime_SIN_FADE_SLOW( PALETTE_ANIME* work ); // �A�j���[�V�������X�V���� ( sin �t�F�[�h�E�x )
static void UpdateAnime_BLINK( PALETTE_ANIME* work ); // �A�j���[�V�������X�V���� ( �_�� )
static void UpdateAnime_BLINK_SHORT( PALETTE_ANIME* work ); // �A�j���[�V�������X�V���� ( �_�ŁE�Z )
static void UpdateAnime_BLINK_LONG( PALETTE_ANIME* work ); // �A�j���[�V�������X�V���� ( �_�ŁE�� )
static void UpdateAnime_BLINK_BRIGHT( PALETTE_ANIME* work ); // �A�j���[�V�������X�V���� ( �� )
static void UpdateAnime_HOLD( PALETTE_ANIME* work ); // �A�j���[�V�������X�V���� ( �Ó] )
static void UpdateAnime_RECOVER( PALETTE_ANIME* work ); // �A�j���[�V�������X�V���� ( �Ó]����̕��A )
//-----------------------------------------------------------------------------
// ��LAYER 1 ����
//-----------------------------------------------------------------------------
static void StartAnime( PALETTE_ANIME* work, ANIME_TYPE type, u16 color ); // �A�j���[�V�������J�n����
static void StopAnime( PALETTE_ANIME* work ); // �A�j���[�V�������~����
static void ResetPalette( PALETTE_ANIME* work ); // �p���b�g�����Z�b�g����
static BOOL CheckAnime( const PALETTE_ANIME* work ); // �A�j���[�V���������ǂ������`�F�b�N����
//-----------------------------------------------------------------------------
// ��LAYER 0 �����E�j��
//-----------------------------------------------------------------------------
static PALETTE_ANIME* CreateWork( HEAPID heapID ); // ���[�N�𐶐�����
static void DeleteWork( PALETTE_ANIME* work ); // ���[�N��j������
static void InitWork( PALETTE_ANIME* work ); // ���[�N������������
static void SetupWork( PALETTE_ANIME* work, u16* dest, u16* src, u8 colorNum ); // ���[�N���Z�b�g�A�b�v����
static void CleanUpWork( PALETTE_ANIME* work ); // ���[�N���N���[���A�b�v����




//=============================================================================
// �������E�j��
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N�𐶐�����
 *
 * @param heapID �g�p����q�[�vID
 * @return ���������A�j���[�V�������[�N
 */
//-----------------------------------------------------------------------------
PALETTE_ANIME* PALETTE_ANIME_Create( HEAPID heapID )
{
  PALETTE_ANIME* work;

  work = CreateWork( heapID ); // ����
  InitWork( work );            // ������

  return work;
}

//-----------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N��j������
 *
 * @param work �j�����郏�[�N
 */
//-----------------------------------------------------------------------------
void PALETTE_ANIME_Delete( PALETTE_ANIME* work )
{
  CleanUpWork( work ); // �N���[���A�b�v
  DeleteWork( work );  // �j��
}

//-----------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
 *
 * @param work
 * @param dest     �p���b�g�f�[�^�̓]����A�h���X
 * @param src      �A�j���[�V�����̌��f�[�^�̃A�h���X
 * @param colorNum �A�j���[�V��������F�̐� ( �p���b�g1�{ = 16 )
 */
//-----------------------------------------------------------------------------
void PALETTE_ANIME_Setup( PALETTE_ANIME* work, u16* dest, u16* src, u8 colorNum )
{
  SetupWork( work, dest, src, colorNum );
}

//=============================================================================
// ������
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
void PALETTE_ANIME_Update( PALETTE_ANIME* work )
{
  UpdateAnime( work );
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������J�n����
 *
 * @param work
 * @param type �J�n����A�j���[�V�������w��
 * @param color �t�F�[�h�J���[
 */
//-----------------------------------------------------------------------------
void PALETTE_ANIME_Start( PALETTE_ANIME* work, ANIME_TYPE type, u16 color )
{
  StartAnime( work, type, color );
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������~����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
void PALETTE_ANIME_Stop( PALETTE_ANIME* work )
{
  StopAnime( work );
}

//-----------------------------------------------------------------------------
/**
 * @brief ���삵�Ă����p���b�g�����Z�b�g����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
void PALETTE_ANIME_Reset( PALETTE_ANIME* work )
{
  ResetPalette( work );
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V���������ǂ������`�F�b�N����
 *
 * @param work
 *
 * @return �A�j���[�V�������Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------
BOOL PALETTE_ANIME_CheckAnime( const PALETTE_ANIME* anime )
{
  return CheckAnime( anime );
}



//=============================================================================
// ������E����
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������J�n����
 *
 * @param work
 * @param type  �J�n����A�j���[�V�������w��
 * @param color �t�F�[�h�J���[
 */
//-----------------------------------------------------------------------------
static void StartAnime( PALETTE_ANIME* work, ANIME_TYPE type, u16 color )
{
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�

  work->animeFlag  = TRUE;
  work->animeType  = type;
  work->frameCount = 0;
  work->fadeColor  = color;
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������~����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void StopAnime( PALETTE_ANIME* work )
{
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�

  work->animeFlag = FALSE;
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������X�V����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateAnime( PALETTE_ANIME* work )
{
  if( work->setupFlag == FALSE ) { return; } // ���Z�b�g�A�b�v
  if( work->animeFlag == FALSE ) { return; } // �A�j���[�V������~��

  // �A�j���^�C�v�ɉ������X�V����
  switch( work->animeType ) {
  case ANIME_TYPE_SIN_FADE:      UpdateAnime_SIN_FADE( work );      break;
  case ANIME_TYPE_SIN_FADE_FAST: UpdateAnime_SIN_FADE_FAST( work ); break;
  case ANIME_TYPE_SIN_FADE_SLOW: UpdateAnime_SIN_FADE_SLOW( work ); break;
  case ANIME_TYPE_BLINK:         UpdateAnime_BLINK( work );         break;
  case ANIME_TYPE_BLINK_SHORT:   UpdateAnime_BLINK_SHORT( work );   break;
  case ANIME_TYPE_BLINK_LONG:    UpdateAnime_BLINK_LONG( work );    break;
  case ANIME_TYPE_BLINK_BRIGHT:  UpdateAnime_BLINK_BRIGHT( work );  break;
  case ANIME_TYPE_HOLD:          UpdateAnime_HOLD( work );          break;
  case ANIME_TYPE_RECOVER:       UpdateAnime_RECOVER( work );       break;
  default: GF_ASSERT(0);
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������X�V���� ( sin �t�F�[�h )
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateAnime_SIN_FADE( PALETTE_ANIME* work )
{
  u8 evy;

  GF_ASSERT( work->animeFlag ); // �A�j���[�V������~��
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�

  // �t�F�[�h�W��������
  {
    int idx;
    float sinVal;
    idx = (work->frameCount * 65536 / SIN_FADE_ONE_CYCLE_FRAMES) % 65536;
    sinVal = FX_FX16_TO_F32( FX_SinIdx( idx ) );
    evy = (int)( sinVal * 16 + 16 );
    evy = SIN_FADE_MIN_EVY + (SIN_FADE_MAX_EVY - SIN_FADE_MIN_EVY) * evy / 32;
  }

  // �p���b�g��������
  SoftFade( work->originalColor, work->transDest, work->colorNum, evy, work->fadeColor ); 

  // �A�j���[�V�����t���[�������C���N�������g
  work->frameCount++;
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������X�V���� ( sin �t�F�[�h�E�� )
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateAnime_SIN_FADE_FAST( PALETTE_ANIME* work )
{
  u8 evy;

  GF_ASSERT( work->animeFlag ); // �A�j���[�V������~��
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�

  // �t�F�[�h�W��������
  {
    int idx;
    float sinVal;
    idx = (work->frameCount * 65536 / SIN_FADE_FAST_ONE_CYCLE_FRAMES) % 65536;
    sinVal = FX_FX16_TO_F32( FX_SinIdx( idx ) );
    evy = (int)( sinVal * 16 + 16 );
    evy = SIN_FADE_FAST_MIN_EVY + (SIN_FADE_FAST_MAX_EVY - SIN_FADE_FAST_MIN_EVY) * evy / 32;
  }

  // �p���b�g��������
  SoftFade( work->originalColor, work->transDest, work->colorNum, evy, work->fadeColor ); 

  // �A�j���[�V�����t���[�������C���N�������g
  work->frameCount++;
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������X�V���� ( sin �t�F�[�h�E�x )
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateAnime_SIN_FADE_SLOW( PALETTE_ANIME* work )
{
  u8 evy;

  GF_ASSERT( work->animeFlag ); // �A�j���[�V������~��
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�

  // �t�F�[�h�W��������
  {
    int idx;
    float sinVal;
    idx = (work->frameCount * 65536 / SIN_FADE_SLOW_ONE_CYCLE_FRAMES) % 65536;
    sinVal = FX_FX16_TO_F32( FX_SinIdx( idx ) );
    evy = (int)( sinVal * 16 + 16 );
    evy = SIN_FADE_SLOW_MIN_EVY + (SIN_FADE_SLOW_MAX_EVY - SIN_FADE_SLOW_MIN_EVY) * evy / 32;
  }

  // �p���b�g��������
  SoftFade( work->originalColor, work->transDest, work->colorNum, evy, work->fadeColor ); 

  // �A�j���[�V�����t���[�������C���N�������g
  work->frameCount++;
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������X�V���� ( �_�� )
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateAnime_BLINK( PALETTE_ANIME* work )
{
  u8 evy;

  GF_ASSERT( work->animeFlag ); // �A�j���[�V������~��
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�
  GF_ASSERT( work->frameCount <= BLINK_FRAMES ); // �s���Ăяo��

  // �t�F�[�h�W��������
  if( ((work->frameCount / BLINK_WAIT_FRAMES) % 2) == 0 ) {
    evy = BLINK_MIN_EVY;
  }
  else {
    evy = BLINK_MAX_EVY;
  }

  // �p���b�g��������
  SoftFade( work->originalColor, work->transDest, work->colorNum, evy, work->fadeColor ); 

  // �A�j���[�V�����t���[�������C���N�������g
  work->frameCount++;

  // �A�j���[�V�����I��
  if( BLINK_FRAMES < work->frameCount ) {
    StopAnime( work );
    ResetPalette( work );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������X�V���� ( �_�ŁE�Z )
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateAnime_BLINK_SHORT( PALETTE_ANIME* work )
{
  u8 evy;

  GF_ASSERT( work->animeFlag ); // �A�j���[�V������~��
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�
  GF_ASSERT( work->frameCount <= BLINK_SHORT_FRAMES ); // �s���Ăяo��

  // �t�F�[�h�W��������
  if( ((work->frameCount / BLINK_SHORT_WAIT_FRAMES) % 2) == 0 ) {
    evy = BLINK_SHORT_MIN_EVY;
  }
  else {
    evy = BLINK_SHORT_MAX_EVY;
  }

  // �p���b�g��������
  SoftFade( work->originalColor, work->transDest, work->colorNum, evy, work->fadeColor ); 

  // �A�j���[�V�����t���[�������C���N�������g
  work->frameCount++;

  // �A�j���[�V�����I��
  if( BLINK_SHORT_FRAMES < work->frameCount ) {
    StopAnime( work );
    ResetPalette( work );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������X�V���� ( �_�ŁE�� )
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateAnime_BLINK_LONG( PALETTE_ANIME* work )
{
  u8 evy;

  GF_ASSERT( work->animeFlag ); // �A�j���[�V������~��
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�
  GF_ASSERT( work->frameCount <= BLINK_LONG_FRAMES ); // �s���Ăяo��

  // �t�F�[�h�W��������
  if( ((work->frameCount / BLINK_LONG_WAIT_FRAMES) % 2) == 0 ) {
    evy = BLINK_LONG_MIN_EVY;
  }
  else {
    evy = BLINK_LONG_MAX_EVY;
  }

  // �p���b�g��������
  SoftFade( work->originalColor, work->transDest, work->colorNum, evy, work->fadeColor ); 

  // �A�j���[�V�����t���[�������C���N�������g
  work->frameCount++;

  // �A�j���[�V�����I��
  if( BLINK_LONG_FRAMES < work->frameCount ) {
    StopAnime( work );
    ResetPalette( work );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������X�V���� ( �_�ŁE�� )
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateAnime_BLINK_BRIGHT( PALETTE_ANIME* work )
{
  u8 evy;

  GF_ASSERT( work->animeFlag ); // �A�j���[�V������~��
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�
  GF_ASSERT( work->frameCount <= BLINK_BRIGHT_FRAMES ); // �s���Ăяo��

  // �t�F�[�h�W��������
  if( ((work->frameCount / BLINK_BRIGHT_WAIT_FRAMES) % 2) == 0 ) {
    evy = BLINK_BRIGHT_MIN_EVY;
  }
  else {
    evy = BLINK_BRIGHT_MAX_EVY;
  }

  // �p���b�g��������
  SoftFade( work->originalColor, work->transDest, work->colorNum, evy, work->fadeColor ); 

  // �A�j���[�V�����t���[�������C���N�������g
  work->frameCount++;

  // �A�j���[�V�����I��
  if( BLINK_BRIGHT_FRAMES < work->frameCount ) {
    StopAnime( work );
    ResetPalette( work );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������X�V���� ( �Ó] )
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateAnime_HOLD( PALETTE_ANIME* work )
{
  u8 evy;

  GF_ASSERT( work->animeFlag ); // �A�j���[�V������~��
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�

  // �t�F�[�h�W��������
  if( work->frameCount < HOLD_FRAMES ) {
    evy = HOLD_MIN_EVY 
        + (HOLD_MAX_EVY - HOLD_MIN_EVY) * work->frameCount / HOLD_FRAMES;
  }
  else {
    evy = HOLD_MAX_EVY;
  }

  // �p���b�g��������
  SoftFade( work->originalColor, work->transDest, work->colorNum, evy, work->fadeColor ); 

  // �A�j���[�V�����t���[�������C���N�������g
  work->frameCount++;
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������X�V���� ( �Ó]����̕��A )
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateAnime_RECOVER( PALETTE_ANIME* work )
{
  u8 evy;

  GF_ASSERT( work->animeFlag ); // �A�j���[�V������~��
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�

  // �t�F�[�h�W��������
  evy = RECOVER_MAX_EVY 
      - (RECOVER_MAX_EVY - RECOVER_MIN_EVY) * work->frameCount / RECOVER_FRAMES;

  // �p���b�g��������
  SoftFade( work->originalColor, work->transDest, work->colorNum, evy, work->fadeColor ); 

  // �A�j���[�V�����t���[�������C���N�������g
  work->frameCount++;

  // �A�j���[�V�����I��
  if( RECOVER_FRAMES < work->frameCount ) {
    StopAnime( work );
    ResetPalette( work );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �p���b�g�����Z�b�g����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void ResetPalette( PALETTE_ANIME* work )
{
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�

  MI_CpuCopy8( work->originalColor, work->transDest, sizeof(u16) * work->colorNum );
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V���������ǂ������`�F�b�N����
 *
 * @param work
 *
 * @return �A�j���[�V�������Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------
static BOOL CheckAnime( const PALETTE_ANIME* work )
{
  return work->animeFlag;
}


//=============================================================================
// �������E�j��
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N�𐶐�����
 *
 * @param heapID �g�p����q�[�vID
 * @return �����������[�N
 */
//-----------------------------------------------------------------------------
static PALETTE_ANIME* CreateWork( HEAPID heapID )
{
  PALETTE_ANIME* work;

  work = GFL_HEAP_AllocMemory( heapID, sizeof(PALETTE_ANIME) );
  work->heapID = heapID; // �������̃q�[�vID���L��
  return work;
}

//-----------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void DeleteWork( PALETTE_ANIME* work )
{
  GF_ASSERT( work->setupFlag == FALSE ); // �N���[���A�b�v������Ă��Ȃ�

  GFL_HEAP_FreeMemory( work );
}

//-----------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void InitWork( PALETTE_ANIME* work )
{
  HEAPID heapID;

  // �q�[�vID��ޔ�
  heapID = work->heapID;

  // ���[�N��0�N���A
  GFL_STD_MemClear( work, sizeof(PALETTE_ANIME) );

  // �q�[�vID�𕜋A
  work->heapID = heapID;
}

//-----------------------------------------------------------------------------
/**
 * @brief �p���b�g�A�j���[�V�������[�N���Z�b�g�A�b�v����
 *
 * @param work
 * @param dest     �p���b�g�f�[�^�̓]����A�h���X
 * @param src      �A�j���[�V�����̌��f�[�^�̃A�h���X
 * @param colorNum �A�j���[�V��������F�̐� ( �p���b�g1�{ = 16 )
 */
//-----------------------------------------------------------------------------
static void SetupWork( PALETTE_ANIME* work, u16* dest, u16* src, u8 colorNum )
{
  int i;

  GF_ASSERT( work->setupFlag == FALSE ); // ���łɃZ�b�g�A�b�v����Ă���

  work->setupFlag = TRUE;
  work->transDest = dest;
  work->colorNum  = colorNum;

  // ���f�[�^�̃��[�N�𐶐�
  work->originalColor = GFL_HEAP_AllocMemory( work->heapID, sizeof(u16) * colorNum );

  // ���f�[�^���R�s�[
  for( i=0; i < colorNum; i++ )
  {
    work->originalColor[i] = src[i];
  }
}

//-----------------------------------------------------------------------------
 /**
  * @brief �p���b�g�A�j���[�V�������[�N���N���[���A�b�v����
  *
  * @param work
  */
//-----------------------------------------------------------------------------
static void CleanUpWork( PALETTE_ANIME* work )
{
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�

  // ���f�[�^�̃��[�N��j��
  GFL_HEAP_FreeMemory( work->originalColor );

  // �Z�b�g�A�b�v�t���O�𕚂���
  work->setupFlag = FALSE;
}
