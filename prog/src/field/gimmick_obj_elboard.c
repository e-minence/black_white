////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief �d���f����
 * @file  gimmick_obj_elboard.c
 * @author obata
 * @date   2009.10.22
 *
 */
////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gimmick_obj_elboard.h"
#include "system/el_scoreboard.h"
#include "print/printsys.h"  // for PRINTSYS_GetStrWidth
#include "arc/arc_def.h"
#include "arc/font/font.naix"


//==================================================================================
// ���萔
//==================================================================================
#define WRITE_POS_Y  (1)  // �����񏑂����ݐ�y���W
#define CHAR_WIDTH  (13)  // ������[dot]
#define CHAR_HEIGHT (16)  // ��������[dot]
#define MARGIN_SIZE  (1)  // �����ԃX�y�[�X��
#define TEX_WIDTH (1024)  // �e�N�X�`���̕�
#define TEX_HEIGHT  (16)  // �e�N�X�`���̍���
#define MAX_STRLEN (128)  // �j���[�X1������̍ő啶����


//==================================================================================
// ���j���[�X�Ǘ����[�N
//==================================================================================
typedef struct
{
  u8                index; // ���g�̌f�����C���f�b�N�X
  BOOL           dispFlag; // �\�������ǂ���
  BOOL         switchFlag; // ���̃j���[�X�\���X�C�b�`�����������ǂ���
  EL_SCOREBOARD_TEX*  tex; // �e�N�X�`��
  GFL_G3D_OBJ*     g3dObj; // �f����3D�I�u�W�F�N�g
  u16          animeIndex; // �A�j���[�V�����ԍ�
  fx32           nowFrame; // ���݂̃A�j���[�V�����t���[����
  fx32        switchFrame; // ���̃j���[�X�J�n�t���[����
  fx32           endFrame; // �\���I���t���[����
} NEWS;


//==================================================================================
// ���d���f�����[�N
//==================================================================================
struct _GOBJ_ELBOARD
{
  HEAPID                 heapID;  // �g�p����q�[�vID
  u8                    newsNum;  // �\������j���[�X�̐�
  u8                 maxNewsNum;  // �\���\�ȃj���[�X�̍ő吔
  u8                   dispSize;  // ���ʂɕ\���\�ȕ�����
  u8               newsInterval;  // �j���[�X�Ԃ̊Ԋu(������)
  NEWS**                   news;  // �j���[�X�z��
  BOOL*              newsSwitch;  // �j���[�X�\���X�C�b�`�z��
  GFL_G3D_OBJ*           g3dObj;  // �f����3D�I�u�W�F�N�g
  GFL_FONT*                font;  // �j���[�X�`��Ɏg�p����t�H���g
  fx32                    frame;  // ����t���[���J�E���^
};


//==================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==================================================================================
static NEWS* NewsCreate( GOBJ_ELBOARD* elboard, const NEWS_PARAM* param, int index );
static void NewsDelete( NEWS* news );
static void NewsMove_Start( NEWS* news );
static void NewsMove_Update( GOBJ_ELBOARD* elboard, NEWS* news, fx32 frame );
static void NewsMove_End( NEWS* news );


//==================================================================================
// ��������
//==================================================================================

//----------------------------------------------------------------------------------
/**
 * @brief �d���f�����쐬����
 *
 * @param param �f���p�����[�^
 *
 * @return �쐬�����d���f����
 */
//----------------------------------------------------------------------------------
GOBJ_ELBOARD* GOBJ_ELBOARD_Create( ELBOARD_PARAM* param )
{
  int i;
  GOBJ_ELBOARD* elboard;

  // �C���X�^���X����
  elboard = GFL_HEAP_AllocMemory( param->heapID, sizeof(GOBJ_ELBOARD) );

  // ������
  elboard->heapID       = param->heapID;
  elboard->newsNum      = 0;
  elboard->maxNewsNum   = param->maxNewsNum;
  elboard->dispSize     = param->dispSize;
  elboard->newsInterval = param->newsInterval;
  elboard->g3dObj       = param->g3dObj;
  elboard->frame        = 0;

  // �j���[�X�z��E�\���X�C�b�`�z����m��
  elboard->news = GFL_HEAP_AllocMemory(
      param->heapID, sizeof(NEWS*)*param->maxNewsNum );
  elboard->newsSwitch = GFL_HEAP_AllocMemory(
      param->heapID, sizeof(BOOL)*param->maxNewsNum );

  // �A�j���[�V������Ԃ�������
  for( i=0; i<param->maxNewsNum; i++ )
  {
    elboard->news[i]       = NULL;
    elboard->newsSwitch[i] = FALSE;
  }
  elboard->newsSwitch[0] = TRUE;  // �ŏ��̃j���[�X��\������

  // �j���[�X�`��Ɏg�p����t�H���g���쐬
  elboard->font = GFL_FONT_Create(
      ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, elboard->heapID );

  return elboard;
}

//----------------------------------------------------------------------------------
/**
 * @brief �\������j���[�X��ǉ�����
 *
 * @param elboard    �ǉ���̌f����
 * @param news_param �ǉ�����j���[�X
 */
//----------------------------------------------------------------------------------
void GOBJ_ELBOARD_AddNews( GOBJ_ELBOARD* elboard, const NEWS_PARAM* news_param )
{
  // ���łɍő吔��o�^�ς� ==> �o�^���Ȃ�
  if( elboard->maxNewsNum <= elboard->newsNum )
  {
    OBATA_Printf( "------------------------------------------\n" );
    OBATA_Printf( "���łɍő吔�̃j���[�X���o�^����Ă��܂��B\n" );
    OBATA_Printf( "------------------------------------------\n" );
    return;
  }

  // �o�^
  elboard->news[ elboard->newsNum ] =
    NewsCreate( elboard, news_param, elboard->newsNum );
  elboard->newsNum++;
}



//==================================================================================
// ���j��
//==================================================================================

//----------------------------------------------------------------------------------
/**
 * @brief �d���f����j������
 *
 * @param elboard �j������f����
 */
//----------------------------------------------------------------------------------
void GOBJ_ELBOARD_Delete( GOBJ_ELBOARD* elboard )
{
  int i;

  // �t�H���g��j��
  GFL_FONT_Delete( elboard->font );

  // �e�j���[�X��j��
  for( i=0; i<elboard->newsNum; i++ )
  {
    NewsDelete( elboard->news[i] );
  }
  GFL_HEAP_FreeMemory( elboard->news );

  // �\���X�C�b�`�z���j��
  GFL_HEAP_FreeMemory( elboard->newsSwitch );

  // �f�����[�N�{�̂�j��
  GFL_HEAP_FreeMemory( elboard );
}


//==================================================================================
// ������
//==================================================================================

//----------------------------------------------------------------------------------
/**
 * @brief �d���f�����C������
 *
 * @param elboard �������f����
 * @param frame   ����t���[����
 */
//----------------------------------------------------------------------------------
void GOBJ_ELBOARD_Main( GOBJ_ELBOARD* elboard, fx32 frame )
{
  int i;

  // �e�j���[�X�𓮂���
  for( i=0; i<elboard->newsNum; i++ )
  {
    NewsMove_Update( elboard, elboard->news[i], frame );
  }

  // ����t���[�����X�V
  elboard->frame += frame;

  
}


//==================================================================================
// ���擾
//==================================================================================

//----------------------------------------------------------------------------------
/**
 * @brief ���݂̓���t���[�������擾����
 *
 * @param elboard �擾�Ώۂ̌f����
 *
 * @return �w�肵���f���̌��݂̓���t���[����
 */
//----------------------------------------------------------------------------------
fx32 GOBJ_ELBOARD_GetFrame( const GOBJ_ELBOARD* elboard )
{
  return elboard->frame;
}

//----------------------------------------------------------------------------------
/**
 * @brief �d���f�����g�p����q�[�vID���擾����
 *
 * @param elboard �擾�Ώۂ̌f����
 *
 * @return �f�����g�p����q�[�vID
 */
//----------------------------------------------------------------------------------
HEAPID GOBJ_ELBOARD_GetHeapID( const GOBJ_ELBOARD* elboard )
{
  return elboard->heapID;
}

//----------------------------------------------------------------------------------
/**
 * @brief �\������j���[�X�̐����擾����
 *
 * @param elboard �擾�Ώۂ̌f����
 *
 * @return �w�肵���f�����\������j���[�X�̐�
 */
//----------------------------------------------------------------------------------
u8 GOBJ_ELBOARD_GetNewsNum( const GOBJ_ELBOARD* elboard )
{
  return elboard->newsNum;
}


//==================================================================================
// ���ݒ�
//==================================================================================

//----------------------------------------------------------------------------------
/**
 * @brief ����t���[������ݒ肷��
 *
 * @param elboard �ݒ�Ώۂ̌f����
 * @param frame   �ݒ肷��t���[����
 */
//----------------------------------------------------------------------------------
void GOBJ_ELBOARD_SetFrame( GOBJ_ELBOARD* elboard, fx32 frame )
{
  int i;
  int set_frame = FX_Whole( frame );
  int one_round_frame = 0;  // ����ɗv����t���[����
  int* start_frame =        // �e�j���[�X�̃X�^�[�g�t���[��
    GFL_HEAP_AllocMemoryLo( elboard->heapID, sizeof(int)*elboard->newsNum );
  int* now_frame =        // �e�j���[�X�̌��݃t���[��
    GFL_HEAP_AllocMemoryLo( elboard->heapID, sizeof(int)*elboard->newsNum );

  // ���݃t���[�������L��
  elboard->frame = frame;

  // �O����
  for( i=0; i<elboard->newsNum; i++ )
  {
    // �j���[�X�\���X�C�b�`
    elboard->newsSwitch[i] = FALSE;

    // ����ɗv����t���[�������v�Z
    one_round_frame += FX_Whole( elboard->news[i]->switchFrame );

    // �X�^�[�g�t���[�������߂�
    if( i == 0 )
    { // �ŏ��̃j���[�X = 0
      start_frame[i] = 0;
    }
    else
    {
      // �O�j���[�X�̃X�^�[�g�t���[�� + �O�j���[�X�̃X�C�b�`�t���[����
      start_frame[i] =
        start_frame[i-1] + FX_Whole( elboard->news[i-1]->switchFrame );
    }
  }

  // �e�j���[�X�̌��݃t���[�������v�Z
  for( i=0; i<elboard->newsNum; i++ )
  {
    NEWS* news = elboard->news[i];
    now_frame[i] = (set_frame % one_round_frame) - start_frame[i];
    news->dispFlag = (0 < now_frame[i]) && (now_frame[i] < FX_Whole(news->endFrame));
    news->switchFlag = (FX_Whole(news->switchFrame) < now_frame[i]) && (now_frame[i] < FX_Whole(news->endFrame));
    if( now_frame[i] < 0 ) { now_frame[i] = 0; }
    news->nowFrame = now_frame[i] << FX32_SHIFT;

    GFL_G3D_OBJECT_SetAnimeFrame( news->g3dObj, news->animeIndex, (int*)&news->nowFrame );
    if( news->dispFlag ) {
      GFL_G3D_OBJECT_EnableAnime( news->g3dObj, news->animeIndex );
    }
    else {
      GFL_G3D_OBJECT_DisableAnime( news->g3dObj, news->animeIndex );
    }
  }

  // 0�t���[���J�n�Ȃ�, �ŏ��̃j���[�X��\������
  if( (0 < elboard->newsNum) && (set_frame == 0) ) {
    NEWS* news = elboard->news[0];
    news->dispFlag = TRUE;
    GFL_G3D_OBJECT_EnableAnime( news->g3dObj, news->animeIndex );
  }

  // �j���[�X�X�C�b�`�̏�Ԃ��Z�o
  for( i=0; i<elboard->newsNum; i++ )
  {
    int next = (i+1)%elboard->newsNum;
    elboard->newsSwitch[next] =
      elboard->news[i]->switchFlag && !elboard->news[next]->dispFlag;
  }

  // DEBUG:
  OBATA_Printf( "set_frame = %d\n", set_frame );
  OBATA_Printf( "one_round_frame = %d\n", one_round_frame );
  for( i=0; i<elboard->newsNum; i++ )
    OBATA_Printf( "start_frame[%d] = %d\n", i, start_frame[i] );
  for( i=0; i<elboard->newsNum; i++ )
    OBATA_Printf( "now_frame[%d] = %d\n", i, now_frame[i] );
  for( i=0; i<elboard->newsNum; i++ )
    OBATA_Printf( "news[%d]->dispFlag = %d\n", i, elboard->news[i]->dispFlag );
  for( i=0; i<elboard->newsNum; i++ )
    OBATA_Printf( "newsSwitch[%d] = %d\n", i, elboard->newsSwitch[i] );

  // ��n��
  GFL_HEAP_FreeMemory( start_frame );
  GFL_HEAP_FreeMemory( now_frame );
}


//==================================================================================
// ������J�֐�
//==================================================================================

//----------------------------------------------------------------------------------
/**
 * @brief �j���[�X���쐬����
 *
 * @param elboard �ǉ���̓d���f����
 * @param param   �j���[�X�p�����[�^
 * @param index   �j���[�X�ԍ�
 *
 * @return �쐬�����j���[�X
 */
//----------------------------------------------------------------------------------
static NEWS* NewsCreate( GOBJ_ELBOARD* elboard, const NEWS_PARAM* param, int index )
{
  NEWS* news;
  u32 str_width; // ������̕�

  // �C���X�^���X����
  news = GFL_HEAP_AllocMemory( elboard->heapID, sizeof(NEWS) );

  // �e�N�X�`�����쐬
  {
    GFL_MSGDATA* msg; // ���b�Z�[�W�f�[�^
    STRBUF*   strbuf; // �`�敶����
    GFL_G3D_RND* rnd;
    GFL_G3D_RES* res;

    // �`�悷�镶������擾
    msg = GFL_MSG_Create(
        GFL_MSG_LOAD_NORMAL, param->msgArcID, param->msgDatID, elboard->heapID );
    if( param->wordset )
    { // WORDSET�W�J
      STRBUF* strbuf_msg;
      strbuf = GFL_STR_CreateBuffer( MAX_STRLEN, elboard->heapID );
      strbuf_msg = GFL_MSG_CreateString( msg, param->msgStrID );
      WORDSET_ExpandStr( param->wordset, strbuf, strbuf_msg );
      GFL_HEAP_FreeMemory( strbuf_msg );
    }
    else
    { // ���b�Z�[�W�����̂܂܎g�p
      strbuf = GFL_MSG_CreateString( msg, param->msgStrID );
    }
    str_width = PRINTSYS_GetStrWidth( strbuf, elboard->font, MARGIN_SIZE );// ������̕����擾
    // �`���e�N�X�`�����擾
    rnd = GFL_G3D_OBJECT_GetG3Drnd( elboard->g3dObj );
    res = GFL_G3D_RENDER_GetG3DresTex( rnd );
    // �`��
    news->tex = ELBOARD_TEX_Add_Ex( res, param->texName, param->pltName, strbuf,
                                    (CHAR_WIDTH + MARGIN_SIZE) * elboard->dispSize, 
                                    WRITE_POS_Y, elboard->heapID );
    // ��n��
    GFL_HEAP_FreeMemory( strbuf );
    GFL_MSG_Delete( msg );
  }

  // ����ɕK�v�ȃA�j���[�V�����t���[�����v�Z
  {
    GFL_G3D_ANM* anm;
    NNSG3dAnmObj* anm_obj;
    fx32 frame_size;  // �S�A�j���[�V�����t���[����
    int left_pos, right_pos, switch_pos;  // ���[, �E�[, �X�C�b�`���W

    // �S�A�j���[�V�����t���[�������擾
    anm        = GFL_G3D_OBJECT_GetG3Danm( elboard->g3dObj, param->animeIndex );
    anm_obj    = GFL_G3D_ANIME_GetAnmObj( anm );
    frame_size = NNS_G3dAnmObjGetNumFrame( anm_obj );
    // �v�Z�ɕK�v�ȃe�N�X�`�����W���Z�o
    left_pos   = CHAR_WIDTH * elboard->dispSize;
    right_pos  = left_pos + str_width;
    switch_pos = right_pos - left_pos + (elboard->newsInterval * CHAR_WIDTH);
    // �e�퐧��t���[�������v�Z
    news->nowFrame    = 0;
    news->switchFrame = FX_F32_TO_FX32( ((float)switch_pos/(float)TEX_WIDTH) );
    news->switchFrame = FX_Mul( news->switchFrame, frame_size );
    news->endFrame    = FX_F32_TO_FX32( ((float)right_pos/(float)TEX_WIDTH) );
    news->endFrame    = FX_Mul( news->endFrame, frame_size );
    OBATA_Printf( "switchFrame = %d\n", FX_Whole(news->switchFrame) );
    OBATA_Printf( "endFrame = %d\n", FX_Whole(news->endFrame) );
  }

  // ���̑��̏�����
  news->index      = index;
  news->dispFlag   = FALSE;
  news->switchFlag = FALSE;
  news->g3dObj     = elboard->g3dObj;
  news->animeIndex = param->animeIndex;

  // ��\����Ԃɂ���
  GFL_G3D_OBJECT_DisableAnime( news->g3dObj, news->animeIndex );
  GFL_G3D_OBJECT_ResetAnimeFrame( news->g3dObj, news->animeIndex );

  // �쐬�����j���[�X��Ԃ�
  return news;
}

//----------------------------------------------------------------------------------
/**
 * @brief �j���[�X��j������
 *
 * @param news �j������j���[�X
 */
//----------------------------------------------------------------------------------
static void NewsDelete( NEWS* news )
{
  // �e�N�X�`����j��
  ELBOARD_TEX_Delete( news->tex );

  // �j���[�X�{�̂�j��
  GFL_HEAP_FreeMemory( news );
}

//----------------------------------------------------------------------------------
/**
 * @brief �w�肵���j���[�X�̕\�����J�n����
 *
 * @param �\�����J�n����j���[�X
 */
//----------------------------------------------------------------------------------
static void NewsMove_Start( NEWS* news )
{
  // ��\����ԂȂ�, �\���J�n
  if( news->dispFlag != TRUE )
  {
    news->dispFlag   = TRUE;
    news->switchFlag = FALSE;
    news->nowFrame   = 0;
    GFL_G3D_OBJECT_EnableAnime( news->g3dObj, news->animeIndex );
    GFL_G3D_OBJECT_ResetAnimeFrame( news->g3dObj, news->animeIndex );
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief �w�肵���j���[�X�𓮂���
 *
 * @param elboard �j���[�X���Ǘ�����d���f����
 * @param news    �������j���[�X
 * @param frame   �X�V�t���[����
 */
//----------------------------------------------------------------------------------
static void NewsMove_Update( GOBJ_ELBOARD* elboard, NEWS* news, fx32 frame )
{
  // �\�����̓���
  if( news->dispFlag == TRUE )
  {
    // �A�j���[�V�������X�V
    ELBOARD_TEX_Main( news->tex );
    GFL_G3D_OBJECT_IncAnimeFrame( news->g3dObj, news->animeIndex, frame );
    news->nowFrame += frame;

    // �X�C�b�`�t���[���ɓ��B ==> ���̃j���[�X�̃X�C�b�`��ON
    if( (news->switchFrame < news->nowFrame) &&
        (news->switchFlag != TRUE) )
    {
      int next = (news->index + 1) % elboard->newsNum;
      elboard->newsSwitch[next] = TRUE;
      news->switchFlag = TRUE;  // ��x��������
    }

    // �A�j���[�V�����I���t���[���ɓ��B ==> ����I��
    if( news->endFrame < news->nowFrame )
    {
      NewsMove_End( news );
    }
  }
  // ��\�����̓���
  else
  {
    // ���g�̃X�C�b�`��������Ă�����, �\�����J�n����
    if( elboard->newsSwitch[news->index] == TRUE )
    {
      NewsMove_Start( news );
      elboard->newsSwitch[news->index] = FALSE;
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief �w�肵���j���[�X�̕\�����I������
 *
 * @param news �\�����I������j���[�X
 */
//----------------------------------------------------------------------------------
static void NewsMove_End( NEWS* news )
{
  // �\�����Ȃ�, ��\���ɂ���
  if( news->dispFlag == TRUE )
  {
    news->dispFlag = FALSE;
    GFL_G3D_OBJECT_DisableAnime( news->g3dObj, news->animeIndex );
    GFL_G3D_OBJECT_ResetAnimeFrame( news->g3dObj, news->animeIndex );
  }
}
