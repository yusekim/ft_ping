#include "utils.h"

uint16_t calculate_cksum(const void *data, size_t len)
{
	const uint16_t *buf = data;
	uint32_t sum = 0;

	while (len > 1)
	{
		sum += *buf++;
		len -= 2;
	}
	if (len == 1)
		sum += *(const uint8_t *)buf;
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return (uint16_t)(~sum);
}
