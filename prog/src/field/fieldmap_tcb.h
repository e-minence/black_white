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
// fieldmap_tcb_rot_player.c
//============================ 
// �v���C���[�̉�]
void FIELDMAP_TCB_AddTask_RotatePlayer(                // ����
    FIELDMAP_WORK* fieldmap, int frame, int rot_num );    
void FIELDMAP_TCB_AddTask_RotatePlayer_SpeedUp(        // ����
    FIELDMAP_WORK* fieldmap, int frame, int rot_num );    
void FIELDMAP_TCB_AddTask_RotatePlayer_SpeedDown(      // ����
    FIELDMAP_WORK* fieldmap, int frame, int rot_num );   

//============================
// fieldmap_tcb_move_player.c
//============================
// �v���C���[�̈ړ�
void FIELDMAP_TCB_AddTask_MovePlayer(
    FIELDMAP_WORK* fieldmap, int frame, VecFx32* pos );

//============================
// fieldmap_tcb_camera_zoom.c
//============================
// �J�����̃Y�[��
void FIELDMAP_TCB_AddTask_CameraZoom(                   // �����Y�[��
    FIELDMAP_WORK* fieldmap, int frame, fx32 dist );  
void FIELDMAP_TCB_AddTask_CameraZoom_Sharp(             // �}���i�Y�[��
    FIELDMAP_WORK* fieldmap, int frame, fx32 dist );  
