//============================================================================================
/**
 * @file	  debug_wordsearch.h
 * @brief	  �f�o�b�O�p�A�����񌟍��ɂ�� GFL_SKB ���g���₷�����郋�[�`��
            �ǂ̃I�[�o�[���C����ł�����Ȃ��悤��
            �w�b�_�[�C�����C���`���ɂȂ��Ă���
 * @author	k.ohno
 * @date	  2009.08.19
 */
//============================================================================================

#pragma once

//------------------------------------------------------------------------------
/**
 * @brief   ������r
 * @param   const STRBUF* str1   ��r������
 * @param   const STRBUF* str2   ��r������
 * @param   u32 len              ����
 * @retval  �T������TRUE
 */
//------------------------------------------------------------------------------

static inline BOOL DEBUG_WORDSEARCH_sub_strncmp( const STRBUF* str1, const STRBUF* str2, u32 len )
{
	if( GFL_STR_GetBufferLength(str1) < len ){
		return FALSE;
	}
	if( GFL_STR_GetBufferLength(str2) < len ){
		return FALSE;
	}

	{
		const STRCODE *p1 = GFL_STR_GetStringCodePointer( str1 );
		const STRCODE *p2 = GFL_STR_GetStringCodePointer( str2 );
		u32 i;
		for(i=0; i<len; ++i){
			if( *p1++ != *p2++ ){ return FALSE; }
		}
		return TRUE;
	}
	return FALSE;
}


//------------------------------------------------------------------------------
/**
 * @brief   �ڕW������̃T�[�`
 * @param   GFL_MSGDATA *CompMsgManager;	�T���l�r�f���i�[����Ă���}�l�[�W���[
 * @param   STRBUF* fullword;             �擾����ׂ̃e���|����������
 * @param   const STRBUF* word         �T��������
 * @param   int dir                    ���� 1 �� -1
 * @param   int* startidx              �T���ŏ���index�ʒu �T��������index�ʒu
 * @retval  �T������TRUE
 */
//------------------------------------------------------------------------------

static inline BOOL DEBUG_WORDSEARCH_sub_search( GFL_MSGDATA *CompMsgManager, STRBUF* fullword, const STRBUF* word, int dir, int* startidx )
{
	int word_len = GFL_STR_GetBufferLength( word );
	if( word_len )
	{
		int str_cnt, match_cnt, i, allcnt=0;

		str_cnt = GFL_MSG_GetStrCount( CompMsgManager ); //������S��

		i = *startidx;
		while( allcnt < str_cnt )
		{
			i += dir;
			if(i < 0){
				i = str_cnt-1;
			}
			else if(i >= str_cnt){
				i = 0;
			}
			GFL_MSG_GetString( CompMsgManager, i, fullword );
			if( DEBUG_WORDSEARCH_sub_strncmp( word, fullword, GFL_STR_GetBufferLength(word) ) ){
				*startidx = i;
				return TRUE;
			}
			allcnt++;
		}
	}
	return FALSE;
}


