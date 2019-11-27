#include <emuIIC.h>



#define delay_ms(x)	HAL_Delay(x)


void IIC_Init(uint32_t index)
{
	if(index == 1 ){
		IIC_SCL_Out;
		IIC_SDA_Out;
		SCL_H;
		SDA_H;
		delay_ms(20);
	}else if(index == 2 ){
		SCL_OUT_2();
		SDA_OUT_2();
		SCL_H_2;
		SDA_H_2;
		delay_ms(20);
	}
}

static void SDA_OUT(void)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};

 /* GPIO Ports Clock Enable */
 __HAL_RCC_GPIOB_CLK_ENABLE();

 HAL_GPIO_WritePin(IIC1_SDA_GPIO_PORT,IIC1_SDA_PIN,GPIO_PIN_SET);
 GPIO_InitStruct.Pin = IIC1_SDA_PIN;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_PULLUP;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(IIC1_SDA_GPIO_PORT, &GPIO_InitStruct);
}

static void SDA_OUT_2(void)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};

 /* GPIO Ports Clock Enable */
 __HAL_RCC_GPIOB_CLK_ENABLE();

 HAL_GPIO_WritePin(IIC2_SDA_GPIO_PORT,IIC2_SDA_PIN,GPIO_PIN_SET);
 GPIO_InitStruct.Pin = IIC2_SDA_PIN;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_PULLUP;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(IIC2_SDA_GPIO_PORT, &GPIO_InitStruct);
}

static void SDA_IN(void)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};

 /* GPIO Ports Clock Enable */
 __HAL_RCC_GPIOB_CLK_ENABLE();

 HAL_GPIO_WritePin(IIC1_SDA_GPIO_PORT,IIC1_SDA_PIN,GPIO_PIN_SET);
 GPIO_InitStruct.Pin = IIC1_SDA_PIN;
 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
 GPIO_InitStruct.Pull = GPIO_PULLUP;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(IIC1_SDA_GPIO_PORT, &GPIO_InitStruct);
}

static void SDA_IN_2(void)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};

 /* GPIO Ports Clock Enable */
 __HAL_RCC_GPIOB_CLK_ENABLE();

 HAL_GPIO_WritePin(IIC2_SDA_GPIO_PORT,IIC2_SDA_PIN,GPIO_PIN_SET);
 GPIO_InitStruct.Pin = IIC2_SDA_PIN;
 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
 GPIO_InitStruct.Pull = GPIO_PULLUP;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(IIC2_SDA_GPIO_PORT, &GPIO_InitStruct);
}

static void SCL_OUT(void)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};

 /* GPIO Ports Clock Enable */
 __HAL_RCC_GPIOB_CLK_ENABLE();

 HAL_GPIO_WritePin(IIC1_SCL_GPIO_PORT,IIC1_SCL_PIN,GPIO_PIN_SET);
 GPIO_InitStruct.Pin = IIC1_SCL_PIN;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_PULLUP;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(IIC1_SCL_GPIO_PORT, &GPIO_InitStruct);
}

static void SCL_OUT_2(void)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};

 /* GPIO Ports Clock Enable */
 __HAL_RCC_GPIOB_CLK_ENABLE();

 HAL_GPIO_WritePin(IIC2_SCL_GPIO_PORT,IIC2_SCL_PIN,GPIO_PIN_SET);
 GPIO_InitStruct.Pin = IIC2_SCL_PIN;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_PULLUP;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
 HAL_GPIO_Init(IIC2_SCL_GPIO_PORT, &GPIO_InitStruct);
}


static void IIC_Start(void)
{
    IIC_SDA_Out;
    SDA_H;
    SCL_H;
    delay_us(5);
    SDA_L;
    delay_us(5);
    SCL_L;
}
static void IIC_Start_2(void)
{
	SDA_OUT_2();
    SDA_H_2;
    SCL_H_2;
    delay_us(5);
    SDA_L_2;
    delay_us(5);
    SCL_L_2;
}

//????
static void IIC_Stop(void)
{
    IIC_SDA_Out;
    
    SCL_H;
    SDA_L;
    delay_us(5);
    SDA_H;
    delay_us(5);
}

static void IIC_Stop_2(void)
{
	SDA_OUT_2();

    SCL_H_2;
    SDA_L_2;
    delay_us(5);
    SDA_H_2;
    delay_us(5);
}

//应答信号  1为应答，0为不应答
static void IIC_Ack(unsigned char c_ACK)
{
    IIC_SDA_Out;
    
    if(c_ACK)
    {
      SDA_L;                    
    }
    else
    {
      SDA_H;               
    }
    delay_us(5);
    SCL_H;
    delay_us(5);
    SCL_L;                        
}
static void IIC_Ack_2(unsigned char c_ACK)
{
	SDA_OUT_2();

    if(c_ACK)
    {
      SDA_L_2;
    }
    else
    {
      SDA_H_2;
    }
    delay_us(5);
    SCL_H_2;
    delay_us(5);
    SCL_L_2;
}
 //@param  return ret == 1 代表从机应答成功

unsigned char IIC_Write_Byte(unsigned char c_data,uint32_t index)
{
	if(index == 1){
	    unsigned char c_ctr = 0;
	    unsigned char ret = 0;

	    IIC_SDA_Out;
	     SCL_L;
	    for(c_ctr=0;c_ctr<8;c_ctr++)
	    {
	        if(c_data & 0x80)
	        {
	            SDA_H;
	        }
	        else
	        {
	            SDA_L;
	        }

	        c_data <<= 1;

	        SCL_H;
	        delay_us(5);
	        SCL_L;
	        delay_us(5);
	    }

	    IIC_SDA_IN;
			SDA_H;
			delay_us(3);
	    SCL_H;
	    delay_us(5);

	    c_ctr = 0;
	    do
	    {
	        if( Read_SDA == 0)
	        {
	            ret = 1;
	            break;
	        }
	        delay_us(2);
	        c_ctr++;
	    }while(c_ctr < 10);

	   SCL_L;

	    return ret;
	}
	else if(index == 2){
	    unsigned char c_ctr = 0;
	    unsigned char ret = 0;

	    SDA_OUT_2();
	     SCL_L_2;
	    for(c_ctr=0;c_ctr<8;c_ctr++)
	    {
	        if(c_data & 0x80)
	        {
	            SDA_H_2;
	        }
	        else
	        {
	            SDA_L_2;
	        }

	        c_data <<= 1;

	        SCL_H_2;
	        delay_us(5);
	        SCL_L_2;
	        delay_us(5);
	    }

	    SDA_IN_2();
		SDA_H_2;
		delay_us(3);
	    SCL_H_2;
	    delay_us(5);

	    c_ctr = 0;
	    do
	    {
	        if( Read_SDA_2 == 0)
	        {
	            ret = 1;
	            break;
	        }
	        delay_us(2);
	        c_ctr++;
	    }while(c_ctr < 10);

	   SCL_L_2;

	    return ret;
	}

}

unsigned char IIC_Read_Byte(uint32_t index)
{
	if(index == 1){
	    unsigned char i = 0;
	    unsigned char c_data = 0;

	    IIC_SDA_IN;

	    for(i=0;i<8;i++)
	    {
	        SCL_L;
	        delay_us(5);
	        SCL_H;
	        delay_us(5);

	        c_data <<= 1;
	        if(Read_SDA == 1)
	        {
	            c_data |= 1;
	        }
	    }

	   SCL_L;

	    return c_data;
	}
	else if(index == 2){
	    unsigned char i = 0;
	    unsigned char c_data = 0;

	    SDA_IN_2();

	    for(i=0;i<8;i++)
	    {
	        SCL_L_2;
	        delay_us(5);
	        SCL_H_2;
	        delay_us(5);

	        c_data <<= 1;
	        if(Read_SDA_2 == 1)
	        {
	            c_data |= 1;
	        }
	    }

	   SCL_L_2;

	    return c_data;
	}
} 

/*
@param REG_adress :寄存器的地址
              data：向寄存器写入的数据

*/

 void Write_Reg(uint8_t REG_adress,uint16_t data,uint32_t index)
{
	 if(index ==1 ){
		 uint8_t data_temp[2];
		data_temp[0] = data>>8;
		data_temp[1] = data & 0xff;

		IIC_Start();
		IIC_Write_Byte(INA219_Aderss_One,index);//发送从机地址
		IIC_Write_Byte(REG_adress,index);

		IIC_Write_Byte(data_temp[0],index);
		data++;       // 延时一段时间
		data--;
		IIC_Write_Byte(data_temp[1],index);
		IIC_Stop();
	 }else if(index ==2 ){
		 uint8_t data_temp[2];
		data_temp[0] = data>>8;
		data_temp[1] = data & 0xff;

		IIC_Start_2();
		IIC_Write_Byte(INA219_Aderss_One,index);//发送从机地址
		IIC_Write_Byte(REG_adress,index);

		IIC_Write_Byte(data_temp[0],index);
		data++;       // 延时一段时间
		data--;
		IIC_Write_Byte(data_temp[1],index);
		IIC_Stop_2();
	 }
	
}


/*
@param REG_adress :寄存器的地址
             *data：读取寄存器的数据存储的位置

*/
void Read_Reg(uint8_t REG_adress,uint16_t *data,uint32_t index)
{
	if(index ==1 ){
		uint8_t u16_H,u16_L;
		IIC_Start();
		IIC_Write_Byte(INA219_Aderss_One,index);//发送从机地址
		IIC_Write_Byte(REG_adress,index);

		IIC_Start();
		IIC_Write_Byte(INA219_Aderss_One + 0x01 , index);  //  read
		u16_H = IIC_Read_Byte(index);
		// data++;
		REG_adress++;		//延时等一下
		REG_adress--;
		IIC_Ack(1);
		u16_L = IIC_Read_Byte(index);
		IIC_Ack(0);
		IIC_Stop();

		*data = u16_H<<8 | u16_L;
	}
	else if(index ==2 ){
		uint8_t u16_H,u16_L;
		IIC_Start_2();
		IIC_Write_Byte(INA219_Aderss_One,index);//发送从机地址
		IIC_Write_Byte(REG_adress,index);

		IIC_Start_2();
		IIC_Write_Byte(INA219_Aderss_One + 0x01 ,index);  //  read
		u16_H = IIC_Read_Byte(index);
		// data++;
		REG_adress++;		//延时等一下
		REG_adress--;
		IIC_Ack_2(1);
		u16_L = IIC_Read_Byte(index);
		IIC_Ack_2(0);
		IIC_Stop_2();

		*data = u16_H<<8 | u16_L;
	}


}
/*
Function: 获取电流值(mA)
*/

float Get_Current(void)
{
 uint16_t Temp_Data1;//Temp_Data[2],;
// Write_Reg(INA219_Config_Reg,Config_Value); //从新配置一下寄存器
 Write_Reg(INA219_Calibation,8192,0);//33554);
 Read_Reg(INA219_Current,&Temp_Data1,0);     //需要先读取

	
// Read_Reg(INA219_Shunt_Voltage,Temp_Data);
 printf("dat:%#x \r\n",Temp_Data1);
// return (float)(((Temp_Data1[0]<<8) + Temp_Data1[1])*0.01);
 return (float)((int16_t)((int16_t)Temp_Data1)*0.05);
}

float Get_Fif_Currrnt(void)
{
  float FiF_Data[10];
	unsigned char i=0;
	for(i=0;i <10;i++)
	{ 
		FiF_Data[i]= Get_Current();
		delay_ms(5);
	}
	return Filter_data(FiF_Data,10);

}

/*
Function ： 初始化INA219
*/
void INA219_Init(void)
{
 IIC_Init(0);
 Write_Reg(INA219_Calibation,Calibation_Value,0);
 Write_Reg(INA219_Config_Reg,Config_Value,0);//配置寄存器
}



float Filter_data( float *source,unsigned char length)
{
 unsigned int i=0,j=0;
 float  p=0;
	for(i=0;i<length-1;i++)
	{
		 for(j=0;j<length-i-1;j++)
		 {
			  if(source[j] > source[j+1] )
			  {
				p = source[j];
				source[j] = source[j+1];
				source[j+1] = p;
			  }
		 }
	 }
  return source[length/2];
}


#undef delay_ms


