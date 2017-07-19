#include "xprotitle.h"
//////////////////////////////////////////////////////////////////////////
static int argv_len ;
static char ** old_argv ;

//this value is define by gcc or system
extern char ** environ ;

/**
 * @brief Initializes the process setting variables.
 *
 * @param[in]   argc    Argc argument from main.
 * @param[in]   argv    Argv argument from main.
 */
void proctitle_init ( int argc , char * argv[] )
{
  int i = 0;
  char **envp = environ;
  
  if ( argv == NULL ) return;
  
  /* Move environ to new memory, to be able to reuse older one. */
  while ( envp[i] ) i ++ ;
  
  environ = (char **)xmalloc ( sizeof ( char * ) * ( i + 1 ) ) ;
  for ( i = 0 ; envp [i] ; i ++ )
    environ [i] = xstrdup ( envp [i] ) ;
  environ [i] = NULL;
  
  old_argv = argv ;
  if ( i > 0 )
    argv_len = (int)((envp [i - 1] + strlen ( envp [i - 1] )) - old_argv [0]) ;
  else
    argv_len = (int)((old_argv [argc - 1] + strlen ( old_argv [argc - 1] )) - old_argv [0]) ;
}

/**
 * @brief Sets the process' title.
 *
 * @param[in]   new_title   Format string for new process title.
 * @param[in]   args        Format string arguments variable list.
 */
void proctitle_set ( char * new_title )
{
  int i = 0 ;
  
  if ( ( old_argv == NULL )
      || !( new_title )
      || !( new_title[0] ) ) return ;
  
  new_title = xstrdup( new_title ) ;
  
  i = xstrlen ( new_title );
  if ( i > argv_len - 2 )
  {
    i = argv_len - 2 ;
    new_title [i] = '\0' ;
  }
  
  memset ( old_argv[0] , 0, argv_len ) ;
  strcpy ( old_argv[0] , new_title ) ;
  old_argv [1] = NULL ;
  
  xfree ( new_title ) ;
}
//////////////////////////////////////////////////////////////////////////
char ** dump_argv ( int argc , char * argv[] )
{
  char ** dumped_argv = NULL ;
  int i = 0 ;
  
  dumped_argv = (char **)xmalloc ( sizeof (char *) * argc + 1) ;
  for ( i = 0 ; i < argc ; i ++ )
    dumped_argv[i] = xstrdup( argv[i] ) ;
  dumped_argv[i] = NULL ;
  
  return dumped_argv ;
}
void free_argv ( char *argv[] )
{
  int i = 0 ;
  
  for ( i = 0 ; argv[i] ; i ++ )
    xfree ( argv[i] ) ;
  xfree ( argv ) ;
}
//////////////////////////////////////////////////////////////////////////
