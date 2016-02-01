/*
Jake A. Cole
Project 9
11/5/2013
In the Real Time Clock project Timer1 is used to generate
interrupts every hundredth of a second to create a clock
that is displayed on the LCD screen that is able
to be enabled, disabled, reset, and refreshed
*/
void convert(int n, char s[]);
void interrupt (void);

int counter = 0, dflag = 0; //Display flag is used to determine if the LCD
                            //needs to refresh the current value on the screen
char string[6];

void main () {
  TMR1H = 0xF6;  //Load Timer for a hundredth of a second interrupts
  TMR1L = 0x3B;

  PIR1.TMR1IF = 0;      //Clear Timer overflow flag

  T1CON.TMR1ON = 1;     //Enable Timer
  T1CON.T1CKPS1 = 1;    //Set Prescaler
  T1CON.T1CKPS0 = 1;    //to 1:8
  T1CON.T1OSCEN = 0;    //Turn off oscillator

  PIE1.TMR1IE = 1;      //Enable timer overflow interrupts

  ANSEL  = 0x00;        //Clear analog input register
  ANSELH = 0x00;

  CM1CON0 = 0x00;       //Clear the comparators
  CM2CON0 = 0x00;

  IOCB = 0x00;          //Ensures mismatch interrupts are not enabled
  TRISB = 0x00;         //Sets TRISB as an output
  TRISC = 0xFF;         //Sets TRISC as an input
  TRISD = 0x00;         //Sets TRISD as an output

  INTCON.GIE = 1;       //Global interrupt enabled
  INTCON.PEIE = 1;      //Peripheral interrupt enabled

  Lcd_Config(&PORTB, 4, 5, 6, 3, 2, 1, 0); //Configures the LCD screen to PORTB
  LCD_Cmd(LCD_CURSOR_OFF);
  LCD_Cmd(LCD_CLEAR);



while (1) {     //infinite loop
   if (PORTC.F2 == 0) //The LCD will refresh the timer if RC2 is released
      if (dflag == 1) Lcd_Out(1, 1, string); //Displays the Clock on the LCD
   if (PORTC.F3 == 1) {
      counter = 0;       //Resets the timer if RC3 is pressed
      PIR1.TMR1IF = 1;   //Ensures the board displays the value 00.00
      T1CON.TMR1ON = 0;	 //Disables the timer
   }
   if (PORTC.F4 == 1) T1CON.TMR1ON = 0;  //Disables the timer if RC4 is pressed
   if (PORTC.F5 == 1) T1CON.TMR1ON = 1;  //Enables the timer if RC5 is pressed
  }
}

/*
Interrupt reloads the timer when it overflows and
increments the counter every hundredth of a second
*/
void interrupt (void) {
  if (PIR1.TMR1IF == 0) { //Returns if the timer hasnt overflown
    dflag = 0;  //Flags that the board doesn't need to display the Clock
    return;
  }

  TMR1H = 0xF6;  //Load Timer
  TMR1L = 0x3B;
  PIR1.TMR1IF = 0;      //Clear Timer overflow flag
  convert(counter, string);
  dflag = 1; //Flags that the board needs to display the Clock on the LCD
  counter++;   //Increments the counter
  if (counter > 9999) counter = 0;   //Resets the counter if it hits 10,000

  if ((counter / 100) % 5 == 0) PORTD = 0xFF; //Lights up LEDs RD0 to RD7
  else PORTD = 0x00;  //Turns off LEDs RD0 to RD7
  return;
}

/*
Convert converts an integer to a string so it can be displayed on the LCD
*/
void convert(int n, char s[]) {
  s[5] = '\0';             //Adds null onto the string
  s[4] = n % 10 + '0';     //Hundredth of a second
  n /= 10;
  s[3] = n % 10 + '0';     //Tenth of a second
  n /= 10;
  s[2] = '.';              //Decimal place
  s[1] = n % 10 + '0';     //A second
  n /= 10;
  s[0] = n % 10 + '0';     //Ten seconds
  return;
}
