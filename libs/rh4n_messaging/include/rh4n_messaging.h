#ifndef RH4NLIBMESSAGING
#define RH4NLIBMESSAGING

#ifdef __cplusplus
extern "C" {
#endif

#define RH4NLIBMESSAGINGFLAG_OVERRIDE    0x0001
#define RH4NLIBMESSAGINGFLAG_NONBLOCKING 0x0002

#define RH4NLIBMESSAGINGFLAG_NEXT 0x01
#define RH4NLIBMESSAGINGFLAG_NEXTLVL 0x02

#define RH4NLIBMESSAGINGMAXCLIENTS 10
#define RH4NLIBMESSAGING_RESPONSETIMEOUT 5

#define RH4NLIBMESSAGING_TYPESESSIONINFORMATIONS 0x01
#define RH4NLIBMESSAGING_TYPEVARLIST 0x02
#define RH4NLIBMESSAGING_TYPETEXTBLOCK 0x03

#define ASCII_STARTOFHEADER 0x01
#define ASCII_EOT 0x06
#define ASCII_ACK 0x06
#define ASCII_NACK 0x15
#define ASCII_RS 0x1E


typedef struct {
    uint8_t messageType;
    void *message;
} RH4nMessage;

typedef struct {
    uint8_t preable;
    uint8_t messageType;
    uint32_t dataLength;
} RH4nMessageingHeader_t;

int rh4n_messaging_createUDSServer(const char*, uint16_t, RH4nProperties*);
int rh4n_messaging_waitForClient(int, RH4nProperties*);
int rh4n_messaging_connectToUDSServer(const char*, RH4nProperties*);
int rh4n_messaging_waitForData(int, int, int, RH4nProperties*);

int rh4n_messaging_sendHeader(int sendSocket, uint8_t messageType, uint32_t dataLength, RH4nProperties *props);
int rh4n_messaging_recvHeader(int recvSocket, RH4nMessageingHeader_t *header, RH4nProperties *props);
int rh4n_messaging_writeToSocket(int, void*, int, RH4nProperties*);
int rh4n_messaging_readFromSocket(int recvSocket, void *data, int length, RH4nProperties *props);
void rh4n_messaging_sendAcknowledge(int sendSocket, uint8_t ack, RH4nProperties *props);
int rh4n_messaging_recvAcknowledge(int recvSocket, uint8_t *ack, RH4nProperties *props);
int rh4n_messaging_sendDataChunk(int sendSocket, void *data, int length, RH4nProperties *props);
int rh4n_messaging_recvDataChunk(int recvSocket, void *data, int length, RH4nProperties *props);

int rh4n_messaging_sendSessionInformations(int sendSocket, RH4nProperties *props);
int rh4n_messaging_recvSessionInformations(int recvSocket, RH4nProperties *props);


int rh4n_messaging_sendVarlist(int sendSocket, RH4nVarList *varlist, RH4nProperties *props);
int rh4n_messaging_processVarlistNode(int sendSocket, RH4nVarEntry_t *target, RH4nProperties *props, uint32_t level);
int rh4n_messaging_sendVarlistNode(int sendSocket, RH4nVarEntry_t *target, RH4nProperties *props);
int rh4n_messaging_sendVarlistValue(int sendSocket, RH4nVarObj *target, RH4nProperties *props);
int rh4n_messaging_sendVarlistArray(int sendSocket, RH4nVarObj *target, RH4nProperties *props);

int rh4n_messaging_recvVarlist(int recvSocket, RH4nVarList *varlist, RH4nProperties *props);
int rh4n_messaging_recvVarlistNode(int recvSocket, RH4nVarEntry_t *target, RH4nProperties *props);
int rh4n_messaging_recvVarlistValue(int recvSocket, RH4nVarObj *target, RH4nProperties *props);
int rh4n_messaging_recvVarlistArray(int recvSocket, RH4nVarObj *target, RH4nProperties *props);
void rh4n_messaging_recvVarlistAddNode(uint8_t nextFlags, RH4nVarEntry_t *newNode, RH4nVarEntry_t *lastNode, RH4nVarList *varlist);


int rh4n_messaging_sendTextBlock(int sendSocket, char *text, RH4nProperties *props);
int rh4n_messaging_recvTextBlock(int recvSocket, char **text, RH4nProperties *props);

#ifdef __cplusplus
}
#endif
#endif
