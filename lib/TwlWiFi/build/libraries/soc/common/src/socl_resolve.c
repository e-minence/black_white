/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - SOC - libraries
  File:     socl_resolve.c

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
#include <nitroWiFi/socl.h>

#define CPS_RCVBUF          SOCL_TCP_SOCKET_CPS_RCVBUF_SIZE
#define CPS_SNDBUF          SOCL_TCP_SOCKET_CPS_SNDBUF_SIZE
#define CPS_RCVBUF_ALIGNED  ((CPS_RCVBUF + 3) &~3)
#define CPS_SNDBUF_ALIGNED  ((CPS_SNDBUF + 3) &~3)

/*---------------------------------------------------------------------------*
  Name:         SOCL_Resolve

  Description:  DNS サーバに IP アドレスを問い合わせる．
  
  Arguments:    hostname        ホスト名
  
  Returns:      IP アドレス  0 ならエラー
 *---------------------------------------------------------------------------*/
SOCLInAddr SOCL_Resolve(const char* hostname)
{
    u8*         buffers;
    CPSSoc      soc;
    SOCLInAddr  hostip;

    if (!SOCLi_IsInitialized())
    {
        return 0;
    }
    
    if (!hostname)
    {
        return 0;
    }

    buffers = SOCLi_Alloc(CPS_RCVBUF_ALIGNED + CPS_SNDBUF_ALIGNED);

    if (buffers == NULL)
    {
        return 0;
    }

    MI_CpuClear8(&soc, sizeof(CPSSoc)); // 一旦 0 クリアする
    soc.rcvbuf.data = buffers;
    soc.rcvbuf.size = CPS_RCVBUF;
    soc.sndbuf.data = buffers + CPS_RCVBUF_ALIGNED;
    soc.sndbuf.size = CPS_SNDBUF;

    CPS_SocRegister(&soc);
    hostip = CPS_Resolve((char*)hostname);
    CPS_SocUnRegister();

    SOCLi_Free(buffers);

    return hostip;
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_InetAtoH

  Description:  文字表記された IP アドレスを数字に変換する
  
  Arguments:    hostname        文字表記された IP アドレス
  
  Returns:      IP アドレス  0 ならエラー
 *---------------------------------------------------------------------------*/
SOCLInAddr SOCL_InetAtoH(const char* hostname)
{
    SOCLInAddr  hostip;
    OSIntrMode  enable;
    CPSInAddr   dns0, dns1;

    // DNS サーバ引きを一時的に停止させ、文字から数値への変換を行なう．
    // コードサイズの削減のため、CPS 内のルーチンを使用する．
    // 本来は CPS 内部の変換ルーチンを直接呼びだすようにするべき
    enable = OS_DisableInterrupts();
    dns0 = CPSDnsIp[0];
    dns1 = CPSDnsIp[1];
    CPSDnsIp[0] = 0;
    CPSDnsIp[1] = 0;
    hostip = CPS_Resolve((char*)hostname);
    CPSDnsIp[0] = dns0;
    CPSDnsIp[1] = dns1;
    (void)OS_RestoreInterrupts(enable);

    return hostip;
}


/*---------------------------------------------------------------------------*
  Name:         SOCL_GetResolver/SOCL_SetResolver

  Description:  現在の DNS サーバの host ip アドレスを取得したり設定したりする
                まだネットワーク接続ができていななら負の値の SOCL_ENETRESET
                を返す．
 *---------------------------------------------------------------------------*/
int SOCL_GetResolver(SOCLInAddr* dns1, SOCLInAddr* dns2)
{
    SDK_ASSERT(dns1);
    SDK_ASSERT(dns2);

    if (SOCL_GetHostID() == 0)
    {
        return SOCL_ENETRESET;
    }

    *dns1 = (SOCLInAddr) CPSDnsIp[0];
    *dns2 = (SOCLInAddr) CPSDnsIp[1];
    return 0;
}

int SOCL_SetResolver(const SOCLInAddr dns1, const SOCLInAddr dns2)
{
    if (SOCL_GetHostID() == 0)
    {
        return SOCL_ENETRESET;
    }

    CPSDnsIp[0] = (CPSInAddr) dns1;
    if ( 0 == dns1 )
    {
        CPSDhcpMode &= ~CPS_DONOTUSE_DHCP_FOR_DNS;
        CPSDnsIp[1] = 0;
    }
    else
    {
        CPSDhcpMode |= CPS_DONOTUSE_DHCP_FOR_DNS;
        CPSDnsIp[1] = (CPSInAddr) dns2;
    }

    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_GetRemote

  Description:  ソケットの接続先を調べる
  
  Arguments:    s		ソケット
                port		リモートホストのポート番号
                ip		リモートホストの IP アドレス
  
  Returns:      正or 0: 成功
                負    : エラー
                  現状サポートしているエラー値は以下
                    - :  
                  エラー値は追加される場合があるため、他の負の値も全て汎用
                  エラーとして扱うこと．
 *---------------------------------------------------------------------------*/
int SOCL_GetRemote(int s, u16* port, SOCLInAddr* ip)
{
    SOCLSocket*     socket = (SOCLSocket*)s;

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;  // 初期化されていない
    }

    if (SOCL_SocketIsTCP(socket) && (!SOCL_SocketIsConnected(socket) || SOCL_SocketIsClosing(socket)))
    {
        return SOCL_ENOTCONN;   // 接続していない
    }

    *port = socket->cps_socket.remote_port;
    *ip = socket->cps_socket.remote_ip;

    return SOCL_ESUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_GetHostID

  Description:	自ホストの host ip アドレスを取得する
                まだ取得できていななら 0 となる．
  
  Arguments:    なし
  
  Returns:      ホストの IPv4 の IP アドレス(HostByteOrder)
 *---------------------------------------------------------------------------*/
SOCLInAddr SOCL_GetHostID(void)
{
    // リンクが確立していなくてかつ、
    // 割り込み中でなく、DHCP リクエスト最中ならスリープする
    if (CPSMyIp == 0)
    {
        if ((SOCLiDhcpState & (SOCL_DHCP_REQUEST | SOCL_DHCP_CALLBACK)) == SOCL_DHCP_REQUEST)
        {
            if (OS_GetProcMode() != OS_PROCMODE_IRQ)
            {
                OS_Sleep(10);   // 10ms 処理を明け渡す
            }
        }
    }
    else if (SOCLiRequestedIP == 0)
    {
        // IP アドレスが保存されていないなら保存しておく
        SOCLiRequestedIP = CPSMyIp;
    }

    return (volatile SOCLInAddr)CPSMyIp;
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_IsNetworkAvailable

  Description:  ネットワークの接続判定

  Arguments:    なし

  Returns:      接続しているなら TRUE
 *---------------------------------------------------------------------------*/
BOOL SOCL_IsNetworkAvailable(void)
{
    return CPSMyIp != 0 && SOCL_LinkIsOn();
}
