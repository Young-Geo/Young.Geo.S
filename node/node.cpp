////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#include "node.h"
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
node_t * node_init ( char * name )
{
  int len ;
  node_t * n ;
  
  if ( !name || !name[0] )
  {
    xmessage("[node].error init NULL node!") ;
    return NULL ;
  }
  len = xstrlen(name) ;
  if ( len >= MINBUFFER_SIZE )
  {
    xmessage("[node].error init LEN > 256 node!") ;
    return NULL ;
  }
  proctitle_set(name) ;
  
  n = (node_t *)xmalloc(sizeof(node_t)) ;
  strcpy(n ->name, name) ;
  
  n ->subs = xlist_init();
  n ->pubs = xlist_init();
  n ->select_timeout_ms = SELECT_TIMEOUT ;
  return n ;
}
int node_free_timer ( node_t * n )
{
  struct itimerval value;
  
  value.it_value.tv_sec = 0;
  value.it_value.tv_usec = 0;
  value.it_interval = value.it_value;
  setitimer(ITIMER_REAL, &value, NULL);
  return 1 ;
}
int node_reset_timer ( node_t * n , int ms )
{
  struct itimerval ms_gap;
  
  if ( ms <= 0 )
  {
    return 0 ;
  }
  n ->timer.ms = ms ;
  
  ms_gap.it_value.tv_sec = 1 ;
  ms_gap.it_value.tv_usec = 0 ;
  ms_gap.it_interval.tv_sec = n ->timer.ms / 1000 ;
  ms_gap.it_interval.tv_usec = (n ->timer.ms % 1000) * 1000 ;
  setitimer(ITIMER_REAL, &ms_gap, NULL) ;//设定定时器
  return 1 ;
}
void node_free ( node_t ** pn )
{
  node_t * n ;
  xlist * l , * l2 ;
  sub_t * sub ;
  pub_t * pub ;
  argv_t * a ;
  
  if ( !pn ) return ;
  n = *pn ;
  
  if ( !n ) return ;
  *pn = NULL ;
  
  for ( l = n ->pubs ; l && l ->next ; l = l ->next )
  {
    pub = (pub_t *)l ->value ;
    assert(pub) ;
    
    xsk_close(&pub ->fd) ;
    xsk_close(&pub ->listenfd) ;
    simple_buffer_free(&pub ->outbuffer) ;
    
    for ( l2 = pub ->arglist ; l2 && l2 ->next ; l2 = l2 ->next )
    {
      a = (argv_t *)l2 ->value ;
      argv_free(a) ;
    }
  }
  xlist_clean(&n ->pubs);
  for ( l = n ->subs ; l && l ->next ; l = l ->next )
  {
    sub = (sub_t *)l ->value ;
    assert(sub) ;
    
    xsk_close(&sub ->fd) ;
    simple_buffer_free(&sub ->inbuffer) ;
  }
  xlist_clean(&n ->subs) ;
  if ( n ->device.device[0] )
  {
    close(n ->device.fd) ;
  }
  if ( n ->timer.ms > 0 )
  {
    node_free_timer(n) ;
  }
}
////////////////////////////////////////////////////////////////////////////
int node_add_pub ( node_t * n , int port , void * obj , int (* callback) (void * node, void * obj , int fd))
{
  pub_t * pub ;
  char key[MINBUFFER_SIZE] ;
  int fd ;
  
  if ( port <= 0 || port >= 65535 )
  {
    xmessage("[pub %d].node init pub failed!" , port) ;
    return 0 ;
  }
  
  sprintf(key, "%d" , port) ;
  if ( xlist_get(n ->pubs, key) )
  {
    xmessage("[pub %d].node init pub failed! port been used!" , port) ;
    return 0 ;
  }
  
  fd = xsk_init_listen_socket(port, XSK_LISTEN_DEFAULT, NULL) ;
  if ( fd < 0 )
  {
    xmessage("[pub %d].node init pub failed! errno: %d" , port, errno) ;
    return 0 ;
  }
  
  pub = (pub_t *)xmalloc(sizeof(pub_t)) ;
  pub ->listenfd = fd ;
  pub ->port = port ;
  pub ->fd = -1 ;
  pub ->arglist = xlist_init() ;
  pub ->obj = obj ;
  pub ->callback = callback ;
  
  xlist_cat(n ->pubs, key, XLIST_STRING, (char *)pub) ;
  return 1 ;
}
int node_pub ( node_t * n , int port , argv_t * argv , int do_free )
{
  static char buffer [ MINBUFFER_SIZE ] ;
  argv_t * a ;
  pub_t * pub = NULL ;
  
  if ( xlist_isempty(n ->pubs) )
  {
    xmessage("[pub %d].pub list is empty!" , port) ;
    if ( do_free )
    {
      argv_free(argv) ;
    }
    return 0 ;
  }
  
  if ( port == n ->cache_pub_port)
  {
    pub = n ->cache_pub_ref ;
  }
  else
  {
    sprintf(buffer, "%d" , port) ;
    pub = (pub_t *)xlist_getv(n ->pubs, buffer) ;
    if ( pub )
    {
      n ->cache_pub_port = port ;
      n ->cache_pub_ref = pub ;
    }
  }
  if (!pub)
  {
    xmessage("[pub %d].pub not inited!" , port) ;
    return 0 ;
  }
  
  if ( do_free )
  {
    a = argv ;
  }
  else
  {
    a = argv_dup(argv) ;
  }
  
  xlist_cat(pub ->arglist, NULL, XLIST_STRING, (char *)a) ;
  pub ->arglist_size ++ ;
  
  while ( pub ->arglist_size > ARGLIST_MAX )
  {
    a = (argv_t *)xlist_popv(pub ->arglist) ;
    assert(a) ;
    
    argv_free(a) ;
    pub ->arglist_size -- ;
  }
  return 1 ;
}
int node_add_sub (node_t * n , int port , void * obj , int (* callback) (void * node , void * obj , argv_t * argv))
{
  sub_t * sub ;
  
  sub = (sub_t *)xmalloc(sizeof(sub_t)) ;
  sub ->port = port ;
  sub ->fd = -1 ;
  sub ->fd_connecting = -1 ;
  
  sub ->obj = obj ;
  sub ->callback = callback ;
  sub ->fd_connecting_timeout_sum_ms = SUB_CONNECT_SUM_TIMEOUT ;
  sub ->fd_connecting_timeout_ms = SUB_CONNECT_TIMEOUT ;
  
  xlist_cat(n ->subs, NULL, XLIST_STRING, (char *)sub) ;
  return 1 ;
}
int node_init_timer ( node_t * n , int ms , void * obj , int (* callback) (void * node , void * obj) )
{
  if ( ms <= 0 || !callback )
  {
    return 0 ;
  }
  n ->timer.callback = callback ;
  n ->timer.obj = obj ;
  n ->timer.ms = ms ;
  return 1 ;
}
int node_init_device (node_t * n , char * device_path , void * obj ,
                      int (* init_callback) (char * path, void * obj) ,
                      int (* active_callback) (void * node, void * obj , int fd) )
{
  if ( !device_path || !device_path[0] )
  {
    return 0 ;
  }
  memset(n ->device.device, 0, sizeof(n ->device.device)) ;
  strncpy(n ->device.device, device_path, sizeof(n ->device.device) - 1) ;
  n ->device.fd = -1 ;
  n ->device.init_callback = init_callback ;
  n ->device.active_callback = active_callback ;
  n ->device.obj = obj ;
  return 1 ;
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
int node_pub_send ( pub_t * pub )
{
  simple_buffer_t * buffer = &pub ->outbuffer ;
  
  int ret , left , head ;
  argv_t * a ;
  
  left = simple_buffer_left(buffer) ;
  if ( left <= 0 )
  {
    a = (argv_t *)xlist_popv(pub ->arglist) ;
    if ( !a )
    {
      assert(pub ->arglist_size == 0) ;
      return 1 ;
    }
    pub ->arglist_size -- ;
    
    ret = argv_2buffer_size(a) ;
    simple_buffer_expand(buffer, ret) ;
    
    buffer ->end = argv_2buffer(a, buffer ->buffer) ;
    buffer ->idx = 0 ;
    argv_free(a) ;
    
    left = buffer ->end ;
    head = buffer ->end ;
    head = htonl(head) ;
    ret = xsk_snd(pub ->fd, (char *)&head, 4, 0, 0) ;
    if ( ret <= 0 )
    {
      xmessage("[pub %d].send head failed! errno:%d" , pub ->port, errno) ;
      return 0 ;
    }
    pub ->out_timestamp = xgetminsecond() ;
  }
  
  ret = (int)send(pub ->fd, buffer ->buffer + buffer ->idx, left, 0) ;
  if( ret > 0 )
  {
    simple_buffer_earse ( buffer, ret ) ;
    if ( buffer ->idx >= buffer ->end)
    {
      simple_buffer_reset(buffer) ;
      pub ->out_timestamp = 0 ;
    }
  }
  return ret ;
}
void node_pub_reset ( pub_t * pub )
{
  xsk_close(&pub ->fd) ;
  
  pub ->out_timestamp = 0 ;
  simple_buffer_reset(&pub ->outbuffer) ;
  
  xmessage("[pub %d].reset." , pub ->port) ;
}
void node_sub_reset ( sub_t * sub )
{
  xsk_close(&sub ->fd) ;
  xsk_close(&sub ->fd_connecting) ;
  sub ->fd_connecting_time_start = 0 ;
  
  sub ->in_timestamp = 0 ;
  simple_buffer_reset(&sub ->inbuffer) ;
  
  static int logcount ;
  if ( logcount % 20 == 0 )
  {
    xmessage("[sub %d].reset." , sub ->port) ;
  }
  logcount ++ ;
}
void node_pub_check_timeout ( node_t * n )
{
  xlist * l ;
  pub_t * pub ;
  int ms ;
  
  if ( xlist_isempty(n ->pubs) )
  {
    return ;
  }
  
  ms = xgetminsecond() ;
  for ( l = n ->pubs ; l && l ->next ; l = l ->next )
  {
    pub = (pub_t *)l ->value ;
    assert(pub) ;
    
    if ((pub ->fd < 0) ||
        (pub ->out_timestamp <= 0) ||
        ((xlist_isempty(pub ->arglist)) && (simple_buffer_left(&pub ->outbuffer) <= 0)) )
    {
      continue ;
    }
    if ( ms - pub ->out_timestamp > INOUT_TIMEOUT )
    {
      node_pub_reset(pub) ;
      xmessage("[pub %d].pub check timeout!" , pub ->port) ;
    }
  }
}
void node_sub_check_timeout ( node_t * n )
{
  sub_t * sub ;
  xlist * l ;
  int timestamp , pass ;
  
  if ( xlist_isempty(n ->subs) )
  {
    return ;
  }
  
  timestamp = xgetminsecond() ;
  for ( l = n ->subs ; l && l ->next ; l = l ->next )
  {
    sub = (sub_t *)l ->value ;
    assert(sub) ;
    
    if ((sub ->fd < 0) ||
        (sub ->in_timestamp <= 0) ||
        (simple_buffer_left(&sub ->inbuffer) <= 0))
    {
      continue ;
    }
    pass = timestamp - sub ->in_timestamp ;
    if ( pass > INOUT_TIMEOUT )
    {
      xmessage("[sub %d].sub check timeout!" , sub ->port) ;
      node_sub_reset(sub) ;
    }
  }
}
void node_sub_init ( sub_t * sub )
{
  struct sockaddr_in server_addr ;
  unsigned long ul ;
  int ret , error , len , pass ;
  
  assert(sub ->fd < 0) ;
  if ( sub ->fd_connecting < 0 )
  {
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" ) ;
    server_addr.sin_port = htons( (short) sub ->port ) ;
    
    sub ->fd_connecting = socket ( AF_INET , SOCK_STREAM , 0 ) ;
    if( sub ->fd_connecting < 0 )
    {
      xmessage ( "[sub %d].init socket faled! errno:%d", sub ->port, errno ) ;
      goto __failed ;
    }
    
    ul = 1 ;
    ret = ioctl(sub ->fd_connecting, FIONBIO, &ul);//set to NON block
    if ( ret )
    {
      xmessage ( "[sub %d].set socket NON BLOCK mode failed! errno:%d", sub ->port, errno ) ;
      goto __failed ;
    }
    
    ret = connect(sub ->fd_connecting,(struct sockaddr*)&server_addr,sizeof(server_addr)) ;
    if ( ret < 0 && errno != EINPROGRESS )
    {
      xmessage ( "[sub %d].connect failed! errno:%d", sub ->port, errno ) ;
      goto __failed ;
    }
    sub ->fd_connecting_time_start = xgetminsecond() ;
  }
  
  ret = xsk_can_write(sub ->fd_connecting, 0, sub ->fd_connecting_timeout_ms * 1000) ;
  if ( ret <= 0 )
  {
    if ( sub ->fd_connecting_time_start <= 0 )
    {
      sub ->fd_connecting_time_start = xgetminsecond() ;
    }
    pass = xgetminsecond() - sub ->fd_connecting_time_start ;
    if ( pass > sub ->fd_connecting_timeout_sum_ms )
    {
      xmessage("[sub %d].connect timeout! pass %d ms, errno:%d" , sub ->port, pass , errno ) ;
      goto __failed ;
    }
    return ;
  }
  
  len = sizeof(error) ;
  ret = getsockopt(sub ->fd_connecting, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
  if ( ret < 0 || error != 0 )
  {
    static int logcount = 0 ;
    if ( logcount % 20 == 0 )
    {
      xmessage("[sub %d].connect error occur %d! errno:%d" , sub ->port, error , errno ) ;
    }
    logcount ++ ;
    goto __failed ;
  }
  
  ul = 0 ;
  ret = ioctl(sub ->fd_connecting, FIONBIO, &ul);//set to block
  if ( ret )
  {
    xmessage("[sub %d].set to BLOCK mode failed! errno:%d" , sub ->port , errno ) ;
    goto __failed ;
  }
  
  xsk_set_keep_alive_mode ( sub ->fd_connecting ) ;
#ifdef MACOSX
  {
    unsigned int option_value;
    socklen_t option_len;
    
    option_value = 1;
    option_len = sizeof(option_value);
    
    setsockopt(sub ->fd_connecting, SOL_SOCKET, SO_NOSIGPIPE, (void *)&option_value, option_len);
  }
#endif
  
  xmessage("[sub %d].connected!", sub ->port) ;
  sub ->fd = sub ->fd_connecting ;
  sub ->fd_connecting = -1 ;
  simple_buffer_reset(&sub ->inbuffer) ;
  return ;
  
__failed:
  node_sub_reset(sub) ;
  return ;
}
void node_sub_recv ( node_t * n , sub_t * sub )
{
  static argv_t argv ;
  simple_buffer_t * buffer ;
  int ret , head ;
  
  assert(sub ->fd >= 0) ;
  buffer = &sub ->inbuffer ;
  
  if ( buffer ->idx >= buffer ->end )
  {
    //need recv the head
    ret = xsk_rcv(sub ->fd, (char *)&head, 4, 0, 0) ;
    if ( ret <= 0 )
    {
      xmessage("[sub %d].recv failed! errno:%d" , sub ->port, errno) ;
      goto __failed ;
    }
    head = ntohl(head) ;
    if ( head > Mbyte(32) )
    {
      xmessage("[sub %d].recv head = %d(%d Mbyte)! failed" , sub ->port, head , (head / 1024) / 1024) ;
      goto __failed ;
    }
    sub ->in_timestamp = xgetminsecond() ;
    simple_buffer_expand(buffer, head) ;
    buffer ->idx = 0 ;
    buffer ->end = head ;
  }
  
  ret = (int)recv(sub ->fd, buffer ->buffer + buffer ->idx, buffer ->end - buffer ->idx, 0) ;
  if ( ret <= 0 )
  {
    xmessage("[sub %d].recv failed! errno:%d" , sub ->port, errno) ;
    goto __failed ;
  }
  
  buffer ->idx += ret ;
  if ( buffer ->idx < buffer ->end )
  {
    //continue to recv
    return ;
  }
  
  argv_4buffer(&argv, buffer ->buffer, buffer ->end) ;
  sub ->callback(n , sub ->obj , &argv) ;
  
  simple_buffer_reset(buffer) ;
  sub ->in_timestamp = 0 ;
  return ;
  
__failed:
  node_sub_reset(sub) ;
  return ;
}
static int node_timer_actived = 0 ;
void timer_func ( int sig )
{
  node_timer_actived = 1 ;
}
int node_spin (node_t * n)
{
  fd_set rset , wset ;
  int ret , fdmax , fdaccept , ok , i = 0 , check_timestamp = 0 , time_stamp , do_check = 0 ;
  struct timeval tv ;
  char buffer [ BUFFER_SIZE ] ;
  
  xlist * l ;
  sub_t * sub ;
  pub_t * pub ;
  
  struct itimerval ms_gap ;

  if (n ->timer.callback )
  {
    signal(SIGALRM, timer_func);//设置定时器函数
    
    ms_gap.it_value.tv_sec = 1 ;//1秒之后开始启动定时器
    ms_gap.it_value.tv_usec = 0 ;
    ms_gap.it_interval.tv_sec = n ->timer.ms / 1000 ;//每间隔ms毫秒执行一次定时器
    ms_gap.it_interval.tv_usec = (n ->timer.ms % 1000) * 1000 ;
    setitimer(ITIMER_REAL, &ms_gap, NULL) ;//设定定时器
  }
  
  tv.tv_sec = 0 ;
  tv.tv_usec = n ->select_timeout_ms * 1000 ;
  while (1)
  {
    ok = 0 ;
    
    if ( node_timer_actived )
    {
      node_timer_actived = 0 ;
      n ->timer.callback ( n , n ->timer.obj ) ;
    }
    
    if (tv.tv_sec <= 0 && tv.tv_usec <= 0)
    {
      tv.tv_sec = 0 ;
      tv.tv_usec = n ->select_timeout_ms * 1000 ;
    }
    
    fdmax = -1 ;
    FD_ZERO ( &rset ) ;
    FD_ZERO ( &wset ) ;
    if ( xlist_notempty(n ->pubs) )
    {
      for ( l = n ->pubs ; l && l ->next ; l = l ->next )
      {
        pub = (pub_t *)l ->value ;
        assert(pub && pub ->listenfd >= 0) ;
        
        FD_SET ( pub ->listenfd , &rset ) ;
        if ( pub ->listenfd > fdmax ) fdmax = pub ->listenfd ;
        
        if (pub ->fd >= 0 )
        {
          FD_SET ( pub ->fd , &rset ) ;
          if ( pub ->fd > fdmax ) fdmax = pub ->fd ;
          
          if ( (xlist_notempty(pub ->arglist)) || (simple_buffer_left(&pub ->outbuffer) > 0) )
          {
            FD_SET ( pub ->fd , &wset ) ;
            if ( pub ->fd > fdmax ) fdmax = pub ->fd ;
          }
        }
      }
    }
    if ( xlist_notempty(n ->subs) )
    {
      for ( l = n ->subs ; l && l ->next ; l = l ->next )
      {
        sub = (sub_t *)l ->value ;
        if ( sub ->fd >= 0 )
        {
          FD_SET ( sub ->fd , &rset ) ;
          if ( sub ->fd > fdmax ) fdmax = sub ->fd ;
        }
      }
    }
    if ( n ->device.device[0] && (n ->device.fd >= 0) )
    {
      FD_SET ( n ->device.fd , &rset ) ;
      if ( n ->device.fd > fdmax ) fdmax = n ->device.fd ;
    }
    fdmax ++ ;
    
    if ( fdmax <= 0 )
    {
      ret = select(1, NULL, NULL, NULL, &tv) ;
    }
    else
    {
      ret = select (fdmax , &rset , &wset , NULL , &tv ) ;
    }
    
    if ( ret == 0 )
    {
      do_check = 1 ;
    }
    //else if ( (i ++) % 100 == 0 )
    else if ( (i = ((i++) % 100)) == 0 )
    {
      time_stamp = xgetminsecond() ;
      if ( time_stamp - check_timestamp > n ->select_timeout_ms )
      {
        do_check = 1 ;
      }
    }
    else
    {
      do_check = 0 ;
    }
    if ( do_check )
    {
      do_check = 0 ;
      check_timestamp = xgetminsecond() ;
      
      node_pub_check_timeout(n) ;
      node_sub_check_timeout(n) ;
      
      for ( l = n ->subs ; l && l ->next ; l = l ->next )
      {
        sub = (sub_t *)l ->value ;
        if ( sub ->fd >= 0 ) continue ;
        
        node_sub_init(sub) ;
      }
      if ( n ->device.device[0] && n ->device.fd < 0)
      {
        n ->device.fd = n ->device.init_callback(n ->device.device , n ->device.obj) ;
      }
      
      ok = 1 ; goto __next_select ;
    }
    
    if ( ret < 0 )
    {
      if ( errno == EINTR )
      {
        ok = 1 ; goto __next_select ;
      }
      
      xmessage("[node %s].select errno : %d\n" , n ->name, errno) ;
      break ;
    }
    
    if ( n ->device.device[0] && (n ->device.fd >= 0) )
    {
      if (FD_ISSET (n ->device.fd , &rset))
      {
        ret = n ->device.active_callback(n, n ->device.obj, n ->device.fd) ;
        if ( ret <= 0 )
        {
          xmessage("device read failed!") ;
          if ( ret < 0 )
          {
            close(n ->device.fd) ;
          }
          n ->device.fd = -1 ;
        }
        ok = 1 ; goto __next_select ;
      }
    }
    for ( l = n ->pubs ; l && l ->next ; l = l ->next )
    {
      pub = (pub_t *)l ->value ;
      assert(pub) ;
      
      assert(pub ->listenfd >= 0) ;
      if (FD_ISSET ( pub ->listenfd , &rset))
      {
        fdaccept = xsk_accept(pub ->listenfd, NULL) ;
        if ( fdaccept < 0 )
        {
          xmessage("[pub %d].accept failed!" , pub ->port) ;
        }
        else
        {
          node_pub_reset(pub);
          pub ->fd = fdaccept ;
          xmessage("[pub %d].accepted!" , pub ->port) ;
        }
        
        ok = 1 ; goto __next_select ;
      }
      else if ( pub ->fd >= 0 )
      {
        if (FD_ISSET ( pub ->fd , &wset))
        {
          ret = node_pub_send(pub) ;
          if ( ret <= 0 )
          {
            node_pub_reset(pub);
          }
          
          ok = 1 ; goto __next_select ;
        }
        else if (FD_ISSET ( pub ->fd , &rset))
        {
          if ( pub ->callback )
          {
            ret = pub ->callback(n, pub ->obj, pub ->fd) ;
            if ( ret <= 0 )
            {
              node_pub_reset(pub);
            }
          }
          else
          {
            //ignore any data from pub fd
            ret = (int)recv (pub ->fd , buffer , sizeof(buffer), 0 ) ;
            if ( ret <= 0 )
            {
              node_pub_reset(pub);
            }
          }
          ok = 1 ; goto __next_select ;
        }
      }
    }
    for ( l = n ->subs ; l && l ->next ; l = l ->next )
    {
      sub = (sub_t *)l ->value ;
      assert(sub) ;
      
      if ( sub ->fd >= 0 && FD_ISSET ( sub ->fd , &rset) )
      {
        node_sub_recv (n, sub) ;
        ok = 1 ; goto __next_select ;
      }
    }
    
  __next_select:
    if ( !ok )
    {
      xmessage("[node %s].spin loop failed!" , n ->name);
      break ;
    }
  }
  return 0 ;
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
