#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>

// 체크섬 계산 함수
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
    if (argc != 4) {
        printf("Usage: %s <대상 IP> <identifier> <sequence>\n", argv[0]);
        return 1;
    }

    const char *target_ip = argv[1];
    int identifier = atoi(argv[2]);
    int sequence = atoi(argv[3]);

    // raw socket 생성 (ICMP 프로토콜 사용)
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket 생성 실패");
        return 1;
    }

    // 패킷 버퍼 초기화
    char packet[1024];
    memset(packet, 0, sizeof(packet));

    // ICMP 헤더 구성
    struct icmphdr *icmp_hdr = (struct icmphdr *) packet;
    icmp_hdr->type = ICMP_ECHOREPLY;  // Echo Reply
    icmp_hdr->code = 0;
    icmp_hdr->un.echo.id = htons(identifier);
    icmp_hdr->un.echo.sequence = htons(sequence);

    // 간단한 페이로드 추가
    const char *payload = "ABCDEFGHIJKLMNOP";
    int payload_len = strlen(payload);
    memcpy(packet + sizeof(struct icmphdr), payload, payload_len);

    // 패킷 전체 길이 계산 및 체크섬 설정
    int packet_size = sizeof(struct icmphdr) + payload_len;
    icmp_hdr->checksum = 0;
    icmp_hdr->checksum = checksum((unsigned short *) packet, packet_size);

    // 대상 주소 설정
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, target_ip, &addr.sin_addr);

    // 패킷 전송 (dup! 상황을 유도하기 위해 중복 패킷 전송)
    if (sendto(sockfd, packet, packet_size, 0, (struct sockaddr *)&addr, sizeof(addr)) <= 0) {
        perror("sendto 실패");
        close(sockfd);
        return 1;
    }

    printf("중복 ICMP Echo Reply 패킷을 %s로 전송했습니다.\n", target_ip);
    close(sockfd);
    return 0;
}
