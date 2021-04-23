/**
 ********************************************************
 * Copyright (c) 2019-2019, RobotLab Development Team
  * @file   £º rs_device.h
  * @brief  £º Template class for device object.
 *  @Changes£º
 *  Date           Author   Version    Notes
 *  2019-06-22     Mentos    0.0.1     first version
 ********************************************************
 */
#ifndef  _RS_DEVICES_H_
#define  _RS_DEVICES_H_
/* Includes ------------------------------------------------------------------*/
#include <string>
#include <vector>
#include "rs_def.h"
#include "rs_conf.h"
#include <assert.h>

#ifdef FreeRTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#else
      #error RTOS support error!
#endif


/* Private  ------------------------------------------------------------------*/
#ifdef  __cplusplus
template <typename myDevice>
class Device
{
   static std::vector<Device*> device_list;
  private:
    std::string type_name;
    std::string name;
    unsigned char   status;
    unsigned int    last_check_tick;
    unsigned short  device_list_index;
    unsigned short  offline_time;
    unsigned char   port_num;
    void*     com_TxPort;
    void*     com_RxPort;
    myDevice* object_addr;
  
  public:
    Device(std::string* type_name, std::string* name,const myDevice* device_obj)
    {
      this->type_name = *type_name;
      this->name = *name;
      device_list.push_back(&this);
      device_list_index = device_list.size();
      object_addr = device_obj;

    }
    ~Device()
    {
      
    }
    /*
       Set the max wait time for connection detect. When no 
       message lasted longer than this value, device will 
       enter 'Connection Lost'.
    */
    inline void set_offline_time(unsigned short time){ offline_time = time; };
    
    /*
       Check the connection through system tick.This function 
        must be called every time When the device driver is running.
    */
    void check_link(unsigned int current_tick)
    {
      if(current_tick - last_check_tick > offline_time)
      {
        //Counter overflow£¨uint32£©.
        if(current_tick - last_check_tick > 10000) __nop();
        else
          status = Connection_Lost;
      }
      else
        status = Connection_Established;
    }
    
    /*
      Using communication port detached by communication service to send message.
    */
    void send_message(Unified_COB* tx_COB)
    {
       assert(com_TxPort != NULL);
      
      //Packed
      tx_COB->port_num = this->port_num;
#ifdef FreeRTOS
      //Get message to send.
      if(tx_COB != NULL) xQueueSend((QueueHandle_t)com_TxPort, tx_COB, 0);
#else
      #error RTOS support error!
#endif
    }
    
    /*
      Using communication port detached by communication service to read message.
    */
    Msg_Type read_message(Unified_COB* rx_COB)
    {
      assert(com_RxPort != NULL);
      assert(rx_COB != NULL);
      
#ifdef FreeRTOS
      if(xQueueReceive((QueueHandle_t)com_RxPort,rx_COB,0) == pdPASS)
      {
        last_check_tick = xTaskGetTickCount();
        return Get_Message;
      }
      else 
        return No_Message;
#else
      #error RTOS support error!
#endif
    }
   
    /*
      Find out all the devices that have the same type name.
    */
   static void find_device_by_type(std::string* type_name, std::vector<Device>* device_list_of_deviceType)
   {
     //Check the whole list
     for(unsigned short i = 0; i != device_list->size(); ++i )
     {
       if(device_list[i]->type_name == *type_name)
       {
         device_list_of_deviceType->push_back(device_list[i]);
       }
     }
   }
};
/* Exported ------------------------------------------------------------------*/


#endif
#endif

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/

