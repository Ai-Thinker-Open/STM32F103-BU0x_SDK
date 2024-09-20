/**********************************************************************************
 * @file    user_out.c
 * @author Ai-Thinker
 * @version V1.0.0
 * @brief  distance and range output, Users can handle it themselves
 * @date    2024.9.18
**********************************************************************************/
#include "user_out.h"
#include "hal_usart.h"

bool aiio_output = true;
uint16_t user_tag_dist_cm[MAX_ANCHOR_LIST_SIZE] = {0};

/**
 * 设置安信可默认输出的开关状态。
 *
 * @param output 如果为true，则开启输出；如果为false，则关闭输出。
 */
void set_aiio_output_state (bool output)
{
    aiio_output = output;
}

/**
 * 获取安信可默认输出的开关状态。
 *
 * @return 如果输出已开启，则返回true；否则返回false
 */
bool get_aiio_output_state (void)
{
    return aiio_output;
}

/**
 * 输出 twr算法(基站)测量结果，用户可自主处理
 *
 */
void node_twr_output_user (result_t* pRes)
{
    if (pRes == NULL)
    {
        _dbg_printf ("Invalid result pointer\n");
        return;
    }

    for (int i = 0; i < MAX_ANCHOR_LIST_SIZE; i++)
    {
        _dbg_printf ("dist_cm[%zu] = %3.3f m\n", i, pRes->dist_cm[i] / 100.0);
    }
}

/**
 * 输出 twr算法(标签)测量结果，用户可自主处理
 *
 */
void tag_twr_output_user (uint16_t* dis)
{
    if (dis == NULL)
    {
        _dbg_printf ("Invalid result pointer\n");
        return;
    }

    for (int i = 0; i < MAX_ANCHOR_LIST_SIZE; i++)
    {
        _dbg_printf ("dist_cm[%zu] = %3.3f m\n", i, dis[i] / 100.0);
    }
}

/**
 *输出 pdoa算法（基站）测量结果，用户可自主处理
 *
 */
void node_pdoa_output_user (result_pdoa_t* Res)
{
    if (Res == NULL)
    {
        _dbg_printf ("Invalid result pointer\n");
        return;
    }

    _dbg_printf ("dist_cm = %3.3f m\n", Res->dist_cm / 100.0);
    _dbg_printf ("angle = %3.1f \n", Res->angle);
}

/**
 *输出 pdoa算法（标签）测量结果，用户可自主处理
 *
 */
void tag_pdoa_output_user (result_pdoa_t* Res)
{
    if (Res == NULL)
    {
        _dbg_printf ("Invalid result pointer\n");
        return;
    }

    _dbg_printf ("dist_cm = %3.3f m\n", Res->dist_cm / 100.0);
    _dbg_printf ("angle = %3.1f \n", Res->angle);
}





