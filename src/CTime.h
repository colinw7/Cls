#ifndef CTIME_H
#define CTIME_H

#include <string>
#include <sys/time.h>

struct CTimeData;

class CTime {
 public:
  CTime();
  CTime(time_t t);
 ~CTime();

  int getYear();
  int getMonth();
  int getWeekDay();
  int getMonthDay();
  int getYearDay();
  int getHour();
  int getMin();
  int getSec();

  double diff(const CTime &time);
  double diff(time_t t);

  std::string format(const char *format);

  std::string getLsTime(bool show_secs=false);

  std::string getRelTime();

 private:
  void init();
  void initTm();

 private:
  CTimeData *data_ { nullptr };
};

#endif
