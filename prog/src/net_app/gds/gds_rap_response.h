//==============================================================================
/**
 * @file	gds_rap_response.h
 * @brief	gds_rap.c�����M�f�[�^�̔����擾������Ɨ�����������
 * @author	matsuda
 * @date	2008.01.13(��)
 */
//==============================================================================
#ifndef __GDS_RAP_RESPONSE_H__
#define __GDS_RAP_RESPONSE_H__


//==============================================================================
//	�O���֐��錾
//==============================================================================
//--------------------------------------------------------------
//  �V�X�e�����g�p
//--------------------------------------------------------------
extern int GDS_RAP_RESPONSE_MusicalShot_Upload(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res);
extern int GDS_RAP_RESPONSE_MusicalShot_Download(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res);
extern int GDS_RAP_RESPONSE_BattleVideo_Upload(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res);
extern int GDS_RAP_RESPONSE_BattleVideo_Search_Download(GDS_RAP_WORK *gdsrap, 
	POKE_NET_RESPONSE *res);
extern int GDS_RAP_RESPONSE_BattleVideo_Data_Download(GDS_RAP_WORK *gdsrap, 
	POKE_NET_RESPONSE *res);
extern int GDS_RAP_RESPONSE_BattleVideo_Favorite_Upload(GDS_RAP_WORK *gdsrap, 
	POKE_NET_RESPONSE *res);

//--------------------------------------------------------------
//	��M�o�b�t�@����f�[�^�擾�F���[�U�[�p
//--------------------------------------------------------------
extern int GDS_RAP_RESPONSE_MusicalShot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, 
	MUSICAL_SHOT_RECV **dress_array, int array_max);
extern u64 GDS_RAP_RESPONSE_BattleVideo_Upload_DataGet(GDS_RAP_WORK *gdsrap);
extern int GDS_RAP_RESPONSE_BattleVideoSearch_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, 
	BATTLE_REC_OUTLINE_RECV **outline_array, int array_max);
extern int GDS_RAP_RESPONSE_BattleVideoSearch_Download_DataCopy(GDS_RAP_WORK *gdsrap, 
	BATTLE_REC_OUTLINE_RECV *outline_array, int array_max);
extern int GDS_RAP_RESPONSE_BattleVideoData_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, 
	BATTLE_REC_RECV **rec);
extern void GDS_RAP_RESPONSE_BattleVideoFavorite_Upload_RecvPtr_Set(GDS_RAP_WORK *gdsrap);


#endif	//__GDS_RAP_RESPONSE_H__
