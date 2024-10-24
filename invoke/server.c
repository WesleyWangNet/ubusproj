#include <stdio.h>

/*关键头文件*/
#include <libubox/uloop.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

struct prog_attr {
    char name[64];
    int chn_id;
};

#define PROG_MAX 8

static struct ubus_context* ctx = NULL;
static const char* socket_path;
static struct blob_buf b;

enum  
{  
    SCAN_CHNID,  
    SCAN_POLICY_MAX,  
}; 

static struct prog_attr uri_list[PROG_MAX] =
{
    {"program_bj", 1},
    {"program_gz", 1},
    {"program_ss", 1},
    {"", -1},
};

//消息格式，与client的一致
static const struct blobmsg_policy scan_policy[SCAN_POLICY_MAX] = {  
    [SCAN_CHNID] = {.name = "chnID", .type = BLOBMSG_TYPE_INT32},  
};


static int ubus_start_scan(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
    int ret = -1;
    void *json_uri = NULL;
    void *json_list = NULL;
    int idx;
    blob_buf_init(&b, 0);
    
    struct blob_attr *tb[SCAN_POLICY_MAX];
    ret = blobmsg_parse(scan_policy, SCAN_POLICY_MAX, tb, blob_data(msg), blob_len(msg));
    if (ret) {
        printf("error\r\n");
    }
    
    if (tb[SCAN_CHNID] != NULL) 
    {
        int chnid = blobmsg_get_u32(tb[SCAN_CHNID]);  
        if(chnid == 0) 
        {
            json_uri = blobmsg_open_array(&b, "prog_list"); //名字可以随意起
            for(idx = 0; idx < PROG_MAX; idx++) {
                if ('\0' != uri_list[idx].name[0]) {
                    json_list = blobmsg_open_table(&b, NULL);
                    blobmsg_add_string(&b, "name", uri_list[idx].name);
                    blobmsg_add_u32(&b, "channel", uri_list[idx].chn_id);
                    blobmsg_close_table(&b, json_list);
                }
            }
            blobmsg_close_array(&b, json_uri);
            ret = 0;
        }
        else
        {
            for (idx = 0; idx < PROG_MAX; idx++) {
                if ('\0' != uri_list[idx].name[0] && uri_list[idx].chn_id == chnid) {
                    blobmsg_add_string(&b, "name", uri_list[idx].name);
                    ret = 0;
                }
            }
        }
    }
    blobmsg_add_u32(&b, "result", ret);
    ubus_send_reply(ctx, req, b.head);
    return 0;
}

//方法列表
static struct ubus_method scan_methods[] =   
{  
    UBUS_METHOD("scan", ubus_start_scan, scan_policy),  
};

//未使用，但必须设置
static struct ubus_object_type scan_obj_type = UBUS_OBJECT_TYPE("scan_prog", scan_methods);  

static struct ubus_object scan_obj =   
{  
    .name = "scan_prog", /* 对象的名字 */  
    .type = &scan_obj_type,  
    .methods = scan_methods,  
    .n_methods = ARRAY_SIZE(scan_methods),  
}; 

static void ubus_reconn_timer(struct uloop_timeout *timeout)
{
    static struct uloop_timeout retry = {
        .cb = ubus_reconn_timer,
    };
    int t = 2;

    if (ubus_reconnect(ctx, socket_path)) {
        uloop_timeout_set(&retry, t * 1000);
        return ;
    }

    ubus_add_uloop(ctx);
}


static void ubus_connection_lost(struct ubus_context *ctx)
{
    ubus_reconn_timer(NULL);
}

static int server_init(const char *path)
{
    uloop_init(); 
    socket_path = path;
    ctx = ubus_connect(path);
    if (!ctx) {
        printf("ubus connected failed\r\n");
        return -1;
    }
    ctx->connection_lost  = ubus_connection_lost;
    ubus_add_uloop(ctx);

    if (ubus_add_object(ctx, &scan_obj) != 0) {
        printf("ubus add obj failed\r\n");
        return -1;
    }

    return 0;
}

static void server_done(void)
{
    if (ctx)
        ubus_free(ctx);
}

int main(int argc,char **argv)
{
    printf("%s\r\n", argv[1]);
    server_init(argv[1]);
    uloop_run();
    server_done();

    return 0;
}