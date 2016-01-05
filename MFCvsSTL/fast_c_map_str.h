// fast_c_map_str
// A simple and fast associative-array library for C
//
// Copyright (c) 2013 by Alex Fedorov
// alexf2000@yandex.ru http://alexf.name
//

#ifndef FAST_C_MAP_STR
#define FAST_C_MAP_STR

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
    int m_nMemUsed;
    struct fast_c_map_inernal_str * m_pInternals;
	struct fast_c_map_int_membuf * m_pMemBuf;
};

struct fast_c_map_inernal_str
{
    /* private */
    int m_nLetters;
    int m_nBits;
    struct fast_c_map_item * m_pItems;
    struct fast_c_map_inernal_str * m_subItems[];
};

struct fast_c_map_inernal_str2
{
    /* private */
    int m_nLetters;
    int m_nBits;
    struct fast_c_map_item * m_pItems;
    struct fast_c_map_inernal_str * m_subItems[(2*2)];
};

struct fast_c_map_inernal_str3
{
    /* private */
    int m_nLetters;
    int m_nBits;
    struct fast_c_map_item * m_pItems;
    struct fast_c_map_inernal_str * m_subItems[(2*2*2)];
};

struct fast_c_map_inernal_str4
{
    /* private */
    int m_nLetters;
    int m_nBits;
    struct fast_c_map_item * m_pItems;
    struct fast_c_map_inernal_str * m_subItems[(2*2*2*2)];
};

struct fast_c_map_inernal_str5
{
    /* private */
    int m_nLetters;
    int m_nBits;
    struct fast_c_map_item * m_pItems;
    struct fast_c_map_inernal_str * m_subItems[(2*2*2*2*2)];
};

struct fast_c_map_inernal_str6
{
    /* private */
    int m_nLetters;
    int m_nBits;
    struct fast_c_map_item * m_pItems;
    struct fast_c_map_inernal_str * m_subItems[(2*2*2*2*2*2)];
};

struct fast_c_map_inernal_str7
{
    /* private */
    int m_nLetters;
    int m_nBits;
    struct fast_c_map_item * m_pItems;
    struct fast_c_map_inernal_str * m_subItems[(2*2*2*2*2*2*2)];
};

/* MAIN FUNCTIONS */
/* RETURNS MAP STRUCTIRE */
struct fast_c_map_strings * map_new(int nLetters, int nBits, int nMemBuffSize);
struct fast_c_map_inernal_str * map_int_new(int nLetters, int nBits, fast_c_map_strings * map);
/* DELETES MAP STUCTURE AND FREES MEMORY */
int map_del(struct fast_c_map_strings * m);
int map_int_del(struct fast_c_map_inernal_str * m, fast_c_map_strings * map);
/* SETS KEY AND VALUE, VALUE CAN BE NULL */
int   map_set(struct fast_c_map_strings *m, char *name, char *value);
int   map_int_set(struct fast_c_map_inernal_str *m, char *name, char *value, fast_c_map_strings * map);
/* RETURNS VALUE OF A KEY */
char *map_get(struct fast_c_map_strings *m, char *name);
char *map_int_get(struct fast_c_map_inernal_str *m, char *name, fast_c_map_strings * map);
/* FINDS NEXT KEY->VALUE PAIR, TO FIND FIRST PAIR PASS NULL */
struct fast_c_map_item * map_find_next_item(struct fast_c_map_strings * pMap, struct fast_c_map_item * pItem);
struct fast_c_map_item * map_int_find_next_item(struct fast_c_map_inernal_str * pMap, struct fast_c_map_item * pItem, int * bMatch);


/* UTILITIES */
void map_item_del(struct fast_c_map_item * m, fast_c_map_strings * map);
struct fast_c_map_item * map_find_item(struct fast_c_map_inernal_str *m, char *name, int nRecurs, fast_c_map_strings * map);

/* memory handling */
void * fast_c_map_mem_alloc(fast_c_map_strings * map, int nSize);
void fast_c_map_mem_fre(fast_c_map_strings * map, void * ptr);
void fast_c_map_mem_purge(fast_c_map_int_membuf * buf);
void fast_c_map_mem_newbiff(fast_c_map_strings * map, int nSize);

#endif
