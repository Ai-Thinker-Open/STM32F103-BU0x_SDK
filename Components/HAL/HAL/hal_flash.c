#include "hal_flash.h"


void HalFlashInit (void)
{
}

uint8_t HalWrite_Flash (uint32_t Address, uint32_t* buff, uint8_t len)
{
    volatile FLASH_Status FLASHStatus;
    uint8_t k = 0;

    FLASHStatus = FLASH_COMPLETE;
    FLASH_Unlock();
    FLASH_ClearFlag (FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    FLASHStatus = FLASH_ErasePage (Address);

    if (FLASHStatus == FLASH_COMPLETE)
    {
        for (k = 0; (k < len) && (FLASHStatus == FLASH_COMPLETE); k++)
        {
            FLASHStatus = FLASH_ProgramWord (Address, buff[k]);
            Address = Address + 4;
        }

        FLASH_Lock();
    }
    else
    {
        return 0;
    }

    if (FLASHStatus == FLASH_COMPLETE)
    {
        return 1;
    }

    return 0;
}

void HalRead_Flash (uint32_t Address, uint32_t* buff, uint8_t len)
{
    uint8_t k;

    for (k = 0; k < len; k++)
    {
        buff[k] = (* (volatile uint32_t*) Address);
        Address += 4;
    }
}


