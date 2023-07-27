/**
 * @file ka495xx.hpp
 * @author Kyungwoo Min (bluelife85@naver.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __DRIVER_KA495XX_H__
#define __DRIVER_KA495XX_H__

#include "ka495xx_sfr.hpp"

class Ka495xx
{
public:
   Ka495xx();
   Ka495xx(ka495xx_hardwares* hw);
private:
   Ka495xx_interface* interface;
};

#endif /* __DRIVER_KA495XX_H__ */
