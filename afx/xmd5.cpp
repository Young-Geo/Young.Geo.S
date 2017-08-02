#include "xmd5.h"

//////////////////////////////////////////////////////////////////////////
typedef struct _xmd5
{
  unsigned int A,B,C,D;
  //
  int S11, S21, S31, S41;
  int S12, S22, S32, S42;
  int S13, S23, S33, S43;
  int S14, S24, S34, S44;
  //
  int AppendByte;
  //
  unsigned char MsgLen[8];
  //
  char Result[33] ;
} xmd5 ;
//////////////////////////////////////////////////////////////////////////
void xstrupper ( char * str )
{
  char * index = str ;

  XNULL ( index ) ;
  while ( *index )
  {
    if ( ( *index >= 'a' ) && ( *index <= 'z' ) )
    {
      *index -= 'a' - 'A' ;
    }
    index ++ ;
  }
  return ;
}

unsigned int ROTATE_LEFT(unsigned int x,unsigned int n)
{
  return (((x) << (n)) | ((x) >> (32-(n))));
}

unsigned int F(unsigned int x,unsigned int y,unsigned int z)
{
  return ((x & y) | ((~x) & z));
}
unsigned int G(unsigned int x,unsigned int y,unsigned int z)
{
  return ((x & z) | (y & (~z)));
}

unsigned int H(unsigned int x,unsigned int y,unsigned int z)
{
  return x ^ y ^ z;
}

unsigned int I(unsigned int x,unsigned int y,unsigned int z)
{
  return (y ^ (x | (~z)));
}

void FF(unsigned int * a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,int s,unsigned int ac)
{
  (*a) += F ((b), (c), (d)) + (x) + (ac);
  (*a) = ROTATE_LEFT ((*a), (s));
  (*a) += (b);
}

void GG(unsigned int * a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,int s,unsigned int ac)
{
  (*a) += G ((b), (c), (d)) + (x) + (ac);
  (*a) = ROTATE_LEFT ((*a), (s));
  (*a) += (b);
}

void HH(unsigned int * a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,int s,unsigned int ac)
{
  (*a) += H ((b), (c), (d)) + (x) + (ac);
  (*a) = ROTATE_LEFT ((*a), (s));
  (*a) += (b);
}

void II(unsigned int * a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,int s,unsigned int ac)
{
  (*a) += I ((b), (c), (d)) + (x) + (ac);
  (*a) = ROTATE_LEFT ((*a), (s));
  (*a) += (b);
}

void Init( xmd5 * md5 )
{
  memset ( md5 , 0 , sizeof ( xmd5 ) ) ;
  
  md5 ->S11 =  7;	md5 ->S21 =  5;	md5 ->S31 =  4;	md5 ->S41 =  6;
  md5 ->S12 = 12;	md5 ->S22 =  9;	md5 ->S32 = 11;	md5 ->S42 = 10;
  md5 ->S13 = 17;	md5 ->S23 = 14;	md5 ->S33 = 16;	md5 ->S43 = 15;
  md5 ->S14 = 22;	md5 ->S24 = 20;	md5 ->S34 = 23;	md5 ->S44 = 21;
  
  md5 ->A = 0x67452301;  // in memory, this is 0x01234567
  md5 ->B = 0xEFCDAB89;  // in memory, this is 0x89ABCDEF
  md5 ->C = 0x98BADCFE;  // in memory, this is 0xFEDCBA98
  md5 ->D = 0x10325476;  // in memory, this is 0x76543210
}

void Append( xmd5 * md5 , unsigned int MsgLen)
{
  int m = MsgLen % 64;
  
  int hWord=(MsgLen & 0xFFFF0000) >> 16;
  int lWord=MsgLen & 0x0000FFFF;
  
  int hDiv=hWord*8;
  int lDiv=lWord*8;
  
  if(m==0)
    md5 ->AppendByte=56;
  else if(m<56)
    md5 ->AppendByte=56-m;
  else
    md5 ->AppendByte=64-m+56;
  
  md5 ->MsgLen[0] = lDiv & 0xFF ;
  md5 ->MsgLen[1] = (lDiv >> 8) & 0xFF ;
  md5 ->MsgLen[2] = ((lDiv >> 16) & 0xFF) | (hDiv & 0xFF);
  md5 ->MsgLen[3] = (hDiv >> 8) & 0xFF ;
  md5 ->MsgLen[4] = (hDiv >> 16) & 0xFF ;
  md5 ->MsgLen[5] = (hDiv >> 24) & 0xFF ;
  md5 ->MsgLen[6] = 0;
  md5 ->MsgLen[7] = 0;
}

void Transform(xmd5 * md5 , unsigned char Block[64])
{
  unsigned long x[16];
  unsigned int a,b,c,d;
  int i , j ;
  
  for ( i = 0 , j = 0 ; j < 64 ; i++ , j += 4 )
    x[i]=Block[j] | Block[j+1] <<8 | Block[j+2] <<16 | Block[j+3] <<24 ;
  
  a=md5 ->A; b=md5 ->B; c=md5 ->C; d=md5 ->D;
  
  FF (&a, b, c, d, (unsigned int)x[ 0], md5 ->S11, 0xD76AA478); //  1
  FF (&d, a, b, c, (unsigned int)x[ 1], md5 ->S12, 0xE8C7B756); //  2
  FF (&c, d, a, b, (unsigned int)x[ 2], md5 ->S13, 0x242070DB); //  3
  FF (&b, c, d, a, (unsigned int)x[ 3], md5 ->S14, 0xC1BDCEEE); //  4
  FF (&a, b, c, d, (unsigned int)x[ 4], md5 ->S11, 0xF57C0FAF); //  5
  FF (&d, a, b, c, (unsigned int)x[ 5], md5 ->S12, 0x4787C62A); //  6
  FF (&c, d, a, b, (unsigned int)x[ 6], md5 ->S13, 0xA8304613); //  7
  FF (&b, c, d, a, (unsigned int)x[ 7], md5 ->S14, 0xFD469501); //  8
  FF (&a, b, c, d, (unsigned int)x[ 8], md5 ->S11, 0x698098D8); //  9
  FF (&d, a, b, c, (unsigned int)x[ 9], md5 ->S12, 0x8B44F7AF); // 10
  FF (&c, d, a, b, (unsigned int)x[10], md5 ->S13, 0xFFFF5BB1); // 11
  FF (&b, c, d, a, (unsigned int)x[11], md5 ->S14, 0x895CD7BE); // 12
  FF (&a, b, c, d, (unsigned int)x[12], md5 ->S11, 0x6B901122); // 13
  FF (&d, a, b, c, (unsigned int)x[13], md5 ->S12, 0xFD987193); // 14
  FF (&c, d, a, b, (unsigned int)x[14], md5 ->S13, 0xA679438E); // 15
  FF (&b, c, d, a, (unsigned int)x[15], md5 ->S14, 0x49B40821); // 16
  
  GG (&a, b, c, d, (unsigned int)x[ 1], md5 ->S21, 0xF61E2562); // 17
  GG (&d, a, b, c, (unsigned int)x[ 6], md5 ->S22, 0xC040B340); // 18
  GG (&c, d, a, b, (unsigned int)x[11], md5 ->S23, 0x265E5A51); // 19
  GG (&b, c, d, a, (unsigned int)x[ 0], md5 ->S24, 0xE9B6C7AA); // 20
  GG (&a, b, c, d, (unsigned int)x[ 5], md5 ->S21, 0xD62F105D); // 21
  GG (&d, a, b, c, (unsigned int)x[10], md5 ->S22,  0x2441453); // 22
  GG (&c, d, a, b, (unsigned int)x[15], md5 ->S23, 0xD8A1E681); // 23
  GG (&b, c, d, a, (unsigned int)x[ 4], md5 ->S24, 0xE7D3FBC8); // 24
  GG (&a, b, c, d, (unsigned int)x[ 9], md5 ->S21, 0x21E1CDE6); // 25
  GG (&d, a, b, c, (unsigned int)x[14], md5 ->S22, 0xC33707D6); // 26
  GG (&c, d, a, b, (unsigned int)x[ 3], md5 ->S23, 0xF4D50D87); // 27
  GG (&b, c, d, a, (unsigned int)x[ 8], md5 ->S24, 0x455A14ED); // 28
  GG (&a, b, c, d, (unsigned int)x[13], md5 ->S21, 0xA9E3E905); // 29
  GG (&d, a, b, c, (unsigned int)x[ 2], md5 ->S22, 0xFCEFA3F8); // 30
  GG (&c, d, a, b, (unsigned int)x[ 7], md5 ->S23, 0x676F02D9); // 31
  GG (&b, c, d, a, (unsigned int)x[12], md5 ->S24, 0x8D2A4C8A); // 32
  
  HH (&a, b, c, d, (unsigned int)x[ 5], md5 ->S31, 0xFFFA3942); // 33
  HH (&d, a, b, c, (unsigned int)x[ 8], md5 ->S32, 0x8771F681); // 34
  HH (&c, d, a, b, (unsigned int)x[11], md5 ->S33, 0x6D9D6122); // 35
  HH (&b, c, d, a, (unsigned int)x[14], md5 ->S34, 0xFDE5380C); // 36
  HH (&a, b, c, d, (unsigned int)x[ 1], md5 ->S31, 0xA4BEEA44); // 37
  HH (&d, a, b, c, (unsigned int)x[ 4], md5 ->S32, 0x4BDECFA9); // 38
  HH (&c, d, a, b, (unsigned int)x[ 7], md5 ->S33, 0xF6BB4B60); // 39
  HH (&b, c, d, a, (unsigned int)x[10], md5 ->S34, 0xBEBFBC70); // 40
  HH (&a, b, c, d, (unsigned int)x[13], md5 ->S31, 0x289B7EC6); // 41
  HH (&d, a, b, c, (unsigned int)x[ 0], md5 ->S32, 0xEAA127FA); // 42
  HH (&c, d, a, b, (unsigned int)x[ 3], md5 ->S33, 0xD4EF3085); // 43
  HH (&b, c, d, a, (unsigned int)x[ 6], md5 ->S34,  0x4881D05); // 44
  HH (&a, b, c, d, (unsigned int)x[ 9], md5 ->S31, 0xD9D4D039); // 45
  HH (&d, a, b, c, (unsigned int)x[12], md5 ->S32, 0xE6DB99E5); // 46
  HH (&c, d, a, b, (unsigned int)x[15], md5 ->S33, 0x1FA27CF8); // 47
  HH (&b, c, d, a, (unsigned int)x[ 2], md5 ->S34, 0xC4AC5665); // 48
  
  II (&a, b, c, d, (unsigned int)x[ 0], md5 ->S41, 0xF4292244); // 49
  II (&d, a, b, c, (unsigned int)x[ 7], md5 ->S42, 0x432AFF97); // 50
  II (&c, d, a, b, (unsigned int)x[14], md5 ->S43, 0xAB9423A7); // 51
  II (&b, c, d, a, (unsigned int)x[ 5], md5 ->S44, 0xFC93A039); // 52
  II (&a, b, c, d, (unsigned int)x[12], md5 ->S41, 0x655B59C3); // 53
  II (&d, a, b, c, (unsigned int)x[ 3], md5 ->S42, 0x8F0CCC92); // 54
  II (&c, d, a, b, (unsigned int)x[10], md5 ->S43, 0xFFEFF47D); // 55
  II (&b, c, d, a, (unsigned int)x[ 1], md5 ->S44, 0x85845DD1); // 56
  II (&a, b, c, d, (unsigned int)x[ 8], md5 ->S41, 0x6FA87E4F); // 57
  II (&d, a, b, c, (unsigned int)x[15], md5 ->S42, 0xFE2CE6E0); // 58
  II (&c, d, a, b, (unsigned int)x[ 6], md5 ->S43, 0xA3014314); // 59
  II (&b, c, d, a, (unsigned int)x[13], md5 ->S44, 0x4E0811A1); // 60
  II (&a, b, c, d, (unsigned int)x[ 4], md5 ->S41, 0xF7537E82); // 61
  II (&d, a, b, c, (unsigned int)x[11], md5 ->S42, 0xBD3AF235); // 62
  II (&c, d, a, b, (unsigned int)x[ 2], md5 ->S43, 0x2AD7D2BB); // 63
  II (&b, c, d, a, (unsigned int)x[ 9], md5 ->S44, 0xEB86D391); // 64
  
  md5 ->A+=a; md5 ->B+=b; md5 ->C+=c; md5 ->D+=d;
}

/*********************************************
 *********************************************/
char * ToHex( xmd5 * md5 , int UpperCase)
{
  int ResultArray[4]={md5 ->A,md5 ->B,md5 ->C,md5 ->D};
  char Buf[3]={0};
  int i ;
  
  for ( i = 0 ; i < 4 ; i ++ )
  {
    memset(Buf,0,3);
    sprintf(Buf,"%02x",ResultArray[i] & 0x00FF);
    strcat ( md5 ->Result , Buf ) ;
    
    memset(Buf,0,3);
    sprintf(Buf,"%02x",(ResultArray[i] >>  8) & 0x00FF);
    strcat ( md5 ->Result , Buf ) ;
    
    memset(Buf,0,3);
    sprintf(Buf,"%02x",(ResultArray[i] >> 16) & 0x00FF);
    strcat ( md5 ->Result , Buf ) ;
    
    memset(Buf,0,3);
    sprintf(Buf,"%02x",(ResultArray[i] >> 24) & 0x00FF);
    strcat ( md5 ->Result , Buf ) ;
  }
  if(UpperCase) xstrupper(md5 ->Result);
  return md5 ->Result ;
}
//////////////////////////////////////////////////////////////////////////
char * GetMD5OfData(char * input , int inlen , int insize , char * output , int UpperCase )
{
  static char outputstatic[33] ;
  
  xmd5 md5 ;
  unsigned char x[64]={0};
  
  char * inptr = NULL ;
  char * outptr = NULL ;
  char * old_input = NULL ;
  
  int i , Index ;
  
  if ( !input || inlen <= 0 )
    return NULL ;
  
  if ( insize < (inlen + 128) )
  {
    old_input = input ;
    
    insize = inlen + 128 ;
    input = xmalloc ( insize) ;
    memcpy ( input , old_input , inlen ) ;
  }
  
  Init ( &md5 ) ;
  
  Append( &md5 , inlen );
  
  inptr = input + inlen ;
  for( i = 0 ; i < md5.AppendByte ; i++ )
  {
    if( i==0 ) *inptr = 0x80 ;
    else *inptr = 0x0;
    inptr ++ ;
  }
  
  for ( i = 0 ; i < 8 ; i ++ )
  {
    *inptr = md5.MsgLen[i] ;
    inptr ++ ;
  }
  inlen = (int)(inptr - input) ;
  
  for( i=0 , Index = -1 ; i < inlen ; i++)
  {
    x[++Index]=input[i];
    if(Index==63)
    {
      Index=-1;
      Transform(&md5 , x);
    }
  }
  
  outptr = ToHex(&md5 , UpperCase) ;
  if ( output )
  {
    strcpy ( output , outptr ) ;
    outptr = output ;
  }
  else
  {
    strcpy ( outputstatic , outptr ) ;
    outptr = outputstatic ;
  }
  
  if ( old_input )
    xfree ( input ) ;
  
  return outptr ;
}
//////////////////////////////////////////////////////////////////////////
char * GetMD5OfFile( char * FileName , char * output , int UpperCase)
{
  int filesize = 0 ;
  char * input = NULL ;
  int inlen = 0 ;
  
  char * outptr = NULL ;
  
  FILE * pfile = NULL ;
  
  filesize = file_size ( FileName ) ;
  if ( filesize <= 0 )
    return NULL ;
  
  pfile = fopen ( FileName , "rb" ) ;
  if  ( !pfile )
    return NULL ;
  
  inlen = filesize + 128 ;
  input = xmalloc ( inlen) ;
  
  if ( filesize != fread ( input , 1 , filesize , pfile ) )
    goto __finish ;
  
  outptr = GetMD5OfData ( input , filesize , inlen , NULL , UpperCase ) ;
  if ( output )
    strcpy ( output , outptr ) ;
  
__finish:
  if ( input )
    xfree ( input ) ;
  if ( pfile )
    fclose ( pfile ) ;
  
  return outptr ;
}
//////////////////////////////////////////////////////////////////////////
