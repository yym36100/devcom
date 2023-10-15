#include <stdio.h>

typedef unsigned char uint8_t;


// Generate a 7-bit CRC
const unsigned char CRC7_POLY = 0x91;
unsigned char getCRC(const uint8_t * message, unsigned char length){
  unsigned char i, j, crc = 0;
 
  for (i = 0; i < length; i++){
    crc ^= message[i];
    for (j = 0; j < 8; j++){
      if (crc & 1) crc ^= CRC7_POLY;
      crc >>= 1;
    }
  }
  return crc;
}

// Verify a 7-bit CRC
int verifyCRC7(const uint8_t *data, size_t length, uint8_t receivedCRC) {
	uint8_t calculatedCRC = getCRC(data, length);
	return (calculatedCRC == receivedCRC) ? 1 : 0;
}

int main() {
	// Example data (replace with your data)
	uint8_t data[4] = {0x12, 0x34, 0x56};

	// Generate a CRC and append it to the data
	uint8_t crc = getCRC(data, 3);
	data[3] = crc;

	// Simulate data transmission with errors
	// Modify the receivedData or crcReceived to simulate errors
	uint8_t receivedData[] = {0x12, 0x34, 0x56, 0x33};
	uint8_t crcReceived = receivedData[sizeof(receivedData) - 1];

	// Verify the received data
	int isDataValid = verifyCRC7(receivedData, sizeof(receivedData) - 1, crcReceived);

	for(int i=0;i<256;i++){
		data[0]=i;
		uint8_t crc = getCRC(data, 3);
		printf("i=%3d crc=%3d\n",i,crc);
	
	}


	return 0;
}