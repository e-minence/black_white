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
// �J�[�\��ON�̖���
#define CURSOR_ON_ANIME_ONE_CYCLE_FRAMES (120) // ����1��̃t���[����
#define CURSOR_ON_ANIME_MIN_EVY            (0) // �ŏ��t�F�[�h�W���P
#define CURSOR_ON_ANIME_MAX_EVY            (4) // �ő�t�F�[�h�W��

// �I�����̖���
#define SELECT_ANIME_FRAMES      (20) // �A�j���[�V�����t���[����
#define SELECT_ANIME_BLINK_FRAMES (4) // �_�ő҂��t���[����
#define SELECT_ANIME_MIN_EVY      (0) // �ŏ��t�F�[�h�W���P
#define SELECT_ANIME_MAX_EVY     (10) // �ő�t�F�[�h�W��



//=============================================================================
// ���p���b�g�A�j���[�V�������s���[�N
//=============================================================================
struct _PALETTE_ANIME
{
  HEAPID heapID;

  BOOL setupFlag;     // �Z�b�g�A�b�v����Ă��邩�ǂ���
  u16* transDest;     // �]����A�h���X
  u8   colorNum;      // �A�j���[�V��������J���[�� ( �p���b�g1�{ = 16 )
  u16* originalColor; // �A�j���[�V�������f�[�^
  u16  fadeColor;     // �t�F�[�h�J���[

  BOOL       animeFlag;  // �A�j���[�V���������ǂ���
  ANIME_TYPE animeType;  // �A�j���[�V�����^�C�v
  u32        frameCount; // �t���[�����J�E���^
};




//=============================================================================
// ������E����
//=============================================================================
static void StartAnime( PALETTE_ANIME* work, ANIME_TYPE type, u16 color ); // �A�j���[�V�������J�n����
static void StopAnime( PALETTE_ANIME* work ); // �A�j���[�V�������~����
static void UpdateAnime( PALETTE_ANIME* work ); // �A�j���[�V�������X�V����
static void UpdateAnime_CURSOR_ON( PALETTE_ANIME* work ); // �A�j���[�V�������X�V���� ( �J�[�\��ON�̖��� )
static void UpdateAnime_SELECT( PALETTE_ANIME* work ); // �A�j���[�V�������X�V���� ( �I�����̖��� )
static void ResetPalette( PALETTE_ANIME* work ); // �p���b�g�����Z�b�g����
//=============================================================================
// �������E�j��
//=============================================================================
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
  case ANIME_TYPE_CURSOR_ON: UpdateAnime_CURSOR_ON( work ); break;
  case ANIME_TYPE_SELECT:    UpdateAnime_SELECT( work );    break;
  default: GF_ASSERT(0);
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������X�V���� ( �J�[�\��ON�̖��� )
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateAnime_CURSOR_ON( PALETTE_ANIME* work )
{
  u8 evy;

  GF_ASSERT( work->animeFlag ); // �A�j���[�V������~��
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�

  // �t�F�[�h�W��������
  {
    int idx;
    float sinVal;
    idx = (work->frameCount * 65536 / CURSOR_ON_ANIME_ONE_CYCLE_FRAMES) % 65536;
    sinVal = FX_FX16_TO_F32( FX_SinIdx( idx ) );
    evy = (int)( sinVal * 16 + 16 );
    evy = CURSOR_ON_ANIME_MIN_EVY + (CURSOR_ON_ANIME_MAX_EVY - CURSOR_ON_ANIME_MIN_EVY) * evy / 32;
  }

  // �p���b�g��������
  SoftFade( work->originalColor, work->transDest, work->colorNum, evy, work->fadeColor ); 

  // �A�j���[�V�����t���[�������C���N�������g
  work->frameCount++;
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������X�V���� ( �I�����̖��� )
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void UpdateAnime_SELECT( PALETTE_ANIME* work )
{
  u8 evy;

  GF_ASSERT( work->animeFlag ); // �A�j���[�V������~��
  GF_ASSERT( work->setupFlag ); // �Z�b�g�A�b�v����Ă��Ȃ�

  // �t�F�[�h�W��������
  if( ((work->frameCount / SELECT_ANIME_BLINK_FRAMES) % 2) == 0 ) {
    evy = SELECT_ANIME_MIN_EVY;
  }
  else {
    evy = SELECT_ANIME_MAX_EVY;
  }

  // �p���b�g��������
  SoftFade( work->originalColor, work->transDest, work->colorNum, evy, work->fadeColor ); 

  // �A�j���[�V�����t���[�������C���N�������g
  work->frameCount++;

  // �A�j���[�V�����I��
  if( SELECT_ANIME_FRAMES < work->frameCount )
  {
    work->animeFlag = FALSE;
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
  work->heapID = heapID;
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
  // �N���[���A�b�v������Ă��Ȃ�
  GF_ASSERT( work->setupFlag == FALSE );

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

  // 0�N���A
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

  // ���łɃZ�b�g�A�b�v����Ă���
  GF_ASSERT( work->setupFlag == FALSE ); 

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
  // �Z�b�g�A�b�v����Ă��Ȃ�
  GF_ASSERT( work->setupFlag );

  // ���f�[�^�̃��[�N��j��
  GFL_HEAP_FreeMemory( work->originalColor );

  // ���[�N��������
  InitWork( work );
}
