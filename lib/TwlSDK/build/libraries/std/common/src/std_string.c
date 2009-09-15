/*---------------------------------------------------------------------------*
  Project:  TwlSDK - libraries - STD
  File:     std_string.c

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

// �����񑀍�֐��������}�N��
//   (�����̓v���b�g�t�H�[���Ɉˑ����Ȃ����̂�ARM7�ŗL���ɂ����
//    �R���|�[�l���g�����ӂ�邽��ARM9�ł̂ݗL���ɂ���)
#if defined(SDK_ARM9)
// ���Ȃ��Ƃ�GetStringLength�̍����ł��r�b�O�G���f�B�A�����l�����Ă��Ȃ����߁A
// �O�̂��ߑS�Ă̊֐��̎����𓮍�m�F����܂Ń��g���G���f�B�A������B
#if (PLATFORM_BYTES_ENDIAN == PLATFORM_ENDIAN_LITTLE)
#define STRLEN_4BYTE_SKIP		// 4byte�]���͔�0�`�F�b�N�̂�
#define STRLCPY_4BYTE           // 4byte���ɃR�s�[�]��
#define STRNLEN_4BYTE_SKIP      // 4byte���Ɍv��
#define STRCMP_4BYTE            // 4byte���ɔ�r
#define STRNCMP_4BYTE           // 4byte���ɔ�r  <- �s������邽�ߖ���
//#define STRNICMP_DIFF         // ��r�����P
#endif
#endif

/*---------------------------------------------------------------------------*
  Name:         STD_CopyString

  Description:  same to strcpy

  Arguments:    destp : destination pointer
                srcp  : src pointer

  Returns:      pointer to destination
 *---------------------------------------------------------------------------*/
char   *STD_CopyString(char *destp, const char *srcp)
{
    char   *retval = destp;

    SDK_ASSERT(destp && srcp);

    while (*srcp)
    {
        *destp++ = (char)*srcp++;
    }
    *destp = '\0';

    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         STD_CopyLStringZeroFill

  Description:  do not correspond with strlcpy

  Arguments:    destp : destination pointer
                srcp  : src pointer
                n     : copy size + 1

  Returns:      size of src
 *---------------------------------------------------------------------------*/
int STD_CopyLStringZeroFill(char *destp, const char *srcp, int n)
{
    int     i;
    const char *s = srcp;

    SDK_ASSERT(destp && srcp && n >= 0);

    for (i = 0; i < n - 1; i++)
    {
        destp[i] = (char)*s;
        if (*s)
        {
            s++;
        }
    }
    if( i < n )
    {
        destp[i] = '\0';
    }

    return STD_GetStringLength(srcp);
}

/*---------------------------------------------------------------------------*
  Name:         STD_CopyLString

  Description:  same to strlcpy

  Arguments:    destp : destination pointer
                srcp  : src pointer
                siz   : copy size + 1

  Returns:      size of src
 *---------------------------------------------------------------------------*/
int STD_CopyLString(char *destp, const char *srcp, int siz)
{
#if defined(STRLCPY_4BYTE)
	unsigned long  dstalign, srcalign;
	int            i;
	unsigned long  val32, y;
    const char *s = srcp;

    SDK_ASSERT(destp && srcp && siz >= 0);

	// 4byte�A���C���`�F�b�N
	dstalign = (unsigned long)destp & 0x3;
	srcalign = (unsigned long)srcp  & 0x3;

	i = 0;
	// �����Ă��Ȃ���Ε��ʂɃR�s�[
	if( dstalign ^ srcalign )
	{
		goto strlcpy_normal;
	}

	// �擪4byte�`�F�b�N�R�s�[
	if( dstalign )
	{
		for( ; (i + dstalign < 4) && (i < siz - 1); i++ )
		{
	        *(destp + i) = (char)*s;
    	    if (*s)
        	{
            	s++;
        	}
        	else
        	{
            	goto terminate;
        	}
		}
	}

	// 4byte�R�s�[
	for( ; i < siz - 1 - 4 + 1; i += 4 )
	{
		val32 = *(unsigned long*)(srcp + i);
		y = (val32 & 0x7F7F7F7F) + 0x7F7F7F7F;
		y = ~(y | val32 | 0x7F7F7F7F);
		
		if( y != 0 )
			break;

		*(unsigned long*)(destp + i) = val32;
	}

	// �c��byte�R�s�[
	s = srcp + i;

strlcpy_normal:
	// byte�R�s�[
    for ( ; i < siz - 1; i++ )
    {
        destp[i] = (char)*s;
        if (*s)
        {
            s++;
        }
        else
        {
            break;
        }
    }

terminate:
	// �I�[��NULL�ǉ�
    if ((i >= siz - 1) && (siz > 0))
    {
        destp[i] = '\0';
    }

	if( !*s )
    	return (int)i;
	else
		return (int)i + STD_GetStringLength( s );
#else  // original
    int     i;
    const char *s = srcp;

    SDK_ASSERT(destp && srcp && siz >= 0);

    for (i = 0; i < siz - 1; i++)
    {
        destp[i] = (char)*s;
        if (*s)
        {
            s++;
        }
        else
        {
            break;
        }
    }

    if ((i >= siz - 1) && (siz > 0))
    {
        destp[i] = '\0';
    }

    return STD_GetStringLength(srcp);
#endif
}

/*---------------------------------------------------------------------------*
  Name:         STD_SearchChar

  Description:  same to strchr

  Arguments:    srcp : src string
                c    : character to search from src pointer

  Returns:      pointer to destination
 *---------------------------------------------------------------------------*/
char    *STD_SearchChar(const char *srcp, int c)
{
    SDK_POINTER_ASSERT(srcp);

    for (;;srcp++)
    {
        if (c == *srcp)
        {
            return (char*)srcp;
        }
        if ('\0' == *srcp)
        {
            return NULL;
        }
    }
    // not reach here.
}

/*---------------------------------------------------------------------------*
  Name:         STD_SearchCharReverse

  Description:  same to strrchr

  Arguments:    srcp : src string
                c    : character to search from src pointer

  Returns:      pointer to destination
 *---------------------------------------------------------------------------*/
char    *STD_SearchCharReverse(const char *srcp, int c)
{
    const char* found = NULL;

    SDK_POINTER_ASSERT(srcp);

    for (;;srcp++)
    {
        if( c == *srcp )
        {
            found = srcp;
        }

        if( '\0' == *srcp)
        {
            return (char*)found;
        }
    }
    // not reach here.
}

/*---------------------------------------------------------------------------*
  Name:         STD_SearchString

  Description:  same to strstr

  Arguments:    srcp : src string
                str  : string to search from src pointer

  Returns:      pointer to destination
 *---------------------------------------------------------------------------*/
char   *STD_SearchString(const char *srcp, const char *str)
{
    int     i;
    int     n;

    SDK_ASSERT(srcp && str);

    for (i = 0; srcp[i]; i++)
    {
        n = 0;
        while (str[n] && srcp[i + n] == str[n])
        {
            n++;
        }

        if (str[n] == '\0')
        {
            return (char *)&srcp[i];
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         STD_GetStringLength

  Description:  get string length. same to strlen

  Arguments:    str : string

  Returns:      string length
 *---------------------------------------------------------------------------*/
int STD_GetStringLength(const char *str)
{
#if defined(STRLEN_4BYTE_SKIP)
    int     n;
	unsigned long  *p32, y;

	for( n=0; ((unsigned long)(str + n) & 3) != 0; n++ )
	{
		if(!str[n])
			return n;
	}

	p32 = (unsigned long*)(str + n);
	for( ; ; n+=4 )
    {
		y = (*p32 & 0x7F7F7F7F) + 0x7F7F7F7F;
		y = ~(y | *p32 | 0x7F7F7F7F);

		if( y != 0 )
			break;

		p32++;
	}

#if 1
	while (str[n])
    {
        n++;
    }
    return n;
#else
	if( y & 0x000000FF )
		return n;
	else if( y & 0x0000FF00 )
		return n + 1;
	else if( y & 0x00FF0000 )
		return n + 2;
	else
		return n + 3;
	// �����ɓ��B���邱�Ƃ͂��肦�Ȃ�
	return -1;
#endif

#else    // original
    int     n = 0;
    while (str[n])
    {
        n++;
    }
    return n;
#endif
}

/*---------------------------------------------------------------------------*
  Name:         STD_GetStringNLength

  Description:  get string length below len. same to strnlen

  Arguments:    str : string
                len : max length

  Returns:      string length
 *---------------------------------------------------------------------------*/
int STD_GetStringNLength(const char *str, int len)
{
#if defined(STRNLEN_4BYTE_SKIP)
    int     n;
	unsigned long  *p32, y;

	for( n=0; (((unsigned long)(str + n) & 3) != 0) && (n < len); n++ )
	{
		if(!str[n])
			return n;
	}

	p32 = (unsigned long*)(str + n);
	for( ; n < len - 4; n+=4 )
    {
		y = (*p32 & 0x7F7F7F7F) + 0x7F7F7F7F;
		y = ~(y | *p32 | 0x7F7F7F7F);

		if( y != 0 )
			break;

		p32++;
	}

    while( str[n] && (n < len) )
    {
        n++;
    }

	return n;

#else    // original
	int    n = 0;

    while( str[n] && (n < len) )
    {
        n++;
    }

	return n;
#endif
}


/*---------------------------------------------------------------------------*
  Name:         STD_ConcatenateString

  Description:  concatenate strings. same to strcat

  Arguments:    str1 : original string
                str2 : string to concatenate

  Returns:      concatenated string
 *---------------------------------------------------------------------------*/
char   *STD_ConcatenateString(char *str1, const char *str2)
{
    int     n = STD_GetStringLength(str1);
    (void)STD_CopyString(&str1[n], str2);
    return str1;
}

/*---------------------------------------------------------------------------*
  Name:         STD_ConcatenateLString

  Description:  concatenate strings. same to strlcat

  Arguments:    str1 : original string
                str2 : string to concatenate
                size : buffer size of str1

  Returns:      length of str1 + length of str2
 *---------------------------------------------------------------------------*/
int STD_ConcatenateLString(char *str1, const char *str2, int size)
{
    int n;
    int str2_len;

    // str1 �� size �ȓ��� NUL �I�[����Ă��Ȃ��ꍇ size + strlen(str2) ��Ԃ��Ȃ���΂Ȃ�Ȃ�
    for( n = 0; n < size; ++n )
    {
        if( str1[n] == '\0' )
        {
            break;
        }
    }

    if( n >= size )
    {
        return n + STD_GetStringLength(str2);
    }

    str2_len = STD_CopyLString(&str1[n], str2, size - n);

    return n + str2_len;
}

/*---------------------------------------------------------------------------*
  Name:         STD_CompareString

  Description:  compare strings. same to strcmp

  Arguments:    str1, str2 : strings

  Returns:      0 if same
 *---------------------------------------------------------------------------*/
int STD_CompareString(const char *str1, const char *str2)
{
#if defined(STRCMP_4BYTE)
	unsigned long  dstalign, srcalign;
	unsigned char  chr1, chr2;
	unsigned long  lng1, lng2, y;
    int            i;

	// 4byte�A���C���`�F�b�N
	dstalign = (unsigned long)str1 & 0x3;
	srcalign = (unsigned long)str2 & 0x3;

	i = 0;
	// �����Ă��Ȃ���Ε��ʂɔ�r
	if( dstalign ^ srcalign )
	{
		goto strcmp_normal;
	}

	// �擪4byte��r
	if( dstalign )
	{
		for( ; i + dstalign < 4; i++ )
		{
			chr1 = (unsigned char)*(str1 + i);
			chr2 = (unsigned char)*(str2 + i);
			if(chr1 != chr2 || !chr1)
			{
				return (int)(chr1 - chr2);
			}
		}
	}
	// 4byte��r
	for( ; ; i += 4 )
	{
		lng1 = *(unsigned long*)(str1 + i);
		lng2 = *(unsigned long*)(str2 + i);

		if( lng1 != lng2 )
			break;

		y = (lng1 & 0x7F7F7F7F) + 0x7F7F7F7F;
		y = ~(y | lng1 | 0x7F7F7F7F);

		if( y != 0 )
			break;
	}

strcmp_normal:
	// �ʏ��r
	for(;;i++)
    {
		chr1 = (unsigned char)*(str1 + i);
		chr2 = (unsigned char)*(str2 + i);
		if(chr1 != chr2 || !chr1)
		{
			return (int)(chr1 - chr2);
		}
	}

	// �����ɓ��B���邱�Ƃ͂Ȃ�
	return -1;

#else  // original
    while (*str1 == *str2 && *str1)
    {
        str1++;
        str2++;
    }
    return (*str1 - *str2);
#endif
}

/*---------------------------------------------------------------------------*
  Name:         STD_CompareNString

  Description:  same to strncmp

  Arguments:    str1, str2 : strings
                len        : max length

  Returns:      0 if same
 *---------------------------------------------------------------------------*/
int STD_CompareNString(const char *str1, const char *str2, int len)
{
#if defined(STRNCMP_4BYTE)
	unsigned long  dstalign, srcalign;
	unsigned char  chr1, chr2;
	unsigned long  lng1, lng2;
	unsigned long  y;
    int            i;

	if( len == 0 )
		goto end;

	// 4byte�A���C���`�F�b�N
	dstalign = (unsigned long)str1 & 0x3;
	srcalign = (unsigned long)str2 & 0x3;

	i = 0;
	// �����Ă��Ȃ���Ε��ʂɔ�r
	if( dstalign ^ srcalign )
	{
		goto strncmp_normal;
	}

	// �擪4byte��r
	if( dstalign )
	{
		for( ; (i + dstalign < 4) && (i < len); i++ )
		{
			chr1 = (unsigned char)*(str1 + i);
			chr2 = (unsigned char)*(str2 + i);
			if(chr1 != chr2 || !chr1)
			{
				return (int)(chr1 - chr2);
			}
		}
	}
	// 4byte��r
	for( ; i <= len - 4; i += 4 )
	{
		lng1 = *(unsigned long*)(str1 + i);
		lng2 = *(unsigned long*)(str2 + i);

		// ��r�`�F�b�N
		if( lng1 != lng2 )
			break;

		// NULL �`�F�b�N
		y = (lng1 & 0x7F7F7F7F) + 0x7F7F7F7F;
		y = ~(y | lng1 | 0x7F7F7F7F);

		if( y != 0 )
			break;
	}

strncmp_normal:
	// �ʏ��r
    for (; i < len; ++i)
    {
		chr1 = (unsigned char)*(str1 + i);
		chr2 = (unsigned char)*(str2 + i);
		if(chr1 != chr2 || !chr1)
		{
			return (int)(chr1 - chr2);
		}
    }

end:
	// �߂�l
	return 0;
#else
    if (len)
    {
        int     i;
        for (i = 0; i < len; ++i)
        {
            int     c = (int)(MI_ReadByte(str1 + i));
            int     d = (int)(MI_ReadByte(str2 + i));
            if (c != d || !c)
            {
                return (int)(c - d);
            }
        }
    }
    return 0;
#endif
}

/*---------------------------------------------------------------------------*
  Name:         STD_CompareLString

  Description:  same to strlcmp

  Arguments:    str1, str2 : strings
                len        : max length

  Returns:      0 if same
 *---------------------------------------------------------------------------*/
int STD_CompareLString(const char *str1, const char *str2, int len)
{
	int     c, d;

	while(len-- && *str2 != '\0')
	{
        c = (int)(MI_ReadByte(str1));
        d = (int)(MI_ReadByte(str2));
		if( c != d )
		{
			return c - d;
		}
		str1++;
		str2++;
	}
	return (int)(MI_ReadByte(str1));
}

/*---------------------------------------------------------------------------*
  Name:         STD_CompareNString

  Description:  same to strncasecmp

  Arguments:    str1, str2 : strings
                len        : max length

  Returns:      0 if same
 *---------------------------------------------------------------------------*/
int STD_CompareNIString(const char *str1, const char *str2, int len)
{
#if defined(STRNICMP_DIFF)
	int   i;
	char  c, d, diff;
	int   retval = 0;
#define TO_UPPER(x)		( ((x >= 'a') && (x <= 'z')) ? (x - 0x20) : x )

	if( (str1 == str2) || (len == 0) )
	{
		return 0;
	}

	for( i = 0; ; i++ )
	{
		c = *(str1 + i);
		d = *(str2 + i);
		diff = (char)(c - d);

		if( diff == 0 )
		{
			goto check_end;
		}
		else
		{
			if( diff == 'a' - 'A' )
			{
				if( (d >= 'A') && (d <= 'Z') )
				{
					goto check_end;
				}
			}
			else if( diff == 'A' - 'a' )
			{
				if( (c >= 'A') && (c <= 'Z') )
				{
					goto check_end;
				}
			}
			retval = TO_UPPER(c) - TO_UPPER(d);
			break;
		}
check_end:
		if( c == '\0' )
			break;
		if( i >= len - 1 )
			break;
	}

	return retval;
#else
    int     retval = 0;
#define TO_UPPER(x)		( ((x >= 'a') && (x <= 'z')) ? (x - 0x20) : x )

	if( (str1 == str2) || (len == 0) )
	{
		return 0;
	}

	while( !(retval = TO_UPPER(*str1) - TO_UPPER(*str2)) )
	{
		if( (*str1 == '\0') || (--len == 0) )
		{
			break;
		}
		str1++;
		str2++;
	}

    return retval;
#endif
}

/*---------------------------------------------------------------------------*
  Name:         STD_TSScanf

  Description:  �ȃT�C�Y��ړI�Ƃ��� sscanf.
                ��{�I�ȏ����w�� "%(*?)([lh]{,2})([diouxXpn])" �ɑΉ�.

  Arguments:    src           ���͕�����
                fmt           �������䕶����

  Returns:      ������ꂽ�l�̑���.
                ������ł̏I�[���邢�͕s�������o����� -1.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL int STD_TSScanf(const char *src, const char *fmt, ...)
{
    int     ret;
    va_list va;
    va_start(va, fmt);
    ret = STD_TVSScanf(src, fmt, va);
    va_end(va);
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         STDi_IsSpace

  Description:  �w�蕶�����󔒗ޕ���������.

  Arguments:    c             ���肷�镶��

  Returns:      �󔒗ޕ����ł���� TRUE.
 *---------------------------------------------------------------------------*/
static BOOL STDi_IsSpace(int c)
{
    return (c == '\f') || (c == '\n') || (c == '\r') || (c == '\t') || (c == '\v') || (c == ' ');
}

/*---------------------------------------------------------------------------*
  Name:         STDi_GetBitset

  Description:  �r�b�g�Z�b�g�̎w��ʒu�̓��e���擾.

  Arguments:    bitet         �r�b�g�Z�b�g�z��
                i             �����擾����ʒu

  Returns:      ON �Ȃ� TRUE, OFF �Ȃ� FALSE.
 *---------------------------------------------------------------------------*/
inline static u32 STDi_GetBitset(const u32 *bitset, u32 i)
{
    const u32 pos = (u32)(i >> 5UL);
    const u32 bit = (u32)(i & 31UL);
    return ((bitset[pos] >> bit) & 1UL);
}

/*---------------------------------------------------------------------------*
  Name:         STDi_SetBitset

  Description:  �r�b�g�Z�b�g�̎w��ʒu��ON�ɐݒ�.

  Arguments:    bitet         �r�b�g�Z�b�g�z��
                i             ON�ɂ���ʒu

  Returns:      None.
 *---------------------------------------------------------------------------*/
inline static void STDi_SetBitset(u32 *bitset, u32 i)
{
    const u32 pos = (i >> 5UL);
    const u32 bit = (i & 31UL);
    bitset[pos] |= (1UL << bit);
}

/*---------------------------------------------------------------------------*
  Name:         STDi_FillBitset

  Description:  �r�b�g�Z�b�g�̎w��͈� [a,b) ��ON�ɐݒ�.

  Arguments:    bitet         �r�b�g�Z�b�g�z��
                a             �J�n�ʒu
                b             �I���ʒu

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void STDi_FillBitset(u32 *bitset, u32 a, u32 b)
{
    while (a < b)
    {
        const u32 pos = (u32)(a >> 5UL);
        const u32 bit = (u32)(a & 31UL);
        u32     mask = (~0UL << bit);
        a = (pos + 1UL) * 32UL;
        if (a > b)
        {
            mask &= (u32)((1UL << (b & 31UL)) - 1UL);
        }
        bitset[pos] |= mask;
    }
}

/*---------------------------------------------------------------------------*
  Name:         STD_TVSScanf

  Description:  STD_TSScanf �� va_list �Ή���.
                ��{�I�ȏ����w��ɑΉ�.

  Arguments:    src           ���͕�����
                fmt           �������䕶����
                vlist         �p�����[�^

  Returns:      ������ꂽ�l�̑���.
                ������ł̏I�[���邢�͕s�������o����� -1.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL int STD_TVSScanf(const char *src, const char *fmt, va_list vlist)
{
    /* �ƍ�������̋N�_ (%n �̂��߂ɕK�v) */
    const char *src_base = src;
    /* �����Ɉ�x�ł��}�b�`�������ǂ���. (�Ԃ�l -1 �ɍ�p) */
    BOOL    matched = FALSE;
    /* ��������s���ꂽ��. (�Ԃ�l >= 0 �ɍ�p) */
    int     stored = 0;

    /* ����������� 1 ��������� */
    while (*fmt)
    {
        char    c = *fmt;
        /* �󔒕����Ȃ珑���E�ƍ��Ƃ��X�L�b�v */
        if (STDi_IsSpace(c))
        {
            while (STDi_IsSpace(*fmt))
            {
                ++fmt;
            }
            while (STDi_IsSpace(*src))
            {
                ++src;
            }
            continue;
        }
        /* �ʏ핶���Ȃ�Ό����Ɉ�v���Ȃ���΂Ȃ�Ȃ� */
        else if (c != '%')
        {
            if (c != *src)
            {
                break;
            }
            /* SHIFT-JIS�̏ꍇ�͌㑱�o�C�g������  */
            if ((unsigned int)(((unsigned char)c ^ 0x20) - 0xA1) < 0x3C)
            {
                if (c != *++src)
                {
                    break;
                }
            }
            ++src;
            ++fmt;
        }
        /* �G�X�P�[�v�ɂ�� '%' �̔���Ȃ�P�ɔ�r */
        else if (fmt[1] == '%')
        {
            if (c != *src)
            {
                break;
            }
            fmt += 2;
        }
        /* �ϊ��w��Ȃ�Ώ�����͊J�n */
        else
        {
            enum
            {
                flag_plus = 000002,    /* '+' */
                flag_minus = 000010,   /* '-' */
                flag_l1 = 000040,      /* "l" */
                flag_h1 = 000100,      /* "h" */
                flag_l2 = 000200,      /* "ll" */
                flag_h2 = 000400,      /* "hh" */
                flag_unsigned = 010000, /* 'o', 'u', ... */
                flag_ignored = 020000, /* '*' */
                flag_end
            };
            u64     val = 0;
            int     flag = 0, width = 0, radix = 10;
            int     digit;

            /* ����}�~�t���O */
            c = *++fmt;
            if (c == '*')
            {
                flag |= flag_ignored;
                c = *++fmt;
            }
            /* ���w�� */
            while ((c >= '0') && (c <= '9'))
            {
                width = (width * 10) + c - '0';
                c = *++fmt;
            }
            /* �ϊ��C�� */
            switch (c)
            {
            case 'h':
                c = *++fmt;
                if (c != 'h')
                {
                    flag |= flag_h1;
                }
                else
                    flag |= flag_h2, c = *++fmt;
                break;
            case 'l':
                c = *++fmt;
                if (c != 'l')
                    flag |= flag_l1;
                else
                    flag |= flag_l2, c = *++fmt;
                break;
            }
            /* �ϊ��^ */
            switch (c)
            {
            case 'd':                 /* �����t10�i�� */
                radix = 10;
                goto get_integer;
            case 'i':                 /* �����t8/10/16�i�� */
                radix = -1;
                goto get_integer;
            case 'o':                 /* �����t8�i�� */
                radix = 8;
                goto get_integer;
            case 'u':                 /* ������10�i�� */
                radix = 10;
                flag |= flag_unsigned;
                goto get_integer;
            case 'X':                 /* ������16�i�� */
            case 'x':                 /* ������16�i�� */
            case 'p':                 /* �|�C���^�ϊ�(������16�i��) */
                radix = 16;
                flag |= flag_unsigned;
                goto get_integer;
            case 's':                 /* ��󔒕����܂ł̕����� */
            case 'c':                 /* �w�蕝�݂̂̕����� */
                goto store_string;

            case '[':                 /* �����N���X */
                goto store_char_class;

            case 'n':                 /* �}�b�`�ʒu�̊i�[ */
                /* ����񐔂ɂ͊�^���Ȃ��̂ł����Ō����Ă��� */
                if (!(flag & flag_ignored))
                {
                    --stored;
                }
                val = src - src_base;
                c = *++fmt;
                goto store_integer;
            }

//        invalid:
            /* ��͂��c�O�Ȍ��ʂɏI����� */
            break;

          get_integer:
            /* �����̓��͏��� */
            ++fmt;
            c = *src;
            /* �󔒂̃X�L�b�v */
            while (STDi_IsSpace(c))
            {
                c = *++src;
            }
            /* �����̎擾 */
            for (;; c = *++src)
            {
                if (c == '+')
                {
                    flag |= flag_plus;
                }
                else if (c == '-')
                {
                    /* �������^�ł� '-' �͕s�� */
                    if ((flag & flag_unsigned) != 0)
                    {
                        /*
                         * ...�Ƃ݂Ȃ��̂��������͂�����,
                         * gcc �� CW �ł͂���𖳎����Ă���̂�
                         * ����ł� SDK ������ɒǐ�.
                         */
                        //goto invalid;
                    }
                    flag |= flag_minus;
                }
                else
                {
                    break;
                }
            }
            /* �����̎������� (%i) */
            if (radix == -1)
            {
                if (c != '0')
                {
                    radix = 10;
                }
                else if ((src[1] == 'x') || (src[1] == 'X'))
                {
                    radix = 16;
                }
                else
                {
                    radix = 8;
                }
            }
            /* 16�i���Ȃ� "^(0[xX])?" ���X�L�b�v */
            if ((radix == 16) && (c == '0') && ((src[1] == 'x') || (src[1] == 'X')))
            {
                src += 2;
                c = *src;
            }
            /* �����擾 */
            if (width == 0)
            {
                width = 0x7FFFFFFF;
            }
            for (digit = 0; digit < width; ++digit)
            {
                u32     d = (u32)(c - '0');
                if (d >= 10)
                {
                    d -= (u32)('a' - '0');
                    if (d < 6)
                    {
                        d += 10;
                    }
                    else
                    {
                        d -= (u32)('A' - 'a');
                        if (d < 6)
                        {
                            d += 10;
                        }
                    }
                }
                if (d >= radix)
                {
                    break;
                }
                c = *++src;
                val = val * radix + d;
            }
            /* �����͕͂s�� */
            if (digit == 0)
            {
                break;
            }
            /* �������� */
            if (flag & flag_minus)
            {
                val = (u64)(val * -1);
            }
            /* ���Ȃ��Ƃ��ϊ��͐������� */
            matched = TRUE;
          store_integer:
            /* ��� */
            if (!(flag & flag_ignored))
            {
                ++stored;
                if (flag & flag_h2)
                {
                    *va_arg(vlist, u8 *) = (u8)val;
                }
                else if (flag & flag_h1)
                {
                    *va_arg(vlist, u16 *) = (u16)val;
                }
                else if (flag & flag_l2)
                {
                    *va_arg(vlist, u64 *) = (u64)val;
                }
                else
                {
                    *va_arg(vlist, u32 *) = (u32)val;
                }
            }
            continue;

          store_string:
            /* ������̑������ */
            {
                char   *dst = NULL, *dst_bak = NULL;
                ++fmt;
                if (!(flag & flag_ignored))
                {
                    dst = va_arg(vlist, char *);
                    dst_bak = dst;
                }
                /*
                 * s �Ȃ�󔒂܂Ō������� '\0' ��ǉ�
                 * c �Ȃ�w�蕝�܂Ō���
                 */
                if (c == 's')
                {
                    if (width == 0)
                    {
                        width = 0x7FFFFFFF;
                    }
                    for (c = *src; STDi_IsSpace(c); c = *++src)
                    {
                    }
                    for (; c && !STDi_IsSpace(c) && (width > 0); --width, c = *++src)
                    {
                        if (dst)
                        {
                            *dst++ = c;
                        }
                    }
                    /*
                     * dst != dst_bak �Ȃ�Έꕶ���ȏ�������Ă���Ƃ݂Ȃ�
                     * ������NULL��t���Amatched = TRUE, �����������++
                     */
                    if (dst != dst_bak)
                    {
                        *dst++ = '\0';
                        ++stored;
                        matched = TRUE;
                    }
                }
                else
                {
                    if (width == 0)
                    {
                        width = 1;
                    }
                    for (c = *src; c && (width > 0); --width, c = *++src)
                    {
                        if (dst)
                        {
                            *dst++ = c;
                        }
                    }
                    /*
                     * dst != dst_bak �Ȃ�Έꕶ���ȏ�������Ă���Ƃ݂Ȃ�
                     * ������NULL��t���Amatched = TRUE, �����������++
                     */
                    if (dst != dst_bak)
                    {
                        ++stored;
                        matched = TRUE;
                        if(width > 0)
                        {
                            *dst++ = '\0';
                        }
                    }
                }
            }
            continue;

          store_char_class:
            ++fmt;
            /* �����N���X�̑������ */
            {
                char   *dst = NULL;

                u32     bitset[256 / (8 * sizeof(u32))];
                u32     matchcond = 1;
                u32     from = 0;
                BOOL    in_range = FALSE;
                MI_CpuFill32(bitset, 0x00000000UL, sizeof(bitset));
                if (*fmt == '^')
                {
                    matchcond = 0;
                    ++fmt;
                }
                /* �s���G�X�P�[�v */
                if (*fmt == ']')
                {
                    STDi_SetBitset(bitset, (u8)*fmt);
                    ++fmt;
                }
                /* �����N���X�̉�� */
                for (;; ++fmt)
                {
                    /* �I�[�̌��o */
                    if (!*fmt || (*fmt == ']'))
                    {
                        /* �͈͎w�蒆�̏I�[�͒P�̂̕����Ƃ��Ĉ��� */
                        if (in_range)
                        {
                            STDi_SetBitset(bitset, from);
                            STDi_SetBitset(bitset, (u32)'-');
                        }
                        if (*fmt == ']')
                        {
                            ++fmt;
                        }
                        break;
                    }
                    /* �P�������邢�͔͈͎w��̎n�[ */
                    else if (!in_range)
                    {
                        /* �͈͎w��̊J�n */
                        if ((from != 0) && (*fmt == '-'))
                        {
                            in_range = TRUE;
                        }
                        else
                        {
                            STDi_SetBitset(bitset, (u8)*fmt);
                            from = (u8)*fmt;
                        }
                    }
                    /* �͈͎w��̏I�[ */
                    else
                    {
                        u32     to = (u8)*fmt;
                        /* �s���Ȕ͈͎w��͕ʌ̕����w��Ƃ��Ĉ��� */
                        if (from > to)
                        {
                            STDi_SetBitset(bitset, from);
                            STDi_SetBitset(bitset, (u32)'-');
                            STDi_SetBitset(bitset, to);
                        }
                        /* �I�[�̕������͈͂Ɋ܂�ł܂Ƃ߂Đݒ� */
                        else
                        {
                            STDi_FillBitset(bitset, from, to + 1UL);
                        }
                        in_range = FALSE;
                        from = 0;
                    }
                }
                /* �����N���X�ƕ�����̏ƍ� */
                /* ���̎��_�ŏ��Ȃ��Ƃ��ϊ��͐������� */
                matched = TRUE;
                if (!(flag & flag_ignored))
                {
                    ++stored;
                    dst = va_arg(vlist, char *);
                }
                if (width == 0)
                {
                    width = 0x7FFFFFFF;
                }
                for (c = *src; c && (width > 0); --width, c = *++src)
                {
                    if (STDi_GetBitset(bitset, (u8)c) != matchcond)
                    {
                        break;
                    }
                    if (dst)
                    {
                        *dst++ = c;
                    }
                }
                if (dst)
                {
                    *dst++ = '\0';
                }
            }
            continue;

        }
    }

    return (*src || matched) ? stored : -1;

}
