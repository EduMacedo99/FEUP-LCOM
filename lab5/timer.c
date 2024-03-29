#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

uint32_t global_int_counter = 0;
int hook_id = 3;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  
  uint8_t st; //to get the status word of the timer

  if(timer_get_conf(timer, &st) != 0){
    return 1;
  } 

  uint32_t controlWord = TIMER_LSB_MSB; //control word which we will be going to write in the                                               control port (selected MSB after LSB option)

  if(st & BIT(0)){

    controlWord |= BIT(0);
  }

  if(st & BIT(1)){

    controlWord |= BIT(1);
  }

  if(st & BIT(2)){

    controlWord |= BIT(2);
  }

  if(st & BIT(3)){

    controlWord |= BIT(3);
  }


  //control word now has the same 4 least significant bits as the st extracted from the timer

  switch(timer){

    case 0:
      break; //if the timer is timer 0, there is no need to set any bits (bits 6 and 7 should be 0)
    
    case 1:
      controlWord |= TIMER_SEL1; //bit 6 is set to 1
      break;

    case 2:
      controlWord |= TIMER_SEL2; //bit 7 is set to 1
      break;
  }

    //control word is finished


  if(sys_outb(TIMER_CTRL, controlWord) != OK){  //writes control word on the control port

    printf("An error happened!\n");
    return 1;
  }

  uint16_t freqInitVal;

  if(freq <= 18){ //if the freq is less than 19, the value of the frequency that is going to be                     //writen in timer 0 cannot be represented in 16 bits

    printf("Maximum value of the frequency has been surpassed!\n");
    return 1;
  
  }else{

  freqInitVal = (uint16_t) (TIMER_FREQ / freq);  //this is the frequency value that we                                                             //need to write in the timer
  }



  uint8_t lsb, msb;
  util_get_LSB(freqInitVal, &lsb);  //gets the LSB of the frequency
  util_get_MSB(freqInitVal, &msb);  //gets the MSB of the frequency

  switch(timer){

    case 0:
      if(sys_outb(TIMER_0, (uint32_t) lsb) != OK){

        printf("An error happened!\n");
        return 1;
      }

      if(sys_outb(TIMER_0, (uint32_t) msb) != OK){

        printf("An error happened!\n");
        return 1;
      }

      break;

    case 1:
      if(sys_outb(TIMER_1, (uint32_t) lsb) != OK){

        printf("An error happened!\n");
        return 1;
      }

      if(sys_outb(TIMER_1, (uint32_t) msb) != OK){

        printf("An error happened!\n");
        return 1;
      }

      break;


    case 2:
      if(sys_outb(TIMER_2, (uint32_t) lsb) != OK){

        printf("An error happened!\n");
        return 1;
      }

      if(sys_outb(TIMER_2, (uint32_t) msb) != OK){

        printf("An error happened!\n");
        return 1;
      }

      break;
  }

  return 0;
}
 

int (timer_subscribe_int)(uint8_t *bit_no){
  *bit_no = hook_id; //because the hook_id initially has the value of 3 (2^3 = 8), to check if bit #3 of msg.m_notify.interrupts is set, bit_no needs to be 3

  if( sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0){

    printf("An error happened!\n");
    return 1;
  }
  
   return 0;
}


int (timer_unsubscribe_int)() {
 if(sys_irqrmpolicy(&hook_id) != 0){

   printf("An error happened!\n");
   return 1;
 }
 
  return 0;
}

void (timer_int_handler)() {
  global_int_counter++;       //increments global counter
}



int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  
    uint32_t RBCommand = TIMER_RB_CMD;   

    switch(timer){

      case 0:
        RBCommand |= TIMER_RB_SEL(0);   //selects timer 0
        break;

      case 1:
        RBCommand |= TIMER_RB_SEL(1);   //selects timer 1
        break;

      case 2:
        RBCommand |= TIMER_RB_SEL(2);   //selects timer 2
        break;

      default:
        printf("Invalid timer value!\n");
        return 1;
    }



    RBCommand |= TIMER_RB_COUNT_;     //only selects the programmed mode option



    if(sys_outb(TIMER_CTRL, RBCommand) != OK){   //writes in the control port the RB                                                               message

        printf("An error happened!\n");          //error message
        return 1;
    }


    uint32_t byte;     //temporary variable, in order to use sys_inb which requires
                        //a uint32_t

    switch(timer){

      case 0:
        if(sys_inb(TIMER_0, &byte) != OK){

            printf("An error happened!\n");          //error message
            return 1;
        }
        break;


      case 1:
        if(sys_inb(TIMER_1, &byte) != OK){

            printf("An error happened!\n");          //error message
            return 1;
        }
        break;


      case 2:
        if(sys_inb(TIMER_2, &byte) != OK){

            printf("An error happened!\n");          //error message
            return 1;
        }
        break;
    }

    
    *st = (uint8_t)byte;  //passes the 8 LSB of byte, containing the RB Status, to st


  return 0;
}



int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  
  union timer_status_field_val finalValue;

  switch(field){

    case all:

      finalValue.byte = st;
      if(timer_print_config(timer, all, finalValue) != OK){
        printf("An error happened!\n");
        return 1;
      }
      break;

    case initial:

      if( (st & TIMER_LSB) && (st & TIMER_MSB) ){  //if both bits 4 and 5 are 1
        finalValue.in_mode = MSB_after_LSB;
    
      }
      else if( st & TIMER_LSB ){                   //if only bit 4 is 1
        finalValue.in_mode = LSB_only;
  
      }
      else if( st & TIMER_MSB ){                   //if bit 5 is 1
        finalValue.in_mode = MSB_only;
        
      }
      else{
        finalValue.in_mode = INVAL_val;
      }

      if(timer_print_config(timer, initial, finalValue) != OK){
        printf("An error happened!\n");
        return 1;
      }
      break;

    case mode:

      if( (st & TIMER_RATE_GEN) && (st & TIMER_MODE_1) ) //if both bits 1 and 2 are 1
        finalValue.count_mode = 3;    //square wave generator
      else if( st & TIMER_RATE_GEN )                //only bit 2 is 1
        finalValue.count_mode = 2;    //rate generator
      else if( (st & TIMER_MODE_4) && (st & TIMER_MODE_1) )     //if both bits 1 and 3 are 1
        finalValue.count_mode = 5;
      else if( st & TIMER_MODE_4 )                  //only bit 3 is 1
        finalValue.count_mode = 4;
      else if( st & TIMER_MODE_1 )                  //only bit 1 is 1
        finalValue.count_mode = 1;
      else finalValue.count_mode = 0;               //if none of the                                                                              //above are true,                                                                             //bits 1, 2 and 3 are 0,
                                                    //which is mode 0    

      if(timer_print_config(timer, mode, finalValue) != OK){
        printf("An error happened!\n");
        return 1;
      }
      break;


    case base:

      if( st & TIMER_BCD )  //if bit 0 is 1
        finalValue.bcd = true;
      else finalValue.bcd = false; //if bit 0 is 0

      if(timer_print_config(timer, base, finalValue) != OK){
        printf("An error happened!\n");
        return 1;
      }
    break;
  
    default:
      return 1;
  }


  return 0;
}
