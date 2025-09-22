

/*
void Debris::Update( float deltaSeconds )
{
	Entity::Update(deltaSeconds)

	if( m_ageSeconds >= m_lifeTimeSeconds)
	{
		Die();
	}
}

void Debris::Render const
{
	Vertex tempDebrisVerts[DEBRIS_NUM_VERTS]
	for( int = 0; i < DEBRIS_NUM_VERTS; i++ )
	{
		tempDebrisVerts[i] = m_localVerts[i];
		float alphaFloat = RangeMapClamped( m_ageSeconds, 0.f, m_lifespanSeconds, 127.f, 0.f );
		tempDebrisVerts[i].m_color.a = static_cast<unsigned_char>( alphaFloat );
	}

	TransformVertexArrayXY3D( DEBRIS_NUM_Verts, tempDebrisVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_render->DrawVertexArray( DEBRIS_NUM_VERTS, tempDebrisVerts ); 
}
*/