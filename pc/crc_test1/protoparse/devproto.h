#pragma once


#include "stdint.h"
#include "dbg.h"

//protocol
/*
101 id ->32
0 size -> 128
data [0..128]
0 crc (crc 7 bit)
*/


typedef enum {
	gpio_setup = 0,
	gpio_in,
	gpio_out,
	i2c_setup,
	i2c_inout,
	spi_setup,
	spi_inout,
	pwm_setup,
	adc_setup,
	adc_poll,
	adc_report=10,
	uart_setup,
	uart_tx,
	uart_rx,

	version,
	test_led, // req: blink freq resp: ack

	max_id,
} eids;

typedef enum { id = 0, size, data, crc, sready } estate;
typedef enum { ok = 0, iid, isize, icrc, rready, rnotready,elasterr } eres;

template<size_t N>
class cpacket{
public:
	uint8_t id;
	uint8_t size;
	uint8_t data[N];
	uint8_t crc;
	uint8_t i;

	bool isCrcValid(){
		//return crc == getCRC(data, size);
		return crc==crc7_be(0,data,size);
			//uint8_t crc7_be(uint8_t crc, const uint8_t *buffer, size_t len)
	}
};

template<size_t N>
class cdevcom{
public:
	estate state;
	cpacket<N> pack;

	cdevcom() :state(id){}

	const char* err_str(eres r){
		static const char *estr[]={
			"ok",
			"iid",
			"isize",
			"icrc",
			"rready",
			"rnotready"
			"rlasterr"
		};
		r=r<elasterr?r:elasterr;
		return estr[r];
	}

	eres source(cpacket<N> *ppacket){		
		if (state == sready){
			if (ppacket) *ppacket = pack;
			state = id;
			log("id=%2d size=%2d crc=%3d [ ",pack.id,pack.size,pack.crc);
			for(int i=0;i< pack.size;i++){
				log("%3d, ",pack.data[i]);
			}
			log("]\n");
			return ok;
		}		
		return rnotready;
	}

	eres sink(uint8_t c){		
		switch (state){
		case id:
			if (((c & 0x1f) < max_id) && ((c & 0xe0) == 0xa0)) {
				pack.id = c & 0x1f;
				state = size;	
				//log("id=%d\n",pack.id);
				return ok;
			}			
			return iid;
		case size:
			if ((c > 0) && (c < N) && ((c & 0x80) == 0x00)) {
				pack.size = c;
				pack.i = 0;
				state = data;			
				return ok;
			}			
			state = id;
			return isize;
		case data:
			pack.data[pack.i++] = c;			
			if (pack.i == pack.size){ state = crc; }
			return ok;
		case crc:
			if ((c & 0x80) == 0x80) {
				state = id;
				return icrc;
			}
			pack.crc = c;
			if (pack.isCrcValid()) {				
				state = sready;
				return rready;
			}			
			state = id;
			return icrc;

		case sready:
			return rready;
		}
		return ok;
	}

};


