//=============================================================================
/**
 *
 *	@file		beacon_detail_sub.h
 *	@brief  ����Ⴂ�ڍ׉�� �T�u
 *	@author	Miyuki Iwasawa
 *	@data		2010.02.01
 *
 */
//=============================================================================

#pragma once

/*
 *  @brief  �r�[�R���ڍ׉�ʏ����`��
 */
extern void BeaconDetail_InitialDraw( BEACON_DETAIL_WORK* wk );

/*
 *  @brief  �r�[�R���ڍ׉�ʁ@���͎擾
 */
extern int BeaconDetail_InputCheck( BEACON_DETAIL_WORK* wk );

/*
 *  @brief  �r�[�R���ڍ׉�ʁ@���j���[�o�[��ԍX�V
 */
extern void BeaconDetail_ButtonActiveControl( BEACON_DETAIL_WORK* wk );

