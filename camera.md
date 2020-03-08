vec3 Camera::ClientToWorld( vec2 client, float ndcZ = 0.f ){
    vec3 ndc = RangeMap( vec3(client, ndcZ), Vec3::zero, Vec3::one, Vec3(-1,-1,0), Vec3::One);
    // get this to ClientToWorld
    // views
    // projection
    Mat44 proj = GetProjectionMatrix();
    Mat44 worldToClip = proj;
    worldToClip.PushMatrix( GetViewMatrix() );

    Mat44 clipToWorld = Invert( worldToClip );
    Vec4 worldHomogeneous = clipToWorld.Transform( Vec4(ncd, 1 ));
    Vec3 worldPos = worldHomegenours.XYZ() / worldHomogeneous.w;
}

Vec2 Camera::GetOrthoMin(){
    return ClientToWorld( Vec2(0,0));
}

Vec2 Camera::GetOrthoMax(){
    return ClientToWorld( vec2(1,1) );

}
