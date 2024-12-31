#include <moo/constant.hh>
#include <moo/input.hh>

#include <iostream>


namespace moo
{
    std::map<std::string, constant> constants;


    void constant::append_list( pugi::xml_node list )
    {
        std::string prefix = "//enums/enum[@name = \'";
        std::string suffix = "\'][1]";
        std::string search;


        for( auto const & item : list.children( "enum" ) )
        {
            if( item.attribute( "name" ).empty( ) )
            {
                throw "ERROR: Invalid XML format.\n";
            }


            search  = prefix;
            search += item.attribute( "name" ).as_string( );
            search += suffix;


            auto node = root.select_node( search.c_str( ) ).node( );

            if( node.empty( ) )
            {
                throw "ERROR: Invalid XML format.\n";
            }
            else append_node( node );
        }
    }

    void constant::append_node( pugi::xml_node node )
    {
        {
            auto titled = not node.attribute(  "name" ).empty( );
            auto valued = not node.attribute( "value" ).empty( );

            if( not titled or not valued )
            {
                throw "ERROR: Invalid XML format.\n";
            }
        }


        auto title = node.attribute(  "name" ).as_string( );
        auto value = std::strtoull( node.attribute( "value" ).as_string( ), nullptr, 0 );

        if( not constants.contains( title ) )
        {
            constants.emplace( title, constant( title, value ) );
        }
    }

    void constant::remove_list( pugi::xml_node list )
    {
        for( auto const & item : list.children( "enum" ) )
        {
            if( item.attribute( "name" ).empty( ) )
            {
                throw "ERROR: Invalid XML format.\n";
            }


            remove_node( item );
        }
    }

    void constant::remove_node( pugi::xml_node node )
    {
        constants.erase( node.attribute( "name" ).as_string( ) );
    }
}


namespace moo
{
    std::string constant::declare_32( )
    {
        std::size_t name = 0;
        std::string digs = "________";

        for( auto & [ label, value ] : constants )
        {
            if( value.value <= 0xFFFF'FFFF )
            {
                name = std::max( name, value.title.size( ) );
            }
        }

        if( name == 0 ) return { };


        std::string list;
        std::string item;

        list.append( "enum GLenum32 : unsigned int\n{\n" );

        for( auto & [ label, value ] : constants )
        {
            if( value.value > 0xFFFF'FFFF )
            {
                continue;
            }


            std::size_t step = name - value.title.size( );


            item.clear( );

            item.append( 4, ' ' );
            item.append( value.title );
            item.append( step, ' ' );
            item.append( " = 0x" );


            std::uintmax_t num = value.value;
            std::uintmax_t dig = 0;

            for( std::size_t i = 7; i < 8; --i )
            {
                dig = num bitand 0x0F; num >>= 4;


                if( dig < 10 ) digs[ i ] = '0' + dig;
                else           digs[ i ] = 'A' + dig - 10;
            }

            item.append( digs.c_str( ) + 0, 4 ).append( 1, '\'' );
            item.append( digs.c_str( ) + 4, 4 );

            item.append( ",\n" );
            list.append( item );
        }

        list.pop_back( );
        list.pop_back( );
        list.append( "\n};\n" );


        return list;
    };

    std::string constant::declare_64( )
    {
        std::size_t name = 0;
        std::string digs = "________________";

        for( auto & [ label, value ] : constants )
        {
            if( value.value > 0xFFFF'FFFF )
            {
                name = std::max( name, value.title.size( ) );
            }
        }

        if( name == 0 ) return { };


        std::string list;
        std::string item;

        list.append( "enum GLenum64 : unsigned long long int\n{\n" );

        for( auto & [ label, value ] : constants )
        {
            if( value.value <= 0xFFFF'FFFF )
            {
                continue;
            }


            std::size_t step = name - value.title.size( );


            item.clear( );

            item.append( 4, ' ' );
            item.append( value.title );
            item.append( step, ' ' );
            item.append( " = 0x" );


            std::uintmax_t num = value.value;
            std::uintmax_t dig = 0;

            for( std::size_t i = 15; i < 16; --i )
            {
                dig = num bitand 0x0F; num >>= 4;


                if( dig < 10 ) digs[ i ] = '0' + dig;
                else           digs[ i ] = 'A' + dig - 10;
            }

            item.append( digs.c_str( ) +  0, 4 ).append( 1, '\'' );
            item.append( digs.c_str( ) +  4, 4 ).append( 1, '\'' );
            item.append( digs.c_str( ) +  8, 4 ).append( 1, '\'' );
            item.append( digs.c_str( ) + 12, 4 );

            item.append( ",\n" );
            list.append( item );
        }

        list.pop_back( );
        list.pop_back( );
        list.append( "\n};\n" );


        return list;
    }
}