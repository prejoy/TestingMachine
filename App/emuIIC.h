#ifndef  _INA219_H
#define  _INA219_H

#include "stm32f4xx.h"
#include "main.h"

#define  INA219_Aderss_One    0x80   //从机设备 1 地址
#define  INA219_Aderss_Two    0x81   //从机设备 2 地址
//Reg  adress
#define  INA219_Config_Reg    0x00 //   R/W
#define  INA219_Shunt_Voltage 0x01 //   R
#define  INA219_Bus_Voltage   0x02 //   R
#define  INA219_Power         0x03 //   R
#define  INA219_Current       0x04 //   R
#define  INA219_Calibation    0x05 //   R/W
//Reg  
#define  Config_Value         0x41F//0x3c1f // 0x0667 // 0x3c1f    // 32V, +_320mv,BUS ADC 12 bits,Branch ADC 12 bits,默认工作模式（连续检测模式）
#define  Calibation_Value     8388  //4096  //0x5477 //4096   //I lab = 10(-6)



#define IIC_SDA_Out    SDA_OUT()
#define IIC_SDA_IN     SDA_IN()
#define IIC_SCL_Out    SCL_OUT()
 
#define SCL_H         HAL_GPIO_WritePin(IIC1_SCL_GPIO_PORT,IIC1_SCL_PIN,GPIO_PIN_SET)	//GPIO_SetBits(SCL1_Port,SCL1_Pin)
#define SCL_L         HAL_GPIO_WritePin(IIC1_SCL_GPIO_PORT,IIC1_SCL_PIN,GPIO_PIN_RESET)	//GPIO_ResetBits(SCL1_Port,SCL1_Pin)
#define SCL_H_2       HAL_GPIO_WritePin(IIC2_SCL_GPIO_PORT,IIC2_SCL_PIN,GPIO_PIN_SET)
#define SCL_L_2       HAL_GPIO_WritePin(IIC2_SCL_GPIO_PORT,IIC2_SCL_PIN,GPIO_PIN_RESET)


#define SDA_H         HAL_GPIO_WritePin(IIC1_SDA_GPIO_PORT,IIC1_SDA_PIN,GPIO_PIN_SET)	//GPIO_SetBits(SDA1_Port,SDA1_Pin)
#define SDA_L         HAL_GPIO_WritePin(IIC1_SDA_GPIO_PORT,IIC1_SDA_PIN,GPIO_PIN_RESET)	//GPIO_ResetBits(SDA1_Port,SDA1_Pin)
#define SDA_H_2       HAL_GPIO_WritePin(IIC2_SDA_GPIO_PORT,IIC2_SDA_PIN,GPIO_PIN_SET)
#define SDA_L_2       HAL_GPIO_WritePin(IIC2_SDA_GPIO_PORT,IIC2_SDA_PIN,GPIO_PIN_RESET)

#define Read_SDA      HAL_GPIO_ReadPin(IIC1_SDA_GPIO_PORT,IIC1_SDA_PIN)					//GPIO_ReadInputDataBit(SDA1_Port,SDA1_Pin)
#define Read_SDA_2    HAL_GPIO_ReadPin(IIC2_SDA_GPIO_PORT,IIC2_SDA_PIN)


void delay_us(uint32_t t);
void IIC_Init(uint32_t index);
static void SDA_OUT(void);
static void SDA_IN(void);
static void SCL_OUT(void);

static void SDA_OUT_2(void);
static void SDA_IN_2(void);
static void SCL_OUT_2(void);

static void IIC_Start(void);
static void IIC_Start_2(void);
static void IIC_Stop(void);
static void IIC_Stop_2(void);
static void IIC_Ack(unsigned char c_ACK);
static void IIC_Ack_2(unsigned char c_ACK);
unsigned char IIC_Write_Byte(unsigned char c_data,uint32_t index);
unsigned char IIC_Read_Byte(uint32_t index);
void Read_Reg(uint8_t REG_adress,uint16_t *data,uint32_t index);
void Write_Reg(uint8_t REG_adress,uint16_t data,uint32_t index);


//==== no use
float  Get_Current(void);
void INA219_Init(void);
float Filter_data( float *source,unsigned char length);
float Get_Fif_Currrnt(void);

#endif

