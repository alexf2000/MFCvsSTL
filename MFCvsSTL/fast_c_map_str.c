// fast_c_map_str
// A simple and fast associative-array library for C
//
// Copyright (c) 2013 by Alex Fedorov
// alexf2000@yandex.ru http://alexf.name
//

#include "stdafx.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "fast_c_map_str.h"

#pragma warning(disable : 4996)
#pragma comment(linker, "/STACK:20000000")

static int s_aBitmasks[8] = {0,01,03,07,017,037,077,0177};
static int s_aMaxElems[8] = {0,(2),(2*2),(2*2*2),(2*2*2*2),(2*2*2*2*2),(2*2*2*2*2*2),(2*2*2*2*2*2*2)};

struct fast_c_map_strings * map_new(int nLetters, int nBits, int nMemBuffSize)
{
    int i = sizeof(struct fast_c_map_strings);
    struct fast_c_map_strings *m = (struct fast_c_map_strings *)malloc(i);
	m->m_nMemUsed = i;
	m->m_pMemBuf = NULL;
	if (0 != nMemBuffSize)
	{
		fast_c_map_mem_newbiff(m, nMemBuffSize);
	}
	m->m_pInternals = map_int_new(nLetters, nBits, m);
    return m;
}

int map_del(struct fast_c_map_strings * m)
{
    map_int_del(m->m_pInternals, m);
	fast_c_map_mem_purge(m->m_pMemBuf);
    free(m);
    return 1;
}
int map_set(struct fast_c_map_strings * m, char * name, char * value)
{
    int nRet = 0;
    nRet = map_int_set(m->m_pInternals, name, value, m);
    return nRet;
}

char *map_get(struct fast_c_map_strings *m, char *name)
{
    char * ret = map_int_get(m->m_pInternals, name, m);
    return ret;
}

struct fast_c_map_item * map_find_next_item(struct fast_c_map_strings * pMap, struct fast_c_map_item * pItem)
{
    int bMatch = 0;
    struct fast_c_map_item * ret = map_int_find_next_item(pMap->m_pInternals, pItem, &bMatch);
    return ret;
}

/*--------------------------------------------*/

static int fast_c_map_inernal_size_struc(int nBits)
{
	int i = 0;
	if (2 == nBits) i = sizeof(struct fast_c_map_inernal_str2);
	if (3 == nBits) i = sizeof(struct fast_c_map_inernal_str3);
	if (4 == nBits) i = sizeof(struct fast_c_map_inernal_str4);
	if (5 == nBits) i = sizeof(struct fast_c_map_inernal_str5);
	if (6 == nBits) i = sizeof(struct fast_c_map_inernal_str6);
	if (7 == nBits) i = sizeof(struct fast_c_map_inernal_str7);
	return i;
}

struct fast_c_map_inernal_str * map_int_new(int nLetters, int nBits, fast_c_map_strings * map)
{
	int i = fast_c_map_inernal_size_struc(nBits);
	struct fast_c_map_inernal_str *m = (struct fast_c_map_inernal_str *)fast_c_map_mem_alloc(map, i);
	map->m_nMemUsed += i;
    m->m_nLetters = nLetters;
    m->m_nBits = nBits;
    m->m_pItems = NULL;
    for (i = 0; i < s_aMaxElems[m->m_nBits]; i++)
    {
        m->m_subItems[i] = NULL;
    }
    return m;
}

int map_int_del(struct fast_c_map_inernal_str * m, fast_c_map_strings * map)
{
    int i = 0;
    for (i = 0; i < s_aMaxElems[m->m_nBits]; i++)
    {
        if (m->m_subItems[i])
			map_int_del(m->m_subItems[i], map);
    }
    if (m->m_pItems)
		map_item_del(m->m_pItems, map);
	i = fast_c_map_inernal_size_struc(m->m_nBits);
	map->m_nMemUsed -= i;
	fast_c_map_mem_fre(map, m);
    return 1;
}

int map_int_set(struct fast_c_map_inernal_str *m, char *name, char *value, fast_c_map_strings * map)
{
    int nMem = strlen(value) + 1;
	struct fast_c_map_item * item = map_find_item(m, name, 0, map);
    if (NULL != item->value)
    {
		map->m_nMemUsed -= (strlen(item->value) + 1);
		fast_c_map_mem_fre(map, item->value);
        item->value = NULL;
    }
    if (NULL != value)
    {
		item->value = (char *)fast_c_map_mem_alloc(map, nMem);
		map->m_nMemUsed += nMem;
        strcpy(item->value, value);
    }
    return 1;
}

char *map_int_get(struct fast_c_map_inernal_str *m, char *name, fast_c_map_strings * map)
{
	struct fast_c_map_item * item = map_find_item(m, name, 0, map);
    if (NULL == item)
        return NULL;
    return item->value;
}

/*-------------------------------------------------------------------*/

struct fast_c_map_item *map_item_new(char * name, fast_c_map_strings * map)
{
    int nMem = sizeof(struct fast_c_map_item);
	struct fast_c_map_item * i = (struct fast_c_map_item *)fast_c_map_mem_alloc(map, nMem);
	map->m_nMemUsed += nMem;
    i->m_nxt = NULL;
    nMem = strlen(name) + 1;
	i->name = (char *)fast_c_map_mem_alloc(map, nMem);
	map->m_nMemUsed += nMem;
    strcpy(i->name, name);
    i->value = NULL;
    return i;
}

void map_item_del(struct fast_c_map_item * m, fast_c_map_strings * map)
{
    if (m->m_nxt)
		map_item_del(m->m_nxt, map);
    if (m->name)
    {
		map->m_nMemUsed -= (strlen(m->name) + 1);
		fast_c_map_mem_fre(map, m->name);
    }
    if (m->value)
    {
		map->m_nMemUsed -= (strlen(m->value) + 1);
		fast_c_map_mem_fre(map, m->value);
    }
	fast_c_map_mem_fre(map, m);
	map->m_nMemUsed -= sizeof(struct fast_c_map_item);
}

struct fast_c_map_item * map_int_find_next_item(struct fast_c_map_inernal_str * pMap, struct fast_c_map_item * pItem, int * bMatch)
{
    int i = 0;
    struct fast_c_map_item * itm = pMap->m_pItems;
    while (NULL != itm)
    {
        if (NULL == pItem)
            return itm;
        if (1 == *bMatch)
            return itm;
        if (itm == pItem)
        {
            *bMatch = 1;
        }
        itm = itm->m_nxt;
    }
    for (i = 0; i < s_aMaxElems[pMap->m_nBits]; i++)
    {
        struct fast_c_map_inernal_str * sub = pMap->m_subItems[i];
        if (NULL != sub)
        {
            itm = map_int_find_next_item(sub, pItem, bMatch);
            if (NULL != itm)
                return itm;
        }
    }
    return NULL;
}

struct fast_c_map_item * map_find_item(struct fast_c_map_inernal_str *m, char *name, int nRecurs, fast_c_map_strings * map)
{
    struct fast_c_map_item * item = NULL;
    struct fast_c_map_item * igood = NULL;
    struct fast_c_map_item * iret = NULL;
    unsigned char c = name[nRecurs] & s_aBitmasks[m->m_nBits];
    if ((0 != c) && (m->m_nLetters > nRecurs))
    {
        struct fast_c_map_inernal_str * sub = m->m_subItems[c];
        if (NULL == sub)
        {
			sub = map_int_new(m->m_nLetters, m->m_nBits, map);
            m->m_subItems[c] = sub;
        }
		return map_find_item(sub, name, nRecurs + 1, map);
    }
    item = m->m_pItems;
    while (NULL != item)
    {
        char * nm = item->name;
        if (0 == strcmp(nm, name))
            return item;
        igood = item;
        item = item->m_nxt;
    }
	iret = map_item_new(name, map);
    if (igood)
        igood->m_nxt = iret;
    else
        m->m_pItems = iret;
    return iret;
}

void fast_c_map_mem_newbiff(fast_c_map_strings * map, int nSize)
{
	void * pMem = malloc(nSize);
	fast_c_map_int_membuf * pMemBuf = (fast_c_map_int_membuf *)pMem;
	pMemBuf->m_nSize = nSize;
	pMemBuf->m_nCurr = sizeof(fast_c_map_int_membuf);
	pMemBuf->m_pMem = pMem;
	pMemBuf->m_pPrev = NULL;
	if (map->m_pMemBuf)
		pMemBuf->m_pPrev = map->m_pMemBuf;
	map->m_pMemBuf = pMemBuf;
}

void * fast_c_map_mem_alloc(fast_c_map_strings * map, int nSize)
{
	if (!map->m_pMemBuf)
		return malloc(nSize);
	int nAvail = map->m_pMemBuf->m_nSize - map->m_pMemBuf->m_nCurr;
	if (nAvail < nSize)
	{
		fast_c_map_mem_newbiff(map, map->m_pMemBuf->m_nSize);
		nAvail = map->m_pMemBuf->m_nSize - map->m_pMemBuf->m_nCurr;
	}
	char * ret = (char*)map->m_pMemBuf->m_pMem + map->m_pMemBuf->m_nCurr;
	map->m_pMemBuf->m_nCurr += nSize;
	return ret;
}

void fast_c_map_mem_fre(fast_c_map_strings * map, void * ptr)
{
	if (map->m_pMemBuf)
		return;
	free(ptr);
}

void fast_c_map_mem_purge(fast_c_map_int_membuf * buf)
{
	while (buf)
	{
		if (buf->m_pPrev)
			fast_c_map_mem_purge(buf->m_pPrev);
		free(buf->m_pMem);
		break;
	}
}
