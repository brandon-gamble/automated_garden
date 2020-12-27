// https://learn.adafruit.com/multi-tasking-the-arduino-part-1?view=all

/*    rev history
 *
 *    3 - timed switch class -> implemented for air.
 *        - fixed NO/NC problem for using with relay. need to do backwards HIGH/LOW
 *    4 - adding relay_style to argument for timed_switch
 *    5 - rising edge calculator to keep schedule from drifting
 *    6 - added fastLED switch
 *    7 - added led (no data - just a switch on to 12v led)
 *    8 - delayed start on first update
 *    9 - cleaning up debugging code    (removing a lot of print statements)
 *    10 - BROKEN
 *         - added a humidifer class, which has a parent and child switch (parent = fogger, child = fan)
 *         - parent stays on for full duration, child comes on for x seconds every y seconds
 *         - this allows the water vessel to fill up with mist for y seconds (charging) and then
 *         - the fan comes on for x seconds to disburse the mist to environment
 *    11 - fixed 10 syntax to compile
 *    12 - humidity hysteresis
 *    13 - FINALLY FIXED TABBING OMFG
 */
/* future work
 *    function to take in hour or min and convert to ms
 */

class timed_switch
{
    // Class Member Variables
    // These are initialized at startup
    int powerPin;            // the number of the LED pin
    int relay_style;    // 0 is normally open     (i.e. digitalWrite(LOW)    passes power thru relay)
                                        // 1 is normally closed (i.e. digitalWrite(HIGH) passes power thru relay)
    long OnTime;         // milliseconds of on-time
    long OffTime;        // milliseconds of off-time

    int relay_on;
    int relay_off;

    int initial_state; // will the switch start on or off?

    long initial_delay; // this will delay the first state change
    int delay_flag;

    // These maintain the current state
    int powerState;                                 // powerState used to set the LED
    unsigned long previousMillis;     // will store last time LED was updated

    // Constructor - creates a timed_switch
    // and initializes the member variables and state
    public:
    timed_switch(int pin, int relay, long on, long off, int start_state, long delay0)
    {
    ///////////////////////
    // collect arguments //
    ///////////////////////
    powerPin = pin;
    pinMode(powerPin, OUTPUT);        // set the pin to output

    relay_style = relay;

    OnTime = on;
    OffTime = off;

    initial_state = start_state;

    initial_delay = delay0;
    delay_flag = HIGH; // flag starts high

    ////////////////////////////////////
    // initialize relay on/off states //
    ////////////////////////////////////
    /*
     * a bit confusing - the normally open circuit takes HIGH from duino to stay open
     * so if arduino loses power, NO will be open and system goes off
     * but while arduino is attached, a low actually closes the NO
     */
    // int relay_on;
    // int relay_off;
    // normally open
    if(relay_style == 0 ){
        relay_on = LOW;
        relay_off = HIGH;
        initial_state = !initial_state;
    }
    else if(relay_style == 1){
        relay_on = HIGH;
        relay_off = LOW;
    } // end of if logic

    ///////////////////////////////
    // initialize state and time //
    ///////////////////////////////
    digitalWrite(powerPin, relay_off);
    previousMillis = 0;



    } // ^ end of Constructor

    void timer_check()
    {
        // check to see if it's time to change the state of the LED
        unsigned long currentMillis = millis();

        if ((delay_flag == HIGH) && (currentMillis >= initial_delay)) {
            powerState = initial_state;
            digitalWrite(powerPin, powerState);
            delay_flag = LOW;
            previousMillis = currentMillis;    // Remember the time
        }

        else if (delay_flag == LOW) {
            if((powerState == relay_on) && (currentMillis >= OnTime + previousMillis))
            {
                Serial.println("power on, turning off...");
                powerState = !powerState;    // Turn it off
                previousMillis = currentMillis;    // Remember the time
                digitalWrite(powerPin, powerState);    // Update the actual LED
            } // end switch on -> off
            else if ((powerState == relay_off) && (currentMillis >= OffTime + previousMillis))
            {
                Serial.println("power off, turning on...");
                powerState = !powerState;    // turn it on
                previousMillis = currentMillis;     // Remember the time
                digitalWrite(powerPin, powerState);     // Update the actual LED
            } // end switch off -> on
        } // end delay_flag LOW if statement


    } // end timer_check
}; // end class

class timed_switch_PWM
{
    // Class Member Variables
    // These are initialized at startup
    int powerPin;            // the number of the LED pin
    long OnTime;         // milliseconds of on-time
    long OffTime;        // milliseconds of off-time

    int dutyCycle; // pwm duty cycle, 0-255

    // These maintain the current state
    int powerState;                                 // powerState used to set the LED
    unsigned long previousMillis;     // will store last time LED was updated

    // Constructor - creates a timed_switch
    // and initializes the member variables and state
    public:
    timed_switch_PWM(int pin, int duty, long on, long off)
    {
    powerPin = pin;
    pinMode(powerPin, OUTPUT);

    dutyCycle = duty;

    OnTime = on;
    OffTime = off;

    powerState = HIGH;
    previousMillis = 0;

    analogWrite(powerPin, dutyCycle);
    }

    void timer_check()
    {
        // check to see if it's time to change the state of the LED
        unsigned long currentMillis = millis();

         // normally closed
         // pin high is power on
        if((powerState == HIGH) && (currentMillis - previousMillis >= OnTime))
        {
            powerState = !powerState;    // Turn it off
            previousMillis = currentMillis;    // Remember the time
            analogWrite(powerPin, 0);    // Update the actual LED
            Serial.println("pin low, led off");
        }
        else if ((powerState == LOW) && (currentMillis - previousMillis >= OffTime))
        {
            powerState = !powerState;    // turn it on
            previousMillis = currentMillis;     // Remember the time
            analogWrite(powerPin, dutyCycle);     // Update the actual LED
            Serial.println("pin high, led on");
        }

    }
};

class timed_fogger
{
    // Class Member Variables
    // These are initialized at startup
    int fogPin;
    int fanPin;

    int fog_relay_style;    // 0 is normally open     (i.e. digitalWrite(LOW)    passes power thru relay)
                                        // 1 is normally closed (i.e. digitalWrite(HIGH) passes power thru relay)
    int fan_relay_style;

    long OnTime;         // milliseconds of on-time
    long OffTime;        // milliseconds of off-time

    long chargeTime; // time to "charge" mist in water container
    long blowTime;     // time to disperse charged mist

    int fan_relay_on;    // states
    int fan_relay_off;

    int fog_relay_on; // states
    int fog_relay_off;

    int initial_state; // will the switch start on or off?

    int initial_delay; // this will delay the first state change
    int delay_flag;

    // These maintain the current state
    int fog_powerState;                                 // powerState used to set the LED
    int fan_powerState;

    unsigned long fog_previousMillis;     // will store last time LED was updated
    unsigned long fan_previousMillis;     // will store last time LED was updated

    // Constructor - creates a timed_switch
    // and initializes the member variables and state
    public:
    timed_fogger(int fog_pin, int fog_relay, long on, long off, int start_state, int delay0,
                int fan_pin, int fan_relay, int blow_time, int charge_time)
    {
    ///////////////////////
    // collect arguments //
    ///////////////////////
    fogPin = fog_pin;
    fanPin = fan_pin;
    pinMode(fogPin, OUTPUT);        // set the pin to output
    pinMode(fanPin, OUTPUT);        // set the pin to output

    fog_relay_style = fog_relay;
    fan_relay_style = fan_relay;

    OnTime = on;
    OffTime = off;

    chargeTime = charge_time;
    blowTime = blow_time;

    initial_state = start_state;

    initial_delay = delay0;
    delay_flag = HIGH; // flag starts high

    ////////////////////////////////////
    // initialize relay on/off states //
    ////////////////////////////////////
    /*
     * a bit confusing - the normally open circuit takes HIGH from duino to stay open
     * so if arduino loses power, NO will be open and system goes off
     * but while arduino is attached, a low actually closes the NO
     */
    // int relay_on;
    // int relay_off;
    // normally open
    if(fog_relay_style == 0 ){
        fog_relay_on = LOW;
        fog_relay_off = HIGH;
        initial_state = !initial_state;
    }
    else if(fog_relay_style == 1){
        fog_relay_on = HIGH;
        fog_relay_off = LOW;
    } // end of if logic

    if(fan_relay_style == 0 ){
        fan_relay_on = LOW;
        fan_relay_off = HIGH;
    }
    else if(fan_relay_style == 1){
        fan_relay_on = HIGH;
        fan_relay_off = LOW;
    } // end of if logic

    ///////////////////////////////
    // initialize state and time //
    ///////////////////////////////
    digitalWrite(fogPin, fog_relay_off);
    digitalWrite(fanPin, fan_relay_off);

    fog_previousMillis = 0;
    fan_previousMillis = 0;



    } // ^ end of Constructor

    void timer_check()
    {
        // check to see if it's time to change the state of the LED
        unsigned long currentMillis = millis();

        if ((delay_flag == HIGH) && (currentMillis >= initial_delay)) {
            fog_powerState = initial_state;
            digitalWrite(fogPin, fog_powerState);
            delay_flag = LOW;
            fog_previousMillis = currentMillis;    // Remember the time
            fan_previousMillis = currentMillis;
            Serial.println("fog initialized from initial delay");
        }

        else if (delay_flag == LOW) {

            if (fog_powerState == fog_relay_on)
            {
                if ((fan_powerState == fan_relay_on) && (currentMillis >= blowTime + fan_previousMillis))
                {
                    fan_powerState = !fan_powerState;
                    fan_previousMillis = currentMillis;
                    digitalWrite(fanPin, fan_powerState);
                    Serial.println("fog on, fan turned off");
                }
                else if ((fan_powerState == fan_relay_off) && (currentMillis >= chargeTime + fan_previousMillis))
                {
                    fan_powerState = !fan_powerState;
                    fan_previousMillis = currentMillis;
                    digitalWrite(fanPin, fan_powerState);
                    Serial.println("fog on, fan turned on");
                } // end fan switch on -> off
            }

             // if fog has been on long enough, turn it off
            if((fog_powerState == fog_relay_on) && (currentMillis >= OnTime + fog_previousMillis))
            {
                fog_powerState = !fog_powerState;    // Turn it off
                fog_previousMillis = currentMillis;    // Remember the time
                fan_previousMillis = currentMillis;    // Remember the time
                digitalWrite(fogPin, fog_powerState);    // Update the actual LED

                fan_powerState = fan_relay_off;
                digitalWrite(fanPin, fan_powerState);
                Serial.println("FOG AND FAN TURNED OFF");
            } // end fog switch on -> off
            // if fog has been off long enough, turn it on
            else if ((fog_powerState == fog_relay_off) && (currentMillis >= OffTime + fog_previousMillis))
            {
                fog_powerState = !fog_powerState;    // turn it on
                fog_previousMillis = currentMillis;     // Remember the time
                fan_previousMillis = currentMillis;
                digitalWrite(fogPin, fog_powerState);     // Update the actual LED
                Serial.println("FOG TURNED ON");
            } // end switch off -> on
        } // end delay_flag LOW if statement


    } // end timer_check
}; // end class

class fogger
{
    int FOG_POWER_PIN;
    int FAN_POWER_PIN;

    int FOG_RELAY_STYLE;
    int FAN_RELAY_STYLE;

    long CHARGE_TIME;
    long BLOW_TIME;

    int FOG_RELAY_ON;
    int FOG_RELAY_OFF;

    int FAN_RELAY_ON;
    int FAN_RELAY_OFF;

    int FOG_POWER_STATE;
    int FAN_POWER_STATE;

    unsigned long FOG_PREVIOUS_MILLIS;
    unsigned long FAN_PREVIOUS_MILLIS;

    public:
    fogger(int fog_power_pin, int fog_relay_style,
                int fan_power_pin, int fan_relay_style,
                long blow_time, long charge_time)
    {
        FOG_POWER_PIN = fog_power_pin;
        FOG_RELAY_STYLE = fog_relay_style;

        FAN_POWER_PIN = fan_power_pin;
        FAN_RELAY_STYLE = fan_relay_style;

        BLOW_TIME = blow_time;
        CHARGE_TIME = charge_time;

        pinMode(FOG_POWER_PIN, OUTPUT);
        pinMode(FAN_POWER_PIN, OUTPUT);

        if(FOG_RELAY_STYLE == 0){
            FOG_RELAY_ON = LOW;
            FOG_RELAY_OFF = HIGH;
        }
        else if(FOG_RELAY_STYLE == 1){
            FOG_RELAY_ON = HIGH;
            FOG_RELAY_OFF = LOW;
        }

        if(FAN_RELAY_STYLE == 0){
            FAN_RELAY_ON = LOW;
            FAN_RELAY_OFF = HIGH;
        }
        else if(FAN_RELAY_STYLE == 1){
            FAN_RELAY_ON = HIGH;
            FAN_RELAY_OFF = LOW;
        }

        digitalWrite(FOG_POWER_PIN, FOG_RELAY_OFF);
        digitalWrite(FAN_POWER_PIN, FAN_RELAY_OFF);

        // FOG_PREVIOUS_MILLIS = 0;
        FAN_PREVIOUS_MILLIS = 0;
    }

    void timer_check()
    {
        unsigned long currentMillis = millis();

        FOG_POWER_STATE = FOG_RELAY_ON;
        digitalWrite(FOG_POWER_PIN, FOG_POWER_STATE);

        if ((FAN_POWER_STATE == FAN_RELAY_ON) && (currentMillis >= BLOW_TIME + FAN_PREVIOUS_MILLIS))
        {
            FAN_POWER_STATE = !FAN_POWER_STATE;
            FAN_PREVIOUS_MILLIS = currentMillis;
            digitalWrite(FAN_POWER_PIN, FAN_POWER_STATE);
        }
        else if ((FAN_POWER_STATE == FAN_RELAY_OFF) && (currentMillis >= CHARGE_TIME + FAN_PREVIOUS_MILLIS))
        {
            FAN_POWER_STATE = !FAN_POWER_STATE;
            FAN_PREVIOUS_MILLIS = currentMillis;
            digitalWrite(FAN_POWER_PIN, FAN_POWER_STATE);
        }
    }// end timer_check

    void sys_off()
    {
        FOG_POWER_STATE = FOG_RELAY_OFF;
        FAN_POWER_STATE = FAN_RELAY_OFF;
        digitalWrite(FOG_POWER_PIN, FOG_POWER_STATE);
        digitalWrite(FAN_POWER_PIN, FAN_POWER_STATE);
    }
}; // end fogger class


long risingEdgeOffAdjust(long on, long interval){
    /*
     * interval is the interval of time between on cycles
     * on is the time you want switch to be on
     */
    long off;
    off = interval - on;
    return off;
}


////////////////////////////////////////
//                     time tables                            //
////////////////////////////////////////

// 1 hr =    3 600 000 ms
// 3 hr = 10 800 000 ms
long h1 =     3600000;
long h2 =     7200000;
long h3 =    10800000;
long h12 = 43200000;

// 1     min =    60 000 ms
// 2     min = 120 000 ms
// 2.5 min = 150 000 ms
// 5.0 min = 300 000 ms
// 10    min = 600 000 ms
long m4 =    240000;
long m5    = 300000;
long m10 = 600000;

// air(4, 180000, 10800000); // 3.0 min every 3.0 hr
// air(4, 300000, 10800000); // 5.0 min every 3.0 hr

long off_m4_h3 = risingEdgeOffAdjust(m4, h3);

long off_m5_h1 = risingEdgeOffAdjust(m5, h1); // off interval to keep m5_h1 lined up to start every hour
long off_m5_h3 = risingEdgeOffAdjust(m5, h3);


long off_m10_h1 = risingEdgeOffAdjust(m10, h1);
long off_m10_h2 = risingEdgeOffAdjust(m10, h2);

////////////////////////////////////////
//                            switches                            //
////////////////////////////////////////

// switch for fans
//timed_switch(int pin, int relay, long on, long off, int start_state, int delay0);
//timed_switch air(4, 0, m5, off_m5_h1, HIGH, m10); // 5 min every 1 hr (pin 4) (normally open) (rising edge drift accounted for);
timed_switch air(4, 0, m4, off_m4_h3, HIGH, m10); // 5 min every 1 hr (pin 4) (normally open) (rising edge drift accounted for);

// switch for LEDs
timed_switch_PWM light(9, 100, h12, h12);
//timed_switch_PWM light(9, 100, 2000, 2000);

//fog = (fog_pin, fog_relay, on, off, start_state, delay0,
//                 fan_pin, fan_relay, blow_time, charge_time);
//fogger fog(6, 1, 10000, 3000, HIGH, 0,
//                 7, 1, 1000, 3000);
// timed_fogger fog(6, 0, m10, off_m10_h1, HIGH, 0,
                 // 7, 1, 2500, 30000);
// on
// charge 30s, blow 2.5s

// fogger(fog_power_pin, fog_relay_style,
//                fan_power_pin, fan_relay_style,
//                blow_time, charge_time)
fogger fog(6, 0, 7, 1, 3000, 45000);

const int DRY_THRESH = 85;
const int WET_THRESH = 98;
int fog_flag = LOW; // will use this as a global state for hysteresis

#include <DHT.h>
#define DHT_1_PIN 11
#define DHTTYPE DHT22
DHT DHT_1(DHT_1_PIN, DHTTYPE);

float hum_1;
float temp_1;

float hum_avg; //can implement this later as an average humidity when
// multiple sensors are in place

void setup()
{
    Serial.begin(9600);
    Serial.println("");
    Serial.println("");
    Serial.println("-------------------------------------------------------------");
    Serial.println("-------------------------------------------------------------");
    Serial.println("-------------------------------------------------------------");
    Serial.println("oop_env_automation_r12");

    while (!Serial) { // wait for serial connection
        delay(10);
    }

    DHT_1.begin();

//    Serial.println(-10 >= 10);
//    delay(5000);

} // end void setup


void loop()
{
    air.timer_check();
    light.timer_check();
    // fog.timer_check();


    // check humidity
    hum_1 = DHT_1.readHumidity();
    temp_1 = DHT_1.readTemperature(); // given in C
    temp_1 = (temp_1*9/5) + 32; // convert to F

    hum_avg = hum_1; // can make this an average one multiple sensors are set up
    // Serial.println(hum_avg);

    if (hum_avg < DRY_THRESH) // if fogger is off, hum low
    {
        // fog_flag = !fog_flag;
        fog.timer_check();
        // digitalWrite(LED_BUILTIN, HIGH);
        // digitalWrite(5, HIGH);
        // Serial.print("fog loop on: ");
        // Serial.println(hum_avg);
    }
    else if (hum_avg > WET_THRESH)
    {
        // fog_flag = !fog_flag;
        fog.sys_off();
        // digitalWrite(LED_BUILTIN, LOW);
        // digitalWrite(5, LOW);
        // Serial.print("fog loop off: ");
        // Serial.println(hum_avg);
    }


} // end void loop
