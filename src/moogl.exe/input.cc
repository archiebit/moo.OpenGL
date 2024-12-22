#include <moo/input.hh>
#include <pugi/xml.hh>


namespace moo
{
    std::string   input;
    std::string  output;
    std::string version;

    static pugi::xml_document file;
    static pugi::xml_node     root;


    void input_version( int argc, char * argv[] )
    {
        static_cast<void>( argc );
        static_cast<void>( argv );
    }

    void treat_input( )
    {

    }
}