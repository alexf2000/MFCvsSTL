// fast_c_map_str
// A simple and fast associative-array library for C
//
// Copyright (c) 2013 by Alex Fedorov
// alexf2000@yandex.ru http://alexf.name
//

#include "stdafx.h"
#include <malloc.h>
#include "fast_c_map_str.h"

#pragma warning(disable : 4996)
#pragma comment(linker, "/STACK:20000000")

static int s_aBitmasks[9] = {0,01,03,07,017,037,077,0177,0377};
static int s_aMaxElems[9] = {0,(2),(2*2),(2*2*2),(2*2*2*2),(2*2*2*2*2),(2*2*2*2*2*2),(2*2*2*2*2*2*2),(2*2*2*2*2*2*2*2)};

struct fast_c_map_strings * map_new(int nLetters, int nBits, int nMemBuffSize)
{
	int i = sizeof(struct fast_c_map_strings);
	struct fast_c_map_strings * m = (struct fast_c_map_strings *)malloc(i);
	m->m_nMemUsed = i;
	m->m_nMemWast = 0;
	m->m_pMemBuf = NULL;
	if (0 != nMemBuffSize)
	{
		fast_c_map_mem_newbiff(m, nMemBuffSize);
	}
	m->m_pInternals = map_int_new(m, nLetters, nBits);
	i = sizeof(unsigned char[256]);
	memset(m->m_mdict, 0, i);
	m->m_nmCurDic = 1;
	return m;
}

int map_del(struct fast_c_map_strings * map)
{
	if (!map->m_pMemBuf)
		map_int_del(map, map->m_pInternals);
	fast_c_map_mem_purge(map->m_pMemBuf);
	free(map);
	return 1;
}

int map_upd(struct fast_c_map_strings * map, const char *name, const char * strNew, int(*fnPtr)(struct fast_c_map_strings *, struct fast_c_map_item *, const char *))
{
	struct fast_c_map_internal_str *m = map->m_pInternals;
	struct fast_c_map_item * item = map_find_item(map, m, name, 0);
	return fnPtr(map, item, strNew);
}

int map_set(struct fast_c_map_strings * map, const char * name, const char * value)
{
	struct fast_c_map_internal_str *m = map->m_pInternals;
	int nMem = strlen(value) + 1;
	struct fast_c_map_item * item = map_find_item(map, m, name, 0);
	if (NULL != item->value)
	{
		int n = strlen(item->value) + 1;
		fast_c_map_mem_fre(map, item->value, n);
		item->value = NULL;
	}
	if (NULL != value)
	{
		item->value = (char *)fast_c_map_mem_alloc(map, nMem);
		strcpy(item->value, value);
	}
	return 1;
}


char *map_get(struct fast_c_map_strings *map, const char *name)
{
	struct fast_c_map_internal_str *m = map->m_pInternals;
	struct fast_c_map_item * item = map_find_item(map, m, name, 0);
	if (NULL == item)
		return NULL;
	return item->value;
}

struct fast_c_map_item * map_find_next_item(struct fast_c_map_strings * pMap, struct fast_c_map_item * pItem, struct fast_c_map_internal_str ** pHint)
{
	int bMatch = 0;
	struct fast_c_map_item * ret = map_int_find_next_item(pMap->m_pInternals, pItem, &bMatch, pHint);
	return ret;
}

/*--------------------------------------------*/

static int fast_c_map_internal_size_struc(int nBits)
{
	int i = 0;
	switch (nBits)
	{
	case 2:
		i = sizeof(struct fast_c_map_internal_str2);
		break;
	case 3:
		i = sizeof(struct fast_c_map_internal_str3);
		break;
	case 4:
		i = sizeof(struct fast_c_map_internal_str4);
		break;
	case 5:
		i = sizeof(struct fast_c_map_internal_str5);
		break;
	case 6:
		i = sizeof(struct fast_c_map_internal_str6);
		break;
	case 7:
		i = sizeof(struct fast_c_map_internal_str7);
		break;
	case 8:
		i = sizeof(struct fast_c_map_internal_str8);
		break;
	}
	return i;
}

struct fast_c_map_internal_str * map_int_new(struct fast_c_map_strings * map, int nLetters, int nBits)
{
	int i = fast_c_map_internal_size_struc(nBits);
	struct fast_c_map_internal_str *m = (struct fast_c_map_internal_str *)fast_c_map_mem_alloc(map, i);
	m->m_nLetters = nLetters;
	m->m_nBits = nBits;
	m->m_pItems = NULL;
	i = sizeof(struct fast_c_map_internal_str *) * s_aMaxElems[m->m_nBits];
	memset(m->m_subItems, 0, i);
	i = sizeof(unsigned char[FAST_C_MAP_DICT_SIZE]);
	memset(m->m_dict, 0, i);
	m->m_nCurDic = 1;
	return m;
}

int map_int_del(struct fast_c_map_strings * map, struct fast_c_map_internal_str * m)
{
	int i = 0;
	for (i = 0; i < s_aMaxElems[m->m_nBits]; i++)
	{
		if (m->m_subItems[i])
			map_int_del(map, m->m_subItems[i]);
	}
	if (m->m_pItems)
		map_item_del(map, m->m_pItems);
	i = fast_c_map_internal_size_struc(m->m_nBits);
	fast_c_map_mem_fre(map, m, i);
	return 1;
}

/*-------------------------------------------------------------------*/

struct fast_c_map_item *map_item_new(const char * name, struct fast_c_map_strings * map)
{
	int nMem = sizeof(struct fast_c_map_item);
	struct fast_c_map_item * i = (struct fast_c_map_item *)fast_c_map_mem_alloc(map, nMem);
	i->m_nxt = NULL;
	nMem = strlen(name) + 1;
	i->name = (char *)fast_c_map_mem_alloc(map, nMem);
	strcpy(i->name, name);
	i->value = NULL;
	return i;
}

inline void map_item_del(struct fast_c_map_strings * map, struct fast_c_map_item * m)
{
	if (m->m_nxt)
		map_item_del(map, m->m_nxt);
	if (m->name)
	{
		int n = strlen(m->name) + 1;
		fast_c_map_mem_fre(map, m->name, n);
	}
	if (m->value)
	{
		int n = strlen(m->value) + 1;
		fast_c_map_mem_fre(map, m->value, n);
	}
	fast_c_map_mem_fre(map, m, sizeof(struct fast_c_map_item));
}

struct fast_c_map_item * map_int_find_next_item(struct fast_c_map_internal_str * pMap, struct fast_c_map_item * pItem, int * bMatch, struct fast_c_map_internal_str ** pHint)
{
	if (pItem)
	{
		if (pItem->m_nxt)
			return pItem->m_nxt;
	}
	int i = 0;
	struct fast_c_map_item * itm = pMap->m_pItems;
	while (NULL != itm)
	{
		if (NULL == pItem)
		{
			(*pHint) = pMap;
			return itm;
		}
		if (1 == *bMatch)
		{
			(*pHint) = pMap;
			return itm;
		}
		if (itm == pItem)
		{
			*bMatch = 1;
		}
		itm = itm->m_nxt;
	}
	if (*pHint)
	{
		for (i = 0; i < s_aMaxElems[(*pHint)->m_nBits]; i++)
		{
			struct fast_c_map_internal_str * sub = (*pHint)->m_subItems[i];
			if (NULL != sub)
			{
				struct fast_c_map_internal_str * hh = NULL;
				int mm = 1;
				itm = map_int_find_next_item(sub, pItem, &mm, &hh);
				if (NULL != itm)
				{
					if (hh) (*pHint) = hh;
					return itm;
				}
			}
		}
	}
	(*pHint) = NULL;
	for (i = 0; i < s_aMaxElems[pMap->m_nBits]; i++)
	{
		struct fast_c_map_internal_str * sub = pMap->m_subItems[i];
		if (NULL != sub)
		{
			itm = map_int_find_next_item(sub, pItem, bMatch, pHint);
			if (NULL != itm)
			{
				if (NULL == (*pHint))
					(*pHint) = pMap;
				return itm;
			}
		}
	}
	return NULL;
}

inline unsigned char map_mdict_index(struct fast_c_map_strings * m, unsigned char c)
{
	unsigned char x = 0;
	if (0 == c)
		return x;
	x = m->m_mdict[c];
	if (0 == x)
	{
		m->m_mdict[c] = m->m_nmCurDic;
		x = m->m_nmCurDic;
		m->m_nmCurDic++;
	}
	x = x & 0177;
	return x;
}

inline unsigned char map_dict_index(struct fast_c_map_internal_str * m, unsigned char c)
{
	unsigned char x = 0;
	if (0 == c)
		return x;
	x = m->m_dict[c];
	if (0 == x)
	{
		m->m_dict[c] = m->m_nCurDic;
		x = m->m_nCurDic;
		m->m_nCurDic++;
	}
	x = x & s_aBitmasks[m->m_nBits];
	return x;
}

struct fast_c_map_item * map_find_item(struct fast_c_map_strings * map, struct fast_c_map_internal_str *m, const char *name, int nRecurs)
{
	struct fast_c_map_item * item = NULL;
	struct fast_c_map_item * igood = NULL;
	struct fast_c_map_item * iret = NULL;
	unsigned char c = map_mdict_index(map, name[nRecurs]);
	c = map_dict_index(m, c);
	if ((0 != c) && (m->m_nLetters > nRecurs))
	{
		struct fast_c_map_internal_str * sub = m->m_subItems[c];
		if (NULL == sub)
		{
			sub = map_int_new(map, m->m_nLetters, m->m_nBits);
			m->m_subItems[c] = sub;
		}
		return map_find_item(map, sub, name, nRecurs + 1);
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

void fast_c_map_mem_newbiff(struct fast_c_map_strings * map, int nSize)
{
	void * pMem = malloc(nSize);
	struct fast_c_map_int_membuf * pMemBuf = (struct fast_c_map_int_membuf *)pMem;
	pMemBuf->m_nSize = nSize;
	pMemBuf->m_nCurr = sizeof(struct fast_c_map_int_membuf);
	pMemBuf->m_pMem = pMem;
	pMemBuf->m_pPrev = NULL;
	if (map->m_pMemBuf)
		pMemBuf->m_pPrev = map->m_pMemBuf;
	map->m_pMemBuf = pMemBuf;
}

void * fast_c_map_mem_alloc(struct fast_c_map_strings * map, int nSize)
{
	map->m_nMemUsed += nSize;
	if (!map->m_pMemBuf)
		return malloc(nSize);
	if (nSize > map->m_pMemBuf->m_nSize)
		return NULL; // ERROR!!!
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

void fast_c_map_mem_fre(struct fast_c_map_strings * map, void * ptr, int nSize)
{
	map->m_nMemUsed -= nSize;
	map->m_nMemWast += nSize;
	if (map->m_pMemBuf)
	{
		return;
	}
	free(ptr);
}

void fast_c_map_mem_purge(struct fast_c_map_int_membuf * buf)
{
	while (buf)
	{
		if (buf->m_pPrev)
			fast_c_map_mem_purge(buf->m_pPrev);
		free(buf->m_pMem);
		break;
	}
}

int fast_c_map_mem_rlc(struct fast_c_map_item * item, void * vmap2)
{
	struct fast_c_map_strings * map2 = (struct fast_c_map_strings *)vmap2;
	map_set(map2, item->name, item->value);
	return 1;
}

struct fast_c_map_strings * fast_c_map_mem_realloc(struct fast_c_map_strings * map)
{
	int nMemSize = 0;
	if (map->m_pMemBuf)
		nMemSize = map->m_pMemBuf->m_nSize;
	struct fast_c_map_strings * map2 = map_new(map->m_pInternals->m_nLetters, map->m_pInternals->m_nBits, nMemSize);
	map_foreach(map->m_pInternals, map2, fast_c_map_mem_rlc);
	map_del(map);
	return map2;
}

int map_foreach(struct fast_c_map_internal_str * pMap, void * param, int(*fnPtr)(struct fast_c_map_item *, void *))
{
	int i = 0;
	struct fast_c_map_item * itm = pMap->m_pItems;
	while (NULL != itm)
	{
		fnPtr(itm, param);
		itm = itm->m_nxt;
	}
	for (i = 0; i < s_aMaxElems[pMap->m_nBits]; i++)
	{
		struct fast_c_map_internal_str * sub = pMap->m_subItems[i];
		if (NULL != sub)
			map_foreach(sub, param, fnPtr);
	}
	return 1;
}
