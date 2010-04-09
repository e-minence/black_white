///////////////////////////////////////////////////////////////////////////////
/**
 *	@brief	  fld_trade���[�J���\���̒�`
 *	@author		tomoya takahasi, miyuki iwasawa	(obata)
 *	@data		  2006.05.15 (2009.12.09 HGSS���ڐA)
 */
///////////////////////////////////////////////////////////////////////////////
#ifndef __H_FIELD_TRADE_LOCAL_H__
#define __H_FIELD_TRADE_LOCAL_H__

//-------------------------------------
//	�����|�P�����f�[�^
//=====================================
typedef struct 
{
	u32 monsno;			    // �����X�^�[�i���o�[
  u32 formno;         // �t�H�[���i���o�[
  u32 level;          // ���x��
	u32 hp_rnd;			    // HP����
	u32 at_rnd;			    // �U������
	u32 df_rnd;			    // �h�䗐��
	u32 sp_rnd;			    // ��������
	u32 sa_rnd;			    // ���U����
	u32 sd_rnd;			    // ���h����
	u32 tokusei_idx;		// ���� ( ���Ԗڂ̓����� )
  u32 seikaku;        // ���i
  u32 sex;            // ����
	u32	mons_id;		    // ID
	u32 style;			    // �������悳
	u32 beautiful;	    // ��������
	u32 cute;			      // ���킢��
	u32 clever;			    // ��������
	u32 strong;			    // �����܂���
	u32 item;			      // �A�C�e��
	u32 oya_sex;		    // �e����
	u32 fur;			      // �щ�
	u32 world_code;		  // �e�̍��R�[�h
	u32 change_monsno;	// �������郂���X�^�[�i���o�[
	u32 change_monssex;	// ��������|�P�����̐��ʁi0:�I�X,1:���X,2:���ʖ�킸)

} FLD_TRADE_POKEDATA;

#include "include/msg/msg_fld_trade.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
#define FLD_TRADE_GET_POKE_GMM(x)	(x)
#define FLD_TRADE_GET_OYA_GMM(x)	(TRADE_OYA_00+x)

#endif //__H_FIELD_TRADE_LOCAL_H__
