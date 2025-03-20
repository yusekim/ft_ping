#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>

unsigned short checksum(void *b, int len) {
	unsigned short *buf = b;
	unsigned int sum = 0;
	unsigned short result;

	for(sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if(len == 1)
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage: %s <identifier> <sequence>\n", argv[0]);
		return 1;
	}

	const char target_ip[] = "127.0.0.1";
	int identifier = atoi(argv[1]);
	int sequence = atoi(argv[2]);

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		perror("socket 생성 실패");
		return 1;
	}

	char packet[1024];
	memset(packet, 0, sizeof(packet));

	struct icmphdr *icmp_hdr = (struct icmphdr *) packet;
	icmp_hdr->type = ICMP_ECHOREPLY;
	icmp_hdr->code = 0;
	icmp_hdr->un.echo.id = htons(identifier);
	icmp_hdr->un.echo.sequence = htons(sequence);

	const char *payload = "ABCDEFGHIJKLMNOP";
	int payload_len = strlen(payload);
	memcpy(packet + sizeof(struct icmphdr), payload, payload_len);

	int packet_size = sizeof(struct icmphdr) + payload_len;
	icmp_hdr->checksum = 0;
	icmp_hdr->checksum = checksum((unsigned short *) packet, packet_size);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, target_ip, &addr.sin_addr);

	if (sendto(sockfd, packet, packet_size, 0, (struct sockaddr *)&addr, sizeof(addr)) <= 0) {
		perror("sendto 실패");
		close(sockfd);
		return 1;
	}

	printf("중복 ICMP Echo Reply 패킷을 %s로 전송했습니다.\n", target_ip);
	close(sockfd);
	return 0;
}
