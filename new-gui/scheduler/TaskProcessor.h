#ifndef TASK_PROCESSOR_H
#define TASK_PROCESSOR_H

#include "ScheduledTasksList.hxx"

template< class GE >
struct serializer_traits;


template< class GP >
inline bool store( const GP &ge, const std::string &path )
{
	//other possible format: ft = auto_ptr<XMLFormatTarget> (new StdOutFormatTarget ());
	//
	std::auto_ptr< ::xercesc::XMLFormatTarget > ft = std::auto_ptr< ::xercesc::XMLFormatTarget > ( new ::xercesc::LocalFileFormatTarget( path.c_str() ) );
	typename serializer_traits< GP >::converter_t gp;
	xml_schema::namespace_infomap map;			//not used so far
	serializer_traits< GP >::serialize( *ft, ge.IOcopy( gp ) );	//, map 
	return true;
}

template< class GP >
inline bool load( GP &ge, const std::string &path )
{
	std::auto_ptr< typename serializer_traits< GP >::converter_t > pgp = serializer_traits< GP >::deserialize( path, xml_schema::flags::dont_validate );
	ge = *pgp;
	return true;
}



bool readTasks( const std::string &path );

#endif // TASK_PROCESSOR_H