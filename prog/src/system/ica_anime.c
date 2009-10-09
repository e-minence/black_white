/////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  ica�A�j���[�V���� �X�g���[�~���O�Đ�
 * @file   ica_anime.c
 * @author obata
 * @date   2009.09.04
 *
 */
/////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "system/ica_anime.h"

// �f�o�b�O�o�̓X�C�b�`
#define ICA_ANIME_DEBUG


//===================================================================================
/**
 * @brief ica�f�[�^�Ǘ����[�N
 */
//===================================================================================
struct _ICA_ANIME
{
  HEAPID     heapID;        // �g�p����q�[�vID
  ARCHANDLE* arcHandle;     // �A�[�J�C�u�n���h��
  ARCDATID   datID;         // �A�[�J�C�u���f�[�^�C���f�b�N�X
  u32        frameSize;     // �A�j���[�V�����̑S�t���[����
  fx32       nowFrame;      // ���݂̃t���[����
  int        bufStartFrame; // �o�b�t�@�擪�f�[�^�̃t���[����
  int        bufSize;       // �o�b�t�@�T�C�Y
  VecFx32*   transBuf;      // ���s�ړ��o�b�t�@
  VecFx32*   rotateBuf;     // ��]�o�b�t�@
  VecFx32*   scaleBuf;      // �g�k�o�b�t�@
  BOOL       haveTrans;     // ���s�ړ��f�[�^�̗L��
  BOOL       haveRotate;    // ��]�f�[�^�̗L��
  BOOL       haveScale;     // �g�k�f�[�^�̗L��
  BOOL       allocMemory;   // �o�b�t�@�m�ۃt���O
};


//===================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//===================================================================================
void InitAnime( ICA_ANIME* anime );
void LoadAnimeInfo( ICA_ANIME* anime, ARCID arc_id, ARCDATID dat_id, HEAPID heap_id );
void AllocBuffer( ICA_ANIME* anime, int buf_size );
BOOL AssignBuffer( ICA_ANIME* anime, int bufsize, void* work, int worksize );
void UpdateBuf( ICA_ANIME* anime, fx32 start_frame );


//===================================================================================
/**
 * @breif ���J�֐��̒�`
 */
//===================================================================================

//===================================================================================
// �쐬�E�j��
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������쐬���� (�f�[�^�ꊇ�ǂݍ���)
 *
 * @param heap_id  �g�p����q�[�vID
 * @param arc_id   �A�j���[�V�����f�[�^�̃A�[�J�C�u�t�@�C���w��
 * @param dat_id   �A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 * @param work     �����o�b�t�@�Ƃ��Ďg�p���郏�[�N
 * @param worksize ���[�N�T�C�Y
 *
 * @return �쐬�����A�j���[�V�����Ǘ��I�u�W�F�N�g
 *         �쐬�Ɏ��s������ NULL
 */
//-----------------------------------------------------------------------------------
ICA_ANIME* ICA_ANIME_Create( 
    HEAPID heap_id, ARCID arc_id, ARCDATID dat_id, void* work, int worksize )
{
  ICA_ANIME* anime;

  // �A�j���[�V�����Ǘ����[�N���m��
  anime = (ICA_ANIME*)GFL_HEAP_AllocMemory( heap_id, sizeof(ICA_ANIME) );

  // ������
  InitAnime( anime );
  LoadAnimeInfo( anime, arc_id, dat_id, heap_id );
  if( !AssignBuffer( anime, anime->frameSize, work, worksize ) ) return NULL;
  UpdateBuf( anime, 0 );

  return anime;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������쐬���� (�X�g���[�~���O�Đ�)
 *
 * @param heap_id      �g�p����q�[�vID
 * @param arc_id       �A�j���[�V�����f�[�^�̃A�[�J�C�u�t�@�C���w��
 * @param dat_id       �A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 * @param buf_interval �f�[�^�ǂݍ��݂̊Ԋu (�w��t���[�����Ɉ�x, �ǂݍ��݂��s��)
 * @param work         �����o�b�t�@�Ƃ��Ďg�p���郏�[�N
 * @param worksize     ���[�N�T�C�Y
 *
 * @return �쐬�����A�j���[�V�����Ǘ��I�u�W�F�N�g
 *         �쐬�Ɏ��s������ NULL
 */
//-----------------------------------------------------------------------------------
ICA_ANIME* ICA_ANIME_CreateStreaming( 
    HEAPID heap_id, ARCID arc_id, ARCDATID dat_id, int buf_interval, void* work, int worksize )
{
  ICA_ANIME* anime;

  // �A�j���[�V�����Ǘ����[�N���m��
  anime = (ICA_ANIME*)GFL_HEAP_AllocMemory( heap_id, sizeof(ICA_ANIME) );

  // ������
  InitAnime( anime );
  LoadAnimeInfo( anime, arc_id, dat_id, heap_id );
  if( !AssignBuffer( anime, buf_interval, work, worksize ) ) return NULL;
  UpdateBuf( anime, 0 );

  return anime;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������쐬����
 *        ( �����Ńo�b�t�@���m�ہE�f�[�^�ꊇ�ǂݍ��� )
 *
 * @param heap_id �g�p����q�[�vID
 * @param arc_id  �A�j���[�V�����f�[�^�̃A�[�J�C�u�t�@�C���w��
 * @param dat_id  �A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 *
 * @return �쐬�����A�j���[�V�����Ǘ��I�u�W�F�N�g
 */
//-----------------------------------------------------------------------------------
ICA_ANIME* ICA_ANIME_CreateAlloc( HEAPID heap_id, ARCID arc_id, ARCDATID dat_id )
{
  ICA_ANIME* anime;

  // �A�j���[�V�����Ǘ����[�N���m��
  anime = (ICA_ANIME*)GFL_HEAP_AllocMemory( heap_id, sizeof(ICA_ANIME) );

  // ������
  InitAnime( anime );
  LoadAnimeInfo( anime, arc_id, dat_id, heap_id );
  AllocBuffer( anime, anime->frameSize );   // �S�t���[�����̃o�b�t�@���K�v
  UpdateBuf( anime, 0 );

  return anime;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������쐬����
 *        ( �����Ńo�b�t�@���m�ہE�X�g���[�~���O�Đ� )
 *
 * @param heap_id      �g�p����q�[�vID
 * @param arc_id       �A�j���[�V�����f�[�^�̃A�[�J�C�u�t�@�C���w��
 * @param dat_id       �A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 * @param buf_interval �f�[�^�ǂݍ��݂̊Ԋu (�w��t���[�����Ɉ�x, �ǂݍ��݂��s��)
 *
 * @return �쐬�����A�j���[�V�����Ǘ��I�u�W�F�N�g
 */
//-----------------------------------------------------------------------------------
ICA_ANIME* ICA_ANIME_CreateStreamingAlloc( 
    HEAPID heap_id, ARCID arc_id, ARCDATID dat_id, int buf_interval )
{
  ICA_ANIME* anime;

  // �A�j���[�V�����Ǘ����[�N���m��
  anime = (ICA_ANIME*)GFL_HEAP_AllocMemory( heap_id, sizeof(ICA_ANIME) );

  // ������
  InitAnime( anime );
  LoadAnimeInfo( anime, arc_id, dat_id, heap_id );
  AllocBuffer( anime, buf_interval );   // �o�b�t�@�T�C�Y�͓ǂݍ��݊Ԋu
  UpdateBuf( anime, 0 );

  return anime;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V������j������
 *
 * @param anime �j������A�j���[�V����
 */
//-----------------------------------------------------------------------------------
void ICA_ANIME_Delete( ICA_ANIME* anime )
{
  GFL_ARC_CloseDataHandle( anime->arcHandle );
  if( anime->allocMemory )  // �e�o�b�t�@�̓N���X���Ŋm�ۂ����ꍇ�̂݉������
  {
    if( anime->transBuf )
    {
      GFL_HEAP_FreeMemory( anime->transBuf );
    }
    if( anime->rotateBuf ) 
    {
      GFL_HEAP_FreeMemory( anime->rotateBuf );
    }
    if( anime->scaleBuf )  
    {
      GFL_HEAP_FreeMemory( anime->scaleBuf );
    }
  }
  GFL_HEAP_FreeMemory( anime );
}


//===================================================================================
// �A�j���[�V�����X�V
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V������i�߂�
 *
 * @param anime �X�V����A�j���[�V����
 * @param frame �i�߂�t���[����
 */
//-----------------------------------------------------------------------------------
void ICA_ANIME_IncAnimeFrame( ICA_ANIME* anime, fx32 frame )
{
  int now;

  // ���݃t���[�����X�V
  anime->nowFrame += frame;
  now = anime->nowFrame >> FX32_SHIFT;

  // �ŏI�t���[���𒴂�����, �[���t���[���ڂɖ߂�
  if( anime->frameSize <= now )
  {
    anime->nowFrame = 0;
    now = 0;
  }

  // ���݃t���[�����o�b�t�@�����O�͈͂��͂��ꂽ��, �o�b�t�@���X�V����
  if( ( now < anime->bufStartFrame ) ||
      ( (anime->bufStartFrame + anime->bufSize) <= now ) )
  {
    UpdateBuf( anime, anime->nowFrame );
  }
}


//===================================================================================
// ���f�[�^�擾
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�����̑��t���[�������擾����
 *
 * @param anime �擾�ΏۃA�j���[�V����
 * @return �w�肵���A�j���[�V�����S�̂̃t���[����
 */
//-----------------------------------------------------------------------------------
u32 ICA_ANIME_GetMaxFrame( ICA_ANIME* anime )
{
  return anime->frameSize;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�����̌��݂̃t���[�������擾����
 *
 * @param anime �擾�ΏۃA�j���[�V����
 * @return �w�肵���A�j���[�V�����S�̂̃t���[����
 */
//-----------------------------------------------------------------------------------
u32 ICA_ANIME_GetNowFrame( ICA_ANIME* anime )
{
  return anime->nowFrame;
}


//===================================================================================
// �A�j���[�V�����擾
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief ���݃t���[���ɂ����镽�s�ړ��f�[�^���擾����
 *
 * @param anime   �擾�ΏۃA�j���[�V����
 * @param vec_dst �擾�����f�[�^�̊i�[��
 *
 * @return �f�[�^���擾�ł����ꍇ TRUE
 */
//-----------------------------------------------------------------------------------
BOOL ICA_ANIME_GetTranslate( const ICA_ANIME* anime, VecFx32* vec_dst )
{
  // �ǂݍ��񂾃t�@�C���ɕ��s�ړ��f�[�^�����݂���ꍇ�̂ݎ擾�\
  if( anime->haveTrans )
  {
    // �o�b�t�@���C���f�b�N�X���v�Z
    int buf_index = (anime->nowFrame>>FX32_SHIFT) - anime->bufStartFrame;

    // ���݃t���[���̒l���擾
    *vec_dst = anime->transBuf[ buf_index ];
    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���݃t���[���ɂ������]�f�[�^���擾����
 *
 * @param anime   �擾�ΏۃA�j���[�V����
 * @param vec_dst �擾�����f�[�^�̊i�[��
 *
 * @return �f�[�^���擾�ł����ꍇ TRUE
 */
//-----------------------------------------------------------------------------------
BOOL ICA_ANIME_GetRotate( const ICA_ANIME* anime, VecFx32* vec_dst )
{
  // �ǂݍ��񂾃t�@�C���ɉ�]�f�[�^�����݂���ꍇ�̂ݎ擾�\
  if( anime->haveRotate )
  {
    // �o�b�t�@���C���f�b�N�X���v�Z
    int buf_index = (anime->nowFrame>>FX32_SHIFT) - anime->bufStartFrame;

    // ���݃t���[���̒l���擾
    *vec_dst = anime->rotateBuf[ buf_index ];
    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���݃t���[���ɂ�����g�k�f�[�^���擾����
 *
 * @param anime   �擾�ΏۃA�j���[�V����
 * @param vec_dst �擾�����f�[�^�̊i�[��
 *
 * @return �f�[�^���擾�ł����ꍇ TRUE
 */
//-----------------------------------------------------------------------------------
BOOL ICA_ANIME_GetScale( const ICA_ANIME* anime, VecFx32* vec_dst )
{
  // �ǂݍ��񂾃t�@�C���Ɋg�k�f�[�^�����݂���ꍇ�̂ݎ擾�\
  if( anime->haveScale )
  {
    // �o�b�t�@���C���f�b�N�X���v�Z
    int buf_index = (anime->nowFrame>>FX32_SHIFT) - anime->bufStartFrame;

    // ���݃t���[���̒l���擾
    *vec_dst = anime->scaleBuf[ buf_index ];
    return TRUE;
  }

  return FALSE;
} 


//===================================================================================
// �A�j���[�V�����擾( �t���[���w��� )
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief �w��t���[���ɂ����镽�s�ړ��f�[�^���擾����
 *
 * @param anime   �擾�ΏۃA�j���[�V����
 * @param vec_dst �擾�����f�[�^�̊i�[��
 * @param frame   �t���[���w��
 *
 * @return �f�[�^���擾�ł����ꍇ TRUE
 */
//-----------------------------------------------------------------------------------
BOOL ICA_ANIME_GetTranslateAt( const ICA_ANIME* anime, VecFx32* vec_dst, int frame )
{
  // �w��t���[�������[�v������
  frame = frame % anime->frameSize;

  // �S�A�j���[�V�����f�[�^��ێ����Ă���ꍇ�ɂ̂�, �����_���A�N�Z�X���\�Ƃ���
  if( anime->bufSize == anime->frameSize )
  {
    vec_dst->x = anime->transBuf[frame].x;
    vec_dst->y = anime->transBuf[frame].y;
    vec_dst->z = anime->transBuf[frame].z;
    return TRUE;
  }

  // �X�g���[�~���O�Đ����̓����_���A�N�Z�X�s��
  return FALSE;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �w��t���[���ɂ������]�f�[�^���擾����
 *
 * @param anime   �擾�ΏۃA�j���[�V����
 * @param vec_dst �擾�����f�[�^�̊i�[��
 * @param frame   �t���[���w��
 *
 * @return �f�[�^���擾�ł����ꍇ TRUE
 */
//-----------------------------------------------------------------------------------
BOOL ICA_ANIME_GetRotateAt( const ICA_ANIME* anime, VecFx32* vec_dst, int frame )
{
  // �w��t���[�������[�v������
  frame = frame % anime->frameSize;

  // �S�A�j���[�V�����f�[�^��ێ����Ă���ꍇ�ɂ̂�, �����_���A�N�Z�X���\�Ƃ���
  if( anime->bufSize == anime->frameSize )
  {
    vec_dst->x = anime->rotateBuf[frame].x;
    vec_dst->y = anime->rotateBuf[frame].y;
    vec_dst->z = anime->rotateBuf[frame].z;
    return TRUE;
  }

  // �X�g���[�~���O�Đ����̓����_���A�N�Z�X�s��
  return FALSE;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �w��t���[���ɂ�����g�k�f�[�^���擾����
 *
 * @param anime   �擾�ΏۃA�j���[�V����
 * @param vec_dst �擾�����f�[�^�̊i�[��
 * @param frame   �t���[���w��
 *
 * @return �f�[�^���擾�ł����ꍇ TRUE
 */
//-----------------------------------------------------------------------------------
BOOL ICA_ANIME_GetScaleAt( const ICA_ANIME* anime, VecFx32* vec_dst, int frame )
{
  // �w��t���[�������[�v������
  frame = frame % anime->frameSize;

  // �S�A�j���[�V�����f�[�^��ێ����Ă���ꍇ�ɂ̂�, �����_���A�N�Z�X���\�Ƃ���
  if( anime->bufSize == anime->frameSize )
  {
    vec_dst->x = anime->scaleBuf[frame].x;
    vec_dst->y = anime->scaleBuf[frame].y;
    vec_dst->z = anime->scaleBuf[frame].z;
    return TRUE;
  }

  // �X�g���[�~���O�Đ����̓����_���A�N�Z�X�s��
  return FALSE;
}


//===================================================================================
/**
 * @breif ����J�֐��̒�`
 */
//===================================================================================

//---------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�����Ǘ��o�b�t�@������������
 *
 * @param anime ����������A�j���[�V����
 */
//---------------------------------------------------------------------------
void InitAnime( ICA_ANIME* anime )
{
  anime->heapID        = 0;
  anime->arcHandle     = NULL;
  anime->datID         = 0;
  anime->frameSize     = 0;
  anime->nowFrame      = 0;
  anime->bufStartFrame = 0;
  anime->bufSize       = 0;
  anime->transBuf      = NULL;
  anime->rotateBuf     = NULL;
  anime->scaleBuf      = NULL;
  anime->haveTrans     = FALSE;
  anime->haveRotate    = FALSE;
  anime->haveScale     = FALSE;
  anime->allocMemory   = FALSE;
}

//---------------------------------------------------------------------------
/**
 * @brief �A�j���[�V��������ǂݍ���
 *
 * @param anime   �ǂݍ��񂾏��̊i�[��
 * @param arc_id  �A�j���[�V�����f�[�^�̃A�[�J�C�u�t�@�C���w��
 * @param dat_id  �A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 * @param heap_id �g�p����q�[�vID
 */
//---------------------------------------------------------------------------
void LoadAnimeInfo( ICA_ANIME* anime, ARCID arc_id, ARCDATID dat_id, HEAPID heap_id )
{
  u8 scale, rotate, trans;

  // �e��ID, �n���h�����L��
  anime->heapID    = heap_id;
  anime->arcHandle = GFL_ARC_OpenDataHandle( arc_id, heap_id );
  anime->datID     = dat_id;

  // �A�j���[�V�����t���[�������擾
  GFL_ARC_LoadDataOfsByHandle( anime->arcHandle, dat_id, 0, sizeof(u32), &anime->frameSize );

  // �e�A�j���[�V�����f�[�^�̗L�����擾
  GFL_ARC_LoadDataOfsByHandle( anime->arcHandle, anime->datID, 4, sizeof(u8), &scale );
  GFL_ARC_LoadDataOfsByHandle( anime->arcHandle, anime->datID, 5, sizeof(u8), &rotate );
  GFL_ARC_LoadDataOfsByHandle( anime->arcHandle, anime->datID, 6, sizeof(u8), &trans );
  anime->haveScale  = ( scale  != 0 );
  anime->haveRotate = ( rotate != 0 );
  anime->haveTrans  = ( trans  != 0 ); 

#ifdef ICA_ANIME_DEBUG
  OBATA_Printf( "haveScale  = %d\n", scale );
  OBATA_Printf( "haveRotate = %d\n", rotate );
  OBATA_Printf( "haveTrans  = %d\n", trans );
#endif
}

//---------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������ɉ����ăo�b�t�@���m�ۂ���
 *
 * @param anime    �o�b�t�@���m�ۂ���A�j���[�V����
 * @param buf_size �o�b�t�@�T�C�Y
 */
//---------------------------------------------------------------------------
void AllocBuffer( ICA_ANIME* anime, int buf_size )
{
  // �o�b�t�@�T�C�Y���L��
  anime->bufSize = buf_size;

  // �e��o�b�t�@���m��
  if( anime->haveTrans )
  {
    anime->transBuf = GFL_HEAP_AllocMemory( anime->heapID, sizeof(VecFx32) * buf_size );
  }
  if( anime->haveRotate )
  {
    anime->rotateBuf = GFL_HEAP_AllocMemory( anime->heapID, sizeof(VecFx32) * buf_size );
  }
  if( anime->haveScale )
  {
    anime->scaleBuf = GFL_HEAP_AllocMemory( anime->heapID, sizeof(VecFx32) * buf_size );
  }

  // �o�b�t�@���m�ۂ������Ƃ��L������
  anime->allocMemory = TRUE;
}

//---------------------------------------------------------------------------
/**
 * @brief �m�ۍς݂̃�����������o�b�t�@�Ɋ��蓖�Ă�
 *
 * @param anime    ���蓖�đΏۃA�j���[�V���� 
 * @param bufsize  �����o�b�t�@�̑傫��
 * @param work     ���蓖�Ă郏�[�N
 * @param worksize ���[�N�T�C�Y
 *
 * @return ���������蓖�Ă�ꂽ��, TRUE
 */
//---------------------------------------------------------------------------
BOOL AssignBuffer( ICA_ANIME* anime, int bufsize, void* work, int worksize )
{
  int minsize = 0;
  int    size = sizeof(VecFx32) * bufsize; // �o�b�t�@1�{������̃T�C�Y
  int   index = 0;

  // �Œ���K�v�ȃ������ʂ��v�Z
  if( anime->haveTrans )  minsize += size;
  if( anime->haveRotate ) minsize += size;
  if( anime->haveScale )  minsize += size;

  // �w�肳�ꂽ���[�N������Ȃ��ꍇ
  if( worksize < minsize )
  {
    OBATA_Printf( "------------------------------------------\n" );
    OBATA_Printf( "ICA_ANIME �ɓn���ꂽ���[�N�����������܂��B\n" );
    OBATA_Printf( "------------------------------------------\n" );
    return FALSE;
  }

  // ���[�N�����蓖�Ă�
  if( anime->haveTrans )
  {
    anime->transBuf = (VecFx32*)( (int)work + size * index );
    index++;
  }
  if( anime->haveRotate )
  {
    anime->rotateBuf = (VecFx32*)( (int)work + size * index );
    index++;
  }
  if( anime->haveScale )
  {
    anime->scaleBuf  = (VecFx32*)( (int)work + size * index );
    index++;
  }

  // �o�b�t�@�T�C�Y���L��
  anime->bufSize = bufsize;

  return TRUE;
}

//---------------------------------------------------------------------------
/**
 * @brief �o�b�t�@���X�V����
 * 
 * @param anime       �X�V�Ώۂ̃A�j���[�V�����Ǘ��I�u�W�F�N�g
 * @param start_frame �V�����f�[�^�̊J�n�t���[��
 */
//---------------------------------------------------------------------------
void UpdateBuf( ICA_ANIME* anime, fx32 start_frame )
{
  int   i, j;
  int   frame     = (start_frame >> FX32_SHIFT) % anime->frameSize;
  int   ofs       = 8;  // �w�b�_�T�C�Y
  int   data_size = 0;
  void* data      = NULL;

  // �擪�f�[�^�ւ̃I�t�Z�b�g���v�Z
  if( anime->haveScale  ) ofs += sizeof(float)*3*frame;
  if( anime->haveRotate ) ofs += sizeof(float)*3*frame;
  if( anime->haveTrans  ) ofs += sizeof(float)*3*frame;

  // �f�[�^�T�C�Y���v�Z
  if( anime->haveScale  ) data_size += sizeof(float)*3*anime->bufSize;
  if( anime->haveRotate ) data_size += sizeof(float)*3*anime->bufSize;
  if( anime->haveTrans  ) data_size += sizeof(float)*3*anime->bufSize;

  // �w��t���[���ʒu���� ���t���[�������̃f�[�^���擾��, �o�b�t�@�X�V
  data = GFL_HEAP_AllocMemoryLo( anime->heapID, data_size );
  GFL_ARC_LoadDataOfsByHandle( anime->arcHandle, anime->datID, ofs, data_size, data );
  for( i=0, j=0; i<anime->bufSize; i++ )
  {
    if( anime->haveScale )
    {
      anime->scaleBuf[i].x = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
      anime->scaleBuf[i].y = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
      anime->scaleBuf[i].z = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    }
    if( anime->haveRotate )
    {
      anime->rotateBuf[i].x = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
      anime->rotateBuf[i].y = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
      anime->rotateBuf[i].z = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    }
    if( anime->haveTrans )
    {
      anime->transBuf[i].x = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
      anime->transBuf[i].y = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
      anime->transBuf[i].z = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    }
  }
  GFL_HEAP_FreeMemory( data );

  // �o�b�t�@�J�n�t���[���ʒu���L��
  anime->bufStartFrame = start_frame >> FX32_SHIFT;

#ifdef ICA_ANIME_DEBUG
  OBATA_Printf( "update ica anime buffer\n" );
  OBATA_Printf( "frame     = %d\n", frame );
  OBATA_Printf( "ofs       = %d\n", ofs );
  OBATA_Printf( "data_size = %d\n", data_size );
  OBATA_Printf( "bufSize   = %d\n", anime->bufSize );
#endif
}
