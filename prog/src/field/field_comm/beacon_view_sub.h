/**
 *  @file   beacon_view_sub.h
 *  @brief  ����Ⴂ�ʐM��ʃT�u�֐��Q�w�b�_
 *  @author Miyuki Iwasawa
 *  @date   10.01.19
 */

#pragma once

/*
 *  @brief  �����`��
 */
extern void BeaconView_InitialDraw( BEACON_VIEW_PTR wk );

/*
 *  @brief  ���C�����[�v���ł̓��̓`�F�b�N
 */
extern int BeaconView_CheckInput( BEACON_VIEW_PTR wk );

/*
 *  @brief  ���X�g�㉺�{�^���@�^�b�`����
 */
extern int BeaconView_CheckInoutTouchUpDown( BEACON_VIEW_PTR wk );

/*
 *  @brief  ���ΏۑI�����[�v���ł̓��̓`�F�b�N
 */
extern int BeaconView_CheckInputThanks( BEACON_VIEW_PTR wk );

/*
 *  @brief  ����|�b�v�A�b�v�N���`�F�b�N 
 */
extern BOOL BeaconView_CheckSpecialPopup( BEACON_VIEW_PTR wk );

/*
 *  @brief  ����G�p���[�����|�b�v�A�b�v�N���`�F�b�N 
 */
extern BOOL BeaconView_CheckSpecialGPower( BEACON_VIEW_PTR wk );

/*
 *  @brief  �X�^�b�N���烍�O���ꌏ���o��
 */
extern BOOL BeaconView_CheckStack( BEACON_VIEW_PTR wk );

/*
 *  @brief  �A�N�e�B�u�E�p�b�V�u�ؑ�
 */
extern void BeaconView_SetViewPassive( BEACON_VIEW_PTR wk, BOOL passive_f );

//---------------------------------------------------
/*
 *  @brief  ���X�g�X�N���[�����N�G�X�g
 */
//---------------------------------------------------
extern void BeaconView_ListScrollRepeatReq( BEACON_VIEW_PTR wk );

//---------------------------------------------------
/*
 *  @brief  �v�����g�L���[�@�����X�s�[�h�A�b�v���N�G�X�g
 *
 *  ���������������炵�A�������ׂ��グ�����ɁA
 *  �v�����g�����I���܂ł̃X�s�[�h���A�b�v����
 */
//---------------------------------------------------
extern void BeaconView_PrintQueLimmitUpSet( BEACON_VIEW_PTR wk, BOOL flag );

/*
 *  @brief  �T�u�V�[�P���X�@New���O�G���g���[���C��
 */
extern BOOL BeaconView_SubSeqLogEntry( BEACON_VIEW_PTR wk );

/*
 *  @brief  �T�u�V�[�P���X�@GPower�g�p���C��
 */
extern BOOL BeaconView_SubSeqGPower( BEACON_VIEW_PTR wk );

/*
 *  @brief  �T�u�V�[�P���X�@��烁�C��
 */
extern int BeaconView_SubSeqThanks( BEACON_VIEW_PTR wk );

/*
 *  @brief  ���j���[�o�[�A�j���Z�b�g
 */
extern void BeaconView_MenuBarViewSet( BEACON_VIEW_PTR wk, MENU_ID id, MENU_STATE state );

/*
 *  @brief  ���j���[�o�[�A�j���E�F�C�g
 */
extern BOOL BeaconView_MenuBarCheckAnm( BEACON_VIEW_PTR wk, MENU_ID id );

/*
 *  @brief  �X�N���[���{�^���A�j���Z�b�g
 */
extern void BeaconView_UpDownAnmSet( BEACON_VIEW_PTR wk, SCROLL_DIR dir );

/*
 *  @brief  �X�N���[���{�^����ԃZ�b�g
 */
extern void BeaconView_UpDownViewSet( BEACON_VIEW_PTR wk );

