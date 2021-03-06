#ifndef __H_XTREAP_H__
#define __H_XTREAP_H__
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#include "afx.h"
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
typedef struct _xtreap_node
{
  int size ;
  int key, fix ;
  void * value ;
  struct _xtreap_node * ch[2] ;
} xtreap_node ;
typedef struct _xtreap
{
  xtreap_node * root ;
} xtreap ;
////////////////////////////////////////////////////////////////////////////
void * xtreap_node_push(xtreap_node ** pt ,int key, void * value) ;
void * xtreap_node_find(xtreap_node * t, int key) ;
void xtreap_node_find_range(xtreap_node * t, int keyl, int keyr, void * obj, void (* callback) (void * obj , void * value)) ;
void * xtreap_node_break(xtreap_node ** pt, int key) ;
////////////////////////////////////////////////////////////////////////////
xtreap * xtreap_alloc () ;
void xtreap_free ( xtreap ** pxt ) ;
#define xtreap_push(XT,K,V) xtreap_node_push(&(XT) ->root, K, V)
#define xtreap_find(XT,K) xtreap_node_find((XT) ->root,K)
#define xtreap_break(XT,K) xtreap_node_break(&(XT) ->root,K)
#define xtreap_break_byref(XT,REF) xtreap_node_break(&(XT) ->root,((xtreap_node *)REF) ->key)
#define xtreap_find_range(XT,L,R,OBJ,CALLBACK) xtreap_node_find_range((XT) ->root,L,R,OBJ,CALLBACK)
////////////////////////////////////////////////////////////////////////////
void xtreap_print ( xtreap * xt ) ;
void xtreap_test() ;
////////////////////////////////////////////////////////////////////////////
#endif