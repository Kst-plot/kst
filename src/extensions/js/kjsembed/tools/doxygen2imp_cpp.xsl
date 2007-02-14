<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" />

<xsl:template match="/doxygen/compounddef">

<xsl:variable name="clazz" select="compoundname" />
<xsl:variable name="consmeth" select="sectiondef/memberdef[@kind='function' and @prot='public']" />
<xsl:variable name="enums" select="sectiondef/memberdef[@kind='enum' and @prot='public']" />

#include &lt;qpalette.h>
#include &lt;qpixmap.h>
#include &lt;qfont.h>

#include &lt;klocale.h>
#include &lt;kjs/object.h>
#include &lt;kdebug.h>

#include &lt;kjsembed/jsobjectproxy.h>
#include &lt;kjsembed/jsopaqueproxy.h>
#include &lt;kjsembed/jsbinding.h>

#include &lt;<xsl:value-of select="includes" />>
#include "<xsl:value-of select="substring-before(includes,'.h')"/>_imp.h"

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

<xsl:value-of select="$clazz" />Imp::<xsl:value-of select="$clazz" />Imp( KJS::ExecState *exec, int id )
   : JSProxyImp(exec), mid(id)
{
}

<xsl:value-of select="$clazz" />Imp::~<xsl:value-of select="$clazz" />Imp()
{
}

void <xsl:value-of select="$clazz" />Imp::addBindings( KJS::ExecState *exec, KJS::Object &amp;object )
{
    JSProxy::MethodTable methods[] = {
<xsl:for-each select="$consmeth">
   <xsl:variable name="method_name"><xsl:value-of select="name" />_<xsl:value-of select="position()" /></xsl:variable>
   <xsl:variable name="method_id">Method_<xsl:value-of select="$method_name" /></xsl:variable>
  <xsl:choose>
     <xsl:when test="not(starts-with( name, 'operator' ) or starts-with( name, '~' ) or starts-with( name, $clazz ))">
        { <xsl:value-of select="$method_id" />, "<xsl:value-of select="name" />" },</xsl:when></xsl:choose>
</xsl:for-each>
	{ 0, 0 }
    };

    int idx = 0;
    do {
        <xsl:value-of select="$clazz" />Imp *meth = new <xsl:value-of select="$clazz" />Imp( exec, methods[idx].id );
        object.put( exec , methods[idx].name, KJS::Object(meth) );
        ++idx;
    } while( methods[idx].id );

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
    do {
        object.put( exec, enums[enumidx].id, KJS::Number(enums[enumidx].val), KJS::ReadOnly );
        ++enumidx;
    } while( enums[enumidx].id );
</xsl:if>
}

<xsl:value-of select="$clazz" /> *<xsl:value-of select="$clazz" />Imp::to<xsl:value-of select="$clazz" />( KJS::Object &amp;self )
{
    JSObjectProxy *ob = JSProxy::toObjectProxy( self.imp() );
    if ( ob ) {
        QObject *obj = ob->object();
	if ( obj )
           return dynamic_cast&lt;QComboBox *>( obj );
    }

    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op )
        return 0;

    if ( op->typeName() != "<xsl:value-of select="$clazz" />" )
        return 0;

    return (<xsl:value-of select="$clazz" /> *)( op->toVoidStar() );
}

<!-- Select and invoke the correct method. -->
KJS::Value <xsl:value-of select="$clazz" />Imp::call( KJS::ExecState *exec, KJS::Object &amp;self, const KJS::List &amp;args )
{
    instance = <xsl:value-of select="$clazz" />Imp::to<xsl:value-of select="$clazz" />( self );

    switch( mid ) {
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

    QString msg = i18n( "<xsl:value-of select="$clazz" />Imp has no method with id '%1'" ).arg( mid );
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
      <xsl:choose>
        <xsl:when test=". = 'QString'">
    QString arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toString(exec).qstring() : QString::null;
        </xsl:when>
        <xsl:when test=". = 'const QString &amp;'">
    QString arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toString(exec).qstring() : QString::null;
        </xsl:when>
        <xsl:when test=". = 'const char *'">
    const char *arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toString(exec).ascii() : 0;
        </xsl:when>
        <xsl:when test=". = 'int'">
    int arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toInteger(exec) : -1;
        </xsl:when>
        <xsl:when test=". = 'uint'">
    uint arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toInteger(exec) : -1;
        </xsl:when>
        <xsl:when test=". = 'double'">
    double arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toInteger(exec) : -1;
        </xsl:when>
        <xsl:when test=". = 'bool'">
    bool arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toBoolean(exec) : false;
        </xsl:when>
        <xsl:when test=". = 'const QFont &amp;'">
    QFont arg<xsl:value-of select="position()-1" />; // TODO
        </xsl:when>
        <xsl:when test=". = 'const QPalette &amp;'">
    QPalette arg<xsl:value-of select="position()-1" />; // TODO
        </xsl:when>
        <xsl:when test=". = 'const QStringList &amp;'">
    QStringList arg<xsl:value-of select="position()-1" /> = extractQStringList(exec, args, <xsl:value-of select="position()-1" />);
        </xsl:when>
        <xsl:when test=". = 'const QStrList &amp;'">
    QStrList arg<xsl:value-of select="position()-1" />;
    if ( args.size() >= <xsl:value-of select="position()" /> ) {
      // TODO: populate the list
    }
        </xsl:when>
        <xsl:when test=". = 'const QRect &amp;'">
    QRect arg<xsl:value-of select="position()-1" /> = extractQRect(exec, args, <xsl:value-of select="position()-1" />);
        </xsl:when>
        <xsl:when test=". = 'const QSize &amp;'">
    QSize arg<xsl:value-of select="position()-1" /> = extractQSize(exec, args, <xsl:value-of select="position()-1" />);
        </xsl:when>
        <xsl:when test=". = 'const QPixmap &amp;'">
    QPixmap arg<xsl:value-of select="position()-1" /> = extractQPixmap(exec, args, <xsl:value-of select="position()-1" />);
        </xsl:when>
        <xsl:when test=". = 'const QColor &amp;'">
    QColor arg<xsl:value-of select="position()-1" /> = extractQColor(exec, args, <xsl:value-of select="position()-1" />);
        </xsl:when>
        <xsl:when test=". = 'const QDate &amp;'">
    QDate arg<xsl:value-of select="position()-1" /> = extractQDate(exec, args, <xsl:value-of select="position()-1" />);
        </xsl:when>
        <xsl:when test=". = 'const QTime &amp;'">
    QTime arg<xsl:value-of select="position()-1" /> = extractQTime(exec, args, <xsl:value-of select="position()-1" />);
        </xsl:when>
        <xsl:when test=". = 'const QDateTime &amp;'">
    QDateTime arg<xsl:value-of select="position()-1" /> = extractQDateTime(exec, args, <xsl:value-of select="position()-1" />);
        </xsl:when>
        <xsl:when test=". = 'Policy'">
    QComboBox::Policy arg<xsl:value-of select="position()-1" />; // TODO (hack for combo box)
        </xsl:when>
        <xsl:otherwise>
    // Unsupported parameter <xsl:value-of select="." />
    return KJS::Value();
    
    <xsl:value-of select="." />arg<xsl:value-of select="position()-1" />; // Dummy
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

      <xsl:when test="$rettype = 'QStringList'">
      QStringList ret;
      ret = instance-><xsl:value-of select="name" />( <xsl:for-each select="param">
       arg<xsl:value-of select="position()-1" /><xsl:if test="position() != count(../param)">,</xsl:if></xsl:for-each> );

       return convertToValue( exec, ret );
      </xsl:when>

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
