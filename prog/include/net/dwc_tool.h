//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		dwc_tool.h
 *	@brief  WIFI�֌W�̋��ʃ��W���[���w�b�_
 *	@author	Toru=Nagihashi
 *	@date		2010.03.20
 *
 *  �I�[�o�[���C�𖳎����邽�߁Astatic inline��`�ł�
 *
 *  ����̃c�[���͈ȉ��ł�
 *  �E�s�������`�F�b�N    [DWC_TOOL_BADWORD]
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
//���C�u����
#include <gflib.h>
#include <dwc.h>

//�V�X�e��
#include "system/main.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "message.naix"
#include "msg/msg_namein.h"

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
//
//    �s�������`�F�b�N  ���ʏ���
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
//=============================================================================
/**
 *					�\����
*/
//=============================================================================
#define DWC_TOOL_BADWORD_STR_MAX  (10+EOM_SIZE) //�b��łP�O����+EOM�܂łł�
//-------------------------------------
///	�s���������[�N
//=====================================
typedef struct 
{
  STRCODE   badword_str[ DWC_TOOL_BADWORD_STR_MAX ];
  u16       *p_badword_arry[1];
  char      badword_result[1];
  int       badword_num;

  HEAPID    heapID;
} DWC_TOOL_BADWORD_WORK;


//=============================================================================
/**
 *					�֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �s�������`�F�b�N���X�^�[�g
 *
 *	@param	DWC_TOOL_BADWORD_WORK *p_wk   ���[�N
 *	@param	STRCODE *cp_str                �`�F�b�N����STRBUF(���ŃR�s�[����̂ł���������č\���܂���)
 *	@param	heapID                        �e���|�����p�q�[�vID
 */
//-----------------------------------------------------------------------------
static inline void DWC_TOOL_BADWORD_Start( DWC_TOOL_BADWORD_WORK *p_wk, const STRBUF *cp_str, HEAPID heapID )
{ 
  BOOL ret;

  GFL_STD_MemClear( p_wk, sizeof(DWC_TOOL_BADWORD_WORK));
  p_wk->heapID    = heapID;

/*
  # �����R�[�h��Unicode�i���g���G���f�B�A��UTF16�j���g�p���Ă��������B
  ����ȊO�̕����R�[�h���g�p���Ă���ꍇ�́AUnicode�ɕϊ����Ă��������B
  # �X�N���[���l�[����Unicode�y��IPL�t�H���g�ɂȂ��Ǝ��̕������g�p���Ă���ꍇ�́A�X�y�[�X�ɒu�������Ă��������B
  # �I�[��"\0\0"�iu16��0x0000�j�ł���K�v������܂��B
  # �z����̑S�Ă̕�����̍��v��501�����܂Łi�e������̏I�[���܂ށj�ɂ���K�v������܂��B
  # �z����̕������NULL���w�肷�邱�Ƃ͂ł��܂���B 
  */
  { 
    int i;
    const STRCODE *cp_strcode = GFL_STR_GetStringCodePointer( cp_str );
    GF_ASSERT( GFL_STR_GetBufferLength( cp_str )+EOM_SIZE < DWC_TOOL_BADWORD_STR_MAX );
    for( i = 0; i < GFL_STR_GetBufferLength( cp_str )+EOM_SIZE; i++ )
    { 
      if( GFL_STR_GetEOMCode() == cp_strcode[i] )
      {
        p_wk->badword_str[i]  = 0x0000;
      }
      else
      { 
        p_wk->badword_str[i]  = cp_strcode[i];
      }
    }
    p_wk->p_badword_arry[0] = p_wk->badword_str;
  }
  p_wk->badword_num = 0;


  ret = DWC_CheckProfanityExAsync( (const u16 **)p_wk->p_badword_arry,
                             1,
                             NULL,
                             0,
                             p_wk->badword_result,
                             &p_wk->badword_num,
                             DWC_PROF_REGION_ALL );
  GF_ASSERT( ret );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �s�������`�F�b�N���C��
 *
 *	@param	DWC_TOOL_BADWORD_WORK *p_wk    ���[�N
 *	@param	*p_is_bad_word                    TRUE�Ȃ�Εs������  FALSE�Ȃ�ΐ��핶��
 *
 *	@return TRUE�����I��  FALSE������
 */
//-----------------------------------------------------------------------------
static inline BOOL DWC_TOOL_BADWORD_Wait( DWC_TOOL_BADWORD_WORK *p_wk, BOOL *p_is_bad_word )
{ 
  BOOL ret;
  ret = DWC_CheckProfanityProcess() == DWC_PROF_STATE_SUCCESS;
  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j�b�N�l�[���̕s�������擾
 *
 *	@param	heapID              STRBUF�쐬�p�q�[�vID
 *	@retval STRBUF              strbuf
 */
//-----------------------------------------------------------------------------
static inline STRBUF * DWC_TOOL_CreateBadNickName( HEAPID heapID )
{ 
  GFL_MSGDATA *p_msg  = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_namein_dat, heapID );
  STRBUF      *p_strbuf;

  if ( GET_VERSION() == VERSION_BLACK )
  {
    //�u���b�N�o�[�W�����̏���
    p_strbuf  = GFL_MSG_CreateString( p_msg, NAMEIN_DEF_NAME_005 );
  }
  else
  {
    //�z���C�g�o�[�W�����̏���
    p_strbuf  = GFL_MSG_CreateString( p_msg, NAMEIN_DEF_NAME_004 );
  }

  GFL_MSG_Delete( p_msg );

  return p_strbuf;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �j�b�N�l�[���̕s������STRCODE�ɐݒ�
 *
 *	@param	STRCODE *p_strcode  STRCODE
 *	@param	len                 EOM���܂ޒ���
 *	@param	heapID              �e���|�����p�q�[�vID
 */
//-----------------------------------------------------------------------------
static inline void DWC_TOOL_SetBadNickName( STRCODE *p_strcode, u16 len, HEAPID heapID )
{
  GF_ASSERT( p_strcode );
  GF_ASSERT( len );
  { 
    STRBUF * p_src_buff = DWC_TOOL_CreateBadNickName( heapID );
    const STRCODE *p_src_code = GFL_STR_GetStringCodePointer( p_src_buff );

    int i;
    for( i = 0; i < len-1 && i < GFL_STR_GetBufferLength(p_src_buff); i++ ) //-1��EOM�����l������
    { 
      p_strcode[i]  = p_src_code[i];
    }
    p_strcode[i] = GFL_STR_GetEOMCode();

    GFL_STR_DeleteBuffer( p_src_buff );
  }
}
