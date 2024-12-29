#include <moo/context.hh>
#include <moo/input.hh>
#include <moo/constant.hh>
#include <moo/function.hh>


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