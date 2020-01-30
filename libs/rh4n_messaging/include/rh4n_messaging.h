#ifndef RH4NLIBMESSAGING
#define RH4NLIBMESSAGING

#ifdef __cplusplus
extern "C" {
#endif

#define RH4NLIBMESSAGINGFLAG_OVERRIDE    0x0001
#define RH4NLIBMESSAGINGFLAG_NONBLOCKING 0x0002

#define RH4NLIBMESSAGINGMAXCLIENTS 10
#define RH4NLIBMESSAGING_RESPONSETIMEOUT 120

#define RH4NLIBMESSAGING_TYPESESSIONINFORMATIONS 0x01

#define ASCII_STARTOFHEADER 0x01
#define ASCII_ACK 0x06
#define ASCII_NACK 0x15

typedef struct {
    uint8_t messageType;
    void *message;
} RH4nMessage;

typedef struct {
    uint8_t preable;
    uint8_t messageType;
} RH4nMessageingHeader_t;

int rh4n_messaging_createUDSServer(const char*, uint16_t, RH4nProperties*);
int rh4n_messaging_waitForClient(int, RH4nProperties*);
int rh4n_messaging_connectToUDSServer(const char*, RH4nProperties*);
int rh4n_messaging_waitForData(int, int, int, RH4nProperties*);

int rh4n_messaging_sendHeader(int sendSocket, uint8_t messageType, RH4nProperties *props);
int rh4n_messaging_recvHeader(int recvSocket, RH4nMessageingHeader_t *header, RH4nProperties *props);
int rh4n_messaging_writeToSocket(int, void*, int, RH4nProperties*);
int rh4n_messaging_readFromSocket(int recvSocket, void *data, int length, RH4nProperties *props);
void rh4n_messaging_sendAcknowledge(int sendSocket, uint8_t ack, RH4nProperties *props);
int rh4n_messaging_recvAcknowledge(int recvSocket, uint8_t *ack, RH4nProperties *props);

int rh4n_messaging_sendSessionInformations(int sendSocket, RH4nProperties *props);
int rh4n_messaging_recvSessionInformations(int recvSocket, RH4nProperties *props);

#ifdef __cplusplus
}
#endif
#endif
