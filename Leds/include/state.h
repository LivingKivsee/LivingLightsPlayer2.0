#ifndef __STATE_H__
#define __STATE_H__

#define OUTGPIO0 23
#define OUTGPIO1 22
#define OUTGPIO2 19
#define OUTGPIO3 18

#define INGPIO0 0
#define INGPIO1 1
#define INGPIO2 17

enum OutputState {IDLE, BACK0, BACK1, BACK2, BACK3, BACK4, BACK5, BACK6, BACK7,BACK8, BACK9, BACK10, BACK11, OUT_TRIG1, OUT_TRIG2, OUT_TRIG3};
enum InputState {NOTRIG, TRIG1, TRIG2, TRIG3};

void outputStateInit();
void outputStateEncode (OutputState state);
void inputStateInit();
InputState inputStateDecode();


#endif // __STATE_H__
