/*
 * @file     cmd_fn.c
 * @brief    AT command processing
 * @author   Ai-Thinker
 * @date    2024.5.13
 */

#include "cmd_fn.h"
#include "deca_version.h"
#include "config.h"
#include "node.h"
#include "tag.h"
#include "Generic.h"
#include "lis2dh12.h"
#include "OLED_I2C.h"
#include "shared_functions.h"
//-----------------------------------------------------------------------------
/*
 * 功能:获取版本
 *
 * */
int f_getver (int opt, int argc, char* argv[])
{
    char str[64];
    memset (str, 0, sizeof (str));

    sprintf (str, "\r\ngetver software:%s\r\n", uwb_software_ver); // reserve space for length of JS object
    port_tx_msg (str, strlen (str));
    return 0;
}


/*
 * 功能:保存
 *
 * */
int f_save (int opt, int argc, char* argv[])
{
    App_Module_Sys_Write_NVM();
    _dbg_printf ("\r\nOK\r\n");
    HalDelay_nMs (100);
    NVIC_SystemReset();
    return 0;
}



/*
 * 功能:恢复出厂模式
 *
 * */
int f_restore (int opt, int argc, char* argv[])
{
    App_Module_sys_para_Init();
    App_Module_Sys_Write_NVM();
    _dbg_printf ("\r\nOK\r\n");
    HalDelay_nMs (100);
    NVIC_SystemReset();
    return 0;
}


/*
 * 功能:复位模块
 *
 * */
int f_restart (int opt, int argc, char* argv[])
{
    _dbg_printf ("\r\nOK\r\n");
    NVIC_SystemReset();
    return 0;
}


/*
 * 功能:设置配置信息
 *
 * */
int f_setcfg (int opt, int argc, char* argv[])
{
    if (opt == SET_CMD)
    {
        char str[256];
        memset (str, 0, sizeof (str));
        uint8_t role = 0, ch = 0, rate = 0, id = 0;

        id = atoi (argv[0]);
        role = atoi (argv[1]);
        ch = atoi (argv[2]);
        rate = atoi (argv[3]);

        if ( (role == 0 || role == 1) &&
                (ch == 0  || ch == 1)   &&
                (rate == 0 || rate == 1) &&
                (argc == 4))
        {
            sys_para.param_Config.s.userConfig.role = role;

            if (ch == 0)
            {
                sys_para.param_Config.dwt_config.chan = 9;
            }
            else
            {
                sys_para.param_Config.dwt_config.chan = 5;
            }

            if (rate == 1)
            {
                sys_para.param_Config.dwt_config.dataRate = DWT_BR_6M8;
            }
            else
            {
                sys_para.param_Config.dwt_config.dataRate = DWT_BR_850K;
            }

            if (sys_para.param_Config.s.userConfig.role == 1) //node
            {
                if ( (id <= MAX_ANCHOR_LIST_SIZE))
                {
                    sys_para.param_Config.s.userConfig.nodeAddr = id;
                    sprintf (str, "\r\nsetcfg ID:%d, Role:%d, CH:%d, Rate:%d\r\n",
                             sys_para.param_Config.s.userConfig.nodeAddr, sys_para.param_Config.s.userConfig.role, ch, rate);
                    port_tx_msg (str, strlen (str));
                    _dbg_printf ("\r\nOK\r\n");

                    node_start();
                }
            }
            else  //tag
            {
                if ( (id <= MAX_TAG_LIST_SIZE))
                {
                    sys_para.param_Config.s.userConfig.nodeAddr = id;

                    sprintf (str, "\r\nsetcfg ID:%d, Role:%d, CH:%d, Rate:%d\r\n",
                             sys_para.param_Config.s.userConfig.nodeAddr, sys_para.param_Config.s.userConfig.role, ch, rate);
                    port_tx_msg (str, strlen (str));
                    _dbg_printf ("\r\nOK\r\n");

                    tag_start();
                }

            }

            return 0;
        }
        else
        {
            return -1;
        }
    }
}

/*
 * 功能:获取配置信息
 *
 * */
int f_getcfg (int opt, int argc, char* argv[])
{
    uint8_t ch = 0, rate = 0;
    char str[256];
    memset (str, 0, sizeof (str));

    if (sys_para.param_Config.dwt_config.chan == 9)
    {
        ch = 0;
    }
    else
    {
        ch = 1;
    }

    if (sys_para.param_Config.dwt_config.dataRate == DWT_BR_6M8)
    {
        rate = 1;
    }
    else
    {
        rate = 0;
    }

    sprintf (str, "\r\ngetcfg ID:%d, Role:%d, CH:%d, Rate:%d\r\n",
             sys_para.param_Config.s.userConfig.nodeAddr, sys_para.param_Config.s.userConfig.role, ch, rate);
    port_tx_msg (str, strlen (str));
    return 0;
}

/*
 * 功能:设置工作模式
 *
 * */
int f_setworkmode (int opt, int argc, char* argv[])
{
    char str[64];
    memset (str, 0, sizeof (str));
    uint8_t mode = 0;

    mode = atoi (argv[0]);

    if ( (mode == 0 || mode == 1) &&
            (argc == 1))
    {
        sys_para.param_Config.s.userConfig.workmode = mode;

        if (sys_para.param_Config.s.userConfig.workmode == 1)
        {
            OLED_CLS();
            //HalLed_Mode_Set(HAL_LED1, HAL_LED_MODE_OFF, Sys_mode_led_blink_invalid);
            //dwt_setleds(DWT_LEDS_DISABLE | DWT_LEDS_INIT_BLINK) ;
        }

        sprintf (str, "\r\nworkmode: %d\r\n", sys_para.param_Config.s.userConfig.workmode);
        port_tx_msg (str, strlen (str));
#ifdef JOHHN
#else
        App_Module_Sys_Write_NVM();
        _dbg_printf ("\r\nOK\r\n");
        HalDelay_nMs (100);
        NVIC_SystemReset();
#endif
        return 0;
    }

    return -1;
}

/*
 * 功能:获取工作模式
 *
 * */
int f_getworkmode (int opt, int argc, char* argv[])
{
    char str[64];
    memset (str, 0, sizeof (str));

    sprintf (str, "\r\nworkmode: %d\r\n", sys_para.param_Config.s.userConfig.workmode);
    port_tx_msg (str, strlen (str));
    return 0;
}

/*
 * 功能:设置基本参数
 *
 * */
int f_setdev (int opt, int argc, char* argv[])
{
    uint8_t str[256];
    memset (str, 0, sizeof (str));

    //(配置标签容量,天线延迟,滤波启动,滤波系数Q,滤波系数R,校正系数a,校正系数b,定位启动,定位维度)
    uint8_t cap = 0, kalman_enable = 0, loc_enable = 0, loc_dimen = 0;
    float kalman_Q, kalman_R, para_a, para_b;
    int antdelay = 0;

    cap = atoi (argv[0]);
    kalman_enable = atoi (argv[2]);
    loc_enable = atoi (argv[7]);
    loc_dimen = atoi (argv[8]);
    kalman_Q = atof (argv[3]);
    kalman_R = atof (argv[4]);
    para_a = atof (argv[5]);
    para_b = atof (argv[6]);
    antdelay = atoi (argv[1]);

    if ( (argc == 9))
    {
        app.pConfig->s.userConfig.tagnumSlots = cap;            //最大标签数
        app.pConfig->s.baseConfig.antRx = antdelay;         //天线延迟
        app.pConfig->s.baseConfig.antTx = antdelay;         //天线延迟
        app.pConfig->s.userConfig.nodeFilter = kalman_enable;   //是否滤波
        app.pConfig->s.userConfig.kalman_Q = kalman_Q;      //滤波系数Q
        app.pConfig->s.userConfig.kalman_R = kalman_R;      //滤波系数R
        app.pConfig->s.userConfig.para_a = para_a;          //标定系数a
        app.pConfig->s.userConfig.para_b = para_b;          //标定系数b

//      instance_config_antennadelays(sys_para.dist.AntennaDelay,sys_para.dist.AntennaDelay);
//
//
        sprintf (str, "\r\nsetdev cap:%d anndelay:%d, kalman_enable:%d, kalman_Q:%.3f, kalman_R:%.3f, para_a:%.4f, para_b:%.2f, pos_enable:%d, pos_dimen:%d\r\n",
                 app.pConfig->s.userConfig.tagnumSlots, app.pConfig->s.baseConfig.antRx, app.pConfig->s.userConfig.nodeFilter,
                 app.pConfig->s.userConfig.kalman_Q, app.pConfig->s.userConfig.kalman_R,
                 app.pConfig->s.userConfig.para_a, app.pConfig->s.userConfig.para_b,
                 0, 0);

        port_tx_msg (str, strlen (str));
        return 0;
    }

    return -1;
}


/*
 * 功能:获取基本参数
 *
 * */
int f_getdev (int opt, int argc, char* argv[])
{
    //(获取标签容量,天线延迟,滤波启动,滤波系数Q,滤波系数R,校正系数a,校正系数b,定位启动,定位维度)
    char str[256];
    memset (str, 0, sizeof (str));

    sprintf (str, "\r\ngetdev cap:%d anndelay:%d, kalman_enable:%d, kalman_Q:%.3f, kalman_R:%.3f, para_a:%.4f, para_b:%.2f, pos_enable:%d, pos_dimen:%d\r\n",
             app.pConfig->s.userConfig.tagnumSlots, app.pConfig->s.baseConfig.antRx, app.pConfig->s.userConfig.nodeFilter,
             app.pConfig->s.userConfig.kalman_Q, app.pConfig->s.userConfig.kalman_R,
             app.pConfig->s.userConfig.para_a, app.pConfig->s.userConfig.para_b,
             0, 0);

    port_tx_msg (str, strlen (str));
    return 0;
}

/*
 *  作用：AT指令测试
 *
 * */
int f_test (int opt, int argc, char* argv[])
{
    return 0;
}

/*
 *  作用：获取加速度传感器结果
 *
 * */
int f_getsensor (int opt, int argc, char* argv[])
{
    char str[64];
    float speed = 0;
    int ret = 0;
    memset (str, 0, sizeof (str));

    ret = drv_lis2dh12_get_angle (&speed);

    if (ret != 0)
    {
        return -1;
    }

    sprintf (str, "\r\nangle: %f\r\n", speed);
    port_tx_msg (str, strlen (str));
    return 0;
}


/*
 *  作用：led测试
 *
 * */
int f_testled (int opt, int argc, char* argv[])
{
    char str[64];
    memset (str, 0, sizeof (str));
    uint8_t mode = 0;

    mode = atoi (argv[0]);

    if ( (mode == 0 || mode == 1) &&
            (argc == 1))
    {
        if (mode == 1)
        {
            HalLed_Mode_Set (HAL_LED1, HAL_LED_MODE_OFF, Sys_mode_led_blink_invalid);
            led_test = true;
        }
        else
        {
            led_test = false;
        }

        flow_light (led_test);
    }

    return 0;
}

/*
 *  作用：去除\r\n字符
 *
 * */

void remove_newline_chars (char* str)
{
    char* src = str;
    char* dst = str;

    while (*src)
    {
        if (*src != '\r' && *src != '\n')
        {
            *dst = *src;
            dst++;
        }

        src++;
    }

    *dst = '\0';
}

/*
 *  作用：显示屏测试
 *
 * */
int f_testoled (int opt, int argc, char* argv[])
{
    remove_newline_chars (argv[0]);

    if (argc == 1)
    {
        OLED_CLS();
        OLED_ShowStr (19, 3, argv[0], 2);
    }

    return 0;
}

float distance = 0;
/*
 *  作用：测距
 *
 * */
int f_distance (int opt, int argc, char* argv[])
{
    char str[64];
    memset (str, 0, sizeof (str));

    sprintf (str, "\r\ndistance: %f\r\n", distance);
    port_tx_msg (str, strlen (str));
    return 0;
}

/*PDOA*/
/*
 * 功能:上位机通信命令
 *
 * */
int f_deca (int opt, int argc, char* argv[])
{
    char str[256];
    memset (str, 0, sizeof (str));

    int  hlen;
    hlen = sprintf (str, "JS%04X", 0x5A5A);  // reserve space for length of JS object

    sprintf (&str[strlen (str)], "{\"Info\":{\r\n");
    sprintf (&str[strlen (str)], "\"Device\":\"PDOA Node\",\r\n");
    sprintf (&str[strlen (str)], "\"Version\":\"%s\",\r\n", uwb_software_ver);
    sprintf (&str[strlen (str)], "\"Build\":\"%s %s\",\r\n", __DATE__, __TIME__);
    sprintf (&str[strlen (str)], "\"Driver\":\"%s\"}}", DW3000_DEVICE_DRIVER_VER_STRING);

    sprintf (&str[2], "%04X", strlen (str) - hlen); //add formatted 4X of length, this will erase first '{'
    str[hlen] = '{';                          //restore the start bracket
    sprintf (&str[strlen (str)], "\r\n");
    port_tx_msg ( (uint8_t*) str, strlen (str));
    return 0;
}

/*
 * 作用：获取[发现列表]
 *
 * */
int f_getdlist (int opt, int argc, char* argv[])
{
    uint64_t*    pAddr64;
    uint16_t     size;
    int          jlen, tmp;
    int offset = 0;

    pAddr64     = getDList();
    size        = getDList_size();

    char str[MAX_STR_SIZE];
    memset (str, 0, sizeof (str));

    jlen = (11 + 2);

    if (size > 0)
    {
        tmp = 0;
        tmp = strlen ("\"1122334455667788\"");   //16+2 for every addr64
        jlen += (tmp + 3) * size - 3;
    }

    sprintf (str, "JS%04X", jlen);                 // print pre-calculated length of JS object

    offset += snprintf (str + offset, sizeof (str) - offset, "{\"DList\":[ ");

    //DList cannot be with gaps
    // if changed, will need to change the calculation of jlen
    while (size > 0)
    {
        offset += snprintf (str + offset, sizeof (str) - offset, "\"%08lX%08lX\"", (uint32_t) (*pAddr64 >> 32), (uint32_t) (*pAddr64));

        if (size > 1)
        {
            offset += snprintf (str + offset, sizeof (str) - offset, ",\r\n");
        }

        pAddr64++;
        size--;
    }

    offset += snprintf (str + offset, sizeof (str) - offset, "]}");
    port_tx_msg ( (uint8_t*) str, offset);

    initDList();                                    //clear the Discovered list

    return 0;
}

static void fill_json_tag (char* str, tag_addr_slot_t* tag)
{
    sprintf (str,
             "{\"slot\":\"%04X\",\"a64\":\"%08lX%08lX\",\"a16\":\"%04X\","
             "\"F\":\"%04X\",\"S\":\"%04X\",\"M\":\"%04X\"}",
             tag->slot, (uint32_t) (tag->addr64 >> 32), (uint32_t) (tag->addr64), tag->addr16,
             tag->multFast, tag->multSlow, tag->mode);
}

/*
 *  作用：获取[配对列表]
 *
 * */
int f_getklist (int opt, int argc, char* argv[])
{
    tag_addr_slot_t*    tag;
    int                jlen, size, tmp;
    int offset = 0;

    char str[MAX_STR_SIZE];
    memset (str, 0, sizeof (str));

    /*  16 bytes overhead for JSON
       *  66 bytes per known tag
       *  3 bytes per separation
       *  we need to pre-calculate the length of json string for DList & KList : in order to keep malloc() small
       */
    tag = get_knownTagList();
    size = get_knownTagList_size();

    jlen = (10 + 2);

    if (size > 0)
    {
        tmp = 0;
        fill_json_tag (str, tag);
        tmp = strlen (str);                           //+NN to json for every known tag
        jlen += (tmp + 3) * size - 3;
    }

    sprintf (str, "JS%04X", jlen);                      // 6 print pre-calculated length of JS object
    offset += snprintf (str + offset, sizeof (str) - offset, "{\"KList\":[");

    //KList can be with gaps, so need to scan it whole
    for (int i = 0; i < MAX_KNOWN_TAG_LIST_SIZE; i++)
    {
        if (tag->slot != (uint16_t) (0))
        {
            fill_json_tag (str + offset, tag + i);                   //NN
            int tag_len = strlen (str + offset);
            offset += tag_len;

            if (size > 1)   //last element should not have ',\r\n'
            {
                offset += snprintf (str + offset, sizeof (str) - offset, ",\r\n");
            }

            size--;
        }

        tag++;
    }

    offset += snprintf (str + offset, sizeof (str) - offset, "]}");
    port_tx_msg ( (uint8_t*) str, offset);

    return 0;
}

/*
 *  作用：增加标签到[配对列表]
 *
 * */
int f_addtag (int opt, int argc, char* argv[])
{
    char str[MAX_STR_SIZE];
    memset (str, 0, sizeof (str));

    tag_addr_slot_t*    tag;
    uint64_t           addr64 = 0;
    unsigned int       addr16 = 0, multFast = 0, multSlow = 0, mode = 0, n = 1, hlen;

    addr64 = strtoull (argv[0], NULL, 16);
    addr16 = strtoul (argv[1], NULL, 16);
    multFast = atoi (argv[2]);
    multSlow = atoi (argv[3]);
    mode = atoi (argv[4]);

    if (! (multFast == 0) && ! (multSlow == 0) && (argc == 5))
    {
        tag = add_tag_to_knownTagList (addr64, (uint16_t) addr16);

        if (tag)
        {
            tag->multFast = (uint16_t) multFast;
            tag->multSlow = (uint16_t) multSlow;
            tag->mode = (uint16_t) mode;

            hlen = sprintf (str, "JS%04X", 0x5A5A);  // reserve space for length of JS object
            sprintf (&str[strlen (str)], "{\"TagAdded\": ");
            fill_json_tag (&str[strlen (str)], tag);
            sprintf (&str[strlen (str)], "}"); //\r\n

            sprintf (&str[2], "%04X", strlen (str) - hlen); //add formatted 4X of length, this will kill first '{'
            str[hlen] = '{';                          //restore the start bracket

            sprintf (&str[strlen (str)], "\r\n");
            port_tx_msg ( (uint8_t*) str, strlen (str));

            return 0;
        }

        return 0;
    }

    return -1;
}

/*
 *  作用：删除标签到[配对列表]
 *
 * */
int f_deltag (int opt, int argc, char* argv[])
{
    const char* ret = NULL;

    char str[MAX_STR_SIZE];
    memset (str, 0, sizeof (str));


    uint64_t        addr64 = 0;
    unsigned int    hlen;

    /* "delTag 11AABB4455FF7788" */
    addr64 = strtoull (argv[0], NULL, 16);

    if (argc == 1)
    {
        if (addr64 > 0xFFFF)
        {
            del_tag64_from_knownTagList (addr64);
        }
        else
        {
            del_tag16_from_knownTagList ( (uint16_t) addr64);
        }

        hlen = sprintf (str, "JS%04X", 0x5A5A);  // reserve space for length of JS object
        sprintf (&str[strlen (str)], "{\"TagDeleted\": \"%16llx\"}", addr64);

        sprintf (&str[2], "%04X", strlen (str) - hlen); //add formatted 4X of length, this will erase first '{'
        str[hlen] = '{';                          //restore the start bracket
        sprintf (&str[strlen (str)], "\r\n");
        port_tx_msg ( (uint8_t*) str, strlen (str));

        return 0;
    }

    return -1;
}

/*
 *  作用：角度修正值
 *
 * */
int f_pdoaoff (int opt, int argc, char* argv[])
{
    if (argc == 1)
    {
        app.pConfig->s.s_pdoa.pdoaOffset_deg = (int16_t) atoi (argv[0]);
        return 0;
    }

    return -1;
}

/*
 *  作用：距离修正值
 *
 * */
int f_rngoff (int opt, int argc, char* argv[])
{
    if (argc == 1)
    {
        app.pConfig->s.s_pdoa.rngOffset_mm = (int16_t) atoi (argv[0]);
        return 0;
    }

    return -1;
}

/*
 *  作用：是否开启滤波
 *
 * */
int f_filter (int opt, int argc, char* argv[])
{
    if (argc == 1)
    {
        app.pConfig->s.s_pdoa.motionfilter  = (int16_t) atoi (argv[0]);
        return 0;
    }

    return -1;
}

/*
 *  作用：设置配置参数(发现列表标签数量,绑定列表标签数量,基站PANID,基站设备ID,串口刷新速率)
 *
 * */
int f_pdoasetcfg (int opt, int argc, char* argv[])
{
    char str[256];
    uint16_t DiscList_num, BindList_num, panID, AncID, serail_Rate, filter, user_cmd;
    int n = 0;
    char tmp[10];
    memset (str, 0, sizeof (str));


    DiscList_num = atoi (argv[0]);
    BindList_num = atoi (argv[1]);
    panID = atoi (argv[2]);
    AncID = atoi (argv[3]);
    serail_Rate = atoi (argv[4]);
    filter = atoi (argv[5]);
    user_cmd = atoi (argv[6]);

    if ( (argc == 7) && DiscList_num <= MAX_DISCOVERED_TAG_LIST_SIZE &&
            BindList_num <= MAX_KNOWN_TAG_LIST_SIZE && AncID <= 1)
    {

//      pbss->s.Dlist = DiscList_num;
//      pbss->s.Klist = BindList_num;

        app.pConfig->s.s_pdoa.addr = (int16_t) AncID;
        app.pConfig->s.s_pdoa.panID = (uint16_t) (panID);
        app.pConfig->s.s_pdoa.UartReFreshRate = serail_Rate;
        app.pConfig->s.s_pdoa.motionfilter = filter;
        app.pConfig->s.s_pdoa.user_cmd = user_cmd;
        sprintf (str, "getcfg Dlist:%d KList:%d Net:%04X AncID:%d Rate:%d Filter:%d UserCmd:%d pdoaOffset:%d rngOffset:%d\r\n",
                 app.pConfig->s.s_pdoa.Dlist,
                 app.pConfig->s.s_pdoa.Klist,
                 app.pConfig->s.s_pdoa.panID,
                 app.pConfig->s.s_pdoa.addr,
                 app.pConfig->s.s_pdoa.UartReFreshRate,
                 app.pConfig->s.s_pdoa.motionfilter,
                 app.pConfig->s.s_pdoa.user_cmd,
                 app.pConfig->s.s_pdoa.pdoaOffset_deg,
                 app.pConfig->s.s_pdoa.rngOffset_mm);

        port_tx_msg (str, strlen (str));

        return 0;
    }

    return -1;
}


/*
 *  作用：获取配置参数(发现列表标签数量,绑定列表标签数量,基站PANID,基站设备ID,串口刷新速率)
 *
 * */
int f_pdoagetcfg (int opt, int argc, char* argv[])
{
    char str[256];
    memset (str, 0, sizeof (str));

    sprintf (str, "getcfg Dlist:%d KList:%d Net:%04X AncID:%d Rate:%d Filter:%d UserCmd:%d pdoaOffset:%d rngOffset:%d\r\n",
             app.pConfig->s.s_pdoa.Dlist,
             app.pConfig->s.s_pdoa.Klist,
             app.pConfig->s.s_pdoa.panID,
             app.pConfig->s.s_pdoa.addr,
             app.pConfig->s.s_pdoa.UartReFreshRate,
             app.pConfig->s.s_pdoa.motionfilter,
             app.pConfig->s.s_pdoa.user_cmd,
             app.pConfig->s.s_pdoa.pdoaOffset_deg,
             app.pConfig->s.s_pdoa.rngOffset_mm);

    port_tx_msg (str, strlen (str));

    return 0;
}

/*
 *  作用：修改串口速率
 *
 * */
int f_uart_rate (int opt, int argc, char* argv[])
{
    if (argc == 1)
    {
        app.pConfig->s.s_pdoa.UartReFreshRate   = (int16_t) atoi (argv[0]);
        return 0;
    }

    return -1;
}

/*
 *  作用：命令格式选择 0:Json格式 1:Hex格式
 *
 * */
int f_user_cmd (int opt, int argc, char* argv[])
{
    if (argc == 1)
    {
        app.pConfig->s.s_pdoa.user_cmd = (int16_t) atoi (argv[0]);
        return 0;
    }

    return -1;
}

/*
 * 功能:设置算法模式
 *
 * */
int f_setuwbmode (int opt, int argc, char* argv[])
{
    char str[64];
    memset (str, 0, sizeof (str));
    uint8_t mode = 0;

    mode = atoi (argv[0]);

    if ( (mode == 0 || mode == 1) &&
            (argc == 1))
    {
        sys_para.param_Config.s.userConfig.twr_pdoa_mode = mode;
        port_tx_msg (str, strlen (str));
        return 0;
    }

    return -1;
}

/*
 * 功能:获取算法模式
 *
 * */
int f_getewbmode (int opt, int argc, char* argv[])
{
    char str[64];
    memset (str, 0, sizeof (str));

    sprintf (str, "\r\ntwr_pdoa_mode: %d\r\n", sys_para.param_Config.s.userConfig.twr_pdoa_mode);
    port_tx_msg (str, strlen (str));
    return 0;
}

//-----------------------------------------------------------------------------
/*
 * 命令集
 */
//-----------------------------------------------------------------------------
const command_t known_commands [] =
{
    /*TWR*/
    {"AT+GETVER", f_getver},
    {"AT+SAVE", f_save},
    {"AT+RESTART", f_restart},
    {"AT+RESTORE", f_restore},
    {"AT+GETCFG", f_getcfg},
    {"AT+SETCFG", f_setcfg},
    {"AT+GETDEV", f_getdev},
    {"AT+SETDEV", f_setdev},
    {"AT+GETWORKMODE", f_getworkmode},
    {"AT+SETWORKMODE", f_setworkmode},
    {"AT+GETSENSOR",   f_getsensor},
    {"AT+TESTLED",   f_testled},
    {"AT+TESTOLED",   f_testoled},
    {"AT+DISTANCE",   f_distance},
    /*PDOA*/
    {"AT+DECA$",   f_deca},
    {"AT+GETDLIST",   f_getdlist},
    {"AT+GETKLIST",   f_getklist},
    {"AT+ADDTAG",   f_addtag},
    {"AT+DELTAG",   f_deltag},
    {"AT+PDOAOFF",   f_pdoaoff},
    {"AT+RNGOFF",   f_rngoff},
    {"AT+FILTER",   f_filter},
    {"AT+PDOASETCFG",   f_pdoasetcfg},
    {"AT+PDOAGETCFG",   f_pdoagetcfg},
    {"AT+UARTRATE", f_uart_rate},
    {"AT+USER_CMD", f_user_cmd},
    /*切换*/
    {"AT+GETUWBMODE", f_getewbmode},
    {"AT+SETUWBMODE", f_setuwbmode},

    {"AT", f_test},
    {NULL, NULL}
};