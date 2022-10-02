#include "SlateServer.hpp"
#include <string.h>

result_t SlateServer::bind(uint16_t port){
    if(isBound){
        printf("Attempted to bind SlateServer twice!\n");
        return FAIL;
    }

    slate_netconn = netconn_new(NETCONN_UDP);
    err_t err = netconn_bind(slate_netconn, IP4_ADDR_ANY, port);
   
    if(err == ERR_OK) {
        isBound = true;
        return OK;
    }else{
        printf("Could not bind SlateServer to port %u. err_t: %i\n", port, err);
        netconn_delete(slate_netconn);
        isBound = false;
        return FAIL;
    }
}

result_t SlateServer::connect(ip4_addr_t addr, uint16_t port){
    if(!isBound){
        printf("Need to bind SlateServer before connecting!\n");
        return FAIL;
    }
    
    // If no target address, we disable the server
    if(ip_addr_isany(&addr)){
        isConnected = false;
        netconn_disconnect(slate_netconn);
        return OK;
    }

    err_t err = netconn_connect(slate_netconn, &addr, port);
    
    if(err == ERR_OK) {
        isConnected = true;
        return OK;
    }else{
        printf("Could not connect SlateServer. err_t: %i\n", err);
        isConnected = false;
        return FAIL;
    }
}

result_t SlateServer::send(const uint8_t* message, uint16_t len) {
    if (!isConnected) return OK;
    
    // create a packet
    netbuf *buf = netbuf_new();
    netbuf_alloc(buf, len);

    uint16_t size;
    void *data_ptr;
    netbuf_data(buf, &data_ptr, &size);
    memcpy(data_ptr, message, min(len,size));

    if (size != len){
        printf("Allocated netbuf size (%u) dosen't match data size (%u)\n",size,len);
        return FAIL;
    }

    //send packet
    err_t err = netconn_send(slate_netconn, buf);

    if(err != ERR_OK){
        printf("SlateServer failed to send.\n");
        return FAIL;
    }

    //clean up
    netbuf_delete(buf);
    return OK;
}