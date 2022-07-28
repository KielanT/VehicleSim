#include "ppch.h"
#include "HashTable.h"

// TODO: Create own hash table


TUInt32 AddUpHash(const TUInt8* pKey, const TUInt32 iKeyLen)
{
	TUInt32 iHash = 0;
	TUInt32 iKeyIndex;

	for (iKeyIndex = 0; iKeyIndex < iKeyLen; ++iKeyIndex)
	{
		iHash += pKey[iKeyIndex];

	}

	return iHash;
}


// Jenkins one-at-a-time hashing function, a high performance hashing function with good
// distribution of indexes (few collisions)
TUInt32 JOneAtATimeHash(const TUInt8* pKey, const TUInt32 iKeyLen)
{
	TUInt32 iHash = 0;
	TUInt32 iKeyIndex;

	for (iKeyIndex = 0; iKeyIndex < iKeyLen; ++iKeyIndex)
	{
		iHash += pKey[iKeyIndex];
		iHash += (iHash << 10);
		iHash ^= (iHash >> 6);
	}
	iHash += (iHash << 3);
	iHash ^= (iHash >> 11);
	iHash += (iHash << 15);
	return iHash;
}