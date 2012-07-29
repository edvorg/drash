 #include "clogger.h"

 #include <iostream>

using namespace std;

namespace drash
{

class GnLogStarter
{
public:
    GnLogStarter()
    {
		ofstream out;
        out.open( "greng.log", ios_base::ate );
		out.close();
        CLogger()<<LOG_INFO_PREFIX<<"\"greng.log\" started. made by Edward Knyshov";
	}
} starter;

CLogger::CLogger(void)
{
    out.open( "greng.log", ios_base::app );
    mOpened = out.is_open();
}

CLogger::~CLogger(void)
{
    if ( mOpened == true )
	{
		out<<endl;
		cout<<endl;
		out.close();
	}
}

CLogger& CLogger::operator<<( char _c )
{
    if ( mOpened == true )
	{
        out<<_c;
        cout<<_c;
	}
	return (*this);
}

CLogger& CLogger::operator<<( const char* _str )
{
    if ( mOpened == true )
	{
        out<<_str;
        cout<<_str;
	}
	return (*this);
}

CLogger& CLogger::operator<<( int _v )
{
    if ( mOpened == true )
	{
        out<<_v;
        cout<<_v;
	}
	return (*this);
}


CLogger& CLogger::operator<<( unsigned int _v )
{
    if ( mOpened == true )
	{
        out<<_v;
        cout<<_v;
    }
    return (*this);
}

CLogger& CLogger::operator<<( long _v )
{
    if ( mOpened == true )
    {
        out<<_v;
        cout<<_v;
    }
    return (*this);
}


CLogger& CLogger::operator<<( unsigned long _v )
{
    if ( mOpened == true )
    {
        out<<_v;
        cout<<_v;
    }
    return (*this);
}

CLogger& CLogger::operator<<( float _v )
{
    if ( mOpened == true )
	{
        out<<_v;
        cout<<_v;
	}
    return (*this);
}

CLogger &CLogger::operator <<(double _v)
{
    if ( mOpened == true )
    {
        out<<_v;
        cout<<_v;
    }
    return (*this);
}

} // namespace drash