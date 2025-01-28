#include <CTime.h>
#include <cstring>
#include <time.h>

struct CTimeData {
  time_t     t;
  struct tm *tm;
};

CTime::
CTime()
{
  data_ = new CTimeData();

  data_->t = time(static_cast<time_t *>(nullptr));

  init();
}

CTime::
CTime(time_t t)
{
  data_ = new CTimeData();

  data_->t = t;

  init();
}

CTime::
~CTime()
{
  delete data_;
}

void
CTime::
init()
{
  data_->tm = nullptr;
}

void
CTime::
initTm()
{
  if (! data_->tm) {
    data_->tm = new struct tm;

    memcpy(data_->tm, localtime(&data_->t), sizeof(struct tm));
  }
}

int
CTime::
getYear()
{
  initTm();

  return data_->tm->tm_year + 1900;
}

int
CTime::
getMonth()
{
  initTm();

  return data_->tm->tm_mon;
}

int
CTime::
getWeekDay()
{
  initTm();

  return data_->tm->tm_wday;
}

int
CTime::
getMonthDay()
{
  initTm();

  return data_->tm->tm_mday;
}

int
CTime::
getYearDay()
{
  initTm();

  return data_->tm->tm_yday;
}

int
CTime::
getHour()
{
  initTm();

  return data_->tm->tm_hour;
}

int
CTime::
getMin()
{
  initTm();

  return data_->tm->tm_min;
}

int
CTime::
getSec()
{
  initTm();

  return data_->tm->tm_sec;
}

double
CTime::
diff(const CTime &time)
{
  return difftime(data_->t, time.data_->t);
}

double
CTime::
diff(time_t t)
{
  return difftime(data_->t, t);
}

std::string
CTime::
format(const char *format)
{
  static char str[256];

  initTm();

  strftime(str, 256, format, data_->tm);

  return str;
}

std::string
CTime::
getLsTime(bool show_secs)
{
  std::string str;

  CTime current_time;

  if      (current_time.getYear() == getYear()) {
    // same year within six months, skip year
    if (current_time.getYearDay() - getYearDay() < 183) {
      if (! show_secs)
        str = format("%h %e %H:%M");
      else
        str = format("%h %e %H:%M:%S");
    }
    else
      str = format("%h %e  %Y"); // extra space to align with time (time=HH::MM, year=YYYY)
  }
  else if (current_time.getYear() == getYear() + 1) {
    // prev year within six months, skip year
    if (current_time.getYearDay() + (365 - getYearDay()) < 183) {
      if (! show_secs)
        str = format("%h %e %H:%M");
      else
        str = format("%h %e %H:%M:%S");
    }
    else
      str = format("%h %e  %Y");
  }
  else
    str = format("%h %e  %Y");

  return str;
}
