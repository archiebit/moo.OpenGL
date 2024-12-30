#ifndef MOO_FUNCTION_HH
#define MOO_FUNCTION_HH

#include <string>
#include <vector>
#include <map>

#include <pugi/xml.hh>


namespace moo
{
    class function
    {
    public:
        std::string              function_type;
        std::string              function_name;
        std::vector<std::string> argument_type;
        std::vector<std::string> argument_name;

    public:
        static void append_list( pugi::xml_node list );
        static void append_node( pugi::xml_node node );

        static void remove_list( pugi::xml_node list );
        static void remove_node( pugi::xml_node node );


        static std::string   declare( );
        static std::string    points( );
        static std::string    stores( );
        static std::string implement( );

    private:
        static void make_nice_type( std::string & value );
        static void make_nice_name( std::string & value );

        void make_nice( );
    };


    extern std::map<std::string, function> functions;
}


#endif