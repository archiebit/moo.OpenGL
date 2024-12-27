#ifndef MOO_CONSTANT_HH
#define MOO_CONSTANT_HH

#include <string>
#include <cstdint>
#include <map>

#include <pugi/xml.hh>


namespace moo
{
    class constant
    {
    public:
        std::string    title;
        std::uintmax_t value;

    public:
        static void append_list( pugi::xml_node list );
        static void append_node( pugi::xml_node node );

        static void remove_list( pugi::xml_node list );
        static void remove_node( pugi::xml_node node );
    };


    extern std::map<std::string, constant> constants;
}


#endif