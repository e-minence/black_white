//============================================================================================
/**
 * @file  msc_readfile.c
 * @brief
 * @date  2010.01.18
 */
//============================================================================================

#include <gflib.h>
#include <nnsys/mcs.h>

#include "debug/mcs_readfile.h"

//============================================================================================
//============================================================================================
//----------------------------------------------------------------
//----------------------------------------------------------------
static u32 sMcsFileIOWork[ (NNS_MCS_FILEIO_WORKMEM_SIZE + sizeof(u32) - 1 ) / sizeof(u32) ];

//============================================================================================
//============================================================================================


//----------------------------------------------------------------
//----------------------------------------------------------------
void GF_MCS_FILE_Init(void)
{
  NNS_McsInitFileIO( sMcsFileIOWork );
}

//----------------------------------------------------------------
//----------------------------------------------------------------
BOOL GF_MCS_FILE_Read( const char * path, void * buf, u32 buf_size )
{
  NNSMcsFile infoRead;
  u32 errCode;
  u32 fileSize;
  u32 readSize;

  if ( NNS_McsIsServerConnect() == FALSE) return FALSE;

  // �ǂݍ��ݗp�I�[�v��
  errCode = NNS_McsOpenFile(
    &infoRead,
    path, // �t�@�C����
    NNS_MCS_FILEIO_FLAG_READ | NNS_MCS_FILEIO_FLAG_INCENVVAR ); // �ǂݍ��݃��[�h�A���ϐ��W�J
  if (errCode != 0)
  {
    // �t�@�C���I�[�v�����s
    OS_Printf( "�t�@�C��(%s)�I�[�v�����s:%d\n", path, errCode );
    return FALSE;
  }

  fileSize = NNS_McsGetFileSize( &infoRead );
  if ( fileSize >= buf_size )
  {
    //�t�@�C���T�C�Y���傫������
    NNS_McsCloseFile( &infoRead );
    OS_Printf("Too Large FileSize(%d) > bufsize(%d)\n", fileSize, buf_size );
    return FALSE;
  }
  errCode = NNS_McsReadFile(
      &infoRead,
      buf,
      fileSize,
      &readSize);
  if ( errCode != 0 )
  {
    //�t�@�C���ǂݍ��ݎ��s
    NNS_McsCloseFile( &infoRead );
    OS_Printf( "�t�@�C��(%s)�ǂݍ��ݎ��s:%d\n", path, errCode );
    return FALSE;
  }
  NNS_McsCloseFile( &infoRead );
  return TRUE;
}

