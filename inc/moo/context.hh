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

    private:
        static bool resolve_list( pugi::xml_node list );

        static void save_general_header( );
        static void save_special_header( );
        static void save_special_source( );
    };
}


#endif