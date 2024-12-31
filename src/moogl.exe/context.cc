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
        save_loading_source( );
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
            "\n"
            "\n"
            "    extern \"C\" GLvoid *  GLAPI mooglFindCommand( const GLchar * name );\n"
            "    extern \"C\" GLboolean GLAPI mooglMakeContext( GLvoid * data, GLint major, GLint minor, GLboolean core, GLboolean debug );\n"
            "    extern \"C\" GLvoid    GLAPI mooglDropContext( GLvoid * data );\n"
            "    extern \"C\" GLvoid *  GLAPI mooglBindContext( GLvoid * data );\n"
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
            "#ifdef _WIN32\n"
            "#   include <windows.h>\n"
            "#   undef MemoryBarrier  // OpenGL has function with the same name.\n"
            "#endif\n"
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
            "\n"
            "        void init( )\n"
            "        {\n"
            "            /* STORE */"
            "        }\n"
            "    };\n"
            "}\n"
            "\n"
            "\n"
            "extern \"C\" __declspec( dllexport ) HANDLE WINAPI CreateCommand_/* MAJOR */_/* MINOR */_/* GROUP */( )\n"
            "{\n"
            "    using moo::GL::NONE;\n"
            "    using moo::GL::CORE;\n"
            "    using moo::GL::COMPATIBLE;\n"
            "\n"
            "    using moo::GL::imp::context;\n"
            "\n"
            "\n"
            "    LPVOID Command = HeapAlloc( GetProcessHeap( ), 0, sizeof( context</* MAJOR */, /* MINOR */, /* GROUP */> ) );\n"
            "\n"
            "    if( Command )\n"
            "    {\n"
            "        static_cast<context</* MAJOR */, /* MINOR */, /* GROUP */> *>( Command )->init( );\n"
            "    }\n"
            "\n"
            "    return Command;\n"
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

    void context::save_loading_source( )
    {
        std::fstream file;
        std::string  path = output + "/GL-load.cc";
        std::string  data
        {
            "#include \"GL.hh\"\n"
            "\n"
            "\n"
            "#ifdef _WIN32\n"
            "\n"
            "\n"
            "#include <windows.h>\n"
            "\n"
            "\n"
            "#define WGL_CONTEXT_MAJOR_VERSION             0x0000'2091\n"
            "#define WGL_CONTEXT_MINOR_VERSION             0x0000'2092\n"
            "#define WGL_CONTEXT_FLAGS                     0x0000'2094\n"
            "#define WGL_CONTEXT_DEBUG_BIT                 0x0000'0001\n"
            "#define WGL_CONTEXT_PROFILE_MASK              0x0000'9126\n"
            "#define WGL_CONTEXT_CORE_PROFILE_BIT          0x0000'0001\n"
            "#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x0000'0002\n"
            "\n"
            "\n"
            "extern \"C\"\n"
            "{\n"
            "    PIXELFORMATDESCRIPTOR CONST SimpleInfo\n"
            "    {\n"
            "        .nSize           = sizeof( PIXELFORMATDESCRIPTOR ),\n"
            "        .nVersion        = 1,\n"
            "        .dwFlags         = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,\n"
            "        .iPixelType      = PFD_TYPE_RGBA,\n"
            "        .cColorBits      = 32,\n"
            "        .cRedBits        = 0,\n"
            "        .cRedShift       = 0,\n"
            "        .cGreenBits      = 0,\n"
            "        .cGreenShift     = 0,\n"
            "        .cBlueBits       = 0,\n"
            "        .cBlueShift      = 0,\n"
            "        .cAlphaBits      = 0,\n"
            "        .cAlphaShift     = 0,\n"
            "        .cAccumBits      = 0,\n"
            "        .cAccumRedBits   = 0,\n"
            "        .cAccumGreenBits = 0,\n"
            "        .cAccumBlueBits  = 0,\n"
            "        .cAccumAlphaBits = 0,\n"
            "        .cDepthBits      = 24,\n"
            "        .cStencilBits    = 8,\n"
            "        .cAuxBuffers     = 0,\n"
            "        .iLayerType      = PFD_MAIN_PLANE,\n"
            "        .bReserved       = 0,\n"
            "        .dwLayerMask     = 0,\n"
            "        .dwVisibleMask   = 0,\n"
            "        .dwDamageMask    = 0\n"
            "    };\n"
            "\n"
            "\n"
            "    HGLRC  WINAPI CreateContextDummyGL( HWND Window )\n"
            "    {\n"
            "        HDC    Device = GetDC( Window );\n"
            "        INT    Format = ChoosePixelFormat( Device, & SimpleInfo );\n"
            "        HGLRC Context = NULL;\n"
            "\n"
            "        if( Format == 0 )\n"
            "        {\n"
            "            return NULL;\n"
            "        }\n"
            "\n"
            "        if( SetPixelFormat( Device, Format, & SimpleInfo ) == FALSE )\n"
            "        {\n"
            "            return NULL;\n"
            "        }\n"
            "\n"
            "\n"
            "        if( ( Context = wglCreateContext( Device ) ) )\n"
            "        {\n"
            "            wglMakeCurrent( Device, Context );\n"
            "        }\n"
            "\n"
            "        return Context;\n"
            "    }\n"
            "\n"
            "\n"
            "    HGLRC  WINAPI CreateContextFinalGL( HWND Window, INT Major, INT Minor, BOOL Core, BOOL Debug )\n"
            "    {\n"
            "        using INIT = HGLRC( GLAPI * )( HDC, HGLRC, CONST INT * );\n"
            "\n"
            "\n"
            "        if( CreateContextDummyGL( Window ) == NULL )\n"
            "        {\n"
            "            return NULL;\n"
            "        }\n"
            "\n"
            "\n"
            "        HGLRC Final = NULL;\n"
            "        HGLRC Dummy = wglGetCurrentContext( );\n"
            "        HDC  Device = wglGetCurrentDC( );\n"
            "        INIT   Proc = ( INIT )wglGetProcAddress( \"wglCreateContextAttribsARB\" );\n"
            "\n"
            "        if( Proc == NULL )\n"
            "        {\n"
            "            wglMakeCurrent( NULL, NULL );\n"
            "            wglDeleteContext( Dummy );\n"
            "\n"
            "            return NULL;\n"
            "        }\n"
            "\n"
            "\n"
            "        INT Attributes[]\n"
            "        {\n"
            "            WGL_CONTEXT_MAJOR_VERSION, Major,\n"
            "            WGL_CONTEXT_MINOR_VERSION, Minor,\n"
            "            WGL_CONTEXT_PROFILE_MASK,  0,\n"
            "            WGL_CONTEXT_FLAGS,         0,\n"
            "            0\n"
            "        };\n"
            "\n"
            "        if( Core  ) Attributes[ 5 ] = WGL_CONTEXT_CORE_PROFILE_BIT;\n"
            "        else        Attributes[ 5 ] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT;\n"
            "\n"
            "        if( Debug ) Attributes[ 7 ] = WGL_CONTEXT_DEBUG_BIT;\n"
            "\n"
            "\n"
            "        Final = ( * Proc )( Device, NULL, Attributes );\n"
            "\n"
            "        if( Final == NULL )\n"
            "        {\n"
            "            wglMakeCurrent( NULL, NULL );\n"
            "            wglDeleteContext( Dummy );\n"
            "\n"
            "            return NULL;\n"
            "        }\n"
            "        else\n"
            "        {\n"
            "            wglMakeCurrent( Device, Final );\n"
            "            wglDeleteContext( Dummy );\n"
            "\n"
            "            return Final;\n"
            "        }\n"
            "    }\n"
            "\n"
            "\n"
            "    LPCSTR WINAPI DefineCommandLoading( INT Major, INT Minor, BOOL Core )\n"
            "    {\n"
            "        LPCSTR CONST Strings[ 4 ][ 7 ][ 2 ]\n"
            "        {\n"
            "            {\n"
            "                { \"CreateCommand_1_0_NONE\", \"CreateCommand_1_0_NONE\" },\n"
            "                { \"CreateCommand_1_1_NONE\", \"CreateCommand_1_1_NONE\" },\n"
            "                { \"CreateCommand_1_2_NONE\", \"CreateCommand_1_2_NONE\" },\n"
            "                { \"CreateCommand_1_3_NONE\", \"CreateCommand_1_3_NONE\" },\n"
            "                { \"CreateCommand_1_4_NONE\", \"CreateCommand_1_4_NONE\" },\n"
            "                { \"CreateCommand_1_5_NONE\", \"CreateCommand_1_5_NONE\" }\n"
            "            },\n"
            "            {\n"
            "                { \"CreateCommand_2_0_NONE\", \"CreateCommand_2_0_NONE\" },\n"
            "                { \"CreateCommand_2_1_NONE\", \"CreateCommand_2_1_NONE\" }\n"
            "            },\n"
            "            {\n"
            "                { \"CreateCommand_3_0_COMP\", \"CreateCommand_3_0_CORE\" },\n"
            "                { \"CreateCommand_3_1_COMP\", \"CreateCommand_3_1_CORE\" },\n"
            "                { \"CreateCommand_3_2_COMP\", \"CreateCommand_3_2_CORE\" },\n"
            "                { \"CreateCommand_3_3_COMP\", \"CreateCommand_3_3_CORE\" }\n"
            "            },\n"
            "            {\n"
            "                { \"CreateCommand_4_0_COMP\", \"CreateCommand_4_0_CORE\" },\n"
            "                { \"CreateCommand_4_1_COMP\", \"CreateCommand_4_1_CORE\" },\n"
            "                { \"CreateCommand_4_2_COMP\", \"CreateCommand_4_2_CORE\" },\n"
            "                { \"CreateCommand_4_3_COMP\", \"CreateCommand_4_3_CORE\" },\n"
            "                { \"CreateCommand_4_4_COMP\", \"CreateCommand_4_4_CORE\" },\n"
            "                { \"CreateCommand_4_5_COMP\", \"CreateCommand_4_5_CORE\" },\n"
            "                { \"CreateCommand_4_6_COMP\", \"CreateCommand_4_6_CORE\" }\n"
            "            }\n"
            "        };\n"
            "\n"
            "\n"
            "        if( Major <= 4 and Minor <= 6 )\n"
            "        {\n"
            "            return Strings[ Major - 1 ][ Minor ][ Core ];\n"
            "        }\n"
            "\n"
            "        return NULL;\n"
            "    }\n"
            "\n"
            "\n"
            "    HANDLE WINAPI CreateCommandFinalGL( INT Major, INT Minor, BOOL Core )\n"
            "    {\n"
            "        using MBI = MEMORY_BASIC_INFORMATION;\n"
            "        using FUN = HANDLE( WINAPI * )( );\n"
            "\n"
            "\n"
            "        HMODULE Module = NULL;\n"
            "        FUN       Proc = NULL;\n"
            "        LPCSTR    Name = DefineCommandLoading( Major, Minor, Core );\n"
            "\n"
            "        if( Name )\n"
            "        {\n"
            "            static INT Sample;\n"
            "            static MBI Memory;\n"
            "\n"
            "            if( VirtualQuery( & Sample, & Memory, sizeof( MBI ) ) )\n"
            "            {\n"
            "                Module = ( HMODULE )Memory.AllocationBase;\n"
            "            }\n"
            "\n"
            "            if( Module )\n"
            "            {\n"
            "                Proc = ( FUN )GetProcAddress( Module, Name );\n"
            "            }\n"
            "\n"
            "            if( Proc )\n"
            "            {\n"
            "                return ( * Proc )( );\n"
            "            }\n"
            "        }\n"
            "\n"
            "\n"
            "        return NULL;\n"
            "    }\n"
            "}\n"
            "\n"
            "\n"
            "namespace moo::GL\n"
            "{\n"
            "    extern \"C\" GLvoid *  GLAPI mooglFindCommand( const GLchar * name )\n"
            "    {\n"
            "        PROC Point = wglGetProcAddress( name );\n"
            "        PROC Cases[]\n"
            "        {\n"
            "            ( PROC )(  0 ),\n"
            "            ( PROC )(  1 ),\n"
            "            ( PROC )(  2 ),\n"
            "            ( PROC )(  3 ),\n"
            "            ( PROC )( -1 )\n"
            "        };\n"
            "\n"
            "\n"
            "        for( PROC Occasion : Cases ) if( Occasion == Point )\n"
            "        {\n"
            "            Point = GetProcAddress( GetModuleHandleW( L\"OPENGL32.DLL\" ), name );\n"
            "\n"
            "            break;\n"
            "        }\n"
            "\n"
            "        return reinterpret_cast<GLvoid *>( Point );\n"
            "    }\n"
            "\n"
            "\n"
            "    extern \"C\" GLboolean GLAPI mooglMakeContext( GLvoid * data, GLint major, GLint minor, GLboolean core, GLboolean debug )\n"
            "    {\n"
            "        if( not data ) return false;\n"
            "\n"
            "\n"
            "        HWND Window = static_cast<HWND>( data );\n"
            "\n"
            "        HGLRC  Context = CreateContextFinalGL( Window, major, minor, core, debug );\n"
            "        HANDLE Command = CreateCommandFinalGL( major, minor, core );\n"
            "\n"
            "        if( Context == NULL or Command == NULL )\n"
            "        {\n"
            "            wglMakeCurrent( NULL, NULL );\n"
            "\n"
            "            if( Context ) wglDeleteContext( Context );\n"
            "            if( Command ) HeapFree( GetProcessHeap( ), 0, Command );\n"
            "\n"
            "            return false;\n"
            "        }\n"
            "\n"
            "        SetPropW( Window, L\"moogl:context\", Context );\n"
            "        SetPropW( Window, L\"moogl:command\", Command );\n"
            "\n"
            "        return true;\n"
            "    }\n"
            "\n"
            "\n"
            "    extern \"C\" GLvoid    GLAPI mooglDropContext( GLvoid * data )\n"
            "    {\n"
            "        if( not data ) return;\n"
            "\n"
            "\n"
            "        HWND Window = static_cast<HWND>( data );\n"
            "\n"
            "\n"
            "        HGLRC  Context = ( HGLRC  )RemovePropW( Window, L\"moogl:context\" );\n"
            "        HANDLE Command = ( HANDLE )RemovePropW( Window, L\"moogl:command\" );\n"
            "\n"
            "        if( Context )\n"
            "        {\n"
            "            wglMakeCurrent( NULL, NULL );\n"
            "            wglDeleteContext( Context );\n"
            "        }\n"
            "\n"
            "        if( Command )\n"
            "        {\n"
            "            HeapFree( GetProcessHeap( ), 0, Command );\n"
            "        }\n"
            "    }\n"
            "\n"
            "\n"
            "    extern \"C\" GLvoid *  GLAPI mooglBindContext( GLvoid * data )\n"
            "    {\n"
            "        if( not data )\n"
            "        {\n"
            "            wglMakeCurrent( NULL, NULL );\n"
            "\n"
            "            return nullptr;\n"
            "        }\n"
            "\n"
            "\n"
            "        HWND    Window = static_cast<HWND>( data );\n"
            "        HGLRC  Context = ( HGLRC  )GetPropW( Window, L\"moogl:context\" );\n"
            "        HANDLE Command = ( HANDLE )GetPropW( Window, L\"moogl:command\" );\n"
            "\n"
            "        if( Context and Command )\n"
            "        {\n"
            "            wglMakeCurrent( GetDC( Window ), Context );\n"
            "\n"
            "            return Command;\n"
            "        }\n"
            "\n"
            "        return nullptr;\n"
            "    }\n"
            "}\n"
            "\n"
            "\n"
            "#else\n"
            "#   error Target OS is unsupported!\n"
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