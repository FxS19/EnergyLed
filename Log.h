#pragma once
//#define LOG_ENABLE
class Log{
  public:
    static void writeln(String message)
    {
      #ifdef LOG_ENABLE
      Serial.println(message);
      #endif
    }

    static void write(String message)
    {
      #ifdef LOG_ENABLE
      Serial.print(message);
      #endif
    }
};