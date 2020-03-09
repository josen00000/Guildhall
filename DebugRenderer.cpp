• Can render into camera
• Can render on screen

Static RenderContext* gDRContext = nullptr;
Static Camera* gDebugCamera = nullptr;

Class DebugRenderSystem{
    RenderContext* m_context;
    Camera* m_camera;
}

class DebugRenderObject{
    Timer m_timer;
    bool IsReadyToBeCalled() const;

}

DebugRenderScreenTo( Texture* tex ){
    TextureView* rtv = tex->GetRenderTargetView();
    RenderContext* ctx = tex->GetRenderContext();
    Camera camera;
    Camera->setColorTarget( tex );
    Vec2 min = Vec2::zero;
    Vec2 max = tex->GetDimensions();
    Camera->setprojectionOrthographic( min, max, -1.f, 1.f );

    Camera->SetClearMode( CLEAR_NONE );
    Std::vector<Vert>;
    gDRContext->BeginCamera( camera );


    foreach( object ){
        appendVertices();

    }
    gDRContext->DrawVertexArray( vertices );
    gDRContext->EndCamera();
}


void DebugAddScreenPoint( Vec2 pos, float size, rgba start_color, rgba endColor, float duration ) {
    // Make objects
    // save the information enable to clear and reappend the vertices every frame
    SomeObjectType* obj = createNewObjectFOrThisScreenPoint( pos, size, startColor, endColor, duration );

    AppendVerticesIntoIndexedArray( obj );


}
Internal class / hidden class

void Foo( uint count, ...){
    va_list args;
    va_start( args, count );
    string s = stringv( format, args );
    int v = va_arg(args, int);
    float f = va_arg(args, float);
    va_end(args);

}
Foo( 3, asdfasdfadfadf); // input with infinite number of argument

void Stringv( clar const* format, va_liast args ){
    char buffer[1024];
    vsnprintf_s( buffer, 1024, format, args );
    return buffer;

}
stringf( char const* format, ...){
    va_list args;
    va_start(args, format);
    std::string s = Stringv( format, args );
    va_end(args);
    return s;

}
use stack. Every time 
