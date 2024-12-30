#include <moo/context.hh>
#include <moo/input.hh>
#include <moo/constant.hh>
#include <moo/function.hh>

#include <iostream>
#include <fstream>


namespace moo
{
    void context::init( )
    {
        std::string search = "//feature[@api=\'gl\'][@number=\'<MAJ>.<MIN>\'][1]";
        std::string source;

        int major = std::strtol( moo::major.c_str( ), nullptr, 10 ), maj = 1;
        int minor = std::strtol( moo::minor.c_str( ), nullptr, 10 ), min = 0;


        while( maj <= major and min <= minor )
        {
            source = search;

            source.replace( source.find( "<MAJ>" ), 5, std::to_string( maj ) );
            source.replace( source.find( "<MIN>" ), 5, std::to_string( min ) );


            auto feature = root.select_node( source.c_str( ) ).node( );


            for( auto const & list : feature.children( ) )
            {
                bool append = std::strcmp( list.name( ), "require" ) == 0;
                bool remove = std::strcmp( list.name( ), "remove"  ) == 0;


                if( append and resolve_list( list ) )
                {
                    constant::append_list( list );
                    function::append_list( list );
                }

                if( remove and resolve_list( list ) )
                {
                    constant::remove_list( list );
                    function::remove_list( list );
                }
            }


            if( feature.empty( ) )
            {
                maj += 1;
                min  = 0;
            }
            else
            {
                min += 1;
            }            
        }
    }

    bool context::resolve_list( pugi::xml_node list )
    {
        if( list.attribute( "profile" ).empty( ) )
        {
            return true;
        }


        char const * source = list.attribute( "profile" ).as_string( );
        char const * target = nullptr;

        // Profile presented.
        if( group == "COMPATIBLE" ) target = "compatibility";
        if( group == "CORE"       ) target = "core";


        return std::strcmp( source, target ) == 0;
    }
}


namespace moo
{
    void context::save( )
    {
        save_general_header( );
        save_special_header( );
        save_special_source( );
    }


    void context::save_general_header( )
    {
        std::fstream file;
        std::string  path = output + "/GL.hh";
        std::string  data
        {
            "#ifndef MOO_OPENGL_HH\n"
            "#define MOO_OPENGL_HH\n"
            "\n"
            "\n"
            "#ifdef _WIN32\n"
            "#   define GLAPI __stdcall\n"
            "#else\n"
            "#   define GLAPI\n"
            "#endif\n"
            "\n"
            "\n"
            "namespace moo::GL\n"
            "{\n"
            "#ifdef _WIN32\n"
            "    // Numerical type definitions.\n"
            "    using GLbyte   = signed char;\n"
            "    using GLshort  = signed short int;\n"
            "    using GLint    = signed int;\n"
            "    using GLint64  = signed long long int;\n"
            "\n"
            "    using GLubyte  = unsigned char;\n"
            "    using GLushort = unsigned short int;\n"
            "    using GLuint   = unsigned int;\n"
            "    using GLuint64 = unsigned long long int;\n"
            "\n"
            "    using GLfloat  = float;\n"
            "    using GLdouble = double;\n"
            "\n"
            "\n"
            "    // Utility type definitions.\n"
            "    using GLboolean  = bool;\n"
            "    using GLchar     = char;\n"
            "    using GLvoid     = void;\n"
            "    using GLenum     = unsigned int;\n"
            "    using GLbitfield = unsigned int;\n"
            "    using GLclampf   = float;\n"
            "    using GLclampd   = double;\n"
            "\n"
            "    using GLsizei    = unsigned int;\n"
            "    using GLintptr   = signed long long int;\n"
            "    using GLsizeiptr = unsigned long long int;\n"
            "    using GLcontext  = void *;\n"
            "\n"
            "    enum class GLsync  : unsigned long long int { };\n"
            "    enum class GLfixed : signed int             { };\n"
            "    enum class GLhalf  : signed short int       { };\n"
            "\n"
            "    using GLDEBUGPROC = GLvoid( GLAPI * )( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const GLvoid * userParam );\n"
            "#else\n"
            "#   error Target OS is unsupported!\n"
            "#endif\n"
            "\n"
            "\n"
            "    enum profile\n"
            "    {\n"
            "        NONE       = 0,\n"
            "        CORE       = 1,\n"
            "        COMPATIBLE = 2\n"
            "    };\n"
            "\n"
            "\n"
            "    template <int major, int minor, profile group>\n"
            "    class context;\n"
            "}\n"
            "\n"
            "\n"
            "#endif"
        };


        try
        {
            file.exceptions( ~std::ios::goodbit );
            file.open( path, std::ios::out | std::ios::trunc );

            file.write( data.data( ), data.size( ) );
            file.close( );
        }
        catch( std::exception & )
        {
            std::cerr << "ERROR: Can\'t write to the output file.\n";
            std::exit( 1 );
        }
    }

    void context::save_special_header( )
    {
        std::fstream file;
        std::string  path = output + "/GL-defined.hh";
        std::string  data
        {
            "#ifndef MOO_OPENGL_/* MAJOR */_/* MINOR */_/* GROUP */_HH\n"
            "#define MOO_OPENGL_/* MAJOR */_/* MINOR */_/* GROUP */_HH\n"
            "\n"
            "#include \"GL.hh\"\n"
            "\n"
            "\n"
            "namespace moo::GL\n"
            "{\n"
            "    template <>\n"
            "    class context</* MAJOR */, /* MINOR */, /* GROUP */>\n"
            "    {\n"
            "    private:\n"
            "       ~context( ) = delete;\n"
            "        context( ) = delete;\n"
            "        context( context const &  ) = delete;\n"
            "        context( context const && ) = delete;\n"
            "        context( context &        ) = delete;\n"
            "        context( context &&       ) = delete;\n"
            "\n"
            "        context & operator=( context const &  ) = delete;\n"
            "        context & operator=( context const && ) = delete;\n"
            "        context & operator=( context &        ) = delete;\n"
            "        context & operator=( context &&       ) = delete;\n"
            "\n"
            "\n"
            "    public:\n"
            "        /* PROCS */"
            "    };\n"
            "}\n"
            "\n"
            "\n"
            "#endif"
        };

        define( data );


        try
        {
            file.exceptions( ~std::ios::goodbit );
            file.open( path, std::ios::out | std::ios::trunc );

            file.write( data.data( ), data.size( ) );
            file.close( );
        }
        catch( std::exception & )
        {
            std::cerr << "ERROR: Can\'t write to the output file.\n";
            std::exit( 1 );
        }
    }

    void context::save_special_source( )
    {
        std::fstream file;
        std::string  path = output + "/GL-defined.cc";
        std::string  data
        {
            "#include \"GL.hh\"\n"
            "#include \"GL-defined.hh\"\n"
            "\n"
            "\n"
            "namespace moo::GL::imp\n"
            "{\n"
            "    template <int major, int minor, profile group>\n"
            "    class context;\n"
            "\n"
            "\n"
            "    template <>\n"
            "    class context</* MAJOR */, /* MINOR */, /* GROUP */>\n"
            "    {\n"
            "    public:\n"
            "        /* POINT */"
            "    };\n"
            "\n"
            "\n"
            "    static void context_load( context</* MAJOR */, /* MINOR */, /* GROUP */> * store, void *( * load )( char const * ) )\n"
            "    {\n"
            "        /* STORE */"
            "    }\n"
            "}\n"
            "\n"
            "\n"
            "namespace moo::GL\n"
            "{\n"
            "    /* IMPLS */"
            "}"
        };

        define( data );


        try
        {
            file.exceptions( ~std::ios::goodbit );
            file.open( path, std::ios::out | std::ios::trunc );

            file.write( data.data( ), data.size( ) );
            file.close( );
        }
        catch( std::exception & )
        {
            std::cerr << "ERROR: Can\'t write to the output file.\n";
            std::exit( 1 );
        }
    }
}


namespace moo
{
    void context::define( std::string & target )
    {
        for( ; ; )
        {
            if( std::size_t spot = target.find( "/* MAJOR */" ); spot != std::string::npos )
            {
                change( target, "/* MAJOR */", major );

                continue;
            }


            if( std::size_t spot = target.find( "/* MINOR */" ); spot != std::string::npos )
            {
                change( target, "/* MINOR */", minor );

                continue;
            }


            if( std::size_t spot = target.find( "/* GROUP */" ); spot != std::string::npos )
            {
                change( target, "/* GROUP */", group );

                continue;
            }


            if( std::size_t spot = target.find( "/* PROCS */" ); spot != std::string::npos )
            {
                change( target, "/* PROCS */", function::declare( ) );

                continue;
            }


            if( std::size_t spot = target.find( "/* POINT */" ); spot != std::string::npos )
            {
                change( target, "/* POINT */", function::points( ) );

                continue;
            }


            if( std::size_t spot = target.find( "/* STORE */" ); spot != std::string::npos )
            {
                change( target, "/* STORE */", function::stores( ) );

                continue;
            }


            if( std::size_t spot = target.find( "/* IMPLS */" ); spot != std::string::npos )
            {
                change( target, "/* IMPLS */", function::implement( ) );

                continue;
            }


            break;
        }
    }

    void context::change( std::string & target, std::string const & sample, std::string const & source )
    {
        if( source.empty( ) )
        {
            return;
        }


        std::size_t offset = target.find( sample );
        std::size_t starts;
        std::size_t ending;
        std::size_t spaces;

        if( offset != std::string::npos )
        {
            starts = 0;
            ending = source.find( '\n' );
            spaces = offset - target.rfind( '\n', offset ) - 1;
        }
        else return;

        target.erase( offset, sample.size( ) );


        while( ending != std::string::npos )
        {
            if( starts != 0 )
            {
                target.insert( offset, spaces, ' ' ), offset += spaces;
            }

            target.insert( offset, source, starts, ending - starts + 1 );

            offset = offset + ending - starts + 1;
            starts = ending + 1;
            ending = source.find( '\n', starts );
        }

        target.insert( offset, source, starts );
    }
}