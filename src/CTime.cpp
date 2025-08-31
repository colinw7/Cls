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

std::string
CTime::
getRelTime()
{
  CTime current_time;

  int y1 =              getYear();
  int y2 = current_time.getYear();

  if (y1 == y2) {
    int m1 =              getMonth();
    int m2 = current_time.getMonth();

    if (m1 == m2) {
      int d1 =              getMonthDay();
      int d2 = current_time.getMonthDay();

      if (d1 == d2) {
        int H1 =              getHour();
        int H2 = current_time.getHour();

        if (H1 == H2) {
          int M1 =              getMin();
          int M2 = current_time.getMin();

          if (M1 == M2) {
            int S1 =              getSec();
            int S2 = current_time.getSec();

            if (S1 == S2)
              return "now";
            else
              return std::to_string(S2 - S1) + " secs ago";
          }
          else
            return std::to_string(M2 - M1) + " mins ago";
        }
        else
          return std::to_string(H2 - H1) + "  hrs ago";
      }
      else
        return std::to_string(d2 - d1) + " days ago";
    }
    else
      return std::to_string(m2 - m1) + " mths ago";
  }
  else
    return std::to_string(y2 - y1) + "  yrs ago";
}
