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
  
  Description:  defval メイン関数
                オプション処理して本処理関数を呼び出す
  
  Arguments:    argc   引数の数
                argv   引数
  
  Returns:      0      正常終了
                0以外  エラー
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int     n, fargc;
    char  **fargv;
    BOOL    result;

    //
    // オプション解析
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
            isComment = TRUE;          // 行頭 # のコメントを有効にする
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
    // 処理開始
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

    UnpackFileName(argv0, NULL, &appname, NULL);        // コマンド名を取得

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
  
  Description:  defval メイン関数
                オプション処理して本処理関数を呼び出す
  
  Arguments:    file_in  入力ファイル
                file_out 出力ファイル
  
  Returns:      TRUE   正常終了
                FALSE  エラー
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

        if (isComment && buffer[0] == '#')      // コメント処理
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
