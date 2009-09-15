/*---------------------------------------------------------------------------*
  Project:  TwlSDK - tools - defval
  File:     defval.c

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-05-23#$
  $Rev: 6187 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#include        "defval.h"

extern const unsigned long SDK_DATE_OF_LATEST_FILE;

static void usage(const char *argv0);
static BOOL defval(const char *file_in, const char *file_out);
static BOOL isComment = FALSE;

/*---------------------------------------------------------------------------*
  Name:         main
  
  Description:  defval ���C���֐�
                �I�v�V�����������Ė{�����֐����Ăяo��
  
  Arguments:    argc   �����̐�
                argv   ����
  
  Returns:      0      ����I��
                0�ȊO  �G���[
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int     n, fargc;
    char  **fargv;
    BOOL    result;

    //
    // �I�v�V�������
    //
    while ((n = getopt(argc, argv, "D:M:Cdh")) != -1)
    {
        switch (n)
        {
        case 'D':
            (void)add_dvalue_by_equality(optarg);
            break;

        case 'M':
            if (!add_dvalue_from_file(optarg))
            {
                fprintf(stderr, "Stop.\n");
                return 1;
            }
            break;

        case 'C':
            isComment = TRUE;          // �s�� # �̃R�����g��L���ɂ���
            break;

        case 'd':
            SetDebugMode(TRUE);
            break;

        case 'h':
        case '?':
        case ':':
        default:
            usage(argv[0]);
        }
    }

    //
    // �����J�n
    //
    fargc = argc - optind;
    fargv = argv + optind;

    switch (fargc)
    {
    case 0:
        result = defval("-", "-");
        break;

    case 1:
        result = defval(fargv[0], "-");
        break;

    case 2:
    default:
        result = defval(fargv[0], fargv[1]);
        break;
    }
    return result ? 0 : 1;
}

static void usage(const char *argv0)
{
    char   *appname;

    UnpackFileName(argv0, NULL, &appname, NULL);        // �R�}���h�����擾

    fprintf(stderr,
            "TWL-SDK Development Tool - %s - filter to support user defines\n"
            "Build %lu\n\n"
            "Usage: %s [-DNAME=VALUE...] [-MDEFINES_FILE] [-C] INFILE OUTFILE\n\n",
            appname, SDK_DATE_OF_LATEST_FILE, appname);

    free(appname);
    exit(2);
}

/*---------------------------------------------------------------------------*
  Name:         main
  
  Description:  defval ���C���֐�
                �I�v�V�����������Ė{�����֐����Ăяo��
  
  Arguments:    file_in  ���̓t�@�C��
                file_out �o�̓t�@�C��
  
  Returns:      TRUE   ����I��
                FALSE  �G���[
 *---------------------------------------------------------------------------*/
static BOOL defval(const char *file_in, const char *file_out)
{
    BOOL    result;
    FILE   *fp_in;
    FILE   *fp_out;
    char   *buffer;
    int     buffer_size;

    DebugPrintf("file_in=[%s] file_out=[%s]\n", file_in, file_out);

    result = FALSE;
    buffer = NULL;

    if (NULL == (fp_in = Fopen(file_in, "rb")) || NULL == (fp_out = Fopen(file_out, "wb")))
    {
        goto escape;
    }

    while (NULL != Fgets(&buffer, &buffer_size, fp_in))
    {
        DebugPrintf("buffer=[%s] buffer_size=[%d]\n", buffer, buffer_size);

        if (isComment && buffer[0] == '#')      // �R�����g����
        {
            continue;
        }

        if (!puts_modified_dvalue(fp_out, buffer))
        {
            fprintf(stderr, "Error: Cannot write %s.\n", file_out);
            goto escape;
        }
    }
    result = TRUE;

  escape:
    Free((void **)&buffer);
    Fclose(fp_in);
    Fclose(fp_out);

    return result;
}
