#include "xudp.h"
//////////////////////////////////////////////////////////////////////////
int xudp_init_listen_socket ( int port )
{
  int fd = -1 , on = 1 ;
  struct sockaddr_in sin ;
  int sockflags = 0 ;
  int ok = 0 ;
  int ret = 0 ;
  
  xmessage ( "[udp] bind udp port = %d...\n" , port ) ;
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0)
  {
    xmessage ( "[udp] can not create udp sk. error=%d\n" , errno);
    goto __finish ;
  }
  
  ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)) ;
  if ( ret )
  {
    xmessage ( "[udp] can not set sk to reuse address.error=%d\n" , errno);
    goto __finish ;
  }
  
  /* We must not block on the network socket, in case a packet
   * gets lost between select and recv, otherise the process
   * will stall until the timeout, and other processes trying to
   * log will also stall.
   */
  
  sockflags = fcntl(fd, F_GETFL) ;
  if (sockflags == -1)
  {
    xmessage ( "[udp] can not set sk to nonblock mode.error=%d\n" , errno);
    goto __finish ;
  }
  sockflags |= O_NONBLOCK;
  sockflags = fcntl(fd, F_SETFL, sockflags);
  
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons( (short)port );
  sin.sin_addr.s_addr = INADDR_ANY ;
  ret = bind(fd, (struct sockaddr *) &sin, sizeof(sin)) ;
  if ( ret )
  {
    xmessage ( "[udp] can not bind udp port=%d.error=%d\n" , port , errno);
    goto __finish ;
  }
  
  ok = 1 ;
  xmessage ( "[udp] bind successed.\n" ) ;
  
__finish:
  if ( !ok )
  {
    close(fd);
    fd = -1 ;
  }
  
  return fd;
}
int xudp_init_udp_sender ( char * ip , int port , udp_sender_t * sender , int broadcast )
{
  int ret = 0 ;
  
  if (!sender)
  {
    xmessage ( "[udp] init udp_sender param eror!\n" );
    return 0 ;
  }
  
  sender ->sk = socket(AF_INET, SOCK_DGRAM, 0);
  if ( broadcast )
  {
    broadcast = 1;
    setsockopt(sender ->sk, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
  }
  if (sender ->sk < 0)
  {
    xmessage ( "[udp] init udp_sender can not create udp sk. error=%d\n" , errno);
    return 0 ;
  }
  
  sender ->addr.sin_family = AF_INET;
  sender ->addr.sin_port = htons(port);
  ret = inet_pton(AF_INET, ip, &(sender ->addr.sin_addr));
  if ( ret <= 0 )
  {
    xmessage ( "[udp] init udp_sender invalid ip:%s\n" , ip);
    xsk_close(&(sender ->sk)) ;
    return 0 ;
  }
  
  strcpy(sender ->ip, ip) ;
  sender ->port = port ;
  
  return 1 ;
}
int xudp_recv ( int fd , char buffer[UDP_PKT_MAX_SIZE] , char ip[MINBUFFER_SIZE] , int * port )
{
  struct sockaddr_in frominet ;
  socklen_t frominetlen = sizeof(struct sockaddr_in) ;
  int msglen ;
  char * ipptr ;
  
  msglen = (int)recvfrom( fd, buffer, UDP_PKT_MAX_SIZE - 1, 0, (struct sockaddr *) &frominet, &frominetlen) ;
  ipptr = inet_ntoa(frominet.sin_addr) ;
  if ( !ipptr || !ipptr[0] )
  {
    ipptr = (char*)"unknowhost" ;
  }
  strcpy(ip, ipptr) ;
  if ( port ) * port = frominet.sin_port ;
  
  return msglen ;
}
int xudp_send ( udp_sender_t * sender, char buffer[UDP_PKT_MAX_SIZE] , int size )
{
  int sentlen ;
  
  sentlen = (int)sendto( sender ->sk , buffer , size , 0, (struct sockaddr *)&(sender ->addr), sizeof(sender ->addr)) ;
  return sentlen ;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
