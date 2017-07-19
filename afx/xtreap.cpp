////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#include "xtreap.h"
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
static void xtreap_node_maintain ( xtreap_node * t )
{
  t ->size = 1 ;
  if ( t ->ch[0] ) t ->size += t ->ch[0] ->size ;
  if ( t ->ch[1] ) t ->size += t ->ch[1] ->size ;
}
static void xtreap_node_rotate(xtreap_node ** pt, int d)
{
  xtreap_node * t = *pt ;
  xtreap_node * k = t ->ch [ d ^ 1 ] ;
  
  t ->ch[ d ^ 1 ] = k ->ch[d] ;
  k ->ch[d] = t ;
  
  xtreap_node_maintain(t) ; //必须先维护t，再维护k，因为此时t是k的子节点
  xtreap_node_maintain(k) ;
  
  *pt = k ;
}
static xtreap_node * xtreap_node_alloc (int key , void * value)
{
  xtreap_node * t ;
  
  t = (xtreap_node *) xmalloc(sizeof(xtreap_node)) ;
  t ->size = 1 ;
  t ->fix = rand () ;
  t ->key = key ;
  t ->value = value ;
  t ->ch[0] = NULL ;
  t ->ch[1] = NULL ;
  return t ;
}
static void xtreap_onde_free ( xtreap_node * t )
{
  if ( !t ) return ;
  if ( t ->ch[0] ) xtreap_onde_free (t ->ch[0]) ;
  if ( t ->ch[1] ) xtreap_onde_free (t ->ch[1]) ;
  xfree(t ->value) ;
  xfree(t) ;
}
static void xtreap_node_print ( xtreap_node * t )
{
  if( t == NULL ) return ;
  xtreap_node_print(t ->ch[0]) ;
  printf ("%d " , t ->key);
  xtreap_node_print(t ->ch[1]) ;
}
void * xtreap_node_push(xtreap_node ** pt ,int key, void * value)
{
  xtreap_node * ref ;
  
  assert(value) ;
  if ( !*pt )
  {
    ref = xtreap_node_alloc(key, value) ;
    *pt = ref ;
  }
  else
  {
    xtreap_node * t = *pt ;
    int d = key < t ->key ? 0 : 1 ;  //key相等的元素,插入右边
    
    ref = (xtreap_node *)xtreap_node_push(&(t->ch[d]), key, value) ;
    if(t ->ch[d] ->fix > t ->fix)
    {
      xtreap_node_rotate(&t, d ^ 1) ;
      *pt = t ;
    }
    xtreap_node_maintain(t) ;
  }
  return ref ;
}
//一般来说，在调用删除函数之前要先用Find()函数判断该元素是否存在
void * xtreap_node_break(xtreap_node ** pt, int key)
{
  xtreap_node * t = *pt ;
  void * value ;
  
  if ( !t ) return NULL ;
  if(t ->key == key)
  {
    xtreap_node * tmp = t ;
    if(!t->ch[0])
    {
      *pt = t = t ->ch[1] ;
      value = tmp ->value ;
      xfree(tmp) ;
    }
    else if(!t ->ch[1])
    {
      *pt = t = t->ch[0] ;
      value = tmp ->value ;
      xfree(tmp) ;
    }
    else
    {
      int k = t ->ch[0] ->fix > t ->ch[1] ->fix ? 1 : 0 ;
      xtreap_node_rotate(&t, k) ; *pt = t ;
      value = xtreap_node_break(&(t ->ch[k]), key) ;
    }
  }
  else
  {
    int d = key < t ->key ? 0 : 1 ;
    value = xtreap_node_break(&(t ->ch[d]), key) ;
  }
  if ( t )
  {
    xtreap_node_maintain(t) ;
  }
  return value ;
}
void * xtreap_node_find(xtreap_node * t, int key)
{
  int d ;
  while( t )
  {
    if ( t ->key == key ) return t ->value ;
    d = key < t ->key ? 0 : 1 ;
    t = t ->ch[d] ;
  }
  return NULL ;
}
void xtreap_node_find_range(xtreap_node * t, int keyl, int keyr, void * obj, void (* callback) (void * obj , void * value))
{
  if ( !t )
  {
    return ;
  }
  if ( t ->key < keyl )
  {
    xtreap_node_find_range(t ->ch[1] , keyl , keyr , obj , callback) ;
  }
  else if ( t ->key > keyr )
  {
    xtreap_node_find_range(t ->ch[0] , keyl , keyr , obj , callback) ;
  }
  else
  {
    callback(obj, t ->value) ;
    xtreap_node_find_range(t ->ch[0] , keyl , keyr , obj , callback) ;
    xtreap_node_find_range(t ->ch[1] , keyl , keyr , obj , callback) ;
  }
}
/////////////////////////////////////
/////////////////////////////////////
/////////////////////////////////////
xtreap * xtreap_alloc ()
{
  xtreap * xt = (xtreap *)xmalloc(sizeof(xtreap)) ;
  return xt ;
}
void xtreap_free ( xtreap ** pxt )
{
  xtreap * xt ;
  
  if ( !pxt )return ;
  xt = *pxt ;
  
  if ( !xt ) return ;
  *pxt = NULL ;
  
  xtreap_onde_free(xt ->root);
  xt ->root = NULL ;
  xfree(xt) ;
}
////////////////////////////////////////////////////////////////////////////
void xtreap_print ( xtreap * xt )
{
  printf("\nxtreap tree:\n") ;
  xtreap_node_print(xt ->root);
  printf("\n") ;
}
void xtreap_test_range(void * obj , void * value)
{
  printf("%s " , (char *)value) ;
}
void xtreap_test()
{
  int s = (int)time(NULL) ;
  int i = 0 , count = 1000 ;
  int deleted = 0 ;
  void * v ;
  
  srand(s) ;
  xtreap * xt = xtreap_alloc() ;
  for ( i = 0 ; i < count ; i ++ )
  {
    int key = rand() % count ;
    char buf[32];
    sprintf(buf, "%d", key) ;
    xtreap_push(xt, key, xstrdup(buf)) ;
  }
  xtreap_print(xt) ;
  
  xtreap_node_find_range(xt ->root, 400, 500, NULL, xtreap_test_range) ;
  printf("\n") ;
  
  for ( i = 0 ; i < count / 2 ; i ++ )
  {
    int key = rand() % count ;
    if ( xtreap_find(xt, key) )
    {
      deleted ++ ;
      v = xtreap_break(xt, key) ;
      xfree(v) ;
    }
  }
  xtreap_print(xt) ;
  xtreap_free(&xt) ;
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
