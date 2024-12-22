#ifndef MOO_PROGRAM_INPUT_HH
#define MOO_PROGRAM_INPUT_HH

#include <string>


namespace moo
{
    extern std::string   input; // XML file path.
    extern std::string  output; // Output directory.
    extern std::string version; // Target version.


    void parse_input( int argc, char * argv[] );
    void treat_input( );
}


#endif