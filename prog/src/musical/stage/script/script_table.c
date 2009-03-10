//======================================================================
/**
 * @file	script_table.h
 * @brief	�X�e�[�W �X�N���v�g���� �X�N���v�g�e�[�u��
 * @author	ariizumi
 * @data	09/03/09
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "sta_act_script.h"
#include "sta_act_script_def.h"

//# �͈����𕶎���� ## �͕�����̌��� \ �Ń}�N���̉��s

//extern�Ŋ֐����`���A�ʃ\�[�X�Ŋ֐����錾����Ă��Ă�OK�Ȃ悤��
//extern SCRIPT_FINISH_TYPE STA_SCRIPT_FuncName_Func(STA_SCRIPT_SYS *work,STA_SCRIPT_WORK *scriptWork);
#define SCRIPT_FUNC_DEF(str) extern SCRIPT_FINISH_TYPE STA_SCRIPT_ ## str ## _Func(STA_SCRIPT_SYS *work,STA_SCRIPT_WORK *scriptWork);
#include "script_table.dat"
#undef SCRIPT_FUNC_DEF

//��`���ꂽ���Ɋ֐���z��Ɋi�[
//STA_SCRIPT_FuncName_Func,
#define SCRIPT_FUNC_DEF(str) STA_SCRIPT_ ## str ## _Func,
STA_SCRIPT_FUNC STA_ScriptFuncArr[] =
{
#include "script_table.dat"
};
#undef SCRIPT_FUNC_DEF
