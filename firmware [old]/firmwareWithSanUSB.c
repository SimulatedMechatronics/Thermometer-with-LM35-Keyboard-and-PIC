#include <SanUSB.h>
#define LCD_ENABLE_PIN PIN_D3
#define LCD_RW_PIN PIN_D2
#define LCD_RS_PIN PIN_D1
#define LCD_DATA4 PIN_D4
#define LCD_DATA5 PIN_D5
#define LCD_DATA6 PIN_D6
#define LCD_DATA7 PIN_D7
#define LCD_TYPE 2
#include <lcd.c>
#include <stdlib.h>
#include <usb_cdc.h>
int cols=0;
char tecla='N';
char numero[10]={'0'};
int captura=0;
int pos=0;
int32 decimal=0;
int deb=0;
int32 calibra=0;
int temp=0;
long temper=0;
#int_TIMER0
void  TIMER0_isr(void) 
{
if(deb==0)
{
   cols++;
   if(cols>2) cols=0;
   switch(cols)
   {
      case 0:
      output_high(PIN_E2);
      output_low(PIN_E1);
      output_low(PIN_E0);
      break;
      case 1:
      output_low(PIN_E2);
      output_high(PIN_E1);
      output_low(PIN_E0);
      break;
      case 2:
      output_low(PIN_E1);
      output_high(PIN_E0);
      output_low(PIN_E2);
      break;
   }
   if(input_state(PIN_B3))
   {
      switch(cols)
      {
         case 0:
         tecla='1';
         break;
         case 1:
         tecla='2';
         break;
         case 2:
         tecla='3';
         break;
      }
      deb=20;
   }
   else if(input_state(PIN_B4))
   {
      switch(cols)
      {
         case 0:
         tecla='4';
         break;
         case 1:
         tecla='5';
         break;
         case 2:
         tecla='6';
         break;
      }
      deb=20;
   }
   else if(input_state(PIN_B5))
   {
      switch(cols)
      {
         case 0:
         tecla='7';
         break;
         case 1:
         tecla='8';
         break;
         case 2:
         tecla='9';
         break;
      }
      deb=20;
   }
   else if(input_state(PIN_B6))
   {
      switch(cols)
      {
         case 0:
         tecla='*';
         break;
         case 1:
         tecla='0';
         break;
         case 2:
         tecla='#';
         break;
      }
      deb=20;
   }
   if(tecla=='*')
   {
   numero="0";
   pos=0;
   captura=1;
   }
   if(tecla=='#')
   {
   captura=0;
   decimal = atoi32(numero);
   calibra=decimal - (signed long)((signed long)temp-94)/2;//formula para calibracao
   }
   if(captura==1 && tecla!='#' && tecla!='*' && tecla!='N' && pos<3)
   {
      numero[pos] = tecla;
      pos++;
      numero[pos] = '\0';
      tecla='N';
   }
}
else
{
   deb--;
}
}



void main()
{
   lcd_init();
   setup_adc_ports(AN0|VSS_VDD);
   setup_adc(ADC_CLOCK_DIV_2);
   setup_psp(PSP_DISABLED);
   setup_spi(SPI_SS_DISABLED);
   setup_wdt(WDT_OFF);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_2);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   setup_timer_3(T3_DISABLED|T3_DIV_BY_1);
   setup_comparator(NC_NC_NC_NC);
   setup_vref(FALSE);
   enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);
   usb_cdc_init();
   usb_init();
   usb_task();
   while(true)
   {
      while(captura==1)
      {
         printf(lcd_putc,"\fAlterar valor:\n%s",numero);
         delay_ms(1000);
      }
      set_adc_channel(0); //selecionado AN0
      delay_us(20);
      temp=read_adc(); //guarda temperatura em forma de numero
      temper=calibra + (signed long)((signed long)temp-94)/2;//guarda temperatura a mostrar no display
      printf(lcd_putc,"\fTemperatura:\n%li oC",temper);
      printf(usb_cdc_putc,"\fTemperatura:\n%li oC\r\n",temper);
      delay_ms(7000);
   }
}
