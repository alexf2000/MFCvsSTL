// fast_c_map_str
// A simple and fast associative-array library for C
//
// Copyright (c) 2013 by Alex Fedorov
// alexf2000@yandex.ru http://alexf.name
//

#ifndef FAST_C_MAP_STR
#define FAST_C_MAP_STR

#pragma warning(disable : 4200)

#define FAST_C_MAP_DICT_SIZE (128)

/* Forward only membuf */
struct fast_c_map_int_membuf
{
	void * m_pMem;
	int m_nSize;
	int m_nCurr;
	struct fast_c_map_int_membuf * m_pPrev;
};

/* SINGLE ITEM */
struct fast_c_map_item
{
	char * name;
	char * value;
	/* private */
	struct fast_c_map_item * m_nxt;
};

/* MAP(ASSOCIATIVE ARRAY) OF STRINGS */
struct fast_c_map_strings
{
	struct fast_c_map_internal_str * m_pInternals;
	struct fast_c_map_int_membuf * m_pMemBuf;
	int m_nMemUsed;
	int m_nMemWast;
	/* dict */
	unsigned char m_mdict[256];
	unsigned char m_nmCurDic;
};

struct fast_c_map_internal_str
{
	/* dict */
	unsigned char m_dict[FAST_C_MAP_DICT_SIZE];
	unsigned char m_nCurDic;
	/* private */
	int m_nLetters;
	int m_nBits;
	struct fast_c_map_item * m_pItems;
	struct fast_c_map_internal_str * m_subItems[];
};

struct fast_c_map_internal_str2
{
	/* dict */
	unsigned char m_dict[FAST_C_MAP_DICT_SIZE];
	int m_nCurDic;
	/* private */
	int m_nLetters;
	int m_nBits;
	struct fast_c_map_item * m_pItems;
	struct fast_c_map_internal_str * m_subItems[(2*2)];
};

struct fast_c_map_internal_str3
{
	/* dict */
	unsigned char m_dict[FAST_C_MAP_DICT_SIZE];
	int m_nCurDic;
	/* private */
	int m_nLetters;
	int m_nBits;
	struct fast_c_map_item * m_pItems;
	struct fast_c_map_internal_str * m_subItems[(2*2*2)];
};

struct fast_c_map_internal_str4
{
	/* dict */
	unsigned char m_dict[FAST_C_MAP_DICT_SIZE];
	int m_nCurDic;
	/* private */
	int m_nLetters;
	int m_nBits;
	struct fast_c_map_item * m_pItems;
	struct fast_c_map_internal_str * m_subItems[(2*2*2*2)];
};

struct fast_c_map_internal_str5
{
	/* dict */
	unsigned char m_dict[FAST_C_MAP_DICT_SIZE];
	int m_nCurDic;
	/* private */
	int m_nLetters;
	int m_nBits;
	struct fast_c_map_item * m_pItems;
	struct fast_c_map_internal_str * m_subItems[(2*2*2*2*2)];
};

struct fast_c_map_internal_str6
{
	/* dict */
	unsigned char m_dict[FAST_C_MAP_DICT_SIZE];
	int m_nCurDic;
	/* private */
	int m_nLetters;
	int m_nBits;
	struct fast_c_map_item * m_pItems;
	struct fast_c_map_internal_str * m_subItems[(2*2*2*2*2*2)];
};

struct fast_c_map_internal_str7
{
	/* dict */
	unsigned char m_dict[FAST_C_MAP_DICT_SIZE];
	int m_nCurDic;
	/* private */
	int m_nLetters;
	int m_nBits;
	struct fast_c_map_item * m_pItems;
	struct fast_c_map_internal_str * m_subItems[(2*2*2*2*2*2*2)];
};

struct fast_c_map_internal_str8
{
	/* dict */
	unsigned char m_dict[FAST_C_MAP_DICT_SIZE];
	int m_nCurDic;
	/* private */
	int m_nLetters;
	int m_nBits;
	struct fast_c_map_item * m_pItems;
	struct fast_c_map_internal_str * m_subItems[(2*2*2*2*2*2*2*2)];
};

/* MAIN FUNCTIONS */
/* RETURNS MAP STRUCTIRE */
struct fast_c_map_strings * map_new(int nLetters, int nBits, int nMemBuffSize);
struct fast_c_map_internal_str * map_int_new(struct fast_c_map_strings * map, int nLetters, int nBits);
/* DELETES MAP STUCTURE AND FREES MEMORY */
int map_del(struct fast_c_map_strings * map);
int map_int_del(struct fast_c_map_strings * map, struct fast_c_map_internal_str * m);
/* SETS KEY AND VALUE, VALUE CAN BE NULL */
int map_set(struct fast_c_map_strings * map, const char *name, const char *value);
/* UPDATE ITEM VALUE */
int map_upd(struct fast_c_map_strings * map, const char *name, const char * strNew, int(*fnPtr)(struct fast_c_map_strings *, struct fast_c_map_item *, const char *));
/* RETURNS VALUE OF A KEY */
char *map_get(struct fast_c_map_strings *map, const char *name);
/* FINDS NEXT KEY->VALUE PAIR, TO FIND FIRST PAIR PASS NULL */
struct fast_c_map_item * map_find_next_item(struct fast_c_map_strings * pMap, struct fast_c_map_item * pItem, struct fast_c_map_internal_str ** pHint);
struct fast_c_map_item * map_int_find_next_item(struct fast_c_map_internal_str * pMap, struct fast_c_map_item * pItem, int * bMatch, struct fast_c_map_internal_str ** pHint);
/* PERFORM OPERATION ON EACH PAIR */
int map_foreach(struct fast_c_map_internal_str * pMap, void * param, int(*fnPtr)(struct fast_c_map_item *, void *));


/* UTILITIES */
inline void map_item_del(struct fast_c_map_strings * map, struct fast_c_map_item * m);
struct fast_c_map_item * map_find_item(struct fast_c_map_strings * map, struct fast_c_map_internal_str *m, const char *name, int nRecurs);

/* memory handling */
void * fast_c_map_mem_alloc(struct fast_c_map_strings * map, int nSize);
void fast_c_map_mem_fre(struct fast_c_map_strings * map, void * ptr, int nSize);
void fast_c_map_mem_purge(struct fast_c_map_int_membuf * buf);
void fast_c_map_mem_newbiff(struct fast_c_map_strings * map, int nSize);
struct fast_c_map_strings * fast_c_map_mem_realloc(struct fast_c_map_strings * map);

#endif
