//===================================================================
/**
 * @file	filesystem.c
 * @brief	ROM�t�@�C���V�X�e��
 * @author	GAME FREAK Inc.
 * @date	06.11.09
 *
 * $LastChangedDate$
 * $Id$
 */
//===================================================================
#include <nitro.h>
#include "gflib.h"
#include "filesystem_inter.h"


//------------------------------------------------------------------
/**
 * @brief	�t�@�C���V�X�e��������
 * @param	NONE
 * @return	NONE
*/
//------------------------------------------------------------------
void InitFileSystem(void)
{
  u32 file_table_size;
  void* p_table;

  // �t�@�C���V�X�e��������
  FS_Init(FS_DMA_NUMBER);
  // �t�@�C���Ǘ��e�[�u����RAM�֍ڂ��遨�t�@�C���ւ̍����A�N�Z�X���\
  file_table_size = FS_GetTableSize();
  p_table = OS_AllocFromMainArenaLo(file_table_size, 4);
  SDK_ASSERT(p_table != NULL);
  (void)FS_LoadTable(p_table, file_table_size);
  OS_TPrintf("FileTable Size     = 0x%08x bytes.\n", file_table_size);
  OS_TPrintf("remains of MainRAM = 0x%08x bytes.\n", (u32)(OS_GetMainArenaHi())-(u32)(OS_GetMainArenaLo()));
}

/*  */
