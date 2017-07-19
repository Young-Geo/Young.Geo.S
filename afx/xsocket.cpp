#include "xsocket.h"
//////////////////////////////////////////ssl///////////////////////////////////////////////////////
static int _xinited_win_sock = 0 ;
int init_sock_env ()
{
#ifndef WIN32
  if ( !_xinited_win_sock )
    _xinited_win_sock = 1 ;
  return 1 ;
#else
  WSADATA wsaData ;
  int iResult = 0 ;
  
  if ( _xinited_win_sock ) return 1 ;
  
  iResult = WSAStartup ( MAKEWORD(2, 2) , &wsaData ) ;
  if (iResult != NO_ERROR) return 0 ;
  
  _xinited_win_sock = 1 ;
  return 1 ;
#endif
}

int xsk_socket_option_func_reuse_port ( int fd )
{
  int option = 1 ;
  
  if ( setsockopt ( fd , SOL_SOCKET , SO_REUSEADDR , (const char *)&option ,
                   sizeof( option ) ) < 0 )
  {
    return 0 ;
  }
  return 1 ;
}
int xsk_set_buffer_size (int sk , int size)
{
  unsigned int option_value;
  socklen_t option_len;
  
  if ( size > Kbyte(60) || size <= 0 ) size = Kbyte(60) ;
  
  option_value = size;
  option_len = sizeof(option_value);
  
  if (setsockopt(sk, SOL_SOCKET, SO_SNDBUF, (void*) &option_value, option_len) < 0)
  {
    return 0;
  }
  if (setsockopt(sk, SOL_SOCKET, SO_RCVBUF, (void*) &option_value, option_len) < 0)
  {
    return 0;
  }
  
  return 1 ;
}
int xsk_set_keep_alive_mode(int sk)
{
#ifndef _WIN32
  unsigned int option_value;
  socklen_t option_len;
  
  option_value = 1;
  option_len = sizeof(option_value);
  
  if (setsockopt(sk, SOL_SOCKET, SO_KEEPALIVE, (void*) &option_value, option_len) < 0)
  {
    return 0;
  }
  
#ifdef TCP_KEEPIDLE
  option_value = 5;
  option_len = sizeof(option_value);
  
  if (setsockopt(sk, IPPROTO_TCP, TCP_KEEPIDLE, (void*) &option_value, option_len) < 0)
  {
    return 0;
  }
#endif
  
#endif
  
  return 1;
}

int xsk_init_listen_socket ( int port , int count , __init_sk_fc init_func )
{
  struct sockaddr_in server_addr ;
  int server_socket = -1 ;
  
  if ( port <= 0 )
  {
    xmessage ( "[tcp]bind port = %d failed!\n" , port ) ;
    return -1 ;
  }
  
  if ( count <= 0 || count > XSK_LISTEN_DEFAULT )
  {
    count = XSK_LISTEN_DEFAULT ;
  }
  
  init_sock_env () ;
  
  memset(&server_addr, 0, sizeof(server_addr)) ;
  server_addr.sin_family = AF_INET ;
  server_addr.sin_addr.s_addr = htons( INADDR_ANY ) ;
  server_addr.sin_port = htons( (short)port );
  
  server_socket = socket(AF_INET,SOCK_STREAM,0);
  if( server_socket < 0 ) return -1 ;
  
  xsk_socket_option_func_reuse_port ( server_socket ) ;
  if ( init_func )
    if ( !(*init_func) ( server_socket ) )
      return -1 ;
  
  xmessage ( "[tcp]bind port = %d...\n" , port ) ;
  if( ( bind ( server_socket , (struct sockaddr*)&server_addr , sizeof(server_addr) ) )
     ||( listen ( server_socket , count ) ) )
  {
    xmessage ( "[tcp].[port=%d]bind failed.\n" , port ) ;
    xsk_close ( &server_socket ) ;
    return -1 ;
  }
  xmessage ( "[tcp]bind successed.\n" ) ;
  
  return server_socket ;
}
static int xsk_set_non_block ( int fd , long nonblock )//1 : nonblock , 0 : block
{
  int ret = 0 ;
  
  ret = ioctl(fd, FIONBIO, &nonblock);//set to NON block
  return ret ? 0 : 1 ;
}
int xsk_accept ( int sk , char * remote_ip )
{
  struct sockaddr_in client_addr;
  int length = sizeof( client_addr );
  int r = 0 ;
  
  if ( sk < 0 ) return -1 ;
  if ( remote_ip )remote_ip[0] = 0 ;
  
  r = accept( sk , (struct sockaddr*)&client_addr , (socklen_t*)&length ) ;
  if ( r < 0 ) return -1 ;
  
  if ( remote_ip )
  {
    char * ip = inet_ntoa(client_addr.sin_addr) ;
    strcpy ( remote_ip , ip ) ;
  }
  
  if ( !xsk_set_non_block ( r , 0 ) )
  {
    xmessage ( "[xsk_accept] set sk to block mode error! errno=%d\n" , errno ) ;
    xsk_close(&r) ;
  }
  
  return r ;
}
int xsk_init_client ( char * ip , int port , int timeout )
{
  struct sockaddr_in server_addr ;
  int sfd = -1 ;
  unsigned long ul = 1 ;
  int successfuly = 0 ;
  int ret = 0 ;
  
  if ( !ip || port <= 0 )
    return -1 ;
  
  if ( timeout > XSK_WATI_TIME_MAX )
    timeout = XSK_WATI_TIME_MAX ;
  
  init_sock_env () ;
  
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr( ip ) ;
  server_addr.sin_port = htons( (short) port ) ;
  
  sfd = socket ( AF_INET , SOCK_STREAM , 0 ) ;
  if( sfd < 0 )
  {
    xmessage ( "[xsk_init_client].[%s:%d]connect failed! alloc socket error! errno=%d\n" , ip , port , errno ) ;
    return sfd ;
  }
  
  if ( timeout > 0 )
  {
    ret = ioctl(sfd, FIONBIO, &ul);//set to NON block
    if ( ret )
    {
      xmessage ( "[xsk_init_client].[%s:%d]connect failed! set to [non block] error! errno=%d\n" , ip , port , errno ) ;
      goto __finish ;
    }
  }
  
  xmessage ( "[xsk_init_client].[%s:%d]connecting...\n" , ip , port ) ;
  ret = connect( sfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) ;
  if ( ret < 0 )
  {
    struct timeval tv ;
    int maxfd = 0 ;
    fd_set wset ;
    
    int error = 0 ;
    int len = 0 ;
    
    if ( timeout <= 0 )
    {
      //is block mode
      xmessage ( "[xsk_init_client].[%s:%d]connect failed! errno=%d\n" , ip , port , errno ) ;
      goto __finish ;
    }
    
    //non block mode
    if ( errno != EINPROGRESS )
    {
      xmessage ( "[xsk_init_client].[%s:%d]connect failed! errno=%d\n" , ip , port , errno ) ;
      goto __finish ;
    }
    
    tv.tv_sec = timeout ;
    tv.tv_usec = 0 ;
    
    FD_ZERO ( &wset ) ;
    FD_SET ( sfd , &wset ) ;
    
    maxfd = sfd + 1 ;
    
  __select_call:
    ret = select ( maxfd ,  NULL , &wset , NULL , &tv ) ;
    if ( ret < 0 )
    {
      if ( EINTR == errno ) goto __select_call ;
      
      xmessage ( "[xsk_init_client].[%s:%d]connect failed! select error! errno=%d\n" , ip , port , errno ) ;
      goto __finish ;
    }
    if ( ret == 0 )
    {
      xmessage ( "[xsk_init_client].[%s:%d]connect failed! connect time out! errno=%d\n" , ip , port , errno ) ;
      goto __finish ;
    }
    
    len = sizeof(error) ;
    ret = getsockopt(sfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
    if ( ret < 0 || error != 0 )
    {
      xmessage ( "[xsk_init_client].[%s:%d]connect failed! errno=%d , error=%d\n" , ip , port , errno , error ) ;
      goto __finish ;
    }
    
    //non block connect successfully
  }
  
  if ( timeout > 0 )
  {
    ul = 0 ;
    ret = ioctl(sfd, FIONBIO, &ul);//set to block
    if ( ret )
    {
      xmessage ( "[xsk_init_client].[%s:%d]connect failed! set to [block] error! errno=%d\n" , ip , port , errno ) ;
      goto __finish ;
    }
  }
  
  xsk_set_keep_alive_mode ( sfd ) ;
  
#ifdef MACOSX
  {
    unsigned int option_value;
    socklen_t option_len;
    
    option_value = 1;
    option_len = sizeof(option_value);
    
    setsockopt(sfd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&option_value, option_len);
  }
#endif
  
  successfuly = 1 ;
  xmessage ( "[xsk_init_client].[%s:%d]connected ok!\n" , ip , port ) ;
  
__finish:
  if ( !successfuly )
  {
    xsk_close ( &sfd ) ;
  }
  
  return sfd;
}
int xsk_snd ( int sfd , char * data , int datasize , int flag , int timeout_minsecond )
{
  int len , ret ;
  int sended = 0 ;
  
  while ( 1 )
  {
    len = XXMIN ( ( datasize - sended ) , BUFFER_SIZE ) ;
    
    if ( timeout_minsecond > 0 )
    {
      ret = xsk_can_write(sfd, timeout_minsecond / 1000, (timeout_minsecond % 1000) * 1000) ;
      if ( ret <= 0 )
      {
        return 0 ;
      }
    }

  __send:
    len = (int)send ( sfd , data + sended , len , flag ) ;
    if ( len <= 0 )
    {
      if ( len < 0 && errno == EINTR )
      {
        goto __send ;
      }
      
      xmessage ( "[xsk] snd errno : %d\n" , errno ) ;
      return -1 ;
    }
    sended += len ;
    if ( sended >= datasize ) break ;
  }
  
  return sended ;
}
int xsk_send ( char * data , int datasize , int sfd )
{
  int head = 0 ;
  int headlen = XSK_HEAD_LEN ;
  
  int sended = 0 ;
  
  if ( !data || datasize <= 0 || datasize >= Mbyte(32) || sfd < 0 )
    return 0 ;
  
  //send head
  head = htonl ( datasize ) ;
  
  if ( (sended = xsk_snd ( sfd , (char *)&head , headlen , 0 , 0 )) != headlen )
    return 0 ;
  
  //send data
  sended = xsk_snd ( sfd , data , datasize , 0 , 0 ) ;
  
  //return
  if ( sended != datasize )
  {
    return 0 ;
  }
  return sended ;
}
int xsk_rcv ( int sfd , char * data , int datasize , int flag , int timeout_minsecond )
{
  int len , ret ;
  int recved = 0 ;
  
  while ( 1 )
  {
    len = XXMIN ( BUFFER_SIZE , datasize - recved ) ;
    
    if ( timeout_minsecond > 0 )
    {
      ret = xsk_can_read(sfd, timeout_minsecond / 1000, (timeout_minsecond % 1000) * 1000) ;
      if ( ret <= 0 )
      {
        return 0 ;
      }
    }
    
  __recv:
    len = (int)recv ( sfd , data + recved , len , flag ) ;
    if ( len <= 0 )
    {
      if ( len < 0 && errno == EINTR )
      {
        goto __recv ;
      }
      
      xmessage ( "[xsk] rcv errno : %d\n" , errno ) ;
      return -1 ;
    }
    
    recved += len ;
    if ( recved >= datasize ) break ;
  }

  return recved ;
}
int xsk_recv ( char ** data , int * datasize , int sfd )
{
  int head = 0 ;
  int headlen = XSK_HEAD_LEN ;
  
  int recved = 0 ;
  
  char * buf = NULL ;
  int buflen = 0 ;
  
  if ( !data || !datasize || sfd < 0 )
    return 0 ;
  
  //init
  *data = NULL ;
  *datasize = 0 ;
  
  //recv head
  recved = xsk_rcv ( sfd , (char *)&head , headlen , 0 , 0 ) ;
  if ( recved != headlen ) return 0 ;
  
  buflen = ntohl ( head ) ;
  if ( buflen <= 0 || buflen >= Mbyte(32) )
  {
    xmessage ( "[sk_recv] datasize = %d . error!\n" , buflen ) ;
    return 0 ;
  }
  
  buf = xmalloc ( buflen ) ;
  if ( !buf ) return 0 ;
  
  //recv data
  recved = xsk_rcv ( sfd , buf , buflen , 0 , 0 ) ;
  
  //return
  if ( recved != buflen )
  {
    xfree ( buf ) ;
    return 0 ;
  }
  
  *data = buf ;
  *datasize = buflen ;
  
  return recved ;
}
int xsk_disconnect ( int * sfd )
{
  int i = *sfd ;
  
  if ( i < 0 ) return 0 ;
  shutdown ( i , 2 ) ;
  
  xsk_close ( sfd ) ;
  return 1 ;
}
void xsk_close ( int * sfd )
{
  if ( sfd < 0 ) return ;
#ifndef WIN32
  close ( *sfd ) ;
#else
  closesocket ( *sfd ) ;
#endif
  *sfd = -1 ;
}
int data_left ( int soc )
{
#ifndef WIN32
  int data = 0 ;
  
  if ( soc < 0 )
    return 0 ;
  ioctl ( soc , FIONREAD , &data ) ;
#else
  u_long data = 0 ;
  
  if ( soc < 0 )
    return 0 ;
  ioctlsocket ( soc , FIONREAD , &data ) ;
#endif
  return (int)data ;
}
int xsk_can_read ( int sfd , int sec , int usec )
{
  int maxfd = 0 ;
  fd_set rset ;
  struct timeval tv ;
  int ret = 0 ;
  
  if ( sfd < 0 ) return 0 ;
  
  tv.tv_sec = sec ;
  tv.tv_usec = usec ;
  
  FD_ZERO ( &rset ) ;
  FD_SET ( sfd , &rset ) ;
  
  maxfd = sfd + 1;
  
__select_call:
  ret = select ( maxfd , &rset , NULL , NULL , &tv ) ;
  if ( ret < 0 )
  {
    if ( errno == EINTR ) goto __select_call ;
    return 0 ;
  }
  if ( ret == 0 ) return 0 ;
  if ( FD_ISSET ( sfd , &rset) ) return 1 ;
  else return 0 ;
}
int xsk_can_write ( int sfd , int sec , int usec )
{
  int maxfd = 0 ;
  fd_set wset ;
  struct timeval tv ;
  int ret = 0 ;
  
  if ( sfd < 0 ) return 0 ;
  
  tv.tv_sec = sec ;
  tv.tv_usec = usec ;
  
  FD_ZERO ( &wset ) ;
  FD_SET ( sfd , &wset ) ;
  
  maxfd = sfd + 1;
  
__select_call:
  ret = select ( maxfd , NULL , &wset , NULL , &tv ) ;
  if ( ret < 0 )
  {
    if ( errno == EINTR ) goto __select_call ;
    return 0 ;
  }
  
  if ( ret == 0 ) return 0 ;
  
  if ( FD_ISSET ( sfd , &wset) ) return 1 ;
  else return 0 ;
}
void xsk_forward ( int sfd , int cfd )
{
  int ret = 0 ;
  int gaptime = Second(3) ;
  struct timeval tv ;
  
  if ( sfd < 0 || cfd < 0 ) return ;
  
  memset(&tv, 0, sizeof(tv)) ;
  while (1)
  {
    int maxfd = 0 ;
    fd_set rset ;
    char buffer [ BUFFER_SIZE ] ;
    
    if ( tv.tv_sec <= 0 && tv.tv_usec <= 0 )
    {
      tv.tv_sec = gaptime ;
      tv.tv_usec = 0 ;
    }
    
    FD_ZERO ( &rset ) ;
    FD_SET ( sfd , &rset ) ;
    FD_SET ( cfd , &rset ) ;
    
    maxfd = sfd ;
    if ( cfd > maxfd ) maxfd = cfd ;
    maxfd ++ ;
    
  __select_call:
    ret = select ( maxfd , &rset , NULL , NULL , &tv ) ;
    if ( ret < 0 )
    {
      if ( errno == EINTR ) goto __select_call ;
      break ;
    }
    
    if ( ret == 0 ) continue ;
    
    if ( FD_ISSET ( sfd , &rset) )
    {
      ret = (int)recv ( sfd , buffer , BUFFER_SIZE , 0 ) ;
      if ( ret <= 0 ) break ;
      
      ret = (int)send ( cfd , buffer , ret , 0 ) ;
      if ( ret <= 0 ) break ;
    }
    else if ( FD_ISSET ( cfd , &rset) )
    {
      ret = (int)recv ( cfd , buffer , BUFFER_SIZE , 0 ) ;
      if ( ret <= 0 ) break ;
      
      ret = (int)send ( sfd , buffer , ret , 0 ) ;
      if ( ret <= 0 ) break ;
    } else break ;
  }
  
  return ;
}
//////////////////////////////////////////////////////////////////////////
