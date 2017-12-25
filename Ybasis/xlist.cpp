#include "xlist.h"

static void init_element ( xlist *xlst ,
                          const char * key ,
                          int type ,
                          char * value )
{
  xlst->key = xstrdup(key) ;
  xlst->value = value ;
  xlst->type = type ;
  xlst->hash = xstr_hash (key) ;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static void xlist_exchange ( xlist * i , xlist * j )
{
  int itmp = 0 ;
  char * strtmp = NULL ;

  itmp = i ->type ;
  i ->type = j ->type ;
  j ->type = itmp ;

  itmp = i ->hash ;
  i ->hash = j ->hash ;
  j ->hash = itmp ;

  strtmp = i ->key ;
  i ->key = j ->key ;
  j ->key = strtmp ;

  strtmp = i ->value ;
  i ->value = j ->value ;
  j ->value = strtmp ;
}

xlist * xlist_init ()
{
  xlist * ret = (xlist *) xmalloc ( sizeof ( xlist ) ) ;

  ret ->prev = ret ;

  return ret ;
}

void xlist_cat ( xlist *xlst ,
                const char * key ,
                int type ,
                char * value )
{
  xlist * tail = NULL ;

  XNULL ( xlst ) ;

  tail = xlst ->prev ;
  assert(tail) ;

  tail ->next = ( xlist * ) xmalloc ( sizeof ( xlist ) ) ;
  tail ->next ->prev = tail ;
  xlst ->prev = tail ->next ;

  init_element ( tail , key , type , value ) ;
}
void xlist_combin ( xlist * xlst , xlist * l )
{
  xlist * tail = NULL ;
  
  XNULL(xlst || l) ;
  if ( xlist_isempty(l) )
  {
    return ;
  }
  
  tail = xlst ->prev ;
  assert(tail) ;
  
  xlist_exchange(tail, l) ;
  tail ->next = l ->next ;
  l ->next ->prev = tail ;
  xlst ->prev = l ->prev ;
  
  l ->next = NULL ;
  xlist_clean(&l) ;
}
void xlist_push ( xlist *xlst , const char * key , int type , char * value )
{
  xlist * next = NULL ;
  xlist * ne = NULL ;

  XNULL ( xlst ) ;

  next = xlst ->next ;
  if ( !next )
  {
    xlist_cat(xlst, key, type, value) ;
    return ;
  }
  ne = ( xlist * ) xmalloc ( sizeof ( xlist ) ) ;
  xlst ->next = ne ;
  ne ->prev = xlst ;

  ne ->next = next ;
  next ->prev = ne ;

  init_element ( ne , key , type , value ) ;

  xlist_exchange ( xlst , ne ) ;
}

xlist * xlist_get ( xlist *x , const char *key )
{
  int h ;

  XXNULL ( x , NULL ) ;

  h = xstr_hash ( key ) ;
  while ( x && ( x->next ) )
  {
    if ( ( x->hash == h ) && ( !xstrcmp (x->key , key) ) )
    {
      return x ;
    }
    else
      x = x->next ;
  }
  return NULL ;
}

int xlist_set (
               xlist *xlst ,
               const char * key ,
               char *value )
{
  XXNULL ( xlst , 0 ) ;

  xlst = xlist_get ( xlst , key ) ;
  XXNULL ( xlst , 0 ) ;

  switch ( xlst->type )
  {
    case XLIST_STRING :
      xfree ( xlst ->value ) ;
      break ;
    case XLIST_XLIST :
      xlist_clean ( (xlist **)(&xlst ->value) ) ;
      break ;
    default : break ;
  }

  xlst->value = value ;
  return 1 ;
}

char * xlist_getv (
                   xlist * lst ,
                   const char *key )
{
  XXNULL ( lst , NULL ) ;

  lst = xlist_get (lst, key);
  if ( !lst ) return NULL ;

  return (char *)(lst->value) ;
}

xlist * xlist_index ( xlist * lst , int idx )
{
  xlist * l = NULL ;
  int i = 0 ;

  if ( !lst || idx < 0 ) return NULL ;

  for ( l = lst , i = 0 ; l && l ->next && ( i < idx ) ; l = l ->next , i ++ ) ;

  if ( l && l ->next ) return l ;
  else return NULL ;
}

xlist * xlist_dup ( xlist *src )
{
  xlist *ret = NULL ;
  xlist *dst = NULL ;

  XXNULL ( src , NULL ) ;

  ret = dst = xlist_init () ;

  while (src->next)
  {
    dst->key = xstrdup (src->key) ;
    dst->type = src->type ;
    dst->hash = src->hash ;

    switch (src->type)
    {
      case XLIST_PTR:
        dst->value = src->value;
        break;

      case XLIST_STRING:
        dst->value = xstrdup ((char *) src->value);
        break;

      case XLIST_XLIST:
        dst->value = (char *) xlist_dup ((xlist *) src->value);
        break;
    }
    dst ->next = (xlist *)xmalloc (sizeof (xlist) ) ;
    dst ->next ->prev = dst ;

    dst = dst->next ;
    src = src->next ;
  }
  ret ->prev = dst ;

  return ret ;
}

int xlist_size ( xlist *lst )
{
  int ret = 0 ;
  while ( lst && ( lst ->next ) )
  {
    ret ++ ;
    lst = lst ->next ;
  }

  return ret ;
}

static void xlist_orders ( xlist * xs , int big )
{
  xlist * olds = xs ;
  xlist * i = NULL ;
  xlist * j = NULL ;
  xlist * prej = NULL ;

  xlist * head = NULL ;
  xlist * end = NULL ;
  int hit = 0 ;

  if ( xlist_isempty ( xs ) ) return ;

  head = (xlist *)xmalloc ( sizeof ( xlist ) ) ;
  end = (xlist *)xmalloc ( sizeof ( xlist ) ) ;
  head ->next = end ;
  head ->prev = end ;
  end ->prev = head ;
  end ->next = NULL ;
  for ( i = olds ; i && ( i ->next ) ; )
  {
    xlist * next = i ->next ;

    for ( prej = head , j = head ->next ; j && ( j ->next ) ; prej = j , j = j ->next )
    {
      if ( big ) hit = (int)( xstrcmp ( j ->key , i ->key ) <= 0 ) ;
      else hit = (int)( xstrcmp ( j ->key , i ->key ) >= 0 ) ;
      if ( hit )
      {
        break ;
      }
    }
    prej ->next = i ; i ->prev = prej ;
    i ->next = j ; j ->prev = i ;

    i = next ;
  }

  prej = end ->prev ;
  prej ->next = i ; i ->prev = prej ; head ->prev = i ;
  xfree ( end ) ;//free end

  if ( head ->next != olds )
  {
    xlist * news = head ->next ;

    xlist * pre_new = head ;
    xlist * post_new = news ->next ;
    xlist * pre = olds ->prev ;
    xlist * post = olds ->next ;

    xlist_exchange ( news , olds ) ;

    pre_new ->next = olds ;
    olds ->prev = pre_new ;

    if ( news ->next != olds )
    {
      olds ->next = post_new ;//
      post_new ->prev = olds ;//

      pre ->next = news ;//
      news ->prev = pre ;//
    }
    else
    {
      olds ->next = news ;
      news ->prev = olds ;
    }
    news ->next = post ;
    post ->prev = news ;
  }
  olds ->prev = head ->prev ;
  xfree ( head ) ;//free head
  return ;
}

void xlist_order ( xlist *xs )
{
  xlist_orders ( xs , 0 ) ;
  return ;
}

void xlist_order_big ( xlist *xs )
{
  xlist_orders ( xs , 1 ) ;
  return ;
}

void xlist_reverse ( xlist *xs )
{
  xlist * head = NULL ;
  xlist * tail = NULL ;

  XNULL(xs) ;

  for ( head = xs , tail = head ->prev ->prev ;
       ( head != tail ) && ( tail ->next != head ) ;
       head = head ->next , tail = tail ->prev )
  {
    xlist_exchange(head, tail) ;
  }

  return ;
}

void xlist_dump ( xlist *xs , int level )
{
  const char *spaces = "                    ";
  if (!xs)
  {
    fprintf (stderr , "Error ! xs == NULL\n");
    return;
  }
  if ( level < 0 )
  {
    level	= 0 ;
  }

  while (xs->next)
  {
    switch (xs->type)
    {
      case XLIST_STRING:
        fprintf (stderr, "%sxs->[%s]:[%s]\n", spaces + (20 - level),
                 (xs->key)?xs->key:(char *)"NULL", (xs->value)?(xs->value):(char *)"NULL");
        break;
      case XLIST_XLIST:
        fprintf (stderr, "%sxs->[%s]:\n", spaces + (20 - level), (xs->key)?(xs->key):(char *)"NULL");
        xlist_dump ( (xlist *)( xs->value ) , level + 1);
        break;
      default:
        fprintf (stderr, "%sxs->[%s]:[0x%x]\n", spaces + (20 - level),
                 (xs->key)?(xs->key):(char *)"NULL", (unsigned int)(xs->value-(char*)0) );
        break;
    }
    xs = xs->next;
  }
}
void xlist_terminal ( xlist *x , xlist * pos )
{
  xlist * tail = NULL ;
  XNULL(x) ;

  tail = x ->prev ;

  pos ->prev ->next = tail ;
  tail ->prev ->next = NULL ;
  tail ->prev = pos ->prev ;
  pos ->prev = NULL ;

  xlist_clean(&pos) ;
}
void xlist_clean ( xlist ** px )
{
  xlist * x ;
  if ( !px ) return ;
  x = *px ;
  
  if ( !x )return ;
  *px = NULL ;
  
  while ( x )
  {
    xlist *next = x->next;
    switch (x->type)
    {
      case XLIST_XLIST:
        xlist_clean ( (xlist **)(&(x ->value)) ) ;
        break;
      case XLIST_STRING:
        xfree (x->value);
        break;
      default:break ;
    }
    xfree (x->key);
    xfree (x);
    x = next;
  }
}

void xlist_reset ( xlist *x )
{
  xlist * head = x ;

  while (x)
  {
    xlist *next = x->next;
    switch (x->type)
    {
      case XLIST_XLIST:
        xlist_clean ( (xlist **)(&(x ->value)) ) ;
        break;
      case XLIST_STRING:
        xfree (x->value);
        break;
      default:break ;
    }
    xfree (x->key);

    if ( head != x )
    {
      xfree (x);
    }
    x = next;
  }

  if ( head )
  {
    memset ( head , 0 , sizeof ( xlist ) ) ;
    head ->prev = head ;
  }

  return ;
}
xlist * xlist_break ( xlist * xs , xlist *index )
{
  xlist * pivot = NULL ;

  XXNULL ( xs , NULL ) ;
  XXNULL ( index , NULL ) ;

  pivot = index ->next ;

  xlist_exchange ( index , pivot ) ;

  pivot ->prev ->next = pivot ->next ;
  if ( pivot ->next )
  {
    pivot ->next ->prev = pivot ->prev ;
  }
  if ( pivot == xs ->prev )
  {
    xs ->prev = index ;
  }
  pivot ->next = NULL ;
  pivot ->prev = NULL ;

  return pivot ;
}
xlist * xlist_insert ( xlist * xs , xlist * index , const char * key , int type , char * value )
{
  xlist * inst ;
  
  inst = (xlist *) xmalloc(sizeof(xlist) ) ;
  init_element(inst, key, type, value) ;
  
  xlist_exchange(index, inst) ;
  
  if ( index ->next )
  {
    index ->next ->prev = inst ;
  }
  else
  {
    assert(xs ->prev == index) ;
    xs ->prev = inst ;
  }
  inst ->next = index ->next ;
  index ->next = inst ;
  inst ->prev = index ;
  
  return inst ;
}
void xlist_delete (
                   xlist *xs ,
                   const char *key )
{
  xlist *element = NULL ;

  XNULL ( xs ) ;

  element = xlist_get ( xs , key ) ;

  if ( !element )
    return;

  element = xlist_break ( xs , element ) ;
  xlist_clean ( &element ) ;

  return ;
}

xlist * xlist_pop ( xlist * xs )
{
  xlist * ret = NULL ;

  if ( xlist_isempty ( xs ) ) return NULL ;

  ret = xlist_break ( xs , xs ) ;
  return ret ;
}
char * xlist_popv ( xlist * xs )
{
  xlist * pop = NULL ;
  char * ret ;
  
  if ( xlist_isempty ( xs ) ) return NULL ;
  
  pop = xlist_break ( xs , xs ) ;
  if ( !pop ) return NULL ;
  
  ret = pop ->value ;
  
  pop ->value = NULL ;
  xlist_clean(&pop) ;
  
  return ret ;
}
char * xlist_popv_notzero ( xlist * xs )
{
  xlist * pop = NULL ;
  char * ret = NULL ;
  
  if ( xlist_isempty ( xs ) ) return NULL ;
  
  while (1)
  {
    pop = xlist_break ( xs , xs ) ;
    if ( !pop ) break ;
    
    ret = pop ->value ;
    
    pop ->value = NULL ;
    xlist_clean(&pop) ;
    
    if ( ret ) break ;
  }
  
  return ret ;
}
xlist * xlist_cut ( xlist * xs )
{
  xlist * ret = NULL , * tail = NULL ;
  XXNULL ( xs , NULL ) ;

  if ( xlist_isempty ( xs ) ) return NULL ;

  tail = xs ->prev ->prev ;
  ret = xlist_break ( xs , tail ) ;
  return ret ;
}

xlist * xlist_remove ( xlist * xs , const char * key )
{
  xlist * ret = NULL ;

  XXNULL ( xs , NULL ) ;

  ret = xlist_get ( xs , key ) ;
  return xlist_break ( xs , ret ) ;
}

void	xlist_kill_remove(xlist * xs , const char * key)
{
	xlist *ele = NULL;
	
	XNULL(xs);	
	XNULL(key);
	
	if (!(ele = xlist_remove(xs, key)))
		return;

	switch (ele->type)
	{
      case XLIST_XLIST:
        xlist_clean ( (xlist **)(&(ele->value)) ) ;
        break;
      case XLIST_STRING:
        xfree (ele->value);
        break;
      default:break ;

	}
    xfree (ele->key);
    xfree (ele);
}

char ** xstrsplit ( char *string, char *delimiter, int max_tokens ) ;
void xstrsplit_free (char ** xarray) ;

xlist * xlist_str2lst (char *str , char * gap)
{
  xlist *ret = NULL ;
  int i = 0 ;
  char **deparray = NULL ;

  XXNULL ( str , NULL ) ;
  XXNULL ( str[0] , NULL ) ;
  XXNULL ( gap , NULL ) ;
  XXNULL ( gap[0] , NULL ) ;

  ret = xlist_init () ;

  deparray = xstrsplit ( str , gap , 0 ) ;

  while ( deparray && deparray[i])
  {
    xlist_cat (ret , deparray[i] , XLIST_PTR , NULL);
    i++;
  }

  xstrsplit_free (deparray);

  return ret;
}

void xlist_lst2str (xlist * xs , char ** data , int * len )
{
  //define
  xchain * chain ;

  //chxe
  XNULL ( xs ) ;
  XNULL ( data ) ;
  XNULL ( len ) ;

  chain = xchain_alloc () ;
  //do
  while ( xs )
  {
    if ( xs ->key )
    {
      xchain_add ( chain , xs ->key , (int)(strlen ( xs ->key ) ) ) ;
      xchain_add ( chain , "," , 1 ) ;
    }
    xs = xs ->next ;
  }

  xchain_2data ( chain , data , len ) ;
  xchain_clear ( chain ) ;

  (*len) -- ;
  (*data)[ (*len) ] = 0 ;

  return ;
}
char ** xstrsplit ( char *string, char *delimiter, int max_tokens )
{
  xlist * string_list = NULL ;
  xlist * slist ;
  char **str_array = NULL ;
  char *s = NULL ;
  int n = 0 ;
  int i = 0 ;
  char *remainder = NULL ;
  int delimiter_len = 0 ;

  XXNULL ( string , NULL ) ;
  XXNULL ( delimiter[0] , NULL ) ;

  if (max_tokens < 1)
  {
    max_tokens = XINT_MAX ;
  }
  delimiter_len = (int)strlen (delimiter);

  string_list = xlist_init () ;
  XXNULL ( string_list , NULL ) ;

  remainder = string;
  s = strstr (remainder, delimiter);
  if ( s )
  {
    while ( --max_tokens && s )
    {
      int len ;

      len = (int)(s - remainder) ;
      xlist_cat ( string_list , "0" , XLIST_STRING , xstrndup (remainder, len) ) ;
      n ++ ;
      remainder = s + delimiter_len ;
      s = strstr ( remainder , delimiter ) ;
    }
  }
  if ( remainder )
  {
    xlist_cat ( string_list , "0" , XLIST_STRING , xstrdup (remainder) ) ;
    n ++ ;
  }

  str_array = (char **)xmalloc (8 * n + 1 ) ;

  for ( i = 0 , slist = string_list ;
       (i < n) && ( slist ) && ( slist ->next ) ;
       slist = slist ->next , i ++ )
  {
    s = slist ->value ;
    slist ->value = NULL ;
    str_array[i] = s ;
  }

  xlist_clean ( &string_list ) ;
  return str_array ;
}
void xstrsplit_free (char ** xarray)
{
  int i;

  XNULL ( xarray ) ;

  for (i = 0; xarray[i] != NULL; i++)
  {
    xfree (xarray[i]);
  }

  xfree (xarray);
}
