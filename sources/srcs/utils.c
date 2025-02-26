#include "utils.h"

uint16_t calculate_cksum(const void *data, size_t len)
{
	const uint16_t *buf = data;
	uint32_t sum = 0;

	// 16비트 단위로 덧셈 수행
	while (len > 1)
	{
		sum += *buf++;
		len -= 2;
	}

	// 홀수 바이트가 남은 경우, 0으로 패딩하여 추가
	if (len == 1)
		sum += *(const uint8_t *)buf;

	// 오버플로우(캐리) 처리: 상위 16비트에 있는 값을 하위 16비트에 더함
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	// 1의 보수 취하기
	return (uint16_t)(~sum);
}
