//==============================================================================
/**
 * @file	gra_tool.h
 * @brief	�O���t�B�b�N�c�[��
 * @author	goto
 * @date	2005.11.16
 *
 * �����ɐF�X�ȉ�����������Ă��悢
 *	GS���ڐA�B����system/gra_tool�����g���[�i�[�̕ϊ��ɕK�v�Ȃ��̂����ڐA	Ari090331
 */
//==============================================================================

#ifndef	_UNION_GRA_TOOL_H_
#define _UNION_GRA_TOOL_H_

// -----------------------------------------
//
//	���萔��`
//
// -----------------------------------------
// 2D��1D�ϊ��Ɏg�p
#define CHAR_DOT_X		(8/2)								///< 1�L�����̃o�C�g�T�C�Y
#define CHAR_DOT_Y		(8)									///< 1�L�����̃o�C�g�T�C�Y
#define CHAR_BYTE		(CHAR_DOT_X * CHAR_DOT_Y)			///< 1�L�����o�C�g��

void ChangesInto_1D_from_2D(int arc_id, int index_no, int heap_id, int x, int y, int w, int h, void* buff);

#endif	//_UNION_GRA_TOOL_H_

