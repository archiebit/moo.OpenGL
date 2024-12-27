#ifndef MOO_CONTEXT_HH
#define MOO_CONTEXT_HH

#include <pugi/xml.hh>


namespace moo
{
    class context
    {
    public:
        static void init( );

    private:
        static bool resolve_list( pugi::xml_node list );
    };
}


#endif