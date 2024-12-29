#include <moo/input.hh>
#include <moo/context.hh>


int main( int argc, char * argv[] )
{
    using namespace moo;


    parse_input( argc, argv );
    treat_input( );

    context::init( );
    context::save( );

    return 0;
}