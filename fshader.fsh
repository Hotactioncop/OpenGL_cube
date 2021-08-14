uniform sampler2D qt_Texture0;
varying highp vec2 qt_TexCoord0;

void main(void)
{
    gl_FragColor = /*vec4(1.0,0.0,0.0,1.0);//*/texture2D(qt_Texture0, qt_TexCoord0.st);
}
