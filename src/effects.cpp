#include <Arduino.h>
#include <FastLED.h>

class effects{
    public:
        CRGB *leds;
        int NUM_LED;
        int mode = 0;
        float fps = 0;
        float draw_time = 0;
        int last_draw = 0;
        int frame = 0;
        bool mirror = 0;
        int pointer;
        uint8_t* blades;
        effects(int num, CRGB leds_[]){
            leds = leds_;
            blades = new uint8_t[num];
            NUM_LED = num;
        }
        void tick(){
            frame++;
            draw_time = millis() - last_draw;
            last_draw = millis();
            fps = 1000 / draw_time;
            // Serial.println("===================");
            // Serial.println(frame);
            // Serial.println(draw_time);
            // Serial.println(fps);
            switch (mode) {
                case 0: set_color(CRGB::White); break;
                case 1: set_color(CRGB::Red); break;
                case 2: set_color(CRGB::Green); break;
                case 3: set_color(CRGB::Blue); break;
                case 4: set_rainbow(10000, 0.1); break;
                case 5: set_rainbow(10.66,0.1); break;
                case 6: set_color(CRGB::White); noise_m(10, 10); break;
                case 7: set_color(CRGB::White); noise_h(100, 10, 128); break;
            }
        }
        void set_color(CRGB color){ //поставить цвет
            for (int i = 0; i < NUM_LED; i++) {
                leds[i] = color;
            }
        }
        void set_rainbow(int widht, float speed){ // HSV rainbow
            for (int i = 0; i < NUM_LED; i++) {
                leds[i] = CHSV(((i * widht) + (int) (frame * speed))%255, 255, 255);
            }
        }
        void noise_w(int widht, float speed){ //HSV rainbow noise
            for (int i = 0; i < NUM_LED; i++) {
                leds[i] = CHSV((int)(inoise8(i * widht, (int) (frame * speed)) * 1.4),255, 255);
            }
        }
        void noise_m(int widht, float speed){ // ADD noise
            for (int i = 0; i < NUM_LED; i++) {
                leds[i] = leds[i].nscale8(inoise8(i * widht, (int) (frame * speed)));
            }
        }
        void noise_h(int widht, float speed, int threshold){ // ADD hard noise
            for (int i = 0; i < NUM_LED; i++) {
                leds[i] = inoise8(i * widht, (int) (frame * speed))>=threshold ? leds[i] * 1: leds[i]*0;
            }
        }
        void noise_h_c(int widht, float speed, int threshold, CRGB colorA, CRGB colorB){ // hard noise only two colors
            for (int i = 0; i < NUM_LED; i++) {
                leds[i] = inoise8(i * widht, (int) (frame * speed))>=threshold ? colorA: colorB;
            }
        }
        void noise_h_sm(int widht, float speed, CRGB colorA, CRGB colorB){ // smooth noise two colors
            int blend;
            for (int i = 0; i < NUM_LED; i++) {
                blend = inoise8(i * widht, (int) (frame * speed));
                leds[i] = CRGB(map(blend, 0,255, colorA.r, colorB.r), map(blend, 0,255, colorA.g, colorB.g), map(blend, 0,255, colorA.b, colorB.b));
            }
        }
        void sin_m(int widht, float speed){ // smooth ADD sin
            for (int i = 0; i < NUM_LED; i++) {
                leds[i] = leds[i].nscale8(sin8(i * widht + (int) (frame * speed)));
            }
        }
        void sin_h(int widht, float speed, int threshold){ // hard ADD sin
            for (int i = 0; i < NUM_LED; i++) {
                leds[i] = sin8(i * widht + (int) (frame * speed))>=threshold ? leds[i] * 1: leds[i]*0;
            }
        }
        void sin_h_c(int widht, float speed, int threshold, CRGB colorA, CRGB colorB){ // HARD only two colors sin
            for (int i = 0; i < NUM_LED; i++) {
                leds[i] = sin8(i * widht + (int) (frame * speed))>=threshold ? colorA: colorB;
            }
        }
        void sin_s_c(int widht, float speed, CRGB colorA, CRGB colorB){ // Smooth only two colors sin
            int blend;
            for (int i = 0; i < NUM_LED; i++) {
                blend = sin8(i * widht + (int) (frame * speed));
                leds[i] = CRGB(map(blend, 0,255, colorA.r, colorB.r), map(blend, 0,255, colorA.g, colorB.g), map(blend, 0,255, colorA.b, colorB.b));
            }
        }
        void zik_zak(float speed){ // hard zikzak
            pointer = (int) (frame * speed) % NUM_LED;
            for (int i = 0; i < NUM_LED; i++) {
                leds[i] = i == pointer ? leds[i] * 1: leds[i]*0;
            }
        }
        void zik_zak_i(float speed){ // hard inverse zikzak
            pointer = NUM_LED - (int) (frame * speed) % NUM_LED - 1;
            for (int i = 0; i < NUM_LED; i++) {
                leds[i] = i == pointer ? leds[i] * 1: leds[i]*0;
            }
        }
        void zik_zak_s(float speed) { // smooth zikzak
            pointer = (int) (frame * speed) % NUM_LED;
            for (int i = 0; i < NUM_LED; i++) {

                if (blades[i]>=0) {
                    blades[i] = blades[i] - 0.005;
                }
                if (pointer == i) {
                    blades[i] = 255;
                }
                leds[i] = leds[i].nscale8(blades[i]);
            }
        }
        void zik_zak_si(float speed) { // smooth zikzak inverse
            pointer = NUM_LED - (int) (frame * speed) % NUM_LED - 1;
            for (int i = 0; i < NUM_LED; i++) {
                if (blades[i]>=0) {
                    blades[i] = blades[i] - 0.005;
                }
                if (pointer == i) {
                    blades[i] = 255;
                }
                leds[i] = leds[i].nscale8(blades[i]);
            }
        }
        void bladefade(int how){ // random blades
            for (int i = 0; i < NUM_LED; i++) {
                blades[i] = blades[i] / 2;
                if (random(0, 255) <= how) {
                    blades[i] = 255;
                }
                leds[i] = leds[i].nscale8(blades[i]);
            }
        }
};
