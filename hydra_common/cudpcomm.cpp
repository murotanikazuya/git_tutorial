#include "cudpcomm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <fcntl.h>

#ifdef DEBUG
#include <assert.h>
#endif

CUDPComm::CUDPComm()
{
//    int ret;
//    ret = allocate_buffer(DEFAULT_SOCK_BUF_SZ, DEFAULT_SOCK_BUF_SZ);
#ifdef DEBUG
    assert(ret);
#endif
}

#if 0
CUDPComm::CUDPComm(int buf_size)
{
    int ret;
    ret = allocate_buffer(buf_size, buf_size);
#ifdef DEBUG
    assert(ret);
#endif
}
#endif

CUDPComm::~CUDPComm()
{
    CloseSession( );
//    deallocate_buffer( );
}


int  CUDPComm::OpenSession(char target_address[], int  port_base, int  target_port)
{
#if 0
    if(rx_sock.buffer_size==0)
        return -1;

    if(tx_sock.buffer_size==0)
        return -1;
#endif
    // set data to socket data structure
    rx_sock.port        = port_base;
    tx_sock.port        = target_port;
    strcpy(rx_sock.tgt_address, target_address);
    strcpy(tx_sock.tgt_address, target_address);

    // create sockets
    if( create_rx_socket( )<0 )
        return -1;
    if( create_tx_socket( )<0 )
        return -1;

    return 0;
}

void CUDPComm::CloseSession(void)
{
    int ret;
    ret = fcntl(rx_sock.socket, F_GETFL, 0);
    ret = ret & ~(ret&O_NONBLOCK);
    fcntl(rx_sock.socket, F_SETFL, ret);

    close(rx_sock.socket);
    close(tx_sock.socket);
}

int  CUDPComm::WriteData(unsigned char data[], int data_length)
{
    int n;
#if 0
    if(tx_sock.buffer_size<data_length)
        return -1;

//    memset(tx_sock.buffer, 0, sizeof(unsigned char)*tx_sock.buffer_size);
    memcpy(tx_sock.buffer, data, sizeof(unsigned char)*data_length);

    n = sendto(tx_sock.socket,
               (void*)tx_sock.buffer,
               data_length,
               0,
               (struct sockaddr *)&tx_sock.addr,
               sizeof(tx_sock.addr));
#endif
    n = sendto(tx_sock.socket,
               (void*)data,
               data_length,
               0,
               (struct sockaddr *)&tx_sock.addr,
               sizeof(tx_sock.addr));
    return n;
}

int  CUDPComm::ReadData(unsigned char data[], int data_length)
{
    int n=0;
#if 0
    if(rx_sock.buffer_size<data_length)
        return -1;

    memset(rx_sock.buffer, 0, sizeof(unsigned char)*rx_sock.buffer_size);

    n = recv(rx_sock.socket,
             rx_sock.buffer, sizeof(unsigned char)*data_length, 0);

    memcpy(data, rx_sock.buffer, sizeof(unsigned char)*data_length);
#endif
    n = recv(rx_sock.socket,
             data, sizeof(unsigned char)*data_length, 0);

    return n;
}

/***************** private methods ******************/
#if 0
int CUDPComm::allocate_buffer(int rx_size, int tx_size)
{
    rx_sock.buffer_size = rx_size;
    tx_sock.buffer_size = tx_size;

    rx_sock.buffer = (unsigned char*)malloc(sizeof(unsigned char)*rx_sock.buffer_size);
    if(rx_sock.buffer==NULL)
    {
        perror("RX malloc failed");
        rx_sock.buffer_size = 0;
        return -1;
    }

    tx_sock.buffer = (unsigned char*)malloc(sizeof(unsigned char)*tx_sock.buffer_size);
    if(tx_sock.buffer==NULL)
    {
        perror("TX malloc failed");
        tx_sock.buffer_size = 0;
        return -1;
    }
    return 0;
}

void CUDPComm::deallocate_buffer(void)
{
    if(rx_sock.buffer_size != 0)
    {
        free(rx_sock.buffer);
        rx_sock.buffer_size = 0;
    }
    if(tx_sock.buffer_size != 0)
    {
        free(tx_sock.buffer);
        tx_sock.buffer_size = 0;
    }
}
#endif

int CUDPComm::create_rx_socket(void)
{
    int ret;
    if( (rx_sock.socket = socket(AF_INET, SOCK_DGRAM, 0))<0 )
    {
        perror("socket creation failed");
        return -1;
    }

    // set port non-blocking
    ret = fcntl(rx_sock.socket, F_GETFL, 0);
    fcntl(rx_sock.socket, F_SETFL, ret|O_NONBLOCK);

    rx_sock.addr.sin_family      = AF_INET;
    rx_sock.addr.sin_port        = htons(rx_sock.port);
    rx_sock.addr.sin_addr.s_addr = INADDR_ANY;

    if((ret=bind(rx_sock.socket,
                 (struct sockaddr*)&(rx_sock.addr),
                 sizeof(rx_sock.addr)))==-1)
    {
        perror("bind failed");
        return -1;
    }

    return 0;
}

int CUDPComm::create_tx_socket(void)
{
    if((tx_sock.socket = socket(AF_INET, SOCK_DGRAM, 0))<0)
    {
        perror("failed creating socket");
        return -1;
    }

    tx_sock.addr.sin_family = AF_INET;
    tx_sock.addr.sin_port   = htons(tx_sock.port);
    if(!inet_pton(AF_INET, tx_sock.tgt_address,
                  &(tx_sock.addr.sin_addr.s_addr)) )
    {
        return -1;
    }

    return 0;
}
