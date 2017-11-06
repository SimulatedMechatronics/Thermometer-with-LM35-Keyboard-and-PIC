#include <firm.h>
#include <stdlib.h>

#define LCD_ENABLE_PIN PIN_D3
#define LCD_RS_PIN PIN_D1
#define LCD_RW_PIN PIN_D2
#define LCD_DATA4 PIN_D4
#define LCD_DATA5 PIN_D5
#define LCD_DATA6 PIN_D6
#define LCD_DATA7 PIN_D7
#define LCD_TYPE 2
#include <lcd.c>

long temp = 0;//guarda valor analogica registrado em RA0
signed long temper = 0;//temperatura mostrada no display
signed long calibra = 0;//temperatura calibrada menos temperatura atual
int kbtecla = 0; //setagem da coluna do teclado
int debouncer = 0;//gera espera de captura de tecla
char tecla = 'Z';//guarda tecla pressionada
int ativo = 0;//Ativa o modo de captura de dados
int posicao = 0;//ponteiro da string textonum
char textonum[16]={"0"};//guarda dado digitado pelo usuario

#INT_RTCC
void  RTCC_isr(void) {
  kbtecla += 1; //seta proxima coluna do teclado
  if(debouncer == 0) { //caso debouncer igual a 0 nao capture tecla
    if(kbtecla > 2) kbtecla = 0; // volte pra coluna 0 caso esteja na 2
    switch(kbtecla) { //seleciona qual pino vai ta acesso
      case 0:
      output_high(PIN_E0);
      output_low(PIN_E1);
      output_low(PIN_E2);
      break;
      case 1:
      output_high(PIN_E1);
      output_low(PIN_E2);
      output_low(PIN_E0);
      break;
      case 2:
      output_high(PIN_E2);
      output_low(PIN_E1);
      output_low(PIN_E0);
      break;
    }
    if(input_state(PIN_B6)){
      switch(kbtecla) { //verifique qual tecla foi pressionada
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
      debouncer=10; //ativa espera de tecla
    }
    else if(input_state(PIN_B5)) {
      switch(kbtecla) { //verifique qual tecla foi pressionada
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
      debouncer=10; //ativa espera de tecla
    }
    else if(input_state(PIN_B4)) {
      switch(kbtecla) { //verifique qual tecla foi pressionada
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
      debouncer=10; //ativa espera de tecla
    }
    else if(input_state(PIN_B3)) { //verifique qual tecla foi pressionada
      switch(kbtecla) { //verifique qual tecla foi pressionada
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
      debouncer=10; //ativa espera de tecla
    }

    if(tecla=='#') { //caso # pressionado ativa modo captura
      ativo=1;
      posicao=0; //ponteiro eh alterado para primeira coluna
    }
    if(tecla=='*') { //caso * pressionado desativa modo captura
      ativo=0;
      calibra=atoi32(textonum) - (signed long)((signed long)temp-94)/2; //formula para calibracao
    }
    if(ativo==1 && tecla!='Z' && tecla!='#' && tecla!='*') { //capture a tecla digitada
      textonum[posicao]=tecla; //guarda tecla em textonum na posicao
      posicao+=1;
      textonum[posicao]='\0';//acione valor nulo ao final

    }
    tecla='Z';
  }
  if(debouncer > 0) debouncer -= 1; //decremta debouncer caso maior que 0
  clear_interrupt(int_TIMER0);
}

void main() {
   lcd_init();
   setup_adc_ports(AN0);
   setup_adc(ADC_CLOCK_DIV_2|ADC_TAD_MUL_0);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_2);      //131 ms overflow
   enable_interrupts(INT_RTCC);
   enable_interrupts(GLOBAL);
   set_adc_channel(0); //selecionado AN0
   delay_us(50);
   while(TRUE) {
      temp = read_adc(); //guarda temperatura em forma de numero
      temper = calibra + (signed long) ( (signed long)temp - 94) / 2;//guarda temperatura a mostrar no display
      printf(lcd_putc,"\fTemperatura:\n%li oC",temper);//escreve no display LCD
      while(ativo == 1) { //mostra mensagem do LCD caso entrar em modo de captura
         printf(lcd_putc,"\fAlterar para:\n%s",textonum);
         delay_ms(200);
      }
      delay_ms(200);
   }

}
