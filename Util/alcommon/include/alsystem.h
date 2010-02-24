/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


/**
* Module that manage all system functionalities (linux, win32...)
*/

#ifndef AL_SYSTEM_H
#define AL_SYSTEM_H

#include <cstdlib>
#include "altypes.h"
#include "alvalue.h"
#include <stdio.h>
#include "altools.h"

//#define LOGSYSTEM a
#define MAXDIRSIZE 1000

namespace AL
{
  class ALSystem
  {
  public:
    
    static std::string getHostName();

    static FILE * fOpen(const std::string & pathLib , const char * mode);

    static std::string getCurrentDir(void);

    static std::string getEnv(void);

    static std::string getEnv(std::string pStrEnv);

    static std::string getALPath(void)
    {
      std::string result;
      result = getCurrentDir();

      // not delicate at all !
      result = replace(result, "modules/lib/","",false);
      result = replace(result, "modules/lib","",false);
      result = replace(result, "modules\\lib\\","",false);
      result = replace(result, "modules\\lib","",false);
      result = replace(result, "modules\\bin","",false);
      result = replace(result, "modules/bin","",false);
      result = replace(result, "bin\\","",false);
      result = replace(result, "bin","",false);

      // check path and get $AL_DIR if necessary
      std::string strPath = std::string(result + "/preferences/DCM.xml");
      strPath = replace(strPath,"\\/","/");
      if (!IsFileExist(strPath.c_str()))
      {
        result = getCurrentDir();
        std::string::size_type pos = strPath.find("modules");
        if (pos != std::string::npos)
        {
          result = result.substr(0, pos ) ;
          //std::
          //strPath::
        }
      }
      // let's try again
      strPath = std::string(result + "/preferences/DCM.xml");
      if (!IsFileExist(strPath.c_str()))
      {
        return getEnv();  
      }

      return result;
    }

  };

  class ALTime: public ALValue // heritate from ALValue to allow to put it in ALMemory
  {
  public:
    int wYear;
    int wMonth; // from 0 to 11, warning: human think of 1 to 12, so before stringify them for human, we must add one to month and day...
    int wDay; // from 0 to 27/29/30
    int wHour;
    int wMinute;
    int wSecond;
    int wMilliseconds;

    int wDayOfWeek; // not really used/dev (we put it at the end to speed up comparison)

  public:
   ALTime(void)
   {
     this->wYear = 0;
     this->wMonth = 0;
     this->wDay = 0;

    // $$$$ ALEXANDRE MAZEL 2008-08-28: Adding reseting remaining fields
    this->wHour = 0;
    this->wMinute = 0;
    this->wSecond = 0;
    this->wMilliseconds = 0;

    this->wDayOfWeek = 0;
   }

    // YYYYMMDD
    ALTime( int year, int month, int day )
    {
      this->wYear = year;
      this->wMonth = month;
      this->wDay = day;

      // $$$$ ALEXANDRE MAZEL 2008-08-28: Adding reseting remaining fields
      this->wHour = 0;
      this->wMinute = 0;
      this->wSecond = 0;
      this->wMilliseconds = 0;

      this->wDayOfWeek = 0;

    }

    // YYYYMMDD
    ALTime( int year, int month, int day, int hour, int minute, int second, int millisecond = 0 )
    {
      this->wYear = year;
      this->wMonth = month;
      this->wDay = day;

      this->wHour = hour;
      this->wMinute = minute;
      this->wSecond = second;
      this->wMilliseconds = millisecond;
    }

    void reset( void )
    {
      this->wYear = 0;
      this->wMonth = 0;
      this->wDay = 0;

      this->wHour = 0;
      this->wMinute = 0;
      this->wSecond = 0;
      this->wMilliseconds = 0;

      this->wDayOfWeek = 0;
    }

    // return true if this is strictly superior than base
    bool sup(const ALTime& base) const
    {
       if (wYear > base.wYear)
        return true;
      if (wYear < base.wYear)
        return false;

      if (wMonth > base.wMonth)
        return true;
      if (wMonth < base.wMonth)
        return false;

      if (wDay > base.wDay)
        return true;
      if (wDay < base.wDay)
        return false;

      if (wHour > base.wHour)
        return true;
      if (wHour < base.wHour)
        return false;

      if (wMinute > base.wMinute)
        return true;
      if (wMinute < base.wMinute)
        return false;

      if (wSecond > base.wSecond)
        return true;
      if (wSecond < base.wSecond)
        return false;


      return wMilliseconds > base.wMilliseconds;
    }

    bool operator>( const ALTime& base ) const
    {
      return sup(base);
    }

    bool operator<=( const ALTime& base ) const
    {
      bool res = !(sup(base));
        
      return res;
    }

    bool operator<( const ALTime& base ) const
    {
      bool res = !(sup(base)) && ! this->operator ==(base);
        
      return res;
    }


    bool operator==( const ALTime& base ) const
    {
      // Compare eveything but wDayOfWeek
      return  wYear         == base.wYear 
           && wMonth        == base.wMonth 
           && wDay          == base.wDay
           && wHour         == base.wHour 
           && wMinute       == base.wMinute 
           && wSecond       == base.wSecond 
           && wMilliseconds == base.wMilliseconds; 
    }


    void getLocalTime(); // will return the local time: month 0..11 and day 0..29/30

    // warning: this won't output a "human" date (because month is from 0 to 11)
    std::string toString( std::string strFormat = "YYMMDD" ) const // overload ALValue toString
    {
      #define DATEMAX 50
      char tmp[DATEMAX+1];
      snprintf( tmp, DATEMAX, "%2d%2d%2d", this->wYear, this->wMonth, this->wDay );
      //sprintf(tmp,"%2d%2d%2d",this->wYear,this->wMonth,this->wDay); // deprecated
      std::string result(tmp);
      result = replace(result," ","0",false); // le %02d du printf, c'est pas mal aussi... alors qu'ici on pourra avoir des 00/00/2008 => "  /  /2008" :~
      return result;
    }

    // we should provide a powerfull function like php-one date( "DdHMY..." )
    std::string toFrenchTime( void ) const // overload ALValue toString
    {
      const int nLenBuf = 32;
      char buf[nLenBuf];
      // xx/xx/xxxx 12h59m35
      snprintf( buf, nLenBuf, "%02d/%02d/%04d %02dh%02dm%02d", this->wDay+1, this->wMonth+1, this->wYear, this->wHour, this->wMinute, this->wSecond );
      std::string strResult = buf;
      return strResult;
    }


    void operator=(const std::string &strDate)
    {
      AL_ASSERT(strDate.length()>=6);
      std::string strYear = std::string("20") + strDate.substr(0,2);

     
      wYear = atoi(strYear.c_str());
     
      wMonth = atoi(strDate.substr(2,2).c_str());
     
      wDay = atoi(strDate.substr(4,2).c_str());
    }

    ALTime operator+(const AL::ALTime &pTime)
    {
      int month[12]={31,28,31,30,31,30,31,31,30,31,30,31};
      int tmpDay, tmpMonth, tmpYear, tmpHour, tmpMinute, tmpSecond;
      ALTime result;

      tmpSecond = this->wSecond + pTime.wSecond;

      tmpMinute = this->wMinute + pTime.wMinute + tmpSecond / 60;
      tmpSecond = tmpSecond % 60;

      tmpHour = this->wHour + pTime.wHour + tmpMinute / 60;
      tmpMinute = tmpMinute % 60;

      tmpDay = this->wDay + pTime.wDay + tmpHour / 24;
      tmpHour = tmpHour % 24;

      tmpMonth = this->wMonth + pTime.wMonth + tmpDay / month[this->wMonth]; // WARNING: here we use the nbr of day of the current month and not of the month from pTime or sum of this and pTime
      tmpDay = tmpDay % month[this->wMonth];

      tmpYear = this->wYear + pTime.wYear + tmpMonth / 12;
      tmpMonth = tmpMonth % 12;

      result.wYear = tmpYear;
      result.wMonth = tmpMonth;
      result.wDay = tmpDay;

      result.wHour = tmpHour;
      result.wMinute = tmpMinute;
      result.wSecond = tmpSecond;

      return result;
    }

    // WARNING: this method is in inline, only to avoid duplicate it in alsytem_*
    bool writeToFile( FILE* pfAlreadyOpen ) const
    {
      return fwrite( this, sizeof( *this ), 1, pfAlreadyOpen ) == 1;
    }

    bool readFromFile( FILE* pfAlreadyOpen )
    {
      return fread( this, sizeof( *this ), 1, pfAlreadyOpen ) == 1 ;
    }

    bool isNull() const
    {
      // WARNING: we check only the date, because it's the only by default resetted params (eg in constructor by default)
      return      
               this->wYear == 0
            && this->wMonth == 0
            && this->wDay == 0;
    }
 
  };



}


#endif // AL_SYSTEM_H
