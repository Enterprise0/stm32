## V1.0.0
----

- stm32的gpio口不能同时输入和输出，可以操控寄存器改变
```c
//IO方向设置
//stm32f1   PB14
#define IO_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}    //浮空输入
#define IO_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}    //推挽输出
//stm32f4   PB14
#define IO_IN()  {GPIOB->MODER&=0XFCFFFFFF;GPIOB->MODER|=0<<24;}	//浮空输入
#define IO_OUT() {GPIOB->MODER&=0XFCFFFFFF;GPIOB->MODER|=1<<24;\
                        GPIOB->OTYPER&=0XEFFF;GPIOB->OTYPER|=0<<12;}	//推挽输出
```

- 输入模式
    1. 上拉：无输入时gpio为高电平，用于读取输入低电平。
    2. 下拉：无输入时gpio为低电平，用于读取输入高电平。
    3. 浮空：无输入时电平不确定。
    4. 模拟：用于adc。


