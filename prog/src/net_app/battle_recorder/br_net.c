//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_net.c
 *	@brief  �o�g�����R�[�_�[�l�b�g����
 *	@author	Toru=Nagihashi
 *	@data		2010.03.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/main.h"
#include "system/net_err.h"
#include "net/dwc_error.h"

//�A�[�J�C�u(�G���[���b�Z�[�W�p)
#include "msg/msg_battle_rec.h"

//�O�����J
#include "br_net.h"

FS_EXTERN_OVERLAY(gds_comm);

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�V�[�P���X���[�N
//=====================================
typedef struct _BR_NET_SEQ_WORK BR_NET_SEQ_WORK;

//-------------------------------------
///	�o�g�����R�[�_�[�l�b�g����
//=====================================
struct _BR_NET_WORK
{ 
  
  GDS_RAP_WORK        gdsrap;         ///<GDS���C�u�����ANitroDWC�֘A�̃��[�N�\����
  GDS_PROFILE_PTR     p_gds_profile;  ///<GDS�̃v���t�B�[��
  BR_NET_SEQ_WORK     *p_seq;         ///<��ԊǗ�

  BR_NET_REQUEST_PARAM  reqest_param;    ///<���N�G�X�g���ꂽ����
  u32                   response_flag[BR_NET_REQUEST_MAX];  ///<���X�|���X���󂯂����ǂ����̃t���O
  GDS_RAP_ERROR_INFO    error_info;
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//��Ԋ֐�
static void Br_Net_Seq_Nop( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Net_Seq_WaitReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Net_Seq_UploadMusicalShotReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Net_Seq_DownloadMusicalShotReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Net_Seq_UploadBattleVideoReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Net_Seq_DownloadBattleVideoReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Net_Seq_DownloadBattleSearchReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Net_Seq_DownloadNewRankingReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Net_Seq_DownloadFavoriteRankingReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Net_Seq_DownloadSubwayRankingReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Net_Seq_UploadFavoriteBattleReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//���X�|���X�֐�
static void Br_Net_Response_MusicalRegist(void *p_wk_adrs, const GDS_RAP_ERROR_INFO *p_error_info);
static void Br_Net_Response_MusicalGet(void *p_wk_adrs, const GDS_RAP_ERROR_INFO *p_error_info);
static void Br_Net_Response_BattleVideoRegist(void *p_wk_adrs, const GDS_RAP_ERROR_INFO *p_error_info);
static void Br_Net_Response_BattleVideoSearch(void *p_wk_adrs, const GDS_RAP_ERROR_INFO *p_error_info);
static void Br_Net_Response_BattleVideoDataGet(void *p_wk_adrs, const GDS_RAP_ERROR_INFO *p_error_info);
static void Br_Net_Response_BattleVideoFavorite(void *p_wk_adrs, const GDS_RAP_ERROR_INFO *p_error_info);



//-------------------------------------
///	�V�[�P���X�V�X�e��
//=====================================
typedef void (*BR_NET_SEQ_FUNCTION)( BR_NET_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );

static BR_NET_SEQ_WORK *BR_NET_SEQ_Init( void *p_wk_adrs, BR_NET_SEQ_FUNCTION seq_function, HEAPID heapID );
static void BR_NET_SEQ_Exit( BR_NET_SEQ_WORK *p_wk );
static void BR_NET_SEQ_Main( BR_NET_SEQ_WORK *p_wk );
static BOOL BR_NET_SEQ_IsEnd( const BR_NET_SEQ_WORK *cp_wk );
static void BR_NET_SEQ_SetNext( BR_NET_SEQ_WORK *p_wk, BR_NET_SEQ_FUNCTION seq_function );
static void BR_NET_SEQ_End( BR_NET_SEQ_WORK *p_wk );
static void BR_NET_SEQ_SetReservSeq( BR_NET_SEQ_WORK *p_wk, int seq );
static void BR_NET_SEQ_NextReservSeq( BR_NET_SEQ_WORK *p_wk );
static BOOL BR_NET_SEQ_IsComp( const BR_NET_SEQ_WORK *cp_wk, BR_NET_SEQ_FUNCTION seq_function );




//----------------------------------------------------------------------------
/**
 *	@brief  �o�g�����R�[�_�[�ʐM  ������
 *
 *  @param  GAMEDATA        �Q�[���f�[�^
 *  @param  DWCSvlResult    �T�[�r�X���P�[�^
 *	@param	HEAPID heapID   �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
BR_NET_WORK *BR_NET_Init( GAMEDATA *p_gamedata, DWCSvlResult *p_svl, HEAPID heapID )
{ 
  BR_NET_WORK *p_wk;
  
  GFL_OVERLAY_Load( FS_OVERLAY_ID(gds_comm) );

  p_wk = GFL_HEAP_AllocMemory( heapID, sizeof(BR_NET_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(BR_NET_WORK) );

  //GDS������
  { 
    GDSRAP_INIT_DATA gdsrap_data;
    GFL_STD_MemClear( &gdsrap_data, sizeof(GDSRAP_INIT_DATA) );

    gdsrap_data.gamedata      = p_gamedata;
    gdsrap_data.gs_profile_id = MyStatus_GetProfileID( GAMEDATA_GetMyStatus( p_gamedata ) );
    gdsrap_data.pSvl          = p_svl;
    gdsrap_data.response_callback.callback_work                      = p_wk;
    gdsrap_data.response_callback.func_musicalshot_regist            = Br_Net_Response_MusicalRegist;
    gdsrap_data.response_callback.func_musicalshot_get               = Br_Net_Response_MusicalGet;
    gdsrap_data.response_callback.func_battle_video_regist           = Br_Net_Response_BattleVideoRegist;
    gdsrap_data.response_callback.func_battle_video_search_get       = Br_Net_Response_BattleVideoSearch;
    gdsrap_data.response_callback.func_battle_video_data_get         = Br_Net_Response_BattleVideoDataGet;
    gdsrap_data.response_callback.func_battle_video_favorite_regist  = Br_Net_Response_BattleVideoFavorite;

    GDSRAP_Init(&p_wk->gdsrap, &gdsrap_data);  //�ʐM���C�u����������
  }

  //GDS�v���t�B�[��
  {
    SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_gamedata );

    p_wk->p_gds_profile = GDS_Profile_AllocMemory( heapID );
    GDS_Profile_MyDataSet( p_wk->p_gds_profile, p_gamedata );
  }

  //��ԊǗ�
  { 
    p_wk->p_seq = BR_NET_SEQ_Init( p_wk, Br_Net_Seq_Nop, heapID );
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g�����R�[�_�[�ʐM  �j��
 *
 *	@param	BR_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_NET_Exit( BR_NET_WORK *p_wk )
{ 
  BR_NET_SEQ_Exit( p_wk->p_seq );

  GDS_Profile_FreeMemory( p_wk->p_gds_profile );

  GDSRAP_Exit(&p_wk->gdsrap);
  GFL_HEAP_FreeMemory( p_wk );

  GFL_OVERLAY_Unload( FS_OVERLAY_ID(gds_comm) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g�����R�[�_�[�ʐM  ���C������
 *
 *	@param	BR_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_NET_Main( BR_NET_WORK *p_wk )
{ 
  BR_NET_SEQ_Main( p_wk->p_seq );
  GDSRAP_Main(&p_wk->gdsrap);
}
//=============================================================================
/**
 *    ���N�G�X�g
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���N�G�X�g�J�n
 *
 *	@param	BR_NET_WORK *p_wk                 ���[�N
 *	@param	type                              ���N�G�X�g�̎��
 *	@param	BR_NET_REQUEST_PARAM *cp_param    ����
 */
//-----------------------------------------------------------------------------
void BR_NET_StartRequest( BR_NET_WORK *p_wk, BR_NET_REQUEST type, const BR_NET_REQUEST_PARAM *cp_param )
{ 
  GF_ASSERT( type < BR_NET_REQUEST_MAX );

  if( cp_param )
  { 
    p_wk->reqest_param  = *cp_param;
  }
  p_wk->response_flag[ type ] = FALSE;

  switch( type )
  { 
  case BR_NET_REQUEST_MUSICAL_SHOT_UPLOAD:     //�~���[�W�J���A�b�v���[�h
    BR_NET_SEQ_SetNext( p_wk->p_seq, Br_Net_Seq_UploadMusicalShotReq );
    break;
  case BR_NET_REQUEST_MUSICAL_SHOT_DOWNLOAD:   //�~���[�W�J���_�E�����[�h
    BR_NET_SEQ_SetNext( p_wk->p_seq, Br_Net_Seq_DownloadMusicalShotReq );
    break;
  case BR_NET_REQUEST_BATTLE_VIDEO_UPLOAD:     //�r�f�I�A�b�v���[�h
    BR_NET_SEQ_SetNext( p_wk->p_seq, Br_Net_Seq_UploadBattleVideoReq );
    break;
  case BR_NET_REQUEST_BATTLE_VIDEO_DOWNLOAD:   //�r�f�I�_�E�����[�h
    BR_NET_SEQ_SetNext( p_wk->p_seq, Br_Net_Seq_DownloadBattleVideoReq );
    break;
  case BR_NET_REQUEST_VIDEO_SEARCH_DOWNLOAD:   //�r�f�I�����_�E�����[�h
    BR_NET_SEQ_SetNext( p_wk->p_seq, Br_Net_Seq_DownloadBattleSearchReq );
    break;
  case BR_NET_REQUEST_NEW_RANKING_DOWNLOAD:    //�ŐV�����L���O�_�E�����[�h
    BR_NET_SEQ_SetNext( p_wk->p_seq, Br_Net_Seq_DownloadNewRankingReq );
    break;
  case BR_NET_REQUEST_FAVORITE_RANKING_DOWLOAD://�l�C�����L���O�_�E�����[�h
    BR_NET_SEQ_SetNext( p_wk->p_seq, Br_Net_Seq_DownloadFavoriteRankingReq );
    break;
  case BR_NET_REQUEST_SUBWAY_RANKING_DOWNLOAD: //�T�u�E�F�C�����L���O�_�E�����[�h
    BR_NET_SEQ_SetNext( p_wk->p_seq, Br_Net_Seq_DownloadSubwayRankingReq );
    break;
  case BR_NET_REQUEST_FAVORITE_VIDEO_UPLOAD:   //���C�ɓ���A�b�v���[�h
    BR_NET_SEQ_SetNext( p_wk->p_seq, Br_Net_Seq_UploadFavoriteBattleReq );
    break;
  default:
    GF_ASSERT( 0 );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���N�G�X�g�ҋ@
 *
 *	@param	BR_NET_WORK *p_wk   ���[�N
 *
 *	@return TRUE ���� FALSE ���s
 */
//-----------------------------------------------------------------------------
BOOL BR_NET_WaitRequest( BR_NET_WORK *p_wk )
{
  return BR_NET_SEQ_IsComp( p_wk->p_seq, Br_Net_Seq_Nop );
}
//=============================================================================
/**
 *    �擾
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �~���[�W�J���V���b�g�_�E�����[�h�擾
 *
 *	@param	BR_NET_WORK *p_wk ���[�N
 *	@param	*p_data_tbl       �󂯎��|�C���^�e�[�u��
 *	@param	tbl_max           �e�[�u���̗v�f��
 *	@param  p_recv_num        �f�[�^����
 *
 *	@retval  TRUE���݂���  FALSE���݂��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BR_NET_GetDownloadMusicalShot( BR_NET_WORK *p_wk, MUSICAL_SHOT_RECV **pp_data_tbl, int tbl_max, int *p_recv_num )
{ 
  *p_recv_num = GDS_RAP_RESPONSE_MusicalShot_Download_RecvPtr_Set( &p_wk->gdsrap,
      pp_data_tbl, tbl_max );

  return p_wk->response_flag[ BR_NET_REQUEST_MUSICAL_SHOT_DOWNLOAD ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���r�f�I�A�b�v���[�h�����Ƃ��̃o�g���r�f�I�i���o�[�擾
 *
 *	@param	BR_NET_WORK *p_wk ���[�N
 *	@param	*p_video_number   �r�f�I�i���o�[
 *
 *	@retval  TRUE���݂���  FALSE���݂��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BR_NET_GetUploadBattleVideoNumber( BR_NET_WORK *p_wk, u64 *p_video_number )
{ 
  *p_video_number = GDS_RAP_RESPONSE_BattleVideo_Upload_DataGet( &p_wk->gdsrap );
  return p_wk->response_flag[ BR_NET_REQUEST_BATTLE_VIDEO_UPLOAD ];
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���r�f�I�_�E�����[�h�擾
 *
 *	@param	BR_NET_WORK *p_wk         ���[�N
 *	@param  BATTLE_REC_RECV *p_data   �r�f�I��M�f�[�^�ւ̃|�C���^
 *	@param  int *p_video_number       �o�g���r�f�I�i���o�[
 *
 *	@retval  TRUE���݂���  FALSE���݂��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BR_NET_GetDownloadBattleVideo( BR_NET_WORK *p_wk, BATTLE_REC_RECV **pp_data, int *p_video_number )
{ 
  *p_video_number = GDS_RAP_RESPONSE_BattleVideoData_Download_RecvPtr_Set( &p_wk->gdsrap, 
	pp_data );

  return p_wk->response_flag[ BR_NET_REQUEST_BATTLE_VIDEO_DOWNLOAD ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���r�f�I�������ʎ擾
 *
 *	@param	BR_NET_WORK *p_wk ���[�N
 *	@param	*p_data_tbl      �󂯎��o�b�t�@�̃e�[�u��
 *	@param	tbl_max           �e�[�u���̗v�f��
 *	@param	*p_recv_num       �󂯎������
 *
 *	@retval  TRUE���݂���  FALSE���݂��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BR_NET_GetDownloadBattleSearch( BR_NET_WORK *p_wk, BATTLE_REC_OUTLINE_RECV *p_data_tbl, int tbl_max, int *p_recv_num )
{ 
  *p_recv_num = GDS_RAP_RESPONSE_BattleVideoSearch_Download_DataCopy( 
      &p_wk->gdsrap, p_data_tbl, tbl_max );

  return *p_recv_num != 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ŐV�����L���O�_�E�����[�h�擾
 *
 *	@param	BR_NET_WORK *p_wk ���[�N
 *	@param	*p_data_tbl      �󂯎��o�b�t�@�̃e�[�u��
 *	@param	tbl_max           �e�[�u���̗v�f��
 *	@param	*p_recv_num       �󂯎������
 *
 *	@retval  TRUE���݂���  FALSE���݂��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BR_NET_GetDownloadNewRanking( BR_NET_WORK *p_wk, BATTLE_REC_OUTLINE_RECV *p_data_tbl, int tbl_max, int *p_recv_num )
{ 
  *p_recv_num = GDS_RAP_RESPONSE_BattleVideoSearch_Download_DataCopy( 
      &p_wk->gdsrap, p_data_tbl, tbl_max );

  return *p_recv_num != 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �l�C�����L���O�_�E�����[�h�擾
 *
 *	@param	BR_NET_WORK *p_wk ���[�N
 *	@param	*p_data_tbl      �󂯎��o�b�t�@�̃e�[�u��
 *	@param	tbl_max           �e�[�u���̗v�f��
 *	@param	*p_recv_num       �󂯎������
 *
 *	@retval  TRUE���݂���  FALSE���݂��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BR_NET_GetDownloadFavoriteRanking( BR_NET_WORK *p_wk, BATTLE_REC_OUTLINE_RECV *p_data_tbl, int tbl_max, int *p_recv_num )
{ 
  *p_recv_num = GDS_RAP_RESPONSE_BattleVideoSearch_Download_DataCopy( 
      &p_wk->gdsrap, p_data_tbl, tbl_max );

  return *p_recv_num != 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �T�u�E�F�C�����L���O�_�E�����[�h�擾
 *
 *	@param	BR_NET_WORK *p_wk ���[�N
 *	@param	*p_data_tbl      �󂯎��o�b�t�@�̃e�[�u��
 *	@param	tbl_max           �e�[�u���̗v�f��
 *	@param	*p_recv_num       �󂯎������

 *	@retval  TRUE���݂���  FALSE���݂��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BR_NET_GetDownloadSubwayRanking( BR_NET_WORK *p_wk, BATTLE_REC_OUTLINE_RECV *p_data_tbl, int tbl_max, int *p_recv_num )
{ 
  *p_recv_num = GDS_RAP_RESPONSE_BattleVideoSearch_Download_DataCopy( 
      &p_wk->gdsrap, p_data_tbl, tbl_max );
  return *p_recv_num != 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^����M���̃G���[���擾
 *
 *	@param	BR_NET_WORK *p_wk   ���[�N
 *	@param	*p_msg_no           �G���[��\�����郁�b�Z�[�W�ԍ�
 *
 *	@return �G���[�̖߂��i�񋓌^�Q�Ɓj
 */
//-----------------------------------------------------------------------------
BR_NET_ERR_RETURN BR_NET_GetError( BR_NET_WORK *p_wk, int *p_msg_no )
{ 

  //poke_net����̃G���[
  if( p_wk->error_info.occ )
  { 
    int ret = BR_NET_ERR_RETURN_NONE;
	  if( p_wk->error_info.type == GDS_ERROR_TYPE_LIB )
    { 
      //���C�u�����̃G���[
      if( p_wk->error_info.result == POKE_NET_GDS_LASTERROR_NONE )
      { 
        ret = BR_NET_ERR_RETURN_NONE;
      }
      else
      { 
        if( p_msg_no )
        { 
          *p_msg_no = msg_lib_err_000 + p_wk->error_info.result;
        }

        ret = BR_NET_ERR_RETURN_ONCE;
      }
    }
    else if( p_wk->error_info.type == GDS_ERROR_TYPE_STATUS )
    { 
      //���C�u�����̏�ԃG���[
      if( p_wk->error_info.result == POKE_NET_GDS_STATUS_ABORTED )
      { 
        if( p_msg_no )
        { 
          *p_msg_no = msg_st_err_000 + p_wk->error_info.result;
        }
        ret = BR_NET_ERR_RETURN_ONCE;
      }
      else
      { 
        ret = BR_NET_ERR_RETURN_NONE;
      }
    }
    else if( p_wk->error_info.type == GDS_ERROR_TYPE_APP )
    { 
      //�A�v�����Ƃ̃G���[
      //
      switch( p_wk->error_info.req_code )
      { 
      case POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST:
        //�~���[�W�J�����M��
        if( p_wk->error_info.result != POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_SUCCESS )
        { 
          if( p_msg_no )
          { 
            *p_msg_no = msg_err_000 + p_wk->error_info.result;
          }
          ret = BR_NET_ERR_RETURN_TOPMENU;
        }
        break;

      case POKE_NET_GDS_REQCODE_MUSICALSHOT_GET:
        //�~���[�W�J���擾��
        if( p_wk->error_info.result != POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_GET_SUCCESS )
        { 
          if( p_msg_no )
          { 
            *p_msg_no = msg_err_007 + p_wk->error_info.result;
          }
          ret = BR_NET_ERR_RETURN_TOPMENU;
        }
        break;

      case POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST:
        //�o�g���r�f�I���M��
        if( p_wk->error_info.result != POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_SUCCESS )
        { 
          if( p_msg_no )
          { 
            *p_msg_no = msg_err_030 + p_wk->error_info.result;
          }
          ret = BR_NET_ERR_RETURN_TOPMENU;
        }
        break;

      case POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH:
        //�o�g���r�f�I������
        if( p_wk->error_info.result != POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_SUCCESS )
        { 
          if( p_msg_no )
          { 
            *p_msg_no = msg_err_036 + p_wk->error_info.result;
          }
          ret = BR_NET_ERR_RETURN_TOPMENU;
        }
        break;

      case POKE_NET_GDS_REQCODE_BATTLEDATA_GET:
        //�o�g���r�f�I�擾��
        if( p_wk->error_info.result != POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_SUCCESS )
        { 
          if( p_msg_no )
          { 
            *p_msg_no = msg_err_040 + p_wk->error_info.result;
          }
          ret = BR_NET_ERR_RETURN_ONCE;
        }
        break;

      case POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE:
        //�o�g���r�f�I���C�ɓ��著�M��
        //���[�U�[�̃��[�J���ɉe�����Ȃ��̂ŃG���[�ʒm���Ȃ�
        ret = BR_NET_ERR_RETURN_NONE;
        break;
      }
    }

    //�G���[����
    GDSRAP_ErrorInfoClear( &p_wk->gdsrap );
    GFL_STD_MemClear( &p_wk->error_info, sizeof(GDS_RAP_ERROR_INFO) );

    return ret;
  }

  return BR_NET_ERR_RETURN_NONE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �V�X�e���G���[���擾
 *
 *	@param	BR_NET_WORK *p_wk   ���[�N
 *
 *	@return �G���[�̖߂��i�񋓌^�Q�Ɓj
 */
//-----------------------------------------------------------------------------
BR_NET_SYSERR_RETURN BR_NET_GetSysError( BR_NET_WORK *p_wk )
{ 
  //�l�b�g�ڑ�����Ă��Ȃ��Ƃ��̓G���[�͂Ȃ�
  if( p_wk == NULL )
  { 
    return BR_NET_SYSERR_RETURN_NONE;
  }

  //DWC����̃G���[
  if( NetErr_App_CheckError() )
  { 
    GFL_NET_DWC_ERROR_RESULT  result;
    result  = GFL_NET_DWC_ERROR_ReqErrorDisp( TRUE );

    switch( result )
    { 
    case GFL_NET_DWC_ERROR_RESULT_PRINT_MSG:   //���b�Z�[�W��`�悷�邾��
      return BR_NET_SYSERR_RETURN_LIGHT;

    case GFL_NET_DWC_ERROR_RESULT_RETURN_PROC: //PROC���甲���Ȃ���΂Ȃ�Ȃ�
      return BR_NET_SYSERR_RETURN_DISCONNECT;

    case GFL_NET_DWC_ERROR_RESULT_FATAL:       //�d���ؒf�̂��ߖ������[�v�ɂȂ�
      NetErr_App_FatalDispCall();
      GF_ASSERT( 0 );
      break;
    }
  }

  return BR_NET_SYSERR_RETURN_NONE;
}

//=============================================================================
/**
 *  ��Ԋ֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �������Ȃ�
 *
 *	@param	BR_NET_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �����V�[�P���X
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Net_Seq_Nop( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  /*  none  */
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���N�G�X�g�҂�
 *
 *	@param	BR_NET_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �����V�[�P���X
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Net_Seq_WaitReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BR_NET_WORK *p_wk = p_wk_adrs;

  if( GDSRAP_MoveStatusAllCheck( &p_wk->gdsrap ) )
  { 
    BR_NET_SEQ_SetNext( p_seqwk, Br_Net_Seq_Nop );
  }

  //@todo �^�C���A�E�g�����H
}
//----------------------------------------------------------------------------
/**
 *	@brief  �~���[�W�J���V���b�g�A�b�v���[�h
 *
 *	@param	BR_NET_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �����V�[�P���X
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Net_Seq_UploadMusicalShotReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BOOL ret;
  BR_NET_WORK *p_wk = p_wk_adrs;

  ret = GDSRAP_Tool_Send_MusicalShotUpload( &p_wk->gdsrap, p_wk->p_gds_profile, p_wk->reqest_param.cp_upload_musical_shot_data );
  GF_ASSERT( ret );

  BR_NET_SEQ_SetNext( p_seqwk, Br_Net_Seq_WaitReq );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �~���[�W�J���V���b�g�_�E�����[�h
 *
 *	@param	BR_NET_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �����V�[�P���X
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Net_Seq_DownloadMusicalShotReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BOOL ret;
  BR_NET_WORK *p_wk = p_wk_adrs;

  ret = GDSRAP_Tool_Send_MusicalShotDownload( &p_wk->gdsrap, p_wk->reqest_param.download_musical_shot_search_monsno );
  GF_ASSERT( ret );

  BR_NET_SEQ_SetNext( p_seqwk, Br_Net_Seq_WaitReq );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���r�f�I�A�b�v���[�h
 *
 *	@param	BR_NET_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �����V�[�P���X
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Net_Seq_UploadBattleVideoReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BOOL ret;
  BR_NET_WORK *p_wk = p_wk_adrs;

  ret = GDSRAP_Tool_Send_BattleVideoUpload( &p_wk->gdsrap, p_wk->p_gds_profile );
  GF_ASSERT( ret );

  BR_NET_SEQ_SetNext( p_seqwk, Br_Net_Seq_WaitReq );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���r�f�I�_�E�����[�h
 *
 *	@param	BR_NET_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �����V�[�P���X
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Net_Seq_DownloadBattleVideoReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BOOL ret;
  BR_NET_WORK *p_wk = p_wk_adrs;

  ret = GDSRAP_Tool_Send_BattleVideo_DataDownload( &p_wk->gdsrap, p_wk->reqest_param.download_video_number );
  GF_ASSERT( ret );

  BR_NET_SEQ_SetNext( p_seqwk, Br_Net_Seq_WaitReq );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���r�f�I�������ʃ_�E�����[�h
 *
 *	@param	BR_NET_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �����V�[�P���X
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Net_Seq_DownloadBattleSearchReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BOOL ret;
  BR_NET_WORK *p_wk = p_wk_adrs;

  ret = GDSRAP_Tool_Send_BattleVideoSearchDownload( 
      &p_wk->gdsrap, 
      p_wk->reqest_param.download_video_search_data.monsno,
      p_wk->reqest_param.download_video_search_data.battle_mode_no,
      p_wk->reqest_param.download_video_search_data.country_code,
      p_wk->reqest_param.download_video_search_data.local_code
      );
  GF_ASSERT( ret );

  BR_NET_SEQ_SetNext( p_seqwk, Br_Net_Seq_WaitReq );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ŐV�����L���O�_�E�����[�h
 *
 *	@param	BR_NET_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �����V�[�P���X
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Net_Seq_DownloadNewRankingReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BOOL ret;
  BR_NET_WORK *p_wk = p_wk_adrs;

  ret = GDSRAP_Tool_Send_BattleVideoNewDownload( &p_wk->gdsrap );
  GF_ASSERT( ret );

  BR_NET_SEQ_SetNext( p_seqwk, Br_Net_Seq_WaitReq );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �l�C�����L���O�_�E�����[�h
 *
 *	@param	BR_NET_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �����V�[�P���X
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Net_Seq_DownloadFavoriteRankingReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BOOL ret;
  BR_NET_WORK *p_wk = p_wk_adrs;

  ret = GDSRAP_Tool_Send_BattleVideoCommDownload( &p_wk->gdsrap );
  GF_ASSERT( ret );

  BR_NET_SEQ_SetNext( p_seqwk, Br_Net_Seq_WaitReq );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���T�u�E�F�C�����L���O�_�E�����[�h
 *
 *	@param	BR_NET_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �����V�[�P���X
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Net_Seq_DownloadSubwayRankingReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BOOL ret;
  BR_NET_WORK *p_wk = p_wk_adrs;

  ret = GDSRAP_Tool_Send_BattleVideoSubwayDownload( &p_wk->gdsrap );
  GF_ASSERT( ret );

  BR_NET_SEQ_SetNext( p_seqwk, Br_Net_Seq_WaitReq );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���C�ɓ���r�f�I�A�b�v���[�h
 *
 *	@param	BR_NET_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �����V�[�P���X
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Net_Seq_UploadFavoriteBattleReq( BR_NET_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  BOOL ret;
  BR_NET_WORK *p_wk = p_wk_adrs;

  ret = GDSRAP_Tool_Send_BattleVideo_FavoriteUpload( &p_wk->gdsrap, p_wk->reqest_param.upload_favorite_video_number );
  GF_ASSERT( ret );

  BR_NET_SEQ_SetNext( p_seqwk, Br_Net_Seq_WaitReq );
}

//=============================================================================
/**
 *  ���X�|���X�֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �~���[�W�J���A�b�v���[�h���̃��X�|���X�R�[���o�b�N
 *
 *	@param	*p_wk_adrs                        ���[�N
 *	@param	GDS_RAP_ERROR_INFO *p_error_info  �G���[���
 */
//-----------------------------------------------------------------------------
static void Br_Net_Response_MusicalRegist(void *p_wk_adrs, const GDS_RAP_ERROR_INFO *p_error_info)
{ 
  BR_NET_WORK *p_wk = p_wk_adrs;
  OS_TPrintf("�~���[�W�J���V���b�g�̃A�b�v���[�h���X�|���X�擾\n");

  p_wk->response_flag[ BR_NET_REQUEST_MUSICAL_SHOT_UPLOAD ] = TRUE;
  p_wk->error_info  = *p_error_info;

}
//----------------------------------------------------------------------------
/**
 *	@brief  �~���[�W�J���_�E�����[�h���̃��X�|���X�R�[���o�b�N
 *
 *	@param	*p_wk_adrs                        ���[�N
 *	@param	GDS_RAP_ERROR_INFO *p_error_info  �G���[���
 */
//-----------------------------------------------------------------------------
static void Br_Net_Response_MusicalGet(void *p_wk_adrs, const GDS_RAP_ERROR_INFO *p_error_info)
{ 
  BR_NET_WORK *p_wk = p_wk_adrs;

  OS_TPrintf("�~���[�W�J���V���b�g�̃_�E�����[�h���X�|���X�擾\n");
  p_wk->response_flag[ BR_NET_REQUEST_MUSICAL_SHOT_DOWNLOAD ] = TRUE;
  p_wk->error_info  = *p_error_info;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���r�f�I�o�^���̃��X�|���X�R�[���o�b�N
 *
 *	@param	*p_wk_adrs                        ���[�N
 *	@param	GDS_RAP_ERROR_INFO *p_error_info  �G���[���
 */
//-----------------------------------------------------------------------------
static void Br_Net_Response_BattleVideoRegist(void *p_wk_adrs, const GDS_RAP_ERROR_INFO *p_error_info)
{ 
  BR_NET_WORK *p_wk = p_wk_adrs;

  OS_TPrintf("�o�g���r�f�I�o�^���̃_�E�����[�h���X�|���X�擾\n");
  if(p_error_info->occ == TRUE)
  {
    //TRUE�Ȃ�΃G���[�������Ă���̂ŁA�����Ń��j���[��߂��Ƃ��A�v���I�����[�h�ֈڍs�Ƃ�����
    switch(p_error_info->result){
  	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_SUCCESS:		//!< �o�^����
  		param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *)(res->Param);
  		OS_TPrintf("�o�g���r�f�I�o�^��M�����F�o�^�R�[�h%d\n", param->Code);
  		ret = TRUE;
  		break;
  	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_AUTH:		//!< ���[�U�[�F�؃G���[
  		//��check�@�G���[���Ƃɐ�p�̃P�A�ƃ��b�Z�[�W������
  		OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:���[�U�[�F�؃G���[\n");
  		break;
  	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ALREADY:	//!< ���łɓo�^����Ă���
  		//��check�@�G���[���Ƃɐ�p�̃P�A�ƃ��b�Z�[�W������
  		OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:���ɓo�^����Ă���\n");
  		break;
  	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGAL:	//!< �s���ȃf�[�^
  		//��check�@�G���[���Ƃɐ�p�̃P�A�ƃ��b�Z�[�W������
  		OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:�s���f�[�^\n");
  		break;
    case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGAL_RANKINGTYPE:  //!< �s���ȃ����L���O���
      OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:�s���ȃ����L���O���\n");
      break;
  	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPROFILE:	//!< �s���ȃ��[�U�[�v���t�B�[��
  		OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:�s���ȃ��[�U�[\n");
  		break;
  	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPOKEMON_VERIFY: //!< �|�P���������ŃG���[
  		OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:�|�P���������ŃG���[\n");
  	  break;
  	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_UNKNOWN:	//!< ���̑��G���[
  	default:
  		OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:���̑��̃G���[ %d\n", res->Result);
  		break;
    }
  }

  p_wk->response_flag[ BR_NET_REQUEST_BATTLE_VIDEO_UPLOAD ] = TRUE;
  p_wk->error_info  = *p_error_info;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���r�f�I�������̃��X�|���X�R�[���o�b�N
 *
 *	@param	*p_wk_adrs                        ���[�N
 *	@param	GDS_RAP_ERROR_INFO *p_error_info  �G���[���
 */
//-----------------------------------------------------------------------------
static void Br_Net_Response_BattleVideoSearch(void *p_wk_adrs, const GDS_RAP_ERROR_INFO *p_error_info)
{ 
  BR_NET_WORK *p_wk = p_wk_adrs;

  OS_TPrintf("�o�g���r�f�I�����̃_�E�����[�h���X�|���X�擾\n");
  p_wk->response_flag[ BR_NET_REQUEST_VIDEO_SEARCH_DOWNLOAD ] = TRUE;
  p_wk->error_info  = *p_error_info;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���r�f�I�_�E�����[�h���̃��X�|���X�R�[���o�b�N
 *
 *	@param	*p_wk_adrs                        ���[�N
 *	@param	GDS_RAP_ERROR_INFO *p_error_info  �G���[���
 */
//-----------------------------------------------------------------------------
static void Br_Net_Response_BattleVideoDataGet(void *p_wk_adrs, const GDS_RAP_ERROR_INFO *p_error_info)
{ 
  BR_NET_WORK *p_wk = p_wk_adrs;

  OS_TPrintf("�o�g���r�f�I�f�[�^�擾�̃_�E�����[�h���X�|���X�擾\n");

  p_wk->response_flag[ BR_NET_REQUEST_BATTLE_VIDEO_DOWNLOAD ] = TRUE;
  p_wk->error_info  = *p_error_info;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���r�f�I���C�ɓ���o�^���̃��X�|���X�R�[���o�b�N
 *
 *	@param	*p_wk_adrs                        ���[�N
 *	@param	GDS_RAP_ERROR_INFO *p_error_info  �G���[���
 */
//-----------------------------------------------------------------------------
static void Br_Net_Response_BattleVideoFavorite(void *p_wk_adrs, const GDS_RAP_ERROR_INFO *p_error_info)
{ 
  BR_NET_WORK *p_wk = p_wk_adrs;

  OS_TPrintf("�o�g���r�f�I���C�ɓ���o�^�̃_�E�����[�h���X�|���X�擾\n");
  p_wk->response_flag[ BR_NET_REQUEST_FAVORITE_VIDEO_UPLOAD ] =  TRUE;
  p_wk->error_info  = *p_error_info;
}

//=============================================================================
/**
 *  �V�[�P���X�V�X�e��
 */
//=============================================================================
//-------------------------------------
///	�V�[�P���X���[�N
//=====================================
struct _BR_NET_SEQ_WORK
{
	BR_NET_SEQ_FUNCTION	seq_function;		//���s���̃V�[�P���X�֐�
	int seq;											//���s���̃V�[�P���X�֐��̒��̃V�[�P���X
	void *p_wk_adrs;							//���s���̃V�[�P���X�֐��ɓn�����[�N
  int reserv_seq;               //�\��V�[�P���X
};

//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	������
 *
 *	@param	BR_NET_SEQ_WORK *p_wk	���[�N
 *	@param	*p_param				�p�����[�^
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static BR_NET_SEQ_WORK * BR_NET_SEQ_Init( void *p_wk_adrs, BR_NET_SEQ_FUNCTION seq_function, HEAPID heapID )
{	
  BR_NET_SEQ_WORK *p_wk;

	//�쐬
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(BR_NET_SEQ_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(BR_NET_SEQ_WORK) );

	//������
	p_wk->p_wk_adrs	= p_wk_adrs;

	//�Z�b�g
	BR_NET_SEQ_SetNext( p_wk, seq_function );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�j��
 *
 *	@param	BR_NET_SEQ_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void BR_NET_SEQ_Exit( BR_NET_SEQ_WORK *p_wk )
{
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���C������
 *
 *	@param	BR_NET_SEQ_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void BR_NET_SEQ_Main( BR_NET_SEQ_WORK *p_wk )
{	
	if( p_wk->seq_function )
	{	
		p_wk->seq_function( p_wk, &p_wk->seq, p_wk->p_wk_adrs );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I���擾
 *
 *	@param	const BR_NET_SEQ_WORK *cp_wk		���[�N
 *
 *	@return	TRUE�Ȃ�ΏI��	FALSE�Ȃ�Ώ�����
 */	
//-----------------------------------------------------------------------------
static BOOL BR_NET_SEQ_IsEnd( const BR_NET_SEQ_WORK *cp_wk )
{	
	return cp_wk->seq_function == NULL;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���̃V�[�P���X��ݒ�
 *
 *	@param	BR_NET_SEQ_WORK *p_wk	���[�N
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void BR_NET_SEQ_SetNext( BR_NET_SEQ_WORK *p_wk, BR_NET_SEQ_FUNCTION seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I��
 *
 *	@param	BR_NET_SEQ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void BR_NET_SEQ_End( BR_NET_SEQ_WORK *p_wk )
{	
  BR_NET_SEQ_SetNext( p_wk, NULL );
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ ���̃V�[�P���X��\��
 *
 *	@param	BR_NET_SEQ_WORK *p_wk  ���[�N
 *	@param	seq             ���̃V�[�P���X
 */
//-----------------------------------------------------------------------------
static void BR_NET_SEQ_SetReservSeq( BR_NET_SEQ_WORK *p_wk, int seq )
{ 
  p_wk->reserv_seq  = seq;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ �\�񂳂ꂽ�V�[�P���X�֔��
 *
 *	@param	BR_NET_SEQ_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BR_NET_SEQ_NextReservSeq( BR_NET_SEQ_WORK *p_wk )
{ 
  p_wk->seq = p_wk->reserv_seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���݂̃V�[�P���X�Ɠ������ǂ���
 *
 *	@param	const BR_NET_SEQ_WORK *cp_wk  ���[�N
 *	@param	seq_function              �V�[�P���X
 *
 *	@return TRUE���� FALSE�قȂ�
 */
//-----------------------------------------------------------------------------
static BOOL BR_NET_SEQ_IsComp( const BR_NET_SEQ_WORK *cp_wk, BR_NET_SEQ_FUNCTION seq_function )
{ 
  return cp_wk->seq_function  == seq_function;
}
