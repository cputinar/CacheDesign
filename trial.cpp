#include "dmcache.h"

#include <iostream>

#define INDEXMASK 0xFFFFFFFF

using namespace std;

DMCache::DMCache(cacheLevel level, uint32 cacheSize, uint32 blockSize)
                :Cache(level)
{
        cout << "Creating the DM Cache" << endl;

        // Calculate number of clocks
        m_numBlocks = cacheSize / blockSize;
        m_blockSize = blockSize;

        // Calculate number of index bits
        int temp = m_numBlocks;
        m_numIndexBits = 0;
        while (temp >>= 1)
        {
                m_numIndexBits++;
        }

        // Calculate number bits deciding word in block
        temp = m_blockSize;
        m_numBitsWordInBlock = 0;
        while(temp >>= 1)
        {
                m_numBitsWordInBlock++;
        }

        // Calculate number of tag bits
        m_numTagBits = WORD_SIZE - m_numIndexBits - m_numBitsWordInBlock;

        /*Set the number of Bits per word in block*/
        m_numBitsWordInBlock -= 2;

        cout << "NumBlocks:" << m_numBlocks << endl
             << "NumIndexBits:" << m_numIndexBits << endl
             << "NumTagBits:" << m_numTagBits << endl
             << "NumBitsWordInBlock:" << m_numBitsWordInBlock << endl;

        m_cache = new DMCacheBlock[m_numBlocks];
}

uint32 DMCache::getTagFromReference(uint32 reference)
{
        // Calculate tag of reference
        uint32 tag = reference;
        tag = (tag >> (WORD_SIZE - m_numTagBits));

        return tag;
}

uint32 DMCache::getIndexFromReference(uint32 reference)
{
        // Calculate index of reference
        uint32 index = reference;
        uint32 mask = INDEXMASK;
        // Shift to right to get number of bits
        mask >>= (WORD_SIZE - m_numIndexBits);
        // Shift back into proper location compared to reference
        mask <<= (m_numBitsWordInBlock + m_numBitsByteInWord);
        index = (index & mask);
        index >>= (m_numBitsWordInBlock + m_numBitsByteInWord);

        return index;
}

Cache::checkRet DMCache::checkInCache(uint32 reference)
{
        uint32 index = getIndexFromReference(reference);
        uint32 tag = getTagFromReference(reference);

        checkRet ret;

        // See if reference is in cache
        if (m_cache[index].GetTag() == tag && m_cache[index].GetValid())
        {
                ret = CR_HIT;
        }
        else
        {
                ret = CR_MISS;
        }

        return ret;
}

Cache::eviction DMCache::checkAddRequiresEviction(uint32 reference)
{
        uint32 index = getIndexFromReference(reference);

        // If block not yet valid, don't need to evict anything
        if (!m_cache[index].GetValid())
        {
                return E_NOEVICT;
        }
        // Must evict, either dirty or clean
        else
        {
                if (m_cache[index].GetDirty())
                {
                        return E_DIRTY;
                }
                else
                {
                        return E_CLEAN;
                }

        }
}

uint32 DMCache::getAddressToEvict(uint32 reference)
{
        uint32 index = getIndexFromReference(reference);

        uint32 evictTag = m_cache[index].GetTag();

        return reconstructAddress(evictTag, index);
}

void DMCache::addToCache(uint32 reference)
{
        uint32 index = getIndexFromReference(reference);
        uint32 tag = getTagFromReference(reference);

        m_cache[index].SetTag(tag);
        m_cache[index].SetValid(true);
        m_cache[index].SetDirty(false);
}

void DMCache::write(uint32 reference)
{
        uint32 index = getIndexFromReference(reference);
        m_cache[index].SetDirty(true);
}

uint32 DMCache::reconstructAddress(uint32 tag, uint32 index)
{
        uint32 addr = (tag << (WORD_SIZE - m_numTagBits)) |
                                  (index << (m_numBitsWordInBlock + m_numBitsByteInWord));

        return addr;
}


void DMCache::printCache()
{

}