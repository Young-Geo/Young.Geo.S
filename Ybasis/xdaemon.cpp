#include "xdaemon.h"
#include "xprotitle.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void sigchld_func ( int sig )
{
  wait ( NULL ) ;
  return ;
}
void sigpipe_func ( int sig )
{
  return ;
}

void reg_sig_func ()
{
#ifndef WIN32
#ifndef MACOSX

  //LINUX
  signal(SIGCHLD , SIG_IGN) ;
  signal(SIGPIPE , SIG_IGN) ;

#else
  //MAC OS X
  struct sigaction action ;

  //wait child func sig
  action.sa_handler = sigchld_func ;
  action.sa_flags = 0 ;
  sigemptyset ( &action.sa_mask ) ;
  sigaction ( SIGCHLD , &action , 0 ) ;

  action.sa_handler = sigpipe_func ;
  action.sa_flags = 0 ;
  sigemptyset ( &action.sa_mask ) ;
  sigaction ( SIGPIPE , &action , 0 ) ;

#endif
#endif
}

void enable_daemon()
{
#ifndef WIN32
#ifndef MACOSX
  //linux
  int fd = 0 , max_fd = 0 ;

  signal(SIGTTOU , SIG_IGN) ;
  signal(SIGTTIN , SIG_IGN) ;
  signal(SIGTSTP , SIG_IGN) ;
  signal(SIGHUP  , SIG_IGN) ;

  if ( fork() ) exit(0) ;
  if ( setsid() < 0 ) exit(1) ;
  if ( fork() ) exit(0) ;

  max_fd = getdtablesize() ;
  for ( fd = 0 ; fd < max_fd ; fd ++ ) close(fd) ;
  umask(0) ;
#endif
#endif
  reg_sig_func () ;
}

int pid_exist ( int dst_pid )
{
  if ( dst_pid <= 0 )
    return 0 ;
#ifndef WIN32
  return !kill ( dst_pid , 0 ) ;
#else
  return 0 ;
#endif
}

int enable_shadow ( char * argv[] )
{
#ifndef WIN32
  int shadow_pid = -1 ;
  int new_pid = -1 ;
  int pid = getpid() ;

  XXNULL ( pid , 0 ) ;
  XXNULL ( argv[0] , 0 ) ;

  shadow_pid = fork () ;
  if ( shadow_pid < 0 ) return 0 ;
  if ( shadow_pid > 0 ) return 1 ;

  proctitle_set( (char *)"robotleo_shadow" ) ;
  for ( ; 1 ; sleep ( 8 ) )
  {
    if ( pid_exist ( pid ) ) continue ;

    new_pid = fork () ;
    if ( new_pid < 0 ) continue ;
    if ( new_pid > 0 ) exit (0) ;

    execv ( argv[0] , argv ) ;
    exit (0) ;
  }
#endif
  return 0 ;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
