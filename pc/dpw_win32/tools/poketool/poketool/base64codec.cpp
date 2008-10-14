/*---------------------------------------------------------------------------*
  Project:  Pokemon DP WiFi
  File:     base64codec.c

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include "base64codec.h"
#include "dpw_assert.h"

/*-----------------------------------------------------------------------*
					�^�E�萔�錾
 *-----------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*
					�֐��v���g�^�C�v�錾
 *-----------------------------------------------------------------------*/

//--- Auto Function Prototype --- Don't comment here.
static unsigned char base64(unsigned char a);
//--- End of Auto Function Prototype

/*-----------------------------------------------------------------------*
					�O���[�o���ϐ���`
 *-----------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*
					�O���[�o���֐���`
 *-----------------------------------------------------------------------*/

/*!
 *	@brief	BASE64�G���R�[�h
 *  
 *  NULL�I�[�͍s���܂���̂ł����ӂ��������B
 *  
 *	@param[in]	in		���̓f�[�^
 *	@param[out]	out		�o�͐�Bbase64size(in_size)�ȏ�̃T�C�Y��p�ӂ��Ă��������B
 *  @param[in]  in_size	���̓f�[�^�̃T�C�Y
 *
 */
void base64encode(const u8* in, char* out, u32 in_size){

	const unsigned char base64[] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
	u32 loopcount;
	u32 i;

	loopcount = (int)(in_size / 3) * 3;	// floor of div 3

	for( i=0; i<loopcount; i+=3 ){

		*(out++) = base64[(in[0] & 0xff) >> 2];
		*(out++) = base64[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
		*(out++) = base64[((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)];
		*(out++) = base64[in[2] & 0x3f];

		in += 3;
	}


	switch(in_size - loopcount){

	case 2:

		*(out++) = base64[(in[0] & 0xff) >> 2];
		*(out++) = base64[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
		*(out++) = base64[(in[1] & 0x0f) << 2];
		*(out++) = (unsigned char)'=';

		break;

	case 1:

		*(out++) = base64[(in[0] & 0xff) >> 2];
		*(out++) = base64[(in[0] & 0x03) << 4];
		*(out++) = (unsigned char)'=';
		*(out++) = (unsigned char)'=';

		break;
	}

}

/*!
 *	@brief	BASE64�f�R�[�h
 *
 *	@param[in]	in		���̓f�[�^
 *	@param[in]	out		�o�͐�Bin_size / 4 * 3 �ȏ�̃T�C�Y��p�ӂ��Ă��������B
 *  @param[in]  in_size	���̓f�[�^�̃T�C�Y�B0���傫���l��^���Ă��������B
 *
 *  @retval 0	�f�R�[�h���s
 *  @retval 1�ȏ�	�f�R�[�h�����B�Ԃ��ꂽ�l�̃T�C�Y�ŏo�͂���܂����B
 */
u32 base64decode(const char* in, u8* out, u32 in_size){
	
	u32 i;
	u32 pos = 0;

	DPW_TASSERTMSG(in_size != 0, "in_size must not be 0.\n");

	// len ��4�̔{������Ȃ�������s��
	if( in_size % 4 != 0 )
		return 0;

	// 4�o�C�g������
	for( i=0; i<in_size; i+=4 ){

		unsigned char a,b,c,d;

		a = base64(*in++) & 0x3f;
		b = base64(*in++) & 0x3f;
		c = base64(*in++) & 0x7f;
		d = base64(*in++) & 0x7f;

		out[pos++] = ((a << 2) | (b >> 4)) & 0xff;

		if( c < 64 ){

			out[pos++] = ((b << 4) | (c >> 2)) & 0xff;

			if( d < 64 )
				out[pos++] = ((c << 6) | d) & 0xff;

		}

	}

	return pos;
}

/*!
	@brief base64�G���R�[�h��̃T�C�Y�����߂܂��B
	
	@param[in] in_size		�G���R�[�h����f�[�^�T�C�Y

	@return	�G���R�[�h��̃T�C�Y
*/
u32 base64size(u32 in_size) {

	return ((in_size / 3) + (in_size % 3 != 0 ? 1 : 0)) * 4;
}


/*-----------------------------------------------------------------------*
					���[�J���֐���`
 *-----------------------------------------------------------------------*/

/*!
 *	@brief	BASE64�̑Ή��l���擾
 *
 *	@param	a	[in]	���͒l
 *
 *	@return	6bit�l
 *	@retval	64	'='
 */
static unsigned char base64(unsigned char a){

	if(('A' <= a) && ('Z' >= a)){

		return a - 'A';

	}else if(('a' <= a) && ('z' >= a)){
	
		return a - 'a' + 26;
	
	}else if(('0' <= a) && ('9' >= a)){
	
		return a - '0' + 52;
	
	}else{
		switch( a ){
		case '+':
		case '-':	// for URL safe
			return 62;
		case '/':
		case '_':	// for URL safe
			return 63;
		default:
			return 64;
		}
	}

}

