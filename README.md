<div>
  <div align="center">
    <h1>Moo - OpenGL as a C++ class</h1>
    <br/>
  </div>
  <div align="center">
    <a href="https://github.com/archiebit/moo-OpenGL">
      <img src=".github/logo.png" width="256" height="256"/>
    </a>
  </div>
</div>



## About
This program reads the OpenGL API registry from the XML file and
outputs some C++ source files.

This is a small project, for personal study.



## Program Flags and Platform Support
I support Windows for now. Maybe something will change.

Here are all the flags for the program:

`--xml=<PATH>` - Specifies an input XML file with OpenGL registry. Must be provided.<br/>
`--out=<PATH>` - Specifies an output folder. Default value is current directory.<br/>
`--ver='<MAJ>.<MIN> [{ core | compatible }]'` - Specifies API version. Profile must be provided, if the target version is 3.0 or higher. Version numbers are mandatory.<br/>

Example:
```cmd
moogl --xml=gl.xml --ver='3.3 core'
```


## How to Use?
Output files will contain these functions:
- `mooglFindCommand` is for internal usage. It creates function pointers, that depend on the current OpenGL context.<br/>
- `mooglMakeContext` is used to create context. The `data` parameter is platform dependant. On Windows, `data` is a window handle (HWND), that supports OpenGL.<br/>
- `mooglDropContext` is used to delete context.<br/>
- `mooglBindContext` binds a specified context and returns its interface.<br/>

Example:
```C++
void example( void * data )
{
  using namespace moo::GL;


  if( not mooglMakeContext( data, 3, 3, true, false ) )
  {
    throw "Something went wrong!";
  }


  // Cast to the correct context!
  auto gl = static_cast<context<3, 3, CORE> *>( mooglBindContext( data ) );

  gl->ClearColor( 1.0F, 1.0F, 1.0F, 1.0F );
  gl->Clear( gl->GL_COLOR_BUFFER_BIT );


  // Should be at the end of the program.
  mooglDropContext( data );
}
```



## Contacts
If you want message me something interesting or funny, do it via [Telegram](https://t.me/ArchieBIT).