#include<stdio.h>		// for printf() and perror()
#include<unistd.h>		// for close()
#include<stdint.h>		// for uint8_t
#include<stdlib.h>		// for malloc(), free() and EXIT_FAILURE
#include<string.h>		// for memset() function
#include<sys/socket.h>		// for socket()
#include<linux/if_ether.h>	// for ETH_P_ARP
#include<arpa/inet.h>		// for htons()

#define MSG_SIZE 64
#define ETH_HEADER_LEN 14

struct ETHERNET_HEADER {
	uint8_t source_mac[6];
	uint8_t destination_mac[6];
	uint16_t type;
} __attribute__((packed));

struct ARP_HEADER {
	uint16_t htype;
	uint16_t ptype;
	uint8_t hlen;
	uint8_t plen;
	uint16_t opcode;
	uint8_t sender_mac[6];
	uint8_t sender_ip[4];
	uint8_t target_mac[6];
	uint8_t target_ip[4];
}__attribute__((packed));

int main()
{
	int sock_fd;
	char *sender_ip = (char *)malloc(16);
	char *target_ip = (char *)malloc(16);

	memset(sender_ip, 0, sizeof(sender_ip));
	memset(target_ip, 0, sizeof(target_ip));

	// Allocate buffer and set it to 0
	uint8_t *buffer = (uint8_t *)malloc(MSG_SIZE);
	memset(buffer, 0, sizeof(MSG_SIZE));

	// Pointer to the ethernet header
	struct ETHERNET_HEADER *eth_header = (struct ETHERNET_HEADER *)buffer;

	// Pointer to the arp header
	struct ARP_HEADER *arp_header = (struct ARP_HEADER *)(buffer + ETH_HEADER_LEN);

	// Creating a raw socket for reading ARP packets
	if ((sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0){
		perror("socket failed");
		exit (EXIT_FAILURE);}

	printf("\n++++++++++++++++++++++ Waiting for ARP packets +++++++++++++++++++++++++\n");

	while(1){
		if (recvfrom(sock_fd, buffer, MSG_SIZE, 0, NULL, NULL) < 0){
			perror("recvfrom failed");
			exit (EXIT_FAILURE);
		}

		printf("++++++++++ETHERNET HEADER+++++++++++++++\n");
		printf("+ source_mac      : %02X:%02X:%02X:%02X:%02X:%02X\n",
			eth_header->source_mac[0],
			eth_header->source_mac[1],
			eth_header->source_mac[2],
			eth_header->source_mac[3],
			eth_header->source_mac[4],
			eth_header->source_mac[5]
			);

		printf("+ destination_mac : %02X:%02X:%02X:%02X:%02X:%02X\n",
                        eth_header->destination_mac[0],
                        eth_header->destination_mac[1],
                        eth_header->destination_mac[2],
                        eth_header->destination_mac[3],
                        eth_header->destination_mac[4],
                        eth_header->destination_mac[5]
                        );

		printf("+ type            : %x\n",
			ntohs(eth_header->type));
		printf("++++++++++++++++++++++++++++++++++++++++\n");

		if (inet_ntop(AF_INET, &arp_header->sender_ip, sender_ip, 16) < 0) {	
			perror("inet_ntop() failed");
                	exit (EXIT_FAILURE);
		}

		if (inet_ntop(AF_INET, &arp_header->target_ip, target_ip, 16) < 0) {    
                        perror("inet_ntop() failed");
                        exit (EXIT_FAILURE);
                }

		printf("+++++++++++++++ARP HEADER+++++++++++++++\n");
		printf("+ Hardware type   : %d\n", ntohs(arp_header->htype));
		printf("+ Protocol type   : %d\n", ntohs(arp_header->ptype));
		printf("+ Hardware length : %d\n", ntohs(arp_header->hlen));
		printf("+ Protocol length : %d\n", ntohs(arp_header->plen));
		printf("+ Opcode          : %d\n", ntohs(arp_header->opcode));
		printf("+ Sender Mac      : %02X:%02X:%02X:%02X:%02X:%02X\n",
			arp_header->sender_mac[0],
	                arp_header->sender_mac[1],
			arp_header->sender_mac[2],
			arp_header->sender_mac[3],
			arp_header->sender_mac[4],
			arp_header->sender_mac[5]);
		printf("+ Sender Ip       : %s\n", sender_ip);
		printf("+ target Mac      : %02X:%02X:%02X:%02X:%02X:%02X\n",
                        arp_header->target_mac[0],
                        arp_header->target_mac[1],
                        arp_header->target_mac[2],
                        arp_header->target_mac[3],
                        arp_header->target_mac[4],
                        arp_header->target_mac[5]);

		printf("+ Target IP       : %s\n", target_ip);
		printf("++++++++++++++++++++++++++++++++++++++++\n\n");

		memset(sender_ip, 0, sizeof(sender_ip));
		memset(target_ip, 0, sizeof(target_ip));
	}

	free(sender_ip);
	free(target_ip);
	free(buffer);
	close(sock_fd);

	return 0;
}
