//======================================================================
/**
 * @file	select_mode.c
 * @brief	�Q�[���J�n���̊����E�Ђ炪�Ȃ̑I��
 * @author	ariizumi
 * @data	09/05/11
 */
//======================================================================

#ifndef SELECT_MODE_H__
#define SELECT_MODE_H__

#include "savedata/config.h"
#include "savedata/mystatus.h"
#include "savedata/misc.h"

extern const GFL_PROC_DATA SelectModeProcData;

typedef enum
{
  SMT_START_GAME,     //�ŏ�����(�������ʐM
  SMT_CONTINUE_GAME,  //��������(�ʐM�̂�
}SELECT_MODE_TYPE;

typedef struct
{
  SELECT_MODE_TYPE type;
  CONFIG *configSave;
  MYSTATUS *mystatus;
	MISC * miscSave;
}SELECT_MODE_INIT_WORK;

#endif //SELECT_MOJI_MODE_H__
