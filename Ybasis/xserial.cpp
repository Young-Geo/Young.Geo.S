//
//  serial.c
//  serial
//
//  Created by 胡兵兵 on 16/4/1.
//  Copyright © 2016年 xxx_orgnization. All rights reserved.
//

#include "xserial.h"

int xserial_open ( char * xserial_name )
{
  int fd = -1 ;

  if ( !xserial_name || !xserial_name[0] )
  {
    return -1 ;
  }

  fd = open ( xserial_name , O_RDWR|O_NOCTTY|O_NDELAY ) ;
  if ( fd < 0 )
  {
    xmessage("[serial] open [%s] failed! errno:%d\n" , xserial_name , errno ) ;
    return -1 ;
  }

  if ( fcntl(fd, F_SETFL, 0 ) < 0 )
  {
    xmessage("[serial] fcntl failed! errno:%d\n" , errno ) ;

    close(fd) ;
    return -1 ;
  }
  xmessage("[serial] open [%s] ok!\n" , xserial_name ) ;
  return fd ;
}
int xserial_init ( int fd , int speed , int databit , int stopbit , char parity , int flow_ctrl )
{
  struct termios tty;
  
  //get tty
  tcgetattr(fd, &tty);
  
  //set speed
  switch (speed)
  {
    case 0:
#ifdef B0
      speed = B0; break ;
#else
      speed = 0;  break ;
#endif
    case 300:	speed = B300;	break;
    case 600:	speed = B600;	break;
    case 1200:	speed = B1200;	break;
    case 2400:	speed = B2400;	break;
    case 4800:	speed = B4800;	break;
    case 9600:	speed = B9600;	break;
#ifdef B19200
    case 19200:	speed = B19200;	break;
#else /* B19200 */
#  ifdef EXTA
    case 19200:	speed = EXTA;	break;
#   else /* EXTA */
    case 19200:	speed = B9600;	break;
#   endif /* EXTA */
#endif	 /* B19200 */
#ifdef B38400
    case 38400:	speed = B38400;	break;
#else /* B38400 */
#  ifdef EXTB
    case 38400:	speed = EXTB;	break;
#   else /* EXTB */
    case 38400:	speed = B9600;	break;
#   endif /* EXTB */
#endif	 /* B38400 */
#ifdef B57600
    case 57600:	speed = B57600;	break;
#endif
#ifdef B115200
    case 115200:	speed = B115200;	break;
#endif
#ifdef B230400
    case 230400:	speed = B230400;	break;
#endif
#ifdef B460800
    case 460800: speed = B460800; break;
#endif
#ifdef B500000
    case 500000: speed = B500000; break;
#endif
#ifdef B576000
    case 576000: speed = B576000; break;
#endif
#ifdef B921600
    case 921600: speed = B921600; break;
#endif
#ifdef B1000000
    case 1000000: speed = B1000000; break;
#endif
#ifdef B1152000
    case 1152000: speed = B1152000; break;
#endif
#ifdef B1500000
    case 1500000: speed = B1500000; break;
#endif
#ifdef B2000000
    case 2000000: speed = B2000000; break;
#endif
#ifdef B2500000
    case 2500000: speed = B2500000; break;
#endif
#ifdef B3000000
    case 3000000: speed = B3000000; break;
#endif
#ifdef B3500000
    case 3500000: speed = B3500000; break;
#endif
#ifdef B4000000
    case 4000000: speed = B4000000; break;
#endif
    default: return 0 ; break ;
  }
  
  cfsetospeed(&tty, (speed_t)speed);
  cfsetispeed(&tty, (speed_t)speed);
  
  //set databit
  tty.c_cflag &= ~CSIZE ;
  switch (databit)
  {
    case 5: tty.c_cflag |= CS5; break;
    case 6: tty.c_cflag |= CS6; break;
    case 7: tty.c_cflag |= CS7; break;
    case 8:
    default: tty.c_cflag |= CS8; break;
  }
  
  /* Set into raw, no echo mode */
  tty.c_iflag =  IGNBRK;
  tty.c_lflag = 0;
  tty.c_oflag = 0;
  tty.c_cflag |= CLOCAL | CREAD;
  tty.c_cc[VMIN] = 1;
  tty.c_cc[VTIME] = 5;
  
  //set flow ctrl
  if (flow_ctrl == 1)//soft flow ctrl
  {
    tty.c_iflag |= IXON | IXOFF;
  }
  else
  {
    tty.c_iflag &= ~(IXON|IXOFF|IXANY);
  }
  if (flow_ctrl == 2)//hard flow ctrl
  {
    tty.c_cflag |= CRTSCTS;
  }
  else
  {
    tty.c_cflag &= ~CRTSCTS;
  }
  
  //set parity
  tty.c_cflag &= ~(PARENB | PARODD);//NONE
  if (parity == 'E')
  {
    tty.c_cflag |= PARENB;
  }
  else if (parity == 'O')
  {
    tty.c_cflag |= (PARENB | PARODD);
  }
  
  //set stopbit
  if (stopbit == 2)
  {
    tty.c_cflag |= CSTOPB;
  }
  else
  {
    tty.c_cflag &= ~CSTOPB;
  }
  
  tcsetattr(fd, TCSANOW, &tty);
  return 1 ;
}

int  xserial_recv ( int fd , char * buf , int buflen , int timeout_minsecond )
{
  int ret ;
  fd_set fdset ;

  struct timeval time ;

  //just call read
  if ( timeout_minsecond < 0 )
  {
    return (int)read(fd, buf, buflen) ;
  }

  //recv can not wait
  time.tv_sec = timeout_minsecond / 1000 ;
  time.tv_usec = (timeout_minsecond % 1000) * 1000 ;

  FD_ZERO ( &fdset ) ;
  FD_SET ( fd, &fdset ) ;

  ret = select( fd + 1, &fdset, NULL, NULL, &time) ;
  if ( ret > 0 )
  {
    ret = (int)read(fd, buf, buflen) ;
  }

  return ret ;
}

int xserial_send ( int fd , char * buf , int buflen )
{
  int ret ;

  ret = (int) write ( fd, buf, buflen ) ;
  if (ret == buflen )
  {
    return ret;
  }

  return 0 ;
}

void xserial_close ( int * fd )
{
  if ( !fd ) return ;
  close( *fd ) ; *fd = -1 ;
}
