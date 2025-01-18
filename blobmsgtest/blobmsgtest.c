
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

    blobmsg_add_string(&buf, "name", "Openwrt");
    blobmsg_add_u32(&buf, "id", 100);

    //add object
    void *tab1 = blobmsg_open_table(&buf, "tab1");
    blobmsg_add_string(&buf, "tab1_name", "tab1");
    blobmsg_add_u32(&buf, "tab1_id", 101);
    blobmsg_close_table(&buf, tab1);

    void *tab2 = blobmsg_open_table(&buf, "tab2");
    blobmsg_add_string(&buf, "tab2_name", "tab2");
    blobmsg_add_u32(&buf, "tab2_id", 102);
    blobmsg_close_table(&buf, tab2);


    // test hello example
   // blobmsg_add_blob(&buf1, attr);

    char *jsonstr1 = blobmsg_format_json_value(buf1.head, true);
    printf("%s\r\n", jsonstr1);
    char *jsonstr = blobmsg_format_json(buf.head, true);
    printf("%s\r\n", jsonstr);

    free(jsonstr);
    free(jsonstr1);
    blob_buf_free(&buf1);
    blob_buf_free(&buf);
    
    return 0;
}
