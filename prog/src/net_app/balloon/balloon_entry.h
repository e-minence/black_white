//==============================================================================
/**
 * @file	balloon_entry.h
 * @brief	���D����F�G���g���[��ʂ̃w�b�_
 * @author	matsuda
 * @date	2007.11.06(��)
 */
//==============================================================================
#ifndef __BALLOON_ENTRY_H__
#define __BALLOON_ENTRY_H__


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern GFL_PROC_RESULT BalloonEntryProc_Init( GFL_PROC * proc, int * seq );
extern GFL_PROC_RESULT BalloonEntryProc_Main( GFL_PROC * proc, int * seq );
extern GFL_PROC_RESULT BalloonEntryProc_End(GFL_PROC *proc, int *seq);


#endif	//__BALLOON_ENTRY_H__

