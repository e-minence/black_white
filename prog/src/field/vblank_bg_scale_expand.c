///////////////////////////////////////////////////////////////////////////////
/**
 * @brief  VBlank���荞�݊��Ԓ���BG�A�t�B���ϊ�
 * @file   vblank_bg_scale_expand.c
 * @author obata
 * @date   2010.02.05
 *
 * �}�b�v�`�F���W���̉�ʒ�~���Ԓ���BG�ʂ��g�傳���邽�߂�VBlank�֐�.
 * �N���X�t�F�[�h�J�n���ɃL���v�`������BG�ʂɑ΂���, �A�t�B���ϊ���K�p����.
 * ����ȊO�̏�ʂł̎g�p�͍l�����Ă��܂���B
 * ����Ώ�BG�ʂ̓��C����ʂ�BG2�ł��B
 */
///////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "vblank_bg_scale_expand.h"
#include "system/gfl_use.h"

#define MAX_SCALE (1.0f)
#define MAX_FRAME (150)
#define CENTER_X  (128)
#define CENTER_Y  (92)
#define SCROLL_X  (0)
#define SCROLL_Y  (0)


static int   waitCount;  // �҂����ԃJ�E���^
static int   scaleCount; // �t���[���J�E���^
static float scale;      // �g�嗦

int   waitFrame = 0;
int   maxFrame  = 180;
float maxScale  = 1.8f;
float minScale  = 1.0f;
float scaleUpSpeed = 0.0044f;

BOOL  scrollFlag = FALSE;
float scrollSize = 60;

//-----------------------------------------------------------------------------
/**
 * @brief VBlank���荞�ݏ���
 */
//-----------------------------------------------------------------------------
static void VBlankFunc_BGExpand( void )
{
  MtxFx22 matrix;
  fx32 fxScale;
  float frame = 0;
  int scrollY;

  waitCount++;
  if( waitCount < waitFrame ){ return; }

//-------------------------------------------------------------------

#if 0 // 2010.03.08 �g�嗦�̏�����Ȃ��� �� ��ԂłȂ�, �P���ɉ��Z����
  // �g�嗦�X�V
  scaleCount++; 
  frame = scaleCount;
  if( maxFrame < frame ){ frame = maxFrame; }
  OS_Printf( "scaleCount = %d\n", scaleCount );

  // VBlank���ԊO
  if( GX_IsVBlank() == FALSE ){ return; }

  // ���g�嗦���Z�o
  {
    float t, offset;
    t = frame / (float)maxFrame;
    scale = minScale + t * (maxScale - minScale);
    fxScale = FX32_CONST( 1.0f / scale );
  }
#endif
  // VBlank���ԊO
  if( GX_IsVBlank() == FALSE ){ return; }

  // ���g�嗦���Z�o
  scale += scaleUpSpeed;
  fxScale = FX32_CONST( 1.0f / scale );

//-------------------------------------------------------------------

  // �X�N���[����������
  if( scrollFlag ) {
    scrollY = scrollSize * frame / (float)maxFrame;
  }
  else {
    scrollY = 0;
  }

  // �A�t�B���ϊ�
  MTX_Scale22( &matrix, fxScale, fxScale );
  G2_SetBG2Affine( &matrix, CENTER_X, CENTER_Y, SCROLL_X, scrollY );
}


//-----------------------------------------------------------------------------
/**
 * @brief BG�̊g����J�n����
 */
//-----------------------------------------------------------------------------
void StartVBlankBGExpand()
{
  // �g�嗦��������
  scale      = 1.0f;
  scaleCount = 0;
  waitCount  = 0;

  // VBlank�֐���o�^
  GFUser_SetVIntrFunc( VBlankFunc_BGExpand );
}

//-----------------------------------------------------------------------------
/**
 * @brief BG�̊g����I������
 */
//-----------------------------------------------------------------------------
void EndVBlankBGExpand()
{
  MtxFx22 matrix;

  // �A�t�B���ϊ�������
  MTX_Identity22( &matrix );
  G2_SetBG2Affine( &matrix, CENTER_X, CENTER_Y, SCROLL_X, SCROLL_Y );

  // VBlank�֐�������
  GFUser_ResetVIntrFunc();
}
