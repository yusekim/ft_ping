#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>

#define PACKET_SIZE 64

// 체크섬 계산 함수 (ICMP 헤더용)
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <destination IP>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *dest_ip = argv[1];
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, dest_ip, &dest_addr.sin_addr);

    // 1. raw 소켓 생성 (ICMP 프로토콜 사용)
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sockfd < 0) {
        perror("socket 생성 실패");
        exit(EXIT_FAILURE);
    }

    // 2. pselect()에서 사용할 시그널 마스크 설정 (예시로 SIGINT를 블록)
    sigset_t block_mask, orig_mask;
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGINT);
    if(sigprocmask(SIG_BLOCK, &block_mask, &orig_mask) < 0) {
        perror("sigprocmask 실패");
        exit(EXIT_FAILURE);
    }

    int sequence = 0;
    while(1) {
        // 루프 시작 시각 기록 (1초 간격을 맞추기 위해)
        struct timespec start_time, current_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        // 3. ICMP Echo Request 패킷 구성
        char packet[PACKET_SIZE];
        memset(packet, 0, PACKET_SIZE);
        struct icmp *icmp_hdr = (struct icmp *)packet;
        icmp_hdr->icmp_type = ICMP_ECHO;    // Echo Request
        icmp_hdr->icmp_code = 0;
        icmp_hdr->icmp_id = getpid();
        icmp_hdr->icmp_seq = sequence++;
        // 데이터 영역 채우기
        memset(icmp_hdr->icmp_data, 0xa5, PACKET_SIZE - sizeof(struct icmp));
        icmp_hdr->icmp_cksum = 0;
        icmp_hdr->icmp_cksum = checksum(packet, PACKET_SIZE);

        // 4. ICMP Echo Request 전송
        if(sendto(sockfd, packet, PACKET_SIZE, 0,
                  (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
            perror("sendto 실패");
        } else {
            printf("보냄: ICMP Echo Request (seq=%d)\n", icmp_hdr->icmp_seq);
        }

        // 5. pselect()를 위한 타임아웃 설정 (초기 타임아웃은 1초)
        struct timespec timeout;
        timeout.tv_sec = 1;
        timeout.tv_nsec = 0;

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        // 6. pselect() 호출: 응답 대기
        int ret = pselect(sockfd + 1, &readfds, NULL, NULL, &timeout, &orig_mask);
        if(ret < 0) {
            perror("pselect 실패");
            break;
        } else if(ret == 0) {
            // 1초 이내에 응답이 없으면 타임아웃 처리
            printf("타임아웃: seq=%d에 대한 응답 없음\n", icmp_hdr->icmp_seq);
        } else {
            // 7. 데이터가 도착한 경우: ICMP Echo Reply 수신
            char recv_buf[1024];
            struct sockaddr_in recv_addr;
            socklen_t addr_len = sizeof(recv_addr);
            int n = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0,
                             (struct sockaddr *)&recv_addr, &addr_len);
            if(n < 0) {
                perror("recvfrom 실패");
            } else {
                // IP 헤더 길이 고려 (IP 헤더 다음에 ICMP 헤더 위치)
                struct ip *ip_hdr = (struct ip *)recv_buf;
                int ip_hdr_len = ip_hdr->ip_hl * 4;
                struct icmp *icmp_reply = (struct icmp *)(recv_buf + ip_hdr_len);

                if(icmp_reply->icmp_type == ICMP_ECHOREPLY &&
                   icmp_reply->icmp_id == getpid()) {
                    printf("받음: ICMP Echo Reply (seq=%d)\n", icmp_reply->icmp_seq);
                }
            }
        }

        // 8. 남은 시간을 계산하여 1초 간격 유지
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        long elapsed_nsec = (current_time.tv_sec - start_time.tv_sec) * 1000000000LL +
                            (current_time.tv_nsec - start_time.tv_nsec);
        long one_sec_nsec = 1000000000LL;  // 1초 = 1,000,000,000 ns
        if(elapsed_nsec < one_sec_nsec) {
            long sleep_nsec = one_sec_nsec - elapsed_nsec;
            struct timespec sleep_time;
            sleep_time.tv_sec = sleep_nsec / 1000000000LL;
            sleep_time.tv_nsec = sleep_nsec % 1000000000LL;
            nanosleep(&sleep_time, NULL);
        }
    }

    close(sockfd);
    return 0;
}
