#include "ML307R.h"
#include "delay.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "OLED.h"
#include "TIMER.h"

static uint8_t gps_state;
static char *ProductKey     = "k2806fE3jhn";
static char *DeviceName     = "DANPIANJIDUAN";
static char *Password       = "25905d9f7336b745d30df5161e18ad3863374ba43122913fe794d9cd4eab8bfc";
static char *ClientID       = "k2806fE3jhn.DANPIANJIDUAN|securemode=2,signmethod=hmacsha256,timestamp=1767924126996|";
static char *MQTTHostURL    = "iot-06z00dr932rjmi3.mqtt.iothub.aliyuncs.com";
static char *Port           = "1883";

static void ML307R_Rx_Callback(struct Uart_Rx *Uartx);
static void ML307R_UART_Switch(USART_TypeDef *Uartx, uint8_t mode);
static void ML307R_Send_String(struct Uart_Rx *uartx, char *buf,unsigned char len);
static void ML307R_Clear_RxBuff1(struct Uart_Rx *uartx);
static void ML307R_Clear_RxBuff2(struct Uart_Rx *uartx);

static void ML307R_Init(struct tML307R** uartx, _Bool is_gps);
static uint8_t ML307_Send_Data(struct tML307R **dev, char *data);
static void ML307R_Get_GPS(struct tML307R **dev);
static uint32_t ML307R_RxData_Dispose(struct tML307R **dev);

static uint8_t ML307R_SendCmd(struct Uart_Dev* uart, char *cmd, char *res, uint16_t time);
static int16_t Received_Data_Processing(const char *str);
static char* ML307R_Received_Data_Processing_Str(const char *str);

struct tML307R g_tML307R = {
    .uart.name                       = "ML307R",
    .uart.UART_Dev_Rx_Data           = ML307R_Rx_Callback,
    .uart.UART_Switch                = ML307R_UART_Switch,
    .uart.Clear_RxBuff1              = ML307R_Clear_RxBuff1,
    .uart.Clear_RxBuff2              = ML307R_Clear_RxBuff2,
    .uart.UART_Dev_Send              = ML307R_Send_String,
    .Init                            = ML307R_Init,
    .Send_Data                       = ML307_Send_Data,
    .RxData_Dispose                  = ML307R_RxData_Dispose,
    .Get_GPS                         = ML307R_Get_GPS,
};

static void ML307R_Rx_Callback(struct Uart_Rx *Uartx)
{
    #if (ML307R_UARTx == USE_USART1)
        extern UART_HandleTypeDef huart1;
        HAL_UART_Receive_IT(&huart1, &Uartx->Rxdata, 1);
    #elif (ML307R_UARTx == USE_USART2)
        extern UART_HandleTypeDef huart2;
        HAL_UART_Receive_IT(&huart2, &Uartx->Rxdata, 1);
    #elif (ML307R_UARTx == USE_USART3)
        extern UART_HandleTypeDef huart3;
        HAL_UART_Receive_IT(&huart3, &Uartx->Rxdata, 1);
    #endif
    Uartx->Uart_Buff[Uartx->Uart_i++] = Uartx->Rxdata;
    
    if (gps_state == 1)
    {
        // GPS数据解析状态机（保持原有逻辑）
        switch(Uartx->rx_state)
        {
            case 0:
                if (Uartx->Rxdata == 'M')
                    Uartx->rx_state = 1;
                break;
            case 1:
                if (Uartx->Rxdata == 'L')
                    Uartx->rx_state = 2;
                else
                    Uartx->rx_state = 0;
                break;
            case 2:
                if (Uartx->Rxdata == 'B')
                    Uartx->rx_state = 3;
                else
                    Uartx->rx_state = 0;
                break;
            case 3: 
                if (Uartx->Rxdata == 'S')
                    Uartx->rx_state = 4;
                else
                    Uartx->rx_state = 0;
                break;
            case 4:
                if (Uartx->Rxdata == 'L')
                    Uartx->rx_state = 5;
                else
                    Uartx->rx_state = 0;
                break;
            case 5:
                if (Uartx->Rxdata == 'O')
                    Uartx->rx_state = 6;
                else
                    Uartx->rx_state = 0;
                break;
            case 6:
                if (Uartx->Rxdata == 'C')
                    Uartx->rx_state = 7;
                else
                    Uartx->rx_state = 0;
                break;
            case 7:
                if (Uartx->Rxdata == ':')
                    Uartx->rx_state = 8;
                else
                    Uartx->rx_state = 0;
                break;
            case 8:
                if(Uartx->Rxdata == '\n')
                {
                    Uartx->rx_state = 0;
                    Uartx->rx_flag = 1;
                    Uartx->rx_i = 0;
                    gps_state = 0;
                }
                else
                    Uartx->rx_buff[Uartx->rx_i++] = Uartx->Rxdata;
                break;
        }
    }
    else
    {
        // MQTT数据解析状态机（保持原有逻辑）
        switch(Uartx->rx_state)
        {
            case 0:
                if ((Uartx->Rxdata == 'M') && (Uartx->rx_flag == 0))
                    Uartx->rx_state = 1;
                break;
            case 1:
                if (Uartx->Rxdata == 'Q')
                    Uartx->rx_state = 2;
                else
                    Uartx->rx_state = 0;
                break;
            case 2:
                if (Uartx->Rxdata == 'T')
                    Uartx->rx_state = 3;
                else
                    Uartx->rx_state = 0;
                break;
            case 3: 
                if (Uartx->Rxdata == 'T')
                    Uartx->rx_state = 4;
                else
                    Uartx->rx_state = 0;
                break;
            case 4:
                if (Uartx->Rxdata == 'U')
                    Uartx->rx_state = 5;
                else
                    Uartx->rx_state = 0;
                break;
            case 5:
                if (Uartx->Rxdata == 'R')
                    Uartx->rx_state = 6;
                else
                    Uartx->rx_state = 0;
                break;
            case 6:
                if (Uartx->Rxdata == 'C')
                    Uartx->rx_state = 7;
                else
                    Uartx->rx_state = 0;
                break;
            case 7:
                if(Uartx->Rxdata == '}')
                {
                    Uartx->rx_state = 0;
                    Uartx->rx_flag = 1;
                    Uartx->rx_i = 0;
                }
                else
                    Uartx->rx_buff[Uartx->rx_i++] = Uartx->Rxdata;
                break;
        }
    }

}

static void ML307R_UART_Switch(USART_TypeDef *Uartx, uint8_t mode)
{
    uint8_t dat;
    UART_HandleTypeDef huartx;
    #if (ML307R_UARTx == USE_USART1)
        extern UART_HandleTypeDef huart1;
        HAL_UART_Receive_IT(&huart1, &dat, 1);
        huartx = huart1;
    #elif (ML307R_UARTx == USE_USART2)
        extern UART_HandleTypeDef huart2;
        HAL_UART_Receive_IT(&huart2, &dat, 1);
        huartx = huart2;
    #elif (ML307R_UARTx == USE_USART3)
        extern UART_HandleTypeDef huart3;
        huartx = huart3;
        HAL_UART_Receive_IT(&huart3, &dat, 1);
    #endif
    g_tML307R.uart.t_uart.USARTx    = Uartx;
    g_tML307R.uart.t_uart.Huartx    = huartx;
    g_tML307R.uart.t_uart.mode      = mode;
}

struct tML307R* ML307R_Register(char *dev, USART_TypeDef *Uartx)
{
    strcpy(g_tML307R.uart.name, dev);
    g_tML307R.uart.UART_Switch(Uartx, 0);
    RegisterUart(&g_tML307R.uart);
    return &g_tML307R;
}

static void ML307R_Clear_RxBuff1(struct Uart_Rx *uartx)
{
    memset(uartx->Uart_Buff, 0, sizeof(uartx->Uart_Buff));
    uartx->Uart_i = 0;
}

static void ML307R_Clear_RxBuff2(struct Uart_Rx *uartx)
{
    memset(uartx->rx_buff, 0, sizeof(uartx->rx_buff));
    uartx->rx_i = 0;
}

static void ML307R_Init(struct tML307R** dev, _Bool is_gps)
{
    struct Uart_Dev* uart = &(*dev)->uart;
    char cmd[512]   = {0};
    uint8_t outtime = 0;
    
    OLED_ShowChinese(127 / 2 - (strlen("正在登录服务器")/ 2 * 8), 16, "正在登录服务器");
    OLED_Update();
    
    while(ML307R_SendCmd(uart, "AT+MLPMCFG=\"sleepmode\",0,0\r\n", "OK", 1000) == ML307R_ERROR);
    Delay_ms(100);
    
    while(ML307R_SendCmd(uart, "ATE0\r\n","OK", 1000) == ML307R_ERROR);
    Delay_ms(100);
    
    while(ML307R_SendCmd(uart, "AT+CGATT=1\r\n","OK", 1000) == ML307R_ERROR);
    Delay_ms(100);
    
    while(ML307R_SendCmd(uart, "AT+CPIN?\r\n","READY", 1000) == ML307R_ERROR)
    {
        outtime++;
        if (outtime >= 10)
        {
            OLED_ShowChinese(1, 16, "未检测到");
            OLED_ShowString(64, 16, "SIM", 8);
            OLED_ShowChinese(96, 16, "卡");
            OLED_Update();
        }
        Delay_ms(100);
    }
    
    if (ML307R_SendCmd(uart, "AT+MQTTSTATE=0\r\n","MQTTSTATE: 2", 10000) == ML307R_ERROR)
    {
        while(ML307R_SendCmd(uart, "AT+MQTTCFG=\"pingresp\",0,1\r\n","OK", 1000) == ML307R_ERROR);
        Delay_ms(100);

        sprintf(cmd, "AT+MQTTCONN=0,\"%s\",%s,\"%s\",\"%s&%s\",\"%s\"\r\n", 
                MQTTHostURL, Port, ClientID, DeviceName, ProductKey, Password);
        while(ML307R_SendCmd(uart, cmd, "0,0", 10000) == ML307R_ERROR);
        Delay_ms(100);

        sprintf(cmd, "AT+MQTTSUB=0,\"/%s/%s/user/get\",1\r\n", ProductKey, DeviceName);
        while(ML307R_SendCmd(uart, cmd, "OK", 1000) == ML307R_ERROR);
    }
    Delay_ms(1000);
    
    if (is_gps)
    {
        gps_state = 1;
    }
    else
    {
        gps_state = 0;
    }
}

uint16_t ml307r_rx_len;
//static _Bool ML307R_WaitRecive(struct Uart_Rx *uartx)
//{
//    if(uartx->Uart_i == 0)
//        return ML307R_ERROR;
//        
//    if(uartx->Uart_i == ml307r_rx_len)
//    {
//        uartx->Uart_i = 0;
//        return ML307R_OK;
//    }
//    ml307r_rx_len = uartx->Uart_i;
//    return ML307R_ERROR;
//}
static _Bool ML307R_WaitRecive(struct Uart_Rx *uartx)
{
    static uint32_t timeout_counter = 0;
    
    if(uartx->Uart_i == 0)
        return ML307R_ERROR;
    
    // 检查是否超时（500ms）
    if (timeout_counter++ > 500)
    {
        timeout_counter = 0;
        uartx->Uart_i = 0;
        return ML307R_OK;  // 超时也返回OK，避免死等
    }
    
    if(uartx->Uart_i == ml307r_rx_len)
    {
        timeout_counter = 0;
        uartx->Uart_i = 0;
        return ML307R_OK;
    }
    ml307r_rx_len = uartx->Uart_i;
    return ML307R_ERROR;
}


static void ML307R_Send_String(struct Uart_Rx *uartx, char *bufs, unsigned char len)
{
    HAL_UART_Transmit(&uartx->Huartx, (uint8_t *)bufs, len, 0xffff);
}

//static uint8_t ML307R_SendCmd(struct Uart_Dev* uart, char *cmd, char *res, uint16_t time)
//{
//    uart->UART_Dev_Send(&uart->t_uart, (char *)cmd, strlen((const char *)cmd));
//    
//    while(time--)
//    {
//        if(ML307R_WaitRecive(&uart->t_uart) == ML307R_OK)
//        {
//            if(strstr((const char *)uart->t_uart.Uart_Buff, res) != NULL)
//            {
//                uart->Clear_RxBuff1(&uart->t_uart);
//                return ML307R_OK;
//            }
//        }
//        Delay_ms(1);
//    }
//    return ML307R_ERROR;
//}
static uint8_t ML307R_SendCmd(struct Uart_Dev* uart, char *cmd, char *res, uint16_t time)
{
    // 检查模块是否处于错误状态
    if (strstr((char *)uart->t_uart.Uart_Buff, "CME ERROR") != NULL)
    {
        uart->Clear_RxBuff1(&uart->t_uart);
        return ML307R_ERROR;
    }
    
    uart->UART_Dev_Send(&uart->t_uart, (char *)cmd, strlen((const char *)cmd));
    
    while(time--)
    {
        if(ML307R_WaitRecive(&uart->t_uart) == ML307R_OK)
        {
            char *buff = (char *)uart->t_uart.Uart_Buff;
            
            // 检查是否收到错误
            if(strstr(buff, "CME ERROR") != NULL)
            {
                uart->Clear_RxBuff1(&uart->t_uart);
                return ML307R_ERROR;
            }
            
            if(strstr(buff, res) != NULL)
            {
                uart->Clear_RxBuff1(&uart->t_uart);
                return ML307R_OK;
            }
        }
        Delay_ms(1);
    }
    return ML307R_ERROR;
}

static uint8_t ML307_Send_Data(struct tML307R **dev, char *data)
{
    struct Uart_Dev* uart = &(*dev)->uart;
    char cmd[256] = {0};
    uint16_t len = strlen(data);
    static uint8_t retry_count = 0;
    uint8_t max_retry = 3;
    
    sprintf(cmd, "AT+MQTTPUB=0,\"/%s/%s/user/%s\",0,0,0,%u,\"%s\"\r\n", 
            ProductKey, DeviceName, DeviceName, len, data);
    
    for(retry_count = 0; retry_count < max_retry; retry_count++)
    {
        if(ML307R_SendCmd(uart, cmd, "OK", 1200) == ML307R_OK)
        {
            return ML307R_OK;
        }
        Delay_ms(200);
    }
    
    return ML307R_ERROR;
}
// 在ML307R.c中实现
void ML307R_Reconnect(struct Uart_Dev **uart)
{
    char cmd[512] = {0};
    
    // 1. 断开当前连接
    (*uart)->UART_Dev_Send(&(*uart)->t_uart, "AT+MQTTDISC=0\r\n", 16);
    Delay_ms(1000);
    
    // 2. 重新连接
    sprintf(cmd, "AT+MQTTCONN=0,\"%s\",%s,\"%s\",\"%s&%s\",\"%s\"\r\n", 
            MQTTHostURL, Port, ClientID, DeviceName, ProductKey, Password);
    (*uart)->UART_Dev_Send(&(*uart)->t_uart, (char *)cmd, strlen((const char *)cmd));
    Delay_ms(2000);
    
    // 3. 重新订阅
    sprintf(cmd, "AT+MQTTSUB=0,\"/%s/%s/user/get\",1\r\n", ProductKey, DeviceName);
    (*uart)->UART_Dev_Send(&(*uart)->t_uart, (char *)cmd, strlen((const char *)cmd));
    Delay_ms(100);
    
    // 清空接收缓冲区
    (*uart)->Clear_RxBuff1(&(*uart)->t_uart);
    (*uart)->Clear_RxBuff2(&(*uart)->t_uart);
    (*uart)->t_uart.rx_flag = 0;
    (*uart)->t_uart.rx_state = 0;
    (*uart)->t_uart.rx_i = 0;
}

//static uint8_t ML307_Send_Data(struct tML307R **dev, char *data)
//{
//    struct Uart_Dev* uart = &(*dev)->uart;
//    char cmd[256] = {0};
//    uint16_t len = strlen(data);
//    static uint8_t retry_count = 0;
//    uint8_t max_retry = 3;
//    
//    sprintf(cmd, "AT+MQTTPUB=0,\"/%s/%s/user/%s\",0,0,0,%u,\"%s\"\r\n", 
//            ProductKey, DeviceName, DeviceName, len, data);
//    
//    for(retry_count = 0; retry_count < max_retry; retry_count++)
//    {
//        if(ML307R_SendCmd(uart, cmd, "OK", 2000) == ML307R_OK)
//        {
//            // 重要：发送成功后，重新订阅主题
//            char sub_cmd[200] = {0};
//            sprintf(sub_cmd, "AT+MQTTSUB=0,\"/%s/%s/user/get\",1\r\n", ProductKey, DeviceName);
//            
//            // 重新订阅，不等待OK，避免阻塞
//            uart->UART_Dev_Send(&uart->t_uart, (char *)sub_cmd, strlen((const char *)sub_cmd));
//            
//            // 短暂延时，让模块处理订阅命令
//            Delay_ms(50);
//            
//            return ML307R_OK;
//        }
//        Delay_ms(200);
//    }
//    
////    // 如果发送失败，尝试重新连接
////    ML307R_Reconnect(&uart);
//    
//    return ML307R_ERROR;
//}

static int16_t Received_Data_Processing(const char *str)
{
    static char temp[10] = {0};
    uint8_t temp_i = 0;
    char *p1 = NULL;
    memset(temp, 0, sizeof(temp));
    
    if ((p1 = strstr((char *)g_tML307R.uart.t_uart.rx_buff, str)) != NULL)
    {
        p1 += strlen(str) + 3;  // 跳过": "
        while(*p1 != ',' && *p1 != '}')
        {
            temp[temp_i++] = *p1;
            p1++;
        }
        temp[temp_i] = '\0';
        return atoi(temp);
    }
    return ML307R_WAIT;
}

static char* ML307R_Received_Data_Processing_Str(const char *str)
{
    static char temp[20] = {0};
    memset(temp, 0, sizeof(temp));
    uint8_t temp_i = 0;
    char *p1 = NULL;
    
    if ((p1 = strstr((char *)g_tML307R.uart.t_uart.rx_buff, str)) != NULL)
    {
        p1 += strlen(str) + 3;  // 跳过":\""
        while(*p1 != '\"' && *p1 != ',' && *p1 != '}')
        {
            temp[temp_i++] = *p1;
            p1++;
        }
        temp[temp_i] = '\0';
        return temp;
    }
    return " ";
}

extern struct plate_time stall_stop_time[2];
extern uint8_t stall_[2];
extern uint16_t money;

uint32_t ML307R_RxData_Dispose(struct tML307R **dev)
{
    struct Uart_Dev* uart = &(*dev)->uart;
    int16_t value;
    char temp[50] = {0};
    
    if (uart->t_uart.rx_flag == 1)
    {
        OLED_Clear();
        
        // 解析money字段
        value = Received_Data_Processing("money");
        if (value != ML307R_WAIT)
            money = value;
        
        // 解析stall_1字段
        value = Received_Data_Processing("stall_1");
        if (value != ML307R_WAIT)
        {
            // 修改1：直接更新状态，移除无效检查
            uint8_t new_state = value;
            
            // 状态有效性检查（只允许0-4的状态）
            if (new_state <= 4)
            {
                // 修改2：添加状态转换逻辑
                if (new_state == 3)
                {
                    // 状态3：车辆停放完成，等待缴费
                    // 允许从状态2（已占用）转换到状态3（等待缴费）
                    if (stall_[0] == 2)
                    {
                        stall_[0] = new_state;
                        Delay_ms(500);
                    }
                }
                else if (new_state == 1)
                {
                    // 状态1：预约状态
                    stall_[0] = new_state;
                    
                    // 处理车位1的预约信息
                    memset(stall_stop_time[0].yuyue_font_unicode, 0, sizeof(stall_stop_time[0].yuyue_font_unicode));
                    memset(stall_stop_time[0].yuyue_number, 0, sizeof(stall_stop_time[0].yuyue_number));
                    
                    strcpy(temp, ML307R_Received_Data_Processing_Str("font"));
                    if (strcmp(temp, " ") != 0)
                    {
                        strcpy(stall_stop_time[0].yuyue_font_unicode, temp);
                        stall_stop_time[0].yuyue_font_unicode[strlen(stall_stop_time[0].yuyue_font_unicode)] = '\0';
                        stall_stop_time[0].yuyue_flag = PLATE_Y;
                    }
                    
                    strcpy(temp, ML307R_Received_Data_Processing_Str("number"));
                    if (strcmp(temp, " ") != 0)
                    {
                        strcpy(stall_stop_time[0].yuyue_number, temp);
                        stall_stop_time[0].yuyue_number[strlen(stall_stop_time[0].yuyue_number)] = '\0';
                    }
                }
                else
                {
                    // 其他状态（0,2,4）
                    stall_[0] = new_state;
                }
            }
        }
        
        // 解析stall_2字段（同样修改）
        value = Received_Data_Processing("stall_2");
        if (value != ML307R_WAIT)
        {
            uint8_t new_state = value;
            
            if (new_state <= 4)
            {
                if (new_state == 3)
                {
                    if (stall_[1] == 2)
                    {
                        stall_[1] = new_state;
                        Delay_ms(500);
                    }
                }
                else if (new_state == 1)
                {
                    stall_[1] = new_state;
                    
                    memset(stall_stop_time[1].yuyue_font_unicode, 0, sizeof(stall_stop_time[1].yuyue_font_unicode));
                    memset(stall_stop_time[1].yuyue_number, 0, sizeof(stall_stop_time[1].yuyue_number));
                    
                    strcpy(temp, ML307R_Received_Data_Processing_Str("font"));
                    if (strcmp(temp, " ") != 0)
                    {
                        strcpy(stall_stop_time[1].yuyue_font_unicode, temp);
                        stall_stop_time[1].yuyue_font_unicode[strlen(stall_stop_time[1].yuyue_font_unicode)] = '\0';
                        stall_stop_time[1].yuyue_flag = PLATE_Y;
                    }
                    
                    strcpy(temp, ML307R_Received_Data_Processing_Str("number"));
                    if (strcmp(temp, " ") != 0)
                    {
                        strcpy(stall_stop_time[1].yuyue_number, temp);
                        stall_stop_time[1].yuyue_number[strlen(stall_stop_time[1].yuyue_number)] = '\0';
                    }
                }
                else
                {
                    stall_[1] = new_state;
                }
            }
        }
        
        
        // 清空缓冲区
        uart->Clear_RxBuff2(&uart->t_uart);
        uart->t_uart.rx_flag = 0;
        return ML307R_OK;
    }
    return ML307R_ERROR;
}
static void ML307R_Get_GPS(struct tML307R **dev)
{
    struct tML307R* temp_dev = (*dev);
    struct Uart_Dev* uart = &(*dev)->uart;
    
    while(ML307R_SendCmd(uart, "AT+MLBSCFG=\"method\",40\r\n", "OK", 1000) == ML307R_ERROR);
    while(ML307R_SendCmd(uart, "AT+MLBSLOC\r\n", "+MLBSLOC", 1000) == ML307R_ERROR);
    
    while (uart->t_uart.rx_flag == 0);
    
    temp_dev->GPS.longitude = 0;
    temp_dev->GPS.latitude  = 0;
    sscanf((char*)uart->t_uart.rx_buff, " 100,%lf,%lf", &temp_dev->GPS.longitude, &temp_dev->GPS.latitude);
    uart->t_uart.rx_flag = 0;
}

// 在ML307R.c中实现
void ML307R_Reset_Module_State(void)
{
    // 发送空命令AT，重置模块状态
    g_tML307R.uart.UART_Dev_Send(&g_tML307R.uart.t_uart, "AT\r\n", 4);
    
    // 清空接收缓冲区
    g_tML307R.uart.Clear_RxBuff1(&g_tML307R.uart.t_uart);
    g_tML307R.uart.Clear_RxBuff2(&g_tML307R.uart.t_uart);
    
    // 重置状态机
    g_tML307R.uart.t_uart.rx_flag = 0;
    g_tML307R.uart.t_uart.rx_state = 0;
    g_tML307R.uart.t_uart.rx_i = 0;
    
    // 短暂延时，让模块恢复
    Delay_ms(100);
}

