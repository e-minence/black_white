//==============================================================================
/**
 * @file	save_control.h
 * @brief	�Z�[�u�f�[�^�Ǘ��w�b�_
 * @author	matsuda
 * @date	2008.08.28(��)
 */
//==============================================================================
#ifndef __SAVE_CONTROL_H__
#define __SAVE_CONTROL_H__

#include <backup_system.h>


//==============================================================================
//	�萔��`
//==============================================================================
//--------------------------------------------------------------
//	first_status�̌���BIT
//--------------------------------------------------------------
#define NORMAL_NG_BIT			(1<<0)		//�ʏ�Z�[�u�F�~���[�����ONG
#define NORMAL_BREAK_BIT		(1<<1)		//�ʏ�Z�[�u�F�j��
#define FRONTIER_NG_BIT			(1<<2)		//�O���t�����e�B�A�Z�[�u�F�~���[�����ONG
#define FRONTIER_BREAK_BIT		(1<<3)		//�O���t�����e�B�A�Z�[�u�F�j��
#define VIDEO_NG_BIT			(1<<4)		//�O���r�f�I�Z�[�u�F�~���[�����ONG
#define VIDEO_BREAK_BIT			(1<<5)		//�O���r�f�I�Z�[�u�F�j��

//==============================================================================
//	�^��`
//==============================================================================
///�Z�[�u�f�[�^�Ǘ����[�N�^�̕s��`�^
typedef struct _SAVE_CONTROL_WORK SAVE_CONTROL_WORK;


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern SAVE_CONTROL_WORK * SaveControl_SystemInit(int heap_id);
extern SAVE_CONTROL_WORK * SaveControl_GetPointer(void);
extern LOAD_RESULT SaveControl_Load(SAVE_CONTROL_WORK *ctrl);
extern SAVE_RESULT SaveControl_Save(SAVE_CONTROL_WORK *ctrl);
extern void * SaveControl_DataPtrGet(SAVE_CONTROL_WORK *ctrl, GFL_SVDT_ID gmdata_id);



#endif	//__SAVE_CONTROL_H__

