#ifndef __H_XSOCKET_H__
#define __H_XSOCKET_H__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef WIN32
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <WinSock2.h>//for socket
#include <iphlpapi.h> //for get mac
#include <WS2tcpip.h> //for socklen_t

#pragma comment(lib, "Ws2_32.lib")//for socket
#pragma comment(lib, "IPHLPAPI.lib") //for get mac

int init_sock_env () ;
#endif

#include "afx.h"
#include "xlog.h"

/* define */
#define XSK_WATI_TIME Second(3)
#define XSK_WATI_TIME_MAX Second(75)
#define XSK_LISTEN_DEFAULT 8

#define XSK_HEAD_LEN (4)
typedef struct __XSK_PARAM
{
  int sk ;
  void * param ;
} XSK_PARAM ;

typedef int (* __init_svr_fd_callback ) ( int fd ) ;
typedef __init_svr_fd_callback __init_sk_fc ;

int xsk_socket_option_func_reuse_port ( int fd ) ;
int xsk_set_keep_alive_mode(int sk) ;
int xsk_set_buffer_size (int sk , int size) ;

int xsk_init_listen_socket ( int port , int count , __init_sk_fc init_func ) ;
int xsk_accept ( int sk , char * remote_ip ) ;
int xsk_init_client ( char * ip , int port , int timeout ) ;
int xsk_snd ( int sfd , char * data , int datasize , int flag , int timeout_minsecond ) ;
int xsk_send ( char * data , int datasize , int sfd ) ;
int xsk_rcv ( int sfd , char * data , int datasize , int flag , int timeout_minsecond ) ;
int xsk_recv ( char ** data , int * datasize , int sfd ) ;
int xsk_disconnect ( int * sfd ) ;
void xsk_close ( int * sfd ) ;

int xsk_can_read ( int sfd , int sec , int usec ) ;
int xsk_can_write ( int sfd , int sec , int usec ) ;

void xsk_forward ( int sfd , int cfd ) ;

int data_left ( int soc ) ;
//////////////////////////////////////////////////////////////////////////
#endif
