/*---------------------------------------------------------------------------*
  Project:  Pokemon DP WiFi
  File:     trans_pokecode.h

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*! @file
	@brief	�|�P�����R�[�h<->UTF-16LE�ϊ����C�u����
	
	@author	kitayama(kitayama_shigetoshi@nintendo.co.jp)
	
	@version 0.01 (2006/07/20)
		@li �V�K�����[�X���܂����B
*/

#ifndef TRANS_POKECODE_H_
#define TRANS_POKECODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>
#include "dpw_typedef.h"

/*-----------------------------------------------------------------------*
					�^�E�萔�錾
 *-----------------------------------------------------------------------*/

#define POKECODE_UNUSED_01			0xe0
#define POKECODE_RESERVED_BEGIN		0x1e0
#define POKECODE_RESERVED_END		0x1fd
#define POKECODE_RESERVED2_BEGIN	0x201   // �n���O���܂ł͈̔�
#define POKECODE_RESERVED2_END		0x400
#define POKECODE_HANGEUL_BEGIN		(POKECODE_RESERVED2_END + 1)
#define POKECODE_HANGEUL_END		0xD6f
#define POKECODE_NUM				POKECODE_HANGEUL_END
#define POKECODE_EOM				0xffff


/*-----------------------------------------------------------------------*
					�O���[�o���ϐ���`
 *-----------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*
					�֐��O���錾
 *-----------------------------------------------------------------------*/

//--- Auto Function Prototype --- Don't comment here.
s32 pokecode_to_unicode(const u16* pokecode, wchar_t* unicode, s32 unicode_buf_size);
s32 unicode_to_pokecode(const wchar_t* unicode, u16* pokecode, s32 pokecode_buf_size);
BOOL IsHangeulName(const u16* pokecode, u32 length);
//--- End of Auto Function Prototype

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TRANS_POKECODE_H_ */
