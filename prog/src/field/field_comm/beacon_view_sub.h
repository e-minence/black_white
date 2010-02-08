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
 *  @brief  �X�^�b�N���烍�O���ꌏ���o��
 */
extern BOOL BeaconView_CheckStack( BEACON_VIEW_PTR wk );

/*
 *  @brief  ���j���[�o�[�A�j���Z�b�g
 */
extern void BeaconView_MenuBarViewSet( BEACON_VIEW_PTR wk, MENU_ID id, MENU_STATE state );

/*
 *  @brief  ���j���[�o�[�A�j���E�F�C�g
 */
extern BOOL BeaconView_MenuBarCheckAnm( BEACON_VIEW_PTR wk, MENU_ID id );

