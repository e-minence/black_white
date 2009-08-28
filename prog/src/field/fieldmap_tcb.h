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
void FIELDMAP_TCB_ROT_PLAYER_AddTask(                // ����
    FIELDMAP_WORK* fieldmap, int frame, int rot_num );    
void FIELDMAP_TCB_ROT_PLAYER_AddTask_SpeedUp(        // ����
    FIELDMAP_WORK* fieldmap, int frame, int rot_num );    
void FIELDMAP_TCB_ROT_PLAYER_AddTask_SlowDown(       // ����
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

//=======================
// fieldmap_tcb_camera.c
//=======================
// �J�����̃Y�[��
void FIELDMAP_TCB_CAMERA_AddTask_Zoom(
    FIELDMAP_WORK* fieldmap, int frame, fx32 dist );  

// �J�����̉�]
void FIELDMAP_TCB_CAMERA_AddTask_Yaw(                   // ���[��]
    FIELDMAP_WORK* fieldmap, int frame, u16 angle );   
void FIELDMAP_TCB_CAMERA_AddTask_Pitch(                 // �s�b�`��]
    FIELDMAP_WORK* fieldmap, int frame, u16 angle );   
