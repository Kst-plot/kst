<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" />

<xsl:template match="/doxygen/compounddef">

<!-- Find the name of the class -->
<xsl:variable name="clazz" select="compoundname" />

<!-- Find the constructors and methods -->
<xsl:variable name="consmeth" select="sectiondef/memberdef[@kind='function' and @prot='public']" />

<!-- Find the enums -->
<xsl:variable name="enums" select="sectiondef/memberdef[@kind='enum' and @prot='public']" />

#include &lt;qcstring.h>
#include &lt;qimage.h>
#include &lt;qpainter.h>
#include &lt;qpalette.h>
#include &lt;qpixmap.h>
#include &lt;qfont.h>

#include &lt;kjs/object.h>

#include &lt;kjsembed/global.h>
#include &lt;kjsembed/jsobjectproxy.h>
#include &lt;kjsembed/jsopaqueproxy.h>
#include &lt;kjsembed/jsbinding.h>

#include &lt;<xsl:value-of select="includes" />>
#include "<xsl:value-of select="translate($clazz,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')"/>_imp.h"

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

<xsl:value-of select="$clazz" />Imp::<xsl:value-of select="$clazz" />Imp( KJS::ExecState *exec, int mid, bool constructor )
   : JSProxyImp(exec), id(mid), cons(constructor)
{
}

<xsl:value-of select="$clazz" />Imp::~<xsl:value-of select="$clazz" />Imp()
{
}

/**
 * Adds bindings for static methods and enum constants to the specified Object.
 */
void <xsl:value-of select="$clazz" />Imp::addStaticBindings( KJS::ExecState *exec, KJS::Object &amp;object )
{
    JSProxy::MethodTable methods[] = {
<xsl:for-each select="consmeth">
   <xsl:variable name="method_name"><xsl:value-of select="name" />_<xsl:value-of select="position()" /></xsl:variable>
   <xsl:variable name="method_id">Method_<xsl:value-of select="$method_name" /></xsl:variable>
  <xsl:choose>
     <xsl:when test="@static = 'yes'">
        { <xsl:value-of select="$method_id" />, "<xsl:value-of select="name" />" },</xsl:when></xsl:choose>
</xsl:for-each>
	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].name ) {
        if ( lastName != methods[idx].name ) {
            <xsl:value-of select="$clazz" />Imp *meth = new <xsl:value-of select="$clazz" />Imp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    }

<xsl:if test="count($enums) != 0">
    //
    // Define the enum constants
    //
    struct EnumValue {
	const char *id;
	int val;
    };

    EnumValue enums[] = {
<xsl:for-each select="$enums">
        // enum <xsl:value-of select="./name" />
        <xsl:for-each select="./enumvalue">
        { "<xsl:value-of select="./name" />", <xsl:value-of select="$clazz" />::<xsl:value-of select="./name" /> },</xsl:for-each>
</xsl:for-each>
	{ 0, 0 }
    };

    int enumidx = 0;
    while( enums[enumidx].id ) {
        object.put( exec, enums[enumidx].id, KJS::Number(enums[enumidx].val), KJS::ReadOnly );
        ++enumidx;
    }
</xsl:if>
}

/**
 * Adds bindings for instance methods to the specified Object.
 */
void <xsl:value-of select="$clazz" />Imp::addBindings( KJS::ExecState *exec, KJS::Object &amp;object )
{
    JSProxy::MethodTable methods[] = {
<xsl:for-each select="$consmeth">
   <xsl:variable name="method_name"><xsl:value-of select="name" />_<xsl:value-of select="position()" /></xsl:variable>
   <xsl:variable name="method_id">Method_<xsl:value-of select="$method_name" /></xsl:variable>
  <xsl:choose>
     <xsl:when test="not( @static = 'yes'
                          or starts-with( name, 'operator' )
                          or starts-with( name, '~' )
                          or starts-with( name, $clazz ))">
        { <xsl:value-of select="$method_id" />, "<xsl:value-of select="name" />" },</xsl:when></xsl:choose>
</xsl:for-each>
	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].name ) {
        if ( lastName != methods[idx].name ) {
            <xsl:value-of select="$clazz" />Imp *meth = new <xsl:value-of select="$clazz" />Imp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    }
}

/**
 * Extract a <xsl:value-of select="$clazz" /> pointer from an Object.
 */
<xsl:value-of select="$clazz" /> *<xsl:value-of select="$clazz" />Imp::to<xsl:value-of select="$clazz" />( KJS::Object &amp;self )
{
    JSObjectProxy *ob = JSProxy::toObjectProxy( self.imp() );
    if ( ob ) {
        QObject *obj = ob->object();
	if ( obj )
           return dynamic_cast&lt;<xsl:value-of select="$clazz" /> *>( obj );
    }

    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op )
        return 0;

    if ( op->typeName() != "<xsl:value-of select="$clazz" />" )
        return 0;

    return (<xsl:value-of select="$clazz" /> *)( op->toVoidStar() );
}

/**
 * Select and invoke the correct constructor.
 */
KJS::Object <xsl:value-of select="$clazz" />Imp::construct( KJS::ExecState *exec, const KJS::List &amp;args )
{
   switch( id ) {
<xsl:for-each select="$consmeth">
  <xsl:choose>
    <xsl:when test="name = $clazz">
     <xsl:variable name="cons_name"><xsl:value-of select="name" />_<xsl:value-of select="position()" /></xsl:variable>
   <xsl:variable name="cons_id">Constructor_<xsl:value-of select="$cons_name" /></xsl:variable>
         case <xsl:value-of select="$cons_id" />:
             return <xsl:value-of select="$cons_name" />( exec, args );
             break;
    </xsl:when>
  </xsl:choose>
</xsl:for-each>
         default:
             break;
    }

    QString msg = i18n("<xsl:value-of select="$clazz" />Cons has no constructor with id '%1'").arg(id);
    KJS::Object err = KJS::Error::create( exec, KJS::ReferenceError, msg.utf8() );
    exec->setException( err );
    return err;
}

<!-- Implementations of the constructors. -->
<xsl:for-each select="$consmeth">
  <xsl:choose>
    <xsl:when test="(name = $clazz)">
     <xsl:variable name="cons_name"><xsl:value-of select="name" />_<xsl:value-of select="position()" /></xsl:variable>
   <xsl:variable name="cons_id">Constructor_<xsl:value-of select="$cons_name" /></xsl:variable>
KJS::Object <xsl:value-of select="$clazz" />Imp::<xsl:value-of select="$cons_name" />( KJS::ExecState *exec, const KJS::List &amp;args )
{
    <xsl:if test="($clazz = 'QCanvasItem') or ($clazz = 'QCanvasPolygonalItem')">
#if 0 // This constructor has been disabled by the XSL template
    </xsl:if>

    <xsl:for-each select="param/type">

      <xsl:variable name="idx" select="position()-1" />

      <xsl:choose>
        <xsl:when test=". = 'QString'">
    QString arg<xsl:value-of select="$idx" /> = extractQString(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QString &amp;'">
    QString arg<xsl:value-of select="$idx" /> = extractQString(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'QString &amp;'">
    QString arg<xsl:value-of select="$idx" /> = extractQString(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QCString &amp;'">
    const QCString arg<xsl:value-of select="$idx" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="$idx" />].toString(exec).ascii() : 0;
        </xsl:when>
        <xsl:when test=". = 'const char *'">
    const char *arg<xsl:value-of select="$idx" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="$idx" />].toString(exec).ascii() : 0;
        </xsl:when>
        <xsl:when test=". = 'int'">
    int arg<xsl:value-of select="$idx" /> = extractInt(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'uint'">
    uint arg<xsl:value-of select="$idx" /> = extractUInt(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'double'">
    double arg<xsl:value-of select="$idx" /> = extractDouble(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'bool'">
    bool arg<xsl:value-of select="$idx" /> = extractBool(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QFont &amp;'">
    QFont arg<xsl:value-of select="$idx" /> = extractQFont(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QPalette &amp;'">
    QPalette arg<xsl:value-of select="$idx" /> = extractQPalette(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QStringList &amp;'">
    QStringList arg<xsl:value-of select="$idx" /> = extractQStringList(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QStrList &amp;'">
    QStrList arg<xsl:value-of select="$idx" /> = extractQStrList(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QPoint &amp;'">
    QPoint arg<xsl:value-of select="$idx" /> = extractQPoint(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QRect &amp;'">
    QRect arg<xsl:value-of select="$idx" /> = extractQRect(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QSize &amp;'">
    QSize arg<xsl:value-of select="$idx" /> = extractQSize(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QPixmap &amp;'">
    QPixmap arg<xsl:value-of select="$idx" /> = extractQPixmap(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QImage &amp;'">
    QImage arg<xsl:value-of select="$idx" /> = extractQImage(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QColor &amp;'">
    QColor arg<xsl:value-of select="$idx" /> = extractQColor(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QDate &amp;'">
    QDate arg<xsl:value-of select="$idx" /> = extractQDate(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QTime &amp;'">
    QTime arg<xsl:value-of select="$idx" /> = extractQTime(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QDateTime &amp;'">
    QDateTime arg<xsl:value-of select="$idx" /> = extractQDateTime(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>

        <xsl:when test=". = 'WFlags'">
    Qt::WFlags arg<xsl:value-of select="$idx" />; // TODO (hack for QCanvasView)
        </xsl:when>

        <xsl:otherwise>
    // Unsupported parameter <xsl:value-of select="." />
    return KJS::Object();
    
    <xsl:value-of select="." /> arg<xsl:value-of select="$idx" />; // Dummy
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>

    // We should now create an instance of the <xsl:value-of select="$clazz" /> object

    <xsl:value-of select="$clazz" /> *ret = new <xsl:value-of select="$clazz" />(
       <xsl:for-each select="param">
          arg<xsl:value-of select="position()-1" /><xsl:if test="position() != count(../param)">,</xsl:if>
       </xsl:for-each> );

    <xsl:if test="($clazz = 'QCanvasItem') or ($clazz = 'QCanvasPolygonalItem')">
#endif // This constructor has been disabled by the XSL template
    </xsl:if>
}
    </xsl:when>
  </xsl:choose>
</xsl:for-each>

<!-- Select and invoke the correct method. -->
KJS::Value <xsl:value-of select="$clazz" />Imp::call( KJS::ExecState *exec, KJS::Object &amp;self, const KJS::List &amp;args )
{
    instance = <xsl:value-of select="$clazz" />Imp::to<xsl:value-of select="$clazz" />( self );

    switch( id ) {
<xsl:for-each select="$consmeth">
   <xsl:variable name="method_name"><xsl:value-of select="name" />_<xsl:value-of select="position()" /></xsl:variable>
   <xsl:variable name="method_id">Method_<xsl:value-of select="$method_name" /></xsl:variable>
  <xsl:choose>
     <xsl:when test="not(starts-with( name, 'operator' ) or starts-with( name, '~' ) or starts-with( name, $clazz ))">
    case <xsl:value-of select="$method_id" />:
        return <xsl:value-of select="$method_name" />( exec, self, args );
        break;
    </xsl:when>
  </xsl:choose>
</xsl:for-each>
    default:
        break;
    }

    QString msg = i18n( "<xsl:value-of select="$clazz" />Imp has no method with id '%1'" ).arg( id );
    KJS::Object err = KJS::Error::create( exec, KJS::ReferenceError, msg.utf8() );
    exec->setException( err );
    return err;
}

<!-- Create the implementation for each method. -->
<xsl:for-each select="$consmeth">
<xsl:variable name="method">
<xsl:value-of select="$clazz" />Imp::<xsl:value-of select="name" />_<xsl:value-of select="position()" />
</xsl:variable>

  <xsl:choose>
    <xsl:when test="(name != $clazz) and not(starts-with(name, 'operator') or starts-with(name, '~'))">
<!-- Binding defined for a specific method. -->
KJS::Value <xsl:value-of select="$method" />( KJS::ExecState *exec, KJS::Object &amp;obj, const KJS::List &amp;args )
{
    <xsl:for-each select="param/type">

      <xsl:variable name="idx" select="position()-1" />

      <xsl:choose>
        <xsl:when test=". = 'QString'">
    QString arg<xsl:value-of select="$idx" /> = extractQString(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QString &amp;'">
    QString arg<xsl:value-of select="$idx" /> = extractQString(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'QString &amp;'">
    QString arg<xsl:value-of select="$idx" /> = extractQString(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QCString &amp;'">
    const QCString arg<xsl:value-of select="$idx" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="$idx" />].toString(exec).ascii() : 0;
        </xsl:when>
        <xsl:when test=". = 'const char *'">
    const char *arg<xsl:value-of select="$idx" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="$idx" />].toString(exec).ascii() : 0;
        </xsl:when>
        <xsl:when test=". = 'int'">
    int arg<xsl:value-of select="$idx" /> = extractInt(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'uint'">
    uint arg<xsl:value-of select="$idx" /> = extractUInt(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'double'">
    double arg<xsl:value-of select="$idx" /> = extractDouble(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'bool'">
    bool arg<xsl:value-of select="$idx" /> = extractBool(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QFont &amp;'">
    QFont arg<xsl:value-of select="$idx" /> = extractQFont(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QPalette &amp;'">
    QPalette arg<xsl:value-of select="$idx" /> = extractQPalette(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QStringList &amp;'">
    QStringList arg<xsl:value-of select="$idx" /> = extractQStringList(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QStrList &amp;'">
    QStrList arg<xsl:value-of select="$idx" /> = extractQStrList(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QPoint &amp;'">
    QPoint arg<xsl:value-of select="$idx" /> = extractQPoint(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QRect &amp;'">
    QRect arg<xsl:value-of select="$idx" /> = extractQRect(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QSize &amp;'">
    QSize arg<xsl:value-of select="$idx" /> = extractQSize(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QPixmap &amp;'">
    QPixmap arg<xsl:value-of select="$idx" /> = extractQPixmap(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QImage &amp;'">
    QImage arg<xsl:value-of select="$idx" /> = extractQImage(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QColor &amp;'">
    QColor arg<xsl:value-of select="$idx" /> = extractQColor(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QDate &amp;'">
    QDate arg<xsl:value-of select="$idx" /> = extractQDate(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QTime &amp;'">
    QTime arg<xsl:value-of select="$idx" /> = extractQTime(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>
        <xsl:when test=". = 'const QDateTime &amp;'">
    QDateTime arg<xsl:value-of select="$idx" /> = extractQDateTime(exec, args, <xsl:value-of select="$idx" />);
        </xsl:when>

	<!-- Hacks for enum types, this should be handled in a much cleaner way. -->

        <xsl:when test=". = 'Policy'">
    QComboBox::Policy arg<xsl:value-of select="$idx" />; // TODO (hack for combo box)
        </xsl:when>
        <xsl:when test=". = 'Shape'">
    QFrame::Shape arg<xsl:value-of select="$idx" />; // TODO (hack for frame)
        </xsl:when>
        <xsl:when test=". = 'Shadow'">
    QFrame::Shadow arg<xsl:value-of select="$idx" />; // TODO (hack for frame)
        </xsl:when>
        <xsl:when test=". = 'FILE *'">
    FILE *arg<xsl:value-of select="$idx" />; // TODO (hack for qfile)
        </xsl:when>
        <xsl:when test=". = 'Offset'">
    QFile::Offset arg<xsl:value-of select="$idx" />; // TODO (hack for qfile)
        </xsl:when>
        <xsl:when test=". = 'EncoderFn'">
    QFile::EncoderFn arg<xsl:value-of select="$idx" />; // TODO (hack for qfile)
        </xsl:when>
        <xsl:when test=". = 'DecoderFn'">
    QFile::DecoderFn arg<xsl:value-of select="$idx" />; // TODO (hack for qfile)
        </xsl:when>
        <xsl:when test=". = 'FrameAnimationType'">
    QCanvasSprite::FrameAnimationType arg<xsl:value-of select="$idx" />; // TODO (hack for QCanvasSprite)
        </xsl:when>
        <xsl:when test=". = 'WFlags'">
    Qt::WFlags arg<xsl:value-of select="$idx" />; // TODO (hack for QCanvasView)
        </xsl:when>

	<!-- Unsupported for now -->

        <xsl:when test=". = 'const QByteArray &amp;'">
    QByteArray arg<xsl:value-of select="$idx" />; // TODO (hack for qfile)
        </xsl:when>
        <xsl:when test=". = 'const QPointArray &amp;'">
    QPointArray arg<xsl:value-of select="$idx" />; // TODO (hack for qcanvas)
        </xsl:when>
        <xsl:when test=". = 'QPainter &amp;'">
    QPainter arg<xsl:value-of select="$idx" />; // TODO (hack for qcanvas)
        </xsl:when>
        <xsl:when test=". = 'const QPainter &amp;'">
    QPainter arg<xsl:value-of select="$idx" />; // TODO (hack for qcanvas)
        </xsl:when>
        <xsl:when test=". = 'const QWMatrix &amp;'">
    QWMatrix arg<xsl:value-of select="$idx" />; // TODO (hack for qcanvasview)
        </xsl:when>

        <xsl:otherwise>
    // Unsupported parameter <xsl:value-of select="." />
    return KJS::Value();
    
    <xsl:value-of select="." /> arg<xsl:value-of select="$idx" />; // Dummy
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>

    <!-- Specifies the return type -->
    <xsl:variable name="rettype">
    <xsl:choose>
       <xsl:when test="starts-with(type,'virtual ')">
          <xsl:value-of select="substring-after(type,'virtual ')" />
       </xsl:when>
       <xsl:otherwise><xsl:value-of select="type" /></xsl:otherwise>
    </xsl:choose>
    </xsl:variable>

    <xsl:choose>

      <xsl:when test="$rettype = 'bool'">
      bool ret;
      ret = instance-><xsl:value-of select="name" />( <xsl:for-each select="param">
       arg<xsl:value-of select="position()-1" /><xsl:if test="position() != count(../param)">,</xsl:if></xsl:for-each> );
      return KJS::Boolean( ret );
      </xsl:when>

      <xsl:when test="$rettype = 'int'">
      int ret;
      ret = instance-><xsl:value-of select="name" />( <xsl:for-each select="param">
       arg<xsl:value-of select="position()-1" /><xsl:if test="position() != count(../param)">,</xsl:if></xsl:for-each> );
      return KJS::Number( ret );
      </xsl:when>

      <xsl:when test="$rettype = 'uint'">
      uint ret;
      ret = instance-><xsl:value-of select="name" />( <xsl:for-each select="param">
       arg<xsl:value-of select="position()-1" /><xsl:if test="position() != count(../param)">,</xsl:if></xsl:for-each> );
      return KJS::Number( ret );
      </xsl:when>

      <xsl:when test="$rettype = 'double'">
      double ret;
      ret = instance-><xsl:value-of select="name" />( <xsl:for-each select="param">
       arg<xsl:value-of select="position()-1" /><xsl:if test="position() != count(../param)">,</xsl:if></xsl:for-each> );
      return KJS::Number( ret );
      </xsl:when>

      <xsl:when test="$rettype = 'QString'">
      QString ret;
      ret = instance-><xsl:value-of select="name" />( <xsl:for-each select="param">
       arg<xsl:value-of select="position()-1" /><xsl:if test="position() != count(../param)">,</xsl:if></xsl:for-each> );
      return KJS::String( ret );
      </xsl:when>

      <xsl:when test="$rettype = 'const char *'">
      const char *ret;
      ret = instance-><xsl:value-of select="name" />( <xsl:for-each select="param">
       arg<xsl:value-of select="position()-1" /><xsl:if test="position() != count(../param)">,</xsl:if></xsl:for-each> );
      return KJS::String( ret );
      </xsl:when>

      <!-- Value returns -->

      <xsl:when test="($rettype = 'QRect') or ($rettype = 'QSize') or ($rettype = 'QPoint')
                      or ($rettype = 'QPixmap') or ($rettype = 'QImage') or ($rettype = 'QBrush')
                      or ($rettype = 'QPen') or ($rettype = 'QDate') or ($rettype = 'QDateTime')
                      or ($rettype = 'QTime')">
      <xsl:value-of select="$rettype"/> ret;
      ret = instance-><xsl:value-of select="name" />( <xsl:for-each select="param">
       arg<xsl:value-of select="position()-1" /><xsl:if test="position() != count(../param)">,</xsl:if></xsl:for-each> );

       return convertToValue( exec, ret );
      </xsl:when>
      <xsl:when test="$rettype = 'QStringList'">
      QStringList ret;
      ret = instance-><xsl:value-of select="name" />( <xsl:for-each select="param">
       arg<xsl:value-of select="position()-1" /><xsl:if test="position() != count(../param)">,</xsl:if></xsl:for-each> );

       return convertToValue( exec, ret );
      </xsl:when>

      <!-- Special returns -->

      <xsl:when test="$rettype = 'void'">
      instance-><xsl:value-of select="name" />( <xsl:for-each select="param">
       arg<xsl:value-of select="position()-1" /><xsl:if test="position() != count(../param)">,</xsl:if></xsl:for-each> );
      return KJS::Value(); // Returns void
      </xsl:when>

      <xsl:otherwise>
      instance-><xsl:value-of select="name" />( <xsl:for-each select="param">
       arg<xsl:value-of select="position()-1" /><xsl:if test="position() != count(../param)">,</xsl:if></xsl:for-each> );
      return KJS::Value(); // Returns '<xsl:value-of select="$rettype" />'
      </xsl:otherwise>
    </xsl:choose>
}
    </xsl:when>
  </xsl:choose>
</xsl:for-each>

} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:

</xsl:template>
</xsl:stylesheet>
