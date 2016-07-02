#ifndef CUDPCOMM_H
#define CUDPCOMM_H

#include "cipcomm.h"

#include <netinet/in.h>

#define DEFAULT_SOCK_BUF_SZ 4096

typedef struct
{
    int socket;
    struct sockaddr_in addr;
//    unsigned char *buffer;
//    int buffer_size;
    int port;
    char tgt_address[16];
} sock_data_t;

class CUDPComm : public CIPComm
{
public:
    CUDPComm();
//    CUDPComm(int buf_size);
    ~CUDPComm();
    int  OpenSession(char target_address[], int  port_base, int  target_port);
    void CloseSession(void);
    int  WriteData(unsigned char data[], int data_length);
    int  ReadData(unsigned char data[], int data_length);
private:
//    int  allocate_buffer(int rx_size, int tx_size);
//    void deallocate_buffer(void);
    int create_rx_socket(void);
    int create_tx_socket(void);

    sock_data_t rx_sock;
    sock_data_t tx_sock;
};

#endif // CUDPCOMM_H
