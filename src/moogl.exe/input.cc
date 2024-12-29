#include <moo/input.hh>
#include <pugi/xml.hh>

#include <iostream>
#include <fstream>


namespace moo
{
    std::string  input;
    std::string output;
    std::string  major;
    std::string  minor;
    std::string  group;

    static pugi::xml_document file;
    pugi::xml_node            root;

    static std::string const usage
    {
        "This program produces C++ source files, that load OpenGL\n"
        "and encapsulate it in a class.\n"
        "\n"
        "moogl --ver=\'<VERSION>\' --xml=<PATH> [--out=<PATH>]\n"
        "\n"
        "USAGE:\n"
        "    --ver=\'<VERSION>\' : Specifies OpenGL API version.\n"
        "                          Major and minor numbers must be presented.\n"
        "                          Profile is not needed if the target version is below 3.0.\n"
        "                          Examples:\n"
        "                              \'3.3 compatible\'\n"
        "                              \'1.0\'\n"
        "                              \'4.6 core\'\n"
        "\n"
        "    --xml=<PATH>        : Specifies path to the source XML file.\n"
        "\n"
        "    --out=<PATH>        : Specifies output path. Default is the current path.\n"
        "\n"
        "EXAMPLE:\n"
        "    moogl --ver=\'3.3 core\' --xml=gl.xml\n"
    };


    void parse_input( int argc, char * argv[] )
    {
        bool versioned = false;
        bool   inputed = false;
        bool  outputed = false;

        try
        {
            for( int i = 1; i < argc; ++i )
            {
                if( std::strncmp( argv[ i ], "--xml=", 6 ) == 0 )
                {
                    if( inputed )
                    {
                        std::clog << "INFO:  New input file was ignored.\n";

                        continue;
                    }


                    input = argv[ i ] + 6;


                    inputed = true; continue;
                }


                if( std::strncmp( argv[ i ], "--out=", 6 ) == 0 )
                {
                    if( outputed )
                    {
                        std::clog << "INFO:  New output folder was ignored.\n";

                        continue;
                    }


                    output = argv[ i ] + 6;


                    outputed = true; continue;
                }


                if( std::strncmp( argv[ i ], "--ver=", 6 ) == 0 )
                {
                    if( versioned )
                    {
                        std::clog << "INFO:  New API version was ignored.\n";

                        continue;
                    }

                    major = "1";
                    minor = "0";
                    group = "NONE";

                    char space[ 11 ];
                    int    maj;
                    int    min;
                    int   size;
                    int  count = std::sscanf( argv[ i ], "--ver=\'%i.%i%n", & maj, & min, & size );


                    if( count != 2 )
                    {
                        throw "ERROR: Invalid version format.\n";
                    }

                    major = std::to_string( maj );
                    minor = std::to_string( min );


                    count = std::sscanf( argv[ i ] + size + 1, "%10s\'", space );

                    if( maj >= 3 )
                    {
                        if( count == 0 )
                        {
                            throw "ERROR: Invalid version format.\n";
                        }

                        if( std::strncmp( space, "core",        4 ) == 0 )
                        {
                            group = "CORE";
                        }
                        if( std::strncmp( space, "compatible", 10 ) == 0 )
                        {
                            group = "COMPATIBLE";
                        }

                        if( group == "NONE" )
                        {
                            throw "ERROR: Profile must be presented.\n";
                        }
                    }


                    versioned = true; continue;
                }
            }


            if( not versioned )
            {
                throw "ERROR: Version was not provided.\n";
            }

            if( not inputed )
            {
                throw "ERROR: Input XML file was not provided.\n";
            }

            if( not outputed )
            {
                output = ".";
            }
        }
        catch( char const * message )
        {
            std::cerr << message << '\n';
            std::cerr << usage;

            std::exit( 1 );
        }
    }

    void treat_input( )
    {
        std::ifstream stream;
        std::string     data;

        try
        {
            stream.exceptions( ~std::ifstream::goodbit );
            stream.open( input, std::ifstream::in );

            stream.seekg( 0, std::ifstream::end ); data.append( stream.tellg( ), ' ' );
            stream.seekg( 0, std::ifstream::beg );

            stream.read( data.data( ), data.size( ) );
            stream.close( );

            file.load_string( data.data( ) );

            root = file.child( "registry" );

            if( root.empty( ) )
            {
                std::cerr << "ERROR: Invalid XML format.\n\n";
                std::cerr << usage;
                std::exit( 1 );
            }
        }
        catch( std::exception const & )
        {
            std::cerr << "ERROR: An error occured while opening a file.\n";
            std::exit( 1 );
        }
    }
}