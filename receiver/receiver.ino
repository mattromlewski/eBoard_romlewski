/*
 * eSk8 Reciever - Matthew A. Romlewski
 * June 28 2020
 */
#include <math.h> 

#define RXSTATE_INIT_WAIT       0
#define RXSTATE_READ_CMD_TYPE   1
#define RXSTATE_BUFFERING       2
#define RXSTATE_READCHECKSUM    3
#define RXSTATE_EXEC_CMD        4

#define RXFLAG_INIT             0xE6 
#define RXFLAG_CHECKSUM         0xF0

#define CMD_UPDATE_THROTTLE     
#define CMD_POKE_WATCHDOG
#define CMD_READ_SPEED        

const int PWM_PIN = 2;
const float MAX_RX_THROTTLE = 330.0;

char g_rx_buffer[128] = {};
uint16_t g_rx_state = RXSTATE_INIT_WAIT;
bool g_buf_initialized = false;
uint16_t g_rx_buf_counter = 0;
char g_rx_buf[16] = {};
char g_cmd_type = 0x0;

static void update_throttle(int rx_throttle) {
  static uint16_t throttle = rx_throttle;
  uint16_t tx_pwm = 255 * throttle / MAX_RX_THROTTLE;
  Serial.print("PWM Value: ");
  Serial.println(tx_pwm);
}

/** Calculate the checksum of a char array.
 * If sum of char values is <= 255, checksum = that sum.
 * If sum of char values is > 255, then the checksum is 
 * the remainder of the total value after it has been 
 * divided by 256.
 * @param buf array of characters to calc upon.
 * @param len the length of the char array.
 * @return the calculated 8-bit checksum
 */
static char simple_checksum(char buf[], int len) {
  uint16_t pre_sum = 0;
  for(int i=0; i < len; i++) {
    pre_sum += buf[i];
  }
  uint16_t bytes_subtracted = pre_sum / 256; // will truncate
  return pre_sum - (bytes_subtracted * 256);
}

static uint16_t string2int(char str[], uint16_t str_len) {
  int16_t exponent = str_len - 1;
  uint16_t sum = 0;  
  for(int i = 0; i < str_len; i++) {
    int digit = (str[i] - '0');
    sum += pow(digit, exponent);
    exponent--;
  }
  return sum;
}

void setup() {
  Serial.begin(115200);
  Serial3.begin(38400);
  pinMode(PWM_PIN, OUTPUT);
  char g_rx_buffer[4] ="";
  uint16_t g_rx_state = RXSTATE_INIT_WAIT;
  char testStr[10] = {0x1F,0x23, 0x25};
  uint16_t test_sum = string2int(testStr, 3);
  Serial.print("test sum: ");
  Serial.println(test_sum);
}

void loop() {
  Serial.println(g_rx_state);
  switch(g_rx_state) {
    case RXSTATE_INIT_WAIT:
      if(Serial3.available() > 0){
        char rx_byte = Serial3.read();
        Serial.println(rx_byte);
        if(rx_byte == RXFLAG_INIT) {
          g_rx_state = RXSTATE_READ_CMD_TYPE;
        }
      }
    case RXSTATE_READ_CMD_TYPE:
      if(Serial3.available() > 0){
        char rx_byte = Serial3.read();
        g_cmd_type = rx_byte;
        g_rx_state = RXSTATE_BUFFERING;
      }
    case RXSTATE_BUFFERING:
      if(Serial3.available() > 0){
        char rx_byte = Serial3.read();
        if(rx_byte == RXFLAG_CHECKSUM) {
          /* end of payload.
           * don't set buf count to zero because we need 
           * to know where to insert cmd type byte.
           */
          g_rx_state = RXSTATE_READCHECKSUM;
        }
        else {
          g_rx_buf[g_rx_buf_counter] = rx_byte;
          g_rx_buf_counter += 1;
        }
      }
    case RXSTATE_READCHECKSUM:
      if(Serial3.available() > 0) {
          char rx_byte = Serial3.read();
      }
      // insert message and command type into check buffer
      char check_buf[g_rx_buf_counter + 1] = {};
      for(int i = 0; i < g_rx_buf_counter; i++) {
        check_buf[i] = g_rx_buf[i];
      }
      check_buf[g_rx_buf_counter] = g_cmd_type;
      // calculate checksum
      char calc_checksum = simple_checksum(check_buf, g_rx_buf_counter + 1);
      //Serial.println(calc_checksum);
      //Serial.println("hello");
      g_rx_buf_counter = 0;
      g_rx_state = RXSTATE_INIT_WAIT;

      // if passed, go on to action phase
      

      // failed, go on to waiting phase

      // reset buffer count

    //case RXSTATE_ACTION:

  }
//  if(Serial3.available() > 0){
//    ul_rx_throttle = Serial3.read();
//    //ul_rx_throttle = 0x0A;
//    Serial.print("byte received: ");
//    Serial.write(ul_rx_throttle);
//    Serial.println("");
//  } 
}
