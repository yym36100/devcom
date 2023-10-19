
#include "stdint.h"
#include <stdlib.h>
#include <stdio.h>

#include "ringbuff.h"
#include "dbg.h"
#include "crc7.h"
#include "devproto.h"

enum { wrong_id = 1, wrong_len = 2, wrong_crc = 4 };
uint8_t gendata(uint8_t *buff, uint8_t id, uint8_t len, uint8_t flags){
	

	if (flags&wrong_id) buff[0] = 0xff;
	else buff[0] = 0xa0 | id;

	if (flags&wrong_len) buff[1] = 0xfe;
	else buff[1] = len;

	for (int i = 0; i < len; i++){
		buff[2 + i] = rand();
	}
	if (flags&wrong_crc) buff[2 + len] = 0xfd;
	else buff[2 + len] = crc7_be(0,&buff[2], len);
	//uint8_t crc7_be(uint8_t crc, const uint8_t *buffer, size_t len)
	return 2 + len + 1;
}

int main(int argc, char* argv[])
{
	cringbuff<uint8_t, 200> rb;
	uint8_t buff[100];
	uint8_t count;
	cdevcom<20> com;
	eres rr;

	for (int i = 0; i < 5; i++){
		count = gendata(buff, i+1/*rand()%max_id*/, 1+(rand()&0x7), 0);
		//buff[count++]=0xff; // this injects errors
		//buff[count++]=0xff; // this injects errors
		count += gendata(&buff[count], i+1, 5+(rand()&0x7), wrong_crc);
		 
		//produce
		uint8_t *p = buff;
		while (count > 0){
			if(!rb.Add(*p++)){
				log("add error");
			}
			count--;
		}
	}

	// consume
	uint8_t j = 0,res;
	while (!rb.isEmpty()){		
		bool pres = rb.peek(&res,j++);
		if(!pres){
			log("peek error\n");
			break;
		}
		rr=com.sink(res);
		if((rr!=ok)&&(rr!=rready)){
			log("sink error : %s\n",com.err_str(rr));
			rb.discard(j);
			j=0;
		}
		if(rr==rready){
			com.source(0);
			rb.discard(j);
			j=0;
		}
	}
	

#if 0
	cringbuff rb;
	printf("size=%d\n", rb.getSize());
	rb.list();
	rb.Add(1);
	rb.Add(2);
	rb.Add(3);
	rb.Add(4);
	rb.Add(5);
	rb.Remove(0);
	rb.Add(6);
	rb.Add(7);
	rb.Remove(0);
	rb.Add(8);
	rb.Remove(0);
	rb.Remove(0);
	rb.Remove(0);
	rb.Add(9);
	rb.Add(10);
	rb.Add(11);
	rb.Remove(0);
	rb.Remove(0);
	rb.Add(12);
	rb.Add(13);
	rb.peek(0);
	rb.peek(1);
	rb.peek(2);
	rb.peek(3);

	rb.list();
	rb.discard(1);
	rb.list();
	rb.peek(1);
	rb.discard(1);
	rb.list();
	rb.discard(2);
	rb.list();
#endif

#if 0
	uint8_t testdata[] = { 1, 0xa1, 0xa1, 3, 4, 5, 6, 17,
		0xa2, 1,2,19,
		0xa3, 2, 10,11,11, };

	cdevcom com;
	eres res;

	//for (int i = 0; i < sizeof(testdata); i++)
	uint8_t start = 0;
	uint8_t index = 0;
	while (1){

		res = com.sink(testdata[start + index]);
		if (res == rready) {
			start += index+1;
			index = 0;
			log("N: parse success, restart at %d\n", start);
		}
		else {
			if (res == ok) index++;
			else
			{
				start++;
				index = 0;
				log("E: parse error, retry at %d\n", start);
			}
		}

		if (res == rready)com.source(0);

		if ((start + index) >= sizeof(testdata))break;
	}

#endif
	return 0;
	}

//printf("%d:res = %d state=%d  \n", i, res,com.state);

