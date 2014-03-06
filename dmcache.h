/*
 * dmcache.h
 *
 *  Created on: Apr 17, 2012
 *      Author: kody
 */

#ifndef DMCACHE_H_
#define DMCACHE_H_

#include "simdatatypes.h"
#include "cache.h"
/*This structure will define operations on the direct mapped cache*/

typedef struct{
        boolean valid : 1;
        boolean dirty : 1;
        uint32 tag;
}__attribute__((__packed__))dmcacheblock;

/*Direct Mapped Functions*/
boolean createDMCache(cache_s* cache);
boolean deleteDMCache(cache_s* cache);
boolean checkDMCache(cache_s* cache, uint32 reference, boolean isWrite);
kickout addValueToDMCache(cache_s* cache, uint32 reference, opcode isWrite);
void printDMCache(cache_s* cache);
#endif /* DMCACHE_H_ */