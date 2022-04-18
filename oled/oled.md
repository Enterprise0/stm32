## i2c
stm32f103c6t6a的cubemx初始化i2c有bug，发送i2c数据没有电平变化。需要在`HAL_I2C_MspInit`添加gpio速度配置：
```c
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
```