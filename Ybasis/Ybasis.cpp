#include "Ybasis.h"

void* Ymalloc(IN u_32 size)
{
	void *p = NULL;
	if (size > YINT_MAX)
		return NULL;
	else {
		p = malloc(size);
		if (!p)
			return NULL;
		memset(p, 0, size);
	}
	return p;
}

void Yfree(IN void *p)
{
	if (p) free(p);
}

unsigned int Ystr_hash(IN const char* str)
{  
	const char* _str = str;
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc.. 37  
	unsigned int hash = 0;
	while (*_str)
		hash = hash * seed + (*_str++);
	return hash;
}

char * Ystrdup (IN const char * str )
{
  char * ret  = NULL ;
  
  if ( !str ) return NULL ;
  
  ret = Ymalloc( (unsigned int)strlen(str) );
  strcpy (ret, str);
  
  return ret ;
}

char * Ystrndup (IN char * str , int n )
{
  char * ret  = NULL;
  
  if ( !str ) return NULL;

  ret = Ymalloc(n + 1);
  strncpy(ret, str, n);
  
  return ret ;
}

void * Ymemdup (IN void * mem , int n )
{
  void * ret  = NULL ;
  
  if (!mem) return NULL;
  
  ret = Ymalloc(n + 1);
  memcpy (ret, mem, n);
  return ret;
}

int Ystrcmp (IN const char * a , IN const char * b )
{
  if ( !a )
  {
    if ( !b )
    {
      return 0 ;
    }
    else
    {
      return -1 ;
    }
  }
  else
  {
    if ( !b )
    {
      return 1 ;
    }
    return strcmp(a, b) ;
  }
}


