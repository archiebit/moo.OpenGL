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