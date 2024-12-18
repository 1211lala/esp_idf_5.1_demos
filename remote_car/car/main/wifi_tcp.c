#include "wifi_ap.h"
#include "lwip/sockets.h"
#include "ledc.h"
void tcp_server_task(void *arg)
{
    /* 新建网络描述符 */
    int hostfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in listen_sockaddr = {};
    listen_sockaddr.sin_family = AF_INET;
    inet_aton(ap.info.ip, listen_sockaddr.sin_addr.s_addr); /* 将本机IP设置为监听IP */
    listen_sockaddr.sin_port = htons(8000);                 /* 监听端口 */
    /* 绑定 */
    if (0 != bind(hostfd, (struct sockaddr *)&listen_sockaddr, sizeof(struct sockaddr_in)))
    {
        ESP_LOGE("task_tcp_server", "bind fail !!!");
    }
    if (0 != listen(hostfd, 3))
    {
        ESP_LOGE("task_tcp_server", "listen fail !!!");
    }
    socklen_t len = sizeof(struct sockaddr_in);
wait_client:
    struct sockaddr_in clientsock;
    int clientfd = accept(hostfd, (struct sockaddr *)&clientsock, &len);

    printf("%s\n", inet_ntoa(clientsock.sin_addr.s_addr));
    uint8_t r_buf[100];
    while (1)
    {
        ssize_t ret = recv(clientfd, r_buf, sizeof(r_buf), 0);
        if (ret <= 0)
        {
            MotorA.speed = 0;
            MotorA.angle = 0;
            close(clientfd);
            goto wait_client;
        }
        if (r_buf[0] == 0xaa && r_buf[1] == 0xff && r_buf[4] == 0xff && r_buf[5] == 0xaa)
        {

            MotorA.speed = r_buf[2] - 128;
            MotorA.angle = r_buf[3] - 128;

            printf("电机速度: %d  舵机角度%d\n", MotorA.speed, MotorA.angle);
        }
    }
    close(hostfd);
}
/* aa ff 01 01 ff aa  */