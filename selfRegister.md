void app::startup{
    SubscribeEvent( "help", ShowHelp );
    DefineEvent( "help", ShowHelp, "Displays all available commands", EVENT_IS+VISIBLE_TO_DEVCONSOLE_BIT );

}

void DevConsole::RunCommand( char const* command ){
    if( EventSystemIsCommandDevConsoleVisible( command )){
        FireEvent( command );

    }

}

void ShowHelp(){
    foreach( defined event that is visible to dev console){
        Print event info;

    }

}

self regis
// 1 subscribe Event
void SubscribeEvent( name, callback );
void EventSystem::EventSystem(){
    SubscribeAllKnownEvents();
}

void EventSystem::SubscribeAllKnownEvnets(){
    for( event_registrar in registrar_list ){
        SubscribeEvent( event_registrar.name, event_registrar.callback );

    }
}


struct EventRegistrar{
    char const* name;
    callback callback;

    eventRegistrar( char const* name, callback cb )
    :name(name)
    ,callback(cb)
    {
    gRegistrarlist[gRegistrarCount] = this;
    ++gRegistrarCount;
    }
};

static void SomeMethod( NamedStrings& str );
static EventRegistrar gRegisterSomeMethod( "some_method", somemethod );
static void someMethod( NamedStrings& str ){
    // implementation

}

COMMAND( "some_method "){
// implementation

}

// do not use in HEADER
// Macro is defined in HEADER // might cause duplicate of command
// Command in body
#define COMMAND( name) \
    static void name##_impl( NameStrings& args ); \ // static void some_method_impl( NamedStrings& )
    static EventRegistrar name##_register( #name, name##_impl ); \ // static EventRegistrar some_method_register( "some_method", some_method_impl );
    static void name##_impl( NameStrings& args )                    // static void some_method_impl( Named)
// when do global constructors call?
// when program starts : before main
// after program is loaded into memory
//  - code memory
//  - data memory

// top of eventsystem.cpp
static uint constexpr MAX_REGISTERED_EVENTS( 128 )
static eventRegistar gRegistrarlist[MAX_REGISTERED_EVENTS]
static uint gRegistrarCount = 0;
static Foo* gSomePointer = nullptr; // this is the reason singleton patterns exist;
