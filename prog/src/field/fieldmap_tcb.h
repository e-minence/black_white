////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �t�B�[���h�}�b�vTCB �^�X�N�����֐��Q
 * @file   fieldmap_tcb.h
 * @author obata
 * @date   2009.08.25
 *
 */
////////////////////////////////////////////////////////////////////////
#pragma once


//============================
// fieldmap_tcb_camera_zoom.c
//============================
// �J�����̃Y�[��
extern void FIELDMAP_TCB_AddTask_CameraZoom(                   // �����Y�[��
    FIELDMAP_WORK* fieldmap, int frame, fx32 dist );  
extern void FIELDMAP_TCB_AddTask_CameraZoom_Sharp(             // �}���i�Y�[��
    FIELDMAP_WORK* fieldmap, int frame, fx32 dist );  
