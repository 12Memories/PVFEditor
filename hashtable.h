#include "windows.h"

typedef struct _hash_pair
{
	char* szkey;
	int value;
}hash_pair,*phash_pair;

class Hashtable
{
public:
	Hashtable(int isize)
	{
		MaxSize  	= isize;
		ItemNum		= 0;				

		pInteralArray = (hash_pair*)malloc(MaxSize*sizeof(hash_pair));
		memset(pInteralArray,0,MaxSize*sizeof(hash_pair));
	};

	~Hashtable()
	{
		free(pInteralArray);
	}

public:
	int AddPair(char* key,int val)
	{
		if(ItemNum>MaxSize*70/100)
		{
			return -1;
		}

		int hash_val = _hashv_(key);
		while(char* okey = pInteralArray[hash_val].szkey)//冲突
		{			
			if(strcmp(okey,key)==0)
			{
				//已经存在key
				return 1;
			}

			//有冲突
			if(++hash_val==MaxSize)
			{
				//满了
				hash_val = 0;
			}
		}

		//write
		pInteralArray[hash_val].szkey = key;
		pInteralArray[hash_val].value = val;	

		ItemNum++;

		return 0;
	}

	hash_pair* Find(char* key)
	{
		int hash_val = _hashv_(key);
		while(char* okey = pInteralArray[hash_val].szkey)//冲突
		{
			if(strcmp(okey,key)==0)
			{
				//已经存在key
				return &pInteralArray[hash_val];
			}

			//下一个重复的key
			if(++hash_val == MaxSize)
			{
				//满了
				hash_val = 0;
			}
		}
		return NULL;
	}
protected:
	unsigned int _hashv_(char* _Begin)
	{	// hash range of elements
		unsigned int _Val = 2166136261U;
		char* _End = _Begin+strlen(_Begin);
		while(_Begin != _End)
			_Val = 16777619U * _Val ^ (unsigned int)*_Begin++;
		return _Val%MaxSize;
	}


public:
	unsigned int ItemNum;
	unsigned int MaxSize;	
	hash_pair*	pInteralArray;
};