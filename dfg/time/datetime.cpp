#include "DateTime.hpp"
#include "../dfgAssert.hpp"

#if DFG_LANGFEAT_CHRONO_11

#ifdef _WIN32
    #include <Windows.h>
#endif

auto DFG_MODULE_NS(time)::DFG_CLASS_NAME(UtcOffsetInfo)::offsetDiffInSeconds(const DFG_CLASS_NAME(UtcOffsetInfo)& other) const -> int32
{
    if (isSet() && other.isSet())
        return other.offsetInSeconds() - offsetInSeconds();
    else
    {
        DFG_ASSERT_IMPLEMENTED((isSet() || m_offsetValue == s_nNotSetValueMimicing) && (other.isSet() || other.m_offsetValue == s_nNotSetValueMimicing)); // Only implemented for mimic special value.
        return 0; // If either one or both are mimicing, there's no utc offset difference. 
    }
}

DFG_MODULE_NS(time)::DFG_CLASS_NAME(DateTime)::DFG_CLASS_NAME(DateTime)(int year, int month, int day, int hour, int minute, int second, int milliseconds, DFG_CLASS_NAME(UtcOffsetInfo) utcOffsetInfo)
{
    m_year = static_cast<uint16>(year);
    m_month = static_cast<uint8>(month);
    m_day = static_cast<uint8>(day);
    m_milliSecSinceMidnight = millisecondsSinceMidnight(hour, minute, second, milliseconds);
    m_utcOffsetInfo = utcOffsetInfo;
}

DFG_MODULE_NS(time)::DFG_CLASS_NAME(DateTime)::DFG_CLASS_NAME(DateTime)(const SYSTEMTIME& st)
{
    m_year = st.wYear;
    m_month = static_cast<uint8>(st.wMonth);
    m_day = static_cast<uint8>(st.wDay);
    m_milliSecSinceMidnight = millisecondsSinceMidnight(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}

auto DFG_MODULE_NS(time)::DFG_CLASS_NAME(DateTime)::millisecondsSinceMidnight(const int hour, const int minutes, const int seconds, const int milliseconds) -> uint32
{
    return 1000 * (hour * 3600 + minutes * 60 + seconds) + milliseconds;
}

#ifdef _WIN32

auto DFG_MODULE_NS(time)::DFG_CLASS_NAME(DateTime)::privTimeDiff(const SYSTEMTIME& st0, const SYSTEMTIME& st1) -> std::chrono::duration<int64, std::ratio<1, 10000000>>
{
    const auto toInteger = [](const _SYSTEMTIME& st)
    {
        FILETIME ft;
        SystemTimeToFileTime(&st, &ft);
        ULARGE_INTEGER uli;
        uli.LowPart = ft.dwLowDateTime;
        uli.HighPart = ft.dwHighDateTime;
        return std::chrono::duration<int64, std::ratio<1, 10000000>>(static_cast<int64>(uli.QuadPart));
    };

    const auto i0 = toInteger(st0);
    const auto i1 = toInteger(st1);
    return i1 - i0;
}

auto DFG_MODULE_NS(time)::DFG_CLASS_NAME(DateTime)::timeDiffInSecondsI(const SYSTEMTIME& st0, const SYSTEMTIME& st1) -> std::chrono::seconds
{
    const auto diff = privTimeDiff(st0, st1);
    typedef decltype(diff) TimeDiffReturnType;
    return std::chrono::seconds(diff.count() * TimeDiffReturnType::period::num / TimeDiffReturnType::period::den);
}

SYSTEMTIME DFG_MODULE_NS(time)::DFG_CLASS_NAME(DateTime)::toSystemTime() const
{
    SYSTEMTIME st;
    st.wYear = m_year;
    st.wMonth = m_month;;
    //st.wDayOfWeek = ; TODO
    st.wDay = m_day;
    st.wHour = hour();
    st.wMinute = minute();
    st.wSecond = second();
    st.wMilliseconds = millisecond();
    return st;
}
#endif // _WIN32

auto DFG_MODULE_NS(time)::DFG_CLASS_NAME(DateTime)::systemTime_utc() -> DFG_CLASS_NAME(DateTime)
{
#ifdef _WIN32
    SYSTEMTIME stUtc;
    GetSystemTime(&stUtc); // Returned time is in UTC
    DFG_CLASS_NAME(DateTime) dt(stUtc);
    dt.m_utcOffsetInfo.setOffsetInSeconds(0);
    return dt;
#else
    #error Implementation of DFG_CLASS_NAME(DateTime)::systemTime_utc() is not available on current platform.
#endif
}

auto DFG_MODULE_NS(time)::DFG_CLASS_NAME(DateTime)::systemTime_local() -> DFG_CLASS_NAME(DateTime)
{
#ifdef _WIN32
    SYSTEMTIME stUtc;
    GetSystemTime(&stUtc); // Returned time is in UTC
    SYSTEMTIME stLocal;
    SystemTimeToTzSpecificLocalTime(nullptr /*nullptr = use current time zone */, &stUtc, &stLocal);
    DFG_CLASS_NAME(DateTime) dt(stLocal);
    dt.m_utcOffsetInfo.setOffset(timeDiffInSecondsI(stUtc, stLocal));
    return dt;
#else
    #error Implementation of DFG_CLASS_NAME(DateTime)::systemTime_local() is not available on current platform.
#endif
}

std::chrono::duration<double> DFG_MODULE_NS(time)::DFG_CLASS_NAME(DateTime)::secondsTo(const DFG_CLASS_NAME(DateTime)& other) const
{
    return privTimeDiff(toSystemTime(), other.toSystemTime()) - std::chrono::duration<double>(m_utcOffsetInfo.offsetDiffInSeconds(other.m_utcOffsetInfo));
}

#endif // DFG_LANGFEAT_CHRONO_11
