//=============================================================================
/**
 * @file	net_whpipe_deb.c
 * @brief	�ʐM�V�X�e��  �t�B�[���h���l���̓f�o�b�O�@�\�I�[�o�[���C�z�u�\�[�X
 * @author	GAME FREAK Inc.
 * @date    2010.05.21
 *
 * @note  �ʐMOff���ł����l���͎��f�o�b�O�@�\����Ăяo����֐���z�u���邽�߂ɁA
 *        net_whpipe.c���番�� by iwasawa
 */
//=============================================================================
#include "net/net_whpipe.h"

#ifdef PM_DEBUG

#include "net_alone_test.h"
extern u8 DebugAloneTest;

//-----------------------------------------------
/**
 *  @brief  �ڑ��������R�[�h�̃f�o�b�O���l���́@
 */
//-----------------------------------------------
u32 DEBUG_NET_WHPIPE_AloneTestCodeGet( void )
{
  if(DebugAloneTest == _DEBUG_ALONETEST_DEFAULT){
    return 0;
  }
  return DebugAloneTest;
}

///�l��ݒ肷�邽�߂̊֐�
void DEBUG_NET_WHPIPE_AloneTestCodeSet( u32 value )
{
  if( value == 0 ){
    value = _DEBUG_ALONETEST_DEFAULT;
  }
  DebugAloneTest = value;

  //�ʐM���L���Ȃ�A�ݒ�l�𑦎����f
  if(GFL_NET_IsInit() == TRUE){
    DEBUG_GFL_NET_AloneTestCodeReflect();
  }
}

#endif  //PM_DEBUG


