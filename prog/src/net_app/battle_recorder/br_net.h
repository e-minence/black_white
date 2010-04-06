//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_net.h
 *	@brief  �o�g�����R�[�_�[�l�b�g����
 *	@author	Toru=Nagihashi
 *	@date		2010.03.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "gamesystem/game_data.h"
#include "../gds/gds_rap.h"
#include "../gds/gds_rap_response.h"

#include "../lib/poke_net/poke_net/gds_header/gds_battle_rec_sub.h" //���̂�����include����ĂȂ��̂ŕʌŌĂ�
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�l�b�g�C���^�C�v
//=====================================
typedef enum
{
  BR_NET_ERR_REPAIR_TYPE_NONE,            //�G���[���N���Ă��Ȃ�
  BR_NET_ERR_REPAIR_TYPE_RETURN_TOPMENU,  //�g�b�v���j���[�֖߂�
  BR_NET_ERR_REPAIR_TYPE_TETURN_ONCE,     //�P�O�֖߂�
} BR_NET_ERR_REPAIR_TYPE;


//-------------------------------------
///	���N�G�X�g
//=====================================
typedef enum
{
  BR_NET_REQUEST_MUSICAL_SHOT_UPLOAD,     //�~���[�W�J���A�b�v���[�h
  BR_NET_REQUEST_MUSICAL_SHOT_DOWNLOAD,   //�~���[�W�J���_�E�����[�h
  BR_NET_REQUEST_BATTLE_VIDEO_UPLOAD,     //�r�f�I�A�b�v���[�h
  BR_NET_REQUEST_BATTLE_VIDEO_DOWNLOAD,   //�r�f�I�_�E�����[�h
  BR_NET_REQUEST_VIDEO_SEARCH_DOWNLOAD,   //�r�f�I�����_�E�����[�h
  BR_NET_REQUEST_NEW_RANKING_DOWNLOAD,    //�ŐV�����L���O�_�E�����[�h
  BR_NET_REQUEST_FAVORITE_RANKING_DOWLOAD,//�l�C�����L���O�_�E�����[�h
  BR_NET_REQUEST_SUBWAY_RANKING_DOWNLOAD, //�T�u�E�F�C�����L���O�_�E�����[�h
  BR_NET_REQUEST_FAVORITE_VIDEO_UPLOAD,   //���C�ɓ���A�b�v���[�h

  BR_NET_REQUEST_MAX
} BR_NET_REQUEST;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�o�g�����R�[�_�[�l�b�g����
//=====================================
typedef struct _BR_NET_WORK BR_NET_WORK;

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//--- system ---
extern BR_NET_WORK *BR_NET_Init( GAMEDATA *p_gamedata, DWCSvlResult *p_svl, HEAPID heapID );
extern void BR_NET_Exit( BR_NET_WORK *p_wk );
extern void BR_NET_Main( BR_NET_WORK *p_wk );


//--- down/up load ---
typedef struct 
{
  u16 monsno;
  u8 country_code;
  u8 local_code;
  BATTLEMODE_SEARCH_NO battle_mode_no;
} BR_NET_VIDEO_SEARCH_DATA;

typedef union
{ 
  //BR_NET_REQUEST_MUSICAL_SHOT_UPLOAD,     //�~���[�W�J���A�b�v���[�h
  const MUSICAL_SHOT_DATA   *cp_upload_musical_shot_data;
  //BR_NET_REQUEST_MUSICAL_SHOT_DOWNLOAD,   //�~���[�W�J���_�E�����[�h
  u16                       download_musical_shot_search_monsno;
  //BR_NET_REQUEST_BATTLE_VIDEO_UPLOAD,     //�r�f�I�A�b�v���[�h
  /* none */
  //BR_NET_REQUEST_BATTLE_VIDEO_DOWNLOAD,   //�r�f�I�_�E�����[�h
  u64                       download_video_number; 
  //BR_NET_REQUEST_VIDEO_SEARCH_DOWNLOAD,   //�r�f�I�����_�E�����[�h
  BR_NET_VIDEO_SEARCH_DATA  download_video_search_data;
  //BR_NET_REQUEST_NEW_RANKING_DOWNLOAD,    //�ŐV�����L���O�_�E�����[�h
  /* none */
  //BR_NET_REQUEST_FAVORITE_RANKING_DOWLOAD,//�l�C�����L���O�_�E�����[�h
  /* none */
  //BR_NET_REQUEST_SUBWAY_RANKING_DOWNLOAD, //�T�u�E�F�C�����L���O�_�E�����[�h
  /* none */
  //BR_NET_REQUEST_FAVORITE_VIDEO_UPLOAD,   //���C�ɓ���A�b�v���[�h
  u64                       upload_favorite_video_number;
}BR_NET_REQUEST_PARAM;

extern void BR_NET_StartRequest( BR_NET_WORK *p_wk, BR_NET_REQUEST type, const BR_NET_REQUEST_PARAM *cp_param );
extern BOOL BR_NET_WaitRequest( BR_NET_WORK *p_wk );

//--- get download data ---
extern BOOL BR_NET_GetDownloadMusicalShot( BR_NET_WORK *p_wk, MUSICAL_SHOT_RECV **pp_data_tbl, int tbl_max, int *p_recv_num );
extern BOOL BR_NET_GetUploadBattleVideoNumber( BR_NET_WORK *p_wk, u64 *p_video_number );
extern BOOL BR_NET_GetDownloadBattleVideo( BR_NET_WORK *p_wk, BATTLE_REC_RECV **pp_data, int *p_video_number );
extern BOOL BR_NET_GetDownloadBattleSearch( BR_NET_WORK *p_wk, BATTLE_REC_OUTLINE_RECV *p_data_tbl, int tbl_max, int *p_recv_num );
extern BOOL BR_NET_GetDownloadNewRanking( BR_NET_WORK *p_wk, BATTLE_REC_OUTLINE_RECV *p_data_tbl, int tbl_max, int *p_recv_num );
extern BOOL BR_NET_GetDownloadFavoriteRanking( BR_NET_WORK *p_wk, BATTLE_REC_OUTLINE_RECV *p_data_tbl, int tbl_max, int *p_recv_num );
extern BOOL BR_NET_GetDownloadSubwayRanking( BR_NET_WORK *p_wk, BATTLE_REC_OUTLINE_RECV *p_data_tbl, int tbl_max, int *p_recv_num );

//----�G���[
typedef enum
{ 
  BR_NET_ERR_RETURN_NONE,     //�G���[���N���Ă��Ȃ�
  BR_NET_ERR_RETURN_ONCE,     //�P�O�ɖ߂�
  BR_NET_ERR_RETURN_TOPMENU,  //�g�b�v���j���[�֖߂�
}BR_NET_ERR_RETURN;
extern BR_NET_ERR_RETURN BR_NET_GetError( BR_NET_WORK *p_wk, int *p_msg_no );
