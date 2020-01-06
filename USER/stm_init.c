#include "stm_init.h"

void delay(u16 count)
{
    do{}
    while(count--);
}
void delayms(u16 count)
{
   while (count--)
   {
        for (u8 i = 0; i < 20; i++)
            for (u8 j = 0; j < 50; j++);
   }
       
}