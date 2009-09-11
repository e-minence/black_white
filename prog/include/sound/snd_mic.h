//============================================================================
/**
 *
 *	@file		snd_mic.h
 *	@brief  �}�C�N�֘A���W���[��
 *	@author	hosaka genya
 *	@data		2009.09.10
 *
 */
//============================================================================
#pragma once

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//=============================================================================
/**
 *								�v���g�^�C�v�錾
 */
//=============================================================================
extern void SND_MIC_Init( HEAPID heap_id );
extern void SND_MIC_Exit( void );
extern void SND_MIC_Main( void );
extern void SND_MIC_ClearAmpOnWait( void );
extern BOOL SND_MIC_IsAmpOnWaitFlag( void );
extern MICResult SND_MIC_StartAutoSampling( MICAutoParam* p );
extern MICResult SND_MIC_StopAutoSampling(void);
extern void SND_MIC_StopSleep(void);
extern void SND_MIC_ReStartSleep(void);
extern MICResult SND_MIC_ManualSampling(MICSamplingType type ,void* heap,MICCallback callback,void* arg);

