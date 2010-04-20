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

/*
 *  @brief  �T�u�V�[�P���X�@GPower�g�p���C��
 */
extern BOOL BeaconView_SubSeqGPower( BEACON_VIEW_PTR wk );

/*
 *  @brief  �T�u�V�[�P���X�@��烁�C��
 */
extern BOOL BeaconView_SubSeqThanks( BEACON_VIEW_PTR wk );

/*
 *  @brief  ���j���[�o�[�A�j���Z�b�g
 */
extern void BeaconView_MenuBarViewSet( BEACON_VIEW_PTR wk, MENU_ID id, MENU_STATE state );

/*
 *  @brief  ���j���[�o�[�A�j���E�F�C�g
 */
extern BOOL BeaconView_MenuBarCheckAnm( BEACON_VIEW_PTR wk, MENU_ID id );

