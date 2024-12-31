#ifndef MOO_CONTEXT_HH
#define MOO_CONTEXT_HH

#include <pugi/xml.hh>


namespace moo
{
    class context
    {
    public:
        static void init( );
        static void save( );

        static void define( std::string & target );
        static void change( std::string & target, std::string const & sample, std::string const & source );

    private:
        static bool resolve_list( pugi::xml_node list );

        static void save_general_header( );
        static void save_special_header( );
        static void save_special_source( );
        static void save_loading_source( );
    };
}


#endif