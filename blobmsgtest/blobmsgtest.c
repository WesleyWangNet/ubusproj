
#include "libubox/blobmsg.h"
#include "libubox/blobmsg_json.h"
#include <string.h>

#define BUFF_SIZE	0x10000
#define BUFF_CHUNKS	(((BLOB_ATTR_LEN_MASK + 1) / BUFF_SIZE) + 1)

int main(int argc, char **argv)
{

    char *json = NULL;
    struct blob_buf buf; // static  struct blob_buf buf;
    memset(&buf, 0, sizeof(buf));  //如果不是static, 必须初始化

    blobmsg_buf_init(&buf);  //default table
    int prev_len = buf.buflen;

    printf("buf.buflen: %d\r\n", prev_len);


#if 0
    for ( int i = 0; i < BUFF_CHUNKS; i++) {
        struct blob_attr *attr = blob_new(&buf, 0, BUFF_SIZE);
        if (!attr) {
            printf("blob new error\r\n");
            break;
        }
        if (prev_len < buf.buflen) {
            prev_len = buf.buflen;
            continue;
        }
    }

#endif

    return 0;
}