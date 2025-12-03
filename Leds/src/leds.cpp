#include "SdLedsPlayer.h"
#include "state.h"

#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

#define MAX_BRIGHTNESS 255
#define DEFAULT_BRIGHTNESS 50 // range is 0 (off) to 255 (max brightness)
#define STATE_DEBOUNCE_TIME 2

int curr_file_i = 0;
OutputState back_states[] = {BACK0, BACK1, BACK2, BACK3, BACK4, BACK5, BACK6, BACK7, BACK8, BACK9, BACK10, BACK11};
const char *files_iter_rr[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"}; // Make sure file list is not longer than state list
const char *file_trig1 = "1";
const char *file_trig2 = "2";
const char *file_trig3 = "3";

// Song tracking
OutputState state, prevState = IDLE;
InputState inputState = NOTRIG;
unsigned long currSongTime = 0, songStartTime = 0, procTime = 0;
unsigned long stateDebounceDelay = STATE_DEBOUNCE_TIME;
bool allowInterrupt = true;
bool status;

/*
 * SdLedsPlayer is the class that handles reading frames from file on SD card,
 * and writing it to the leds.
 */
SdLedsPlayer sd_leds_player;
unsigned long frame_timestamp;
uint8_t brightness = DEFAULT_BRIGHTNESS;

// Monitoring vars
unsigned long lastMonitorTime = 0;
unsigned long MonitorDelay = 5000;

void setup()
{
    Serial.begin(115200);
    Serial.println("Serial Port Started.");
    while (!sd_leds_player.setup())
    {
        Serial.println("SD card setup failed, fix and reset to continue");
        delay(1000);
    }
    Serial.println("SD card started.");
    sd_leds_player.setBrightness(brightness);
    Serial.print("Brightness set to: ");
    Serial.print(brightness);
    Serial.print(" out of ");
    Serial.println(MAX_BRIGHTNESS);

    // Teensies State setup
    outputStateInit();
    inputStateInit();
    delay(1000);
}

void loop()
{
    // unsigned long tic = millis();

    // if song interruption is allowed, read state from GPIOs and decode
    if (allowInterrupt)
    {
        inputState = inputStateDecode();
        if (inputState != NOTRIG)
        {
            Serial.print("Input state received: ");
            Serial.println(inputState);
            switch (inputState)
            {
            case TRIG1:
                if (sd_leds_player.load_file(file_trig1))
                {
                    state = OUT_TRIG1;
                    allowInterrupt = false;
                    frame_timestamp = sd_leds_player.load_next_frame();
                }
                break;
            case TRIG2:
                if (sd_leds_player.load_file(file_trig2))
                {
                    state = OUT_TRIG2;
                    allowInterrupt = false;
                    frame_timestamp = sd_leds_player.load_next_frame();
                }
                break;
            case TRIG3:
                if (sd_leds_player.load_file(file_trig3))
                {
                    state = OUT_TRIG3;
                    allowInterrupt = false;
                    frame_timestamp = sd_leds_player.load_next_frame();
                }
                break;
            default:
                break;
            }
        }
    }

    // Background LED file loading
    if (!sd_leds_player.is_file_playing())
    {
        state = back_states[curr_file_i];
        Serial.print("No file is playing, loading new file, number: ");
        Serial.println(files_iter_rr[state - 1]);

        status = sd_leds_player.load_file(files_iter_rr[state - 1]); // minus 1 to translate state to filename because IDLE state is 0
        curr_file_i = (curr_file_i + 1) % (sizeof(files_iter_rr) / sizeof(files_iter_rr[0]));
        if (status)
        {
            frame_timestamp = sd_leds_player.load_next_frame();
            allowInterrupt = true;
        }
        else
        {
            state = prevState;
        }
    }

    // State tracking between two teensies
    if (state != prevState)
    {
        songStartTime = millis();
        outputStateEncode(state);
    }
    prevState = state;
    // Holding non IDLE state for a short while so we can use debounce on second teensy to capture state safely
    if (state != IDLE)
    {
        if ((millis() - songStartTime) > stateDebounceDelay)
        {
            state = IDLE;
            songStartTime = millis();
        }
    }

    // Current song frame tracking
    currSongTime = millis() - songStartTime;
    if (currSongTime >= frame_timestamp)
    {
        sd_leds_player.show_next_frame();
        frame_timestamp = sd_leds_player.load_next_frame();
    }

    // Monitor printing, not a must, good for debugging
    // if((millis() - lastMonitorTime) > MonitorDelay) {
    //   Serial.println(F("Leds Alive"));
    //   lastMonitorTime = millis();
    // }

    // Serial.println(millis() - tic);
}