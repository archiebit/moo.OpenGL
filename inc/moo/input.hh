#ifndef MOO_PROGRAM_INPUT_HH
#define MOO_PROGRAM_INPUT_HH

#include <string>

#include <pugi/xml.hh>


namespace moo
{
    extern std::string   input; // XML file path.
    extern std::string  output; // Output directory.
    extern std::string   major;
    extern std::string   minor;
    extern std::string   group;

    extern pugi::xml_node root;

    void parse_input( int argc, char * argv[] );
    void treat_input( );
}


#endif