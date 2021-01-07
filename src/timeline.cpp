#include <Arduino.h>
class event{
    private:
    public:
        int *timestamp;
        int time_start;
        int time_end;
        event(int _time_start, int _time_end){
            time_start = _time_start;
            time_end = _time_end;
        }
        bool between(){
            int start = time_start % 86400;
            int end = time_end % 86400;
            int now = *timestamp % 86400;
            // Serial.print(start); Serial.print(":");
            // Serial.print(now); Serial.print(":");
            // Serial.print(end); Serial.println(":");
            if (end <= start) {
                end += 86400;
                if (now<start) {
                    now += 86400;
                }
            }
            if(end >= now){
                if (now >= start) {
                    return 1;
                }
            }
            return 0;
        }
        int betweenMap(){
            //86400
            int start = time_start % 86400;
            int end = time_end % 86400;
            int now = *timestamp % 86400;
            if (end <= start) {
                end += 86400;
                if (now<start) {
                    now += 86400;
                }
            }
            if(end >= now){
                if (now >= start) {
                    return map(now, start, end, 0, 65535);
                }
            }
            return 0;
        }
        long map(long x, long in_min, long in_max, long out_min, long out_max)
        {
          return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        }
};
