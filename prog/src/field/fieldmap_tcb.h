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
// fieldmap_tcb_warp_player.c
//============================
// �v���C���[�̃��[�v�ړ�
void FIELDMAP_TCB_WARP_PLAYER_AddTask_DisappearUp(      // �㏸�ޏ�
    FIELDMAP_WORK* fieldmap, int frame, int dist );
void FIELDMAP_TCB_WARP_PLAYER_AddTask_AppearDown(       // �~���o��
    FIELDMAP_WORK* fieldmap, int frame, int dist );
void FIELDMAP_TCB_WARP_PLAYER_AddTask_FallIn(           // �����o��
    FIELDMAP_WORK* fieldmap, int frame, int dist );

//============================
// fieldmap_tcb_camera_zoom.c
//============================
// �J�����̃Y�[��
void FIELDMAP_TCB_AddTask_CameraZoom(
    FIELDMAP_WORK* fieldmap, int frame, fx32 dist );  

//============================
// fieldmap_tcb_camera_rot.c
//============================
// �J�����̉�]
void FIELDMAP_TCB_AddTask_CameraRotate_Yaw(             // ���[��]
    FIELDMAP_WORK* fieldmap, int frame, u16 angle );   
void FIELDMAP_TCB_AddTask_CameraRotate_Pitch(           // �s�b�`��]
    FIELDMAP_WORK* fieldmap, int frame, u16 angle );   
