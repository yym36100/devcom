#pragma once

#include "stdint.h"
#include "dbg.h"

template<class T, size_t N> 
class cringbuff{
public:
	enum { size = N };
	uint16_t ri, wi;
	T data[size];

	cringbuff() :ri(0), wi(0){}

	bool isEmpty() { return ri == wi; }
	bool isFull() { return next(wi) == ri; }
	
	bool Add(T d){		
		if (!isFull()){
			if(d==0){
				log("dbg");
			}
			data[wi] = d;
			wi = next(wi);			
			return true;
		}		
		return false;
	}

	bool Remove(T *d){		
		if (!isEmpty()){
			T res = data[ri];
			ri = next(ri);
			if(d)*d=res;			
			return true;
		}
		return false;
	}

	uint16_t getSize(){
		int16_t delta = wi - ri;
		if (delta < 0) delta += size;
		return delta;
	}
	uint16_t getFree(){ return size - getSize()-1; }

	bool peek(T *d,uint16_t offset){
		if (offset < getSize()){
			offset += ri;
			if (offset >= size)offset -= size;
			if(d)*d=data[offset];
			return true;			
		}
		else{		
			return false;
		}
	}
	bool discard(uint16_t noelements){		
		if (noelements > getSize()){
			noelements = getSize();			
		}
		ri += noelements;
		if (ri >= size)ri -= size;		
		return true;
	}

	void list(){
		uint16_t tr = ri, tw = wi;
		if (tr == tw){
			log("RB list:empty\n");
			return;
		}
		log("RB list: {s=%d f=%d} ", getSize(),getFree());
		while (1){
			printf("[%d]=%d ", tr, data[tr]);
			if (next(tr) == tw)break;
			tr = next(tr);
		}
		log("\n");
	}

private:
	uint16_t next(uint16_t n){
		uint16_t t = n + 1;
		if (t == size) t = 0;
		return t;
	}
};
