/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NHTTP - libraries
  File:     NHTTP_stdlib_NTR.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#include "nhttp.h"
#include "NHTTPlib_int.h"
#include "NHTTP_stdlib_NTR.h"

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_strncmp
  Description:  �W���֐���strncmp
  Arguments:
  Returns:
 *---------------------------------------------------------------------------*/
extern int NHTTPi_strncmp(const char* p1_p, const char* p2_p, int size)
{
    return STD_StrNCmp(p1_p, p2_p, size);
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_memcpy
  Description:  �W���֐���memcpy
  Arguments:
  Returns:
 *---------------------------------------------------------------------------*/
extern void NHTTPi_memcpy(void* dst_p, const void* src_p, int size)
{
    MI_CpuCopy8(src_p, dst_p, (u32)size);
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_strlen
  Description:  �W���֐���strlen
  Arguments:
  Returns:
 *---------------------------------------------------------------------------*/
extern int NHTTPi_strlen(const char *str_p)
{
    return STD_StrLen(str_p);
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_strnlen
  Description:  �W���֐���strlen
  Arguments:
  Returns:
 *---------------------------------------------------------------------------*/
extern int NHTTPi_strnlen(const char *str_p, int len)
{
    return STD_StrNLen(str_p, len);
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_strcmp   
  Description:  �W���֐���strcmp
  Arguments:
  Returns:
 *---------------------------------------------------------------------------*/
extern int NHTTPi_strcmp(const char *str1_p, const char *str2_p)
{
    return STD_StrCmp(str1_p, str2_p);
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_memclr
  Description:  �w��T�C�Y��0�Ŗ��߂�
  Arguments:
  Returns:
 *---------------------------------------------------------------------------*/
extern void NHTTPi_memclr(void* p_p, int size)
{
    MI_CpuClear8(p_p, (u32)size);
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_strnicmp  
  Description:  �啶��������������strncmp
  Arguments:
  Returns:
 *---------------------------------------------------------------------------*/
extern int NHTTPi_strnicmp(const char* p1_p, const char* p2_p, int size)
{
    char c1, c2;
    
    for (; size > 0; size--)
    {
        c1 = *p1_p++;
        c2 = *p2_p++;
        
        if ((!c1 || !c2) && (!c1 && !c2))
        {
            size = 0;
            break;
        }
        
        if (LOWERCASE(c1) != LOWERCASE(c2))
        {
            break;
        }
    }
    return size;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_getUrlEncodedSize
  Description:  �������x-www-form-urlencoded�ŃG���R�[�h�����Ƃ��̃T�C�Y�𓾂�
  Arguments:    p_p  
  Returns:      
 *---------------------------------------------------------------------------*/
extern int NHTTPi_getUrlEncodedSize(const char* p_p)
{
    int n, c;
    
    for (n = 0, c = *p_p++; c != 0; c = *p_p++)
    {
        if (((c >= '0') && (c <= '9')) ||
            ((c >= 'A') && (c <= 'Z')) ||
            ((c >= 'a') && (c <= 'z')) ||
            (c == ' '))
        {
            n += 1;
        }
        else
        {
            n += 3;		// %HH
        }
    }
    
    return n;
}
/*---------------------------------------------------------------------------*
  Name:         NHTTPi_getUrlEncodedSize
  Description:  �������x-www-form-urlencoded�ŃG���R�[�h�����Ƃ��̃T�C�Y�𓾂�
  Arguments:    i_p
  Returns:
 *---------------------------------------------------------------------------*/
extern int NHTTPi_getUrlEncodedSize2(const char* p_p, int len)
{
    int i = 0;
    int n, c;
    
    for (i = 0, n = 0, c = *p_p++; i < len; c = *p_p++, i++)
    {
        if (((c >= '0') && (c <= '9')) ||
            ((c >= 'A') && (c <= 'Z')) ||
            ((c >= 'a') && (c <= 'z')) ||
            (c == ' '))
        {
            n += 1;
        }
        else
        {
            n += 3;		// %HH
        }
    }
    
    return n;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_encodeUrlChar
  Description:  ������x-www-form-urlencoded�ŃG���R�[�h����
  Arguments:    p_p
                c
  Returns:      
 *---------------------------------------------------------------------------*/
extern int NHTTPi_encodeUrlChar(char* p_p, char c)
{
    int h, l;

    if (c == ' ')
    {
        p_p[0] = '+';
        return 1;
    }
    
    if (((c >= '0') && (c <= '9')) ||
        ((c >= 'A') && (c <= 'Z')) ||
        ((c >= 'a') && (c <= 'z')))
    {
        p_p[0] = c;
        return 1;
    }
    
    h = (c >> 4) & 0xf;
    l = c & 0xf;
    p_p[0] = '%';
    p_p[1] = (char)((h < 10) ? (h + '0') : (h - 10 + 'A'));
    p_p[2] = (char)((l < 10) ? (l + '0') : (l - 10 + 'A'));
    
    return 3;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_strToHex
  Description:  16�i������𐔒l�ɕϊ�����
                0 �` 7fffffff�܂�
  Arguments:    p_p
                len
  Returns:      
 *---------------------------------------------------------------------------*/
extern int NHTTPi_strToHex(const char* p_p, int len)
{
    int i, n, e;
    char c;
    
    if (len > 8)
    {
        return -1;
    }
    
    if ((len == 8) & (p_p[0] > '7'))
    {
        return -1;
    }

    n = 0;
    e = FALSE;

    for (i = 0; i < len; i++)
    {
        c = (char)LOWERCASE(p_p[i]);
        if ((c >= '0') && (c <= '9'))
        {
            n = (n << 4) + c - '0';
            e = TRUE;
        }
        else if ((c >= 'a') && (c <= 'f'))
        {
            n = (n << 4) + c - 'a' + 10;
            e = TRUE;
        }
        else if (e && ((c == ' ') || (c == 0)))
        {
            break;
        }
        else if (!e && (c == ' '))
        {
            continue;
        }
        else
        {
            return -1;
	}
    }
    
    return n;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_strToInt
  Description:  ��������10�i������𐔒l�ɕϊ�����
  Arguments:    p_p
                len
  Returns:      
 *---------------------------------------------------------------------------*/
extern int NHTTPi_strToInt(const char* p_p, int len)
{
    int i, n, e, prev;
    char c;
    
    if (len > 10)
        return -1;

    n = 0;
    e = FALSE;

    for (i = 0; i < len; i++)
    {
        c = p_p[i];
        if (e && ((c == ' ') || (c == 0)))
        { 
            break;
        }
        else if (!e && (c == ' '))
        { 
            continue;
        }
        else if (!((c >= '0' ) && (c <= '9')))
        { 
            return -1;
        }
        
        e = TRUE;
        prev = n;
        n = 10 * n + c - '0';
        if (prev > n)
            return -1;
    }
    return n;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_intToStr
  Description:  ���l�𕶎���ɕϊ�����
                �������NULL�I�[���Ȃ�
  Arguments:    p_p
                n
  Returns:      ������
 *---------------------------------------------------------------------------*/
extern int NHTTPi_intToStr(char* p_p, u32 n)
{
    int i, b;
    unsigned int x;
    const unsigned int rdx[9] = { 1000000000,
                                  100000000,
                                  10000000,
                                  1000000,
                                  100000,
                                  10000,
                                  1000,
                                  100,
                                  10 };
    int len = 0;

    b = FALSE;
    
    for (i = 0; i < 9; i++)
    {
        if (n >= rdx[i])
        {
            x = n / rdx[i];
            n -= x * rdx[i];
            b = TRUE;
            p_p[len++] = (char)(x + '0');
        }
        else if (b)
        {
            p_p[len++] = '0';
        }
    }
    p_p[len++] = (char)(n + '0');
    
    return len;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_compareToken
  Description:  �g�[�N���P�ʂŔ�r����
                �啶���Ə������̈Ⴂ�͖�������
  Arguments:    p1_p
                p2_p
  Returns:      0       equal
                != 0    not equal
 *---------------------------------------------------------------------------*/
extern int NHTTPi_compareToken(const char* p1_p, const char* p2_p)
{
    for (; LOWERCASE(*p1_p) == LOWERCASE(*p2_p);  p1_p++, p2_p++)
    {
        if ((*p1_p == 0) || (*p1_p == ' '))
        {
            return 0;
        }
    }
    
    return -1;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_strtonum
  Description:  �����������l�̕�����𕄍��������l�ɕϊ�����
                �r���̋󔒂͂��ׂĖ��������
  Arguments:    p_p
                size
  Returns:      
 *---------------------------------------------------------------------------*/
extern int NHTTPi_strtonum(const char* p_p, int size)
{
    int n, numFound;
    
    for (numFound=0, n=0; size--; p_p++)
    {
        if (*p_p == ' ')
        {
            continue;
        }
        
        if ((*p_p >= '0') & (*p_p <= '9'))
        {
            n = n * 10 + *p_p - '0';
            if (++numFound > 9)
            {
                return -1;
            }
	}
    }
    
    return numFound ? n : (-1);
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_memfind
  Description:  �w��̈悩�����̃p�^�[����T��
  Arguments:    p
                size
                pattern
                patternsize
  Returns:      0       found
                != 0    not found
 *---------------------------------------------------------------------------*/
extern int NHTTPi_memfind(const char* p_p, int size, const char* pattern_p, int patternsize)
{
    int i, j;
    char const* m;
    
    if (size < patternsize)
    {
        return -1;
    }
    
    for (i = 0;  i < size - patternsize + 1;  i++)
    {
        if (p_p[i] == pattern_p[0])
        {
            m = &p_p[i];
            
            for (j = 1; j < patternsize; j++)
            {
                if (m[j] != pattern_p[j])
                {
                    break;
                }
            }
            
            if (j == patternsize)
            {
                return 0;
            }
        }
    }
    
    return -1;
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_Base64Encode
  Description:  Base64�G���R�[�h
  Arguments:    dst_p   �ϊ���
                src_p   �ϊ��O
  Returns:      �ϊ���̕�����̒���
 *---------------------------------------------------------------------------*/
extern int NHTTPi_Base64Encode(char* dst_p, const char* src_p)
{
    char *tbl = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char *wp = dst_p;
    int i;
    int len = NHTTPi_strlen(src_p);
    
    for (i = 0; i < len; i += 3)
    {
        *wp++ = tbl[src_p[0] >> 2];
        *wp++ = tbl[((src_p[0] & 3) << 4) + (src_p[1] >> 4)];
        *wp++ = tbl[((src_p[1] & 0xf) << 2) + (src_p[2] >> 6)];
        *wp++ = tbl[src_p[2] & 0x3f];
        src_p += 3;
    }
    if (i == len+1)
    {
        *(wp - 1) = '=';
    }
    else if (i == len+2)
    {
        *(wp - 1) = *(wp - 2) = '=';
    }
    *wp = '\0';
    
    return NHTTPi_strlen(dst_p);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

