/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - SOC - include
  File:     so2soc.h

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
#ifndef NITROWIFI_SO2SOC_H_
#define NITROWIFI_SO2SOC_H_

#ifdef __cplusplus

extern "C" {
#endif

//---------------------------------------------------------------------------
// defines
//---------------------------------------------------------------------------
#define SO_VENDOR_NINTENDO  SOC_VENDOR_NINTENDO
#define SO_VERSION          SOC_VERSION

#define SO_PF_UNSPEC        SOC_PF_UNSPEC
#define SO_PF_INET          SOC_PF_INET
#define IP_INET             SOC_PF_INET

#define SO_AF_UNSPEC        SOC_AF_UNSPEC
#define SO_AF_INET          SOC_AF_INET

#define SO_INET_ADDRSTRLEN  SOC_INET_ADDRSTRLEN

#define SO_SOCK_STREAM      SOC_SOCK_STREAM
#define SO_SOCK_DGRAM       SOC_SOCK_DGRAM

#define SO_MSG_OOB          SOC_MSG_OOB
#define SO_MSG_PEEK         SOC_MSG_PEEK
#define SO_MSG_DONTWAIT     SOC_MSG_DONTWAIT

// Socket option levels
#define SO_SOL_SOCKET               SOC_SOL_SOCKET
#define SO_SOL_IP                   SOC_SOL_IP
#define SO_SOL_ICMP                 SOC_SOL_ICMP
#define SO_SOL_TCP                  SOC_SOL_TCP
#define SO_SOL_UDP                  SOC_SOL_UDP

#define SO_IPPROTO_ICMP             SOC_IPPROTO_ICMP
#define SO_IPPROTO_IGMP             SOC_IPPROTO_IGMP
#define SO_IPPROTO_TCP              SOC_IPPROTO_TCP
#define SO_IPPROTO_UDP              SOC_IPPROTO_UDP

#define SO_INADDR_ANY               SOC_INADDR_ANY
#define SO_INADDR_BROADCAST         SOC_INADDR_BROADCAST
#define SO_INADDR_LOOPBACK          SOC_INADDR_LOOPBACK
#define SO_INADDR_UNSPEC_GROUP      SOC_INADDR_UNSPEC_GROUP
#define SO_INADDR_ALLHOSTS_GROUP    SOC_INADDR_ALLHOSTS_GROUP
#define SO_INADDR_MAX_LOCAL_GROUP   SOC_INADDR_MAX_LOCAL_GROUP

// SO_GetSockOpt() / SO_SetSockOpt()
#define SO_IP_TOS               SOC_IP_TOS
#define SO_IP_TTL               SOC_IP_TTL
#define SO_IP_MULTICAST_LOOP    SOC_IP_MULTICAST_LOOP
#define SO_IP_MULTICAST_TTL     SOC_IP_MULTICAST_TTL
#define SO_IP_ADD_MEMBERSHIP    SOC_IP_ADD_MEMBERSHIP
#define SO_IP_DROP_MEMBERSHIP   SOC_IP_DROP_MEMBERSHIP
#define SO_SO_REUSEADDR         SOC_SO_REUSEADDR
#define SO_SO_LINGER            SOC_SO_LINGER
#define SO_SO_OOBINLINE         SOC_SO_OOBINLINE
#define SO_SO_SNDBUF            SOC_SO_SNDBUF
#define SO_SO_RCVBUF            SOC_SO_RCVBUF
#define SO_SO_SNDLOWAT          SOC_SO_SNDLOWAT
#define SO_SO_RCVLOWAT          SOC_SO_RCVLOWAT
#define SO_SO_TYPE              SOC_SO_TYPE
#define SO_TCP_NODELAY          SOC_TCP_NODELAY
#define SO_TCP_MAXSEG           SOC_TCP_MAXSEG

// SO_Shutdown()
#define SO_SHUT_RD      SOC_SHUT_RD
#define SO_SHUT_WR      SOC_SHUT_WR
#define SO_SHUT_RDWR    SOC_SHUT_RDWR

// SO_Fcntl()
#define SO_F_GETFL      SOC_F_GETFL
#define SO_F_SETFL      SOC_F_SETFL
#define SO_O_NONBLOCK   SOC_O_NONBLOCK

// SO_Poll()
#define SO_POLLRDNORM       SOC_POLLRDNORM
#define SO_POLLRDBAND       SOC_POLLRDBAND
#define SO_POLLPRI          SOC_POLLPRI
#define SO_POLLWRNORM       SOC_POLLWRNORM
#define SO_POLLWRBAND       SOC_POLLWRBAND
#define SO_POLLERR          SOC_POLLERR
#define SO_POLLHUP          SOC_POLLHUP
#define SO_POLLNVAL         SOC_POLLNVAL
#define SO_POLLIN           SOC_POLLIN
#define SO_POLLOUT          SOC_POLLOUT

#define SO_INFTIM           SOC_INFTIM

#define SO_E2BIG            SOC_E2BIG
#define SO_EACCES           SOC_EACCES
#define SO_EADDRINUSE       SOC_EADDRINUSE
#define SO_EADDRNOTAVAIL    SOC_EADDRNOTAVAIL
#define SO_EAFNOSUPPORT     SOC_EAFNOSUPPORT
#define SO_EAGAIN           SOC_EAGAIN
#define SO_EALREADY         SOC_EALREADY
#define SO_EBADF            SOC_EBADF
#define SO_EBADMSG          SOC_EBADMSG
#define SO_EBUSY            SOC_EBUSY
#define SO_ECANCELED        SOC_ECANCELED
#define SO_ECHILD           SOC_ECHILD
#define SO_ECONNABORTED     SOC_ECONNABORTED
#define SO_ECONNREFUSED     SOC_ECONNREFUSED
#define SO_ECONNRESET       SOC_ECONNRESET
#define SO_EDEADLK          SOC_EDEADLK
#define SO_EDESTADDRREQ     SOC_EDESTADDRREQ
#define SO_EDOM             SOC_EDOM
#define SO_EDQUOT           SOC_EDQUOT
#define SO_EEXIST           SOC_EEXIST
#define SO_EFAULT           SOC_EFAULT
#define SO_EFBIG            SOC_EFBIG
#define SO_EHOSTUNREACH     SOC_EHOSTUNREACH
#define SO_EIDRM            SOC_EIDRM
#define SO_EILSEQ           SOC_EILSEQ
#define SO_EINPROGRESS      SOC_EINPROGRESS
#define SO_EINTR            SOC_EINTR
#define SO_EINVAL           SOC_EINVAL
#define SO_EIO              SOC_EIO
#define SO_EISCONN          SOC_EISCONN
#define SO_EISDIR           SOC_EISDIR
#define SO_ELOOP            SOC_ELOOP
#define SO_EMFILE           SOC_EMFILE
#define SO_EMLINK           SOC_EMLINK
#define SO_EMSGSIZE         SOC_EMSGSIZE
#define SO_EMULTIHOP        SOC_EMULTIHOP
#define SO_ENAMETOOLONG     SOC_ENAMETOOLONG
#define SO_ENETDOWN         SOC_ENETDOWN
#define SO_ENETRESET        SOC_ENETRESET
#define SO_ENETUNREACH      SOC_ENETUNREACH
#define SO_ENFILE           SOC_ENFILE
#define SO_ENOBUFS          SOC_ENOBUFS
#define SO_ENODATA          SOC_ENODATA
#define SO_ENODEV           SOC_ENODEV
#define SO_ENOENT           SOC_ENOENT
#define SO_ENOEXEC          SOC_ENOEXEC
#define SO_ENOLCK           SOC_ENOLCK
#define SO_ENOLINK          SOC_ENOLINK
#define SO_ENOMEM           SOC_ENOMEM
#define SO_ENOMSG           SOC_ENOMSG
#define SO_ENOPROTOOPT      SOC_ENOPROTOOPT
#define SO_ENOSPC           SOC_ENOSPC
#define SO_ENOSR            SOC_ENOSR
#define SO_ENOSTR           SOC_ENOSTR
#define SO_ENOSYS           SOC_ENOSYS
#define SO_ENOTCONN         SOC_ENOTCONN
#define SO_ENOTDIR          SOC_ENOTDIR
#define SO_ENOTEMPTY        SOC_ENOTEMPTY
#define SO_ENOTSOCK         SOC_ENOTSOCK
#define SO_ENOTSUP          SOC_ENOTSUP
#define SO_ENOTTY           SOC_ENOTTY
#define SO_ENXIO            SOC_ENXIO
#define SO_EOPNOTSUPP       SOC_EOPNOTSUPP
#define SO_EOVERFLOW        SOC_EOVERFLOW
#define SO_EPERM            SOC_EPERM
#define SO_EPIPE            SOC_EPIPE
#define SO_EPROTO           SOC_EPROTO
#define SO_EPROTONOSUPPORT  SOC_EPROTONOSUPPORT
#define SO_EPROTOTYPE       SOC_EPROTOTYPE
#define SO_ERANGE           SOC_ERANGE
#define SO_EROFS            SOC_EROFS
#define SO_ESPIPE           SOC_ESPIPE
#define SO_ESRCH            SOC_ESRCH
#define SO_ESTALE           SOC_ESTALE
#define SO_ETIME            SOC_ETIME
#define SO_ETIMEDOUT        SOC_ETIMEDOUT
#define SO_ETXTBSY          SOC_ETXTBSY
#define SO_EWOULDBLOCK      SOC_EWOULDBLOCK
#define SO_EXDEV            SOC_EXDEV

#define SO_MAXDNAME         SOC_MAXDNAME

// Name for SOConfig.alloc() and SOConfig.free()
#define SO_MEM_TCP_INFO         SOC_MEM_TCP_INFO
#define SO_MEM_TCP_SENDBUF      SOC_MEM_TCP_SENDBUF
#define SO_MEM_TCP_RECVBUF      SOC_MEM_TCP_RECVBUF
#define SO_MEM_UDP_INFO         SOC_MEM_UDP_INFO
#define SO_MEM_UDP_SENDBUF      SOC_MEM_UDP_SENDBUF
#define SO_MEM_UDP_RECVBUF      SOC_MEM_UDP_RECVBUF
#define SO_MEM_TIMEWAITBUF      SOC_MEM_TIMEWAITBUF
#define SO_MEM_REASSEMBLYBUF    SOC_MEM_REASSEMBLYBUF
#define SO_MEM_SOCKADDR         SOC_MEM_SOCKADDR
#define SO_MEM_CANONNAME        SOC_MEM_CANONNAME
#define SO_MEM_ADDRINFO         SOC_MEM_ADDRINFO

// Flags for SOConfig{}.flag
#define SO_FLAG_DHCP        SOC_FLAG_DHCP
#define SO_FLAG_PPPoE       SOC_FLAG_PPPoE
#define SO_FLAG_ZEROCONF    SOC_FLAG_ZEROCONF

//
#define IP_ERR_NONE                 SOC_IP_ERR_NONE
#define IP_ERR_BUSY                 SOC_IP_ERR_BUSY
#define IP_ERR_UNREACHABLE          SOC_IP_ERR_UNREACHABLE
#define IP_ERR_RESET                SOC_IP_ERR_RESET
#define IP_ERR_NOT_EXIST            SOC_IP_ERR_NOT_EXIST
#define IP_ERR_EXIST                SOC_IP_ERR_EXIST
#define IP_ERR_SOCKET_UNSPECIFIED   SOC_IP_ERR_SOCKET_UNSPECIFIED
#define IP_ERR_INS_RESOURCES        SOC_IP_ERR_INS_RESOURCES
#define IP_ERR_CLOSING              SOC_IP_ERR_CLOSING
#define IP_ERR_AGAIN                SOC_IP_ERR_AGAIN
#define IP_ERR_TIMEOUT              SOC_IP_ERR_TIMEOUT
#define IP_ERR_REFUSED              SOC_IP_ERR_REFUSED
#define IP_ERR_INVALID              SOC_IP_ERR_INVALID
#define IP_ERR_INV_SOCKET           SOC_IP_ERR_INV_SOCKET
#define IP_ERR_INV_OPTION           SOC_IP_ERR_INV_OPTION

//
#define IP_ERR_CANCELED         SOC_IP_ERR_CANCELED
#define IP_ERR_DATASIZE         SOC_IP_ERR_DATASIZE
#define IP_ERR_SOURCE_QUENCH    SOC_IP_ERR_SOURCE_QUENCH
#define IP_ERR_NETDOWN          SOC_IP_ERR_NETDOWN

// IPGetConfigError()
#define IP_ERR_DHCP_TIMEOUT         SOC_IP_ERR_DHCP_TIMEOUT
#define IP_ERR_DHCP_EXPIRED         SOC_IP_ERR_DHCP_EXPIRED
#define IP_ERR_DHCP_NAK             SOC_IP_ERR_DHCP_NAK
#define IP_ERR_PPPoE_TIMEOUT        SOC_IP_ERR_PPPoE_TIMEOUT
#define IP_ERR_PPPoE_SERVICE_NAME   SOC_IP_ERR_PPPoE_SERVICE_NAME
#define IP_ERR_PPPoE_AC_SYSTEM      SOC_IP_ERR_PPPoE_AC_SYSTEM
#define IP_ERR_PPPoE_GENERIC        SOC_IP_ERR_PPPoE_GENERIC
#define IP_ERR_LCP                  SOC_IP_ERR_LCP
#define IP_ERR_AUTH                 SOC_IP_ERR_AUTH
#define IP_ERR_IPCP                 SOC_IP_ERR_IPCP
#define IP_ERR_PPP_TERMINATED       SOC_IP_ERR_PPP_TERMINATED
#define IP_ERR_ADDR_COLLISION       SOC_IP_ERR_ADDR_COLLISION
#define IP_ERR_LINK_DOWN            SOC_IP_ERR_LINK_DOWN

// DNS Error
#define IP_ERR_DNS_MAX      SOC_IP_ERR_DNS_MAX
#define IP_ERR_DNS_MIN      SOC_IP_ERR_DNS_MIN

#define DNS_ERROR_MASK      SOC_DNS_ERROR_MASK
#define DNS_ERROR_NONE      SOC_DNS_ERROR_NONE
#define DNS_ERROR_FORMAT    SOC_DNS_ERROR_FORMAT
#define DNS_ERROR_SERVER    SOC_DNS_ERROR_SERVER
#define DNS_ERROR_NAME      SOC_DNS_ERROR_NAME
#define DNS_ERROR_NOTIMP    SOC_DNS_ERROR_NOTIMP
#define DNS_ERROR_REFUSED   SOC_DNS_ERROR_REFUSED

#define IP_ERR_DNS_FORMAT   SOC_IP_ERR_DNS_FORMAT
#define IP_ERR_DNS_SERVER   SOC_IP_ERR_DNS_SERVER
#define IP_ERR_DNS_NAME     SOC_IP_ERR_DNS_NAME
#define IP_ERR_DNS_NOTIMP   SOC_IP_ERR_DNS_NOTIMP
#define IP_ERR_DNS_REFUSED  SOC_IP_ERR_DNS_REFUSED

//---------------------------------------------------------------------------
// typedefs
//---------------------------------------------------------------------------
#define SOSockAddr      SOCSockAddr
#define SOInAddr        SOCInAddr
#define SOSockAddrIn    SOCSockAddrIn
#define SOLinger        SOCLinger
#define SOIpMreq        SOCIpMreq
#define SOPollFD        SOCPollFD
#define SOHostEnt       SOCHostEnt
#define SOResolver      SOCResolver
#define SOConfig        SOCConfig
#define IPSocket        SOCIPSocket
#define IP_SOCKLEN      sizeof(SOCIPSocket)
#define IP_ALEN         SOC_ALEN
#define IPTime          SOCTime
#define MAC_ALEN        SOC_MAC_ALEN

//---------------------------------------------------------------------------
// consts
//---------------------------------------------------------------------------
#define IPAddrAny       SOCAddrAny      // 0.  0.  0.  0
#define IPAddrLoopback  SOCAddrLoopback // 127.0.  0.  1
#define IPAddrLimited   SOCAddrLimited  // 255.255.255.255

//---------------------------------------------------------------------------
// functions
//---------------------------------------------------------------------------
#define SO_NtoHl                SOC_NtoHl
#define SO_NtoHs                SOC_NtoHs
#define SO_HtoNl                SOC_HtoNl
#define SO_HtoNs                SOC_HtoNs

#define SO_InetAtoN             SOC_InetAtoN
#define SO_InetNtoA             SOC_InetNtoA

#define SO_InetPtoN             SOC_InetPtoN
#define SO_InetNtoP             SOC_InetNtoP

#define SO_Socket               SOC_Socket
#define SO_Close                SOC_Close
#define SO_Shutdown             SOC_Shutdown

#define SO_Bind                 SOC_Bind
#define SO_Connect              SOC_Connect
#define SO_GetPeerName          SOC_GetPeerName
#define SO_GetSockName          SOC_GetSockName

#define SO_Read                 SOC_Read
#define SO_Recv                 SOC_Recv
#define SO_RecvFrom             SOC_RecvFrom

#define SO_Write                SOC_Write
#define SO_Send                 SOC_Send
#define SO_SendTo               SOC_SendTo

#define SO_Accept               SOC_Accept
#define SO_Listen               SOC_Listen

#define SO_SockAtMark           SOC_SockAtMark

#define SO_GetSockOpt           SOC_GetSockOpt
#define SO_SetSockOpt           SOC_SetSockOpt

#define SO_Fcntl                SOC_Fcntl

#define SO_Poll                 SOC_Poll

#define SO_GetHostByAddr        SOC_GetHostByAddr
#define SO_GetHostByName        SOC_GetHostByName
#define SO_FreeAddrInfo         SOC_FreeAddrInfo
#define SO_GetNameInfo          SOC_GetNameInfo

#define SO_GetAddrInfo          SOC_GetAddrInfo
#define SO_GetAddrInfoAsync     SOC_GetAddrInfoAsync

#define SO_GetHostID            SOC_GetHostID

#define SO_Init                 SOC_Init
#define SO_Reset                SOC_Reset
#define SO_Startup              SOC_Startup
#define SO_Cleanup              SOC_Cleanup

#define SO_SetResolver          SOC_SetResolver
#define SO_GetResolver          SOC_GetResolver

#define IPInterface             SOCInterface
#define IP_GetMacAddr           SOC_GetMacAddr
#define IP_GetAddr              SOC_GetAddr
#define IP_GetAlias             SOC_GetAlias
#define IP_GetGateway           SOC_GetGateway
#define IP_GetNetmask           SOC_GetNetmask
#define IP_GetBroadcastAddr     SOC_GetBroadcastAddr
#define IP_GetLinkState         SOC_GetLinkState
#define IP_GetConfigError       SOC_GetConfigError
#define IP_ClearConfigError     SOC_ClearConfigError

#define DHCP_GetStatus          SOC_DHCPGetStatus
#define DHCPInfo                SOCDHCPInfo;
#define DHCP_STATE_INIT         SOC_DHCP_STATE_INIT
#define DHCP_STATE_SELECTING    SOC_DHCP_STATE_SELECTING
#define DHCP_STATE_REQUESTING   SOC_DHCP_STATE_REQUESTING
#define DHCP_STATE_BOUND        SOC_DHCP_STATE_BOUND
#define DHCP_STATE_RENEWING     SOC_DHCP_STATE_RENEWING
#define DHCP_STATE_REBINDING    SOC_DHCP_STATE_REBINDING
#define DHCP_STATE_INIT_REBOOT  SOC_DHCP_STATE_INIT_REBOOT
#define DHCP_STATE_REBOOTING    SOC_DHCP_STATE_REBOOTING

#ifdef __cplusplus

}
#endif

#endif // NITROWIFI_SO2SOC_H_
