//==============================================================================================
/**
 * @file	debug_print.c
 * @date	2007.02.13
 * @author	taya GAME FREAK inc.
 */
//==============================================================================================

#include "system/debug_print.h"


// �ꎞ�I�ɕʊ��ł��o�͂�������񂪂���ꍇ�A����Printf�L�q�҂�ID���`����
#define REGULAR_AUTHOR_ID	(AUTHOR_FREE)


#ifdef DEBUG_PRINT_ENABLE

//==============================================================================================
/**
 * ���胆�[�U�̊��ŃR���p�C�������ꍇ�̂ݓ��삷��Printf�֐�
 *
 * @param   author_id		���[�UID�idebug_print.h�ɋL�q�j
 * @param   fmt				OS_TPrintf�ɓn�������t��������
 *
 */
//==============================================================================================
void DEBUG_Printf( AUTHOR_ID author_id, const char * fmt, ... )
{
	if(	(author_id==AUTHOR_FREE)
	||	(author_id==DEFINED_AUTHOR_ID)
	||	(author_id==REGULAR_AUTHOR_ID)
	){
		va_list vlist;

		va_start(vlist, fmt);
		OS_TVPrintf(fmt, vlist);
		va_end(vlist);
	}
}

#endif
