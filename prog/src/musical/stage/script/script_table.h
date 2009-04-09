//======================================================================
/**
 * @file	script_table.h
 * @brief	�X�e�[�W �X�N���v�g���� �X�N���v�g�e�[�u��
 * @author	ariizumi
 * @data	09/03/09
 */
//======================================================================
#ifndef SCRIPT_TABLE_H__
#define SCRIPT_TABLE_H__

#include "system/vm_cmd.h"

#include "sta_act_script.h"
#include "sta_act_script_def.h"

//��`���ꂽ����enum��`
//SCRIPT_ENUM_FuncName,
#define SCRIPT_FUNC_DEF(str) SCRIPT_ENUM_ ## str ,
enum
{
#include "script_table.dat"
	SCRIPT_ENUM_MAX,
};
#undef SCRIPT_FUNC_DEF


//�X�N���v�g�e�[�u��
extern VMCMD_FUNC STA_ScriptFuncArr[];

#endif SCRIPT_TABLE_H__

