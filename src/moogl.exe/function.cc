#include <moo/function.hh>
#include <moo/input.hh>

#include <iostream>


namespace moo
{
    std::map<std::string, function> functions;


    void function::append_list( pugi::xml_node list )
    {
        std::string prefix = "//commands/command/proto/name[. = \'";
        std::string suffix = "\'][1]";
        std::string search;


        for( auto const & item : list.children( "command" ) )
        {
            if( item.attribute( "name" ).empty( ) )
            {
                throw "ERROR: Invalid XML format.\n";
            }


            search  = prefix;
            search += item.attribute( "name" ).as_string( );
            search += suffix;


            auto node = root.select_node( search.c_str( ) ).node( ).parent( ).parent( );

            if( node.empty( ) )
            {
                throw "ERROR: Invalid XML format.\n";
            }
            else append_node( node );
        }
    }

    void function::append_node( pugi::xml_node node )
    {
        {
            bool named = node.child( "proto" ).child( "name" ).child_value( );
            bool typed = node.child( "proto" ).child_value( );

            if( not named or not typed )
            {
                throw "ERROR: Invalid XML format.\n";
            }
        }


        function item;


        // Function name and return type.
        {
            std::string type;
            std::string name;

            for( auto const & part : node.child( "proto" ).children( ) )
            {
                auto is_text = part.type( ) == pugi::node_pcdata;
                auto is_type = std::strcmp( part.name( ), "ptype" ) == 0;
                auto is_name = std::strcmp( part.name( ), "name"  ) == 0;

                if( is_text )
                {
                    type += part.value( );
                }
                else if( is_type )
                {
                    type += part.child_value( );
                }
                else if( is_name )
                {
                    if( not name.empty( ) )
                    {
                        throw "ERROR: Invalid XML format.\n";
                    }

                    name += part.child_value( );

                    break;
                }
            }


            if( type.empty( ) or name.empty( ) )
            {
                throw "ERROR: Invalid XML format.\n";
            }

            item.function_type = std::move( type );
            item.function_name = std::move( name );
        }


        // Function's argument types and names.
        {
            for( auto const & args : node.children( "param" ) )
            {
                std::string type;
                std::string name;

                for( auto const & part : args.children( ) )
                {
                    auto is_text = part.type( ) == pugi::node_pcdata;
                    auto is_type = std::strcmp( part.name( ), "ptype" ) == 0;
                    auto is_name = std::strcmp( part.name( ), "name"  ) == 0;

                    if( is_text )
                    {
                        type += part.value( );
                    }
                    else if( is_type )
                    {
                        type += part.child_value( );
                    }
                    else if( is_name )
                    {
                        if( not name.empty( ) )
                        {
                            throw "ERROR: Invalid XML format.\n";
                        }

                        name += part.child_value( );

                        break;
                    }
                }


                if( type.empty( ) or name.empty( ) )
                {
                    throw "ERROR: Invalid XML format.\n";
                }

                item.argument_type.push_back( std::move( type ) );
                item.argument_name.push_back( std::move( name ) );
            }
        }


        item.make_nice( );

        functions.emplace( item.function_name, item );
    }

    void function::remove_list( pugi::xml_node list )
    {
        for( auto const & item : list.children( "command" ) )
        {
            if( item.attribute( "name" ).empty( ) )
            {
                throw "ERROR: Invalid XML format.\n";
            }


            remove_node( item );
        }
    }

    void function::remove_node( pugi::xml_node node )
    {
        std::string name = node.attribute( "name" ).as_string( );

        make_nice_name( name );

        functions.erase( name );
    }


    void function::make_nice_type( std::string & value )
    {
        std::size_t npos = std::string::npos;
        std::size_t    i = 0;

        // Splits "**" to "* *".
        for( i = value.find( "*"    ); i != npos; i = value.find( "*", i    ) )
        {
            char prev = ( i == 0                 ) ? '\x00' : value[ i - 1 ];
            char next = ( i == value.size( ) - 1 ) ? '\x00' : value[ i + 1 ];

            std::string_view cases = " *";


            if( prev and cases.find( prev ) == npos )
            {
                value.insert( std::next( value.cbegin( ), i + 0 ), ' ' );

                i += 2; continue;
            }

            if( next and cases.find( next ) == npos )
            {
                value.insert( std::next( value.cbegin( ), i + 1 ), ' ' );

                i += 2; continue;
            }

            break;
        }

        // Changes "void" to "GLvoid".
        for( i = value.find( "void" ); i != npos; i = value.find( "void", i ) )
        {
            value.insert( std::next( value.cbegin( ), i ), { 'G', 'L' } );

            i += 6;
        }


        while( value.size( ) and std::isspace( value.front( ) ) )
        {
            value.erase( value.cbegin( ) );
        }

        while( value.size( ) and std::isspace( value.back( )  ) )
        {
            value.pop_back( );
        }


        if( value.empty( ) )
        {
            throw "ERROR: Invalid XML format.\n";
        }
    }

    void function::make_nice_name( std::string & value )
    {
        while( value.size( ) and std::isspace( value.front( ) ) )
        {
            value.erase( value.cbegin( ) );
        }

        while( value.size( ) and std::isspace( value.back( )  ) )
        {
            value.pop_back( );
        }


        if( value.empty( ) )
        {
            throw "ERROR: Invalid XML format.\n";
        }
    }

    void function::make_nice( )
    {
        make_nice_type( function_type );
        make_nice_name( function_name );

        for( auto & item : argument_type )
        {
            make_nice_type( item );
        }

        for( auto & item : argument_name )
        {
            make_nice_name( item );
        }
    }
}


namespace moo
{
    std::string function::declare( )
    {
        std::size_t type = 0;
        std::size_t name = 0;

        for( auto & [ label, value ] : functions )
        {
            type = std::max( type, value.function_type.size( ) );
            name = std::max( name, value.function_name.size( ) );
        }


        std::string list;
        std::string item;

        for( auto & [ label, value ] : functions )
        {
            std::size_t type_step = type - value.function_type.size( );
            std::size_t name_step = name - value.function_name.size( );


            item.clear( );

            item.append( value.function_type.c_str( ) );        // Write return type.
            item.append( type_step, ' ' );                      // Aligning.
            item.append( 1, ' ' );                              // Space between return type and command name.
            item.append( name_step, ' ' );                      // Aligning.
            item.append( value.function_name.c_str( ) + 2 );    // Skip 'GL' prefix.

            item.append( "(" );

            for( std::size_t i = 0; i < value.argument_type.size( ); ++i )
            {
                item.append( 1, ' ' ).append( value.argument_type[ i ] );
                item.append( 1, ' ' ).append( value.argument_name[ i ] );

                if( i != value.argument_type.size( ) - 1 ) item.append( "," );
            }

            item.append( " );\n" );
            list.append( item );
        }

        return list;
    }

    std::string function::points( )
    {
        std::size_t type = 0;
        std::size_t name = 0;

        for( auto & [ label, value ] : functions )
        {
            type = std::max( type, value.function_type.size( ) );
            name = std::max( name, value.function_name.size( ) );
        }


        std::string list;
        std::string item;

        for( auto & [ label, value ] : functions )
        {
            std::size_t type_step = type - value.function_type.size( );
            std::size_t name_step = name - value.function_name.size( );


            item.clear( );

            item.append( value.function_type.c_str( ) );
            item.append( type_step, ' ' );
            item.append( 1, ' ' );
            item.append( "( GLAPI * " );
            item.append( name_step, ' ' );
            item.append( value.function_name.c_str( ) + 2 );
            item.append( " )" );

            item.append( "(" );

            for( std::size_t i = 0; i < value.argument_type.size( ); ++i )
            {
                item.append( 1, ' ' ).append( value.argument_type[ i ] );

                if( i != value.argument_type.size( ) - 1 ) item.append( "," );
            }

            item.append( " );\n" );
            list.append( item );
        }

        return list;
    }
}