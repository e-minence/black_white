/*---------------------------------------------------------------------------*
  Project:  TWL-System - libraries - g2d
  File:     g2d_TextCanvas.c

  Copyright 2004-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1155 $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <string.h>

#undef NNS_G2D_UNICODE
#include <nnsys/g2d/g2d_TextCanvas.h>






//****************************************************************************
// global functions
//****************************************************************************

//----------------------------------------------------------------------------
// 描画
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawString

  Description:  最初の改行文字もしくは終端文字までを描画します。

  Arguments:    pTxn:   描画対象の TextCanvas へのポインタ。
                x:      描画開始位置の x 座標。
                y:      描画開始位置の y 座標。
                cl:     文字色の色番号。
                str:    描画する文字列。
                pPos:   改行文字まで描画した場合は改行文字の次の文字への
                        ポインタを、終端文字まで描画した場合はNULLを格納する
                        バッファへのポインタ。
                        不要な場合はNULLを指定することができます。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawString(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int cl,
    const void* str,
    const void** pPos
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    , NNSiG2dTextDirection d
#endif
)
{
    const void* pos;        // 描画対象の文字へのポインタ
    int charSpace;          // 文字間隔 (1文字目の文字の右端と2文字目の文字の左端の間隔)
    const NNSG2dFont* pFont;
    u16 c;
    NNSiG2dSplitCharCallback getNextChar;

    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_POINTER_ASSERT( str );
    NNS_G2D_ASSERT( NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))
                    <= NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp );
    NNS_G2D_MINMAX_ASSERT( cl, 0,
        (1 << NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp)
        - (1 << NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))) + 1 );

    charSpace   = pTxn->hSpace;
    pFont       = pTxn->pFont;
    pos         = str;
    getNextChar = NNSi_G2dFontGetSpliter(pFont);


    while( (c = getNextChar((const void**)&pos)) != 0 )
    {
        if( c == '\n' )
        {
            break;
        }

        // 1文字描画
        {
            const int w = NNS_G2dCharCanvasDrawChar(pTxn->pCanvas, pFont, x, y, cl, c) + charSpace;
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
            x += w * d.x;
            y += w * d.y;
#else
            x += w;
#endif
        }
    }

    if( pPos != NULL )
    {
        *pPos = (c == '\n') ? pos: NULL;
    }
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dTextCanvasDrawTextAlign

  Description:  areaWidth の幅内で左右中寄せをして描画します。

  Arguments:    pTxn:       描画対象の TextCanvas へのポインタ。
                x:          描画開始位置の x 座標。
                y:          描画開始位置の y 座標。
                cl:         文字色の色番号。
                flags:      描画位置を指定するフラグ。
                txt:        描画する文字列。
                areaWidth:  右寄せ、中寄せの際に参照される描画領域幅。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawTextAlign(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int areaWidth,
    int cl,
    u32 flags,
    const void* txt
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    , NNSiG2dTextDirection d
#endif
)
{
    const void* str;    // 描画対象の文字列へのポインタ
    int linefeed;       // 行送り（1行目の行の上端と2行目の行の上端の間隔）
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    int linefeedx;
    int linefeedy;
    int line;
#endif
    int charSpace;      // 文字間隔 (1文字目の文字の右端と2文字目の文字の左端の間隔)
    const NNSG2dFont* pFont;
    int px, py;

    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_POINTER_ASSERT( txt );
    NNS_G2D_ASSERT( NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))
                    <= NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp );
    NNS_G2D_MINMAX_ASSERT( cl, 0,
        (1 << NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp)
        - (1 << NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))) + 1 );

    charSpace = pTxn->hSpace;
    linefeed = NNS_G2dFontGetLineFeed(pTxn->pFont) + pTxn->vSpace;
    pFont = pTxn->pFont;
    str = txt;
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    linefeedx = linefeed * -d.y;
    linefeedy = linefeed * d.x;
    line = 0;
#else
    py = y;
#endif


    while( str != NULL )
    {
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
        px = x + line * linefeedx;
        py = y + line * linefeedy;
#else
        px = x;
#endif

        if( flags & NNS_G2D_HORIZONTALALIGN_RIGHT )
        {
            const int width = NNS_G2dTextCanvasGetStringWidth(pTxn, str, NULL);
            const int offset = areaWidth - width;
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
            px += offset * d.x;
            py += offset * d.y;
#else
            py += offset;
#endif
        }
        else if( flags & NNS_G2D_HORIZONTALALIGN_CENTER )
        {
            const int width = NNS_G2dTextCanvasGetStringWidth(pTxn, str, NULL);
            const int offset = (areaWidth + 1) / 2 - (width + 1) / 2;
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
            px += offset * d.x;
            py += offset * d.y;
#else
            py += offset;
#endif
        }

#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
        NNSi_G2dTextCanvasDrawString(pTxn, px, py, cl, str, &str, d);
        line++;
#else
        NNSi_G2dTextCanvasDrawString(pTxn, px, py, cl, str, &str);
        py += linefeed;
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawText

  Description:  (x,y) に対して flags で指定される配置で文字列を描画します。

  Arguments:    pTxn:   TextCanvasへのポインタ。
                x:      描画基準座標。
                y:      描画基準座標。
                cl:     文字の描画色番号。
                flags:  配置指定フラグ。
                txt:    描画する文字列。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawText(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int cl,
    u32 flags,
    const void* txt
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    , NNSiG2dTextDirection d
#endif
)
{
    NNSG2dTextRect area;

    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_POINTER_ASSERT( txt );
    NNS_G2D_ASSERT( NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))
                    <= NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp );
    NNS_G2D_MINMAX_ASSERT( cl, 0,
        (1 << NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp)
        - (1 << NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))) + 1 );

    // 左上座標を求める
    {
        area = NNS_G2dTextCanvasGetTextRect(pTxn, txt);

        if( flags & NNS_G2D_HORIZONTALORIGIN_CENTER )
        {
            const int offset = - (area.width + 1) / 2;
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
            x += offset * d.x;
            y += offset * d.y;
#else
            x += offset;
#endif
        }
        else if( flags & NNS_G2D_HORIZONTALORIGIN_RIGHT )
        {
            const int offset = - area.width;
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
            x += offset * d.x;
            y += offset * d.y;
#else
            x += offset;
#endif
        }

        if( flags & NNS_G2D_VERTICALORIGIN_MIDDLE )
        {
            const int offset = - (area.height + 1) / 2;
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
            x += offset * -d.y;
            y += offset * d.x;
#else
            y += offset;
#endif
        }
        else if( flags & NNS_G2D_VERTICALORIGIN_BOTTOM )
        {
            const int offset = - area.height;
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
            x += offset * -d.y;
            y += offset * d.x;
#else
            y += offset;
#endif
        }
    }

#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    NNSi_G2dTextCanvasDrawTextAlign(pTxn, x, y, area.width, cl, flags, txt, d);
#else
    NNSi_G2dTextCanvasDrawTextAlign(pTxn, x, y, area.width, cl, flags, txt);
#endif
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawTextRect

  Description:  矩形を基準に文字列を配置して描画します。
                矩形内に収まるように描画するわけではありません。

  Arguments:    pTxn:   TextCanvasへのポインタ。
                x:      矩形左上座標。
                y:      矩形左上座標。
                cl:     文字の描画色番号。
                flags:  配置指定フラグ。
                txt:    描画する文字列。
                w:      矩形幅。
                h:      矩形高さ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawTextRect(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int w,
    int h,
    int cl,
    u32 flags,
    const void* txt
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    , NNSiG2dTextDirection d
#endif
)
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_POINTER_ASSERT( txt );
    NNS_G2D_ASSERT( NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))
                    <= NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp );
    NNS_G2D_MINMAX_ASSERT( cl, 0,
        (1 << NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp)
        - (1 << NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))) + 1 );

    // y 座標をフラグに応じて補正
    {
        if( flags & NNS_G2D_VERTICALALIGN_BOTTOM )
        {
            const int height = NNS_G2dTextCanvasGetTextHeight(pTxn, txt);
            const int offset = h - height;
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
            x += offset * -d.y;
            y += offset * d.x;
#else
            y += offset;
#endif
        }
        else if( flags & NNS_G2D_VERTICALALIGN_MIDDLE )
        {
            const int height = NNS_G2dTextCanvasGetTextHeight(pTxn, txt);
            const int offset = (h + 1)/2 - (height + 1) / 2;
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
            x += offset * -d.y;
            y += offset * d.x;
#else
            y += offset;
#endif
        }
    }

#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    NNSi_G2dTextCanvasDrawTextAlign(pTxn, x, y, w, cl, flags, txt, d);
#else
    NNSi_G2dTextCanvasDrawTextAlign(pTxn, x, y, w, cl, flags, txt);
#endif
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawTaggedText

  Description:  タグが埋め込まれた複数行の文字列を描画します。
                制御文字(0x00-0x09,0x0B-0x1F) を文字列中で
                発見するとコールバックを呼び出します。
                タグの処理は全てコールバックに任されています。

  Arguments:    pTxn:       描画対象の TextCanvas へのポインタ。
                x:          描画基準位置の x 座標。
                y:          描画基準位置の y 座標。
                cl:         文字色の色番号。
                str:        描画する文字列。
                cbFunc:     タグによって呼ばれるコールバック関数。
                cbParam:    コールバック関数に渡されるユーザデータ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawTaggedText(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int cl,
    const void* txt,
    NNSG2dTagCallback cbFunc,
    void* cbParam
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    , NNSiG2dTextDirection d
#endif
)
{
    const void* pos;    // 描画対象の文字へのポインタ
    int linefeed;       // 行送り（1行目の行の上端と2行目の行の上端の間隔）
    int charSpace;      // 文字間隔 (1文字目の文字の右端と2文字目の文字の左端の間隔)
    const NNSG2dFont* pFont;
    NNSG2dTagCallbackInfo cbInfo;
    u16 c;
    NNSiG2dSplitCharCallback getNextChar;

    int px = x;
    int py = y;

    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_FONT_ASSERT( pTxn->pFont );
    NNS_G2D_POINTER_ASSERT( txt );
    NNS_G2D_ASSERT( NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))
                    <= NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp );
    NNS_G2D_MINMAX_ASSERT( cl, 0,
        (1 << NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp)
        - (1 << NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))) + 1 );
    NNS_G2D_POINTER_ASSERT( cbFunc );

    cbInfo.txn      = *pTxn;
    cbInfo.cbParam  = cbParam;

    charSpace   = cbInfo.txn.hSpace;
    pFont       = cbInfo.txn.pFont;
    linefeed    = NNS_G2dFontGetLineFeed(pFont) + cbInfo.txn.vSpace;
    pos         = txt;
    getNextChar = NNSi_G2dFontGetSpliter(pFont);

#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    linefeed   *= (d.x != 0) ? d.x: -d.y;
#endif

    while( (c = getNextChar((const void**)&pos)) != 0 )
    {
        if( c < ' ' )
        {
            if( c == '\n' )
                // 改行の処理
            {
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
                if( d.x == 0 )
                {
                    px += linefeed;
                    py = y;
                }
                else
#endif
                {
                    px = x;
                    py += linefeed;
                }
            }
            else
                // コールバック呼び出し
            {
                cbInfo.str = (const NNSG2dChar*)pos;
                cbInfo.x = px;
                cbInfo.y = py;
                cbInfo.clr = cl;

                cbFunc(c, &cbInfo);
                NNS_G2D_TEXTCANVAS_ASSERT( &(cbInfo.txn) );
                NNS_G2D_FONT_ASSERT( cbInfo.txn.pFont );
                NNS_G2D_POINTER_ASSERT( cbInfo.str );
                NNS_G2D_MINMAX_ASSERT( cl, 0, 255 );

                pos = (const void*)cbInfo.str;
                px = cbInfo.x;
                py = cbInfo.y;
                cl = cbInfo.clr;

                pFont       = cbInfo.txn.pFont;
                charSpace   = cbInfo.txn.hSpace;
                linefeed    = NNS_G2dFontGetLineFeed(pFont) + cbInfo.txn.vSpace;
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
                linefeed   *= (d.x != 0) ? d.x: -d.y;
#endif
            }

            continue;
        }
        else
            // 1文字描画
        {
            const int w = NNS_G2dCharCanvasDrawChar(cbInfo.txn.pCanvas, cbInfo.txn.pFont, px, py, cl, c) + charSpace;
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
            px += w * d.x;
            py += w * d.y;
#else
            px += w;
#endif
        }
    }
}

//--------------------------------------------------------------------
