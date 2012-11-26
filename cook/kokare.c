#ifndef MIXER_SHIFT
#warning No MIXER_SHIFT defined
#define MIXER_SHIFT 0
#endif

#include "avr.h"

#define IN_PUMP_IN_BIT 0x04
#define in_pump_fixed 0x50

#define OUT_PUMP_IN_BIT 0x08
#define out_pump_fixed  0x50

#define MIXER_IN_BIT 0x40
#define mixer_fixed 0x40

#define COOLER_IN_BIT 0x20
#define COOLER_OUT_BIT 0x10
#define cooler_fixed 0xff

#define HEATER_IN_BIT 0x10
#define HEATER_OUT_BIT 0x20
#define heater_fixed 0xff

#define max_level 0x380
#define heat_level 0x100
#define cool_level 0x100
#define empty_level 0x040
#define min_temp 0x20

// #define max_level 0xe0
// #define heat_level 0x40
// #define cool_level 0x40
// #define empty_level 0x10
// #define min_temp 0x08 


#define ALL_DIGITAL_ACTIVE_BITS (IN_PUMP_IN_BIT | \
				 OUT_PUMP_IN_BIT | \
				 MIXER_IN_BIT | \
				 COOLER_IN_BIT | \
				 HEATER_IN_BIT)
#define ERROR_BIT 0x80

#define CONF_DIG_IN(channel) (0x20 | (channel)&0x1f)
#define CONF_DIG_OUT(channel) (0x40 | (channel)&0x1f)

#define CONF_DIGITAL_IN(chan, config) \
  putchannel(31, (0x20|(chan)&0x1f)|(config)&0xffffff00)
#define CONF_DIGITAL_OUT(chan, config) \
  putchannel(31, (0x40|(chan)&0x1f)|(config)&0xffffff00)
#define CONF_ANALOG_IN(chan, config) \
  putchannel(31, (0x60|(chan)&0x1f)|(config)&0xffffff00)
#define CONF_ANALOG_OUT(chan, config) \
  putchannel(31, (0x80|(chan)&0x1f)|(config)&0xffffff00)
#define CONF_END() putchannel(31, 0)
#define CONF_RESOLUTION(bits) ((bits)<<10)|0x000
#define CONF_MIN(value) (value)|0x100
#define CONF_MAX(value) (value)|0x200
#define CONF_POSITIVE_VOLT(volt) ((long)(volt)<<14)


volatile unsigned char error;

static void putchar(unsigned char ch);

typedef struct rate_record {
  volatile unsigned char digital;
  volatile unsigned char analog;
  volatile unsigned char serial;
  volatile unsigned char rate;
} rate_record;

volatile rate_record in_pump;  
volatile rate_record out_pump;  
volatile rate_record mixer;  
volatile rate_record heater;  
volatile rate_record cooler;  
volatile unsigned char empty;
volatile unsigned char empty_rate;
volatile unsigned int level;
volatile unsigned int temp;
volatile unsigned long serial_value;
volatile unsigned char serial_length;
volatile unsigned char serial_readbits;
volatile unsigned char serial_readchannels;
volatile unsigned char serial_readconfig;


static void zero(volatile rate_record *port) {
  port->digital = 0;
  port->analog = 0;
  port->serial = 0;
  port->rate = 0;
}

static void select_rate(volatile rate_record *port);

static unsigned int filter(volatile unsigned int old, unsigned int adc);

static void puthex(unsigned int c, unsigned int v);

__attribute__ ((signal)) void  ADC_CC()
{
  unsigned char channel = in(ADMUX) & 0x0f;
  unsigned char low = in(ADCL);
  unsigned char high = in(ADCH);
  unsigned int value = (high <<8) | low;
/*
  puthex(channel,high);
  puthex(channel,low);
  puthex(channel,value);
*/
  switch (channel) {
    case 0: { 
      channel = 1; 
      level = filter(level, value); 
    } break;
    case 1: { 
      channel = 2; 
      temp = filter(temp, value); 
    } break;
    case 2: { 
      channel = 3; 
      cooler.analog = filter(cooler.analog, value>>2); 
    } break;
    case 3: { 
      channel = 4; 
      heater.analog = filter(heater.analog, value>>2); 
    } break;
    case 4: { 
      channel = 5; 
      in_pump.analog = filter(in_pump.analog, value>>2); 
    } break;
    case 5: { 
      channel = 0; 
      out_pump.analog = filter(out_pump.analog, value>>2); 
    } break;
    default: { channel = 0; } break;
  }
  out(ADMUX, 0xc0 | channel); // Internal Vref, right adjust
  out(ADCSR, 0xcf);          // Enable ADC interrupts, Clock/128 

}

typedef enum { cmd_clear_bit, cmd_set_bit, 
	       cmd_read_bit, cmd_read_chan } command;

__attribute__ ((signal)) void  UART_RXC() 
/*
 * +-+-+-+-+-+-+-+-+
 * |0|cmd|  chan   | 
 * +-+-+-+-+-+-+-+-+
 *
 *   00 = bit clear, 01 = bit set, 10 = bit get, 11 = chan get
 *
 * +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+ 
 * |1| bit8...bit2 |  |0|bit|  chan   |
 * +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+ 
 *
 * +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+ 
 * |1|bit15...bit9 |  |1| bit8...bit2 |  |0|bit|  chan   |
 * +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+ 
 *
 * ...
 *
 * +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+ 
 * |1|bit31...bit30|  |1|bit29...bit23| ... |0|bit|  chan   |
 * +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+ 
 *
 */
{
  char ch = in(UDR);
  serial_length++;
  if ((ch & 0x80) == 0x80) {
    serial_value = (serial_value << 7) | (ch & 0x7f);
  } else {
    serial_value = (serial_value << 2) | ((ch & 0x60) >> 5);
    unsigned char chan = ch & 0x1f;
    if (serial_length == 1) {
      unsigned char cmd = (serial_value & 0x03);
      switch (cmd) {
	case cmd_clear_bit: { 
	  // Active LOW
	  switch (chan) {
	    case 0: { in_pump.serial = in_pump_fixed; } break;
	    case 1: { out_pump.serial = out_pump_fixed; } break;
	    case 2: { heater.serial = heater_fixed; } break;
	    case 3: { mixer.serial = mixer_fixed; } break;
	    case 4: { cooler.serial = cooler_fixed; } break;
	  }
	} break;
	case cmd_set_bit: { 
	  switch (chan) {
	    case 0: { in_pump.serial = 0; } break;
	    case 1: { out_pump.serial = 0; } break;
	    case 2: { heater.serial = 0; } break;
	    case 3: { mixer.serial = 0; } break;
	    case 4: { cooler.serial = 0; } break;
	  }
	} break;
	case cmd_read_bit: { 
	  if (chan <= 7) { serial_readbits |= (1<<chan); }
	} break;
	case cmd_read_chan: { 
	  if (chan <= 7) { serial_readchannels |= (1<<chan); }
	  if (chan == 31) { serial_readconfig = 1; }
	} break;
      }
    } else {
      switch (chan) {
	case 0: { heater.serial = serial_value; } break;
	case 1: { cooler.serial = serial_value; } break;
	case 2: { in_pump.serial = serial_value; } break;
	case 3: { out_pump.serial = serial_value; } break;
	case 4: { mixer.serial = serial_value; } break;
      }
    }
    serial_value = 0;
    serial_length = 0;
  }
}

__attribute__ ((signal)) void TIM0_OVF() 
{
  static unsigned char counter;
  static unsigned char cooler_rate;
  static unsigned char heater_rate;
  unsigned char port_b;
  unsigned char port_d;

  out(TCNT0, 255-61);   // Next interrupt 16e6/1024/61 -> 256Hz
  
  // Low frequency PWM
  counter++;
  port_b = 0;
  port_d = 0; 
  if (cooler_rate && counter <= cooler_rate) { port_b |= COOLER_OUT_BIT; }
  if (heater_rate && counter <= heater_rate) { port_b |= HEATER_OUT_BIT; }
  if (error) { port_d |= ERROR_BIT; } 
  out(PORTB, ~port_b);
  out(PORTD, (~port_d) | 0x7c);  // Pull up on input bits

  // Read digital inputs (active low)
  port_d= ~in(PIND);
  
  in_pump.digital = 0;
  out_pump.digital = 0;
  mixer.digital = 0;
  cooler.digital = 0;
  heater.digital = 0;
  if ((port_d & ALL_DIGITAL_ACTIVE_BITS) != ALL_DIGITAL_ACTIVE_BITS) {
    if (port_d & IN_PUMP_IN_BIT) { in_pump.digital = in_pump_fixed; }
    if (port_d & OUT_PUMP_IN_BIT) { out_pump.digital = out_pump_fixed; }
    if (port_d & MIXER_IN_BIT) { mixer.digital = mixer_fixed; }
    if (port_d & COOLER_IN_BIT) { cooler.digital = cooler_fixed; }
    if (port_d & HEATER_IN_BIT) { heater.digital = heater_fixed; }
  }

  select_rate(&in_pump);
  select_rate(&out_pump);
  select_rate(&mixer);
  select_rate(&heater);
  select_rate(&cooler);
  error = 0;
  if (in_pump.rate != 0 && level > max_level) {
    in_pump.rate = 0;
    error = 1;
  }
  if (heater.rate != 0 && level < heat_level ) {
    heater.rate = 0;
    error = 1;
  }  
  if (cooler.rate != 0 && ((level < cool_level) || (temp < min_temp))) {
    cooler.rate = 0;
    error = 1;
  }
  if (empty) {
    if (in_pump.rate != 0 || out_pump.rate != 0 || mixer.rate != 0 || 
	heater.rate != 0 || cooler.rate != 0) {
      // Turn off emptying if somebody is controlling
      empty = 0;
    } else if (level > empty_level) {
      // Empty forever until level goes low enough
      empty = 255;
      empty_rate = 0xff;
    } else {
      empty--;
    }
    out_pump.rate = empty_rate;
  }
  
  out(OCR1AH, 0);
  out(OCR1AL, in_pump.rate);
  out(OCR1BH, 0);
  out(OCR1BL, (mixer.rate>>MIXER_SHIFT)&0xff );
  out(OCR2, out_pump.rate);
  cooler_rate = cooler.rate;
  heater_rate = heater.rate;
}

static void putchar(unsigned char ch) 
{
  
  while ((in(UCSRA) & 0x20) == 0) {};
  out(UDR, ch);
}

void putbit(unsigned char channel, unsigned char value) 
{
  if (value) {
    putchar(0x20 | channel);
  } else {
    putchar(0x00 | channel);
  }
}

static void putonehex(char val) {
  if (val < 10) {
    putchar(val + '0');
  } else if (val < 16) {
    putchar(val + 'a' - 10);
  }
}

static void puthex(unsigned int c, unsigned int v){ 
  putonehex((c >> 4) & 0x0f);
  putonehex(c & 0x0f);
  putchar(':');
  putonehex((v >> 12) & 0x0f);
  putonehex((v >> 8) & 0x0f);
  putonehex((v >> 4) & 0x0f);
  putonehex((v) & 0x0f);
  putchar('\r');
  putchar('\n');

}


void putchannel(unsigned char channel, unsigned long value) 
{
  if (value >= (1L<<30)) { putchar(0x80 | ((value >> 30) & 0x03)); }
  if (value >= (1L<<23)) { putchar(0x80 | ((value >> 23) & 0x7f)); }
  if (value >= (1L<<16)) { putchar(0x80 | ((value >> 16) & 0x7f)); }
  if (value >= (1L<< 9)) { putchar(0x80 | ((value >> 9) & 0x7f)); }
  putchar(0x80 | ((value >> 2) & 0x7f));
  putchar(((value << 5) & 0x60) | channel);
}

/*
 * Motors are PWM controlled with 16e6/256/2 == 31 kHz
 * Heater and cooler are PWM controlled with 1 Hz
 */
int main() 
{
  serial_value = 0;
  serial_length = 0;
  serial_readbits = 0;
  serial_readchannels = 0;
  serial_readconfig = 0;
  zero(&in_pump);
  zero(&out_pump);
  zero(&mixer);
  zero(&cooler);
  zero(&heater);
  level = 0;
  temp = 0;
  empty = 255;
  empty_rate = 0;


  out(PORTD, 0x7c);     // Pull up 
  out(DDRD, 0x80); 	// PortD, bit 7 output 
  out(PORTB, 0xff);
  out(DDRB, 0xff); 	// PortB, all outputs 
  out(TCCR0, 0x05);	// Timer0, Clock / 1024 
  out(TCCR1A, 0xf1);    // OC1A & OC1B 8 bit phase correct PWM, active low 
  out(TCCR1B, 0x01);    // Clock / 1 
  out(TCCR2, 0x71);     // OC2 8 bit phase correct, active low Clock/1 
  out(TIMSK, 0x01);	// Enable Timer0 interrupts 
  out(TCNT0, 255-61);   // 16e6/1024/61 -> 256Hz 
  out(UCSRA, 0x00);     // USART: 
  out(UCSRB, 0x98);     // USART: RxIntEnable|RxEnable|TxEnable 
  out(UCSRC, 0x86);     // USART: 8bit, no parity, 9600 
  out(UBRRH, 0x0); 	// USART: 115200 @ 14.7456MHz
  out(UBRRL, 7); 	// USART: 115200 @ 14.7456MHz
  out(ADMUX, 0xc0);     // Internal Vref, right adjust
  out(ADMUX, 0xce);     // Internal Vref, right adjust, read 1.22V (Vbg)
  out(ADCSR, 0xcf);     // Enable ADC interrupts, Clock/128 
  out(SREG, 0x80);	// Global interrupt enable 

  while (1) {
    unsigned char bits, channels, config;
    out(SREG, 0x00);    // Global interrupt disable 
    bits = serial_readbits;
    serial_readbits = 0;
    channels = serial_readchannels;
    serial_readchannels = 0;
    config = serial_readconfig;
    serial_readconfig = 0;
    out(SREG, 0x80);    // Global interrupt enable 
    if (bits & 0x01) { putbit(0, error); }
    if (channels & 0x01) { putchannel(0, temp); }
    if (channels & 0x02) { putchannel(1, level); }
    if (channels & 0x04) { putchannel(2, in_pump.rate); }
    if (channels & 0x08) { putchannel(3, out_pump.rate); }
    if (channels & 0x10) { putchannel(4, heater.rate); }
    if (channels & 0x20) { putchannel(5, mixer.rate); }
    if (channels & 0x40) { putchannel(6, cooler.rate); }
    if (config) {
      CONF_DIGITAL_IN(0, CONF_RESOLUTION(1));			// Error bit

      CONF_DIGITAL_OUT(0, CONF_RESOLUTION(1));			// Pump in 
      CONF_DIGITAL_OUT(1, CONF_RESOLUTION(1));			// Pump out
      CONF_DIGITAL_OUT(2, CONF_RESOLUTION(1));			// Heater
      CONF_DIGITAL_OUT(3, CONF_RESOLUTION(1));			// Mixer
      CONF_DIGITAL_OUT(4, CONF_RESOLUTION(1));			// Cooler

      CONF_ANALOG_IN(0, CONF_RESOLUTION(10));			// Temp
      CONF_ANALOG_IN(0, CONF_MIN(CONF_POSITIVE_VOLT(0)));
      CONF_ANALOG_IN(0, CONF_MAX(CONF_POSITIVE_VOLT(10)));
      CONF_ANALOG_IN(1, CONF_RESOLUTION(10));			// Level
      CONF_ANALOG_IN(1, CONF_MIN(CONF_POSITIVE_VOLT(0)));
      CONF_ANALOG_IN(1, CONF_MAX(CONF_POSITIVE_VOLT(10)));
      CONF_ANALOG_IN(2, CONF_RESOLUTION(8));			// Pump in
      CONF_ANALOG_IN(2, CONF_MIN(CONF_POSITIVE_VOLT(0)));
      CONF_ANALOG_IN(2, CONF_MAX(CONF_POSITIVE_VOLT(10)));
      CONF_ANALOG_IN(3, CONF_RESOLUTION(8));			// Pump out
      CONF_ANALOG_IN(3, CONF_MIN(CONF_POSITIVE_VOLT(0)));
      CONF_ANALOG_IN(3, CONF_MAX(CONF_POSITIVE_VOLT(10)));
      CONF_ANALOG_IN(4, CONF_RESOLUTION(8));			// Heater
      CONF_ANALOG_IN(4, CONF_MIN(CONF_POSITIVE_VOLT(0)));
      CONF_ANALOG_IN(4, CONF_MAX(CONF_POSITIVE_VOLT(10)));
      CONF_ANALOG_IN(5, CONF_RESOLUTION(8));			// Mixer
      CONF_ANALOG_IN(5, CONF_MIN(CONF_POSITIVE_VOLT(0)));
      CONF_ANALOG_IN(5, CONF_MAX(CONF_POSITIVE_VOLT(10)));
      CONF_ANALOG_IN(6, CONF_RESOLUTION(8));			// Cooler
      CONF_ANALOG_IN(6, CONF_MIN(CONF_POSITIVE_VOLT(0)));
      CONF_ANALOG_IN(6, CONF_MAX(CONF_POSITIVE_VOLT(10)));

      CONF_ANALOG_OUT(0, CONF_RESOLUTION(8));			// Heater
      CONF_ANALOG_OUT(0, CONF_MIN(CONF_POSITIVE_VOLT(0)));
      CONF_ANALOG_OUT(0, CONF_MAX(CONF_POSITIVE_VOLT(10)));
      CONF_ANALOG_OUT(1, CONF_RESOLUTION(8));			// Cooler
      CONF_ANALOG_OUT(1, CONF_MIN(CONF_POSITIVE_VOLT(0)));
      CONF_ANALOG_OUT(1, CONF_MAX(CONF_POSITIVE_VOLT(10)));
      CONF_ANALOG_OUT(2, CONF_RESOLUTION(8));			// Pump in
      CONF_ANALOG_OUT(2, CONF_MIN(CONF_POSITIVE_VOLT(0)));
      CONF_ANALOG_OUT(2, CONF_MAX(CONF_POSITIVE_VOLT(10)));
      CONF_ANALOG_OUT(3, CONF_RESOLUTION(8));			// Pump out
      CONF_ANALOG_OUT(3, CONF_MIN(CONF_POSITIVE_VOLT(0)));
      CONF_ANALOG_OUT(3, CONF_MAX(CONF_POSITIVE_VOLT(10)));
      CONF_ANALOG_OUT(4, CONF_RESOLUTION(8));			// Mixer
      CONF_ANALOG_OUT(4, CONF_MIN(CONF_POSITIVE_VOLT(0)));
      CONF_ANALOG_OUT(4, CONF_MAX(CONF_POSITIVE_VOLT(10)));

      CONF_END();
    }
  }
}


static void select_rate(volatile rate_record *port) {
  unsigned char rate;
  rate = 0;
  if (port->serial != 0) {
    rate = port->serial;
  }
  if (port->analog > 10) {
    rate = port->analog;
    if (port->serial != 0) {
      port->serial = 0;
    }
  }
  if (port->digital != 0) {
    rate = port->digital;
    if (port->serial != 0) {
      port->serial = 0;
    }
  }
  if (port->rate != rate) {
    port->rate = rate;
  }
}

static unsigned int filter(volatile unsigned int old, unsigned int adc) 
{
  unsigned int result;
  if (old < adc) {
    result = old + 1;
  } else if (old > adc) {
    result = old - 1;
  } else {
    result = old;
  }
  return result;
}
